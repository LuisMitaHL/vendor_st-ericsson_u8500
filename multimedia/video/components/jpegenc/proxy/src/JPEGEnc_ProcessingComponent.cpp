/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#define _CNAME_ JPEGEnc_Proxy
#include <stdio.h>
#include "osi_trace.h"


#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegenc_proxy_src_JPEGEnc_ProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#include "JPEGEnc_Proxy.h"
#include "VFM_Port.h"
#include "JPEGEnc_ProcessingComponent.h"

extern OMX_U32 _nEXIFHeaderSize;

#define DEFAULT_EXIF_LENGTH 20

static const t_uint8 jpeg_zigzag_order[64] = {
	   0,  1,  5,  6, 14, 15, 27, 28,
	   2,  4,  7, 13, 16, 26, 29, 42,
	   3,  8, 12, 17, 25, 30, 41, 43,
	   9, 11, 18, 24, 31, 40, 44, 53,
	  10, 19, 23, 32, 39, 45, 52, 54,
	  20, 22, 33, 38, 46, 51, 55, 60,
	  21, 34, 37, 47, 50, 56, 59, 61,
	  35, 36, 48, 49, 57, 58, 62, 63
};

/// @ingroup JPEGEncoder
/// @brief Max size of the JPEG header
extern OMX_U16 _nHeaderSize;
extern OMX_U32 _nEXIFHeaderSize;
extern OMX_U8  _nJFIFappSegmentSize;

/// @ingroup JPEGEncoder
/// @brief If we want to optimize the computation of the header and quantization
/// table or not (if already computed)
extern int _statusOptimize; // Coming from JPEGEnc_ParamAndConfig.cpp

// some constants dedicated to huffman for the length and code
extern t_uint16 _nHuffmanLength;
extern t_uint16 _nHuffmanCode;

#define EXIFHEADER (64*1024)
JPEGEnc_ProcessingComponent::JPEGEnc_ProcessingComponent()
{
	OstTraceInt1(TRACE_API, "In JPEGEnc_ProcessingComponent : In Constructor  <line no %d> ",__LINE__);
	headerMPCPosition = 0x0;
	headerSizePosition = 0x0;
	tempPosition = 0x0;
}


OMX_ERRORTYPE JPEGEnc_ProcessingComponent::addEXIFHeader(JPEGEnc_Proxy *jpegenc,OMX_BUFFERHEADERTYPE* pBuffer,OMX_U8 **header_start,
														 OMX_U8 **exifheader_start,OMX_U32 *exifheaderSize,
														 OMX_U32 nHeaderSize)
{

	#define copy_array(_dst, _src, _n) {	\
		for (i = 0; i < _n; i++) {			\
			*_dst++ = _src[i];				\
		}									\
	}
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ProcessingComponent : In addEXIFHeader  <line no %d> ",jpegenc,__LINE__);
	OMX_ERRORTYPE exiferror = OMX_ErrorUndefined;

	t_uint8 *exifaddr_pointer;
	t_uint8 *refexifaddr_pointer;
	t_uint8 *addr_pointer;
	t_uint8 *headeraddr_pointer;
	OMX_U32 actual_headerSize = 0;
	OMX_OTHER_EXTRADATATYPE extradata,*pHeader;
	t_uint8 SOI_tab[2] = { 0xff, 0xd8 };
	t_uint8 exif_header[DEFAULT_EXIF_LENGTH] = {0xFF,0xE1,0xFF,0xFE,
												0x45,0x78,0x69,0x66,
												0x00,0x00,0x49,0x49,
												0x2A,0x00,0x08,0x00,
												0x00,0x00,0x00,0x00}; //defaulf header to be written
	OMX_U32 in = 0,size_app1 = 0;
	OMX_U32 off_sum=0;
	t_sint32 i;
	int flag_app1=0;

	exifaddr_pointer = (t_uint8 *) *exifheader_start;//sysAddr.logical;
	refexifaddr_pointer = (t_uint8 *) *exifheader_start;//sysAddr.logical;

	memset(refexifaddr_pointer,0x00,_nEXIFHeaderSize);


	copy_array(exifaddr_pointer, SOI_tab, 2);
