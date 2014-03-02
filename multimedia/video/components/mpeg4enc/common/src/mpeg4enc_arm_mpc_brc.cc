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
	#include <mpeg4enc/arm_nmf/brc.nmf>
	#include "vfm_vec_mpeg4.idt"
	#include <common/inc/mpeg4enc_arm_mpc_common.h>
	#include <common/inc/mpeg4enc_arm_mpc_brc.h> //common header file for both arm and mpc
	#include "TraceObject.h"
	#include <brc.hpp>
#else
	// Include self generated NMF header file for this component
	#include <mpeg4enc/mpc/brc.nmf>
	#include "vfm_vec_mpeg4.idt"
	#include <mpeg4enc/common/inc/mpeg4enc_arm_mpc_common.h>
	#include <mpeg4enc/common/inc/mpeg4enc_arm_mpc_brc.h> //common header file for both arm and mpc
	#include "fsm/generic/include/FSM.h"
	//< #define __DEBUG_TRACE_ENABLE
	#include <brc.h>
#endif

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#ifdef __MPEG4ENC_ARM_NMF
	#include "video_components_mpeg4enc_arm_nmf_brc_src_brcTraces.h"
#else
	#include "video_components_mpeg4enc_mpc_brc_src_brcTraces.h"
#endif
#endif

#ifdef __MPEG4ENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		#define LOGS_ENABLED	
	#endif
#else
	#define LOGS_ENABLED
#endif

//> Taken from reference. For the time being, ref does not define IP_QP_CORR flag.
#ifdef IP_QP_CORR
#undef _VBR_IP_QP_CORRELATION
#else
#define _VBR_IP_QP_CORRELATION
#endif

#define VIQP

#ifndef __MPEG4ENC_ARM_NMF
// Global allocation for all trace ressources
//<>static TraceInfo_t EXTMEM traceInfo   	= {0,0,0};
//<>static TRACE_t     EXTMEM traceObject 	= {&traceInfo, 0};
static TRACE_t * EXTMEM this ;//<>			= (TRACE_t *)&traceObject;
static TRACE_t   EXTMEM brcTraceObj ;
#endif

#ifdef __MPEG4ENC_ARM_NMF
mpeg4enc_arm_nmf_brc::mpeg4enc_arm_nmf_brc()
{
	once_global = 1;
	
	eotFifo.eotData[0].bitstreamSizeInBits = 0;		//> FIXME: more members to initialize
	eotFifo.eotData[1].bitstreamSizeInBits = 0;		//> FIXME: more members to initialize
	eotFifo.ptrWrite = 0;

	skipFifo[0].gobFrameId = 0;
	skipFifo[0].pictureCodingType = 0;
	skipFifo[0].pictureNb = 0;
	skipFifo[0].pts = 0;
	skipFifo[0].roundValue = 0;
	skipFifo[0].temporalSh.cumulTimeSlot = 0;	//> FIXME: more members to initialize
	skipFifo[0].temporalSp.moduloTimeBase = 0;	//> FIXME: more members to initialize

	state_cbr.bitRateDelayed = 0;
	state_cbr.brcTargetMinPred[0] = 0;
	state_cbr.brcTargetMinPred[1] = 0;
	state_cbr.buffer = 0;
	state_cbr.bufferDepletion = 0;
	state_cbr.bufferFakeTs = 0;
	state_cbr.bufferMod = 0;
	state_cbr.deltaTicks = 0;
	state_cbr.deltaTimeStamp = 0;
	state_cbr.fakeFlag = 0;
	state_cbr.fixedVopTimeIncrement = 0;
	state_cbr.frameRate = 0;
	state_cbr.govFlag = 0;
	state_cbr.initTsModuloOld = 0;
	state_cbr.intraPeriod = 0;
	state_cbr.maxBufferLevel = 0;
	state_cbr.nextFrameRate = 0;
	state_cbr.oldModuloTimeBase = 0;
	state_cbr.picTarget = 0;
	state_cbr.pictureCodingType[0] = 0;
	state_cbr.pictureCounter = 0;
	state_cbr.prevBuffer = 0;
	state_cbr.prevPictureCodingType = 0;
	state_cbr.prevPts = 0;
	state_cbr.prevStrategicSkip = 0;
	state_cbr.prevVopTimeIncrement = 0;
	state_cbr.ptsDiff = 0;
	state_cbr.sMax = 0;
	state_cbr.saveBrcOut.bitRate = 0; 	//> FIXME: more members to initialize
	state_cbr.skipCount[0] = 0;
	state_cbr.skipCount[1] = 0;
	state_cbr.targetBuffLevel = 0;
	state_cbr.vopTimeIncrementResolution = 0;

	state_qp_constant.bitRateDelayed = 0;
	state_qp_constant.brcTargetMinPred[0] = 0;	
	state_qp_constant.buffer = 0;
	state_qp_constant.bufferDepletion = 0;
	state_qp_constant.bufferMod = 0;
	state_qp_constant.deltaTicks = 0;
	state_qp_constant.deltaTimeStamp = 0;
	state_qp_constant.fixedVopTimeIncrement = 0;
	state_qp_constant.frameRate = 0;
	state_qp_constant.initTsModuloOld = 0;
	state_qp_constant.intraPeriod = 0;
	state_qp_constant.maxBufferLevel = 0;
	state_qp_constant.nextFrameRate = 0;
	state_qp_constant.oldModuloTimeBase = 0;
	state_qp_constant.picTarget = 0;
	state_qp_constant.pictureCodingType[0] = 0;
	state_qp_constant.pictureCounter = 0;
	state_qp_constant.prevBuffer = 0;
	state_qp_constant.prevPts = 0;
	state_qp_constant.prevStrategicSkip = 0;
	state_qp_constant.prevVopTimeIncrement = 0;
	state_qp_constant.ptsCor = 0;
	state_qp_constant.sMax = 0;
	state_qp_constant.saveBrcOut.bitRate = 0;
	state_qp_constant.skipCount[0] = 0;
	state_qp_constant.skipPrevCount = 0;
	state_qp_constant.targetBuffLevel = 0;
	state_qp_constant.vopTimeIncrementResolution = 0;

	state_vbr.bitRateDelayed = 0;
	state_vbr.brcTargetMinPred[0] = 0;
	state_vbr.buffer = 0;
	state_vbr.bufferDepletion = 0;
	state_vbr.bufferMod = 0;
	state_vbr.deltaTicks = 0;
	state_vbr.deltaTimeStamp = 0;
	state_vbr.fixedVopTimeIncrement = 0;
	state_vbr.frameRate = 0;
	state_vbr.initTsModuloOld = 0;
	state_vbr.intraPeriod = 0;
	state_vbr.maxBufferLevel = 0;
	state_vbr.minBaseQuality = 0;
	state_vbr.minFrameRate = 0;
	state_vbr.nextFrameRate = 0;
	state_vbr.oldModuloTimeBase = 0;
	state_vbr.picTarget = 0;
	state_vbr.pictureCodingType[0] = 0;
	state_vbr.pictureCounter = 0;
	state_vbr.prevBuffer = 0;
	state_vbr.prevPts = 0;
	state_vbr.prevVopTimeIncrement = 0;
	state_vbr.ptsCor = 0;
	state_vbr.sMax = 0;
	state_vbr.saveBrcOut.bitRate = 0;
	state_vbr.skipCount[0] = 0;
	state_vbr.skipPrevCount = 0;
	state_vbr.targetBuffLevel = 0;
	state_vbr.vopTimeIncrementResolution = 0;

	isCurrentItSkip = 0;
	isCurrentStrategicSkip = 0;
}

mpeg4enc_arm_nmf_brc::~mpeg4enc_arm_nmf_brc()
{
}

#endif

t_uint32 COMP_BRC(maxVOPsize)(t_uint32 mbnum)
{
	t_uint32 smax;

	if(mbnum<=99) smax=64 * ONE_KB;
	else if(mbnum<=396)	smax=256 * ONE_KB;
	else if(mbnum<=1584) smax=512 * ONE_KB;
	else smax=1024 * ONE_KB;

	return smax;
}

void COMP_BRC(NoBRC_InitSeq)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip)
{
	t_uint32 MBnum = (mp4_par_in->window_height *
					  mp4_par_in->window_width) / 256;
	t_uint16 deltaTStamp;
	t_uint32 max_BUFFER_level = 0;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  NoBRC_InitSeq() enter\n");
	#endif

	/* pts correction */
	pts = pts - pStateQpConstant->ptsCor;

	*pIsPreviousSkip = FALSE;

	pStateQpConstant->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);

	/*init hcl specific variable*/
	pStateQpConstant->skipCount[0] = 0;
	pStateQpConstant->skipCount[1] = 0;
	pStateQpConstant->pictureCodingType[0] = I_TYPE;
	pStateQpConstant->pictureCodingType[1] = I_TYPE;
	pStateQpConstant->brcTargetMinPred[0] = 0;
	pStateQpConstant->brcTargetMinPred[1] = 0;
	pStateQpConstant->ptsCor = 0;
	pStateQpConstant->skipPrevCount = 0;

	/* NoBRC_InitSeq() code */
	/* VBR host variable */
	pStateQpConstant->frameRate = ((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment;

	pStateQpConstant->nextFrameRate = pStateQpConstant->frameRate;

	if(mp4_par_in->flag_short_header == 1)
	{
		pStateQpConstant->vopTimeIncrementResolution = 30000;
		pStateQpConstant->fixedVopTimeIncrement = 1001;
	}
	else
	{
		pStateQpConstant->vopTimeIncrementResolution = SVP_SHAREDMEM_FROM16(mp4_par_in->vop_time_increment_resolution);
		pStateQpConstant->fixedVopTimeIncrement = SVP_SHAREDMEM_FROM16(mp4_par_in->fixed_vop_time_increment);
	}

	pStateQpConstant->bufferMod = 0;
	pStateQpConstant->prevVopTimeIncrement = 0;
	pStateQpConstant->oldModuloTimeBase = 0;
	if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_NONE)
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  buffering model none\n");//print
		#endif

		pStateQpConstant->sMax = maxVOPsize(MBnum);
	}
	else if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
	{
		pStateQpConstant->sMax = maxVOPsize(MBnum);
		pStateQpConstant->maxBufferLevel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateQpConstant->frameRate >> 1)) / pStateQpConstant->frameRate;
		pStateQpConstant->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateQpConstant->frameRate >> 1)) / pStateQpConstant->frameRate;
		pStateQpConstant->targetBuffLevel = (3277 * pStateQpConstant->picTarget) >> 15;
	}
	else
	{
		pStateQpConstant->sMax = SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer);
		pStateQpConstant->maxBufferLevel = BRCMIN(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer),SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6);
		pStateQpConstant->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateQpConstant->frameRate >> 1)) / pStateQpConstant->frameRate;
		pStateQpConstant->targetBuffLevel = (pStateQpConstant->maxBufferLevel + 1) >> 1;
	}

	pStateQpConstant->buffer = 0;
	pStateQpConstant->initTsModuloOld = (t_sint16)-((pStateQpConstant->fixedVopTimeIncrement % pStateQpConstant->vopTimeIncrementResolution) * 
													(((t_sint32)pStateQpConstant->vopTimeIncrementResolution / mp4_par_in->vop_time_increment_resolution + ((pStateQpConstant->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment) >> 1)) /
													 (pStateQpConstant->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment)));
	deltaTStamp = (t_uint16)((pStateQpConstant->fixedVopTimeIncrement % pStateQpConstant->vopTimeIncrementResolution) * 
							 (((t_uint32)pStateQpConstant->vopTimeIncrementResolution / mp4_par_in->vop_time_increment_resolution + ((pStateQpConstant->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment) >> 1)) /
							  (pStateQpConstant->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment)));

	deltaTStamp = deltaTStamp / pStateQpConstant->fixedVopTimeIncrement; 
	pStateQpConstant->bufferDepletion = (deltaTStamp * SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) * pStateQpConstant->fixedVopTimeIncrement) / pStateQpConstant->vopTimeIncrementResolution;

	pStateQpConstant->intraPeriod = mp4_custom_par_in->no_of_p_frames * mp4_par_in->fixed_vop_time_increment;	//< FIXME added by kiran
	pStateQpConstant->deltaTicks = 0;
	pStateQpConstant->prevStrategicSkip = 0;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  brc output params\n");
	#endif

	/*hamac brc value*/
	pbrc_par_out->pictureCodingType = I_TYPE;
	pbrc_par_out->quant = SVP_SHAREDMEM_FROM16(mp4_par_in->quant);
	
	if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_NONE)
		pbrc_par_out->brcType = SVA_BRC_QP_BUFFERING_NONE;
	else if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
		pbrc_par_out->brcType = SVA_BRC_QP_CONSTANT_HRD;
	else
		pbrc_par_out->brcType = SVA_BRC_QP_CONSTANT_VBV_ANNEX_G;
	
	pbrc_par_out->brcFrameTarget = 0;
	pbrc_par_out->brcTargetMinPred = pStateQpConstant->bufferDepletion;

	if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
	{
		pbrc_par_out->brcTargetMaxPred = 4 * pStateQpConstant->maxBufferLevel + pStateQpConstant->sMax + pStateQpConstant->picTarget;
	}
	else
	{
		pbrc_par_out->brcTargetMaxPred = (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6) + pStateQpConstant->picTarget;
	}

	//>Experimental! Taken from reference code
	pbrc_par_out->brcTargetMaxPred = BRCMAX( 0, (t_sint32)max_BUFFER_level );
	//>end
	pbrc_par_out->skipCount = 0;
	pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
	pbrc_par_out->frameRate = pStateQpConstant->frameRate;
	pbrc_par_out->deltaTarget = pStateQpConstant->targetBuffLevel - pStateQpConstant->buffer;    
	pbrc_par_out->minQp = 2; 
	pbrc_par_out->maxQp = 31;
	pbrc_par_out->vopTimeIncrementResolution = pStateQpConstant->vopTimeIncrementResolution;
	pbrc_par_out->fixedVopTimeIncrement = pStateQpConstant->fixedVopTimeIncrement;
	pbrc_par_out->smax = pStateQpConstant->sMax;
	pbrc_par_out->minBaseQuality = 0;/*not use in cbr*/
	pbrc_par_out->minFrameRate = 0;/*not use in cbr*/
	pbrc_par_out->maxBuffLevel = pStateQpConstant->maxBufferLevel;
	pbrc_par_out->tsSeconds = 0;
	pbrc_par_out->tsModulo = 0;
	pbrc_par_out->firstISkippedFlag = 0;
	pbrc_par_out->initTsModuloOld = pStateQpConstant->initTsModuloOld;

	/* save brcOut */
	pStateQpConstant->saveBrcOut = *pbrc_par_out;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  save brc state\n");
	#endif
	
	/*init dataFifo*/
	eotFifo.eotData[0].bitstreamSizeInBits = pStateQpConstant->bufferDepletion;
	eotFifo.eotData[0].bufferFullness = 0;
	eotFifo.eotData[0].skipPrev = 0;
	eotFifo.eotData[0].skipCurrent = 0;
	eotFifo.ptrWrite = 1;
	
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  increment pictureCounter\n");
	#endif

	pStateQpConstant->pictureCounter++;

	PRINT_VAR(pStateQpConstant->pictureCounter);

	PRINT_VAR(-pStateQpConstant->initTsModuloOld);
	PRINT_VAR(pStateQpConstant->fixedVopTimeIncrement);

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  calculate deltatimestamps\n");
	#endif

	pStateQpConstant->deltaTimeStamp = ((t_sint16)(-pStateQpConstant->initTsModuloOld))/(t_sint16)pStateQpConstant->fixedVopTimeIncrement;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  calculate framerate\n");
	#endif

	/* not exactely the same frameRate */
	//> pStateQpConstant->frameRate = (t_uint16)(((mp4_par_in->vop_time_increment_resolution) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment)<<10;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  save brc prev timestamps\n");
	#endif

	pStateQpConstant->prevPts = pts;	//< FIXME take care of timestamps

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  NoBRC_InitSeq() exit\n");
	#endif

	return;
}
/**Buffer Depletion added to the function argument to pass the value calculated here to the algo**/ 
void COMP_BRC(CBR_InitSeq)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip ,t_uint32 *buffer_depletion)
{
	//>t_sint32 bufferLevel;
	t_uint32 MBnum = (mp4_par_in->window_height *
					  mp4_par_in->window_width) / 256;
	t_uint16 deltaTStamp;
	t_uint32 seconds;
	t_uint32 modulo;
	t_uint32 bitsPerPixel;
	t_uint16 viqp;
#ifdef LOGS_ENABLED
	printf("Mpeg4enc brc:  CBR_InitSeq() enter\n");
#endif
	OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() enter");
	*pIsPreviousSkip = FALSE;

	pStateCBR->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);

	PRINT_VAR(pStateCBR->bitRateDelayed);

	/*init hcl specific variable*/
	pStateCBR->skipCount[0] = 0;
	pStateCBR->skipCount[1] = 0;
	pStateCBR->pictureCodingType[0] = I_TYPE;
	pStateCBR->pictureCodingType[1] = I_TYPE;
	pStateCBR->brcTargetMinPred[0] = 0;
	pStateCBR->brcTargetMinPred[1] = 0;
	pStateCBR->prevPictureCodingType = I_TYPE;
	pStateCBR->fakeFlag = 1;
	pStateCBR->ptsDiff = 0;

	/* MaSaCBR_InitSeq() code*/
	/*VBR host variable*/
	/* +Change as framerate is truncated due to typecasting of t_uint16 otherwise wrong framerate is passed to algo leading to incorrect calculations in algo(done for ER - 354700 and ER - 350503)*/
	PRINT_VAR(((t_uint32)mp4_par_in->vop_time_increment_resolution << 10));
	PRINT_VAR((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10));
	PRINT_VAR(mp4_par_in->fixed_vop_time_increment);
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() (((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)) -%x",(((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)));
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() ((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)) -%x",((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)));
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() (mp4_par_in->fixed_vop_time_increment) -%x",(mp4_par_in->fixed_vop_time_increment));
	if(mp4_par_in->flag_short_header == 0)
	{
		//pStateCBR->frameRate = ((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment;
		pStateCBR->frameRate = ((((t_uint32)mp4_par_in->vop_time_increment_resolution) << 10) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment;
	}
	else
	{
		pStateCBR->frameRate = mp4_par_in->framerate;
	}
	/* -Change as framerate is truncated due to typecasting of t_uint16 otherwise wrong framerate is passed to algo leading to incorrect calculations in algo(done for ER - 354700 and ER - 350503)*/
	
	PRINT_VAR(pStateCBR->frameRate);
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() pStateCBR->frameRate -%x",pStateCBR->frameRate);
	PRINT_VAR(pStateCBR->frameRate);
	pStateCBR->nextFrameRate = pStateCBR->frameRate;
	if(mp4_par_in->flag_short_header == 1)
	{

		pStateCBR->vopTimeIncrementResolution = 30000;
		pStateCBR->fixedVopTimeIncrement = 1001;
	}
	else
	{
		pStateCBR->vopTimeIncrementResolution = SVP_SHAREDMEM_FROM16(mp4_par_in->vop_time_increment_resolution);
		pStateCBR->fixedVopTimeIncrement = mp4_par_in->fixed_vop_time_increment;
	}

	pStateCBR->bufferMod = 0;
	pStateCBR->prevVopTimeIncrement = 0;
	pStateCBR->oldModuloTimeBase = 0;

	if(mp4_par_in->flag_short_header == 1)
	{
		pStateCBR->maxBufferLevel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateCBR->frameRate >> 1)) / pStateCBR->frameRate;
		pStateCBR->sMax = maxVOPsize(MBnum);
		pStateCBR->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateCBR->frameRate >> 1)) / pStateCBR->frameRate;
		pStateCBR->targetBuffLevel = (3277 * pStateCBR->picTarget) >> 15;
	}
	else
	{
		pStateCBR->maxBufferLevel = BRCMIN(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer),SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6);
		pStateCBR->sMax = SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer);
		pStateCBR->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateCBR->frameRate >> 1)) / pStateCBR->frameRate;
		pStateCBR->targetBuffLevel = (3277 * pStateCBR->picTarget) >> 15;
	}

	PRINT_VAR(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer));
	PRINT_VAR(mp4_custom_par_in->vbv_occupancy);
	PRINT_VAR(pStateCBR->maxBufferLevel);
	PRINT_VAR(pStateCBR->sMax);
	PRINT_VAR(pStateCBR->picTarget);
	PRINT_VAR(pStateCBR->targetBuffLevel);
	OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer) -%x,mp4_custom_par_in->vbv_occupancy - %x",SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer),mp4_custom_par_in->vbv_occupancy);
	OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() (pStateCBR->maxBufferLevel) -%x ,(pStateCBR->sMax) -%x",(pStateCBR->maxBufferLevel),(pStateCBR->sMax));
	OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitSeq() (pStateCBR->picTarget) -%x , pStateCBR->targetBuffLevel -%x",(pStateCBR->picTarget),pStateCBR->targetBuffLevel);
	/* +Change as maxBufferlevel is used for I_PicTarget calculation in m4e_constant_rate_control.c at line 248 and should not be set 0 for first frame(done for ER - 350503)*/
	// Sent as 0 for frame1
	//pStateCBR->maxBufferLevel = 0;
	/* -Change as maxBufferlevel is used for I_PicTarget calculation in m4e_constant_rate_control.c at line 248 and should not be set 0 for first frame(done for ER - 350503)*/

	pStateCBR->buffer = 0;

	pStateCBR->initTsModuloOld = (t_sint16)-((pStateCBR->fixedVopTimeIncrement % pStateCBR->vopTimeIncrementResolution) * 
											 (((t_sint32)pStateCBR->vopTimeIncrementResolution / mp4_par_in->vop_time_increment_resolution + ((pStateCBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment) >> 1)) /
											  (pStateCBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment)));  

	deltaTStamp = (t_uint16)((pStateCBR->fixedVopTimeIncrement % pStateCBR->vopTimeIncrementResolution) * 
							 (((t_uint32)pStateCBR->vopTimeIncrementResolution / mp4_par_in->vop_time_increment_resolution + ((pStateCBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment) >> 1)) /
							  (pStateCBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment)));
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc deltaTStamp-%x",deltaTStamp);
	deltaTStamp = deltaTStamp / pStateCBR->fixedVopTimeIncrement; 
	
	PRINT_VAR(pStateCBR->initTsModuloOld);
	PRINT_VAR(deltaTStamp);

	pStateCBR->bufferDepletion = (deltaTStamp * SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) * pStateCBR->fixedVopTimeIncrement) / pStateCBR->vopTimeIncrementResolution;
	////PRINT_VAR(mp4_custom_par_in->no_of_p_frames);
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc pStateCBR->initTsModuloOld -%x",pStateCBR->initTsModuloOld);
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc deltaTStamp-%x",deltaTStamp);
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc pStateCBR->bufferDepletion -%x",pStateCBR->bufferDepletion);
	PRINT_VAR(pStateCBR->fixedVopTimeIncrement);
	//pStateCBR->intraPeriod = mp4_custom_par_in->no_of_p_frames * pStateCBR->fixedVopTimeIncrement;	//< FIXME added by kiran
	PRINT_VAR(pStateCBR->intraPeriod);
	/* +Change start for ER350503 MPEG4Enc for wrong I frame Interval */
	if(mp4_par_in->flag_short_header == 1)
	{
	pStateCBR->intraPeriod = (((mp4_custom_par_in->no_of_p_frames * 30000)*1001)/(1001*((mp4_par_in->framerate)>>10)));//<Change for H263
	}
	else
	/* -Change start for ER350503 MPEG4Enc for wrong I frame Interval */
	{
	pStateCBR->intraPeriod = mp4_custom_par_in->no_of_p_frames * pStateCBR->fixedVopTimeIncrement;	//< FIXME added by kiran
	}
	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc pStateCBR->intraPeriod -%x",pStateCBR->intraPeriod);
	if(mp4_par_in->flag_short_header == 1)
	{
		pts = (pts + 1500) / 3003;
		seconds = (pts * 1001) / 30000;
		modulo = (pts * 1001) % 30000;
		PRINT_VAR(pStateCBR->prevVopTimeIncrement);
		PRINT_VAR(pStateCBR->oldModuloTimeBase);
	}
	/*Change start for 417264 since initial timestamp could be non-zero*/
	else
	{
			seconds = pts / 90000;
			modulo = pts % 90000;
			modulo = (modulo * pStateCBR->vopTimeIncrementResolution) / 90000;
	}
	pStateCBR->prevVopTimeIncrement = (t_sint16)modulo;
	pStateCBR->oldModuloTimeBase = (t_uint16)seconds;
	/*Change end for 417264 since initial timestamp could be non-zero*/
	//pStateCBR->intraPeriod = ((pStateCBR->intraPeriod/1001)*1001);
	pStateCBR->deltaTicks = 0;
	pStateCBR->prevStrategicSkip = 0;
	pStateCBR->govFlag = 0;

	PRINT_VAR(pStateCBR->bufferDepletion);

