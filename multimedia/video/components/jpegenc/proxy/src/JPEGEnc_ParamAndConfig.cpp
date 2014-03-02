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

#define _CNAME_ Vfm_jpegenc
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegenc_proxy_src_JPEGEnc_ParamAndConfigTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#include "JPEGEnc_ParamAndConfig.h"

/// Default quantification tables
// coef. are given in the zig-zag order
static const t_uint8 _LumaQuantification_default[64] = {
	16, 11, 12, 14, 12, 10, 16, 14,
	13, 14, 18, 17, 16, 19, 24, 40,
	26, 24, 22, 22, 24, 49, 35, 37,
	29, 40, 58, 51, 61, 60, 57, 51,
	56, 55, 64, 72, 92, 78, 64, 68,
	87, 69, 55, 56, 80, 109, 81, 87,
	95, 98, 103, 104, 103, 62, 77, 113,
	121, 112, 100, 120, 92, 101, 103, 99
};

static const t_uint8 _ChromaQuantification_default[64] = {
	17, 18, 18, 24, 21, 24, 47, 26,
	26, 47, 99, 66, 56, 66, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99
};

// some constants dedicated to huffman for the length and code
t_uint16 _nHuffmanLength = 16;
t_uint16 _nHuffmanCode = 256;

// Huffman Table as they appear in the header
// Cf. JPEG norm pages 40, 148 and 158


static const OMX_U8 _BITS_ACLuma_default[] = {
	0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03,
	0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d
};

