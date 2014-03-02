/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*----------------------------------------------------------------------------
 |  Common Code for both ARM and MPC - this file is included inside MPC or   |
 |  ARM-NMF code separated by compilation flag.                              |
 -----------------------------------------------------------------------------*/


#ifdef __MPEG4ENC_ARM_NMF
	// Include self generated NMF header file for this component
	#include <mpeg4enc/arm_nmf/parser.nmf>
	#include "vfm_vec_mpeg4.idt"
	#include <common/inc/mpeg4enc_arm_mpc_common.h>
	#include <common/inc/mpeg4enc_arm_mpc.h> //common header file for both arm and mpc
	#include <parser.hpp>
#else
	// Include self generated NMF header file for this component
	#include <mpeg4enc/mpc/ddep.nmf>
	#include "fsm/component/include/Component.inl"
	#include "vfm_vec_mpeg4.idt"
	#include <mpeg4enc/common/inc/mpeg4enc_arm_mpc_common.h>
	#include <mpeg4enc/common/inc/mpeg4enc_arm_mpc.h> //common header file for both arm and mpc

	//>#define __DEBUG_TRACE_ENABLE
	//>#define __TRACE_CIRCULAR_DISABLE
	#include <ddep.h>
#endif

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#ifdef __MPEG4ENC_ARM_NMF
	#include "video_components_mpeg4enc_arm_nmf_parser_src_parserTraces.h"
#else
	#include "video_components_mpeg4enc_mpc_ddep_src_ddepTraces.h"
#endif
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef __MPEG4ENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		#define LOGS_ENABLED
	#endif
#else
	#define LOGS_ENABLED
#endif
#ifdef __MPEG4ENC_ARM_NMF
	#define BUFFERFLAG_EOS 			OMX_BUFFERFLAG_EOS
	#define BUFFERFLAG_ENDOFRAME 	OMX_BUFFERFLAG_ENDOFFRAME
	#define BUFFERFLAG_SYNCFRAME	OMX_BUFFERFLAG_SYNCFRAME
#endif

#ifndef __MPEG4ENC_ARM_NMF
// Global allocation for all trace ressources
//<>static TraceInfo_t EXTMEM traceInfo   	= {0,0,0};
//<>static TRACE_t     EXTMEM traceObject 	= {&traceInfo, 0};
static TRACE_t *   EXTMEM this ;//<>			= (TRACE_t *)&traceObject;
#endif

//> unsure of [
//> FIX ME!! Maybe required in arm-nmf too!
#ifndef __MPEG4ENC_ARM_NMF
static Buffer_t SHAREDMEM   fakeBufferDesc;
static Buffer_p     fakeBufIn = (Buffer_p)&fakeBufferDesc;

//> declared in common/inc
ts_ddep_bitstream_buf_link_and_header SHAREDMEM *ps_ddep_bitstream_buf_link_and_header;

typedef struct
{
	t_uint16 index_to_next_not_required_bitstream_buffer;
	t_uint16 current_bitstream_buffer_index;
	t_uint16 last_index_ps_ddep_bitstream_buf_link_and_header;
	t_ahb_address physical_ps_ddep_bitstream_buf_link_and_header;
	t_uint32 ps_ddep_bitstream_buf_link_and_header;
} t_ddep_bitstream_mgmt;

static t_ddep_bitstream_mgmt bsm;
#endif
//> ]

#define ConvertMult16(x) ((x%16)?(x/16 + 1)*16 : x )

void COMP(WriteBits)(t_sva_ec_writestream_header *pStream, t_uint32 data, t_uint32 nbBits)
{
	t_uint48 tmp;

	/*write in scratch buffer first*/
	if(nbBits!=32)
	{
		tmp=((data)&((1<<nbBits)-1));
	}//NOTE : can precompute mask table to accelerate
	else
	{
		tmp=data;
	}

	tmp=(tmp<<(48-pStream->nbBitsValid-nbBits));
	pStream->buffer+=tmp;
	pStream->nbBitsValid+=nbBits;
	pStream->totalBitsWritten+=nbBits;

	/*try to write data into destination buffer*/
	while(pStream->nbBitsValid>=16)
	{
		tmp=((pStream->buffer>>32)&0xffff);

		*pStream->currBuffer++= /*0xaabb*/SWAP16((t_uint16)tmp);
		pStream->buffer=(pStream->buffer<<16);
		pStream->nbBitsValid-=16;
	}
}

void COMP(flushWriteStream)(t_sva_ec_writestream_header *pStream)
{
	if(pStream->nbBitsValid!=0)
	{
		t_uint32 nbStuffBits=16-pStream->nbBitsValid;

		WriteBits(pStream,0,nbStuffBits);
		/*correct pStream->totalBitsWritten so it doesn't take into account padding bits*/
		pStream->totalBitsWritten-=nbStuffBits;
	}
}

t_uint32 COMP(SH_headerCreation)(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters,
						   t_sva_ec_save *ap_current_header, t_sva_ec_save *ap_previous_header,
						   t_sva_timestamp_value pts)
{
//>	t_short_value source_format;
	t_uint32 header_size;
	t_uint16 width;
	t_uint16 height;

	//>t_uint32 temp;
	//>t_uint24 SHAREDMEM* ptr_mpc;

	//>t_uint8 header_data[SVA_EC_MP4_SH_MAX_HEADER_SIZE];

	//>t_uint8 temporalReferenceMsb;
	//>t_uint8 temporalReferenceLsb;

	t_uint32 ptsDifference;

#ifdef __MPEG4ENC_ARM_NMF
	static t_sint32 last_ufep_occur = 0;
	t_sint32 passed_seconds = last_ufep_occur/pMecIn_parameters->framerate;
	//>t_uint32 UFEP_present = FALSE;
#endif

	//>t_uint32 incDifference;

	t_sva_ec_writestream_header writeStream;
	t_uint16 *addr_pointer;

	t_uint32 source_format;
	t_uint32 temp_code;
	t_uint32 bits_written;
	t_uint16 custom_size = 0;

	OstTraceFiltInst0(TRACE_API, "=> Mpeg4enc ddep:  SH_headerCreation()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SH_headerCreation() enter\n");
	#endif

	/* Header writing
	   -------------- */
	addr_pointer = (t_uint16 SHAREDMEM* )(param_desc_mpc_addr);
#if 0
	printf("Mpeg4enc ddep:  header mpc address\n");
	PRINT_VAR(addr_pointer);
#endif
	writeStream.buffer = 0;
	writeStream.currBuffer= addr_pointer;
	writeStream.endbuffer = addr_pointer+HEADER_BUFFER_SIZE;
	writeStream.nbBitsValid=0;
	writeStream.totalBitsWritten=0;

	width = pMecIn_parameters->window_width;
	height = pMecIn_parameters->window_height;

	if(pMecIn_parameters->picture_coding_type == 0)
		ap_current_header->pictureCodingType = 0/*I_FRAME*/;
	else if(pMecIn_parameters->picture_coding_type == 1)
		ap_current_header->pictureCodingType = 1/*P_FRAME*/;
	else
		ap_current_header->pictureCodingType = 1/*P_FRAME*/;

	if(frame_number==1)
	{
		ap_current_header->temporalSh.tr = 0;
		ap_current_header->temporalSh.cumulTimeSlot = 0;
		ap_current_header->temporalSh.slotDelay = 0;
		//< ap_current_header->pts = 0;
		ap_current_header->pts = pts;
		//>temporalReferenceMsb= 0x00;
		//>temporalReferenceLsb= 0x00;
		ptsDifference = 0;
	}
	else
	{
		/* Set by the application */
		ap_current_header->pts = pts;

		/*compute pts difference*/
		ptsDifference = ap_current_header->pts - ap_previous_header->pts;

		/*update tr. Use ptsDifference*/
		ap_current_header->temporalSh.tr = ( ((ptsDifference + ap_previous_header->temporalSh.cumulTimeSlot + MP4_SH_ROUND_VALUE) / MP4_SH_CLOCK_SLOT)
											 + ap_previous_header->temporalSh.tr )&0x000000ff;

		ap_current_header->temporalSh.cumulTimeSlot = (ptsDifference + ap_previous_header->temporalSh.cumulTimeSlot + MP4_SH_ROUND_VALUE) % MP4_SH_CLOCK_SLOT;
		ap_current_header->temporalSh.cumulTimeSlot -= MP4_SH_ROUND_VALUE;

		/*compute intermediate parameters*/
		//>temporalReferenceMsb=(t_uint8)((ap_current_header->temporalSh.tr&0xc0)>>6);
		//>temporalReferenceLsb=(t_uint8)((ap_current_header->temporalSh.tr&0x3f)<<2);
	}

	if((width==SVA_EC_MP4_SQCIF_WIDTH) && (height==SVA_EC_MP4_SQCIF_HEIGHT))
	{
		source_format=SVA_EC_MP4_SH_SOURCE_FORMAT_SQCIF;
	}
	else if((width==SVA_EC_MP4_QCIF_WIDTH) && (height==SVA_EC_MP4_QCIF_HEIGHT))
	{
		source_format=SVA_EC_MP4_SH_SOURCE_FORMAT_QCIF;
	}
	else if((width==SVA_EC_MP4_CIF_WIDTH) && (height==SVA_EC_MP4_CIF_HEIGHT))
	{
		source_format=SVA_EC_MP4_SH_SOURCE_FORMAT_CIF;
	}
	else if((width==SVA_EC_MP4_CIF4_WIDTH) && (height==SVA_EC_MP4_CIF4_HEIGHT))
	{
		source_format=SVA_EC_MP4_SH_SOURCE_FORMAT_CIF4;
		custom_size = 1;
	}
	else if((width==SVA_EC_MP4_CIF16_WIDTH) && (height==SVA_EC_MP4_CIF16_HEIGHT))
	{
		source_format=SVA_EC_MP4_SH_SOURCE_FORMAT_CIF16;
		custom_size = 1;
	}
	else if((width==SVA_EC_MP4_VGA_WIDTH) && (height==SVA_EC_MP4_VGA_HEIGHT))
	{
		source_format=0x06;//SVA_EC_MP4_SH_SOURCE_FORMAT_VGA; custom format changes
		custom_size = 1;
	}
	else if((width==SVA_EC_MP4_QVGA_WIDTH) && (height==SVA_EC_MP4_QVGA_HEIGHT))
	{
		source_format=0x06;//SVA_EC_MP4_SH_SOURCE_FORMAT_QVGA; custom format changes
		custom_size = 1;
	}
	else
	{
		return(0);	// this should be used as error code and checked and user notfied with an error event
					// currently, if this case arises, the SH stream will be generated with texture data only
					// as encoding proceeds normally as no error checking is done
					// but the stream will not have header and will be unplayable
	} // TBW

	PRINT_VAR(source_format);
	PRINT_VAR(ap_current_header->pictureCodingType);
	PRINT_VAR(writeStream.totalBitsWritten);

	OstTraceFiltInst1(TRACE_DEBUG, "source_format = %d", source_format);
	/*fill header*/

#if 0
	header_data[0]=0x00;
	header_data[1]=0x00;
	header_data[2]=(t_uint8)(0x80+temporalReferenceMsb);
	header_data[3]=(t_uint8)(0x02+temporalReferenceLsb);
#endif

	//Picture Start Code (PSC) - 22 bits
	WriteBits(&writeStream,0x00,EIGHT_BITS);
	WriteBits(&writeStream,0x00,EIGHT_BITS);
	WriteBits(&writeStream,0x02,TWO_BITS);
	WriteBits(&writeStream,0x00,FOUR_BITS);

	//Temporal Reference (TR) - 8 bits
	WriteBits(&writeStream,ap_current_header->temporalSh.tr,EIGHT_BITS);

	//PTYPE - Variable Length
	//
	WriteBits(&writeStream,0x02,TWO_BITS);
	//Split Screen Indicator + Document Camera Indicator + Full Picture Freeze Release
	WriteBits(&writeStream,0x00,THREE_BITS);

#ifdef __MPEG4ENC_ARM_NMF
	if((!pMecIn_parameters->h263_p3)&&(source_format!=0x06))
	{
#endif
		//Source Format - 3 bits
		WriteBits(&writeStream,source_format,THREE_BITS);
		OstTraceFiltInst1(TRACE_FLOW, "Writing pictureCodingType %d in SH header", ap_current_header->pictureCodingType);
		//Picture Coding Type
		WriteBits(&writeStream,ap_current_header->pictureCodingType,ONE_BIT);
		WriteBits(&writeStream,0x00,FOUR_BITS);

#if 0
	header_data[4]=(t_uint8)((source_format<<2)+(ap_current_header->pictureCodingType<<1));
	header_data[5]=0;

	//>ptr_mpc = (t_uint16 SHAREDMEM* )(param_desc_mpc_addr);
	ptr_mpc = (t_uint24 SHAREDMEM* )(param_desc_mpc_addr);

	temp = header_data[1] | header_data[0]<<8 ;
	temp = SWAP16(temp);
	*((t_uint16 SHAREDMEM*)ptr_mpc) = /*SVP_SHAREDMEM_TO16*/(temp);

	temp = header_data[3] | header_data[2]<<8;
	temp = SWAP16(temp);
	*((t_uint16 SHAREDMEM*)ptr_mpc+1) = /*SVP_SHAREDMEM_TO16*/(temp);

	temp = header_data[5] | header_data[4]<<8;
	temp = SWAP16(temp);
	*((t_uint16 SHAREDMEM*)ptr_mpc+2) = /*SVP_SHAREDMEM_TO16*/(temp);

	header_size = 43;
#endif

#ifdef __MPEG4ENC_ARM_NMF
	}
	else		//for if(!pMecIn_parameters->h263_p3)
	{
		//Source Format - 3 bits
		// Writing it as '111', indicating the presence of PLUSPTYPE
		WriteBits(&writeStream,SVA_EC_MP4_SH_SOURCE_FORMAT_EXTENDED_PTYPE,THREE_BITS);

		//H263 Profile 3 related header information will be written now

		//We just calculate whether (the last time UFEP was written as '001' was
		// five seconds or five pictures before) OR (this is an INTRA frame).
		// If it was, then we have to write it as '001' this time.
		// Otherwise, just increment the last count as 1 (remember, it is a static variable!)
		if((passed_seconds >= 5 && last_ufep_occur >= 5) || ap_current_header->pictureCodingType == 0) {
			last_ufep_occur = 0;
		}
		else {
			last_ufep_occur += 1;
		}

		PRINT_VAR(last_ufep_occur);

		if(last_ufep_occur == 0)
		{
			//This means, UFEP will be written as '001' here, and the optional part OPPTYPE will be added

			//UFEP - 3 bits
			// Writing it as '001', so the optional part OPPTYPE will be added
			WriteBits(&writeStream,0x01,THREE_BITS);

			//OPPTYPE - 18 bits
			//Now adding Optional Part of PLUSPTYPE

			//Source Format
			WriteBits(&writeStream,source_format,THREE_BITS);

			//Custom PCF, UMV mode (Annex D), SAC mode (Annex E) and Advanced Prediction Mode (Annex F)
			WriteBits(&writeStream,0x00,FOUR_BITS);

			// Advanced INTRA Coding, Deblocking Filter and Slice Structured mode
			// Optional Reference Picture Selection, Independent Segment Decoding, Alternative INTER VLC
			// Optional Modified Quantization mode
			temp_code = (t_uint8)((pMecIn_parameters->adv_intra_coding == TRUE ? 0x80:0)
							| (pMecIn_parameters->deblock_filter == TRUE ? 0x40:0)
							| (pMecIn_parameters->slice_mode >= 1 ? 0x20:0)
							| (pMecIn_parameters->mod_quant_mode == TRUE ? 0x02:0)
							| 0x01);

				PRINT_VAR(pMecIn_parameters->adv_intra_coding);
				PRINT_VAR(pMecIn_parameters->deblock_filter);
				PRINT_VAR(pMecIn_parameters->slice_mode);
				PRINT_VAR(pMecIn_parameters->mod_quant_mode);

			WriteBits(&writeStream,temp_code,EIGHT_BITS);

			//Reserved bits - 3, all zeros
			WriteBits(&writeStream,0x00,THREE_BITS);

		}
		else
		{
			//This means, UFEP will be written as '000' here, and no OPPTYPE will be added

			//UFEP - 3 bits
			// Writing it as '000', so no OPPTYPE will NOT be added
			WriteBits(&writeStream,0x00,THREE_BITS);
		}

		//MPPTYPE - 9 bits

		//Picture Type Code + RPR + RRU + RTYPE
#if 0
		temp_code = ((ap_current_header->pictureCodingType==0 ? 0:0x20) | (mp4_par->rtype << 2));
#else
		temp_code = ((ap_current_header->pictureCodingType==0 ? 0:0x20) | 0);
#endif
		WriteBits(&writeStream,temp_code,EIGHT_BITS);

		WriteBits(&writeStream,0x01,ONE_BIT);

		// CPM field (Continuous Presence Multipoint) (disabled in Profile 3)
		WriteBits(&writeStream,0x00,ONE_BIT);

		//!!!!!!!!
		//TODO: Have to add support for PAR, EPAR,
		if(last_ufep_occur == 0 && custom_size == 1)
		{
			//PAR Code
			WriteBits(&writeStream,0x01,FOUR_BITS);
			//PWI
			temp_code = (width>>2)-1;
			WriteBits(&writeStream,temp_code,NINE_BITS);

			WriteBits(&writeStream,0x01,ONE_BIT);

			//PHI
			temp_code = (height>>2);//-1; Custom picture format changes
			WriteBits(&writeStream,temp_code,NINE_BITS);
		}

		// Slice Structure Submodes (disabled in Profile 3) to be added if Slice Structure mode enabled and UFEP set to 1
		if(pMecIn_parameters->slice_mode >= 1 && last_ufep_occur == 0) {
			WriteBits(&writeStream,0x00,TWO_BITS);
		}
	}
#endif

	//>PRINT_VAR(writeStream.totalBitsWritten);
//>	ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size = SVP_SHAREDMEM_TO16(header_size); //size in bits!
	header_size = SVP_SHAREDMEM_TO16(writeStream.totalBitsWritten);
	ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size = header_size; //size in bits!

	bits_written = writeStream.totalBitsWritten;
	//>printf("Total bits Written: 0x%x\n", writeStream.totalBitsWritten);
	while(bits_written %16 != 0) {
			//>printf("Writing extra bits ");
			WriteBits(&writeStream,0x00,ONE_BIT);
			bits_written++;
	}
	//>printf("Written %d extra bits\n", bits_written - ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size);
	//> We write extra bits into the header to make the total number of bits a multiple of 16
	//>WriteBits(&writeStream,0x00,FOUR_BITS);
	//>WriteBits(&writeStream,0x00,ONE_BIT);


	OstTraceFiltInst0(TRACE_API, "<= Mpeg4enc ddep:  SH_headerCreation()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SH_headerCreation() exit\n");
	#endif


	return header_size;
}