//			exifaddr_pointer +=2; //writing SOI tag

	if (pBuffer->nFlags & OMX_BUFFERFLAG_EXTRADATA)
	{
		OstTraceFiltStatic1(TRACE_FLOW, "In JPEGEnc_ProcessingComponent : In addEXIFHeader EXTRA DATA Flag Set <line no %d> ",jpegenc,__LINE__);
		pBuffer->nFlags = pBuffer->nFlags & (~OMX_BUFFERFLAG_EXTRADATA);
		addr_pointer = (OMX_U8 *)(pBuffer->pBuffer + pBuffer->nOffset + pBuffer->nFilledLen);
		if(((OMX_U32)addr_pointer%4)!=0)
			addr_pointer=(OMX_U8 *)(((OMX_U32)addr_pointer+3)/4);
		memcpy((OMX_U8*)&extradata,addr_pointer,sizeof(OMX_OTHER_EXTRADATATYPE));
		while(extradata.eType!=OMX_ExtraDataNone)
		{
			if((t_uint32)(extradata.eType == (t_uint32)(OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker) && (flag_app1 == 0))
			{
				OstTraceFiltStatic1(TRACE_FLOW, "In JPEGEnc_ProcessingComponent : In addEXIFHeader writing  provided APP1 marker segment <line no %d> ",jpegenc,__LINE__);
				flag_app1=1;
				pHeader = (OMX_OTHER_EXTRADATATYPE*)addr_pointer;
				addr_pointer = (OMX_U8 *)&(pHeader->data);
				OMX_SYMBIAN_EXIFMARKERTYPE *pHeader1;
				pHeader1 = (OMX_SYMBIAN_EXIFMARKERTYPE *)addr_pointer;
				OstTraceFiltStatic2(TRACE_DEBUG, "In JPEGEnc_ProcessingComponent : In addEXIFHeader Reading address : 0x%x  <line no %d> ",jpegenc,(OMX_U32)addr_pointer,__LINE__);
				size_app1 = pHeader1->nSize;//it gives the size of APP1 marker segment. 
				addr_pointer = (OMX_U8 *)&(pHeader1->nAppMarkerData);
				for (in = 0;in < size_app1;in++)
				{
					((OMX_U8 *)exifaddr_pointer)[in] = ((OMX_U8 *)addr_pointer)[in];
						//printf("Charaacters written : %d \n",in);
				}
				exifaddr_pointer +=size_app1;
				break;
			}
			else
			{
				off_sum = off_sum + extradata.nSize;			//reaching to the next OMX_OTHER_EXTRADATATYPE
				addr_pointer = pBuffer->pBuffer + pBuffer->nOffset + pBuffer->nFilledLen;
				if(((OMX_U32)addr_pointer%4)!=0)
					addr_pointer=(OMX_U8 *)(((OMX_U32)addr_pointer+3)/4);
				addr_pointer = addr_pointer + off_sum;
				OstTraceFiltStatic2(TRACE_DEBUG, "In JPEGEnc_ProcessingComponent : In addEXIFHeader Reading address : 0x%x  <line no %d> ",jpegenc,(OMX_U32)addr_pointer,__LINE__);
				memcpy((OMX_U8*)&extradata,addr_pointer,sizeof(OMX_OTHER_EXTRADATATYPE));
			}
		}
		if(flag_app1==1)
		{
			refexifaddr_pointer[4] = 0xFF;//writing the length of EXIF header
			refexifaddr_pointer[5] = 0xFE;//writing the length of EXIF header
		}
		else
		{
			//simply copy the exif_header
			OstTraceFiltStatic1(TRACE_FLOW, "In JPEGEnc_ProcessingComponent : In addEXIFHeader writing default APP1 marker segment <line no %d> ",jpegenc,__LINE__);
			for (in = 0;in<DEFAULT_EXIF_LENGTH;in++)
			{
				((OMX_U8 *)exifaddr_pointer)[in] = ((OMX_U8 *)exif_header)[in];
				//printf("Charaacters written : %d \n",in);
			}
			exifaddr_pointer +=DEFAULT_EXIF_LENGTH;
		}
	}
	else
	{
		//simply copy the exif_header
		OstTraceFiltStatic1(TRACE_FLOW, "In JPEGEnc_ProcessingComponent : In addEXIFHeader writing default APP1 marker segment <line no %d> ",jpegenc,__LINE__);
		for (in = 0;in<DEFAULT_EXIF_LENGTH;in++)
		{
			((OMX_U8 *)exifaddr_pointer)[in] = ((OMX_U8 *)exif_header)[in];
				//printf("Charaacters written : %d \n",in);
		}
		exifaddr_pointer +=DEFAULT_EXIF_LENGTH;
	}

//	exifaddr_pointer = refexifaddr_pointer + EXIFHEADER +3;//for for FFD8 tag
	exifaddr_pointer = refexifaddr_pointer + EXIFHEADER +2;//for for FFD8 tag
	//now writing to the main header
	headeraddr_pointer = (t_uint8 *) *header_start;//sysAddr.logical;
	headeraddr_pointer += 2; //skipping the SOI tag already written

/*	if (pBuffer->nFlags & OMX_BUFFERFLAG_EXTRADATA)
	{
		exifaddr_pointer+= 2;//leaving 2 bytes to compensate for FFD8 tags
		actual_headerSize +=2; //FIX for 2 bytes
	}*/
	memcpy (exifaddr_pointer,headeraddr_pointer,nHeaderSize-2); //skipping the SOI tag

//	actual_headerSize = exifaddr_pointer - refexifaddr_pointer + nHeaderSize - 2;
	actual_headerSize = EXIFHEADER + nHeaderSize;
	*exifheaderSize = actual_headerSize ;
	memcpy((OMX_U8 *)headerMPCPosition,refexifaddr_pointer,actual_headerSize);//copying full header
	if (jpegenc->isMPCobject)
	{
		 OMX_U32 tempValue;
		 tempValue = ENDIANESS_CONVERSION((*exifheaderSize)*8);
		 memcpy((OMX_U32 *)headerSizePosition,&tempValue,4);
	}
	else
	{
		memcpy((OMX_U32 *)headerSizePosition,exifheaderSize,4);
	}
	exiferror = OMX_ErrorNone;

	#undef copy_array

	return exiferror;
}