#ifdef LOGS_ENABLED
	printf("Mpeg4enc brc:  brc output params\n");
#endif

	/*hamac brc value*/
	pbrc_par_out->pictureCodingType = I_TYPE;
	pbrc_par_out->quant = 0;	/*not use in cbr*/
	pbrc_par_out->brcType = SVA_BRC_CBR;
	pbrc_par_out->brcFrameTarget = 0; /*not use in cbr*/
	pbrc_par_out->brcTargetMinPred = pStateCBR->bufferDepletion;

	PRINT_VAR(pStateCBR->maxBufferLevel);
	PRINT_VAR(pStateCBR->sMax);
	OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc pStateCBR->maxBufferLevel -%x ,pStateCBR->sMax -%x",pStateCBR->maxBufferLevel,pStateCBR->sMax);
	if(mp4_par_in->flag_short_header == 1)
	{
		pbrc_par_out->brcTargetMaxPred = 4 * pStateCBR->maxBufferLevel + pStateCBR->sMax + pStateCBR->picTarget;
	}
	else
	{
		PRINT_VAR(mp4_custom_par_in->vbv_occupancy<<6);
		PRINT_VAR(pStateCBR->picTarget);
		PRINT_VAR(pStateCBR->picTarget);
		pbrc_par_out->brcTargetMaxPred = (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6) + pStateCBR->picTarget;
	}

	PRINT_VAR(pbrc_par_out->brcTargetMinPred);
	PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
	PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
	
	//< fill inout params

	bitsPerPixel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateCBR->frameRate) << 10) * 100 /
						(SVP_SHAREDMEM_FROM16(mp4_par_in->window_width) * SVP_SHAREDMEM_FROM16(mp4_par_in->window_height));

/*
	PRINT_VAR(SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate));
	PRINT_VAR(pStateCBR->frameRate);
	PRINT_VAR((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateCBR->frameRate));
	PRINT_VAR(((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateCBR->frameRate) << 10));
	PRINT_VAR(((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateCBR->frameRate) << 10) * 100);
	PRINT_VAR((SVP_SHAREDMEM_FROM16(mp4_par_in->window_width) * SVP_SHAREDMEM_FROM16(mp4_par_in->window_height)));

	PRINT_VAR(bitsPerPixel);
*/
	if(bitsPerPixel <= 2)
		viqp = 24;
	else if(bitsPerPixel > 2 && bitsPerPixel <= 4)
		viqp = 16;
    else if(bitsPerPixel > 4 && bitsPerPixel <= 6)
		viqp = 12;
	else if(bitsPerPixel > 6 && bitsPerPixel <= 10)
		viqp = 8;
	else
		viqp = 4;
	/*+change start for ER - 351300 strange blocks*/
		viqp = 16;
	/*-change start for ER - 351300 strange blocks*/	
	//< fill inout parameters

#ifdef VIQP 		// for variable initial Qp; based on firmware's calculation of the Qp parameter
	mp4_par_inout->I_Qp 			= viqp;
	mp4_par_inout->P_Qp 			= viqp;
#else
	mp4_par_inout->I_Qp 			= 16;
	mp4_par_inout->P_Qp 			= 16;
#endif

	mp4_par_inout->BPPprev = 64;
	mp4_par_inout->ts_modulo_old = pStateCBR->initTsModuloOld;
	mp4_par_inout->BUFFER_depletion = SVP_SHAREDMEM_TO16(pStateCBR->bufferDepletion);
	mp4_par_inout->gov_flag = 0;
	mp4_par_inout->buffer_fullness = SVP_SHAREDMEM_TO16(pStateCBR->bufferDepletion);
	mp4_par_inout->bitstream_size = SVP_SHAREDMEM_TO16(pStateCBR->bufferDepletion);
	mp4_par_inout->PictQpSumIntra = 0;


	pbrc_par_out->skipCount = 0;
	pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
	pbrc_par_out->frameRate = pStateCBR->frameRate;
	pbrc_par_out->deltaTarget = pStateCBR->targetBuffLevel - pStateCBR->buffer + pStateCBR->bufferDepletion;    
	pbrc_par_out->deltaTarget = 0;
	pbrc_par_out->minQp = 2;
	pbrc_par_out->maxQp = 31;
	pbrc_par_out->vopTimeIncrementResolution = pStateCBR->vopTimeIncrementResolution;
	pbrc_par_out->fixedVopTimeIncrement = pStateCBR->fixedVopTimeIncrement;
	pbrc_par_out->smax = pStateCBR->sMax;
	pbrc_par_out->minBaseQuality = 0;/*not use in cbr*/
	pbrc_par_out->minFrameRate = 0;/*not use in cbr*/
	pbrc_par_out->maxBuffLevel = pStateCBR->maxBufferLevel;
/*Change start for 370949*/
	pbrc_par_out->tsSeconds = seconds;
	pbrc_par_out->tsModulo = modulo;
/*Change end for 370949*/
	pbrc_par_out->firstISkippedFlag = 0;/*not use in cbr*/
	pbrc_par_out->initTsModuloOld = 0;//>pStateCBR->initTsModuloOld;

	/* save brcOut */
	pStateCBR->saveBrcOut = *pbrc_par_out;
	*buffer_depletion = pStateCBR->bufferDepletion;
	/*init dataFifo*/
	eotFifo.eotData[0].bitstreamSizeInBits = 0;
	eotFifo.eotData[0].bufferFullness = 0;
	eotFifo.eotData[0].skipPrev = 0;
	eotFifo.eotData[0].skipCurrent = 0;
	eotFifo.ptrWrite = 1;

	pStateCBR->pictureCounter++;
	pStateCBR->deltaTimeStamp = ((t_sint16)(-pStateCBR->initTsModuloOld))/(t_sint16)pStateCBR->fixedVopTimeIncrement; 

	PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
	PRINT_VAR(pStateCBR->deltaTimeStamp);
	OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc pbrc_par_out->brcTargetMaxPred -%x ,pStateCBR->deltaTimeStamp -%x",pbrc_par_out->brcTargetMaxPred,pStateCBR->deltaTimeStamp);
	/* not exactely the same frameRate */
	//J/pStateCBR->frameRate = (t_uint16)(((mp4_par_in->vop_time_increment_resolution) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment)<<10;

	pStateCBR->prevPts = pts;	//< FIXME take care of timestamps

#ifdef LOGS_ENABLED
	printf("Mpeg4enc brc:  CBR_InitSeq() exit\n");
#endif
	return;
}