t_uint32 COMP(SP_headerCreation)(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters,
						   t_sva_ec_save *ap_current_header, t_sva_ec_save *ap_previous_header,
						   t_sva_timestamp_value pts, t_uint32 buf_level)
{
	t_sva_ec_writestream_header writeStream;

	t_uint16 *addr_pointer;	//> changed from t_uint8* to t_uint16*
	//>t_uint32 nbBits=0;
	t_uint32 bitRate;
	t_uint32 vbvBufferSize;
	t_uint32 vbvOccupancy;
	//>t_uint32 swisBuffer;

	//> t_uint8 temporalReferenceMsb;
	//> t_uint8 temporalReferenceLsb;

	t_uint32 ptsDifference;
	t_uint32 incDifference;

	t_uint32 modulo_time_base = 0;

	OstTraceFiltInst0(TRACE_API, "=> Mpeg4enc ddep:  SP_headerCreation()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SP_headerCreation() enter\n");
	#endif


	//>swisBuffer = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->swis_buffer);
	bitRate = SVP_SHAREDMEM_FROM16(pMecIn_parameters->bit_rate);
	vbvBufferSize = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->vbv_buffer_size)/16384;	//> Dividing, because the value passed from proxy side was the actual value, and not in 16384 units
	vbvOccupancy = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->vbv_occupancy);

	OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of bitRate (0x%x) at line no %d \n",bitRate,__LINE__);
	PRINT_VAR(bitRate);

	//According to the MPEG4 standard, in the repeated VOS+VO+VOL headers, the first_half_vbv_occupancy
	// and latter_half_vbv_occupancy may be changed to specify
	// the VBV occupancy just before the removal of the first VOP following the repeated VOL Header.
	// This is followed by the reference encoder.
	vbvOccupancy = ((vbvOccupancy<<6)-buf_level)>>6;

	/* Header writing
	   -------------- */
	addr_pointer = (t_uint16 SHAREDMEM* )(param_desc_mpc_addr);
#if 0
	OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  header mpc address\n");
	printf("Mpeg4enc ddep:  header mpc address\n");
	PRINT_VAR(addr_pointer);
#endif
	writeStream.buffer = 0;
	writeStream.currBuffer= addr_pointer;
	writeStream.endbuffer = addr_pointer+HEADER_BUFFER_SIZE;
	writeStream.nbBitsValid=0;
	writeStream.totalBitsWritten=0;

	ap_current_header->pictureCodingType = pMecIn_parameters->picture_coding_type;

	// Fill header informations
	if(frame_number==1)
	{
		//< fixed to be able to accept non-zero initial timestamps also

		ap_current_header->temporalSp.remainForOffset=0;
		ap_current_header->temporalSp.moduloTimeBase=0;
		//< ap_current_header->temporalSp.vopTimeIncrement=0;
		ap_current_header->temporalSp.vopTimeIncrement= (pts*pMecIn_parameters->vop_time_increment_resolution)/90000;
		ap_current_header->temporalSp.vopTimeIncrementBitSize = 0;
		//< ap_current_header->pts = 0;
		ap_current_header->pts = pts;
		ap_previous_header = ap_current_header;
		ptsDifference = 0;
		while((1<<ap_current_header->temporalSp.vopTimeIncrementBitSize) < pMecIn_parameters->vop_time_increment_resolution)
		{
			ap_current_header->temporalSp.vopTimeIncrementBitSize++;
		}
		//> Added for cases where frame_rate = 1. otherwise, VOP_TIME_INCREMENT field in VOP header will not be written at all!
		if( ap_current_header->temporalSp.vopTimeIncrementBitSize == 0 )
		{
			ap_current_header->temporalSp.vopTimeIncrementBitSize = 1;
		}
		/* For the next header information of the first frame */

		while(ap_current_header->temporalSp.vopTimeIncrement >= pMecIn_parameters->vop_time_increment_resolution)
		{
			ap_current_header->temporalSp.vopTimeIncrement -= pMecIn_parameters->vop_time_increment_resolution;
			ap_current_header->temporalSp.moduloTimeBase++;
		}

	}
	else
	{
		/* Set by application */
		//< ap_current_header->pts = ((90000U*(frame_num-1))/pMecIn_parameters->vop_time_increment_resolution);
		ap_current_header->pts = pts;

		/*compute pts difference*/
		ptsDifference = ap_current_header->pts - ap_previous_header->pts;

		//< PRINT_VAR(ptsDifference);

		/*update temporal reference value. Use ptsDifference*/
		/*convert ptsDifference in 90Khz to incDifference in vopTimeIncrementResolutionHz value*/
		incDifference = (ptsDifference * pMecIn_parameters->vop_time_increment_resolution +
						 ap_previous_header->temporalSp.remainForOffset)/90000;

		//< PRINT_VAR(incDifference);

		ap_current_header->temporalSp.remainForOffset = (ptsDifference * pMecIn_parameters->vop_time_increment_resolution +
														 ap_previous_header->temporalSp.remainForOffset)%90000;

		/*compute moduloTimeBase and vopTimeIncrement value*/
		//< PRINT_VAR(ap_previous_header->temporalSp.vopTimeIncrement);
		ap_current_header->temporalSp.vopTimeIncrement = ap_previous_header->temporalSp.vopTimeIncrement + incDifference;
		ap_current_header->temporalSp.moduloTimeBase = 0;

#if 0
		PRINT_VAR(ap_current_header->temporalSp.moduloTimeBase);
		PRINT_VAR(ap_current_header->temporalSp.vopTimeIncrement);
		PRINT_VAR(pMecIn_parameters->vop_time_increment_resolution);
#endif

		while(ap_current_header->temporalSp.vopTimeIncrement >= pMecIn_parameters->vop_time_increment_resolution)
		{
#if 0
			PRINT_VAR(ap_current_header->temporalSp.vopTimeIncrement);
			PRINT_VAR(pMecIn_parameters->vop_time_increment_resolution);
#endif

			ap_current_header->temporalSp.vopTimeIncrement -= pMecIn_parameters->vop_time_increment_resolution;
			ap_current_header->temporalSp.moduloTimeBase++;
#if 0
			PRINT_VAR(ap_current_header->temporalSp.vopTimeIncrement);
			PRINT_VAR(pMecIn_parameters->vop_time_increment_resolution);
#endif

		}

		//The following check ensures that if the previous frame was skipped and if the second-count changed
		// at the frame (for instance, changed from 1.67 to 2.00 seconds), then the moduloTimeBase of the
		// current frame (that may be written in the bitstream) will be correct. Otherwise, since the
		// VOP_TIME_INCREMENT of the current frame is not >= VOP_TIME_INCREMENT_RESOLUTION, the above while-loop
		// will not run, and MODULO_TIME_BASE will remain 0, resulting in wrong value written in VOP header.
		if(previous_skipped_flag)
		{
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of previous_skipped_flag (0x%x) at line no %d \n",previous_skipped_flag,__LINE__);
			PRINT_VAR(previous_skipped_flag);
			/* +Change for enabling skipping as modulo time base should be calculated from the previous non skipped frame with positive modulo time base(done for ER - 354700)*/
			ap_current_header->temporalSp.moduloTimeBase += ap_previous_header->temporalSp.moduloTimeBase;
			/* -Change for enabling skipping as modulo time base should be calculated from the previous non skipped frame with positive modulo time base(done for ER - 354700)*/

		}

#if 0
		PRINT_VAR(ap_current_header->temporalSp.moduloTimeBase);
#endif
		pMecIn_parameters->modulo_time_base   = ap_current_header->temporalSp.moduloTimeBase;
		pMecIn_parameters->vop_time_increment = ap_current_header->temporalSp.vopTimeIncrement;
	}
	//< experimental change //< FIXME
	pMecIn_parameters->modulo_time_base   = ap_current_header->temporalSp.moduloTimeBase;
	pMecIn_parameters->vop_time_increment = ap_current_header->temporalSp.vopTimeIncrement;


	/*write VOS+VO+VOL for first frame*/
	if(/*(frame_number !=1) && */(pMecIn_parameters->picture_coding_type == 0))
	{
		// if 	isSystemHeaderAddBeforeIntra active then reference model start with vos+vo so ....

		if(1)
		{
			WriteBits(&writeStream,VOS_START_CODE/*0x000001b0*/,THIRTY_TWO_BITS);

			switch(pMecIn_custom_parameters->profile)
			{
				case(0):	//simple profile
					switch(pMecIn_custom_parameters->level)
					{
						case(0):	//level0
							WriteBits(&writeStream,0x08,EIGHT_BITS);
							break;
							/*case(OMX_VIDEO_MPEG4Level0b):
								WriteBits(&writeStream,0x01,8);
								break;*/
						case(1):	//level1
							WriteBits(&writeStream,0x01,EIGHT_BITS);
							break;
						case(2):	//level2
							WriteBits(&writeStream,0x02,EIGHT_BITS);
							break;
						case(3):	//level3
							WriteBits(&writeStream,0x03,EIGHT_BITS);
							break;
						case(4):	//level4a
							WriteBits(&writeStream,0x04,EIGHT_BITS);
							break;
						case(5):	//level5
							WriteBits(&writeStream,0x05,EIGHT_BITS);
							break;
						default:
							WriteBits(&writeStream,0x06,EIGHT_BITS);	//choose default L6
							break;
					}
					break;
				default:
					WriteBits(&writeStream,0x06,EIGHT_BITS);	//by defualt we choose SP@L6
					break;
			}

			WriteBits(&writeStream,VO_START_CODE/*0x000001b5*/,THIRTY_TWO_BITS);
			WriteBits(&writeStream,0,ONE_BIT);
			WriteBits(&writeStream,1,FOUR_BITS);

			if( pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_UNKNOWN)
			{
				WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_signal_type
				WriteBits(&writeStream,1,TWO_BITS/*2*/);	// stuffing_bits
			}
			else
			{
				WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_signal_type
				WriteBits(&writeStream,0x5,THREE_BITS/*3*/);		// video_format

				if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_range is 0
				}
				else if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_FULLRANGE)
				{
					WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_range is 1
				}

				WriteBits(&writeStream,1,ONE_BIT/*1*/);		// colour_description

				if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// color_primaries
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// transfer_characteristics
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// matrix_coefficients
				}
				else if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// color_primaries
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// transfer_characteristics
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// matrix_coefficients
				}

				if(writeStream.totalBitsWritten %8 !=0)
				{
					WriteBits(&writeStream,0,ONE_BIT/*1*/);
					while(writeStream.totalBitsWritten %8 !=0)
					{
						WriteBits(&writeStream,1,ONE_BIT/*1*/);
					}
				}
			}
		}
		//write video object
		WriteBits(&writeStream,VO_CODE/*0x00000100*/,THIRTY_TWO_BITS);
		//write vol
		WriteBits(&writeStream,VOL_CODE/*0x00000120*/,THIRTY_TWO_BITS);
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,1,EIGHT_BITS);
		WriteBits(&writeStream,0,ONE_BIT);

		if(pMecIn_custom_parameters->par_horizontal == 1 && pMecIn_custom_parameters->par_vertical == 1)
		{
			WriteBits(&writeStream,1,FOUR_BITS/*4*/);	// Aspect Ratio	= Square
		}
		else if(pMecIn_custom_parameters->par_horizontal == 4 && pMecIn_custom_parameters->par_vertical == 3)
		{
			WriteBits(&writeStream,0x2,FOUR_BITS/*4*/);	// Aspect Ratio	= 4:3
		}
		else if(pMecIn_custom_parameters->par_horizontal == 16 && pMecIn_custom_parameters->par_vertical == 9)
		{
			WriteBits(&writeStream,0x4,FOUR_BITS/*4*/);	// Aspect Ratio	= 16:9
		}
		else
		{
			WriteBits(&writeStream,0xF,FOUR_BITS/*4*/);	// Use Extended_PAR
			WriteBits(&writeStream,pMecIn_custom_parameters->par_horizontal,EIGHT_BITS/*8*/);	//Horizontal Resolution
			WriteBits(&writeStream,pMecIn_custom_parameters->par_vertical,EIGHT_BITS/*8*/);	//Vertical Resolution
		}

		OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: pMecIn_custom_parameters->short_dci_enable: %d ", pMecIn_custom_parameters->short_dci_enable);
		#ifndef __MPEG4ENC_ARM_NMF
			PRINT_VAR(pMecIn_custom_parameters->short_dci_enable);
		#endif

		/*in case of no bufferring we don't write vbv info*/
		if(pMecIn_custom_parameters->buffering_model == SVA_BUFFERING_NONE || (pMecIn_custom_parameters->short_dci_enable == 1))	//FIXME define index/structs for buffering model
		{
			WriteBits(&writeStream,0,ONE_BIT);//no vbv
		}
		else
		{
			WriteBits(&writeStream,1,ONE_BIT);//vol control param
			WriteBits(&writeStream,1,TWO_BITS);//chroma format
			WriteBits(&writeStream,1,ONE_BIT);//low delay
			WriteBits(&writeStream,1,ONE_BIT);//vbv_param
			WriteBits(&writeStream,0,FIFTEEN_BITS);//first_half_bit_rate
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
			WriteBits(&writeStream,bitRate/400,FIFTEEN_BITS);//first_half_bit_rate
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit

			//< if(swisBuffer%16384)
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384+1)>>3,FIFTEEN_BITS);//first_half_vbv_buffer_size
			//< else
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384)>>3,FIFTEEN_BITS);//first_half_vbv_buffer_size
			WriteBits(&writeStream,vbvBufferSize>>3,FIFTEEN_BITS/*15*/);//first_half_vbv_buffer_size

			WriteBits(&writeStream,1,ONE_BIT);//marker_bit

			//< if(swisBuffer%16384)
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384+1)&7,THREE_BITS);//latter_half_vbv_buffer_size
			//< else
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384)&7,THREE_BITS);//latter_half_vbv_buffer_size

			WriteBits(&writeStream,vbvBufferSize & 0x7,THREE_BITS/*3*/);//latter_half_vbv_buffer_size

			//< WriteBits(&writeStream,0,ELEVEN_BITS);//first_half_vbv_occupancy
			WriteBits(&writeStream,vbvOccupancy>>15,ELEVEN_BITS/*11*/);//first_half_vbv_occupancy
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
			//< WriteBits(&writeStream,vbvOccupancy/64,FIFTEEN_BITS);//first_half_vbv_occupancy
			WriteBits(&writeStream,vbvOccupancy & 0x7FFF,FIFTEEN_BITS/*15*/);//latter_half_vbv_occupancy
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
		}
		WriteBits(&writeStream,0,TWO_BITS);//rectangular
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->vop_time_increment_resolution,SIXTEEN_BITS/*16*/);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//fix_vop_rated
		//WriteBits(&writeStream,pMp4Conf->vopTimeIncrement,nbBits);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->window_width,THIRTEEN_BITS);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->window_height,THIRTEEN_BITS);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//interlaced
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//sprite enable
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,1,ONE_BIT);//complexity disable
		if(pMecIn_parameters->data_partitioned == 1)
		{
			WriteBits(&writeStream,0,ONE_BIT); // resync marker disable
			WriteBits(&writeStream,1,ONE_BIT);//partitionned data
			if(pMecIn_parameters->reversible_vlc == 1)
			{
				WriteBits(&writeStream,1,ONE_BIT);//reversible code
			}
			else
			{
				WriteBits(&writeStream,0,ONE_BIT);//no reversible code
			}
		}
		else
		{
			WriteBits(&writeStream,1,ONE_BIT);
			WriteBits(&writeStream,0,ONE_BIT);//no partitionned data
		}

		WriteBits(&writeStream,0,ONE_BIT);//scalability

		if(writeStream.totalBitsWritten %8 !=0)
		{
			WriteBits(&writeStream,0,ONE_BIT);
			while(writeStream.totalBitsWritten %8 !=0)
			{
				WriteBits(&writeStream,1,ONE_BIT);
			}
		}
	}	//end of if

	#ifndef __MPEG4ENC_ARM_NMF
		printf("End of writing VOS+VO+VOL header\n");
	#endif
	PRINT_VAR(writeStream.totalBitsWritten);

	//write vop header until quant  value
	WriteBits(&writeStream,VOP_CODE/*0x000001b6*/,THIRTY_TWO_BITS);	//vop start code

	#ifdef LOGS_ENABLED
		PRINT_VAR(pMecIn_parameters->picture_coding_type);
	#endif

	if(pMecIn_parameters->picture_coding_type == 0/*OMX_VIDEO_PictureTypeI*/)/*current picture coding type I*/
		WriteBits(&writeStream,/*OMX_VIDEO_PictureTypeI*/0,TWO_BITS);	//write 00
	else
		WriteBits(&writeStream,/*OMX_VIDEO_PictureTypeP*/1,TWO_BITS);	//write 01

	/*timestamp related fields*/
	modulo_time_base = ap_current_header->temporalSp.moduloTimeBase;

	while(modulo_time_base !=0)
	{
		WriteBits(&writeStream,1,ONE_BIT);
		modulo_time_base--;
	}
	WriteBits(&writeStream,0,ONE_BIT);
	WriteBits(&writeStream,1,ONE_BIT);	//sva marker bit
	WriteBits(&writeStream,ap_current_header->temporalSp.vopTimeIncrement, ap_current_header->temporalSp.vopTimeIncrementBitSize);
	WriteBits(&writeStream,1,ONE_BIT);
	WriteBits(&writeStream,1,ONE_BIT);
	/*vop rounding related bits*/
	//if current picture type is P
	if(pMecIn_parameters->picture_coding_type == 1/*OMX_VIDEO_PictureTypeP*/)
	{
		if(pMecIn_parameters->rounding_type == SVA_RTYPE_MODE_CONSTANT_ZERO)
			WriteBits(&writeStream,0,ONE_BIT);
		else if(pMecIn_parameters->rounding_type == SVA_RTYPE_MODE_CONSTANT_ONE)
			WriteBits(&writeStream,1,ONE_BIT);
		else
			WriteBits(&writeStream,1-pMecIn_parameters->rounding_type,ONE_BIT);
	}
	WriteBits(&writeStream,0,THREE_BITS);

	#ifndef __MPEG4ENC_ARM_NMF
		printf("End of writing VOP header\n");
	#endif
	PRINT_VAR(writeStream.totalBitsWritten);

	flushWriteStream(&writeStream);

	ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size = SVP_SHAREDMEM_TO16(writeStream.totalBitsWritten); //size in bits!

	OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size (0x%x) at line no %d \n",ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size,__LINE__);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size);

	OstTraceFiltInst0(TRACE_API, "<= Mpeg4enc ddep:  SP_headerCreation()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SP_headerCreation() exit\n");
	#endif

	return writeStream.totalBitsWritten;
}



#ifdef	__MPEG4ENC_ARM_NMF
t_uint32 mpeg4enc_arm_nmf_parser::Write_VOS_VO_VOL(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters, t_uint16 *addr_pointer)
{
	t_sva_ec_writestream_header writeStream;

	//>t_uint32 nbBits=0;
	t_uint32 bitRate;
	t_uint32 vbvBufferSize;
	t_uint32 vbvOccupancy;
	//>t_uint32 swisBuffer;


	OstTraceFiltInst0(TRACE_API, "=> Mpeg4enc ddep:  Write_VOS_VO_VOL()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SP_Write_VOS_VO_VOL() enter\n");
	#endif



	//>swisBuffer = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->swis_buffer);
	bitRate = SVP_SHAREDMEM_FROM16(pMecIn_parameters->bit_rate);
	vbvBufferSize = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->vbv_buffer_size)/16384;	//J/ Dividing, because the value passed from proxy side was the actual value, and not in 16384 units
	vbvOccupancy = SVP_SHAREDMEM_FROM16(pMecIn_custom_parameters->vbv_occupancy);

	/* Header writing
	   -------------- */