//following the notations from MPC code
OMX_U8 JPEGEnc_ProcessingComponent::fromBufferTypeToJPEGType(JPEGEnc_Proxy *jpegenc)
{
	OMX_U8 color_value = 1; //OMX_COLOR_FormatYUV420Planar default
	if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome) {
		return 0;
	}
	else if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420Planar){
			color_value = 1;
	}
	else if(jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV422Planar){
		color_value = 2;
	}
	else if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar || jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) {

		return 4;
	}
	else if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY) {
		color_value = 5;
	}
	else if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420SemiPlanar) {
		color_value = 6;
	}


        else {
		DBC_ASSERT(1==0);
	}
	OstTraceFiltStatic2(TRACE_API, "In JPEGEnc_ProcessingComponent :  fromBufferTypeToJPEGType DONE Color value : %d <line no %d> ",jpegenc,color_value,__LINE__);
	return color_value;
}


// Quantization tables and huffman tables can be found in
// - reference code: file jpeg_globals.h  (same as in the JPEG recommendations of the specifications
// - validation code: file val_JPEGEnc_Proxy.c (differents)
void JPEGEnc_ProcessingComponent::compute_quantization_table(JPEGEnc_Proxy *jpegenc)
{
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ProcessingComponent : In Constructor  <line no %d> ",jpegenc,__LINE__);
	t_uint32 scale_factor;
	t_sint16 i;
	t_uint16 quality_factor;
	const t_uint8 *pt0, *pt1;
	t_sint16 val0, val1;

	if (!jpegenc->mParam.QuantToCompute.get()) {
		// no need to recompute it

		return;
	}

	DBC_PRECONDITION(1 <= jpegenc->mParam.QFactor && jpegenc->mParam.QFactor <= 100);
	if (jpegenc->mParam.QFactor < 50) {
		quality_factor = (jpegenc->mParam.QFactor > 1) ? jpegenc->mParam.QFactor : 1;	//Max(aQualityFactor,1);
		scale_factor = 5000 / quality_factor;
	} else {
		quality_factor = (jpegenc->mParam.QFactor < 100) ? jpegenc->mParam.QFactor : 100;	//Min(aQualityFactor,100);
		scale_factor = 200 - (quality_factor * 2);
	}

	if (jpegenc->mParam.rotation == VFM_ROTATION_NONE) {
		pt0 = jpegenc->mParam._LumaQuantification;
		pt1 = jpegenc->mParam._ChromaQuantification;
	} else {

        t_sint16 x=0,z=63;
		for(t_sint16 i=0;i<8;i++)
		{
			int y=x+i;
			int w=z-i;
			for(t_sint16 j=i;j>=0;j--)
			{
				jpegenc->mParam._LumaQuantificationRotate[x++]=jpegenc->mParam._LumaQuantification[y--];
				jpegenc->mParam._ChromaQuantificationRotate[x++]=jpegenc->mParam._ChromaQuantification[y--];
			}
			for(t_sint16 j=i;j>=0;j--)
			{
				if(i==7)
					break;
				jpegenc->mParam._LumaQuantificationRotate[z--]=jpegenc->mParam._LumaQuantification[w++];
				jpegenc->mParam._ChromaQuantificationRotate[z--]=jpegenc->mParam._ChromaQuantification[w++];
			}

		}
		pt0 = jpegenc->mParam._LumaQuantificationRotate;
		pt1 = jpegenc->mParam._ChromaQuantificationRotate;
	}

	for (i = 0; i < 64; i++) {
		val0 = (pt0[i] * scale_factor + 50) / 100;
		val1 = (pt1[i] * scale_factor + 50) / 100;
		if (val0 <= 0)
			val0 = 1;
		else if (val0 > 255)
			val0 = 255;
		if (val1 <= 0)
			val1 = 1;
		else if (val1 > 255)
			val1 = 255;

		jpegenc->mParam.QuantTable.QuantizationLuma[i] = val0;
		jpegenc->mParam.QuantTable.QuantizationChroma[i] = val1;

	}

	if (_statusOptimize) {
		jpegenc->mParam.QuantToCompute.reset();
	}
}



