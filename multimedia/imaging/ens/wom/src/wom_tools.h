/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_TOOLS_H_
#define _WOM_TOOLS_H_

#if 0
typedef struct
//*************************************************************************************************************
{ //Description of image format
	int         ColorFormat;
	float       OverallPixelDepth;
	int         NbPlan;
	float       PixelDepth0;
	float       PixelDepth1;
	float       PixelDepth2;
	const char *Name;
	const char *Description;
} _sWomImageFormatDescription;

typedef struct
//*************************************************************************************************************
{
	unsigned int              ImageSize;          // Size of image in bytes
	unsigned int              StrideInPixel;      // The stride in pixel not in byte
	const _sWomImageFormatDescription *pFormatDescription;
	void Init() { ImageSize=0; StrideInPixel=0; pFormatDescription=NULL;}
} _tWomImageInfo;
#endif


struct sWomBufferInfo
//*************************************************************************************************************
{
	OMX_BUFFERHEADERTYPE *pBufferHdr;
	int                   Format;
	int                   Width;
	int                   Height;
	int                   Stride;
	int                   SliceHeight;
	const void           *pPort;
	int                   Error; // Output
	//const _tWomImageInfo *pImageInfo; //The image info stuct
	void InitDefault();
};

/**
@brief Structur used for internal processing of buffers
*/
typedef struct
//*************************************************************************************************************
{
	void                 *pProcessor;
	struct sWomBufferInfo Input;
	struct sWomBufferInfo Output;
	struct sWomBufferInfo Metadata;
	void                 *DataPtr; //Use for internal processing
	void                 *UserPtr;
} _tWomConvertionConfig;


//WOM_API int ComputeImageSize(const int format, const size_t stride, const size_t height, size_t &ComputeSize);

// Define when to send a callback
#define WOM_BUFFERFLAG_CALLBACK_MASK   (OMX_BUFFERFLAG_EOS )

#endif //_WOM_TOOLS_H_