//>	addr_pointer = (t_uint16 SHAREDMEM* )(param_desc_mpc_addr);

	writeStream.buffer = 0;
	writeStream.currBuffer= addr_pointer;
	writeStream.endbuffer = addr_pointer+HEADER_BUFFER_SIZE;
	writeStream.nbBitsValid=0;
	writeStream.totalBitsWritten=0;

	/*write VOS+VO+VOL for first frame*/
	if(/*(frame_number !=1) && */(pMecIn_parameters->picture_coding_type == 0))
	{
		// if 	isSystemHeaderAddBeforeIntra active then reference model start with vos+vo so ....

		if(1)
		{
			WriteBits(&writeStream,VOS_START_CODE/*0x000001b0*/,THIRTY_TWO_BITS);

			switch(pMecIn_custom_parameters->profile)
			{
				case(0):	//simple profile
					switch(pMecIn_custom_parameters->level)
					{
						case(0):	//level0
							WriteBits(&writeStream,0x08,EIGHT_BITS);
							break;
							/*case(OMX_VIDEO_MPEG4Level0b):
								WriteBits(&writeStream,0x01,8);
								break;*/
						case(1):	//level1
							WriteBits(&writeStream,0x01,EIGHT_BITS);
							break;
						case(2):	//level2
							WriteBits(&writeStream,0x02,EIGHT_BITS);
							break;
						case(3):	//level3
							WriteBits(&writeStream,0x03,EIGHT_BITS);
							break;
						case(4):	//level4a
							WriteBits(&writeStream,0x04,EIGHT_BITS);
							break;
						case(5):	//level5
							WriteBits(&writeStream,0x05,EIGHT_BITS);
							break;
						default:
							WriteBits(&writeStream,0x06,EIGHT_BITS);	//choose default L6
							break;
					}
					break;
				default:
					WriteBits(&writeStream,0x06,EIGHT_BITS);	//by defualt we choose SP@L6
					break;
			}

			WriteBits(&writeStream,VO_START_CODE/*0x000001b5*/,THIRTY_TWO_BITS);
			WriteBits(&writeStream,0,ONE_BIT);
			WriteBits(&writeStream,1,FOUR_BITS);

			if( pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_UNKNOWN)
			{
				WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_signal_type
				WriteBits(&writeStream,1,TWO_BITS/*2*/);	// stuffing_bits
			}
			else
			{
				WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_signal_type
				WriteBits(&writeStream,0x5,THREE_BITS/*3*/);		// video_format

				if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_range is 0
				}
				else if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_FULLRANGE)
				{
					WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_range is 1
				}

				WriteBits(&writeStream,1,ONE_BIT/*1*/);		// colour_description

				if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// color_primaries
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// transfer_characteristics
					WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// matrix_coefficients
				}
				else if(pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_FULLRANGE || pMecIn_custom_parameters->color_primary == SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE)
				{
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// color_primaries
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// transfer_characteristics
					WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// matrix_coefficients
				}

				if(writeStream.totalBitsWritten %8 !=0)
				{
					WriteBits(&writeStream,0,ONE_BIT/*1*/);
					while(writeStream.totalBitsWritten %8 !=0)
					{
						WriteBits(&writeStream,1,ONE_BIT/*1*/);
					}
				}
			}
		}
		//write video object
		WriteBits(&writeStream,VO_CODE/*0x00000100*/,THIRTY_TWO_BITS);
		//write vol
		WriteBits(&writeStream,VOL_CODE/*0x00000120*/,THIRTY_TWO_BITS);
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,1,EIGHT_BITS);
		WriteBits(&writeStream,0,ONE_BIT);

		if(pMecIn_custom_parameters->par_horizontal == 1 && pMecIn_custom_parameters->par_vertical == 1)
		{
			WriteBits(&writeStream,1,FOUR_BITS/*4*/);	// Aspect Ratio	= Square
		}
		else if(pMecIn_custom_parameters->par_horizontal == 4 && pMecIn_custom_parameters->par_vertical == 3)
		{
			WriteBits(&writeStream,0x2,FOUR_BITS/*4*/);	// Aspect Ratio	= 4:3
		}
		else if(pMecIn_custom_parameters->par_horizontal == 16 && pMecIn_custom_parameters->par_vertical == 9)
		{
			WriteBits(&writeStream,0x4,FOUR_BITS/*4*/);	// Aspect Ratio	= 16:9
		}
		else
		{
			WriteBits(&writeStream,0xF,FOUR_BITS/*4*/);	// Use Extended_PAR
			WriteBits(&writeStream,pMecIn_custom_parameters->par_horizontal,EIGHT_BITS/*8*/);	//Horizontal Resolution
			WriteBits(&writeStream,pMecIn_custom_parameters->par_vertical,EIGHT_BITS/*8*/);	//Vertical Resolution
		}

		/*in case of no bufferring we don't write vbv info*/
		if((pMecIn_custom_parameters->buffering_model == SVA_BUFFERING_NONE)|| (pMecIn_custom_parameters->short_dci_enable == OMX_TRUE))	//FIXME define index/structs for buffering model
		{
			WriteBits(&writeStream,0,ONE_BIT);//no vbv
		}
		else
		{
			WriteBits(&writeStream,1,ONE_BIT);//vol control param
			WriteBits(&writeStream,1,TWO_BITS);//chroma format
			WriteBits(&writeStream,1,ONE_BIT);//low delay
			WriteBits(&writeStream,1,ONE_BIT);//vbv_param
			WriteBits(&writeStream,0,FIFTEEN_BITS);//first_half_bit_rate
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
			WriteBits(&writeStream,bitRate/400,FIFTEEN_BITS);//first_half_bit_rate
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit

			//< if(swisBuffer%16384)
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384+1)>>3,FIFTEEN_BITS);//first_half_vbv_buffer_size
			//< else
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384)>>3,FIFTEEN_BITS);//first_half_vbv_buffer_size
			WriteBits(&writeStream,vbvBufferSize>>3,FIFTEEN_BITS/*15*/);//first_half_vbv_buffer_size

			WriteBits(&writeStream,1,ONE_BIT);//marker_bit

			//< if(swisBuffer%16384)
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384+1)&7,THREE_BITS);//latter_half_vbv_buffer_size
			//< else
			//< 	WriteBits(&writeStream,(vbvBufferSize/16384)&7,THREE_BITS);//latter_half_vbv_buffer_size

			WriteBits(&writeStream,vbvBufferSize & 0x7,THREE_BITS/*3*/);//latter_half_vbv_buffer_size

			//< WriteBits(&writeStream,0,ELEVEN_BITS);//first_half_vbv_occupancy
			WriteBits(&writeStream,vbvOccupancy>>15,ELEVEN_BITS/*11*/);//first_half_vbv_occupancy
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
			//< WriteBits(&writeStream,vbvOccupancy/64,FIFTEEN_BITS);//first_half_vbv_occupancy
			WriteBits(&writeStream,vbvOccupancy & 0x7FFF,FIFTEEN_BITS/*15*/);//latter_half_vbv_occupancy
			WriteBits(&writeStream,1,ONE_BIT);//marker_bit
		}
		WriteBits(&writeStream,0,TWO_BITS);//rectangular
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->vop_time_increment_resolution,SIXTEEN_BITS/*16*/);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//fix_vop_rated
		//WriteBits(&writeStream,pMp4Conf->vopTimeIncrement,nbBits);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->window_width,THIRTEEN_BITS);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,pMecIn_parameters->window_height,THIRTEEN_BITS);
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//interlaced
		WriteBits(&writeStream,1,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);//sprite enable
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,0,ONE_BIT);
		WriteBits(&writeStream,1,ONE_BIT);//complexity disable
		if(pMecIn_parameters->data_partitioned == 1)
		{
			WriteBits(&writeStream,0,ONE_BIT); // resync marker disable
			WriteBits(&writeStream,1,ONE_BIT);//partitionned data
			if(pMecIn_parameters->reversible_vlc == 1)
			{
				WriteBits(&writeStream,1,ONE_BIT);//reversible code
			}
			else
			{
				WriteBits(&writeStream,0,ONE_BIT);//no reversible code
			}
		}
		else
		{
			WriteBits(&writeStream,1,ONE_BIT);
			WriteBits(&writeStream,0,ONE_BIT);//no partitionned data
		}

		WriteBits(&writeStream,0,ONE_BIT);//scalability

		if(writeStream.totalBitsWritten %8 !=0)
		{
			WriteBits(&writeStream,0,ONE_BIT);
			while(writeStream.totalBitsWritten %8 !=0)
			{
				WriteBits(&writeStream,1,ONE_BIT);
			}
		}
	}	//end of if

	OstTraceFiltInst0(TRACE_API, "<= Mpeg4enc ddep:  Write_VOS_VO_VOL()");
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  SP_Write_VOS_VO_VOL() exit\n");
	#endif


	return writeStream.totalBitsWritten;
}

#endif

STATIC_FLAG void mp4e_init_buffer_link(t_ahb_address physical_address, ts_ddep_buffer_descriptor *buffer_desc)
{
#ifdef __MPEG4ENC_ARM_NMF

#else
	bsm.ps_ddep_bitstream_buf_link_and_header = (ts_ddep_bitstream_buf_link_and_header SHAREDMEM*)(buffer_desc->nMpcAddress);
	ps_ddep_bitstream_buf_link_and_header = bsm.ps_ddep_bitstream_buf_link_and_header;

	bsm.physical_ps_ddep_bitstream_buf_link_and_header = physical_address;

	bsm.last_index_ps_ddep_bitstream_buf_link_and_header=(buffer_desc->nSize)/sizeof(ts_ddep_bitstream_buf_link_and_header);


	if(bsm.last_index_ps_ddep_bitstream_buf_link_and_header==0)	printf("Mpeg4enc ddep:  last_index_ps_ddep_bitstream_buf_link=0");

	bsm.index_to_next_not_required_bitstream_buffer = 0;
	buffer_desc->nSize-=bsm.last_index_ps_ddep_bitstream_buf_link_and_header*sizeof(ts_ddep_bitstream_buf_link_and_header);
	//buffer_desc->nPhysicalAddress=bsm.last_index_ps_ddep_bitstream_buf_link_and_header*sizeof(ts_ddep_bitstream_buf_link_and_header);

	/* create buffer link */
	/*blocks physical address = physical_add + sizeof(link_str)*4*block_index */
#endif
}

#ifdef __MPEG4ENC_ARM_NMF
STATIC_FLAG void mp4e_fill_link_structure(OMX_BUFFERHEADERTYPE_p pSrc)
#else
STATIC_FLAG void mp4e_fill_link_structure(Buffer_p pSrc)
#endif
{
#ifdef __MPEG4ENC_ARM_NMF

#else
	t_uint16 at_index = bsm.current_bitstream_buffer_index;
	t_ahb_address curr;

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_fill_link_structure() enter\n");
		printf("Mpeg4enc ddep:  mp4e_fill_link_structure() enter\n");
		PRINT_VAR(at_index);
		OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of at_index (0x%x) at line no %d \n",at_index,__LINE__);
	#endif

	OstTraceFiltInst3(TRACE_FLOW, "Mpeg4enc ddep Value of current_bitstream_buffer_index :0x%x last_index_ps_ddep_bitstream_buf_link_and_header : 0x%x at line no %d\n",bsm.current_bitstream_buffer_index,bsm.last_index_ps_ddep_bitstream_buf_link_and_header,__LINE__);
	if(bsm.current_bitstream_buffer_index>=bsm.last_index_ps_ddep_bitstream_buf_link_and_header)
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc ddep:  No More buffer link header possible\n");
		#endif
		while(1);
	}

	curr = bsm.physical_ps_ddep_bitstream_buf_link_and_header + sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)2*((t_uint32)at_index);

	//PRINT_VAR(curr);
	//PRINT_VAR(at_index);

	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16((curr+(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)2)|EXT_BIT);
	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(at_index?((curr-(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)2))|EXT_BIT:(t_uint32)0);
	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(pSrc->address));/*|EXT_BIT; *//*FIXME:  + CED(pSrc->nOffset) ignored offset*/
	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(pSrc->address) + 2*SVP_SHAREDMEM_FROM24(pSrc->allocLen));/*|EXT_BIT);*/	/*ignored offset*/
#if 0
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start ));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   ));
#endif
	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_buffer_p = SVP_SHAREDMEM_TO16((t_uint32)pSrc);

	bsm.current_bitstream_buffer_index++;

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_fill_link_structure() exit\n");
		printf("Mpeg4enc ddep:  mp4e_fill_link_structure() exit\n");
	#endif

#endif
}

STATIC_FLAG void mp4e_mark_end_link_structure()
{
#ifdef __MPEG4ENC_ARM_NMF

#else
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_mark_end_link_structure() enter\n");
		printf("Mpeg4enc ddep:  mp4e_mark_end_link_structure() enter\n");
	#endif

	{
		/*at first step add fakebuffer in the end */
		Buffer_p FakeBuffer = fakeBufIn;
		mp4e_fill_link_structure(fakeBufIn); /*make sure that nFlag is 0xFFFFFFFFul to inform it not send this buffer back */
	}

	{
		t_uint32 at_index = bsm.current_bitstream_buffer_index-1;
		t_ahb_address curr;

		curr = bsm.physical_ps_ddep_bitstream_buf_link_and_header + sizeof(ts_ddep_bitstream_buf_link_and_header)*2*at_index;

		//bsm.ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = bsm.physical_ps_ddep_bitstream_buf_link_and_header|EXT_BIT;
		//bsm.ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_prev_buf_link = curr|EXT_BIT;
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(0);
		ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(0);

		/*FIXME.re-check point: add -48 byte workaround*/
		ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start) - 48ul);
#if 0
		PRINT_VAR(at_index);
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start ));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   ));
#endif
#if 0
		OstTraceFiltInst0(TRACE_FLOW, "\n");
		printf("\n");
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_next_buf_link));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_prev_buf_link));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start ));
		PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_end   ));

#endif
	}

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_mark_end_link_structure() exit\n");
		printf("Mpeg4enc ddep:  mp4e_mark_end_link_structure() exit\n");
	#endif

#endif
}

#ifdef __MPEG4ENC_ARM_NMF
STATIC_FLAG void mp4e_get_next_not_required_bitstream_buffer(OMX_BUFFERHEADERTYPE_p *bufferIn)
#else
STATIC_FLAG void mp4e_get_next_not_required_bitstream_buffer(Buffer_p *bufferIn)
#endif
{
#ifdef __MPEG4ENC_ARM_NMF

#else
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_get_next_not_required_bitstream_buffer() enter\n");
		printf("Mpeg4enc ddep:  mp4e_get_next_not_required_bitstream_buffer() enter\n");
	#endif

	*bufferIn=0;

	if(bsm.current_bitstream_buffer_index==0) return;

#if 0
	PRINT_VAR(bsm.current_bitstream_buffer_index);
	PRINT_VAR(bsm.index_to_next_not_required_bitstream_buffer);
#endif

	while(1)
	{
		Buffer_p SHAREDMEM pBuffer;
		if(bsm.index_to_next_not_required_bitstream_buffer == bsm.current_bitstream_buffer_index)
		{
			/*reset indexes*/
			bsm.index_to_next_not_required_bitstream_buffer = 0;
			bsm.current_bitstream_buffer_index = 0;
			break;
		}

		pBuffer = SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[bsm.index_to_next_not_required_bitstream_buffer].s_ddep_buffer_p);
		bsm.index_to_next_not_required_bitstream_buffer++;

		//if (pBuffer != Fake_buffer && pBuffer != Last_buffer)
		if(pBuffer == fakeBufIn) continue;

		*bufferIn = pBuffer;
		break;
	}

	#ifdef LOGS_ENABLED
		//<>PG PRINT_VAR(bsm.index_to_next_not_required_bitstream_buffer);
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  mp4e_get_next_not_required_bitstream_buffer() exit\n");
		PRINT_VAR(bsm.index_to_next_not_required_bitstream_buffer);
		printf("Mpeg4enc ddep:  mp4e_get_next_not_required_bitstream_buffer() exit\n");
	#endif
#endif
}

#ifndef __MPEG4ENC_ARM_NMF
/*****************************************************************************/
/**
 * \brief  disableFWCodeexection
 *
 * Set whether FW code exec is to be disabled for ARM load computation
 */
/*****************************************************************************/
PUT_PRAGMA
void METH(disableFWCodeexection)(t_bool value)
{
	isFWDisabled = value;
}
#endif

/*****************************************************************************/
/**
 * \brief  setNeeds
 *
 * Set to this component memory needs (internal buffers)
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(setNeeds)(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc)
{
/*channelId, linklistbufferDesc, debugbufferDesc*/
	/*	Description of buffer needs :
			- bufferDesc0 : linked list mgmt
			- bufferDesc1 : Debug Buffer inside which debug information will be stored
	*/
#ifdef __MPEG4ENC_ARM_NMF
	// currently, not doing anything for arm-nmf