void COMP_BRC(VBR_InitSeq)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion)
{
	//>t_sint32 bufferLevel;
	t_uint16 i;
	t_uint32 bitsPerPixel;
	t_uint16 viqp;
	/*Change start for 370949*/
	t_uint32 seconds;
	t_uint32 modulo;
	/*Change end for 370949*/
	OstTraceFiltInst0(TRACE_FLOW, "In VBR_InitSeq");
	/* pts correction */
	pts = pts - pStateVBR->ptsCor;

	*pIsPreviousSkip = FALSE;

	/*init hcl specific variable*/
	pStateVBR->skipCount[0] = 0;
	pStateVBR->skipCount[1] = 0;
	pStateVBR->pictureCodingType[0] = I_TYPE;
	pStateVBR->pictureCodingType[1] = I_TYPE;
	pStateVBR->brcTargetMinPred[0] = 0;
	pStateVBR->brcTargetMinPred[1] = 0;
	pStateVBR->ptsCor = 0;
	pStateVBR->skipPrevCount = 0;

	/* MaSaVBR_InitSeq() code*/
	/*VBR host variable*/
	/* +Change as framerate is truncated due to typecasting of t_uint16 otherwise wrong framerate is passed to algo leading to incorrect calculations in algo(done for ER - 354700 and ER - 350503)*/
	PRINT_VAR(((t_uint32)mp4_par_in->vop_time_increment_resolution << 10));
	PRINT_VAR((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10));
	PRINT_VAR(mp4_par_in->fixed_vop_time_increment);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq (((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)) - %x",(((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)));
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq ((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)) - %x",((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10)));
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq (mp4_par_in->fixed_vop_time_increment) - %x",(mp4_par_in->fixed_vop_time_increment));
	if(mp4_par_in->flag_short_header == 0)
	{
		//pStateCBR->frameRate = ((t_uint16)((t_uint32)mp4_par_in->vop_time_increment_resolution << 10) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment;
		pStateVBR->frameRate = ((((t_uint32)mp4_par_in->vop_time_increment_resolution) << 10) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment;
	}
	else
	{
		pStateVBR->frameRate = mp4_par_in->framerate;
	}
	/* -Change as framerate is truncated due to typecasting of t_uint16 otherwise wrong framerate is passed to algo leading to incorrect calculations in algo(done for ER - 354700 and ER - 350503)*/
	
	PRINT_VAR(pStateVBR->frameRate);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->frameRate -%x",pStateVBR->frameRate);
	pStateVBR->nextFrameRate = pStateVBR->frameRate;

	pStateVBR->minFrameRate = (t_uint16)SVP_SHAREDMEM_FROM16(mp4_par_in->min_framerate) << 10;

	if(mp4_par_in->flag_short_header == 1)
	{

		pStateVBR->vopTimeIncrementResolution = 30000;
		pStateVBR->fixedVopTimeIncrement = 1001;
	}
	else
	{
		pStateVBR->vopTimeIncrementResolution = SVP_SHAREDMEM_FROM16(mp4_par_in->vop_time_increment_resolution);
		pStateVBR->fixedVopTimeIncrement = mp4_par_in->fixed_vop_time_increment;
	}

	pStateVBR->bufferMod = 0;
	pStateVBR->prevVopTimeIncrement = 0;
	pStateVBR->oldModuloTimeBase = 0;
	PRINT_VAR(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy));
	pStateVBR->maxBufferLevel = BRCMIN(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer),(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6));
	pStateVBR->sMax = SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer) -%x",SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer));
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy) -%x",SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy));
	#ifdef _VBR_IP_QP_CORRELATION
		pStateVBR->targetBuffLevel = pStateVBR->maxBufferLevel/4;
		pStateVBR->targetBuffLevel = BRCMIN(pStateVBR->targetBuffLevel,((pStateVBR->maxBufferLevel+1)>>1));
	#else
		pStateVBR->targetBuffLevel = ((pStateVBR->maxBufferLevel + 1) >> 1);
	#endif

    //dipti:
	//pStateVBR->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateVBR->frameRate >> 1)) / pStateVBR->frameRate;
	pStateVBR->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateVBR->frameRate)<<10)+((((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate))%pStateVBR->frameRate)<<10)+(pStateVBR->frameRate>>1)) /pStateVBR->frameRate;	/* max_buff_level = bitrate/framerate */
	
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->picTarget - %x",pStateVBR->picTarget);
	switch((t_sva_brc_spatial_quality)mp4_custom_par_in->spatial_quality)
	{
		case SVA_SPATIAL_QUALITY_NONE:
			pStateVBR->minBaseQuality = 31;
			break;
		case SVA_SPATIAL_QUALITY_LOW:
			pStateVBR->minBaseQuality = 18;
			break;
		case SVA_SPATIAL_QUALITY_MEDIUM:
			pStateVBR->minBaseQuality = 13;
			break;
		case SVA_SPATIAL_QUALITY_HIGH:
			pStateVBR->minBaseQuality = 8;
			break;
		default:
			//< brcError = SVA_BRC_NOT_SUPPORTED;
			pStateVBR->minBaseQuality = 31;
			break;
	}


	pbrc_par_out->brcTargetMaxPred = BRCMAX( 0, (t_sint32)pStateVBR->maxBufferLevel  );	   //< FIXME t_sint32 -> t_uint32

	PRINT_VAR(pStateVBR->maxBufferLevel);
	PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->maxBufferLevel -%x",pStateVBR->maxBufferLevel);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pbrc_par_out->brcTargetMaxPred -%x",pbrc_par_out->brcTargetMaxPred);
	pStateVBR->buffer = 0;

	pStateVBR->initTsModuloOld = (t_sint16)-((pStateVBR->fixedVopTimeIncrement % pStateVBR->vopTimeIncrementResolution) * 
											 (((t_sint32)pStateVBR->vopTimeIncrementResolution / mp4_par_in->vop_time_increment_resolution + ((pStateVBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment) >> 1)) /
											  (pStateVBR->fixedVopTimeIncrement / mp4_par_in->fixed_vop_time_increment)));

	//>pStateVBR->intraPeriod = mp4_custom_par_in->no_of_p_frames * mp4_par_in->fixed_vop_time_increment;	//< FIXME added by kiran
	//>pStateVBR->intraPeriod = mp4_custom_par_in->no_of_p_frames * pStateVBR->fixedVopTimeIncrement;	//< FIXME added by jayant
	/*Change start for 417264 intra-period calculation taking framerate into consideration for h263 ,as it is taken care of in mpeg4*/
	if(mp4_par_in->flag_short_header == 1)
	{
	pStateVBR->intraPeriod = (((mp4_custom_par_in->no_of_p_frames * 30000)*1001)/(1001*((mp4_par_in->framerate)>>10)));//<Change for H263
	}
	else
	/* -Change start for ER350503 MPEG4Enc for wrong I frame Interval */
	{
	pStateVBR->intraPeriod = mp4_custom_par_in->no_of_p_frames * pStateVBR->fixedVopTimeIncrement;	//< FIXME added by kiran
	}
	/*Change end for 417264 intra-period calculation taking framerate into consideration for h263  ,as it is taken care of in mpeg4*/
	pStateVBR->deltaTicks = ((t_sint16)(-pStateVBR->initTsModuloOld))/(t_sint16)pStateVBR->fixedVopTimeIncrement;
	pStateVBR->bufferDepletion = (pStateVBR->deltaTicks * SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) * pStateVBR->fixedVopTimeIncrement) / pStateVBR->vopTimeIncrementResolution;
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->intraPeriod -%x",pStateVBR->intraPeriod);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->deltaTicks -%x",pStateVBR->deltaTicks);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pStateVBR->bufferDepletion -%x",pStateVBR->bufferDepletion);
	OstTraceFiltInst1(TRACE_FLOW, "In VBR_InitSeq pts -%x",pts);
	/*Change start for 370949*/
	if(mp4_par_in->flag_short_header == 1)
		{
			//>printf("before");
			//>PRINT_VAR(pts);
			pts = (pts + 1500) / 3003;
			//>printf("pts = (pts + 1500) / 3003;\n");
			//>PRINT_VAR(pts);
			seconds = (pts * 1001) / 30000;
			//>PRINT_VAR(pts * 1001);
			modulo = (pts * 1001) % 30000;

		}
		else
		{
			seconds = pts / 90000;
			modulo = pts % 90000;
			modulo = (modulo * pStateVBR->vopTimeIncrementResolution) / 90000;
		}
		pStateVBR->prevVopTimeIncrement = (t_sint16)modulo;
		pStateVBR->oldModuloTimeBase = (t_uint16)seconds;
/*Change end for 370949*/
/*
	PRINT_VAR(mp4_custom_par_in->no_of_p_frames);
	PRINT_VAR(mp4_par_in->fixed_vop_time_increment);
	PRINT_VAR((t_sint16)pStateVBR->fixedVopTimeIncrement);
	PRINT_VAR(pStateVBR->initTsModuloOld);
	PRINT_VAR(-pStateVBR->initTsModuloOld);
	PRINT_VAR((t_sint16)(-pStateVBR->initTsModuloOld));
	PRINT_VAR(pStateVBR->intraPeriod);
	PRINT_VAR(pStateVBR->deltaTicks);
*/
		OstTraceFiltInst2(TRACE_FLOW, "In VBR_InitSeq modulo -%x seconds -%x",modulo,seconds);
	pStateVBR->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);

	bitsPerPixel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate)/pStateVBR->frameRate) << 10) * 100 /
						(SVP_SHAREDMEM_FROM16(mp4_par_in->window_width) * SVP_SHAREDMEM_FROM16(mp4_par_in->window_height));
	
	PRINT_VAR(bitsPerPixel);

	if(bitsPerPixel <= 2)
		viqp = 24;
	else if(bitsPerPixel > 2 && bitsPerPixel <= 4)
		viqp = 16;
    else if(bitsPerPixel > 4 && bitsPerPixel <= 6)
		viqp = 12;
	else if(bitsPerPixel > 6 && bitsPerPixel <= 10)
		viqp = 8;
	else
		viqp = 4;

	//< fill inout parameters

#ifdef VIQP 		// for variable initial Qp; based on firmware's calculation of the Qp parameter
	mp4_par_inout->I_Qp 			= viqp;
	mp4_par_inout->P_Qp 			= viqp;
#else
	mp4_par_inout->I_Qp 			= 8;
	mp4_par_inout->P_Qp 			= 16;
#endif
	mp4_par_inout->min_pict_quality	= SVP_SHAREDMEM_TO16(pStateVBR->minBaseQuality);
	mp4_par_inout->ts_modulo_old	= SVP_SHAREDMEM_TO16(pStateVBR->initTsModuloOld);
	mp4_par_inout->BUFFER_depletion = SVP_SHAREDMEM_TO16(pStateVBR->bufferDepletion);

	for (i=1;i<TS_VECTOR_SIZE;i++)
	{
		/* the array is initialized assuming that there aren't gaps in the timestamps */
		mp4_par_inout->ts_vector[i] = ((TS_VECTOR_SIZE-1)*mp4_par_inout->ts_modulo_old)	- ((i-1)*mp4_par_inout->ts_modulo_old);
	}

	//mp4_par_inout->intra_Qp_flag = 1;

	

	PRINT_VAR(pStateVBR->intraPeriod);
	PRINT_VAR(pStateVBR->deltaTicks);
	PRINT_VAR(pStateVBR->bufferDepletion);
	PRINT_VAR(pStateVBR->bitRateDelayed);

#ifdef LOGS_ENABLED
	printf("Mpeg4enc brc:  brc output params\n");
#endif
	/* hamac brc value */
	pbrc_par_out->pictureCodingType = I_TYPE;
	pbrc_par_out->quant = 0;	/*not use in vbr*/
	pbrc_par_out->brcType = SVA_BRC_VBR;
	pbrc_par_out->brcFrameTarget = 0; /*not use in vbr*/
	pbrc_par_out->brcTargetMinPred = 0;
	pbrc_par_out->skipCount = 0;
	pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
	pbrc_par_out->frameRate = pStateVBR->frameRate;
	pbrc_par_out->deltaTarget = pStateVBR->targetBuffLevel - pStateVBR->buffer;
	pbrc_par_out->deltaTarget = 0;
	pbrc_par_out->minQp = 2; 
	pbrc_par_out->maxQp = 31;
	pbrc_par_out->vopTimeIncrementResolution = pStateVBR->vopTimeIncrementResolution;
	pbrc_par_out->fixedVopTimeIncrement = pStateVBR->fixedVopTimeIncrement;
	pbrc_par_out->smax = pStateVBR->sMax;
	pbrc_par_out->minBaseQuality = pStateVBR->minBaseQuality;
	pbrc_par_out->minFrameRate = pStateVBR->minFrameRate;
	pbrc_par_out->maxBuffLevel = 0;/*not use in vbr*/
/*Change start for 370949*/
	pbrc_par_out->tsSeconds = seconds;
	pbrc_par_out->tsModulo = modulo;
/*Change end for 370949*/
	pbrc_par_out->firstISkippedFlag = 0;
	pbrc_par_out->initTsModuloOld = pStateVBR->initTsModuloOld;

	/* save brcOut */
	pStateVBR->saveBrcOut = *pbrc_par_out;

	/* init dataFifo */
	eotFifo.eotData[0].bitstreamSizeInBits = pStateVBR->bufferDepletion;
	eotFifo.eotData[0].bufferFullness = 0;
	eotFifo.eotData[0].skipPrev = 0;
	eotFifo.eotData[0].skipCurrent = 0;
	eotFifo.ptrWrite = 1;

	*buffer_depletion = pStateVBR->bufferDepletion;

	pStateVBR->pictureCounter++;

	pStateVBR->deltaTimeStamp = ((t_sint16)(-pStateVBR->initTsModuloOld))/(t_sint16)pStateVBR->fixedVopTimeIncrement;

	/* not exactely the same frameRate */
	//J/pStateVBR->frameRate = (t_uint16)(((mp4_par_in->vop_time_increment_resolution) + (mp4_par_in->fixed_vop_time_increment >> 1)) / mp4_par_in->fixed_vop_time_increment)<<10;

	pStateVBR->prevPts = pts;	//< FIXME take care of timestamps
	
	return;
}

