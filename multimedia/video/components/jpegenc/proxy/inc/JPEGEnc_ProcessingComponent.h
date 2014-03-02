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


#ifndef __JPEGENC_Processing_H
#define __JPEGENC_Processing_H

#include "osi_trace.h"
//#include "JPEGEnc_Proxy.h"
#include "VFM_Port.h"
#include "host/eventhandler.hpp"
//#include "SharedBuffer.h"
//#include "hw_buffer_lib.h"

#include "t1xhv_vec_jpeg.idt"
#include "vfm_vec_jpeg.idt"

#include <cm/inc/cm_macros.h>
#include "jpegenc/arm_nmf/jpegenc_sw.hpp"
#ifdef HVA_JPEGENC
#include "jpegenc/arm_nmf/hva_jpegenc.hpp"
#endif
#include "t1xhv_vec_jpeg.idt"
#include "vfm_vec_jpeg.idt"

class JPEGEnc_Proxy;

class JPEGEnc_ProcessingComponent
{
	public:

	void setValue(OMX_U32 value)
	{
		headerMPCPosition = value;
	}
	void setHeaderSizeValue(OMX_U32 value)
	{
		headerSizePosition = value;
	}

	JPEGEnc_ProcessingComponent();

	OMX_ERRORTYPE addEXIFHeader(JPEGEnc_Proxy *jpegenc,OMX_BUFFERHEADERTYPE* pBuffer,OMX_U8 **header_start,
								OMX_U8 **exifheader_start,OMX_U32 *exifheaderSize,
								OMX_U32 nHeaderSize);

	OMX_U8 fromBufferTypeToJPEGType(JPEGEnc_Proxy *jpegenc);

	// Quantization tables and huffman tables can be found in
	// - reference code: file jpeg_globals.h  (same as in the JPEG recommendations of the specifications
	// - validation code: file val_JPEGEnc_Proxy.c (differents)
	void compute_quantization_table(JPEGEnc_Proxy *jpegenc);

	//function for creating the header of the JPEG image
	void headerCreation(JPEGEnc_Proxy *jpegenc,OMX_U8 **,OMX_U32 *);

	// for programming the IN paramters of the FW structure
	void set_pJecIn_parameters(JPEGEnc_Proxy *jpegenc, ts_t1xhv_vec_jpeg_param_in *pJecIn_parameters);

private:
	OMX_U32 headerMPCPosition;
	OMX_U32 headerSizePosition;
	OMX_U32 *tempPosition;

};

#endif // __JPEGENC_Processing_H