#else

	// assuming 16 bit memory allocation from ARM side
	bbm_desc.nSize>>=1;
	debugBuffer_desc.nSize>>=1;

	#ifdef __DEBUG_TRACE_ENABLE
		mpc_trace_init(debugBuffer_desc.nMpcAddress, debugBuffer_desc.nSize);
	#endif

		printf("Mpeg4enc ddep:  setNeeds sizes bbm=%X, dbg=%X\n", bbm_desc.nSize, debugBuffer_desc.nSize);

	#if 0
		PRINT_VAR(bbm_desc.nSize            );
		PRINT_VAR(bbm_desc.nPhysicalAddress );
		PRINT_VAR(bbm_desc.nLogicalAddress  );
		PRINT_VAR(bbm_desc.nMpcAddress      );
	#endif

		s_mpeg4e_ddep_desc.bbm_desc = bbm_desc;
		s_mpeg4e_ddep_desc.s_debug_buffer_desc = debugBuffer_desc;

		iSleep.setDebug(0, s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress, 0);

		PRINT_VAR(isFWDisabled);
		//>OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of isFWDisabled (0x%x) at line no %d \n",isFWDisabled,__LINE__);
		if (isFWDisabled) {
			//>OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc ddep: isFWDisabled is TRUE\n");
			printf("MPEG4Enc ddep: isFWDisabled is TRUE\n");
			codec_algo.setDebug(DBG_MODE_NO_HW, convto16bitmode(s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		}
		else {
			//>OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc ddep: isFWDisabled is FALSE\n");
			printf("MPEG4Enc ddep: isFWDisabled is FALSE\n");
			codec_algo.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		}

		//< iResource.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		iBrc.setDebug(DBG_MODE_NORMAL, s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress, 0);
		PRINT_VAR(s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress);
		//>OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress (0x%x) at line no %d \n",s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress,__LINE__);

		mp4e_init_buffer_link(bbm_desc.nPhysicalAddress, &bbm_desc);
		{
			/*mpeg4e_init_buffer_link(bbm_desc.nPhysicalAddress, &bbm_desc); to be implemented for actual bbm*/
			//ps_ddep_bitstream_buf_link_and_header = (ts_ddep_bitstream_buf_link_and_header SHAREDMEM *)bbm_desc.nMpcAddress;
		}
#endif
}



/*****************************************************************************/
/**
 * \brief  setCropForStab
 *
 * Set the crop parameters for Video Stabilization
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(setCropForStab)(t_uint32 crop_left_offset, t_uint32 crop_top_offset, t_uint32 overscan_width, t_uint32 overscan_height)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc ddep: setCropForStab()");
	printf("Mpeg4enc ddep:  setCropForStab() enter\n");

	mStabCropVectorX[stab_crop_array_end_plus1] = crop_left_offset;
	mStabCropVectorY[stab_crop_array_end_plus1] = crop_top_offset;

	mStabCropWindowWidth[stab_crop_array_end_plus1]		= overscan_width;
	mStabCropWindowHeight[stab_crop_array_end_plus1]	= overscan_height;

	stab_crop_array_end_plus1++;

	OstTraceFiltInst2(TRACE_FLOW, "In setCropForStab:: Offsets X,Y= 0x%x 0x%x \n", crop_left_offset, crop_top_offset);
	OstTraceFiltInst2(TRACE_FLOW, "In setCropForStab:: Overscan WindowW,H= 0x%x 0x%x\n", overscan_width, overscan_height);

	if(stab_crop_array_end_plus1 == STAB_ARRAY_SIZE) {
		stab_crop_array_end_plus1	= 0;
	}

	OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: setCropForStab()");

	printf("Mpeg4enc ddep:  setCropForStab() exit\n");
}




/*****************************************************************************/
/**
 * \brief  setConfig
 *
 * Unused
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(setConfig)(t_uint16 channelId)
{
	//>t_uint32 x = 0x12345678ul;
}

STATIC_FLAG t_uint16 COMP(download_parameters)(void)
{
	t_uint16 retry = 0;
	t_uint16 l_atomic_current_conf;

	OstTraceInt0(TRACE_API, "=> Mpeg4enc ddep:  download_parameters()");


	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  download_parameters() enter\n");
	#endif


	if(!set_param_done)
	{
		OstTraceInt0(TRACE_FLOW, "<= Mpeg4enc ddep:  download_parameters() (!set_param_done)");
		return 0;
	}

	l_atomic_current_conf = ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit;
	OstTraceInt1(TRACE_FLOW, "<= Mpeg4enc ddep:  l_atomic_current_conf -%d",l_atomic_current_conf);
	if(l_atomic_current_conf == last_read_conf_no) //already using latest configuration
	{
		OstTraceInt0(TRACE_FLOW, "<= Mpeg4enc ddep:  download_parameters() (l_atomic_current_conf == last_read_conf_no)");
		return 1;
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  download_parameters()==>new setting available\n");
	#endif


	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_fwd_ref_buffer));

	while(1)
	{
		t_uint16 SHAREDMEM * src;
		t_uint16 SHAREDMEM * dest;
		t_uint16 count;
		t_uint16 atomic_entry_exit;

		t_uint16 SHAREDMEM *host_address_start, *mpc_address_start, *tmp_address_start;
		t_uint16 SHAREDMEM *host_cust_address_start, *mpc_cust_address_start, *tmp_cust_address_start;
		ts_ddep_vec_mpeg4_param_desc SHAREDMEM *null_desc = (ts_ddep_vec_mpeg4_param_desc SHAREDMEM *)NULL;

		t_uint16 param_buff_size1, param_buff_size2;

		retry++;
		if(retry > 10)
		{/*raise error event here*/
			OstTraceInt0(TRACE_FLOW, "<= Mpeg4enc ddep:  download_parameters() (retry > 10)");
			return 0;
		}

	host_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set;
	tmp_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set;
	//from start to in_param
	param_buff_size1 = sizeof(ts_t1xhv_vec_frame_buf_in) + sizeof(ts_t1xhv_vec_frame_buf_out) + sizeof(ts_t1xhv_vec_internal_buf) +
				sizeof(ts_t1xhv_vec_header_buf) + sizeof(ts_t1xhv_bitstream_buf_pos) + sizeof(ts_t1xhv_bitstream_buf_pos) +
				sizeof(ts_t1xhv_vec_mpeg4_param_in);

	host_cust_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set +((t_uint32)(&null_desc->s_in_custom_parameters ));
	tmp_cust_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set + ((t_uint32)(&null_desc->s_in_custom_parameters ));
	//size of custom_param
	param_buff_size2 = sizeof(ts_ddep_vec_mpeg4_custom_param_in);

	//Start filling
	atomic_entry_exit = ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit;
	while(param_buff_size1--) *tmp_address_start++=*host_address_start++;
	while(param_buff_size2--) *tmp_cust_address_start++=*host_cust_address_start++;
	if(atomic_entry_exit == ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry)
	{
		mpc_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set;
		tmp_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set;
		mpc_cust_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set + ((t_uint32)(&null_desc->s_in_custom_parameters));
		tmp_cust_address_start = (t_uint16 SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set + ((t_uint32)(&null_desc->s_in_custom_parameters ));

		param_buff_size1 = sizeof(ts_t1xhv_vec_frame_buf_in) + sizeof(ts_t1xhv_vec_frame_buf_out) + sizeof(ts_t1xhv_vec_internal_buf) +
				sizeof(ts_t1xhv_vec_header_buf) + sizeof(ts_t1xhv_bitstream_buf_pos) + sizeof(ts_t1xhv_bitstream_buf_pos) +
				sizeof(ts_t1xhv_vec_mpeg4_param_in);
		param_buff_size2 = sizeof(ts_ddep_vec_mpeg4_custom_param_in);

		//atomically copied from host to tmp, now copy it to main mpc_param_set
		while(param_buff_size1--) *mpc_address_start++=*tmp_address_start++;
		while(param_buff_size2--) *mpc_cust_address_start++=*tmp_cust_address_start++;

		last_read_conf_no = atomic_entry_exit;

		#ifdef LOGS_ENABLED
			printf("Mpeg4enc ddep:  new setting captured in %d attempts\n", retry);
		#endif

		break;
	}
		// else retry
	}

	new_config_received = 1;
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.framerate);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.bit_rate);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.atomic_count);

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  download_parameters() exit\n");
	#endif

	OstTraceInt0(TRACE_API, "<= Mpeg4enc ddep:  download_parameters()");

	return 1;
}

/*****************************************************************************/
/**
 * \brief  setParameter
 *
 * Set to this component parameters (i.e. those which are passed to be FW)
  */
/*****************************************************************************/
PUT_PRAGMA
#ifdef __MPEG4ENC_ARM_NMF
void METH(setParameter)(t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg4_param_desc)
#else
void METH(setParameter)(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg4_param_desc)
#endif
{
#ifdef __MPEG4ENC_ARM_NMF

	#ifdef ENABLE_ARMNMF_LOGS
		//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  setParameter() enter\n");
		printf("Mpeg4enc ddep:  setParameter() enter\n");
	#endif

	ts_ddep_vec_mpeg4_param_desc *null_desc = (ts_ddep_vec_mpeg4_param_desc *)NULL;

	/*keep mpc address for dynamic configuration*/
	ps_ddep_vec_mpeg4_param_desc_dh			= (ts_ddep_vec_mpeg4_param_desc_dh *)ddep_vec_mpeg4_param_desc.nLogicalAddress;
	ps_ddep_vec_mpeg4_param_desc_mpc 		= (ts_ddep_vec_mpeg4_param_desc *)&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set;
	ps_ddep_vec_mpeg4_param_desc_host_set 	= (ts_ddep_vec_mpeg4_param_desc *)&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set;
	ps_ddep_vec_mpeg4_param_desc_tmp_set 	= (ts_ddep_vec_mpeg4_param_desc *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set;

	//> ? ddep_vec_mpeg4_param_desc.nSize>>=1;

	param.algoId = algoId;

	/* now set physical addresses*/
	/*OPTIMIZATION POINT*/
	param.addr_in_frame_buffer      = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_frame_buffer         ));
	param.addr_out_frame_buffer     = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_out_fram_buffer         ));
	param.addr_internal_buffer      = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_internal_buffer         ));
	param.addr_header_buffer        = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_header_buf              ));
	param.addr_in_bitstream_buffer  = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ));
	param.addr_out_bitstream_buffer = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ));
	param.addr_in_parameters        = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_parameters           ));
	param.addr_out_parameters       = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_out_parameters          ));
	param.addr_in_frame_parameters  = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_frame_parameters ));
	param.addr_out_frame_parameters = (t_uint32)ps_ddep_vec_mpeg4_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_frame_parameters ));


	//physical_address_desc = physical_address_desc + sizeof(ts_ddep_vdc_mpeg4_param_desc);
	ddep_vec_mpeg4_param_desc.nSize -= sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	ddep_vec_mpeg4_param_desc.nLogicalAddress += sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	//> ddep_vec_mpeg4_param_desc.nMpcAddress += sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	//> ddep_vec_mpeg4_param_desc.nPhysicalAddress += sizeof(ts_ddep_vec_mpeg4_param_desc_dh)*(t_uint32)2;

	//> new_phy_addr = VFM_ROUND_UPPER(ddep_vec_mpeg4_param_desc.nPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	//> ddep_vec_mpeg4_param_desc.nMpcAddress = ddep_vec_mpeg4_param_desc.nMpcAddress + ((new_phy_addr - ddep_vec_mpeg4_param_desc.nPhysicalAddress)>>1);
	//> ddep_vec_mpeg4_param_desc.nPhysicalAddress = VFM_ROUND_UPPER(ddep_vec_mpeg4_param_desc.nPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	//param_desc_mpc_addr = ((ddep_vec_mpeg4_param_desc.nLogicalAddress+0xf)&0xfffffff0);
	param_desc_mpc_addr = ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.addr_header_buffer;

#else
	t_uint32 new_phy_addr;

	t_uint32 physical_address_desc = ddep_vec_mpeg4_param_desc.nPhysicalAddress;
	ts_ddep_vec_mpeg4_param_desc SHAREDMEM *null_desc = (ts_ddep_vec_mpeg4_param_desc SHAREDMEM *)NULL;

	#ifdef LOGS_ENABLED
		//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  setParameter() enter\n");
		printf("Mpeg4enc ddep:  setParameter() enter\n");
	#endif

	/*keep mpc address for dynamic configuration*/
	ps_ddep_vec_mpeg4_param_desc_dh 		= (ts_ddep_vec_mpeg4_param_desc_dh SHAREDMEM *)ddep_vec_mpeg4_param_desc.nMpcAddress;
	ps_ddep_vec_mpeg4_param_desc_mpc 		= (ts_ddep_vec_mpeg4_param_desc SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set;
	ps_ddep_vec_mpeg4_param_desc_host_set 	= (ts_ddep_vec_mpeg4_param_desc SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set;
	ps_ddep_vec_mpeg4_param_desc_tmp_set 	= (ts_ddep_vec_mpeg4_param_desc SHAREDMEM *)&ps_ddep_vec_mpeg4_param_desc_dh->tmp_param_set;

	ddep_vec_mpeg4_param_desc.nSize>>=1;

	param.algoId = algoId;

	/* now set physical addresses*/
	/*OPTIMIZATION POINT*/
	param.addr_in_frame_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_in_frame_buffer         ))*(t_uint32)2;
	param.addr_out_frame_buffer     = physical_address_desc + ((t_uint32)(&null_desc->s_out_fram_buffer         ))*(t_uint32)2;
	param.addr_internal_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_internal_buffer         ))*(t_uint32)2;
	param.addr_header_buffer        = physical_address_desc + ((t_uint32)(&null_desc->s_header_buf              ))*(t_uint32)2;
	param.addr_in_bitstream_buffer  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ))*(t_uint32)2;
	param.addr_out_bitstream_buffer = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ))*(t_uint32)2;
	param.addr_in_parameters        = physical_address_desc + ((t_uint32)(&null_desc->s_in_parameters           ))*(t_uint32)2;
	param.addr_out_parameters       = physical_address_desc + ((t_uint32)(&null_desc->s_out_parameters          ))*(t_uint32)2;
	param.addr_in_frame_parameters  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_frame_parameters ))*(t_uint32)2;
	param.addr_out_frame_parameters = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_frame_parameters ))*(t_uint32)2;

	//physical_address_desc = physical_address_desc + sizeof(ts_ddep_vdc_mpeg4_param_desc);
	ddep_vec_mpeg4_param_desc.nSize -= sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	ddep_vec_mpeg4_param_desc.nMpcAddress += sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	ddep_vec_mpeg4_param_desc.nPhysicalAddress += sizeof(ts_ddep_vec_mpeg4_param_desc_dh)*(t_uint32)2;

	new_phy_addr = VFM_ROUND_UPPER(ddep_vec_mpeg4_param_desc.nPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	ddep_vec_mpeg4_param_desc.nMpcAddress = ddep_vec_mpeg4_param_desc.nMpcAddress + ((new_phy_addr - ddep_vec_mpeg4_param_desc.nPhysicalAddress)>>1);
	ddep_vec_mpeg4_param_desc.nPhysicalAddress = VFM_ROUND_UPPER(ddep_vec_mpeg4_param_desc.nPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	param_desc_mpc_addr = ((ddep_vec_mpeg4_param_desc.nMpcAddress+0xf)&0xfffff0);

#endif

	set_param_done = 1;
	get_codec_parameter=1;
	last_read_conf_no = 0;

	download_parameters();

	#ifdef LOGS_ENABLED
		//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  setParameter() exit\n");
		printf("Mpeg4enc ddep:  setParameter() exit\n");
	#endif
}

STATIC_FLAG void COMP(release_resources)(void)
{
#ifdef __MPEG4ENC_ARM_NMF
	//nothing to do for ARM-NMF
	get_hw_resource=0; //changes for ER 350963/ER349672
#else
	// the HW resource must be released to avoid dead lock
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  release_resources() enter\n");
		printf("Mpeg4enc ddep:  release_resources() enter\n");
	#endif
	ENTER_CRITICAL_SECTION;
	if(get_hw_resource)
		iResource.freeResource(RESOURCE_MMDSP, &iInformResourceStatus);
	get_hw_resource=0;
	EXIT_CRITICAL_SECTION;

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  release_resources() exit\n");
		printf("Mpeg4enc ddep:  release_resources() exit\n");
	#endif
#endif
}

#ifndef __MPEG4ENC_ARM_NMF
t_uint16 buffer_available_atinput(Component *this)
{
	if((bufIn==0) && Port_queuedBufferCount(&mPorts[0]))
	{
		bufIn = Port_dequeueBuffer(&this->ports[0]);
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc Ddep: New Input buffer available\n");
		return 1;
	}
	else if(bufIn)
		return 1;
	else
		return 0;
}

t_uint16 buffer_available_atoutput(Component *this)
{
	if((bufOut==0) && Port_queuedBufferCount(&mPorts[1]))
	{
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  dequeue buffer\n");
		printf("Mpeg4enc ddep:  dequeue buffer\n");
	#endif
		bufOut = Port_dequeueBuffer(&this->ports[1]);
		return 1;
	}
	else if(bufOut)
		return 1;
	else
		return 0;
}
#endif

void add_buffer_to_next_link()
{
#ifdef __MPEG4ENC_ARM_NMF

#else
	t_uint16 forcount =0;
	Buffer_p dummy_bufOut;
	Buffer_t SHAREDMEM temp_buffer;

	dummy_bufOut = &temp_buffer;

	bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
	bufOut->nTimeStamph = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStamph));
	bufOut->nTimeStampl = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStampl));

	dummy_bufOut->address = SVP_SHAREDMEM_TO24(0x10000);
	dummy_bufOut->allocLen = SVP_SHAREDMEM_TO24(0);

	#ifdef LOGS_ENABLED
		PRINT_VAR(bufOut->address);
		PRINT_VAR(bufOut->allocLen);
		printf("\n");
		PRINT_VAR(partial_end_algo_count);
		printf("\n");
	#endif
	for(forcount=0; forcount<partial_end_algo_count; forcount++)
	{
		OstTraceFiltInst2(TRACE_FLOW, "MPEG4Enc_ddep, Value of dummy_bufOut->address (0x%x) at line no %d \n",dummy_bufOut->address,__LINE__);
		PRINT_VAR(dummy_bufOut->address);
		OstTraceFiltInst2(TRACE_FLOW, "MPEG4Enc_ddep, Value of dummy_bufOut->allocLen (0x%x) at line no %d \n",dummy_bufOut->allocLen,__LINE__);
		PRINT_VAR(dummy_bufOut->allocLen);
		mp4e_fill_link_structure(dummy_bufOut);//setbufferlink for bitstream buffer
	}

	mp4e_fill_link_structure(bufOut);//setbufferlink for bitstream buffer
	mp4e_mark_end_link_structure();

	PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->address));
	OstTraceFiltInst2(TRACE_FLOW, "MPEG4Enc_ddep, Value of (SVP_SHAREDMEM_FROM24(bufOut->address)) (0x%x) at line no %d \n",(SVP_SHAREDMEM_FROM24(bufOut->address)),__LINE__);

	ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16(s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
	ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->address));
	ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.bitstream_offset = SVP_SHAREDMEM_TO16(0);
#endif

}

/*****************************************************************************/
/**
 * \brief  endAlgo
 *
 *	callback from FW
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(endAlgo)(t_t1xhv_status status, t_t1xhv_encoder_info errors, t_uint32 durationInTicks)
{
	#ifdef LOGS_ENABLED
		hcl_printf("ddepEnA--");
		STORE_END_ALGO_TICKS(ps_ddep_vec_mpeg4_param_desc_mpc/*, _till_ticks*/);
		PRINT_VAR_MY( TICKS());

		hcl_printf("ddepDuT--");
		PRINT_VAR_MY(durationInTicks);
		hcl_printf("\n");

		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  endAlgo() enter\n");
		printf("Mpeg4enc ddep:  endAlgo() enter\n");
	#endif

	iSendLogEvent.eventEndAlgo(status,errors, durationInTicks);

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  endAlgo() exit\n");
		printf("Mpeg4enc ddep:  endAlgo() exit\n");
	#endif
}