/* The first time we enter here we are programming the second frame !! */
/* the first frame is programmed by InitSeq */
void COMP_BRC(NoBRC_InitPict)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip)
{
	t_sint32 bufferLevel;
	t_uint32 ptsDiff;
	t_sva_brc_eot_data *pEotDataNMinus2;
	t_uint32	max_BUFFER_level	= 0;

#ifdef LOGS_ENABLED
	printf("Mpeg4enc brc:  NoBRC_InitPict() enter\n");
#endif

	/* pts correction */
	pts = pts - pStateQpConstant->ptsCor;

	ptsDiff = pts - pStateQpConstant->prevPts;	//<FIXME , take care of timestamps
	pEotDataNMinus2 = &eotFifo.eotData[1-(pStateQpConstant->pictureCounter%2)];

	pStateQpConstant->prevPts = pts;	//<FIXME , take care of timestamps
	/*first detect if we replay a picture*/
	if(ptsDiff == 0)
	{
		*pIsPreviousSkip = TRUE;

		/*need to get back programming*/
		*pbrc_par_out = pStateQpConstant->saveBrcOut;

		/* handle special case of all first pictures skipped */
		/* note that we use pStateQpConstant->pictureCounter - 1 since pStateQpConstant->pictureCounter*/
		/* has already been incremented */
		if(pbrc_par_out->brcType == SVA_BRC_QP_CONSTANT_VBV_ANNEX_G)	//< FIXME
		//< if((mp4_custom_par_in->buffering_model == SVA_BUFFERING_ANNEXG) || (mp4_custom_par_in->buffering_model == SVA_BUFFERING_VBV))
		{
			if(pStateQpConstant->pictureCounter - 1 == pStateQpConstant->skipCount[1] + 1)
			{
				pbrc_par_out->tsSeconds = 0;
				pbrc_par_out->tsModulo = 0;
				pbrc_par_out->firstISkippedFlag = 1;
			}
		}

		/*fix skip count*/
		pStateQpConstant->skipCount[0] = pStateQpConstant->skipCount[1] + 1;
		pbrc_par_out->skipCount = pStateQpConstant->skipCount[0];

		/*need to know if previous was an Intra to fix picture type*/
		/*deltaTicks is reset. Modif in ref v2.4*/
		if(pStateQpConstant->pictureCodingType[1] == I_TYPE)
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
			pbrc_par_out->quant = SVP_SHAREDMEM_FROM16(mp4_custom_par_in->IPictureQp);
		}
		pStateQpConstant->pictureCodingType[0] = pbrc_par_out->pictureCodingType;

		if(pbrc_par_out->brcType == SVA_BRC_QP_CONSTANT_VBV_ANNEX_G)
        //< if((mp4_custom_par_in->buffering_model == SVA_BUFFERING_ANNEXG) || (mp4_custom_par_in->buffering_model == SVA_BUFFERING_ANNEXG))
		{
			if(pStateQpConstant->pictureCodingType[1] == I_TYPE)
			{
				pbrc_par_out->brcTargetMinPred = pStateQpConstant->brcTargetMinPred[1];
			}
			pStateQpConstant->brcTargetMinPred[0] = pStateQpConstant->brcTargetMinPred[1];
		}
	}
	else
	{
		t_uint32 seconds;
		t_uint32 modulo;
		t_uint32 currTicks;
		t_uint32 prevTicks;

		/*handle pts correction variable*/
		if(pEotDataNMinus2->skipPrev == 1 || pStateQpConstant->prevStrategicSkip == 1)
		{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  skikPrev == 1 ||  prevStrategicSkip == 1\n");
		#endif
			pStateQpConstant->skipPrevCount++;
			if(pStateQpConstant->pictureCounter - 1 ==  pStateQpConstant->skipPrevCount)
			{
				/* first pictures have been it skipped */
				pStateQpConstant->ptsCor += (pts - pStateQpConstant->prevPts);
				pts = pStateQpConstant->prevPts;
			}
		}
		pStateQpConstant->prevPts = pts;

		/*run NoBRC_host_PostPict() + NoBRC_HOST_InitPict() code*/
		/* 
		 *  NoBRC_host_PostPict() : this concern picture n-2
		 *  Reprogramming stuff in case of skip is done differently in HCL.
		 */
		/* Normal buffer handling */
		if(pEotDataNMinus2->skipPrev == 1 || pStateQpConstant->prevStrategicSkip == 1)
		{
			pStateQpConstant->buffer = BRCMAX((t_sint32) pStateQpConstant->buffer - (t_sint32) pStateQpConstant->bufferDepletion,0);
		}
		else
		{
			PRINT_VAR((t_sint32) pEotDataNMinus2->bitstreamSizeInBits);
			PRINT_VAR((t_sint32) pStateQpConstant->bufferDepletion);
			pStateQpConstant->buffer = BRCMAX((t_sint32) pStateQpConstant->buffer + (t_sint32) pEotDataNMinus2->bitstreamSizeInBits - (t_sint32) pStateQpConstant->bufferDepletion,0);
		}

		/*retriewe interrupt skip info for picture n-1 if it's already available*/
		if((eotFifo.ptrWrite != (pStateQpConstant->pictureCounter % 2)) && (eotFifo.eotData[1-eotFifo.ptrWrite].skipPrev == 1))
		{
			*pIsPreviousSkip = TRUE;
		}

		/*retriewe strategic skip info for picture n-1*/
		if((mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD) && (pStateQpConstant->buffer >  pStateQpConstant->picTarget))
		{
			*pIsPreviousSkip = TRUE;
			pStateQpConstant->prevStrategicSkip = 1;
		}
		else
		{
			pStateQpConstant->prevStrategicSkip = 0;
		}

		/*maintain skipCount variable*/
		pStateQpConstant->skipCount[1] = pStateQpConstant->skipCount[0];
		if(*pIsPreviousSkip == TRUE)
		{
			pStateQpConstant->skipCount[0] = pStateQpConstant->skipCount[0] + 1;
		}
		else
		{
			pStateQpConstant->skipCount[0] = 0;
		}

		/* NoBRC_HOST_InitPict() code */
		/*
		 * First compute seconds and modulo. Modulo must be in pStateQpConstant->vopTimeIncrementResolution units
		 */

		pStateQpConstant->bufferDepletion = (pStateQpConstant->deltaTimeStamp * pStateQpConstant->bitRateDelayed * pStateQpConstant->fixedVopTimeIncrement + pStateQpConstant->bufferMod) / pStateQpConstant->vopTimeIncrementResolution;
		pStateQpConstant->bufferMod = (t_uint16)(pStateQpConstant->deltaTimeStamp * pStateQpConstant->bitRateDelayed * pStateQpConstant->fixedVopTimeIncrement + pStateQpConstant->bufferMod) % pStateQpConstant->vopTimeIncrementResolution;
		pStateQpConstant->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);

		if(mp4_par_in->flag_short_header == 1)
		{
			PRINT_VAR(pts);
			pts = (pts + 1500) / 3003;
			PRINT_VAR(pts);
			seconds = (pts * 1001) / 30000;
			modulo = (pts * 1001) % 30000;
		}
		else
		{
			//< printf("Mpeg4enc brc:  pts at calc ");
			//< PRINT_VAR(pts);
			seconds = pts / 90000;
			modulo = pts % 90000;
			modulo = (modulo * pStateQpConstant->vopTimeIncrementResolution) / 90000;
		}

		PRINT_VAR(seconds);
		PRINT_VAR(modulo);

		currTicks = ((seconds * pStateQpConstant->vopTimeIncrementResolution) + modulo);
		prevTicks = ((pStateQpConstant->oldModuloTimeBase * pStateQpConstant->vopTimeIncrementResolution) + pStateQpConstant->prevVopTimeIncrement);
		pStateQpConstant->deltaTicks += (currTicks-prevTicks);

		PRINT_VAR(currTicks);
		PRINT_VAR(prevTicks);

		pStateQpConstant->deltaTimeStamp = ((modulo - pStateQpConstant->prevVopTimeIncrement) + (seconds - pStateQpConstant->oldModuloTimeBase) * pStateQpConstant->vopTimeIncrementResolution + (pStateQpConstant->fixedVopTimeIncrement >> 1)) / pStateQpConstant->fixedVopTimeIncrement;

		PRINT_VAR(pStateQpConstant->deltaTimeStamp);

		pStateQpConstant->prevVopTimeIncrement = (t_sint16)modulo;
		pStateQpConstant->oldModuloTimeBase = (t_uint16)seconds;

		if(pbrc_par_out->brcType != 0)
		{
			pStateQpConstant->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateQpConstant->frameRate >> 1)) / pStateQpConstant->frameRate;
		}

		if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
		{
			pStateQpConstant->targetBuffLevel = (3277 * pStateQpConstant->picTarget) >> 15;
		}

		/*fill pbrc_par_out*/
		if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_NONE)
		{
			pbrc_par_out->brcType = SVA_BRC_QP_BUFFERING_NONE;
		}
		else if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
		{
			pbrc_par_out->brcType = SVA_BRC_QP_CONSTANT_HRD;
		}
		else
		{
			pbrc_par_out->brcType = SVA_BRC_QP_CONSTANT_VBV_ANNEX_G;
		}
		pbrc_par_out->brcFrameTarget = 0; /*not use in qp constant*/
		pbrc_par_out->brcTargetMinPred = pStateQpConstant->bufferDepletion - pStateQpConstant->buffer;
		if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
		{
			pStateQpConstant->maxBufferLevel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateQpConstant->frameRate >> 1)) / pStateQpConstant->frameRate;
		}


		if(mp4_custom_par_in->buffering_model == SVA_BUFFERING_HRD)
		{
			pbrc_par_out->brcTargetMaxPred = BRCMAX(0, 4 * (t_sint32)pStateQpConstant->maxBufferLevel + (t_sint32)pStateQpConstant->sMax - (t_sint32)pStateQpConstant->buffer);
		}
		else
		{
			PRINT_VAR((t_sint32)pStateQpConstant->buffer);
			pbrc_par_out->brcTargetMaxPred = BRCMAX(0, (t_sint32)(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6) - (t_sint32)pStateQpConstant->buffer);
		}

		//>Experimental! Taken from reference code
		pbrc_par_out->brcTargetMaxPred = BRCMAX(  0, ( (t_sint32)max_BUFFER_level -(t_sint32)pStateQpConstant->buffer  )  );
		//>end

		pbrc_par_out->skipCount = pStateQpConstant->skipCount[0];
		pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
		pbrc_par_out->frameRate = SVP_SHAREDMEM_FROM16(pStateQpConstant->vopTimeIncrementResolution); //>pStateQpConstant->frameRate;
		pbrc_par_out->deltaTarget = pStateQpConstant->targetBuffLevel - pStateQpConstant->buffer;
		pbrc_par_out->minQp = 2; 
		pbrc_par_out->maxQp = 31;
		pbrc_par_out->vopTimeIncrementResolution = pStateQpConstant->vopTimeIncrementResolution;
		pbrc_par_out->fixedVopTimeIncrement = pStateQpConstant->fixedVopTimeIncrement;
		pbrc_par_out->smax = pStateQpConstant->sMax;
		pbrc_par_out->minBaseQuality = 0; /*not use in cbr*/
		pbrc_par_out->minFrameRate = 0;	/*not use in cbr*/
		pbrc_par_out->maxBuffLevel = pStateQpConstant->maxBufferLevel;
		pbrc_par_out->tsSeconds = pStateQpConstant->oldModuloTimeBase;
		pbrc_par_out->tsModulo = pStateQpConstant->prevVopTimeIncrement;
		pbrc_par_out->firstISkippedFlag = 0;
		pbrc_par_out->initTsModuloOld = pStateQpConstant->initTsModuloOld;
		/* info need to fill vol header
		 * note that these info need only to be provide for an I
		 * and when isSystemHeaderAddBeforeIntra is active. We send
		 * them anyway for each picture.
		*/

		//< pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (pBrcParam->vbvBufferSize >> 14);	//FIXME
		pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_buffer_size) >> 14);

		bufferLevel = (t_sint32)pStateQpConstant->buffer - (t_sint32)pStateQpConstant->bufferDepletion;
		pbrc_par_out->bufferSizeForVbv = bufferLevel;

		PRINT_VAR(pStateQpConstant->deltaTicks);
		PRINT_VAR(pStateQpConstant->intraPeriod);

		/*
		 * pictureCodingType is set in intra in the following case :
		 *      - An intra must be inserted since deltaTicks reach intraPeriod
		 *      - Previous skip picture was an intra. Note that in this
		 *        case deltaTicks IS reset. (Modif in ref. v2.4)
		*/
		if(pStateQpConstant->pictureCodingType[0] == I_TYPE)
		{
			pStateQpConstant->deltaTicks = 0;
		}
		PRINT_VAR(mp4_custom_par_in->force_intra);
		if((pStateQpConstant->deltaTicks >= pStateQpConstant->intraPeriod) || (mp4_custom_par_in->force_intra==1)/*pDesc->isFlagIntraRequest == TRUE*/)
			{
			#ifdef LOGS_ENABLED
				printf("Mpeg4enc brc:  NoBRC deltaTicks >= intraPeriod\n");
			#endif
				pbrc_par_out->pictureCodingType = I_TYPE;
				mp4_custom_par_in->force_intra = 0;/*pDesc->isFlagIntraRequest = FALSE;*/
				
				pbrc_par_out->quant = mp4_custom_par_in->IPictureQp;
			}
			else
			{
				pbrc_par_out->pictureCodingType = P_TYPE;
				pbrc_par_out->quant = mp4_custom_par_in->PPictureQp;
			}

		/*need to fix intra decision*/
		if((*pIsPreviousSkip == TRUE) && (pStateQpConstant->pictureCodingType[0] == I_TYPE))
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
			pbrc_par_out->quant = mp4_custom_par_in->IPictureQp;
		}

		/*need to fix min pred*/
		if(pbrc_par_out->brcType == SVA_BRC_QP_CONSTANT_VBV_ANNEX_G)
		//< if((mp4_custom_par_in->buffering_model == SVA_BUFFERING_VBV) || (mp4_custom_par_in->buffering_model == SVA_BUFFERING_ANNEXG))
		{
			if((*pIsPreviousSkip == TRUE) && (pStateQpConstant->pictureCodingType[0] == I_TYPE))
			{
				pbrc_par_out->brcTargetMinPred = pStateQpConstant->brcTargetMinPred[0];
			}
		}

		/* handle special case of all first pictures skipped */
		if(pbrc_par_out->brcType == SVA_BRC_QP_CONSTANT_VBV_ANNEX_G)
		//< if((mp4_custom_par_in->buffering_model == SVA_BUFFERING_VBV) || (mp4_custom_par_in->buffering_model == SVA_BUFFERING_ANNEXG))	//< BIG FIXME
		{
			if((*pIsPreviousSkip == TRUE) && (pStateQpConstant->pictureCounter == pStateQpConstant->skipCount[0]))
			{
				pbrc_par_out->tsSeconds = 0;
				pbrc_par_out->tsModulo = 0;
				pbrc_par_out->firstISkippedFlag = 1;
			}
		}

		/*save picture type historic*/
		pStateQpConstant->pictureCodingType[1] = pStateQpConstant->pictureCodingType[0];
		pStateQpConstant->pictureCodingType[0] = pbrc_par_out->pictureCodingType;

		/*save min_pred history*/
		pStateQpConstant->brcTargetMinPred[1] = pStateQpConstant->brcTargetMinPred[0];
		pStateQpConstant->brcTargetMinPred[0] = pbrc_par_out->brcTargetMinPred;


		/*save brcOut*/
		pStateQpConstant->saveBrcOut = *pbrc_par_out;

		/*update picture counter*/
		pStateQpConstant->pictureCounter++;
	}
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  NoBRC_InitPict() exit\n");
	#endif

	return;
}
/**Buffer Depletion added to the function argument to pass the value calculated here to the algo**/ 
void COMP_BRC(CBR_InitPict)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion)
{
	t_sint32 bufferLevel;

	t_uint32 ptsDiff = pts - pStateCBR->prevPts; 
	t_sva_brc_eot_data *pEotDataNMinus2 = &eotFifo.eotData[1-(pStateCBR->pictureCounter%2)];

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  CBR_InitPict() enter\n");
	#endif
		OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitPict() enter");
	pStateCBR->prevPts = pts;

	OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  CBR_InitPict() pts -%x",pts);
	/*first detect if we replay a picture*/
	if(ptsDiff == 0)
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  Replay the picture\n");
		#endif

		*pIsPreviousSkip = TRUE;

		/*need to get back programming*/
		*pbrc_par_out = pStateCBR->saveBrcOut;

		/*fix skip count*/
		pStateCBR->skipCount[0] = pStateCBR->skipCount[1] + 1;
		pbrc_par_out->skipCount = pStateCBR->skipCount[0];

		/*need to know if previous was an Intra to fix picture type*/
		/*deltaTicks is reset. Modif in ref v2.4*/
		if(pStateCBR->pictureCodingType[1] == I_TYPE)
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
		}
		pStateCBR->pictureCodingType[0] = pbrc_par_out->pictureCodingType;
	}
	else
	{
		t_uint32 seconds;
		t_uint32 modulo;
		t_uint32 currTicks;
		t_uint32 prevTicks;

		/*run VBR_host_PostPict() + VBR_HOST_InitPict() code*/
		/* 
		 *  VBR_host_PostPict() : this concern picture n-2
		 *  Reprogramming stuff in case of skip is done differently in HCL.
		 */
		/* Normal buffer handling */
		//mp4_par_inout->bitstream_size= pEotDataNMinus2->bitstreamSizeInBits - pEotDataNMinus2->stuffingbits;
		//mp4_par_inout->stuffing_bits = pEotDataNMinus2->stuffingbits;
		//mp4_par_inout->Skip_Current  = pEotDataNMinus2->skipCurrent;
		OstTraceFiltInst3(TRACE_FLOW, "=> Mpeg4enc:buffer -%x,bitsreamsize -%x,bufferdep -%x",(pStateCBR->buffer),(pEotDataNMinus2->bitstreamSizeInBits),(pStateCBR->bufferDepletion));	
		if(pEotDataNMinus2->skipPrev == 1 || pStateCBR->prevStrategicSkip == 1)
		{
			#ifdef LOGS_ENABLED
				printf("Mpeg4enc:  pStateCBR->buffer calculation\n");
			#endif
				OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc:  pStateCBR->buffer calculation");
			pStateCBR->buffer = BRCMAX((t_sint32) (pStateCBR->buffer) - (t_sint32) (pStateCBR->bufferDepletion),0);
		}
		else
		{
			#ifdef LOGS_ENABLED
				printf("Mpeg4enc:  pStateCBR->buffer calculation2\n");
			#endif
				OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc:  pStateCBR->buffer calculation2");
			pStateCBR->buffer = BRCMAX((t_sint32) (pStateCBR->buffer) + (t_sint32) (pEotDataNMinus2->bitstreamSizeInBits) - (t_sint32) (pStateCBR->bufferDepletion),0);
		}

		PRINT_VAR(pStateCBR->buffer);
		OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc:  pStateCBR->buffer - %x",pStateCBR->buffer);
		/* time stamp modification for partly optimal timeStamp. Only for SP */
		if(mp4_par_in->flag_short_header == 0)
		{
			if(pStateCBR->fakeFlag == 1)
			{
				if((pEotDataNMinus2->skipPrev == 0) && (pStateCBR->prevStrategicSkip == 0) &&
				   (pStateCBR->pictureCounter > 2))
				{
					pStateCBR->fakeFlag = 0;
				}
				else if(pStateCBR->pictureCounter > 2)
				{
					pStateCBR->ptsDiff += ptsDiff;
				}
			}
		}

		/* partly optimal timeStamp. Only for SP */
		if(mp4_par_in->flag_short_header == 0)
		{
			if((pStateCBR->prevPictureCodingType == P_TYPE) && (pStateCBR->prevStrategicSkip == 0) && (pStateCBR->govFlag == 0))
			{
				pStateCBR->govFlag = 1;
				pStateCBR->buffer = BRCMAX((t_sint32)(pStateCBR->bufferFakeTs) + (t_sint32) (pEotDataNMinus2->bitstreamSizeInBits) - (t_sint32) (pStateCBR->bufferDepletion),0);
			}

			if((pStateCBR->prevPictureCodingType == I_TYPE) && (pStateCBR->govFlag == 0) && (pEotDataNMinus2->skipPrev == 0) && (pStateCBR->prevStrategicSkip == 0))	/* added compare to ref VI :  5649*/
			{
				pStateCBR->bufferFakeTs = pStateCBR->buffer;
			}
		}

		/*retriewe interrupt skip info for picture n-1 if it's already available*/
		if((eotFifo.ptrWrite != (pStateCBR->pictureCounter % 2)) && (eotFifo.eotData[1-eotFifo.ptrWrite].skipPrev == 1))
		{
			*pIsPreviousSkip = TRUE;
		}
		
		PRINT_VAR(pStateCBR->buffer);
		PRINT_VAR(pStateCBR->picTarget);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateCBR->buffer -%x , pStateCBR->picTarget -%x",pStateCBR->buffer,pStateCBR->picTarget);
		/*retriewe strategic skip info for picture n-1*/
		if(pStateCBR->buffer > pStateCBR->picTarget)
		{
			#ifdef LOGS_ENABLED
				printf("Mpeg4enc brc:  prevstrategicskip\n");
			#endif
				OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  prevstrategicskip");
			*pIsPreviousSkip = TRUE;
			pStateCBR->prevStrategicSkip = 1;
		}
		else
		{
			pStateCBR->prevStrategicSkip = 0;
		}

		/*maintain skipCount variable*/
		pStateCBR->skipCount[1] = pStateCBR->skipCount[0];
		if(*pIsPreviousSkip == TRUE)
		{
			pStateCBR->skipCount[0] = pStateCBR->skipCount[0] + 1;
		}
		else
		{
			pStateCBR->skipCount[0] = 0;
		}

		/* VBR_HOST_InitPict() code */
		/*
		 * First compute seconds and modulo. Modulo must be in pStateCBR->vopTimeIncrementResolution units
		 */

		pStateCBR->bufferDepletion = (pStateCBR->deltaTimeStamp * pStateCBR->bitRateDelayed * pStateCBR->fixedVopTimeIncrement + pStateCBR->bufferMod) / pStateCBR->vopTimeIncrementResolution;
		/*Change start for ER 425400 Extra parenthesis added to avoid truncation*/
		pStateCBR->bufferMod = (t_uint16)((pStateCBR->deltaTimeStamp * pStateCBR->bitRateDelayed * pStateCBR->fixedVopTimeIncrement + pStateCBR->bufferMod) % pStateCBR->vopTimeIncrementResolution);
		/*Change end for ER 425400 Extra parenthesis added to avoid truncation*/
		pStateCBR->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);

		PRINT_VAR(pStateCBR->bufferDepletion);
		PRINT_VAR(pStateCBR->bufferMod);
		PRINT_VAR(pStateCBR->bitRateDelayed);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateCBR->bufferDepletion -%x , pStateCBR->bufferMod -%x",pStateCBR->bufferDepletion,pStateCBR->bufferMod);
		pts -= pStateCBR->ptsDiff;

		if(mp4_par_in->flag_short_header == 1)
		{
			pts = (pts + 1500) / 3003;
			seconds = (pts * 1001) / 30000;
			modulo = (pts * 1001) % 30000;
		}
		else
		{
			seconds = pts / 90000;
			modulo = pts % 90000;
			modulo = (modulo * pStateCBR->vopTimeIncrementResolution) / 90000;
		}

		PRINT_VAR(seconds);
		PRINT_VAR(modulo);
		PRINT_VAR(pStateCBR->vopTimeIncrementResolution);
		PRINT_VAR(pStateCBR->oldModuloTimeBase);
		PRINT_VAR(pStateCBR->prevVopTimeIncrement);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  seconds -%x , modulo - %x",seconds,modulo);
		//currTicks = ((seconds * pStateCBR->vopTimeIncrementResolution) + modulo);
		//prevTicks = ((pStateCBR->oldModuloTimeBase * pStateCBR->vopTimeIncrementResolution) + pStateCBR->prevVopTimeIncrement);
		/*
		 * pictureCodingType is set in intra in the following case :
		 *      - An intra must be inserted since deltaTicks reach intraPeriod
		 *      - Previous skip picture was an intra. Note that in this
		 *        case deltaTicks IS reset. (Modif in ref. v2.4)
		*/
		/* +Change start for ER350503 MPEG4Enc for wrong I frame Interval */		
		if(pStateCBR->pictureCodingType[0] == I_TYPE)
		{
			//pStateCBR->deltaTicks = (currTicks-prevTicks);
			pStateCBR->deltaTicks = ((seconds - pStateCBR->oldModuloTimeBase)*pStateCBR->vopTimeIncrementResolution + (modulo - pStateCBR->prevVopTimeIncrement));
        #ifndef __MPEG4ENC_ARM_NMF
		#endif
		}
		else 
		/* -Change start for ER350503 MPEG4Enc for wrong I frame Interval */
		{
			//pStateCBR->deltaTicks += (currTicks-prevTicks);
			pStateCBR->deltaTicks += ((seconds - pStateCBR->oldModuloTimeBase)*pStateCBR->vopTimeIncrementResolution + (modulo - pStateCBR->prevVopTimeIncrement));
        #ifndef __MPEG4ENC_ARM_NMF
		#endif
		}
		//PRINT_VAR(currTicks);
		//PRINT_VAR(prevTicks);
		////PRINT_VAR(pStateCBR->deltaTicks);
		////PRINT_VAR(pStateCBR->intraPeriod);
		//////PRINT_VAR(pStateCBR->deltaTicks);

		pStateCBR->deltaTimeStamp = ((modulo - pStateCBR->prevVopTimeIncrement) + (seconds - pStateCBR->oldModuloTimeBase) * pStateCBR->vopTimeIncrementResolution + (pStateCBR->fixedVopTimeIncrement >> 1)) / pStateCBR->fixedVopTimeIncrement;
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateCBR->deltaTicks -%x , pStateCBR->deltaTicks - %x",pStateCBR->deltaTicks,pStateCBR->deltaTicks);
		PRINT_VAR(pStateCBR->deltaTimeStamp);

		if(mp4_par_in->flag_short_header == 0)
		{
			if(pStateCBR->govFlag == 1 || pStateCBR->prevStrategicSkip == 0)
			{
				pStateCBR->prevVopTimeIncrement = (t_sint16)modulo;
				pStateCBR->oldModuloTimeBase = (t_uint16)seconds;
			}
		}
		else
		{
			pStateCBR->prevVopTimeIncrement = (t_sint16)modulo;
			pStateCBR->oldModuloTimeBase = (t_uint16)seconds;

			PRINT_VAR(pStateCBR->prevVopTimeIncrement);
			PRINT_VAR(pStateCBR->oldModuloTimeBase);
		}


		pStateCBR->picTarget = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateCBR->frameRate >> 1)) / pStateCBR->frameRate;
		pStateCBR->targetBuffLevel = (3277 * pStateCBR->picTarget) >> 15;

		PRINT_VAR(pStateCBR->picTarget);
		PRINT_VAR(pStateCBR->targetBuffLevel);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateCBR->picTarget -%x , pStateCBR->targetBuffLevel - %x",pStateCBR->picTarget,pStateCBR->targetBuffLevel);
		/*fill pbrc_par_out*/
		pbrc_par_out->quant = 0;	/*not use in cbr*/
		pbrc_par_out->brcType = SVA_BRC_CBR;
		pbrc_par_out->brcFrameTarget = 0; /*not use in cbr*/
		pbrc_par_out->brcTargetMinPred = pStateCBR->bufferDepletion - pStateCBR->buffer;

		if(mp4_par_in->flag_short_header == 1)
		{
			pStateCBR->maxBufferLevel = ((SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate) << 10) + (pStateCBR->frameRate >> 1)) / pStateCBR->frameRate;
		}
		else
		{
			pStateCBR->maxBufferLevel = BRCMIN(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->swis_buffer),SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6);
		}


		if(mp4_par_in->flag_short_header == 1)
		{
			pbrc_par_out->brcTargetMaxPred = BRCMAX(0, 4 * (t_sint32)pStateCBR->maxBufferLevel + (t_sint32)pStateCBR->sMax - (t_sint32)pStateCBR->buffer + (t_sint32)pStateCBR->bufferDepletion);
		}
		else
		{
			pbrc_par_out->brcTargetMaxPred = BRCMAX(0, (t_sint32)(SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6) - (t_sint32)pStateCBR->buffer + (t_sint32)pStateCBR->bufferDepletion);
		}
		
		*buffer_depletion = pStateCBR->bufferDepletion;

		PRINT_VAR(pbrc_par_out->brcTargetMinPred);
		PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pbrc_par_out->brcTargetMinPred -%x , pbrc_par_out->brcTargetMaxPred - %x",pbrc_par_out->brcTargetMinPred,pbrc_par_out->brcTargetMaxPred);
		pbrc_par_out->skipCount = pStateCBR->skipCount[0];
		pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
		pbrc_par_out->frameRate = pStateCBR->frameRate;
		pbrc_par_out->deltaTarget = pStateCBR->targetBuffLevel - pStateCBR->buffer + pStateCBR->bufferDepletion;
		pbrc_par_out->minQp = 2; 
		pbrc_par_out->maxQp = 31;
		pbrc_par_out->vopTimeIncrementResolution = pStateCBR->vopTimeIncrementResolution;
		pbrc_par_out->fixedVopTimeIncrement = pStateCBR->fixedVopTimeIncrement;
		pbrc_par_out->smax = pStateCBR->sMax;
		pbrc_par_out->minBaseQuality = 0; /*not use in cbr*/
		pbrc_par_out->minFrameRate = 0;	/*not use in cbr*/
		pbrc_par_out->maxBuffLevel = pStateCBR->maxBufferLevel;
		pbrc_par_out->tsSeconds = pStateCBR->oldModuloTimeBase;
		pbrc_par_out->tsModulo = pStateCBR->prevVopTimeIncrement;
		pbrc_par_out->firstISkippedFlag = 0;/*not use in cbr*/
		pbrc_par_out->initTsModuloOld = 0;/*not use in cbr*/
		/* info need to fill vol header
		 * note that these info need only to be provide for an I
		 * and when isSystemHeaderAddBeforeIntra is active. We send
		 * them anyway for each picture.
		*/

		//< pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (pBrcParam->vbvBufferSize >> 14);	//FIXME
		pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_buffer_size) >> 14);

		bufferLevel = (t_sint32)pStateCBR->buffer - (t_sint32)pStateCBR->bufferDepletion;
		pbrc_par_out->bufferSizeForVbv = bufferLevel;

		PRINT_VAR(bufferLevel);
		/* save previous picture coding type*/
		pStateCBR->prevPictureCodingType = pStateCBR->pictureCodingType[0];

		//< PRINT_VAR(pStateCBR->prevPictureCodingType);

		PRINT_VAR(pStateCBR->deltaTicks);
		PRINT_VAR(pStateCBR->intraPeriod);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateCBR->deltaTicks - %x,pStateCBR->intraPeriod -%x",pStateCBR->deltaTicks,pStateCBR->intraPeriod);
		
		if(pStateCBR->deltaTicks >= pStateCBR->intraPeriod || mp4_custom_par_in->force_intra/*pDesc->isFlagIntraRequest == TRUE*/)
			{
				#ifdef LOGS_ENABLED
					printf("Mpeg4enc brc:  CBR deltaTicks >= intraPeriod\n");
				#endif
				OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  CBR deltaTicks >= intraPeriod");
				pbrc_par_out->pictureCodingType = I_TYPE;
				mp4_custom_par_in->force_intra = 0;/*pDesc->isFlagIntraRequest = FALSE;*/
			}
			else
			{
				pbrc_par_out->pictureCodingType = P_TYPE;
			}

		PRINT_VAR(*pIsPreviousSkip);
		PRINT_VAR(pStateCBR->pictureCodingType[0]);
		/*need to fix intra decision*/
		if((*pIsPreviousSkip == TRUE) && (pStateCBR->pictureCodingType[0] == I_TYPE))
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
		}

		/*save picture type historic*/
		pStateCBR->pictureCodingType[1] = pStateCBR->pictureCodingType[0];
		pStateCBR->pictureCodingType[0] = pbrc_par_out->pictureCodingType;

		/*save brcOut*/
		pStateCBR->saveBrcOut = *pbrc_par_out;

		/*update picture counter*/
		pStateCBR->pictureCounter++;
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  CBR_InitPict() exit\n");
	#endif

	return;
}