void JPEGEnc_ProcessingComponent::headerCreation(JPEGEnc_Proxy *jpegenc,
                                                 OMX_U8 **header_start ,
                                                 OMX_U32 *headerSize )
{
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ProcessingComponent : In headerCreation  <line no %d> ",jpegenc,__LINE__);
	#define copy_array(_dst, _src, _n) {	\
		for (i = 0; i < _n; i++) {			\
			*_dst++ = _src[i];				\
		}									\
	}


	t_sint16 i;
	t_uint8 *addr_pointer;
	t_sint16 window_width, window_height;
	t_uint8 SOI_tab[2] = { 0xff, 0xd8 };
	t_uint8 APP0_tab[18] = { 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x02, 0x01, 0x00, 0x48, 0x00, 0x48, 0x00, 0x00};

	/* Define "Standard Quality" Quantization Table in zig-zag order. Should be followed by QuantizationLuma and QuantizationChroma */
	t_uint8 DQT0_tab[5] = { 0xff, 0xdb, 0x00, 0x43, 0x00 };
	t_uint8 DQT1_tab[5] = { 0xff, 0xdb, 0x00, 0x43, 0x01 };

	t_uint8 SOF0_Y_tab[13] = { 0xff, 0xc0,	/* Baseline DCT                  */
		0x00, 0x0b, 0x08, 0x00, 0xb0, 0x01, 0x60,	/* Lh Ll P Yh° Yl° Xh° Xl°       */
		0x01, 0x01, 0x11, 0x00
	};							/* Nf Ci HiVi Tq                 */

	t_uint8 SOF0_YCbCr_tab[19] = { 0xff, 0xc0,	/* Baseline DCT                  */
		0x00, 0x11, 0x08, 0x00, 0xb0, 0x01, 0x60,	/* Lh Ll P Yh° Yl° Xh° Xl°       */
		0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01,	/* Nf C1 H1V1° Tq1 C2 H2V2° Tq2  */
		0x03, 0x11, 0x01
	};							/* C3 H3V3° Tq3°                 */

  	/* Define Huffman Table */
	t_uint8 LumaDC[5] = { 0xff, 0xc4, 0x00, 0x1f, 0x00 };
	t_uint8 LumaAC[5] = { 0xff, 0xc4, 0x00, 0xb5, 0x10 };
	t_uint8 ChromaDC[5] = { 0xff, 0xc4, 0x00, 0x1f, 0x01 };
	t_uint8 ChromaAC[5] = { 0xff, 0xc4, 0x00, 0xb5, 0x11 };

	t_uint8 DRI_tab[6] = { 0xff, 0xdd,	/* Define Restart Interval */
		0x00, 0x04, 0x01, 0x60
	};

	t_uint8 SOS_Y_tab[10] = { 0xff, 0xda,	/* Start Of Scan */
		0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x00
	};							/* Lh Ll Ns C1 Tda1 Ss Se AhAl */

	t_uint8 SOS_YCbCr_tab[14] = { 0xff, 0xda,	/* Start Of Scan */
		0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11,
		0x00, 0x3f, 0x00
	};

	if (!jpegenc->mParam.HeaderToGenerate.get()) {
		// the header has been already computed

		return;
	}

	/* Consistency Checking */
	if (_statusOptimize) {
		DBC_PRECONDITION(!jpegenc->mParam.QuantToCompute.get());
	}
	DBC_PRECONDITION((jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420Planar) ||
					 (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome)   ||
					 (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV422Planar) ||
					 (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420SemiPlanar) ||
					 (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY) ||
					 (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar) ||
                     (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar))


	window_width = jpegenc->getFrameWidth(1);
	window_height = jpegenc->getFrameHeight(1);

	/* Update picture size information
	   ------------------------------- */
	if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome)
	{	//Y only
		if (jpegenc->mParam.rotation == VFM_ROTATION_NONE)
		{
			SOF0_Y_tab[5] = (window_height & 0xff00) >> 8;
			SOF0_Y_tab[6] = window_height & 0xff;
			SOF0_Y_tab[7] = (window_width & 0xff00) >> 8;
			SOF0_Y_tab[8] = window_width & 0xff;
		}
		else
		{
			SOF0_Y_tab[5] = (window_width & 0xff00) >> 8;
			SOF0_Y_tab[6] = window_width & 0xff;
			SOF0_Y_tab[7] = (window_height & 0xff00) >> 8;
			SOF0_Y_tab[8] = window_height & 0xff;
		}
		SOF0_YCbCr_tab[11] = 0x11;
	}
	else
	{					/* 420 - 422 -444 */
		if (jpegenc->mParam.rotation == VFM_ROTATION_NONE)
		{
			SOF0_YCbCr_tab[5] = (window_height & 0xff00) >> 8;
			SOF0_YCbCr_tab[6] = window_height & 0xff;
			SOF0_YCbCr_tab[7] = (window_width & 0xff00) >> 8;
			SOF0_YCbCr_tab[8] = window_width & 0xff;
		}
		else
		{
			SOF0_YCbCr_tab[5] = (window_width & 0xff00) >> 8;
			SOF0_YCbCr_tab[6] = window_width & 0xff;
			SOF0_YCbCr_tab[7] = (window_height & 0xff00) >> 8;
			SOF0_YCbCr_tab[8] = window_height & 0xff;
		}

		if ((jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV422Planar) ||
		    (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatCbYCrY))
			SOF0_YCbCr_tab[11] = 0x21;
		//else if (inputBufferFormat == FMT_444) //to be fixed for 444 format
		//	SOF0_YCbCr_tab[11] = 0x11;
	}
	DRI_tab[4] = (jpegenc->mParam.getRestartInterval() & 0xff00) >> 8;
	DRI_tab[5] = (jpegenc->mParam.getRestartInterval() & 0x00ff);

	/* Header writing
	   -------------- */
	addr_pointer = (t_uint8 *) *header_start;//sysAddr.logical;
	copy_array(addr_pointer, SOI_tab, 2);

	if (jpegenc->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
	{
		if((window_height > THUMBNAIL_H)&&(window_width > THUMBNAIL_W))
			copy_array(addr_pointer, APP0_tab, 18);
	}

	/* Quantization tables
	   ------------------- */
	copy_array(addr_pointer, DQT0_tab, 5);
	copy_array(addr_pointer, jpegenc->mParam.QuantTable.QuantizationLuma, 64);

	if (jpegenc->mParam.inputBufferFormat != (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome)
		{
		copy_array(addr_pointer, DQT1_tab, 5);
		copy_array(addr_pointer, jpegenc->mParam.QuantTable.QuantizationChroma, 64);
	}

	/* Start of Frame
	   -------------- */
	if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome)
		{
		copy_array(addr_pointer, SOF0_Y_tab, 13);
	} else {
		copy_array(addr_pointer, SOF0_YCbCr_tab, 19);
	}

	/* Huffman table
	   ------------- */
	// DC luma table
	copy_array(addr_pointer, LumaDC, 5);
	copy_array(addr_pointer, jpegenc->mParam._BITS_DCLuma, _nHuffmanLength);
	int length=0;
	for(int j=0;j<_nHuffmanLength;j++)
	{
		length += jpegenc->mParam._BITS_DCLuma[j];
	}
	copy_array(addr_pointer, jpegenc->mParam._HUFFVAL_DCLuma, length);

	// AC luma table
	copy_array(addr_pointer, LumaAC, 5);
	copy_array(addr_pointer, jpegenc->mParam._BITS_ACLuma, _nHuffmanLength);
	length=0;
	for(int j=0;j<_nHuffmanLength;j++)
	{
		length += jpegenc->mParam._BITS_ACLuma[j];
	}
	copy_array(addr_pointer, jpegenc->mParam._HUFFVAL_ACLuma, length);

	// Chroma table if required
		if (jpegenc->mParam.inputBufferFormat != (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome)
		{
		copy_array(addr_pointer, ChromaDC, 5);
		copy_array(addr_pointer, jpegenc->mParam._BITS_DCChroma, _nHuffmanLength);
		length=0;
		for(int j=0;j<_nHuffmanLength;j++)
		{
			length += jpegenc->mParam._BITS_DCChroma[j];
		}
		copy_array(addr_pointer, jpegenc->mParam._HUFFVAL_DCChroma, length);

		// AC Chroma table
		copy_array(addr_pointer, ChromaAC, 5);
		copy_array(addr_pointer, jpegenc->mParam._BITS_ACChroma, _nHuffmanLength);
		length=0;
		for(int j=0;j<_nHuffmanLength;j++)
		{
			length += jpegenc->mParam._BITS_ACChroma[j];
		}
		copy_array(addr_pointer, jpegenc->mParam._HUFFVAL_ACChroma, length);
	}

	/* Restart Interval
	   ---------------- */
	if(((window_height > THUMBNAIL_H)&&(window_width > THUMBNAIL_W)))//According to Camera SAS V1.3
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "JPEGENC_Processing : In headerCreation Resolution greater than of thumbnail type\n",jpegenc);
		copy_array(addr_pointer, DRI_tab, 6);
	}

	/* Start of scan
	   ------------- */
	if (jpegenc->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome) {
		copy_array(addr_pointer, SOS_Y_tab, 10);
	} else {
		copy_array(addr_pointer, SOS_YCbCr_tab, 14);
	}

	/*FIXME.0 */
	(*headerSize) = addr_pointer - (t_uint8 *)*header_start;//sysAddr.logical;

	if (jpegenc->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
	{
		DBC_POSTCONDITION((*headerSize) <= (_nHeaderSize+_nJFIFappSegmentSize));
	}
	else
	{
		DBC_POSTCONDITION((*headerSize) <= _nHeaderSize);
	}
	if (_statusOptimize) {
		jpegenc->mParam.HeaderToGenerate.reset();
	}

#undef copy_array

	OstTraceFiltStatic0(TRACE_API, "JPEGENC_Processing : Header Creation Done \n",jpegenc);

}