PUT_PRAGMA
void METH(eventEndAlgo)(t_t1xhv_status status, t_t1xhv_encoder_info errors, t_uint32 durationInTicks)
{
	t_uint32 bitstream_size = 0;
	//>t_uint32 packet_bitstream_size = 0;
	t_uint16 no_of_packets=0;
	t_uint16 update_algo_called = 0;
	t_uint16 forcount =0;

	#ifdef LOGS_ENABLED
		hcl_printf("ddepEvEn--");
		PRINT_VAR_MY(TICKS());

		OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc ddep: eventEndAlgo()");
		printf("Mpeg4enc ddep:  eventEndAlgo() enter\n");
	#endif


	#ifdef LOGS_ENABLED
		if(status == STATUS_JOB_COMPLETE)
		{
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: status = STATUS_JOB_COMPLETE");
			printf("Mpeg4enc ddep:  STATUS_JOB_COMPLETE\n");
		}
		else if(status == STATUS_JOB_ABORTED)
		{
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: status = STATUS_JOB_ABORTED");
			printf("Mpeg4enc ddep:  STATUS_JOB_ABORTED\n");
		}
		else if(status == STATUS_PAYLOAD_BUFFER_NEEDED)
		{
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: status = STATUS_PAYLOAD_BUFFER_NEEDED");
			printf("Mpeg4enc ddep:  STATUS_PAYLOAD_BUFFER_NEEDED\n");
		}
		else if(status == STATUS_BUFFER_NEEDED)
		{
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: status = STATUS_BUFFER_NEEDED");
			printf("Mpeg4enc ddep:  STATUS_BUFFER_NEEDED\n");
		}
		else
		{
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: status = STATUS_JOB_UNKNOWN");
			printf("Mpeg4enc ddep:  STATUS_JOB_UNKNOWN\n");
		}
	#endif

	if(!bufOut)
	{
		return;
	}

	if((status == STATUS_JOB_COMPLETE) && ((errors == VEC_ERT_NONE)||(errors == VEC_INFO_SKIP)))
	{
		#ifdef LOGS_ENABLED
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Normal endAlgo");
			printf("Mpeg4enc ddep:  Normal endAlgo\n");
		#endif

		set_flag_for_eow = 0;

		OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: Abt to call sva_EC_MP4_SetFrameParamOut (BRC) at line no %d",__LINE__);
		iBrc.sva_EC_MP4_SetFrameParamOut(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_parameters), &(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters));
		/* +Change start for ER 352933  video output delay in VT call due to encoder output delay */
		/* moved from here
		if(bufIn)
		{
			ReleaseBuffer(0, bufIn);
			bufIn = 0;

			#ifdef LOGS_ENABLED
				emptyThisBufferDoneCount++;
				OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return emptyThisBuffer %d", emptyThisBufferDoneCount);
				printf("Mpeg4enc ddep: return emptyThisBuffer %X\n", emptyThisBufferDoneCount);
			#endif
		}
		*/
		/* -Change start for ER 352933  video output delay in VT call due to encoder output delay */
		PRINT_VAR(partial_end_algo_count);
		OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:Value of partial_end_algo_count (0x%x) at line no %d",partial_end_algo_count,__LINE__);

		for(forcount=0; forcount<partial_end_algo_count; forcount++)
		{
			mp4e_get_next_not_required_bitstream_buffer(&bufOut);
		}

		partial_end_algo_count = 0;

		while(1)
		{
			mp4e_get_next_not_required_bitstream_buffer(&bufOut);
			if(bufOut==0)
			{
				//printf("break\n");
				break;
			}

			if(bufOut)
			{
				bitstream_size = SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size) +
								 SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);

				no_of_packets = ps_ddep_vec_mpeg4_param_desc_mpc->s_out_parameters.vp_num;

#ifndef __MPEG4ENC_ARM_NMF
				if (isFWDisabled) {
					bitstream_size = 0;
					no_of_packets = 0;
				}
#endif

				bitstream_size = bitstream_size - prev_bitstream_size;
				no_of_packets = no_of_packets - prev_no_of_packets;

				prev_bitstream_size = 0;
				prev_no_of_packets = 0;

				if(errors ==  VEC_ERT_NONE)
				{

					#ifndef __MPEG4ENC_ARM_NMF
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: eventEndAlgo: errors = VEC_ERT_NONE");
						printf("Ddep eventEndAlgo: errors = VEC_ERT_NONE\n");
					#endif

					if((bitstream_size % 16) == 8)
					{
					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFilledLen = SVP_SHAREDMEM_TO24(1+(bitstream_size)/8);	//New
					#else
						bufOut->byteInLastWord = 1;
						bufOut->filledLen = SVP_SHAREDMEM_TO24(1+(bitstream_size)/16);
					#endif

					}
					else
					{
					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFilledLen = SVP_SHAREDMEM_TO24((bitstream_size)/8);	//New
					#else
						bufOut->byteInLastWord = 2;
						bufOut->filledLen = SVP_SHAREDMEM_TO24((bitstream_size)/16);
					#endif

					}


					previous_skipped_flag = 0;

					PRINT_VAR(bufOut->filledLen);
				}
				else if(errors ==  VEC_INFO_SKIP)
				{
					#ifndef __MPEG4ENC_ARM_NMF
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: eventEndAlgo: errors = VEC_INFO_SKIP");
						printf("Ddep eventEndAlgo: errors = VEC_INFO_SKIP\n");
						//>PRINT_VAR();
					#endif
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0); //< send back empty buffer, filledLen is made 0
				#else
					bufOut->byteInLastWord = 2;
					bufOut->filledLen = SVP_SHAREDMEM_TO24(0); //< send back empty buffer, filledLen is made 0
				#endif

					previous_skipped_flag = 1;


				}

				/*//> CHECKME!!! temporary fix for sending a non-zero filledLen to proxy, while using a dummy algo component
			#ifdef __MPEG4ENC_ARM_NMF
				bufOut->nFilledLen = 0x100;
			#endif
				//> end of fix
				*/
#if 0
				PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);
				PRINT_VAR(bufOut->byteInLastWord);
				PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->filledLen));
				PRINT_VAR(SVP_SHAREDMEM_FROM24(bitstream_size));
				PRINT_VAR(no_of_packets);
#endif

				if(eosReached)
				{
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFlags |= BUFFERFLAG_EOS;
				#else
					bufOut->flags |= BUFFERFLAG_EOS;
				#endif

					// OMX_EventBufferFlag is now raised AFTER the output buffer is released. See below.
				}

				if(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.picture_coding_type == 0)
				{
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFlags |= BUFFERFLAG_SYNCFRAME;
				#else
					bufOut->flags |= BUFFERFLAG_SYNCFRAME;
				#endif

				}

				#ifdef __MPEG4ENC_ARM_NMF
					#if 1

					filledLen = bufOut->nFilledLen;

					if((ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 0) && (vos_written == WRITTEN_ONCE)) //when VOS header has been written again by Ddep
					{
						if(bufOut->nFilledLen != 0)	//only if the frame has not been skipped
						{
							//> printf("filledLenLastVOL=%d\n",filledLenLastVOL);
							vos_written = WRITTEN_TWICE;
							//change offset and nFilledLen
							bufOut->nOffset = filledLenLastVOL;
							bufOut->nFilledLen = bufOut->nFilledLen - filledLenLastVOL;
							//> printf("bufOut->nFilledLen=%d\n",bufOut->nFilledLen );

							filledLen = bufOut->nFilledLen;	//checkme, is this really required?
						}
					}

					//Writing the VOS_END_CODE for SP, only in case of ARM-NMF. For MPC-NMF, it is handled in proxy

					if(/*(pProxyComponent->mParam.m_vos_header.isSystemHeaderAddBeforeIntra == OMX_TRUE) && */(bufOut->nFlags & OMX_BUFFERFLAG_EOS))
					{
						if((ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 0))
						{
							// if 4 bytes space is available in current buffer then, insert vos end code, else don't
							if(filledLen <= (bufOut->nAllocLen - 4))
							{
								//>printf("bufOut writing end code\n");
								bufferptr = (t_uint8 *)(bufOut->pBuffer+bufOut->nOffset);
								//>printf("pBuffer=0x%x, nOffset=0x%x, sum=0x%x, filledLen=0x%x\n",bufOut->pBuffer, bufOut->nOffset, bufferptr, filledLen);
								bufferptr = bufferptr + filledLen;

								*bufferptr = 0x00; bufferptr++;	//SP end of stream code
								*bufferptr = 0x00; bufferptr++;
								*bufferptr = 0x01; bufferptr++;
								*bufferptr = 0xb1; bufferptr++;

								filledLen = filledLen + 4;
								bufOut->nFilledLen = filledLen;
							}
						}
					}

					#endif
				#endif



			#ifdef __MPEG4ENC_ARM_NMF
				bufOut->nFlags |= BUFFERFLAG_ENDOFRAME;
			#else
				bufOut->flags |= BUFFERFLAG_ENDOFRAME;
			#endif
				ReleaseBuffer(1, bufOut);

				if(eosReached)
				{
					eosReached = 0;

				#ifdef __MPEG4ENC_ARM_NMF
					iSendLogEvent.sendEventEOS(bufOut->nFlags);
				#else
					iSendLogEvent.sendEventEOS(bufOut->flags);
				#endif
				}

				bufOut =0;

				//The following check is a fix provided to ensure that if the user had set the pause state, the Component_sendCommand was deffered
				// until endAlgo was called for the currently encoding frame. See sendCommand function for the setting of pause_state_called
				if(pause_state_called)
				{
					pause_state_called = 0;

				#ifdef __MPEG4ENC_ARM_NMF
					Component::sendCommand(OMX_CommandStateSet, OMX_StatePause);
				#else
					Component_sendCommand(&mDataDep, OMX_CommandStateSet, OMX_StatePause);
				#endif
				}

				// This variable is also used for the above fix. It is set in processActual just before controlAlgo is called
				control_algo_called = 0;

				#ifdef LOGS_ENABLED
					fillThisBufferDoneCount++;
					OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return fillThisBuffer %d", fillThisBufferDoneCount);
					printf("Mpeg4enc ddep:  return fillThisBuffer %X\n", fillThisBufferDoneCount);
				#endif

				//>PRINT_VAR(frame_number);
			}
		}
		/* +Change start for ER 352933  video output delay in VT call due to encoder output delay */
		if(bufIn)
		{
			ReleaseBuffer(0, bufIn);
			bufIn = 0;
			#ifdef LOGS_ENABLED
				emptyThisBufferDoneCount++;
				OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return emptyThisBuffer %d", emptyThisBufferDoneCount);
				printf("Mpeg4enc ddep: return emptyThisBuffer %X\n", emptyThisBufferDoneCount);
			#endif
		}
		/* -Change start for ER 352933  video output delay in VT call due to encoder output delay */
		// Should we really released the HW on a buffer based
		release_resources();
		#ifdef __MPEG4ENC_ARM_NMF
		processEvent(); /* changes for ER 350963/ER349672 */
		#endif
	}
	else if((status == STATUS_PAYLOAD_BUFFER_NEEDED) && (errors == VEC_ERT_NONE))
	{
		partial_end_algo_count++;

		OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:Value of partial_end_algo_count (0x%x) at line no %d",partial_end_algo_count,__LINE__);
		PRINT_VAR(partial_end_algo_count);

		#ifdef LOGS_ENABLED
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Partial endAlgo\n");
			printf("Mpeg4enc ddep:  Partial endAlgo\n");
		#endif

		set_flag_for_eow = 1;

		PRINT_VAR(partial_end_algo_count);
		OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:Value of partial_end_algo_count (0x%x) at line no %d",partial_end_algo_count,__LINE__);

		for(forcount=0; forcount<partial_end_algo_count-1; forcount++)
		{
			mp4e_get_next_not_required_bitstream_buffer(&bufOut);
		}

		while(1)
		{
			mp4e_get_next_not_required_bitstream_buffer(&bufOut);

			PRINT_VAR(bufOut);
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:Value of bufOut (0x%x) at line no %d",(t_uint32)bufOut,__LINE__);

			if(bufOut)
			{
				bitstream_size = SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size) +
								 SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);

				no_of_packets = ps_ddep_vec_mpeg4_param_desc_mpc->s_out_parameters.vp_num;

				//<vp_pos[32] provides relative byte position of packets from start

				//< packet_bitstream_size = bitstream_size - prev_bitstream_size;

				//< PRINT_VAR(bitstream_size);
				//< PRINT_VAR(prev_bitstream_size);
				//< PRINT_VAR(packet_bitstream_size);

				no_of_packets = no_of_packets - prev_no_of_packets;
				prev_bitstream_size += bitstream_size;
				prev_no_of_packets += no_of_packets;

				if((bitstream_size % 16) == 8)
				{
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFilledLen = SVP_SHAREDMEM_TO24(1+(bitstream_size)/16);
				#else
					bufOut->byteInLastWord = 1;
					bufOut->filledLen = SVP_SHAREDMEM_TO24(1+(bitstream_size)/16);
				#endif
				}
				else
				{
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFilledLen = SVP_SHAREDMEM_TO24((bitstream_size)/16);
				#else
					bufOut->byteInLastWord = 2;
					bufOut->filledLen = SVP_SHAREDMEM_TO24((bitstream_size)/16);
				#endif
				}

#if 0
				PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);
				PRINT_VAR(bufOut->byteInLastWord);
				PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->filledLen));
				PRINT_VAR(bitstream_size);
				//< PRINT_VAR(packet_bitstream_size);
				PRINT_VAR(no_of_packets);
				PRINT_VAR(frame_number);
#endif
				ReleaseBuffer(1,bufOut);
				bufOut = 0;

				#ifdef LOGS_ENABLED
					fillThisBufferDoneCount++;
					OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return fillThisBuffer %d", fillThisBufferDoneCount);
					printf("Mpeg4enc ddep:  return fillThisBuffer %X\n", fillThisBufferDoneCount);
				#endif


			#ifdef __MPEG4ENC_ARM_NMF
				if(buffer_available_atoutput())
			#else
				if(buffer_available_atoutput(&mDataDep))
			#endif
				{
					set_flag_for_eow =0;

					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  already buffer present");
						printf("Mpeg4enc ddep:  already buffer present\n");
					#endif

					add_buffer_to_next_link();

					#ifdef __MPEG4ENC_ARM_NMF
						//nothing
					#else
						codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
					#endif

					update_algo_called = 1;
				}
				else
				{
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  new buffer not found");
						printf("Mpeg4enc ddep:  new buffer not found\n");
					#endif
				}
			}

			if(update_algo_called)
				break;
		}

		//handling send command in case of EOW
		if(handle_send_command_eow)
		{
			handle_send_command_eow=0;
			if(set_flag_for_eow && get_hw_resource)
			{
			#ifdef __MPEG4ENC_ARM_NMF
				bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
				bufOut->nTimeStamp = bufIn->nTimeStamp;
			#else
				bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
				bufOut->nTimeStamph = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStamph));
				bufOut->nTimeStampl = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStampl));
			#endif



				mp4e_fill_link_structure(bufOut);//setbufferlink for bitstream buffer
				mp4e_mark_end_link_structure();


				//>PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->address));
				ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16(s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
			#ifdef __MPEG4ENC_ARM_NMF
				ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)bufOut->pBuffer;
			#else
				ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->address));
			#endif

				ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.bitstream_offset = SVP_SHAREDMEM_TO16(0);

			#ifdef __MPEG4ENC_ARM_NMF
				//nothing
			#else
				codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
			#endif
				set_flag_for_eow=0;
			}
		}
		//< release_resources();	// dont'release resource after packet
	}


	#ifdef LOGS_ENABLED
		hcl_printf("ddepEvEx--");
		PRINT_VAR_MY(TICKS());
		OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: eventEndAlgo()");
		printf("Mpeg4enc ddep:  eventEndAlgo() exit\n");
	#endif

	return;
}

void print_addresses_params()
{
	int i;
#if 0
	PRINT_VAR(param.addr_in_frame_buffer     );
	PRINT_VAR(param.addr_out_frame_buffer    );
	PRINT_VAR(param.addr_internal_buffer     );
	PRINT_VAR(param.addr_header_buffer       );
	//< PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size        );
	PRINT_VAR(param.addr_in_bitstream_buffer );
	PRINT_VAR(param.addr_out_bitstream_buffer);
	PRINT_VAR(param.addr_in_parameters       );
	PRINT_VAR(param.addr_out_parameters      );
	PRINT_VAR(param.addr_in_frame_parameters );
	PRINT_VAR(param.addr_out_frame_parameters);
#endif

#if 0
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_source_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_fwd_ref_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_grab_ref_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_intra_refresh_buffer));
	#ifdef __svp8500_v1__
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_ime_mv_field_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_ime_mv_field_buffer_end));
	#endif

	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_dest_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_deblocking_param_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_motion_vector_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_intra_refresh_buffer));

	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_search_window_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_search_window_end));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_jpeg_run_level_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_h264e_H4D_buffer));
	#ifdef __svp8500_v1__
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_h264e_rec_local));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_h264e_metrics));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_internal_buffer.addr_h264e_cup_context));
	#endif

	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.addr_header_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_header_buf.header_size));
#endif

#if 1
	//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: Inout_params\n");
	printf("Mpeg4enc ddep: Inout_params\n");
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.I_Qp);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.P_Qp);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.last_I_Size);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_SUM);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_count);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_mod);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_seconds_old);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_modulo_old);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.gov_flag);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.avgSAD);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.seqSAD);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.min_pict_quality)
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.diff_min_quality);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.TotSkip);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Skip_Current);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Cprev);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPprev);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.PictQpSum);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.S_overhead);
	for(i=0; i<6;i++)
	{
		PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_vector[i]);
	}
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness_fake_TS);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_saved);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.intra_Qp_flag);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion_fake_TS);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.old_P_Qp_vbr);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount_prev);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.mv_field_index);
	//>PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.PictQpSumIntra);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.hec_count);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevAct);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevEst);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB_flag);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Lprev);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbErr);
#endif
#if 1
	//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: In_params\n");
	printf("Mpeg4enc ddep: In_params\n");
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.picture_coding_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset);
#endif
#if 1
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_header_freq);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_frame_id);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.data_partitioned);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.reversible_vlc);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.hec_freq);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.modulo_time_base);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_max);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_bit_size);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_mb_size);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_me);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.me_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_fcode_forward);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.rounding_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.intra_refresh_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_mb_num);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.cir_period_max);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant);
#endif
#if 1
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_type);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_frame_target);
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_min_pred));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_max_pred));
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.skip_count);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.bit_rate);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.framerate);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_modulo);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_seconds);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_thr);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.delta_target);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.minQp);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.maxQp);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment_resolution);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.fixed_vop_time_increment);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.Smax);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_base_quality);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_framerate);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.max_buff_level);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.first_I_skipped_flag);
	PRINT_VAR((t_sint16)ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_ts_modulo_old);
#endif
#if 1
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[0]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[1]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[2]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[3]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[4]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[5]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[6]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[7]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[0]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[1]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[2]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[3]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[4]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[5]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[6]);
	PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[7]);
#endif

#ifndef __MPEG4ENC_ARM_NMF
#if 0 //OST Trace calls
	OstTraceFiltInst0(TRACE_DEBUG, "Mpeg4enc ddep: Printing Input/Inout Params");
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.I_Qp = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.I_Qp);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.P_Qp = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.P_Qp);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Skip_Current = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Skip_Current);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.picture_coding_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.picture_coding_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header);
#endif
#if 1
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height);
#endif
#if 0
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.data_partitioned = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.data_partitioned);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.modulo_time_base = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.modulo_time_base);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.skip_count = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.skip_count);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.bit_rate = 0x%x", SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.bit_rate));
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.framerate = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.framerate);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment_resolution = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_time_increment_resolution);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.fixed_vop_time_increment = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.fixed_vop_time_increment);

	for(i=0; i<6;i++)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_vector[i] -0x%x",ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_vector[i]);
	}

	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.last_I_Size = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.last_I_Size);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_SUM = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_SUM);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_count = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.comp_count);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_mod = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_mod);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_seconds_old = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_seconds_old);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_modulo_old = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.ts_modulo_old);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.gov_flag = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.gov_flag);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.avgSAD = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.avgSAD);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.seqSAD = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.seqSAD);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.min_pict_quality = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.min_pict_quality);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.diff_min_quality = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.diff_min_quality);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.TotSkip = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.TotSkip);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Cprev = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Cprev);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPprev = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPprev);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.PictQpSum = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.PictQpSum);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.S_overhead = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.S_overhead);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness_fake_TS = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_fullness_fake_TS);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_saved = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.buffer_saved);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.intra_Qp_flag = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.intra_Qp_flag);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion_fake_TS = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BUFFER_depletion_fake_TS);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.old_P_Qp_vbr = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.old_P_Qp_vbr);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount_prev = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.pictCount_prev);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.mv_field_index = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.mv_field_index);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.hec_count = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.hec_count);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevAct = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevAct);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevEst = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbPrevEst);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB_flag = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.CodedMB_flag);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Lprev = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.Lprev);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbErr = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.BPPmbErr);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_header_freq = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_header_freq);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_frame_id = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.gob_frame_id);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.reversible_vlc = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.reversible_vlc);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.hec_freq = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.hec_freq);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_max = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_size_max);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_bit_size = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_bit_size);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_mb_size = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vp_mb_size);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_me = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_me);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.me_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.me_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_fcode_forward = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.vop_fcode_forward);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.rounding_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.rounding_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.intra_refresh_type = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.intra_refresh_type);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_mb_num = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_mb_num);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.cir_period_max = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.cir_period_max);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_frame_target = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_frame_target);
	OstTraceFiltInst1(TRACE_DEBUG, "SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_min_pred) = 0x%x", SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_min_pred));
	OstTraceFiltInst1(TRACE_DEBUG, "SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_max_pred) = 0x%x", SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_target_max_pred));
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_modulo = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_modulo);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_seconds = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.ts_seconds);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_thr = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.air_thr);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.delta_target = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.delta_target);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.minQp = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.minQp);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.maxQp = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.maxQp);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.Smax = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.Smax);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_base_quality = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_base_quality);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_framerate = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.min_framerate);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.max_buff_level = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.max_buff_level);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.first_I_skipped_flag = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.first_I_skipped_flag);
	OstTraceFiltInst1(TRACE_DEBUG, "(t_sint16)ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_ts_modulo_old = 0x%x", (t_sint16)ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_ts_modulo_old);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[0] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[0]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[1] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[1]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[2] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[2]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[3] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[3]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[4] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[4]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[5] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[5]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[6] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[6]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[7] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_first_mb[7]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[0] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[0]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[1] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[1]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[2] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[2]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[3] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[3]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[4] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[4]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[5] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[5]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[6] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[6]);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[7] = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.slice_loss_mb_num[7]);
	//Printing Custom Parameters
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.profile = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.profile);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.level = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.level);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.no_of_p_frames = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.no_of_p_frames);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.buffering_model = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.buffering_model);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.swis_buffer = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.swis_buffer);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.vbv_occupancy = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.vbv_occupancy);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.vbv_buffer_size = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.vbv_buffer_size);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.color_primary = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.color_primary);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.par_horizontal = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.par_horizontal);
	OstTraceFiltInst1(TRACE_DEBUG, "ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.par_vertical = 0x%x", ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.par_vertical);