void COMP_BRC(VBR_InitPict)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion)
{
	t_sint32 bufferLevel;
	t_uint32 ptsDiff;
	t_sva_brc_eot_data *pEotDataNMinus2;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  VBR_InitPict() enter\n");
	#endif
		OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  VBR_InitPict() enter  ");
	/* pts correction */
	pts = pts - pStateVBR->ptsCor;

	*pIsPreviousSkip = FALSE;

	ptsDiff = pts - pStateVBR->prevPts; 
	pEotDataNMinus2 = &eotFifo.eotData[1-(pStateVBR->pictureCounter%2)];

	/*first detect if we replay a picture*/
	if(ptsDiff == 0)
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  Replay the picture\n");
		#endif
			OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  Replay the picture");
		*pIsPreviousSkip = TRUE;

		/*need to get back programming*/
		*pbrc_par_out = pStateVBR->saveBrcOut;

		/* handle special case of all first pictures skipped */
		/* note that we use pStateVBR->pictureCounter - 1 since pStateVBR->pictureCounter*/
		/* has already been incremented */
		if(pStateVBR->pictureCounter - 1 == pStateVBR->skipCount[1] + 1)
		{
			pbrc_par_out->tsSeconds = 0;
			pbrc_par_out->tsModulo = 0;
			pbrc_par_out->firstISkippedFlag = 1;
		}

		/*fix skip count*/
		pStateVBR->skipCount[0] = pStateVBR->skipCount[1] + 1;
		pbrc_par_out->skipCount = pStateVBR->skipCount[0];

		/*need to know if previous was an Intra to fix picture type*/
		/*deltaTicks is reset. Modif in ref v2.4*/
		if(pStateVBR->pictureCodingType[1] == I_TYPE)
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
		}
		pStateVBR->pictureCodingType[0] = pbrc_par_out->pictureCodingType;

		/*fix min pred*/
		if(pStateVBR->pictureCodingType[1] == I_TYPE)
		{
			pbrc_par_out->brcTargetMinPred = pStateVBR->brcTargetMinPred[1];
		}
		pStateVBR->brcTargetMinPred[0] = pStateVBR->brcTargetMinPred[1];
	}
	else
	{
		t_uint32 seconds;
		t_uint32 modulo;
		t_uint32 currTicks;
		t_uint32 prevTicks;


		/*retriewe skip info for picture n-1 if it's already available*/
		if((eotFifo.ptrWrite != (pStateVBR->pictureCounter % 2)) &&
		   (eotFifo.eotData[1-eotFifo.ptrWrite].skipPrev == 1))
		{
			*pIsPreviousSkip = TRUE;
		}

		/*update spatial quality in case of dynamic change*/
		switch((t_sva_brc_spatial_quality)mp4_custom_par_in->spatial_quality/*pBrcParam->spatialQuality*/)
		{
			case SVA_SPATIAL_QUALITY_NONE:
				pStateVBR->minBaseQuality = 31;
				break;
			case SVA_SPATIAL_QUALITY_LOW:
				pStateVBR->minBaseQuality = 18;
				break;
			case SVA_SPATIAL_QUALITY_MEDIUM:
				pStateVBR->minBaseQuality = 13;
				break;
			case SVA_SPATIAL_QUALITY_HIGH:
				pStateVBR->minBaseQuality = 8;
				break;
			default:
				//brcError = SVA_BRC_NOT_SUPPORTED;	//< FIXME
				pStateVBR->minBaseQuality = 31;
				break;
		}
		/*maintain skipCount variable*/
		pStateVBR->skipCount[1] = pStateVBR->skipCount[0];
		if(*pIsPreviousSkip == TRUE)
		{
			pStateVBR->skipCount[0] = pStateVBR->skipCount[0] + 1;
		}
		else
		{
			pStateVBR->skipCount[0] = 0;
		}

		/*handle pts correction variable*/
		if(pEotDataNMinus2->skipPrev == 1)
		{
			pStateVBR->skipPrevCount++;
			if(pStateVBR->pictureCounter - 1 ==  pStateVBR->skipPrevCount)
			{
				/* first pictures have been it skipped */
				pStateVBR->ptsCor += (pts - pStateVBR->prevPts);
				pts = pStateVBR->prevPts;
			}
		}
		pStateVBR->prevPts = pts;

		/*run VBR_host_PostPict() + VBR_HOST_InitPict() code*/
		/* 
		 *  VBR_host_PostPict() : this use info from picture n-2
		 *  Reprogramming stuff in case of skip is done differently in HCL.
		 */
		if(pEotDataNMinus2->skipPrev == 1) /*indicate than n-2 has been It skip*/
		{
			pStateVBR->buffer = BRCMAX((t_sint32) pStateVBR->buffer - (t_sint32) pStateVBR->bufferDepletion,0);
		}
		else
		{
			pStateVBR->buffer = BRCMAX((t_sint32) pStateVBR->buffer + (t_sint32) pEotDataNMinus2->bitstreamSizeInBits - (t_sint32) pStateVBR->bufferDepletion,0);
		}

		/* VBR_HOST_InitPict() code */
		/*
		 * First compute seconds and modulo. Modulo must be in pStateVBR->vopTimeIncrementResolution units
		 */
		pStateVBR->bufferDepletion = (pStateVBR->deltaTimeStamp * pStateVBR->bitRateDelayed * pStateVBR->fixedVopTimeIncrement + pStateVBR->bufferMod) / pStateVBR->vopTimeIncrementResolution;
		/*Change start for ER 425400 Extra parenthesis added to avoid truncation*/
		pStateVBR->bufferMod = (t_uint16)((pStateVBR->deltaTimeStamp * pStateVBR->bitRateDelayed * pStateVBR->fixedVopTimeIncrement + pStateVBR->bufferMod) % pStateVBR->vopTimeIncrementResolution);
		/*Change end for ER 425400 Extra parenthesis added to avoid truncation*/
		pStateVBR->bitRateDelayed = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);   

		PRINT_VAR(pStateVBR->bufferDepletion);
		PRINT_VAR(pStateVBR->bufferMod);
		PRINT_VAR(pStateVBR->bitRateDelayed);
		OstTraceFiltInst1(TRACE_FLOW, "=> Mpeg4enc brc:  pStateVBR->buffer -%x",pStateVBR->buffer);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateVBR->bufferDepletion -%x ,pStateVBR->bufferMod - %x",pStateVBR->bufferDepletion,pStateVBR->bufferMod);
		if(mp4_par_in->flag_short_header == 1)
		{
			pts = (pts + 1500) / 3003;
			seconds = (pts * 1001) / 30000;
			modulo = (pts * 1001) % 30000;
		}
		else
		{
			seconds = pts / 90000;
			modulo = pts % 90000;
			modulo = (modulo * pStateVBR->vopTimeIncrementResolution) / 90000;
		}

		PRINT_VAR(seconds);
		PRINT_VAR(modulo);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  seconds -%x,modulo -%x",seconds,modulo);
		PRINT_VAR(pStateVBR->vopTimeIncrementResolution);
		PRINT_VAR(pStateVBR->oldModuloTimeBase);
		PRINT_VAR(pStateVBR->prevVopTimeIncrement);
		/*Change start for ER 417264 modification needed in delta ticks */
		/*
		 * pictureCodingType is set in intra in the following case :
		 *      - An intra must be inserted since deltaTicks reach intraPeriod
		 *      - Previous skip picture was an intra. Note that in this
		 *        case deltaTicks IS reset. (Modif in ref. v2.4)
		*/
		if(pStateVBR->pictureCodingType[0] == I_TYPE)
		{
			/*Change start for 370949*/
			pStateVBR->deltaTicks = ((seconds - pStateVBR->oldModuloTimeBase)*pStateVBR->vopTimeIncrementResolution + (modulo - pStateVBR->prevVopTimeIncrement));
			/*Change end for 370949*/
		}
		else
		/*Change end for ER 417264 modification needed in delta ticks */
		{
/*Change start for 370949*/
//		currTicks = ((seconds * pStateVBR->vopTimeIncrementResolution) + modulo);
//		prevTicks = ((pStateVBR->oldModuloTimeBase * pStateVBR->vopTimeIncrementResolution) + pStateVBR->prevVopTimeIncrement);
		pStateVBR->deltaTicks += ((seconds - pStateVBR->oldModuloTimeBase)*pStateVBR->vopTimeIncrementResolution + (modulo - pStateVBR->prevVopTimeIncrement));
/*Change end for 370949*/
		}
		PRINT_VAR(currTicks);
		PRINT_VAR(prevTicks);
		PRINT_VAR(pStateVBR->deltaTicks);

		pStateVBR->deltaTimeStamp = ((modulo - pStateVBR->prevVopTimeIncrement) + (seconds - pStateVBR->oldModuloTimeBase) * pStateVBR->vopTimeIncrementResolution + (pStateVBR->fixedVopTimeIncrement >> 1)) / pStateVBR->fixedVopTimeIncrement;

		PRINT_VAR(pStateVBR->deltaTimeStamp);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pStateVBR->deltaTimeStamp -%x,pStateVBR->deltaTicks -%x",pStateVBR->deltaTimeStamp,pStateVBR->deltaTicks);
		pStateVBR->prevVopTimeIncrement = (t_sint16)modulo;
		pStateVBR->oldModuloTimeBase = (t_uint16)seconds;

		/*fill pbrc_par_out*/
		pbrc_par_out->quant = 0;	/*not use in vbr*/
		pbrc_par_out->brcType = SVA_BRC_VBR;
		pbrc_par_out->brcFrameTarget = 0; /*not use in vbr*/
		pbrc_par_out->brcTargetMinPred = pStateVBR->bufferDepletion - pStateVBR->buffer;

		pbrc_par_out->brcTargetMaxPred = BRCMAX(0, (t_sint32) (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_occupancy)<<6) - (t_sint32) pStateVBR->buffer);
		PRINT_VAR(pbrc_par_out->brcTargetMinPred);
		PRINT_VAR(pbrc_par_out->brcTargetMaxPred);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:  pbrc_par_out->brcTargetMinPred-%x,pbrc_par_out->brcTargetMaxPred-%x",pbrc_par_out->brcTargetMinPred,pbrc_par_out->brcTargetMaxPred);
		pbrc_par_out->skipCount = pStateVBR->skipCount[0];
		pbrc_par_out->bitRate = SVP_SHAREDMEM_FROM16(mp4_par_in->bit_rate);
		pbrc_par_out->frameRate = pStateVBR->frameRate;
		pbrc_par_out->deltaTarget = pStateVBR->targetBuffLevel - pStateVBR->buffer;            
		pbrc_par_out->minQp = 2;
		pbrc_par_out->maxQp = 31;
		pbrc_par_out->vopTimeIncrementResolution = pStateVBR->vopTimeIncrementResolution;
		pbrc_par_out->fixedVopTimeIncrement = pStateVBR->fixedVopTimeIncrement;
		pbrc_par_out->smax = pStateVBR->sMax;
		pbrc_par_out->minBaseQuality = pStateVBR->minBaseQuality;
		pbrc_par_out->minFrameRate = pStateVBR->minFrameRate;
		pbrc_par_out->maxBuffLevel = 0;/*not use in vbr*/
		pbrc_par_out->tsSeconds = seconds;
		pbrc_par_out->tsModulo = modulo;
		pbrc_par_out->firstISkippedFlag = 0;
		pbrc_par_out->initTsModuloOld = pStateVBR->initTsModuloOld;
		/* info need to fill vol header
		 * note that these info need only to be provide for an I
		 * and when isSystemHeaderAddBeforeIntra is active. We send
		 * them anyway for each picture.
		*/

		//< pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (pBrcParam->vbvBufferSize >> 14);	//FIXME
		pbrc_par_out->vbvBufferSizeIn16384BitsUnit = (SVP_SHAREDMEM_FROM16(mp4_custom_par_in->vbv_buffer_size) >> 14);

		bufferLevel = (t_sint32)pStateVBR->buffer;//> - (t_sint32)pStateVBR->bufferDepletion;
		pbrc_par_out->bufferSizeForVbv = bufferLevel;

		*buffer_depletion = pStateVBR->bufferDepletion;
		/*Change start for ER 417264 modification needed in delta ticks code in previous version removed*/
		/*Change end for ER 417264 modification needed in delta ticks  code in previous version removed*/
		if(pStateVBR->deltaTicks >= pStateVBR->intraPeriod ||  mp4_custom_par_in->force_intra/*pDesc->isFlagIntraRequest == TRUE*/)
			{
				#ifdef LOGS_ENABLED
					printf("Mpeg4enc brc:  VBR deltaTicks >= intraPeriod\n");
				#endif
				OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  VBR deltaTicks >= intraPeriod");
				pbrc_par_out->pictureCodingType = I_TYPE;
				mp4_custom_par_in->force_intra = 0;/*pDesc->isFlagIntraRequest = FALSE;*/
			}
			else
			{
				pbrc_par_out->pictureCodingType = P_TYPE;
			}
			
		PRINT_VAR(*pIsPreviousSkip);
		PRINT_VAR(pStateVBR->pictureCodingType[0]);
		OstTraceFiltInst2(TRACE_FLOW, "=> Mpeg4enc brc:(*pIsPreviousSkip) -%x (pStateVBR->pictureCodingType[0]) -%x",(*pIsPreviousSkip),(pStateVBR->pictureCodingType[0]));
		/*need to fix intra decision*/
		if((*pIsPreviousSkip == TRUE) && (pStateVBR->pictureCodingType[0] == I_TYPE))
		{
			pbrc_par_out->pictureCodingType = I_TYPE;
		}

		/*need to fix min pred*/
		if((*pIsPreviousSkip == TRUE) && (pStateVBR->pictureCodingType[0] == I_TYPE))
		{
			pbrc_par_out->brcTargetMinPred = pStateVBR->brcTargetMinPred[0];
		}

		/* handle special case of all first pictures skipped */
		if((*pIsPreviousSkip == TRUE) && (pStateVBR->pictureCounter == pStateVBR->skipCount[0]))
		{
			pbrc_par_out->tsSeconds = 0;
			pbrc_par_out->tsModulo = 0;
			pbrc_par_out->firstISkippedFlag = 1;
		}

		/*save picture type historic*/
		pStateVBR->pictureCodingType[1] = pStateVBR->pictureCodingType[0];
		pStateVBR->pictureCodingType[0] = pbrc_par_out->pictureCodingType;

		/*save min_pred history*/
		pStateVBR->brcTargetMinPred[1] = pStateVBR->brcTargetMinPred[0];
		pStateVBR->brcTargetMinPred[0] = pbrc_par_out->brcTargetMinPred;

		/*save brcOut*/
		pStateVBR->saveBrcOut = *pbrc_par_out;

		/*update picture counter*/
		pStateVBR->pictureCounter++;
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  VBR_InitPict() exit\n");
	#endif

	return;
}