void JPEGEnc_ProcessingComponent::set_pJecIn_parameters(JPEGEnc_Proxy *jpegenc, ts_t1xhv_vec_jpeg_param_in *pJecIn_parameters)
{

	t_uint16	i, k;
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ProcessingComponent : In set_pJecIn_parameters  <line no %d> ",jpegenc,__LINE__);
	if (_statusOptimize) {
		DBC_PRECONDITION(!jpegenc->mParam.QuantToCompute.get() && !jpegenc->mParam.HeaderToGenerate.get());
	}
	pJecIn_parameters->frame_width  			   = (jpegenc->getFrameWidth(0) +0xf)&(~0xf);
	pJecIn_parameters->frame_height 			   = (jpegenc->getFrameHeight(0)+0xf)&(~0xf);
	pJecIn_parameters->window_width 			   = (jpegenc->getFrameWidth(1) +0xf)&(~0xf);
	pJecIn_parameters->window_height			   = (jpegenc->getFrameHeight(1)+0xf)&(~0xf);
	pJecIn_parameters->window_horizontal_offset    = jpegenc->mParam.cropping_parameter.nLeft;		// x is in width
	pJecIn_parameters->window_vertical_offset	   = jpegenc->mParam.cropping_parameter.nTop;
  	pJecIn_parameters->last_slice				   = 1;	// specifies whether the current JPEG encode process includes the end of the picture or not
	pJecIn_parameters->enable_optimized_quant	   = jpegenc->mParam.isOptimizeQuantTableEnable;
	pJecIn_parameters->target_bpp				   = jpegenc->mParam.targetBpp;
	pJecIn_parameters->enable_optimized_huffman    = 0;
	pJecIn_parameters->rotation 				   = jpegenc->mParam.rotation;
	pJecIn_parameters->reserved_1				   = 0;
	pJecIn_parameters->reserved_2				   = 0;
	pJecIn_parameters->sampling_mode				= fromBufferTypeToJPEGType(jpegenc);

	if(((pJecIn_parameters->window_height > THUMBNAIL_H)&&(pJecIn_parameters->window_width > THUMBNAIL_W)))//Acc to Camera SAS v1.3
		pJecIn_parameters->restart_interval            = jpegenc->mParam.getRestartInterval();
	else
		pJecIn_parameters->restart_interval            = 0;

	OstTraceFiltStatic0(TRACE_DEBUG, "JPEGENC_Processing :  Input Parameter Values\n",jpegenc);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Frame Width : %d\n",jpegenc,pJecIn_parameters->frame_width);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Frame Height : %d\n",jpegenc,pJecIn_parameters->frame_height);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Window Width : %d\n",jpegenc,pJecIn_parameters->window_width);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Window Height : %d\n",jpegenc,pJecIn_parameters->window_height);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Window Horizontal Offset : %d\n",jpegenc,pJecIn_parameters->window_horizontal_offset);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Window Vertical Offset : %d\n",jpegenc,pJecIn_parameters->window_vertical_offset);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Rotation : %d\n",jpegenc,pJecIn_parameters->rotation);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Sampling Mode : %d\n",jpegenc,pJecIn_parameters->sampling_mode);
	OstTraceFiltStatic1(TRACE_DEBUG, "JPEGENC_Processing :  Restart Interval : %d\n",jpegenc,pJecIn_parameters->restart_interval);
	// Huffman Table
	// deprecated now
	/*
	for (i=0;i<12;i++) {
		pJecIn_parameters->huffman_luma_code_dc[i]    = jpegenc->mParam.ParamLumaHuffmanCodeDcTable[i];
        pJecIn_parameters->huffman_luma_size_dc[i]    = jpegenc->mParam.ParamLumaHuffmanSizeDcTable[i];
        pJecIn_parameters->huffman_chroma_code_dc[i]  = jpegenc->mParam.ParamChromaHuffmanCodeDcTable[i];
		pJecIn_parameters->huffman_chroma_size_dc[i]  = jpegenc->mParam.ParamChromaHuffmanSizeDcTable[i];
	}
	for (i=0;i<256;i++) {
		pJecIn_parameters->huffman_luma_code_ac[i]    = jpegenc->mParam.ParamLumaHuffmanCodeAcTable[i];
        pJecIn_parameters->huffman_luma_size_ac[i]    = jpegenc->mParam.ParamLumaHuffmanSizeAcTable[i];
        pJecIn_parameters->huffman_chroma_code_ac[i]  = jpegenc->mParam.ParamChromaHuffmanCodeAcTable[i];
        pJecIn_parameters->huffman_chroma_size_ac[i]  = jpegenc->mParam.ParamChromaHuffmanSizeAcTable[i];
	}
	*/

	// Quantization table, in column-order
	k = 0;
	for (i=0;i<64;i++) {
		pJecIn_parameters->quant_luma[k] = jpegenc->mParam.QuantTable.QuantizationLuma[jpeg_zigzag_order[i]];
		pJecIn_parameters->quant_chroma[k] = jpegenc->mParam.QuantTable.QuantizationChroma[jpeg_zigzag_order[i]];
		k = (k+8)%64;
		if (k<8) k++;
	}
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ProcessingComponent : set_pJecIn_parameters DONE <line no %d> ",jpegenc,__LINE__);
}