#endif
#endif
}

// Call in case of State transition from Executing to Idle
#ifdef __MPEG4ENC_ARM_NMF
STATIC_FLAG void COMP(reset)()
#else
static void reset(struct Component *this)
#endif
{
	#ifdef LOGS_ENABLED
		//>OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  reset\n");
		printf("Mpeg4enc ddep:  reset\n");
	#endif

	release_resources();
}

STATIC_FLAG void COMP(disablePortIndication)(t_uint32 portIdx)
{
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  disablePortIndication\n");
		printf("Mpeg4enc ddep:  disablePortIndication\n");
	#endif
}

STATIC_FLAG void COMP(enablePortIndication)(t_uint32 portIdx)
{
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  enablePortIndication\n");
		printf("Mpeg4enc ddep:  enablePortIndication\n");
	#endif
}

STATIC_FLAG void COMP(flushPortIndication)(t_uint32 portIdx)
{
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  flushPortIndication\n");
	#endif
}

STATIC_FLAG void COMP(stateChangeIndication)(OMX_STATETYPE oldState, OMX_STATETYPE newState)
{
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  stateChangeIndication\n");
	#endif
}


#ifdef __MPEG4ENC_ARM_NMF
void mpeg4enc_arm_nmf_parser::process()
#else
static void process(Component *this)
#endif
{
	#ifdef LOGS_ENABLED
		hcl_printf("ddepPro--");
		PRINT_VAR_MY(TICKS());
		OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc ddep: process()");
		printf("Mpeg4enc ddep:  process() enter\n");
	#endif

	if(!set_flag_for_eow)
	{
		if(get_hw_resource)
		{
			// resource already aquired and in use

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: process() (get_hw_resource)");
		printf("Mpeg4enc ddep:  process() exit (get_hw_resource)\n");
	#endif

			return;
		}

		if(get_hw_resource_requested)
		{
			// resource already requested and in use

			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: process() (get_hw_resource_requested)");
				printf("Mpeg4enc ddep:  process() exit (get_hw_resource_requested)\n");
			#endif

			return;
		}

		// Check buffers and Parameter availabilities
	#ifdef __MPEG4ENC_ARM_NMF
		if(get_codec_parameter && buffer_available_atinput() && buffer_available_atoutput())
	#else
		if(get_codec_parameter && buffer_available_atinput(&mDataDep) && buffer_available_atoutput(&mDataDep))
	#endif
		{
			#ifdef LOGS_ENABLED
				hcl_printf("ddepAlR--");
				PRINT_VAR_MY(TICKS());
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  iResource.allocResource() enter\n");
				printf("Mpeg4enc ddep:  iResource.allocResource() enter\n");
			#endif

			download_parameters();

			#ifdef __MPEG4ENC_ARM_NMF
				#ifdef ENABLE_ARMNMF_LOGS
						OstTraceFiltInst0(TRACE_FLOW, "About to call processActual\n");
						printf("About to call processActual\n");
				#endif
				    /* + changes for ER 350963/ER349672 */
					get_hw_resource=1;
					/* - changes for ER 350963/ER349672 */
					processActual();
			#else
					// Call to RM to get the HW resource
					get_hw_resource_requested = 1;
					iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  iResource.allocResource() exit\n");
						printf("Mpeg4enc ddep:  iResource.allocResource() exit\n");
					#endif
			#endif
		}

	}

	if(set_flag_for_eow)
	{

	#ifdef __MPEG4ENC_ARM_NMF
		if(buffer_available_atoutput())
	#else
		if(buffer_available_atoutput(&mDataDep))
	#endif
		{
			if(get_hw_resource)			//< redundant check ?
				set_flag_for_eow=0;

			add_buffer_to_next_link();

			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  updateAlgo() exit\n");
				printf("Mpeg4enc ddep:  updateAlgo() exit\n");
			#endif

			#ifdef __MPEG4ENC_ARM_NMF
				//nothing
			#else
				codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
			#endif

		}
	}

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: process()");
		printf("Mpeg4enc ddep:  process() exit\n");
	#endif

}

//To take care of changed TimeStamp value, now based on OMX_Ticks
t_sva_timestamp_value COMP(HclTicksFromMicroSeconds)( t_sva_timestamp_value pts )
{
	t_uint48 nHclTickFreqBy10K 				= 9;	//90*1000 actual
	t_uint48 nMicroSecondsPerSecondBy10K	= 100;	//1*1000*1000 actual
	t_uint48 tsVal;

	//tsVal = (((t_uint56)timeStampH)<<48) + ((t_uint56)timeStampL);

	tsVal = (t_uint48)(pts);
	tsVal *= nHclTickFreqBy10K;
	//pts = tsVal;
	pts = tsVal/nMicroSecondsPerSecondBy10K;

#if 0	//round after 50 only
	if( (tsVal % nMicroSecondsPerSecondBy10K) >= (nMicroSecondsPerSecondBy10K/2) ) {
		pts++;
	}
#else	//round if non-zero fraction
	if( (tsVal % nMicroSecondsPerSecondBy10K) > 0 ) {
		//> PRINT_VAR(tsVal % nMicroSecondsPerSecondBy10K);
		pts++;
	}
#endif

	return pts;
}


#ifdef __MPEG4ENC_ARM_NMF
void mpeg4enc_arm_nmf_parser::processActual()
#else
static void processActual(Component *this)
#endif
{

	t_sva_timestamp_value pts = 0;

	t_uint32 buf_level, last_bitstream_size;

	if(!bufOut)
	{
		return;
	}


	#ifdef LOGS_ENABLED
		hcl_printf("ddepPAc--");
		PRINT_VAR_MY(TICKS());

		OstTraceFiltInst0(TRACE_API, "=> Mpeg4enc ddep: processActual()");
		printf("Mpeg4enc ddep:  processActual() enter\n");
	#endif

	{



		#ifdef __MPEG4ENC_ARM_NMF
			bufOut->nTimeStamp = bufIn->nTimeStamp;
			pts  = bufIn->nTimeStamp;
			#ifdef ENABLE_ARMNMF_LOGS
				PRINT_VAR(pts);
			#endif
		#else
			bufOut->nTimeStamph = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStamph));
			bufOut->nTimeStampl = SVP_SHAREDMEM_TO24(SVP_SHAREDMEM_FROM24(bufIn->nTimeStampl));
			pts  = SVP_SHAREDMEM_FROM24(bufIn->nTimeStamph) << 16 | SVP_SHAREDMEM_FROM24(bufIn->nTimeStampl);
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: pts=0x%x at line no %d", pts, __LINE__);
			PRINT_VAR(pts);
		#endif

		pts = HclTicksFromMicroSeconds(pts);

		#ifdef LOGS_ENABLED
			PRINT_VAR(pts);
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: pts=0x%x after conversion, at line no %d", pts, __LINE__);
		#endif

		if(once)
		{
			once = !once;
			prev_pts = pts;
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16(s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
		#ifdef __MPEG4ENC_ARM_NMF
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)bufOut->pBuffer;
		#else
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->address));
		#endif
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.bitstream_offset = SVP_SHAREDMEM_TO16(0);
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.I_Qp = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant;
			ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.P_Qp = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant;
		}

		// In case of short header, there is no requirment to have a separate first buffer having header information
		// so the previous logic used to return first buffer with filledLen = 0, for SH. This caused a problem when
		// mpeg4enc was used with MUXER (audio+video). It req'd the first buffer to have header info (w/ or w/o data)
		// So now, frame_number is set to 1 in case of SH.
		if( frame_number == 0 && ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 1 )
		{
			frame_number = 1;
		}

		// release one bitstream buffer empty, it will be filled with vos+vo+vol on arm side
		// image buffer is not released along normally
		// such vos + vo + vol in separate buffer is a requested requirement.
		if(frame_number == 0)
		{
		#ifdef __MPEG4ENC_ARM_NMF
			bufOut->nFilledLen  = SVP_SHAREDMEM_TO24(0);
		#else
			bufOut->filledLen  = SVP_SHAREDMEM_TO24(0);
			bufOut->byteInLastWord = 2;
		#endif


#if 0
			PRINT_VAR(bufIn);
			PRINT_VAR(bufOut);
			PRINT_VAR(SVP_SHAREDMEM_FROM24(bufIn->filledLen));
			PRINT_VAR(bufIn->flags);
#endif
			// if image buffer is flagged witn eos and fillLen =0, that means no more image buffer
			// are going to be pushed and there is nothing to encode. release both bitstream and image buffers
			// but stream buffer will be filled with vos + vo + vol header and vos end code on arm side
	#ifdef __MPEG4ENC_ARM_NMF
			if((bufIn->nFlags & BUFFERFLAG_EOS) && (bufIn->nFilledLen == 0))
	#else
			if((bufIn->flags & BUFFERFLAG_EOS) && (bufIn->filledLen == 0))
	#endif
			{
				#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  first bufIn nFillLen = 0 and EOS flag set\n");
						printf("Mpeg4enc ddep:  first bufIn nFillLen = 0 and EOS flag set\n");
				#endif

				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFlags |= BUFFERFLAG_EOS;
				#else
					bufOut->flags |= BUFFERFLAG_EOS;
				#endif

				ReleaseBuffer(0,bufIn);
				bufIn = 0;

				#ifdef LOGS_ENABLED
					emptyThisBufferDoneCount++;
					OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return emptyThisBuffer %d", emptyThisBufferDoneCount);
					printf("Mpeg4enc ddep: return emptyThisBuffer %X\n", emptyThisBufferDoneCount);
				#endif
			}


			#ifdef __MPEG4ENC_ARM_NMF
				#if 1

				filledLen = bufOut->nFilledLen;

				if( (ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 0) && (vos_written == NOT_WRITTEN) )
				{
					vos_written = WRITTEN_ONCE;
					// Writing header for the first frame, only in case of arm-nmf
					OMX_U32 filledLen_bits;

					bufferptr = (t_uint8 *)(bufOut->pBuffer + bufOut->nOffset);
					filledLen_bits = Write_VOS_VO_VOL(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters), &(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters), (t_uint16*)bufferptr);

					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width 	= ConvertMult16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width);
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height 	= ConvertMult16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height);
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height = ConvertMult16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_height);
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width 	= ConvertMult16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_width);

					bufOut->nFilledLen = filledLen_bits/8;
					filledLenLastVOL = bufOut->nFilledLen;
					bufOut->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;	//OMX 1.1.2 only, first buffer only has config data.
				}

				//Writing the VOS_END_CODE for SP, only in case of ARM-NMF. For MPC-NMF, it is handled in proxy

				if(/*(pProxyComponent->mParam.m_vos_header.isSystemHeaderAddBeforeIntra == OMX_TRUE) && */(bufOut->nFlags & OMX_BUFFERFLAG_EOS))
				{
					if((ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 0))
					{
						// if 4 bytes space is available in current buffer then, insert vos end code, else don't
						if(filledLen <= (bufOut->nAllocLen - 4))
						{
							bufferptr = (t_uint8 *)(bufOut->pBuffer+bufOut->nOffset);
							bufferptr = bufferptr + filledLen;

							*bufferptr = 0x00; bufferptr++;	//SP end of stream code
							*bufferptr = 0x00; bufferptr++;
							*bufferptr = 0x01; bufferptr++;
							*bufferptr = 0xb1; bufferptr++;

							filledLen = filledLen + 4;
							bufOut->nFilledLen = filledLen;
						}
					}
				}

				#endif
			#endif


			ReleaseBuffer(1, bufOut);
			bufOut = 0;

			#ifdef LOGS_ENABLED
				fillThisBufferDoneCount++;
				OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return fillThisBuffer %d", fillThisBufferDoneCount);
				printf("Mpeg4enc ddep:  return fillThisBuffer %X\n", fillThisBufferDoneCount);
			#endif

			release_resources();
			frame_number++ ; //one frame programmed, increment frame_num count
		}
		else
		{	// frame_number = 1 onwards actual encoding starts

		#ifdef __MPEG4ENC_ARM_NMF
			if(bufIn->nFilledLen != 0)
		#else
			if(SVP_SHAREDMEM_FROM24(bufIn->filledLen) != 0)
		#endif
			{
				// encode the current frame only if the current timestamp is gte previous "valid" timestamp
				// if the condition is satisfied, current timestamp is made the valid previous timestamp
				// for next frame
				// i.e. if user pushes buffer whose timestamps are lets say 10, 20, 30, 40, 10, 20, 30, 40, 50
				// of these 9 frames, first 4 frames will be encoded as they have timestamp in non-decreasing order
				// but 5th frame is not encoded as its timestamp is 10 which is less than prev_timestamp which is 40.
				// also here prev_timestamp is not updated to 10 and still remains at 40. Hence the 6th frame whose
				// timestamp is 20 is also not encoded. This continues till the 9th frame where the timestamp
				// more than the valid prev_timestamp 40.
				// Interesting thing to note here is that inspite of the increasing timstamps from 5th frame onwards
				// they are not encoded till 9th as they have timestamps less than 40.
				// This situation can arise when the encoder is tunneled with camera and camera adjusts the timestamps midway
				// after emitting few frames.
				// If such checks are npt implemented the ptsDiff(timestamp difference ) becomes negative in BRC module,
				// which gets wrapped around and gets converted to a large positive number and takes large time to return
				// user gets a feeling that encoder has hung, as it has stuck in large while loop
				// Refer VI 26599
				/* +Change end for ER 350388 SVA panic during switching camera */
				//If the difference between previous time stamp and the present time stamp exceeds the threshold value, the present
				//frame shall be skipped so as to handle frames with abrupt changes in time stamps.
				if((pts >= prev_pts) && ((pts - prev_pts) < (90000*80)))
				{
				/* -Change end for ER 350388 SVA panic during switching camera */
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  resource available, process buffers");
						printf("Mpeg4enc ddep:  resource available, process buffers\n");
					#endif

				#ifdef __MPEG4ENC_ARM_NMF
					if(bufIn->nFlags & BUFFERFLAG_EOS)
				#else
					if(bufIn->flags & BUFFERFLAG_EOS)
				#endif
					{
						#ifdef LOGS_ENABLED
							OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Case1, FillLen !=0 and EOS flag set");
							printf("Mpeg4enc ddep:  Case1, FillLen !=0 and EOS flag set \n");
						#endif

						eosReached = 1;
					}
					else
					{
						#ifdef LOGS_ENABLED
							OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Case1, FillLen !=0 and EOS flag not set");
							printf("Mpeg4enc ddep:  Case1, FillLen !=0 and EOS flag not set \n");
						#endif
					}

#if 0
					PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->address));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->address) + 2*SVP_SHAREDMEM_FROM24(bufOut->allocLen));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(bufIn->address));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(bufIn->address)+ 2*SVP_SHAREDMEM_FROM24(bufIn->allocLen));

					PRINT_VAR(s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
#endif

					//create bufferlinklist
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
				#else
					bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
				#endif
					mp4e_fill_link_structure(bufOut);//setbufferlink for bitstream buffer
					mp4e_mark_end_link_structure();

					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16(s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
				#ifdef __MPEG4ENC_ARM_NMF
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)bufOut->pBuffer;
				#else
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->address));
				#endif
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_bitstream_buffer.bitstream_offset = SVP_SHAREDMEM_TO16(0);

					if(frame_number != 1)
					{
						//program next frame
						ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.init_me          = 0x0;	//continue mot_est using prev subtask data (used from validation code)

						//< destination buffer (reconstructed image of the current encoded image) is
						//< fwd reference buffer for the next frame. Hence swap the pointers after every
						//< iteration
						//< swap dest buffer addr and fwd ref buffer addr.
						/* +Change for enabling enabling skipping as prediction should not be made from skipped frames so swapping is prevented if previous frame was skipped(done for ER - 354700)*/
						if((!new_config_received || (new_config_received && (frame_number%2 == 0)))&&(!previous_skipped_flag))
						/* -Change for enabling enabling skipping as prediction should not be made from skipped frames so swapping is prevented if previous frame was skipped(done for ER - 354700)*/
						{
							t_ahb_address tmpbuffer =0;

							tmpbuffer = ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_dest_buffer;
							ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_dest_buffer = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_fwd_ref_buffer;
							ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_fwd_ref_buffer = tmpbuffer;

							tmpbuffer = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_intra_refresh_buffer;
							ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_intra_refresh_buffer = ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_intra_refresh_buffer;
							ps_ddep_vec_mpeg4_param_desc_mpc->s_out_fram_buffer.addr_intra_refresh_buffer = tmpbuffer;

							new_config_received = 0;
						}
					}

					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: frame_number = 0x%x at line no %d", frame_number, __LINE__);
					PRINT_VAR(frame_number);


				#ifdef __MPEG4ENC_ARM_NMF
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_source_buffer = (t_ahb_address)bufIn->pBuffer;
				#else
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn->address));
				#endif



					// Setting the cropping parameters for video stabilization - currently supported only by MPC codec
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset 	= mStabCropVectorX[stab_crop_array_start];
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset 	= mStabCropVectorY[stab_crop_array_start];

					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width 	= mStabCropWindowWidth[stab_crop_array_start];
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height 	= mStabCropWindowHeight[stab_crop_array_start];

					stab_crop_array_start++;
					if(stab_crop_array_start == STAB_ARRAY_SIZE) {
							stab_crop_array_start = 0;
					}

					PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of (ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset) (0x%x) at line no %d \n",(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_horizontal_offset),__LINE__);
					PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of (ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset) (0x%x) at line no %d \n",(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.window_vertical_offset),__LINE__);
					PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of (ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width) (0x%x) at line no %d \n",(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_width),__LINE__);
					PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  Value of (ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height) (0x%x) at line no %d \n",(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.frame_height),__LINE__);