void COMP_BRC(BRC_InitSeq)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion)
{
	
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  BRC_InitSeq() enter\n");
	#endif
	
	switch(mp4_par_in->brc_type)
	{
		case (BRC_CONST_QP)://constant Qp
			*pIsPreviousSkip = FALSE;
			NoBRC_InitSeq(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip);
			break;
		case (BRC_CBR)://CBR
			*pIsPreviousSkip = FALSE;
			/**Buffer Depletion added to the function argument to pass the value calculated here to the algo**/ 
			CBR_InitSeq(mp4_par_in, mp4_custom_par_in, mp4_par_inout, pts, pbrc_par_out, pIsPreviousSkip, buffer_depletion);
			break;
		case (BRC_VBR)://VBR
			*pIsPreviousSkip = FALSE;
			VBR_InitSeq(mp4_par_in, mp4_custom_par_in, mp4_par_inout, pts, pbrc_par_out, pIsPreviousSkip, buffer_depletion);
			break;
		default:
			*pIsPreviousSkip = FALSE;
			NoBRC_InitSeq(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip);
			break;
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  BRC_InitSeq() exit\n");
	#endif

	return;
}

void COMP_BRC(BRC_InitPict)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion)
{

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  BRC_InitPict() enter\n");
	#endif
		OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4enc brc:  BRC_InitPict() enter");
	switch(mp4_par_in->brc_type)
	{
		case (BRC_CONST_QP)://constant Qp
			*pIsPreviousSkip = FALSE;
			PRINT_VAR(mp4_custom_par_in->force_intra);
            NoBRC_InitPict(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip);
			break;
		case (BRC_CBR)://CBR
			*pIsPreviousSkip = FALSE;
			/**Buffer Depletion added to the function argument to pass the value calculated here to the algo**/ 
			CBR_InitPict(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip, buffer_depletion);
			break;
		case (BRC_VBR)://VBR
			*pIsPreviousSkip = FALSE;
			VBR_InitPict(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip, buffer_depletion);
			break;
		default:
			*pIsPreviousSkip = FALSE;
			NoBRC_InitPict(mp4_par_in, mp4_custom_par_in, pts, pbrc_par_out, pIsPreviousSkip);
			break;
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  BRC_InitPict() exit\n");
	#endif
}

t_sva_ec_save *pCur, *pPrev;
t_sva_ec_save current, previous;

PUT_PRAGMA
void METH(SP_GetNextFrameParamIn)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_uint32 frame_number, t_uint32 *buf_level, t_uint32 last_bitstream_size)
{
	t_bool isPreviousSkip;
	t_bool isPictureReplay;
	t_uint32 buffer_depletion = 0;
	//>t_uint32 i;

	t_sva_brc_out brc_par_out;

	// Initializing brc_par_out structure
	brc_par_out.pictureCodingType = 0;
	brc_par_out.quant = 0;
	brc_par_out.brcType = 0;
	brc_par_out.brcFrameTarget = 0;
	brc_par_out.brcTargetMinPred = 0;
	brc_par_out.brcTargetMaxPred = 0;
	brc_par_out.skipCount = 0;
	brc_par_out.bitRate = 0;
	brc_par_out.frameRate = 0;
	brc_par_out.deltaTarget = 0;
	brc_par_out.minQp = 0;
	brc_par_out.maxQp = 0;
	brc_par_out.vopTimeIncrementResolution = 0;
	brc_par_out.fixedVopTimeIncrement = 0;
	brc_par_out.smax = 0;
	brc_par_out.minBaseQuality = 0;
	brc_par_out.minFrameRate = 0;
	brc_par_out.maxBuffLevel = 0;
	brc_par_out.tsSeconds = 0;
	brc_par_out.tsModulo = 0;
	brc_par_out.firstISkippedFlag = 0;
	brc_par_out.initTsModuloOld = 0;
	brc_par_out.vbvBufferSizeIn16384BitsUnit = 0;
	brc_par_out.bufferSizeForVbv = 0;

	pCur = &current;
	pCur->pts = pts;

	pPrev = &previous;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  SP_GetNextFrameParamIn() enter\n");
	#endif
	OstTraceFiltInst1(TRACE_API, "=> Mpeg4enc brc:  SP_GetNextFrameParamIn() frame_number -%x",frame_number);
	if(frame_number == 1)
	{
		//if frame_number is 1, that means that BRC should reset it's state, and be ready to encode the first frame
		once_global = 1;
	}
	PRINT_VAR(once_global);

	/*get info from brc concerning param in*/
	if(/*pState->pictureCounter == 0*/once_global) //< FIXME 
	{
		once_global = !once_global;

		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  once\n");
		 #endif

		pStateQpConstant = &state_qp_constant;
		pStateCBR = &state_cbr;
		pStateVBR = &state_vbr;

		current.gobFrameId = 1;
		current.pictureCodingType = 0;
		current.pictureNb = 0;
		current.pts = pts;
		current.roundValue = 0;
		BRC_InitSeq(mp4_par_in, mp4_custom_par_in, mp4_par_inout, pts, &brc_par_out, &isPreviousSkip, &buffer_depletion);
	}
	else
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  !once\n");
		#endif
		PRINT_VAR(mp4_custom_par_in->force_intra);
		PRINT_VAR(mp4_custom_par_in->atomic_count);
		BRC_InitPict(mp4_par_in, mp4_custom_par_in, pts, &brc_par_out, &isPreviousSkip, &buffer_depletion);
	}

	/*
	 * Now is the fun part !!!!!!!
	 * At this point we know if previous picture has been skip or not. Moreover by compare pDesc->pts with the
	 * last one push we can detect the case where a picture is programmed twice due to skip interrupt.
	 * We then define pPrev tha point on previous not skip picture info.
	*/

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  fun part begin\n");
	#endif

	if(pCur->pts == skipFifo[0].pts)
	{
		isPictureReplay = TRUE;
	}
	else
	{
		isPictureReplay = FALSE;
	}

	if(isPreviousSkip == TRUE)
	{
		if(isPictureReplay == TRUE)
		{
			pPrev = &skipFifo[2];
		}
		else
		{
			pPrev = &skipFifo[1];
		}
	}
	else
	{
		pPrev = &skipFifo[0];
	}


	if(frame_number == 1)
	{
		//reset the structure saved from the previous call, in case the ports were disabled and enabled
		pPrev->gobFrameId					= 0;
		pPrev->pictureCodingType			= 0;
		pPrev->pictureNb 					= 0;
		pPrev->pts 							= 0;
		pPrev->roundValue 					= 0;
		pPrev->temporalSp.moduloTimeBase 	= 0;
		pPrev->temporalSp.remainForOffset 	= 0;
		pPrev->temporalSp.vopTimeIncrement 	= 0;
		pPrev->temporalSp.vopTimeIncrementBitSize = 0;
	}

#if 0
	PRINT_VAR(pPrev->gobFrameId);
	PRINT_VAR(pPrev->pictureCodingType);
	PRINT_VAR(pPrev->pictureNb);
	PRINT_VAR(pPrev->pts);
	PRINT_VAR(pPrev->roundValue);
	PRINT_VAR(pPrev->temporalSp.moduloTimeBase);
	PRINT_VAR(pPrev->temporalSp.remainForOffset);
	PRINT_VAR(pPrev->temporalSp.vopTimeIncrement);
	PRINT_VAR(pPrev->temporalSp.vopTimeIncrementBitSize);
