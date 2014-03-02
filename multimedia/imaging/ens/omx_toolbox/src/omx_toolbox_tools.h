/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _OMX_TOOLBOX_TOOLS_H_
#define _OMX_TOOLBOX_TOOLS_H_

#ifndef OMX_TOOLBOX_API
	#ifdef __SYMBIAN32__
		#define OMX_TOOLBOX_API
	#else
		#define OMX_TOOLBOX_API
	#endif
#endif

#define INIT_OMX_STRUCT_INDEX(type, name) name.nSize=sizeof(type); name.nPortIndex=0;\
	getImgOmxIlSpecVersion(&name.nVersion);

enum
{
	eNoError          =0,
	eNullPointer      =1,
	eUnsupportedFormat=2,
	eNotImplemented   =3,
	eMemoryAlloc      =4,
	eOtherError       =5
};

enum
{
	//eError_None,
	eError_CannotAllocate_PortInfo = -1000,
	eError_CannotAllocate_Ports,
	eError_CannotAllocateHeader,
	eError_CannotAllocateHeaderList,
	eError_CorruptedMetada,
	eError_WOM_LastError,
};

OMX_TOOLBOX_API float ComputePixelDepth(OMX_U32 format);
OMX_TOOLBOX_API int FormatUserComponentName(char *&aUserName, const char *aRoot, const char *aName);

class Wom_AsyncPort ;
class Wom_Processor;

typedef struct
//*************************************************************************************************************
{
	int         ColorFormat;
	float       OverallPixelDepth;
	int         NbPlan;
	float       PixelDepth0;
	float       PixelDepth1;
	float       PixelDepth2;
	const char *Name;
	const char *Description;
} _sImageFormatDescription;

typedef struct
{
	unsigned int              ImageSize;          // Size of image in bytes
	unsigned int              StrideInPixel;      // The stride in pixel not in byte
	const _sImageFormatDescription *pFormatDescription; // if skipping is enable
	void Init() { ImageSize=0; StrideInPixel=0; pFormatDescription=NULL;}
} _tImageInfo;

struct sBufferInfo
{
	OMX_BUFFERHEADERTYPE *pBufferHdr;
	int                   Format;
	int                   Width;
	int                   Height;
	int                   Stride;
	const Wom_AsyncPort  *pPort;
	int                   Error; // Output
	const _tImageInfo    *pImageInfo; //The image info stuct
};


/**
@brief Structur used for internal processing of buffers
*/
class AsyncProcessorFSM;
typedef struct
{
	Wom_Processor *pProcessor;
	struct sBufferInfo Input;
	struct sBufferInfo Output;
	struct sBufferInfo Metadata;
	void              *DataPtr; //Use for internal processing
	void              *UserPtr;
} _tConvertionConfig;

OMX_TOOLBOX_API int ComputeImageSize(const int format, const size_t stride, const size_t height, size_t &ComputeSize);

OMX_TOOLBOX_API const _sImageFormatDescription* GetImageFormatDescription(int format);

OMX_TOOLBOX_API const char *GetStringOmxState(int aState);
OMX_TOOLBOX_API const char *GetStringOmxEvent(int aEvent);
OMX_TOOLBOX_API const char *GetStringOmxCommand(int aCmd);
OMX_TOOLBOX_API const char *GetStringOmxError(OMX_ERRORTYPE aError);

OMX_TOOLBOX_API int GetExtraData(const OMX_BUFFERHEADERTYPE* pOmxBufHdr, const int aStructType, char *&pStruct, size_t *size=NULL);

#if ( defined (__SYMBIAN32__) && !defined(GET_AND_SET_TRACE_LEVEL))
	#define GET_AND_SET_TRACE_LEVEL(a)
#endif


#endif // _OMX_TOOLBOX_TOOLS_H_