/* +Change start for ER 348538 I-frame request is not applied to correct frame */
	#ifdef __MPEG4ENC_ARM_NMF
					if(bufIn->nFlags & 0x100)
					{
						bufIn->nFlags &= ~0x100;
	#else
					if(bufIn->flags & 0x100)
					{
						bufIn->flags &= ~0x100;
	#endif
						ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra = 1;
					}
/* -Change end for ER 348538 I-frame request is not applied to correct frame */
					if(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 1)
					{
						//PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra);
						OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Abt to call SH_GetNextFrameParamIn (BRC) for brc_type (0x%x) at line no %d \n",ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_type,__LINE__);

						iBrc.SH_GetNextFrameParamIn(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters),
													&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters),
                                                    &(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters),
													pts,
													frame_number);

						ps_ddep_vec_mpeg4_param_desc_host_set->s_in_custom_parameters.force_intra = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra;

						//PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra);
						SH_headerCreation(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters), &(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters), &current_header, &previous_header, pts);
					}
					else
					{
						//PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra);

						#ifdef LOGS_ENABLED
							hcl_printf("ddepGNP--");
							PRINT_VAR_MY(TICKS());
						#endif

						last_bitstream_size = SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.bitstream_size) + SVP_SHAREDMEM_FROM16(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.stuffing_bits);
						OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Abt to call SP_GetNextFrameParamIn (BRC) for brc_type (0x%x) at line no %d \n",ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.brc_type,__LINE__);

						iBrc.SP_GetNextFrameParamIn(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters),
													&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters),
													&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters),
													pts,
													frame_number, &buf_level, last_bitstream_size);
						ps_ddep_vec_mpeg4_param_desc_host_set->s_in_custom_parameters.force_intra = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra;

						//> PRINT_VAR(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters.force_intra);
						SP_headerCreation(&(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters), &(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_custom_parameters), &current_header, &previous_header, pts, buf_level);
					}

					previous_header = current_header;


#if (defined(__ndk8500_a0__) || defined(__svp8500_v1__))	//< select CDME algo for v1, rest all values obsolete
					ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.me_type          = 0x0;
#endif

					if(ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.picture_coding_type == 0)
						ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.I_Qp;
					else
						ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.quant = ps_ddep_vec_mpeg4_param_desc_mpc->s_in_out_frame_parameters.P_Qp;


					//>#ifdef LOGS_ENABLED
						print_addresses_params();
					//>#endif


				control_algo_called = 1;
					#ifdef LOGS_ENABLED
						printf("Mpeg4enc ddep:  codec_algo.configureAlgo\n");
					#endif
					OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: Calling codec_algo.configureAlgo");

				#ifdef __MPEG4ENC_ARM_NMF
					iAlgo.configureAlgo(param.addr_in_frame_buffer,
										param.addr_out_frame_buffer,
										param.addr_internal_buffer,
										param.addr_header_buffer,
										param.addr_in_bitstream_buffer,
										param.addr_out_bitstream_buffer,
										param.addr_in_parameters,
										param.addr_out_parameters,
										param.addr_in_frame_parameters,
										param.addr_out_frame_parameters);

					// Start  also synchronous
					#ifdef ENABLE_ARMNMF_LOGS
						OstTraceFiltInst0(TRACE_FLOW, "\n codec_algo.controlAlgo entering");
						printf("\n codec_algo.controlAlgo entering");
					#endif

					iAlgo.controlAlgo(CMD_START, 0, 1);
				#else
					// Synchronous call
					codec_algo.configureAlgo(&iEndAlgo,
											 convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
											 convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
											 convto16bitmode(param.addr_internal_buffer|EXT_BIT),
											 convto16bitmode(param.addr_header_buffer|EXT_BIT),
											 convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
											 convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
											 convto16bitmode(param.addr_in_parameters|EXT_BIT),
											 convto16bitmode(param.addr_out_parameters|EXT_BIT),
											 convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
											 convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));

					// Start  also synchronous
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  codec_algo.controlAlgo\n");
						printf("Mpeg4enc ddep:  codec_algo.controlAlgo\n");
						hcl_printf("ddepCoA--");
						STORE_CONTROL_ALGO_TICKS(ps_ddep_vec_mpeg4_param_desc_mpc/*, _from_ticks*/);
						PRINT_VAR_MY(TICKS());
					#endif

					codec_algo.controlAlgo(CMD_START, /*s_channelId*/0, 1);	//< channelId is not used set as 0

				#endif

					frame_number++ ; //one frame programmed, increment frame_num count
					prev_pts = pts;
				}
				else	//< if(pts >= prev_pts)
				{
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Case1, FillLen !=0 and incorrect timestamp\n");
						printf("Mpeg4enc ddep:  Case1, FillLen !=0 and incorrect timestamp\n");
					#endif
					/* +Change end for ER 350388 SVA panic during switching camera */

					#ifdef __MPEG4ENC_ARM_NMF
					if(bufIn->nFlags & BUFFERFLAG_EOS)
					#else
					if(bufIn->flags & BUFFERFLAG_EOS)
					#endif
					{

					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFlags |= BUFFERFLAG_EOS;
					#else
						bufOut->flags |= BUFFERFLAG_EOS;
					#endif

						//> OMX_EventBufferFlag is now raised AFTER we release the output buffer. See below.
					}
					/* -Change end for ER 350388 SVA panic during switching camera */

					if(bufOut !=0)
					{
					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
					#else
						bufOut->byteInLastWord = 2;
						bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
					#endif

					}

					PRINT_VAR(bufIn);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of bufIn (0x%x) at line no %d \n",(t_uint32)bufIn,__LINE__);
					PRINT_VAR(bufOut);
					OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of bufOut (0x%x) at line no %d \n",(t_uint32)bufOut,__LINE__);
					//>PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->filledLen));
					//>PRINT_VAR(bufOut->flags);

					ReleaseBuffer(0,bufIn);
					ReleaseBuffer(1,bufOut);

					bufIn = 0;
					bufOut = 0;

					#ifdef LOGS_ENABLED
						emptyThisBufferDoneCount++;
						fillThisBufferDoneCount++;
						OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return emptyThisBuffer %d", emptyThisBufferDoneCount);
						OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return fillThisBuffer %d", fillThisBufferDoneCount);
						printf("Mpeg4enc ddep:  return emptyThisBuffer %X\n", emptyThisBufferDoneCount);
						printf("Mpeg4enc ddep:  return fillThisBuffer %X\n",fillThisBufferDoneCount);
					#endif

					release_resources();

				} //< if(pts >= prev_pts)
			}	//< if(SVP_SHAREDMEM_FROM24(bufIn->filledLen) != 0)
		#ifdef __MPEG4ENC_ARM_NMF
			else if(SVP_SHAREDMEM_FROM24(bufIn->nFilledLen) == 0)
		#else
			else if(SVP_SHAREDMEM_FROM24(bufIn->filledLen) == 0)
		#endif
			{

			#ifdef __MPEG4ENC_ARM_NMF
				if(bufIn->nFlags & BUFFERFLAG_EOS)
			#else
				if(bufIn->flags & BUFFERFLAG_EOS)
			#endif
				{
					#ifdef LOGS_ENABLED
						OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Case2, FillLen =0(dummy buffer) and EOS flag set\n");
						printf("Mpeg4enc ddep:  Case2, FillLen =0(dummy buffer) and EOS flag set\n");
					#endif
				}
				else
				{
					#ifdef LOGS_ENABLED
							OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Case2, FillLen =0 and EOS not set \n");
					printf("Mpeg4enc ddep:  Case2, FillLen =0 and EOS not set \n");
					#endif
				}

				if(bufOut !=0)
				{
				#ifdef __MPEG4ENC_ARM_NMF
					bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
				#else
					bufOut->byteInLastWord = 2;
					bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
				#endif

				#ifdef __MPEG4ENC_ARM_NMF
					if(bufIn->nFlags & BUFFERFLAG_EOS)
				#else
					if(bufIn->flags & BUFFERFLAG_EOS)
				#endif
					{

					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFlags |= BUFFERFLAG_EOS;
					#else
						bufOut->flags |= BUFFERFLAG_EOS;
					#endif

						//> OMX_EventBufferFlag is now raised AFTER we release the output buffer. See below.
					}
				}



			#ifdef __MPEG4ENC_ARM_NMF
				#if 1

				filledLen = bufOut->nFilledLen;

				//Writing the VOS_END_CODE for SP, only in case of ARM-NMF. For MPC-NMF, it is handled in proxy

				if(/*(pProxyComponent->mParam.m_vos_header.isSystemHeaderAddBeforeIntra == OMX_TRUE) && */(bufOut->nFlags & OMX_BUFFERFLAG_EOS))
				{
					if((ps_ddep_vec_mpeg4_param_desc_mpc->s_in_parameters.flag_short_header == 0))
					{
						// if 4 bytes space is available in current buffer then, insert vos end code, else don't
						if(filledLen <= (bufOut->nAllocLen - 4))
						{
							bufferptr = (t_uint8 *)(bufOut->pBuffer+bufOut->nOffset);
							bufferptr = bufferptr + filledLen;

							*bufferptr = 0x00; bufferptr++;	//SP end of stream code
							*bufferptr = 0x00; bufferptr++;
							*bufferptr = 0x01; bufferptr++;
							*bufferptr = 0xb1; bufferptr++;

							filledLen = filledLen + 4;
							bufOut->nFilledLen = filledLen;
						}
					}
				}
				#endif
			#endif

				PRINT_VAR(bufIn);
				OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of bufIn (0x%x) at line no %d \n",(t_uint32)bufIn,__LINE__);
				PRINT_VAR(bufOut);
				OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of bufOut (0x%x) at line no %d \n",(t_uint32)bufOut,__LINE__);
				//>PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut->filledLen));
				//>PRINT_VAR(bufOut->flags);

			#ifdef __MPEG4ENC_ARM_NMF
				bufIn->nFilledLen = SVP_SHAREDMEM_TO24(0);
			#else
				bufIn->filledLen = SVP_SHAREDMEM_TO24(0);
			#endif

				ReleaseBuffer(0,bufIn);
				ReleaseBuffer(1,bufOut);

			#ifdef __MPEG4ENC_ARM_NMF
				if((bufOut != 0) && (bufOut->nFlags & BUFFERFLAG_EOS)) {
					iSendLogEvent.sendEventEOS(bufOut->nFlags);
				}
			#else
				if((bufOut != 0) && (bufOut->flags & BUFFERFLAG_EOS)) {
					iSendLogEvent.sendEventEOS(bufOut->flags);
				}
			#endif

				bufIn = 0;
				bufOut = 0;

				#ifdef LOGS_ENABLED
					emptyThisBufferDoneCount++;
					fillThisBufferDoneCount++;
					OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return emptyThisBuffer %d", emptyThisBufferDoneCount);
					OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: return fillThisBuffer %d", fillThisBufferDoneCount);
					printf("Mpeg4enc ddep:  return emptyThisBuffer %X\n", emptyThisBufferDoneCount);
					printf("Mpeg4enc ddep:  return fillThisBuffer %X\n", fillThisBufferDoneCount);
				#endif

				release_resources();
			}
		}
	}

	iSendLogEvent.eventProcess();
	//< FAIL_PRINT("before processActual exiting\n");

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "<= Mpeg4enc ddep: processActual()");
		printf("Mpeg4enc ddep:  processActual() exit\n");
	#endif
}

PUT_PRAGMA
void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
	int i=0;
    //Port_setTunnelStatus(&mPorts[portIdx],isTunneled);
    if (portIdx == -1)
    {
        for (i=0; i<TOTAL_PORT_COUNT; i++)
        {
            if (isTunneled & (1<<i))
            {
                //mPort[i].setTunnelStatus(true);

				#ifdef __MPEG4ENC_ARM_NMF
					mPorts[i].setTunnelStatus(true);
				#else
					Port_setTunnelStatus(&mPorts[i],true);
				#endif
			}
            else
            {
                //mPort[i].setTunnelStatus(false);

				#ifdef __MPEG4ENC_ARM_NMF
					mPorts[i].setTunnelStatus(false);
				#else
					Port_setTunnelStatus(&mPorts[i],false);
				#endif

            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
			#ifdef __MPEG4ENC_ARM_NMF
				mPorts[portIdx].setTunnelStatus(true);
			#else
				Port_setTunnelStatus(&mPorts[portIdx],true);
			#endif

            //mPort[portIdx].setTunnelStatus(true);
        }
        else
        {
			#ifdef __MPEG4ENC_ARM_NMF
				mPorts[portIdx].setTunnelStatus(false);
			#else
				Port_setTunnelStatus(&mPorts[portIdx],false);
			#endif

            //mPort[portIdx].setTunnelStatus(false);
        }
    }

	#ifdef LOGS_ENABLED
		//>OstTraceFiltInst0(TRACE_FLOW, "\n setTunnelStatus");
		printf("\n setTunnelStatus");
	#endif

}

PUT_PRAGMA
//< void fsmInit(t_uint16 portsDisabled,t_uint16 portsTunneled)
void METH(fsmInit)(fsmInit_t init)
{
    t_uint16 portsDisabled = init.portsDisabled;
    t_uint16 portsTunneled = init.portsTunneled;

#ifdef __MPEG4ENC_ARM_NMF

	if (init.traceInfoAddr)
	{
		setTraceInfo(init.traceInfoAddr, init.id1);
		iBrc.set_trace((void *)(init.traceInfoAddr), init.id1);
	}

	mPorts[0].init(InputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptrFIXME: mBufIn_list*/
								 INPUT_BUFFER_COUNT, /* bufferCount */
								 &inputport, /* return interface */
								 0, /* portIdx */
								 portsDisabled&1, /* isDisabled */
								 portsTunneled&1,
								 (Component *)this /* componentOwner */
								 );

	mPorts[1].init(OutputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptr FIXME: mBufOut_list*/
								 OUTPUT_BUFFER_COUNT, /* bufferCount */
								 &outputport, /* return interface */
								 1, /* portIdx */
								 (portsDisabled >> 1)&1, /* isDisabled */
								 (portsTunneled>>1)&1,
								 (Component *)this /* componentOwner */
								 );
   Component::init(2, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly

#else

    if (init.traceInfoAddr)
    {
		//>this->mTraceInfoPtr->dspAddr = init.traceInfoAddr;
		//this->mTraceInfoPtr->traceEnable = init.traceInfoAddr->traceEnable;
		//this->mId1 = init.id1;

		FSM_traceInit(&mDataDep, init.traceInfoAddr, init.id1);
		this = (TRACE_t *)&mDataDep;
		iBrc.set_trace((void *)(init.traceInfoAddr), init.id1);
		OstTraceInt3(TRACE_FLOW, "Mpeg4enc ddep:  dspAddr (0x%x) parentHandle (0x%x) traceEnable (0x%x) ",this->mTraceInfoPtr->dspAddr,this->mTraceInfoPtr->parentHandle,this->mTraceInfoPtr->traceEnable);
	}

	// Port are non supplier , fifo size is one ,
	Port_init(&mPorts[0], InputPort, false, 1, NULL, &mFifoIn, INPUT_BUFFER_COUNT, &inputport, 0, init.portsDisabled&1, init.portsTunneled&1, &mDataDep);
	Port_init(&mPorts[1], OutputPort,false, 1, NULL, &mFifoOut, OUTPUT_BUFFER_COUNT, &outputport, 1, (init.portsDisabled>>1)&1, (init.portsTunneled>>1)&1, &mDataDep);

	mDataDep.reset      = reset;
	mDataDep.process    = process;
	mDataDep.disablePortIndication = disablePortIndication;
	mDataDep.enablePortIndication = enablePortIndication;
	mDataDep.flushPortIndication = flushPortIndication;
	mDataDep.stateChangeIndication = stateChangeIndication;

	Component_init(&mDataDep, TOTAL_PORT_COUNT, mPorts, &proxy);

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  Component_init done\n");
		printf("Mpeg4enc ddep:  Component_init done\n");
	#endif

#endif
}


#ifdef __MPEG4ENC_ARM_NMF

void mpeg4enc_arm_nmf_parser::processEvent(void)
{
	Component::processEvent();
}

void mpeg4enc_arm_nmf_parser::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	#ifdef ENABLE_ARMNMF_LOGS
		emptyThisBufferCount++;
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: emptyThisBuffer %d", emptyThisBufferCount);
		printf("Mpeg4enc ddep:  emptyThisBuffer %X\n", emptyThisBufferCount);
	#endif
	Component::deliverBuffer(0, pBuffer);
}

void mpeg4enc_arm_nmf_parser::fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	#ifdef LOGS_ENABLED
		fillThisBufferCount++:
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: fillThisBuffer %d", fillThisBufferCount);
		printf("Mpeg4enc ddep:  fillThisBuffer %X\n", fillThisBufferCount);
	#endif
	Component::deliverBuffer(1, pBuffer);
}

mpeg4enc_arm_nmf_parser::mpeg4enc_arm_nmf_parser()
{
	#ifdef ENABLE_ARMNMF_LOGS
		//>OstTraceFiltInst0(TRACE_FLOW, "mpeg4enc_arm_nmf_parser::mpeg4enc_arm_nmf_parser\n");
		printf("mpeg4enc_arm_nmf_parser::mpeg4enc_arm_nmf_parser\n");
	#endif

	set_flag_for_eow = 0;
	handle_send_command_eow = 0;
	eosReached = 0;
	get_codec_parameter = 0;

	get_hw_resource = 0;
	get_hw_resource_requested = 0;

	frame_number = 0;

	bufIn = NULL;
	bufOut = NULL;

	stab_crop_array_start = 0;
	stab_crop_array_end_plus1 = 0;

	fillThisBufferCount = 0;
	emptyThisBufferCount = 0;
	fillThisBufferDoneCount = 0;
	emptyThisBufferDoneCount = 0;

	set_param_done = 0;

	prev_bitstream_size = 0;
	prev_no_of_packets = 0;
	partial_end_algo_count=0;

	compState = OMX_StateIdle;

	param_desc_mpc_addr = NULL;

	last_read_conf_no = 0;
	new_config_received = 0;

	vos_written = NOT_WRITTEN;
	filledLenLastVOL = 0;

	once = 1;

	prev_pts = 0;

	filledLen = 0;

	previous_skipped_flag = 0;

	control_algo_called = 0;
	pause_state_called = 0;

	param.ready = 0;
    param.algoId = ID_SEC_JPEG;
    param.addr_in_frame_buffer = NULL;
    param.addr_out_frame_buffer = NULL;
    param.addr_internal_buffer = NULL;
    param.addr_header_buffer = NULL;
    param.addr_in_bitstream_buffer = NULL;
    param.addr_out_bitstream_buffer = NULL;
    param.addr_in_parameters = NULL;
    param.addr_out_parameters = NULL;
    param.addr_in_frame_parameters = NULL;
    param.addr_out_frame_parameters = NULL;

    s_mpeg4e_ddep_desc.bbm_desc.nSize = 0;
    s_mpeg4e_ddep_desc.bbm_desc.nPhysicalAddress = 0;
    s_mpeg4e_ddep_desc.bbm_desc.nLogicalAddress = 0;
    s_mpeg4e_ddep_desc.bbm_desc.nMpcAddress = 0;

	s_mpeg4e_ddep_desc.s_debug_buffer_desc.nSize = 0;
	s_mpeg4e_ddep_desc.s_debug_buffer_desc.nPhysicalAddress = 0;
	s_mpeg4e_ddep_desc.s_debug_buffer_desc.nLogicalAddress = 0;
	s_mpeg4e_ddep_desc.s_debug_buffer_desc.nMpcAddress = 0;

	current_header.gobFrameId = 0;
	current_header.pictureCodingType = 0;
	current_header.pictureNb = 0;
	current_header.pts = 0;
	current_header.roundValue = 0;
	current_header.temporalSh.cumulTimeSlot = 0;
	current_header.temporalSh.slotDelay = 0;
	current_header.temporalSh.tr = 0;
	current_header.temporalSp.moduloTimeBase = 0;
	current_header.temporalSp.remainForOffset = 0;
	current_header.temporalSp.vopTimeIncrement = 0;
	current_header.temporalSp.vopTimeIncrementBitSize = 0;

	previous_header.gobFrameId = 0;
	previous_header.pictureCodingType = 0;
	previous_header.pictureNb = 0;
	previous_header.pts = 0;
	previous_header.roundValue = 0;
	previous_header.temporalSh.cumulTimeSlot = 0;
	previous_header.temporalSh.slotDelay = 0;
	previous_header.temporalSh.tr = 0;
	previous_header.temporalSp.moduloTimeBase = 0;
	previous_header.temporalSp.remainForOffset = 0;
	previous_header.temporalSp.vopTimeIncrement = 0;
	previous_header.temporalSp.vopTimeIncrementBitSize = 0;

	mStabCropVectorX[0] = 0;
	mStabCropVectorY[0] = 0;
	mStabCropWindowWidth[0] = 0;
	mStabCropWindowHeight[0] = 0;
	stab_crop_array_start = 0;
	stab_crop_array_end_plus1 = 0;
}