#endif
#if 0
	PRINT_VAR(pCur->gobFrameId);
	PRINT_VAR(pCur->pictureCodingType);
	PRINT_VAR(pCur->pictureNb);
	PRINT_VAR(pCur->pts);
	PRINT_VAR(pCur->roundValue);
	PRINT_VAR(pCur->temporalSp.moduloTimeBase);
	PRINT_VAR(pCur->temporalSp.remainForOffset);
	PRINT_VAR(pCur->temporalSp.vopTimeIncrement);
	PRINT_VAR(pCur->temporalSp.vopTimeIncrementBitSize);
#endif


	/*compute pictureNb*/
	pCur->pictureNb = pPrev->pictureNb + 1;

	/*compute temporal reference*/
	if(pCur->pictureNb == 0)
	{
		pCur->temporalSp.remainForOffset=0;
		pCur->temporalSp.moduloTimeBase=0;
		pCur->temporalSp.vopTimeIncrement=0;

		pPrev->temporalSp.remainForOffset=0;
		pPrev->temporalSp.moduloTimeBase=0;
		pPrev->temporalSp.vopTimeIncrement=0;

#ifdef SNAPSHOT_DEFINED
		pDesc->isSnapshotNeeded = TRUE;
#endif
	}
	else
	{
		t_uint32 ptsDifference;
		t_uint32 incDifference;

		/*compute pts difference*/
		ptsDifference = pCur->pts - pPrev->pts;

		/*update temporal reference value. Use ptsDifference*/
		/*convert ptsDifference in 90Khz to incDifference in vopTimeIncrementResolutionHz value*/
		incDifference=(ptsDifference * mp4_par_in->vop_time_increment_resolution + pPrev->temporalSp.remainForOffset)/90000;
		pCur->temporalSp.remainForOffset = (ptsDifference * mp4_par_in->vop_time_increment_resolution + pPrev->temporalSp.remainForOffset)%90000;
		/* parly optimal time stamp stuff */
		/*if (pDesc->conf.brcMode == SVA_CBR)*/ if(mp4_par_in->brc_type == BRC_CBR)	   //< FIXME
		{
#ifdef SNAPSHOT_DEFINED
			if(pDesc->isSnapshotNeeded == TRUE)
			{
				pDesc->isSnapshotNeeded =FALSE;
				pDesc->saveVopTimeIncrement = incDifference;
				pDesc->saveRemainForOffset = pCur->temporalSp.remainForOffset;
			}

			else
#endif
				if((pPrev->pictureNb == 0) && (isPreviousSkip == TRUE))
			{
#ifdef VOPTIME_DEFINED
				incDifference = pDesc->saveVopTimeIncrement;
				pCur->temporalSp.remainForOffset = pDesc->saveRemainForOffset;
#endif
			}
		}
		/*compute moduloTimeBase and vopTimeIncrement value*/
		pCur->temporalSp.vopTimeIncrement = pPrev->temporalSp.vopTimeIncrement + incDifference;
		
		/*correct vopTimeIncrement in case of frame skipping */
		if(isPreviousSkip == TRUE)
		{
			if(isPictureReplay == TRUE)
			{
				if(skipFifo[2].temporalSp.vopTimeIncrement > skipFifo[0].temporalSp.vopTimeIncrement)
				{
					while(pCur->temporalSp.vopTimeIncrement >= mp4_par_in->vop_time_increment_resolution)
						pCur->temporalSp.vopTimeIncrement -= mp4_par_in->vop_time_increment_resolution;
				}

			}
			else
			{
				if(skipFifo[1].temporalSp.vopTimeIncrement > skipFifo[0].temporalSp.vopTimeIncrement)
				{
					while(pCur->temporalSp.vopTimeIncrement >= mp4_par_in->vop_time_increment_resolution)
						pCur->temporalSp.vopTimeIncrement -= mp4_par_in->vop_time_increment_resolution;
				}
			}
		}

		pCur->temporalSp.moduloTimeBase = 0;
		while(pCur->temporalSp.vopTimeIncrement >= mp4_par_in->vop_time_increment_resolution)
		{
			pCur->temporalSp.vopTimeIncrement -= mp4_par_in->vop_time_increment_resolution;
			pCur->temporalSp.moduloTimeBase++;
		}
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  fun part end\n");
	
		printf("Mpeg4enc brc:  param fill begin\n");
	#endif

	/*fill brc parameters*/
	mp4_par_in->picture_coding_type = brc_par_out.pictureCodingType;
	pCur->pictureCodingType = brc_par_out.pictureCodingType;
	//< mp4_par_in->quant = brc_par_out.quant;	//FIXME
	//< mp4_par_in->brc_type = brc_par_out.brcType;		//< BIG FIXME
	mp4_par_in->brc_frame_target = SVP_SHAREDMEM_TO16(brc_par_out.brcFrameTarget);
	mp4_par_in->brc_target_min_pred = SVP_SHAREDMEM_TO16(brc_par_out.brcTargetMinPred);
	mp4_par_in->brc_target_max_pred = SVP_SHAREDMEM_TO16(brc_par_out.brcTargetMaxPred);
	//   pMpeg4ParamIn->skip_count=pDesc->brcOut.skipCount;
	mp4_par_in->skip_count = brc_par_out.skipCount;					//changed for brc
	mp4_par_in->bit_rate = SVP_SHAREDMEM_TO16(brc_par_out.bitRate);
	mp4_par_in->ts_modulo = SVP_SHAREDMEM_TO16((t_short_value)brc_par_out.tsModulo);
	mp4_par_in->ts_seconds = (t_ushort_value)brc_par_out.tsSeconds;
	mp4_par_in->framerate = brc_par_out.frameRate;
	mp4_par_in->delta_target = SVP_SHAREDMEM_TO16(brc_par_out.deltaTarget);
	mp4_par_in->minQp = brc_par_out.minQp;
	mp4_par_in->maxQp = brc_par_out.maxQp;
	mp4_par_in->vop_time_increment_resolution = SVP_SHAREDMEM_TO16(brc_par_out.vopTimeIncrementResolution);
	mp4_par_in->fixed_vop_time_increment = brc_par_out.fixedVopTimeIncrement;
	PRINT_VAR(brc_par_out.smax);
	mp4_par_in->Smax = SVP_SHAREDMEM_TO16(brc_par_out.smax);
	PRINT_VAR(mp4_par_in->Smax);
	mp4_par_in->min_base_quality = brc_par_out.minBaseQuality;
	mp4_par_in->min_framerate = brc_par_out.minFrameRate;
	mp4_par_in->max_buff_level = SVP_SHAREDMEM_TO16(brc_par_out.maxBuffLevel);
	mp4_par_in->first_I_skipped_flag = brc_par_out.firstISkippedFlag;
	mp4_par_in->init_ts_modulo_old = SVP_SHAREDMEM_TO16(brc_par_out.initTsModuloOld);
	/*Change start for 370949*/
	mp4_par_inout->BUFFER_depletion = SVP_SHAREDMEM_TO16(buffer_depletion);
	/*Change end for 370949*/
	// Used to write Vbv_occupancy in the header
	
	if( isPreviousSkip )
	{
		*buf_level =  BRCMAX( (t_sint32)brc_par_out.bufferSizeForVbv - (t_sint32)buffer_depletion, 0);
	}
	else
	{
		*buf_level =  BRCMAX( (t_sint32)brc_par_out.bufferSizeForVbv + (t_sint32)(last_bitstream_size) - (t_sint32)buffer_depletion, 0);
	}
	//< commented the (mp4_par_in->gob_header_freq != 0) check as ref code was not doing that check,
	//< this is just to make the params passed to fw, match with the ref code
	if((pPrev->pictureCodingType != pCur->pictureCodingType)/* && (mp4_par_in->gob_header_freq != 0)*/)
	{
		pCur->gobFrameId = (pPrev->gobFrameId+1)&0x3;
	}
	mp4_par_in->gob_frame_id = pCur->gobFrameId;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  param fill end\n");
	#endif

	mp4_custom_par_in->force_intra = 0;/*pDesc->isFlagIntraRequest = FALSE;*/

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  skipfifo begin\n");
	#endif

	/*shift fifo*/
	if(isPreviousSkip == TRUE)
	{
		if(isPictureReplay == TRUE)
		{
			skipFifo[1] = skipFifo[2];
		}
	}
	else
	{
		skipFifo[2] = skipFifo[1];
		skipFifo[1] = skipFifo[0];
	}
	skipFifo[0] = *pCur;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  skipfifo end\n");
		printf("Mpeg4enc brc:  SP_GetNextFrameParamIn() exit\n");
	#endif
}

PUT_PRAGMA
void METH(SH_GetNextFrameParamIn)(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_uint32 frame_number)
{
	t_bool isPreviousSkip;
	t_bool isPictureReplay;
	t_uint32 buffer_depletion;
	//>t_uint32 i;

	t_sva_brc_out brc_par_out;

	// Initializing brc_par_out structure
	brc_par_out.pictureCodingType = 0;
	brc_par_out.quant = 0;
	brc_par_out.brcType = 0;
	brc_par_out.brcFrameTarget = 0;
	brc_par_out.brcTargetMinPred = 0;
	brc_par_out.brcTargetMaxPred = 0;
	brc_par_out.skipCount = 0;
	brc_par_out.bitRate = 0;
	brc_par_out.frameRate = 0;
	brc_par_out.deltaTarget = 0;
	brc_par_out.minQp = 0;
	brc_par_out.maxQp = 0;
	brc_par_out.vopTimeIncrementResolution = 0;
	brc_par_out.fixedVopTimeIncrement = 0;
	brc_par_out.smax = 0;
	brc_par_out.minBaseQuality = 0;
	brc_par_out.minFrameRate = 0;
	brc_par_out.maxBuffLevel = 0;
	brc_par_out.tsSeconds = 0;
	brc_par_out.tsModulo = 0;
	brc_par_out.firstISkippedFlag = 0;
	brc_par_out.initTsModuloOld = 0;
	brc_par_out.vbvBufferSizeIn16384BitsUnit = 0;
	brc_par_out.bufferSizeForVbv = 0;


	pCur = &current;
	pCur->pts = pts;

	pPrev = &previous;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  SH_GetNextFrameParamIn() enter\n");
	#endif
		OstTraceFiltInst1(TRACE_API, "=> Mpeg4enc brc:  SP_GetNextFrameParamIn() frame_number -%x",frame_number);
	/*get info from brc concerning param in*/
	if(/*pState->pictureCounter == 0*/once_global) //< FIXME 
	{
		once_global = !once_global;

		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  once\n");
		#endif

		pStateQpConstant = &state_qp_constant;
		pStateCBR = &state_cbr;
		pStateVBR = &state_vbr;

		pCur->gobFrameId = 1;
		pCur->pictureCodingType = 0;
		pCur->pictureNb = 0;
		pCur->pts = pts;
		pCur->roundValue = 0;

		BRC_InitSeq(mp4_par_in, mp4_custom_par_in, mp4_par_inout, pts, &brc_par_out, &isPreviousSkip, &buffer_depletion);
	}
	else
	{
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  !once\n");
		#endif

		PRINT_VAR(mp4_custom_par_in->force_intra);
		PRINT_VAR(mp4_custom_par_in->atomic_count)
        BRC_InitPict(mp4_par_in, mp4_custom_par_in, pts, &brc_par_out, &isPreviousSkip, &buffer_depletion);
	}

	/*
	 * Now is the fun part !!!!!!!
	 * At this point we know if previous picture has been skip or not. Moreover by compare pDesc->pts with the
	 * last one push we can detect the case where a picture is programmed twice due to skip interrupt.
	 * We then define pPrev tha point on previous not skip picture info.
	*/

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  fun part begin\n");	
	#endif

	if(((unsigned)pCur->pts == (unsigned)skipFifo[0].pts) && ((unsigned)pCur->pictureNb != ~0))
	{
		isPictureReplay = TRUE;
	}
	else
	{
		isPictureReplay = FALSE;
	}

	if(isPreviousSkip == TRUE)
	{
		if(isPictureReplay == TRUE)
		{
			pPrev = &skipFifo[2];
		}
		else
		{
			pPrev = &skipFifo[1];
		}
	}
	else
	{
		pPrev = &skipFifo[0];
	}

	if(frame_number == 1)
	{
		//reset the structure saved from the previous call, in case the ports were disabled and enabled
		pPrev->gobFrameId					= 0;
		pPrev->pictureCodingType			= 0;
		pPrev->pictureNb 					= 0;
		pPrev->pts 							= 0;
		pPrev->roundValue 					= 0;
		pPrev->temporalSh.cumulTimeSlot 	= 0;
		pPrev->temporalSh.slotDelay 	= 0;
		pPrev->temporalSh.tr 	= 0;
	}

#if 0
	PRINT_VAR(pPrev->gobFrameId);
	PRINT_VAR(pPrev->pictureCodingType);
	PRINT_VAR(pPrev->pictureNb);
	PRINT_VAR(pPrev->pts);
	PRINT_VAR(pPrev->roundValue);
	PRINT_VAR(pPrev->temporalSp.moduloTimeBase);
	PRINT_VAR(pPrev->temporalSp.remainForOffset);
	PRINT_VAR(pPrev->temporalSp.vopTimeIncrement);
	PRINT_VAR(pPrev->temporalSp.vopTimeIncrementBitSize);
#endif
#if 0
	PRINT_VAR(pCur->gobFrameId);
	PRINT_VAR(pCur->pictureCodingType);
	PRINT_VAR(pCur->pictureNb);
	PRINT_VAR(pCur->pts);
	PRINT_VAR(pCur->roundValue);
	PRINT_VAR(pCur->temporalSp.moduloTimeBase);
	PRINT_VAR(pCur->temporalSp.remainForOffset);
	PRINT_VAR(pCur->temporalSp.vopTimeIncrement);
	PRINT_VAR(pCur->temporalSp.vopTimeIncrementBitSize);
#endif


	/*compute pictureNb*/
	pCur->pictureNb = pPrev->pictureNb + 1;

	/*compute temporal reference*/
	if(pCur->pictureNb == 0)
	{
		pCur->temporalSh.tr = 0;
		pCur->temporalSh.cumulTimeSlot = 0;
		pCur->temporalSh.slotDelay = 0;

		pPrev->temporalSh.tr = 0;
		pPrev->temporalSh.cumulTimeSlot = 0;
		pPrev->temporalSh.slotDelay = 0;

	}
	else
	{
		t_uint32 ptsDifference;

		/*compute pts difference*/
		ptsDifference = pCur->pts - pPrev->pts;

		/*update tr. Use ptsDifference*/
		pCur->temporalSh.tr = ((ptsDifference + pPrev->temporalSh.cumulTimeSlot + MP4_SH_ROUND_VALUE) / MP4_SH_CLOCK_SLOT + pPrev->temporalSh.tr)&0xff;
		pCur->temporalSh.cumulTimeSlot = (ptsDifference + pPrev->temporalSh.cumulTimeSlot + MP4_SH_ROUND_VALUE) % MP4_SH_CLOCK_SLOT;
		pCur->temporalSh.cumulTimeSlot -= MP4_SH_ROUND_VALUE;

		/*rounding of tr*/
		/*pCur->temporalSh.slotDelay = pPrev->temporalSh.slotDelay;
		if (pCur->temporalSh.cumulTimeSlot !=0 && pCur->temporalSh.slotDelay == 0)
		{
			pCur->temporalSh.tr++;
			pCur->temporalSh.slotDelay = 1;
		}
		else if (pCur->temporalSh.cumulTimeSlot == 0 && pCur->temporalSh.slotDelay !=0)
		{
			pCur->temporalSh.tr--;
			pCur->temporalSh.slotDelay = 0;
		}*/
	}

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  fun part end\n");

		printf("Mpeg4enc brc:  param fill begin\n");
	#endif

	/*fill brc parameters*/
	mp4_par_in->picture_coding_type=brc_par_out.pictureCodingType;
	pCur->pictureCodingType = brc_par_out.pictureCodingType;
	//< mp4_par_in->quant = brc_par_out.quant;	//FIXME
	//< mp4_par_in->brc_type = brc_par_out.brcType;	//< BIG FIXME
	mp4_par_in->brc_frame_target = SVP_SHAREDMEM_TO16(brc_par_out.brcFrameTarget);
	mp4_par_in->brc_target_min_pred = SVP_SHAREDMEM_TO16(brc_par_out.brcTargetMinPred);
	mp4_par_in->brc_target_max_pred = SVP_SHAREDMEM_TO16(brc_par_out.brcTargetMaxPred);
	mp4_par_in->skip_count = brc_par_out.skipCount;
	mp4_par_in->bit_rate = SVP_SHAREDMEM_TO16(brc_par_out.bitRate);
	mp4_par_in->ts_modulo = (t_short_value)brc_par_out.tsModulo;
	mp4_par_in->ts_seconds = (t_ushort_value)brc_par_out.tsSeconds;
	mp4_par_in->framerate = brc_par_out.frameRate;
	mp4_par_in->delta_target = SVP_SHAREDMEM_TO16(brc_par_out.deltaTarget);
	mp4_par_in->minQp = brc_par_out.minQp;
	mp4_par_in->maxQp = brc_par_out.maxQp;
	mp4_par_in->vop_time_increment_resolution = SVP_SHAREDMEM_TO16(brc_par_out.vopTimeIncrementResolution);
	mp4_par_in->fixed_vop_time_increment = brc_par_out.fixedVopTimeIncrement;
	PRINT_VAR(brc_par_out.smax);
	mp4_par_in->Smax = SVP_SHAREDMEM_TO16(brc_par_out.smax);
	PRINT_VAR(mp4_par_in->Smax);
	mp4_par_in->min_base_quality = brc_par_out.minBaseQuality;
	mp4_par_in->min_framerate = brc_par_out.minFrameRate;
	mp4_par_in->max_buff_level = SVP_SHAREDMEM_TO16(brc_par_out.maxBuffLevel);
	mp4_par_in->first_I_skipped_flag = brc_par_out.firstISkippedFlag;
	mp4_par_in->init_ts_modulo_old = SVP_SHAREDMEM_TO16(brc_par_out.initTsModuloOld);
	/*Change start for 370949*/
	mp4_par_inout->BUFFER_depletion = SVP_SHAREDMEM_TO16(buffer_depletion);
	/*Change end for 370949*/
	//< commented the (mp4_par_in->gob_header_freq != 0) check as ref code was not doing that check,
	//< this is just to make the params passed to fw, match with the ref code
    if((pPrev->pictureCodingType != pCur->pictureCodingType) /*&& (mp4_par_in->gob_header_freq != 0)*/)
	{
		pCur->gobFrameId = (pPrev->gobFrameId+1)&0x3;
	}
	mp4_par_in->gob_frame_id = pCur->gobFrameId;


#if 0
	/*fill mpeg4 short header parameters*/

	if(pMpeg4ParamIn->flag_short_header == TRUE)
	{
		if(pMpeg4ParamIn->vp_bit_size != 0)//segmented mode
		{
			pMpeg4ParamIn->minQp = 4; //CR 155 must be forced
			pMpeg4ParamIn->vp_size_type = 0; //CR 155 must be forced

		}
	}

	pCur->roundValue = pMpeg4ParamIn->rounding_type;

#endif

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  param fill end\n");
	#endif

	mp4_custom_par_in->force_intra = 0;/*pDesc->isFlagIntraRequest = FALSE;*/

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  skipfifo begin\n");
	#endif

	/*shift fifo*/
	if(isPreviousSkip == TRUE)
	{
		if(isPictureReplay == TRUE)
		{
			skipFifo[1] = skipFifo[2];
		}
	}
	else
	{
		skipFifo[2] = skipFifo[1];
		skipFifo[1] = skipFifo[0];
	}
	skipFifo[0] = *pCur;

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  skipfifo end\n");
		printf("Mpeg4enc brc:  SH_GetNextFrameParamIn() exit\n");
	#endif
}