static const OMX_U8 _HUFFVAL_ACLuma_default[] = {
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const OMX_U8 _BITS_DCLuma_default[] = {
	0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const OMX_U8 _HUFFVAL_DCLuma_default[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const OMX_U8 _BITS_ACChroma_default[] = {
	0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04,
	0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77
};

static const OMX_U8 _HUFFVAL_ACChroma_default[] = {
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const OMX_U8 _BITS_DCChroma_default[] = {
	0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const OMX_U8 _HUFFVAL_DCChroma_default[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

JPEGEnc_ParamAndConfig::JPEGEnc_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{
	OstTraceInt1(TRACE_API, "In JPEGEnc_ParamAndConfig : In Constructor <line no %d> ",__LINE__);
    // initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(0, 0);
    t_uint16  *pCode;
	t_uint16  *pSize;
    // specific attibutes initialization
	cropping_parameter.nLeft = 0;
	cropping_parameter.nTop = 0;
	cropping_parameter.nWidth = 0;
	cropping_parameter.nHeight = 0;
	cropping_parameter.nPortIndex=1;
	cropping_parameter.nSize=0;
	rotation_val = 0;
	m_nSvaMcps = 50;

	inputBufferFormat = (OMX_COLOR_FORMATTYPE) OMX_COLOR_FormatYUV420MBPackedSemiPlanar;
	outputCompressionFormat = (OMX_IMAGE_CODINGTYPE) OMX_IMAGE_CodingJPEG;

	nNumber_param=0;
    QFactor = 50;        // default value same as the one of the reference code
    targetBpp = 0;        // no default as we are using QFactor by default
    isOptimizeQuantTableEnable = OMX_FALSE;
    rotation = VFM_ROTATION_NONE;
	for(int i=0;i<64;i++)
	{
			_LumaQuantification[i]=_LumaQuantification_default[i];
			_ChromaQuantification[i]=_ChromaQuantification_default[i];
			 _LumaQuantificationRotate[i]=0;
			 _ChromaQuantificationRotate[i]=0;
			 QuantTable.QuantizationChroma[i]=0;
			 QuantTable.QuantizationLuma[i]=0;
	}

	for(int i=0;i<256;i++)
	{
			 _HUFFVAL_DCChroma[i]=_HUFFVAL_DCChroma_default[i];
			 _HUFFVAL_ACChroma[i]=_HUFFVAL_ACChroma_default[i];
			 _HUFFVAL_DCLuma[i]=_HUFFVAL_DCLuma_default[i];
			 _HUFFVAL_ACLuma[i]=_HUFFVAL_ACLuma_default[i];
	}
	for(int i=0;i<16;i++)
	{
			 _BITS_DCChroma[i]=_BITS_DCChroma_default[i];
			 _BITS_ACChroma[i]=_BITS_ACChroma_default[i];
			 _BITS_DCLuma[i]=_BITS_DCLuma_default[i];
			 _BITS_ACLuma[i]=_BITS_ACLuma_default[i];
			 _BITS_DCChroma_shifted[i+1]=_BITS_DCChroma_default[i];
			 _BITS_DCLuma_shifted[i+1]=_BITS_DCLuma_default[i];
			 _BITS_ACChroma_shifted[i+1]=_BITS_ACChroma_default[i];
			 _BITS_ACLuma_shifted[i+1]=_BITS_ACLuma_default[i];
	}
	for(int i=0;i<12;i++)
	{
		ParamLumaHuffmanCodeDcTable[i]=0;
		ParamLumaHuffmanSizeDcTable[i]=0;
		ParamChromaHuffmanCodeDcTable[i]=0;
		ParamChromaHuffmanSizeDcTable[i]=0;
	}
	for(int i=0;i<256;i++)
	{
		ParamLumaHuffmanCodeAcTable[i]=0;
		ParamLumaHuffmanSizeAcTable[i]=0;
		ParamChromaHuffmanCodeAcTable[i]=0;
		ParamChromaHuffmanSizeAcTable[i]=0;
	}
		pCode=ParamLumaHuffmanCodeAcTable;
		pSize=ParamLumaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACLuma_shifted,_HUFFVAL_ACLuma,pCode,pSize);
		pCode=ParamChromaHuffmanCodeAcTable;
		pSize=ParamChromaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACChroma_shifted,_HUFFVAL_ACChroma,pCode,pSize);
		pCode=ParamLumaHuffmanCodeDcTable;
		pSize=ParamLumaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCLuma_shifted,_HUFFVAL_DCLuma,pCode,pSize);
		pCode=ParamChromaHuffmanCodeDcTable;
		pSize=ParamChromaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCChroma_shifted,_HUFFVAL_DCChroma,pCode,pSize);
		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;
		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;

}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::reset()
{
	OstTraceInt1(TRACE_API, "In JPEGEnc_ParamAndConfig : In reset <line no %d> ",__LINE__);
    HeaderToGenerate.set();
    //vertical_offset = horizontal_offset = 0;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexConfigCommonRotate(OMX_PTR pt_org, OMX_S32 *rotation, OMX_BOOL *has_changed)
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexConfigCommonRotate <line no %d> ",__LINE__);
    OMX_CONFIG_ROTATIONTYPE *pt = (OMX_CONFIG_ROTATIONTYPE *)pt_org;
    OMX_S32 rotationOld = *rotation;
	OMX_S32 diff=0;
    switch (pt->nRotation) {
    case 0:
	case 180:
    case 270:
    case 90:
        *rotation = pt->nRotation;
        break;
    default:
        return OMX_ErrorBadParameter;
    }
	diff = ((rotationOld - *rotation) > 0)?(rotationOld - *rotation):(*rotation - rotationOld);
    *has_changed = (OMX_BOOL)((diff/90)%2);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexConfigCommonRotate(OMX_PTR pt_org, OMX_S32 rotation) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexConfigCommonRotate <line no %d> ",__LINE__);
    OMX_CONFIG_ROTATIONTYPE *pt = (OMX_CONFIG_ROTATIONTYPE *)pt_org;
    switch (rotation) {
    case 0:
	case 180:
    case 270:
    case 90:
        pt->nRotation = rotation;
        break;
    default:
        return OMX_ErrorBadParameter;
    }


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexConfigTargetbpp(OMX_PTR pt_org, OMX_U16 *targetBpp, OMX_BOOL *has_changed)
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexConfigTargetbpp <line no %d> ",__LINE__);
    *has_changed = (OMX_BOOL)(*targetBpp != ((VFM_CONFIG_TARGETBPP *)pt_org)->targetBpp);
    *targetBpp = ((VFM_CONFIG_TARGETBPP *)pt_org)->targetBpp;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexConfigTargetbpp(OMX_PTR pt_org, OMX_U16 targetBpp) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexConfigTargetbpp <line no %d> ",__LINE__);
    ((VFM_CONFIG_TARGETBPP *)pt_org)->targetBpp = targetBpp;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexParamImagePortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamImagePortFormat <line no %d> ",__LINE__);
    *has_changed = OMX_TRUE;
    OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
    if (pt->nPortIndex==IPB+0) {            // input port
        HeaderToGenerate.set();
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat == OMX_IMAGE_CodingUnused, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420MBPackedSemiPlanar || pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar|| pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV422Planar ||
                pt->eColorFormat==(OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatMonochrome || pt->eColorFormat== (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420PackedPlanar, OMX_ErrorBadParameter);
       inputBufferFormat = pt->eColorFormat;
        // nothing to be check on pt->nIndex
    } else  {        // output port
        RETURN_XXX_IF_WRONG(pt->eCompressionFormat == OMX_IMAGE_CodingJPEG, OMX_ErrorBadParameter);
        RETURN_XXX_IF_WRONG(pt->eColorFormat == OMX_COLOR_FormatUnused, OMX_ErrorBadParameter);
        // nothing to be check on pt->nIndex
    }
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamImagePortFormat Color Format : %d ",pt->eColorFormat);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexParamImagePortFormat(OMX_PTR pt_org) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamImagePortFormat <line no %d> ",__LINE__);
    OMX_IMAGE_PARAM_PORTFORMATTYPE *pt = (OMX_IMAGE_PARAM_PORTFORMATTYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);

    if (pt->nPortIndex==IPB+0) {            // input port
        pt->eCompressionFormat = OMX_IMAGE_CodingUnused;
        pt->eColorFormat = inputBufferFormat;
    } else {        // output port
        pt->eCompressionFormat = OMX_IMAGE_CodingJPEG;
        pt->eColorFormat = OMX_COLOR_FormatUnused;
        // nothing to be check on pt->nIndex
    }
    pt->nIndex = 0xFF;        // dummy value
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamImagePortFormat Color Format : %d ",pt->eColorFormat);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexParamQuantizationTable(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamQuantizationTable <line no %d> ",__LINE__);
    *has_changed = OMX_TRUE;
    OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE *pt = (OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
	switch(pt->eQuantizationTable){
	case OMX_IMAGE_QuantizationTableLuma:
		for(int i=0;i<64;i++)
		_LumaQuantification[i]=pt->nQuantizationMatrix[i];
		break;
	case OMX_IMAGE_QuantizationTableChroma:
		for(int i=0;i<64;i++)
		_ChromaQuantification[i]=pt->nQuantizationMatrix[i];
		break;
	case OMX_IMAGE_QuantizationTableChromaCb:
		 return OMX_ErrorNotImplemented;
        case OMX_IMAGE_QuantizationTableChromaCr:
		 return OMX_ErrorNotImplemented;
        case OMX_IMAGE_QuantizationTableMax:
		 return OMX_ErrorNotImplemented;
        default:
		return OMX_ErrorBadParameter;
        }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexParamQuantizationTable(OMX_PTR pt_org) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamQuantizationTable <line no %d> ",__LINE__);
    OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE *pt = (OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
	switch(pt->eQuantizationTable){
	case OMX_IMAGE_QuantizationTableLuma:
		for(int i=0;i<64;i++)
		pt->nQuantizationMatrix[i]=_LumaQuantification[i];
		break;
	case OMX_IMAGE_QuantizationTableChroma:
		for(int i=0;i<64;i++)
		pt->nQuantizationMatrix[i]=_ChromaQuantification[i];
		break;
	case OMX_IMAGE_QuantizationTableChromaCb:
		 return OMX_ErrorNotImplemented;
        case OMX_IMAGE_QuantizationTableChromaCr:
		 return OMX_ErrorNotImplemented;
        case OMX_IMAGE_QuantizationTableMax:
		 return OMX_ErrorNotImplemented;
        default:
		return OMX_ErrorBadParameter;
        }


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::convertHuffmanTable(t_uint8 pBits[],t_uint8 pVal[], t_uint16 pCode[], t_uint16 pSize[])
{
	OstTraceInt1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In convertHuffmanTable <line no %d> ",__LINE__);
	t_uint16 k,i;
	t_uint8 j;
    t_uint16 lastK;
    t_uint16 huffCode[256];
    t_uint16 huffSize[256];
    t_uint16 code;
    t_uint16 size;

    /* we reset the tables */
    memset (huffCode,0x0,256*sizeof(t_uint16));
    memset (huffSize,0x0,256*sizeof(t_uint16));


    /* generation of huffman code size (CCITT_Rec_T81 figure C.1) */
    /* ---------------------------------------------------------- */

    k=0;i=1;j=1;

startLoopC1:
    if (j > pBits[i])
    {
        i = i + 1;
        j = 1;
        if (i>16) goto endLoopC1;
        else goto startLoopC1;
    }
    else
    {
        huffSize[k] = i;
        k = k +1;
        j = j + 1;
        goto startLoopC1;
    }

endLoopC1:

    huffSize[k] = 0;
    lastK = k;

    /* generation of huffman code table (CCITT_Rec_T81 figure C.2) */
    /* ----------------------------------------------------------- */

    k = 0;
    code = 0;
    size = huffSize[0];
startLoopC2:
    huffCode[k] = code;
    code = code + 1;
    k = k + 1;
    if (huffSize[k] == size) goto startLoopC2;
    else
    {
        if (huffSize[k]==0) goto endLoopC2;
        else goto startCodeLoop;
    }

startCodeLoop:
    code = code << 1;
    size = size +1;
    if (huffSize[k] == size) goto startLoopC2;
    else goto startCodeLoop;

endLoopC2:



    /* reordering of codes  */
    /* -------------------- */
    k = 0;
    do
    {
        i = pVal[k];
        pCode[i] = (t_uint16) huffCode[k];
        pSize[i] = (t_uint16) huffSize[k];
        k++;

    } while (k < lastK);


    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexParamHuffmanTable(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamHuffmanTable <line no %d> ",__LINE__);
    *has_changed = OMX_TRUE;
    OMX_IMAGE_PARAM_HUFFMANTTABLETYPE *pt = (OMX_IMAGE_PARAM_HUFFMANTTABLETYPE *)pt_org;
	t_uint16  *pCode;
	t_uint16  *pSize;
	t_uint16  count1,count2;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
	switch(pt->eHuffmanTable){
	case OMX_IMAGE_HuffmanTableAC:
		for(int i=0;i<16;i++)
		{
		_BITS_ACLuma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_ACChroma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_ACLuma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_ACChroma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
        }
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_ACLuma[i]=pt->nHuffmanTable[i];
		_HUFFVAL_ACChroma[i]=pt->nHuffmanTable[i];
		}

	for(int i=0;i<256;i++)
	{
		ParamLumaHuffmanCodeAcTable[i]=0;
		ParamLumaHuffmanSizeAcTable[i]=0;
		ParamChromaHuffmanCodeAcTable[i]=0;
		ParamChromaHuffmanSizeAcTable[i]=0;
	}

		pCode=ParamLumaHuffmanCodeAcTable;
		pSize=ParamLumaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACLuma_shifted,_HUFFVAL_ACLuma,pCode,pSize);
		pCode=ParamChromaHuffmanCodeAcTable;
		pSize=ParamChromaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACChroma_shifted,_HUFFVAL_ACChroma,pCode,pSize);

		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		break;
	case OMX_IMAGE_HuffmanTableDC:
		for(int i=0;i<16;i++)
		{
		_BITS_DCLuma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_DCChroma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_DCLuma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		_BITS_DCChroma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		}
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_DCLuma[i]=pt->nHuffmanTable[i];
		_HUFFVAL_DCChroma[i]=pt->nHuffmanTable[i];
		}

	for(int i=0;i<12;i++)
	{
		ParamLumaHuffmanCodeDcTable[i]=0;
		ParamLumaHuffmanSizeDcTable[i]=0;
		ParamChromaHuffmanCodeDcTable[i]=0;
		ParamChromaHuffmanSizeDcTable[i]=0;
	}
		pCode=ParamLumaHuffmanCodeDcTable;
		pSize=ParamLumaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCLuma_shifted,_HUFFVAL_DCLuma,pCode,pSize);
		pCode=ParamChromaHuffmanCodeDcTable;
		pSize=ParamChromaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCChroma_shifted,_HUFFVAL_DCChroma,pCode,pSize);

		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		break;
	case OMX_IMAGE_HuffmanTableACLuma:
		count1=0;
		count2=0;
		for(int i=0;i<16;i++)
		{
		_BITS_ACLuma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		if(_BITS_ACLuma[i]==_BITS_ACChroma[i])
		count1++;
		_BITS_ACLuma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		}
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_ACLuma[i]=pt->nHuffmanTable[i];
		if(_HUFFVAL_ACLuma[i]==_HUFFVAL_ACChroma[i])
		count2++;
		}

		for(int i=0;i<256;i++)
	{
		ParamLumaHuffmanCodeAcTable[i]=0;
		ParamLumaHuffmanSizeAcTable[i]=0;
	}
		pCode=ParamLumaHuffmanCodeAcTable;
		pSize=ParamLumaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACLuma_shifted,_HUFFVAL_ACLuma,pCode,pSize);
		if((count2==256)&&(count1==16))
		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		else
		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;
        break;
	case OMX_IMAGE_HuffmanTableACChroma:
		count1=0;
		count2=0;
		for(int i=0;i<16;i++)
		{
		_BITS_ACChroma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		if(_BITS_ACLuma[i]==_BITS_ACChroma[i])
		count1++;
		_BITS_ACChroma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		}
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_ACChroma[i]=pt->nHuffmanTable[i];
		if(_HUFFVAL_ACLuma[i]==_HUFFVAL_ACChroma[i])
		count2++;
		}
		for(int i=0;i<256;i++)
	{
		ParamChromaHuffmanCodeAcTable[i]=0;
		ParamChromaHuffmanSizeAcTable[i]=0;
	}
		pCode=ParamChromaHuffmanCodeAcTable;
		pSize=ParamChromaHuffmanSizeAcTable;
		convertHuffmanTable(_BITS_ACChroma_shifted,_HUFFVAL_ACChroma,pCode,pSize);
		if((count2==256)&&(count1==16))
		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		else
		SAME_AC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;
		break;
	case OMX_IMAGE_HuffmanTableDCLuma:
		count1=0;
		count2=0;
		for(int i=0;i<16;i++)
		{
		_BITS_DCLuma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		if(_BITS_DCLuma[i]==_BITS_DCChroma[i])
		count1++;
		_BITS_DCLuma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		}
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_DCLuma[i]=pt->nHuffmanTable[i];
		if(_HUFFVAL_DCLuma[i]==_HUFFVAL_DCChroma[i])
		count2++;
		}
		for(int i=0;i<12;i++)
	    {
		ParamLumaHuffmanCodeDcTable[i]=0;
		ParamLumaHuffmanSizeDcTable[i]=0;
		}

		pCode=ParamLumaHuffmanCodeDcTable;
		pSize=ParamLumaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCLuma_shifted,_HUFFVAL_DCLuma,pCode,pSize);
		if((count2==256)&&(count1==16))
		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		else
		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;
		break;
	case OMX_IMAGE_HuffmanTableDCChroma:
		count1=0;
		count2=0;
		for(int i=0;i<16;i++)
		{
		_BITS_DCChroma[i]=pt->nNumberOfHuffmanCodeOfLength[i];
		if(_BITS_DCLuma[i]==_BITS_DCChroma[i])
		count1++;
		_BITS_DCChroma_shifted[i+1]=pt->nNumberOfHuffmanCodeOfLength[i];
		}
		for(int i=0;i<256;i++)
		{
		_HUFFVAL_DCChroma[i]=pt->nHuffmanTable[i];
		if(_HUFFVAL_DCLuma[i]==_HUFFVAL_DCChroma[i])
		count2++;
		}
		for(int i=0;i<12;i++)
	{
        ParamChromaHuffmanCodeDcTable[i]=0;
		ParamChromaHuffmanSizeDcTable[i]=0;
	}
		pCode=ParamChromaHuffmanCodeDcTable;
		pSize=ParamChromaHuffmanSizeDcTable;
		convertHuffmanTable(_BITS_DCChroma_shifted,_HUFFVAL_DCChroma,pCode,pSize);
		if((count2==256)&&(count1==16))
		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_TRUE;
		else
		SAME_DC_HUFFMAN_TABLE_PROVIDED=OMX_FALSE;
		break;
	case OMX_IMAGE_HuffmanTableMax:
		return OMX_ErrorNotImplemented;
	default:
		return OMX_ErrorBadParameter;
        }

	OstTraceFiltInst0(TRACE_DEBUG, "JPEGENC_ParamNConfig : setIndexParamHuffmanTable Done \n");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexParamHuffmanTable(OMX_PTR pt_org) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamHuffmanTable <line no %d> ",__LINE__);
    OMX_IMAGE_PARAM_HUFFMANTTABLETYPE *pt = (OMX_IMAGE_PARAM_HUFFMANTTABLETYPE *)pt_org;
    RETURN_XXX_IF_WRONG(pt->nPortIndex==IPB+0 || pt->nPortIndex==IPB+1, OMX_ErrorBadPortIndex);
	switch(pt->eHuffmanTable){
	case OMX_IMAGE_HuffmanTableAC:
		if(SAME_AC_HUFFMAN_TABLE_PROVIDED==OMX_FALSE)
		return OMX_ErrorSeperateTablesUsed;
		else
		{
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_ACLuma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_ACLuma[i];
        break;
		}
	case OMX_IMAGE_HuffmanTableDC:
		if(SAME_DC_HUFFMAN_TABLE_PROVIDED==OMX_FALSE)
		return OMX_ErrorSeperateTablesUsed;
		else
		{
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_DCLuma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_DCLuma[i];
        break;
		}
	case OMX_IMAGE_HuffmanTableACLuma:
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_ACLuma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_ACLuma[i];
        break;
	case OMX_IMAGE_HuffmanTableACChroma:
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_ACChroma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_ACChroma[i];
		break;
	case OMX_IMAGE_HuffmanTableDCLuma:
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_DCLuma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_DCLuma[i];
		break;
	case OMX_IMAGE_HuffmanTableDCChroma:
		for(int i=0;i<16;i++)
		pt->nNumberOfHuffmanCodeOfLength[i]=_BITS_DCChroma[i];
		for(int i=0;i<256;i++)
		pt->nHuffmanTable[i]=_HUFFVAL_DCChroma[i];
		break;
	case OMX_IMAGE_HuffmanTableMax:
		return OMX_ErrorNotImplemented;
	default:
		return OMX_ErrorBadParameter;
	}


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexParamPortDefinition(OMX_PTR pt_org) const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamPortDefinition <line no %d> ",__LINE__);
    return OMX_ErrorNotImplemented;
}

t_uint16 JPEGEnc_ParamAndConfig::getRestartInterval() const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getRestartInterval <line no %d> ",__LINE__);
    t_uint32 width = getComponent()->getFrameWidth(IPB+1);
    width = (width+0xf) & (~0xf);
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getRestartInterval width : %d ",width);
    return (8*width) / 16;
}
OMX_ERRORTYPE JPEGEnc_ParamAndConfig::setIndexParamResourceSvaMcps(OMX_PTR pPtr){
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamResourceSvaMcps <line no %d> ",__LINE__);
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    OMX_U32 mcps = pMcpsType->nSvaMips;
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In setIndexParamResourceSvaMcps MIPS Value : %d ",pMcpsType->nSvaMips);
    if (mcps > 200) {
        return OMX_ErrorBadParameter;
    }
    resourceSvaMcps(mcps);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_ParamAndConfig::getIndexParamResourceSvaMcps(OMX_PTR pPtr) const{
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamResourceSvaMcps <line no %d> ",__LINE__);
    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    pMcpsType->nSvaMips = resourceSvaMcps();
	OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ParamAndConfig : In getIndexParamResourceSvaMcps MIPS Value : %d ",pMcpsType->nSvaMips);
    return OMX_ErrorNone;
}