mpeg4enc_arm_nmf_parser::~mpeg4enc_arm_nmf_parser()
{

}

t_nmf_error mpeg4enc_arm_nmf_parser::construct(void)
{
	#ifdef ENABLE_ARMNMF_LOGS
		//>OstTraceFiltInst0(TRACE_FLOW, "construct called\n");
		printf("construct called\n");
	#endif
	return NMF_OK;
}


void mpeg4enc_arm_nmf_parser::destroy()
{

}

OMX_BOOL mpeg4enc_arm_nmf_parser::buffer_available_atinput()
{
	if((bufIn == NULL) && (mPorts[0].queuedBufferCount()))
	{
		#ifdef ENABLE_ARMNMF_LOGS
			OstTraceFiltInst0(TRACE_FLOW, "\n  remove buffer - input");
			printf("\n  remove buffer - input");
		#endif
		OstTraceInt0(TRACE_FLOW, "MPEG4Enc Ddep: New Input buffer available\n");
		bufIn = mPorts[0].dequeueBuffer();
		return OMX_TRUE;
	}
	else if(bufIn)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}


OMX_BOOL mpeg4enc_arm_nmf_parser::buffer_available_atoutput()
{
	if((bufOut == NULL) && (mPorts[1].queuedBufferCount()))
	{
		#ifdef ENABLE_ARMNMF_LOGS
			OstTraceFiltInst0(TRACE_FLOW, "\n  remove buffer - output");
			printf("\n  remove buffer - output");
		#endif
		bufOut = mPorts[1].dequeueBuffer();
		return OMX_TRUE;
	}
	else if(bufOut)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

#else //ifdef __MPEG4ENC_ARM_NMF

#pragma force_dcumode
void METH(eventReleasebuffer)(t_uint32 port_idx)
{

#if 0
	OmxEvent ev;
	OstTraceFiltInst0(TRACE_FLOW, "entering eventReleasebuffer\n");
	printf("entering eventReleasebuffer\n");
	ev.fsmEvent.signal =  OMX_RETURNBUFFER_SIG;
	ev.args.returnBuffer.portIdx = port_idx;
	if(port_idx)
	{
		ev.args.returnBuffer.buffer = bufIn;
		bufIn=0;
	}
	else
	{
		ev.args.returnBuffer.buffer = bufOut_slice;
		bufOut_slice=0;
	}
	FSM_dispatch(&mDataDep,&ev);
	OstTraceFiltInst0(TRACE_FLOW, "exiting eventReleasebuffer\n");
	printf("exiting eventReleasebuffer\n");
#endif
}

#pragma force_dcumode
void METH(processEvent)()
{
	Component_processEvent(&mDataDep);
}

#pragma force_dcumode
void METH(emptyThisBuffer)(Buffer_p buf)
{
#ifdef LOGS_ENABLED
	hcl_printf("ddepETB--");
	PRINT_VAR_MY(TICKS());	//T7
	emptyThisBufferCount++;
	OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: emptyThisBuffer %d", emptyThisBufferCount);
	printf("Mpeg4enc ddep:  emptyThisBuffer %X\n", emptyThisBufferCount);
#endif
	Component_deliverBuffer(&mDataDep, 0, buf);
}

#pragma force_dcumode
void METH(fillThisBuffer)(Buffer_p buf)
{
	#ifdef LOGS_ENABLED
		hcl_printf("ddepFTB--");
		PRINT_VAR_MY(TICKS());
		fillThisBufferCount++;
		OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep: fillThisBuffer %d", fillThisBufferCount);
		printf("Mpeg4enc ddep:  fillThisBuffer %X\n", fillThisBufferCount);
	#endif

	Component_deliverBuffer(&mDataDep, 1, buf);
}

/*****************************************************************************/
/**
 * \brief  informResourceStatus
 *
 *	Get Ressource status from RM
  */
/*****************************************************************************/
#pragma force_dcumode
void METH(informResourceStatus)(t_t1xhv_resource_status status, t_t1xhv_resource resource)
{
	#ifdef LOGS_ENABLED
		hcl_printf("ddepIRS--");
		PRINT_VAR_MY(TICKS());
		OstTraceFiltInst0(TRACE_FLOW, "\nMpeg4enc ddep:  iSendLogEvent informResourceStatus\n");
		printf("\nMpeg4enc ddep:  iSendLogEvent informResourceStatus\n");
	#endif

	iSendLogEvent.eventInformResourceStatus(status,resource);
}

#pragma force_dcumode
void METH(eventInformResourceStatus)(t_t1xhv_resource_status status,t_t1xhv_resource resource)
{
	FsmEvent ev;

	#ifdef LOGS_ENABLED
		hcl_printf("ddepEvIRS--");
		PRINT_VAR_MY(TICKS());
		OstTraceFiltInst1(TRACE_FLOW, "Mpeg4enc ddep:  informResourceStatus %d\n",status);
		printf("Mpeg4enc ddep:  informResourceStatus %d\n",status);
	#endif

	switch(status)
	{
		case STA_RESOURCE_FREE    :
			// A previously allocated resource is now free, so ready to be used.
			// This will call the function process if componentis still in Executing mode
			// Cannot launch directly the encoder because may be the component is no more in
			// executing
			get_hw_resource=1;
			set_flag_for_eow=0;
			get_hw_resource_requested=0;
			if(compState != OMX_StateExecuting)
				release_resources();
			else
				processActual(&mDataDep);

			break;

		case STA_PARAM_ERROR      :	// Input parameter error so that allocation can't be done.
			PRINT_VAR(status);
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: Value of status (0x%x) at line no %d \n",status,__LINE__);
			break;

		default:
			break;
	}
}

PUT_PRAGMA
void METH(start)()
{
}

#endif	//ifdef __MPEG4ENC_ARM_NMF

PUT_PRAGMA
void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{

	#ifdef	LOGS_ENABLED
		hcl_printf("ddepSeC--");
		PRINT_VAR_MY(TICKS());
		//>OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  sendCommand cmd=%d, param=%d\n", cmd, param);
		printf("Mpeg4enc ddep:  sendCommand cmd=%d, param=%d\n", cmd, param);
	#endif

	if(cmd==OMX_CommandStateSet)
	{
		switch((OMX_STATETYPE)param)
		{
			case OMX_StatePause:
#ifndef __MPEG4ENC_ARM_NMF
				if (compState == OMX_StateExecuting)
				{
					iSleep.allowSleep(&iInformResourceStatus);
//>		OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep:  sendCommand cmd=%d, param=%d\n", cmd, param);
					printf("MPEG4Enc Ddep: iSleep.allowSleep(&iInformResourceStatus)\n");
				}
#endif
				compState = (OMX_STATETYPE)param;
				break;
			case OMX_StateIdle:
#ifndef __MPEG4ENC_ARM_NMF
				if (compState == OMX_StateExecuting)
				{
					iSleep.allowSleep(&iInformResourceStatus);
					//>OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc Ddep: iSleep.allowSleep(&iInformResourceStatus)\n");
					printf("MPEG4Enc Ddep: iSleep.allowSleep(&iInformResourceStatus)\n");
				}
#endif
				compState = (OMX_STATETYPE)param;
 				break;
			case OMX_StateExecuting:
#ifndef __MPEG4ENC_ARM_NMF
				if (compState != OMX_StateExecuting)
				{
					iSleep.preventSleep(&iInformResourceStatus);
					//>OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc Ddep: iSleep.preventSleep(&iInformResourceStatus)\n");
					printf("MPEG4Enc Ddep: iSleep.preventSleep(&iInformResourceStatus)\n");
				}
#endif
				compState = (OMX_STATETYPE)param;
				break;
			default:
			{
				#ifdef __MPEG4ENC_ARM_NMF
					//nothing
				#else
					ASSERT(0);
				#endif
			}
		}
	}
	if((cmd == OMX_CommandPortDisable) || (cmd == OMX_CommandFlush) || ((cmd == OMX_CommandStateSet) && (compState == OMX_StateIdle)))
	{
		handle_send_command_eow = 1;

		PRINT_VAR(handle_send_command_eow);
		PRINT_VAR(set_flag_for_eow);
		PRINT_VAR(get_hw_resource);
		PRINT_VAR(get_hw_resource_requested);
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: sendcommand cmdportdis or flush stateset stateidle");
		if((cmd == OMX_CommandStateSet) && (compState == OMX_StateIdle))
		{
			OstTraceFiltInst2(TRACE_FLOW, "Mpeg4enc ddep: sendcommand cmdportdis or flush stateset stateidle get_hw_resource- %d,get_hw_resource_requested-%d",get_hw_resource,get_hw_resource_requested);
			if((bufOut || bufIn )&& (!get_hw_resource) &&(!get_hw_resource_requested))
			{
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: sendcommand cmdportdis or flush stateset stateidle");
					if(bufOut) {
						#ifdef LOGS_ENABLED
							printf("Mpeg4enc ddep:  port 1\n");
						#endif
							OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: sendcommand Releaseing outbuffer");
					#ifdef __MPEG4ENC_ARM_NMF
						bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
					#else
						bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
					#endif

						ReleaseBuffer(1,bufOut);
						bufOut = 0;
					}
					if(bufIn)
					{
						#ifdef LOGS_ENABLED
							printf("Mpeg4enc ddep:  port 0\n");
						#endif
							OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: sendcommand Releaseing inbuffer");
					#ifdef __MPEG4ENC_ARM_NMF
						bufIn->nFilledLen = SVP_SHAREDMEM_TO24(0);
					#else
						bufIn->filledLen = SVP_SHAREDMEM_TO24(0);
					#endif

						ReleaseBuffer(0,bufIn);
						bufIn=0;
					}
					once = 1;
					prev_pts = 0;
					eosReached=0;
					previous_skipped_flag=0;
					frame_number=0;
			}
		}

		if(cmd == OMX_CommandPortDisable && param == 0)	//Implies: when Port 0 is disabled
		{
			#ifdef	LOGS_ENABLED
				printf("\n[DDep] Port 0 Disabled\n");
			#endif
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep: sendcommand Implies: when Port 0 is disabled");
			//frame_number =0;
			//once = 1;
			//prev_pts = 0;
			once = 1;
			prev_pts = 0;
			eosReached=0;
			previous_skipped_flag=0;
			frame_number=0;
		}

		if(set_flag_for_eow && get_hw_resource)
		{
			#ifdef	LOGS_ENABLED
				printf("Mpeg4enc ddep:  set_flag_for_eow && get_hw_resource\n");
			#endif

			//< linkin_loop();	//< FIXME
			//< add_buffer_to_next_link();
			handle_send_command_eow = 0;
		}
		if((bufOut || bufIn )&& (!get_hw_resource) &&(!get_hw_resource_requested))
		{
			#ifdef LOGS_ENABLED
				printf("Mpeg4enc ddep:  (!get_hw_resource) &&(!get_hw_resource_requested)\n");
			#endif
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  (!get_hw_resource) &&(!get_hw_resource_requested)");
			handle_send_command_eow=0;
			if(bufOut && (param ==1))
			{
				#ifdef LOGS_ENABLED
					printf("Mpeg4enc ddep:  port 1\n");
				#endif

			#ifdef __MPEG4ENC_ARM_NMF
				bufOut->nFilledLen = SVP_SHAREDMEM_TO24(0);
			#else
				bufOut->filledLen = SVP_SHAREDMEM_TO24(0);
			#endif

				ReleaseBuffer(1,bufOut);
				bufOut = 0;
			}
			if(bufIn && (param ==0))
			{
				#ifdef LOGS_ENABLED
					printf("Mpeg4enc ddep:  port 0\n");
				#endif

			#ifdef __MPEG4ENC_ARM_NMF
				bufIn->nFilledLen = SVP_SHAREDMEM_TO24(0);
			#else
				bufIn->filledLen = SVP_SHAREDMEM_TO24(0);
			#endif

				ReleaseBuffer(0,bufIn);
				bufIn=0;
			}
		}

		if((!bufOut) && (!bufIn))
		{
			handle_send_command_eow=0;
			OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  if((!bufOut) && (!bufIn))");
			once = 1;
			prev_pts = 0;
			eosReached=0;
			previous_skipped_flag=0;
			frame_number=0;
		}
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc ddep:  sendCommand exited\n");
	#endif


	if(((cmd == OMX_CommandStateSet) && (param == OMX_StatePause)) && control_algo_called)
	{
		pause_state_called = 1;
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  pause_state_called = 1");
	}
	else
	{
	#ifdef __MPEG4ENC_ARM_NMF
		Component::sendCommand(cmd, param);
	#else
		Component_sendCommand(&mDataDep, cmd, param);
	#endif
	}
}

PUT_PRAGMA
void METH(eventProcess)()
{
	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  eventProcess() enter\n");
		printf("Mpeg4enc ddep:  eventProcess() enter\n");
	#endif

	if(!set_flag_for_eow)
	{
		if(get_hw_resource)
		{
			// resource already aquired and in use
			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  eventProcess() exit (get_hw_resource)\n");
				printf("Mpeg4enc ddep:  eventProcess() exit (get_hw_resource)\n");
			#endif

			return;
		}

		if(get_hw_resource_requested)
		{
			// resource already requested and in use
			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  eventProcess() exit (get_hw_resource_requested)\n");
				printf("Mpeg4enc ddep:  eventProcess() exit (get_hw_resource_requested)\n");
			#endif

			return;
		}

		// Check buffers and Parameter availabilities
	#ifdef __MPEG4ENC_ARM_NMF
		if(get_codec_parameter && buffer_available_atinput() && buffer_available_atoutput())
	#else
		if(get_codec_parameter && buffer_available_atinput(&mDataDep) && buffer_available_atoutput(&mDataDep))
	#endif
		{
			download_parameters();

		#ifdef __MPEG4ENC_ARM_NMF
			#ifdef ENABLE_ARMNMF_LOGS
				printf ("About to call processActual from here \n");
			#endif
			/* + changes for ER 358263/ER374738 */
			get_hw_resource=1;
			/* - changes for ER 358263/ER374738 */
			processActual();
		#else
			// Call to RM to get the HW resource
			get_hw_resource_requested = 1;

			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  iResource.allocResource() enter\n");
				printf("Mpeg4enc ddep:  iResource.allocResource() enter\n");
			#endif

			iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);

			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  iResource.allocResource() exit\n");
				printf("Mpeg4enc ddep:  iResource.allocResource() exit\n");
			#endif
		#endif

		}

	}

	//> PRINT_VAR(set_flag_for_eow);
	if(set_flag_for_eow)
	{
		if(get_hw_resource)			//< redundant check ?
			set_flag_for_eow=0;
		//> PRINT_VAR(set_flag_for_eow);

	#ifdef __MPEG4ENC_ARM_NMF
		if(buffer_available_atoutput())
	#else
		if(buffer_available_atoutput(&mDataDep))
	#endif
		{
			add_buffer_to_next_link();

			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  updateAlgo() enter\n");
				printf("Mpeg4enc ddep:  updateAlgo() enter\n");
			#endif


			#ifdef __MPEG4ENC_ARM_NMF
				//nothing
			#else
				codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
			#endif


			#ifdef LOGS_ENABLED
				OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  updateAlgo() exit\n");
				printf("Mpeg4enc ddep:  updateAlgo() exit\n");
			#endif
		}
	}

	#ifdef LOGS_ENABLED
		OstTraceFiltInst0(TRACE_FLOW, "Mpeg4enc ddep:  eventProcess() exit\n");
		printf("Mpeg4enc ddep:  eventProcess() exit\n");
	#endif

}

#ifdef __MPEG4ENC_ARM_NMF
STATIC_FLAG void COMP(ReleaseBuffer)(t_uint32 port_idx, OMX_BUFFERHEADERTYPE* buf)
#else
STATIC_FLAG void ReleaseBuffer(t_uint32 port_idx, Buffer_p buf)
#endif
{
	#ifdef __MPEG4ENC_ARM_NMF
		if (port_idx == 0)
		{
			#ifdef ENABLE_ARMNMF_LOGS
				OstTraceFiltInst0(TRACE_FLOW, "Releasing the input buffer \n");
				printf("Releasing the input buffer \n");
			#endif
		}
		if (port_idx == 1)
		{
			#ifdef ENABLE_ARMNMF_LOGS
				OstTraceFiltInst0(TRACE_FLOW, "Releasing the output buffer \n");
				printf("Releasing the output buffer \n");
			#endif
		}

		if(isDispatching())
		{
			mPorts[port_idx].returnBuffer(buf);
		}
		else
		{
			#ifdef ENABLE_ARMNMF_LOGS
				OstTraceFiltInst0(TRACE_FLOW, "return from Asyn - input \n");
				printf("return from Asyn - input \n");
			#endif
			returnBufferAsync(port_idx,buf);
    	}
	#else
		OmxEvent ev;

		#ifdef LOGS_ENABLED
			OstTraceFiltInst1(TRACE_FLOW, "entering ReleaseBuffer bufferport_index=%ld\n",(t_uint32)port_idx);
			printf("entering ReleaseBuffer bufferport_index=%ld\n",(t_uint32)port_idx);
		#endif

		ev.fsmEvent.signal =  OMX_RETURNBUFFER_SIG;
		ev.args.returnBuffer.portIdx = port_idx;
		ev.args.returnBuffer.buffer = buf;
		FSM_dispatch(&mDataDep,&ev);

		#ifdef LOGS_ENABLED
			OstTraceFiltInst0(TRACE_FLOW, "exiting ReleaseBuffer\n");
			printf("exiting ReleaseBuffer\n");
		#endif

	#endif
}

PUT_PRAGMA
void METH(sendEventEOS)(t_uint32 flags)
{
	proxy.eventHandler(OMX_EventBufferFlag, 1, flags);
}

#ifdef NONE
/* dummy define for editor */
	#define SHAREDMEM
#endif