#if 0
#define sva_EC_BRC_IsPreviousPictureWasStrategicSkipCbr METH(sva_EC_BRC_IsPreviousPictureWasStrategicSkipCbr)
#pragma force_dcumode
void sva_EC_BRC_IsPreviousPictureWasStrategicSkipCbr()
{
	//< t_sva_brc_descriptor *pDesc = &brcDesc[instanceNum];
	//, t_sva_brc_cbr_state *pState = &pDesc->cbrState;
	t_uint32 buffer=0; 
	buffer = buffer;/*For removing compiler warning*/

	if(pStateCBR->pictureCounter == 0)
	{
		//< return FALSE;	//FIXME
	}
	else
	{
		t_sva_brc_eot_data *pEotDataNMinus2 = &eotFifo.eotData[1-(pStateCBR->pictureCounter%2)];

		/* compute buffer level*/
		if(pEotDataNMinus2->skipPrev == 1 || 
		   pStateCBR->prevStrategicSkip == 1)
		{
			buffer = BRCMAX((t_sint32) pStateCBR->buffer - (t_sint32) pStateCBR->bufferDepletion,0);
		}
		else
		{
			buffer = BRCMAX((t_sint32) pStateCBR->buffer + (t_sint32) pEotDataNMinus2->bitstreamSizeInBits - (t_sint32) pStateCBR->bufferDepletion,0);
		}

		/* handle partly optimal time stamp case */
		if(/*pDesc->mp4Conf.flagShortHeader == FALSE*/0)   //< FIXME
		{
			if((pStateCBR->prevPictureCodingType == P_TYPE) && (pStateCBR->prevStrategicSkip == 0) && (pStateCBR->govFlag == 0))
			{
				buffer = BRCMAX((t_sint32)pStateCBR->bufferFakeTs + (t_sint32) pEotDataNMinus2->bitstreamSizeInBits - (t_sint32) pStateCBR->bufferDepletion,0);
			}
		}

#define SVA_ENABLE_BRC_HCL_WORKAROUND_VI10315
#if defined(SVA_ENABLE_BRC_HCL_WORKAROUND_VI10315) /* Sarvesh: Temporary HCL workaround for parsing error, VI10315 */
		/* Always return FALSE i.e. No frame is skipped, even if skipped return FALSE */
		//< return FALSE;	//FIXME
#else /* else of #if SVA_ENABLE_BRC_HCL_WORKAROUND_VI10315 */
		/* check if previous picture has been strategic skipped */
		if(buffer > (t_sint32) pState->picTarget)
		{
			//< return TRUE;	//FIXME
		}
		else
		{
			//< return FALSE;	//FIXME
		}
#endif /* endif of #if SVA_ENABLE_BRC_HCL_WORKAROUND_VI10315 */
	}
}
#endif

#if 0
#define sva_EC_BRC_IsPreviousPictureWasStrategicSkipQpConstantHrd METH(sva_EC_BRC_IsPreviousPictureWasStrategicSkipQpConstantHrd)
#pragma force_dcumode
void sva_EC_BRC_IsPreviousPictureWasStrategicSkipQpConstantHrd()
{
	//< t_sva_brc_descriptor *pDesc = &brcDesc[instanceNum];
	//< t_sva_brc_qpConstant_state *pState; //< = &pDesc->qpConstantState;		//FIXME
	t_uint32 buffer;

	if(state_qp_constant.pictureCounter == 0/*pStateQpConstant->pictureCounter == 0*/)
	{
		;//< return FALSE;	//FIXME
	}
	else
	{
		t_sva_brc_eot_data *pEotDataNMinus2 = &eotFifo.eotData[1-(pStateQpConstant->pictureCounter%2)];

		/* compute buffer level*/
		if(pEotDataNMinus2->skipPrev == 1 || pStateQpConstant->prevStrategicSkip == 1)
		{
			buffer = BRCMAX((t_sint32) pStateQpConstant->buffer - (t_sint32) pStateQpConstant->bufferDepletion,0);
		}
		else
		{
			buffer = BRCMAX((t_sint32) pStateQpConstant->buffer + (t_sint32) pEotDataNMinus2->bitstreamSizeInBits - (t_sint32) pStateQpConstant->bufferDepletion,0);
		}

		/* check if previous picture has been strategic skipped*/
		if(buffer > pStateQpConstant->picTarget)
		{
			;//return TRUE;		//< FIXME
		}
		else
		{
			;//return FALSE;		//< FIXME
		}
	}
}
#endif

/****************************************************************************/
/* NAME: t_sva_brc_error sva_EC_BRC_FinishPicture(                          */
/* 			t_sva_service_instance_num instanceNum,                         */
/*          t_sva_brc_in *pBrcIn                                            */
/* 			)                                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*       This routine will be called after encoding of a picture to return  */
/*      infos need by brc algorithm. It's call just after EOT.              */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :                                                                     */
/*	- instanceNum: number of the descriptor to use.                         */
/*  - pBrcIn: data coming from algo after picture is finish                 */
/*                                                                          */
/* OUT :                                                                    */
/*  - pIsCurrentStrategicSkip: return info about the fact current is strategic*/
/*                             skip or not.                                 */
/*                                                                          */
/* RETURN:                                                                  */
/*        t_sva_brc_error                                                   */
/*          t.b.d                                                           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
/*
 * DONE
*/
void COMP_BRC(sva_EC_BRC_FinishPicture)(t_sva_brc_in *pBrcIn)
{
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  sva_EC_BRC_FinishPicture() enter\n");
	#endif

	/*store data in eot fifo*/
	eotFifo.eotData[eotFifo.ptrWrite].bitstreamSizeInBits = pBrcIn->bitstreamSize + pBrcIn->stuffingBits;
	eotFifo.eotData[eotFifo.ptrWrite].bufferFullness = 0; 
	eotFifo.eotData[eotFifo.ptrWrite].skipPrev = pBrcIn->brcSkipPrev;
	eotFifo.eotData[eotFifo.ptrWrite].skipCurrent = pBrcIn->skipCurrent;
	eotFifo.ptrWrite = 1 - eotFifo.ptrWrite;
	
	PRINT_VAR(pBrcIn->brcSkipPrev);
	PRINT_VAR(pBrcIn->skipCurrent);

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  sva_EC_BRC_FinishPicture() exit\n");
	#endif
	
	//< return status;		//FIXME
	return;
}

PUT_PRAGMA
void METH(sva_EC_MP4_SetFrameParamOut)(const ts_t1xhv_vec_mpeg4_param_out* pMpeg4ParamOut, const ts_t1xhv_vec_mpeg4_param_inout* pMeg4ParamInOut)
{
	//< t_sva_ec_mp4_descriptor *pDesc=&mp4EncodeDesc[instanceNum];
	//< ts_t1xhv_vec_mpeg4_param_out *pMpeg4ParamOut=(t_sva_vec_mpeg4_param_out *) pParamOut;
	//< ts_t1xhv_vec_mpeg4_param_inout *pMeg4ParamInOut=(t_sva_vec_mpeg4_param_inout *) pParamInout;
	t_sva_brc_in brcIn;
	//< t_sva_brc_error brcError;	//FIXME

	/*			//< FIXME
	t_uint32 videoPacketNbToCopy;
	t_uint32 videoPacketOffset=0;
	t_uint32 i; */

	/*save skip and stream size info*/
	/*skip info*/

	//< pDesc->isCurrentItSkip=(t_bool)((pMpeg4ParamOut->brc_skip_prev!=0)?TRUE:FALSE);		//FIXME
	
	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  sva_EC_MP4_SetFrameParamOut() enter\n");
	#endif

	isCurrentItSkip=(t_bool)((pMpeg4ParamOut->brc_skip_prev!=0)?TRUE:FALSE);

#define SVA_ENABLE_MP4_HCL_WORKAROUND_VI10315
#if defined(SVA_ENABLE_MP4_HCL_WORKAROUND_VI10315) /* Sarvesh: Temporary HCL workaround for parsing error, VI10315 */
	if(/*SVA_CBR == pDesc->conf.brcMode*/0)	   //< FIXME watch
	{
		/* Always assume no frame is strategically skipped */
		//< pDesc->isCurrentStrategicSkip = FALSE;		//FIXME
		isCurrentStrategicSkip = FALSE;
	}
	else
	{
		PRINT_VAR(pMeg4ParamInOut->Skip_Current);
		PRINT_VAR(isCurrentItSkip);
		if((pMeg4ParamInOut->Skip_Current == 1) && /*pDesc->isCurrentItSkip == FALSE*/(isCurrentItSkip == FALSE))  //< FIXME
		{
			//< pDesc->isCurrentStrategicSkip = TRUE;	//FIXME
			isCurrentStrategicSkip = TRUE;
		}
		else
		{
			//< pDesc->isCurrentStrategicSkip = FALSE;	//FIXME
			isCurrentStrategicSkip = FALSE;
		}

		PRINT_VAR(isCurrentStrategicSkip);
	}
#else /* else of #if SVA_ENABLE_MP4_HCL_WORKAROUND_VI10315 */
	if((pMeg4ParamInOut->Skip_Current == 1) && /*pDesc->isCurrentItSkip == FALSE*/(isCurrentItSkip == FALSE))  //< FIXME
	{
		//< pDesc->isCurrentStrategicSkip = TRUE;		//FIXME
		isCurrentStrategicSkip = TRUE;
	}
	else
	{
		//< pDesc->isCurrentStrategicSkip = FALSE;		//FIXME
		isCurrentStrategicSkip = FALSE;
	}
#endif /* endif of #if SVA_ENABLE_MP4_HCL_WORKAROUND_VI10315 */

#if 0	//< FIXME
	/*save bitstream size*/
	pDesc->bitstreamSizeBits=pMeg4ParamInOut->bitstream_size+pMeg4ParamInOut->stuffing_bits;
	/*size info*/


	pDesc->pInfos->encodedFrameSize=(pMeg4ParamInOut->bitstream_size+pMeg4ParamInOut->stuffing_bits+7)/8;
	pDesc->pInfos->vpSliceNum=pMpeg4ParamOut->vp_num;
	if(pMpeg4ParamOut->vp_num>SVA_EC_MPEG4_VP_POS_COUNT)
	{
		videoPacketNbToCopy=SVA_EC_MPEG4_VP_POS_COUNT;videoPacketOffset=pMpeg4ParamOut->vp_num%SVA_EC_MPEG4_VP_POS_COUNT;
	}
	else
	{
		videoPacketNbToCopy=pMpeg4ParamOut->vp_num;
	}
	for(i=0;i<videoPacketNbToCopy;i++)
	{
		pDesc->pInfos->vpSlicePos[i]=pMpeg4ParamOut->vp_pos[(i+videoPacketOffset)%SVA_EC_MPEG4_VP_POS_COUNT];
	}
#endif

	/*provide data to brc*/

	brcIn.bitstreamSize=SVP_SHAREDMEM_FROM16(pMeg4ParamInOut->bitstream_size);
	brcIn.stuffingBits=SVP_SHAREDMEM_FROM16(pMeg4ParamInOut->stuffing_bits);
	brcIn.brcSkipPrev=pMpeg4ParamOut->brc_skip_prev;
	brcIn.skipCurrent=pMeg4ParamInOut->Skip_Current;
	//< brcError=sva_EC_BRC_FinishPicture(instanceNum,&brcIn);	//FIXME

	sva_EC_BRC_FinishPicture(&brcIn);
	//< if (brcError!=SVA_BRC_OK) {return SVA_EC_MP4_BRC_ERROR;}	//FIXME

	#ifdef LOGS_ENABLED
		printf("Mpeg4enc brc:  sva_EC_MP4_SetFrameParamOut() exit\n");
	#endif
	
	//< return SVA_EC_ALGO_OK;			//FIXME
	return;
}
PUT_PRAGMA
void METH(set_trace)(void * addr,t_uint16 id)
{
#ifndef __MPEG4ENC_ARM_NMF
	this = &brcTraceObj;
	if (addr != 0)
	{ // as fsmInit() may be call again to enable port 
		this->mTraceInfoPtr = (TraceInfo_t *)addr;
		this->mId1 = id;
	}
#else
	if (addr != 0)
	{ // as fsmInit() may be call again to enable port 
		
		setTraceInfo((TraceInfo_t *)addr,id);
	}
#endif
}
#ifdef __MPEG4ENC_ARM_NMF

t_nmf_error mpeg4enc_arm_nmf_brc::construct(void)
{
	#ifdef ENABLE_ARMNMF_LOGS
		printf("construct called\n");
	#endif
	return NMF_OK;
}

#else
PUT_PRAGMA
void METH(start)()
{
	#ifdef LOGS_ENABLED
		printf("\nComponent_init done");	
	#endif
}

/*****************************************************************************/
/**
 * \brief  setDebug
 *
 * Set the debug mode of the MPEG4 encoder NMf component
 * 
 * \param	mode.
 * \param	param1.
 * \param	param2.
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(setDebug)(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
	if((mode == DBG_MODE_NORMAL) || (mode == DBG_MODE_NO_HW) || (mode == DBG_MODE_PATTERN))
	{
		//< m4eDebugMode = mode;
		mpc_trace_init(param1, param2);
		#ifdef LOGS_ENABLED
			printf("Mpeg4enc brc:  setDebug Init\n");	
		#endif
	}
} /* End of setDebug() fucntion. */
#endif
