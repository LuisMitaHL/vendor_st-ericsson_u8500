/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WRAPPER_OPENMAX_TOOLS_H_
#define _WRAPPER_OPENMAX_TOOLS_H_

#define S_OK 0

#ifndef WRAPPER_OPENMAX_API
	#ifdef __SYMBIAN32__
		#define WRAPPER_OPENMAX_API IMPORT_C
	#else
		#define WRAPPER_OPENMAX_API IMPORT_C
		#define IMPORT_C
		#define EXPORT_C
	#endif
#endif

#define INIT_OMX_STRUCT_INDEX(type, name) name.nSize=sizeof(type); name.nPortIndex=0;\
	getOmxIlSpecVersion(&name.nVersion);

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


enum Enum_ReportInfo
{
	eReport_Error  =0x01000000,
	eReport_Warning=0x02000000,
	eReport_Debug  =0x03000000,
	eReport_Info   =0x04000000,
	eReport_Msg    =0x05000000,
	eReport_Event  =0x06000000,
	
	eReport_Other  =0xFF000000,

	eReport_UsrMask =0x00FFFFFF, //Part dedicated to user
	eReport_TypeMask=0xFF000000, //Part dedicated to type of reporting
};

typedef int (*_tReportFunction)(int aWhat, const char *aStr,...);

extern int (*_fnReport)(int aWhat, const char *aStr,...); // function pointer for reporting

IMPORT_C int _DefaultReport(int aWhat, const char *aStr,...); // Default reporting function
IMPORT_C void *SetReportingFunc(_tReportFunction ptr=(_tReportFunction)NULL);

typedef struct
{
	void   * Ptr;
	size_t Size;
	void Clear() { Size=0;}
} _tArray;

#if 0  // doesn't compile for x86-linux
typedef struct :public _tArray
{
	size_t   Allocated;
} _tSizableArray;
#endif

// Debug macro
#if (!defined(__SYMBIAN32__) && !defined(__MODULE__))
	#define __MODULE__ __FILE__
#endif

#ifndef WOM_ASSERT
	// Use of !! to workaround overload of :! operator that can end at ||
	#define WOM_ASSERT(a) (void) ( (!!(a)) || (ReportError( -1, "ASSERTION %s:%d\n", __MODULE__, __LINE__), 0) )
#endif

#define WARNING(a) if (!((bool)a)) { _fnReport(eReport_Warning, "\nWARNING assertion %s:%d \n", __MODULE__, __LINE__); } else {  }

IMPORT_C float ComputePixelDepth(OMX_U32 format);
IMPORT_C int FormatUserComponentName(char *&aUserName, const char *aRoot, const char *aName);


class Port;
class openmax_processor;

IMPORT_C int ReportError(int error, const char *format, ...);

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
	const Port           *pPort;
	int                   Error; // Output
	const _tImageInfo    *pImageInfo; //The image info stuct 
};

/**
@brief Structur used for internal processing of buffers
*/
typedef struct
{
	openmax_processor  *pProcessor;
	struct sBufferInfo Input;
	struct sBufferInfo Output;
	struct sBufferInfo Metadata;
	// IFM_CONFIG_FILTER  ImageConfig;
	void              *DataPtr; //Use for internal processing
	void              *UserPtr;
} _tConvertionConfig;

WRAPPER_OPENMAX_API int ComputeImageSize(const int format, const size_t stride, const size_t height, size_t &ComputeSize);

WRAPPER_OPENMAX_API const _sImageFormatDescription* GetImageFormatDescription(int format);

WRAPPER_OPENMAX_API const char *GetStringOmxState(unsigned int aState);
WRAPPER_OPENMAX_API const char *GetStringOmxEvent(unsigned int aEvent);
WRAPPER_OPENMAX_API const char *GetStringOmxCommand(unsigned int aCmd);

class WRAPPER_OPENMAX_API ReferenceCount
//*************************************************************************************************************
{
public:
	/* WRAPPER_OPENMAX_API */ ReferenceCount() { Count=0;}
	/* WRAPPER_OPENMAX_API */ ~ReferenceCount() { WOM_ASSERT(Count ==0); }
	/* WRAPPER_OPENMAX_API */unsigned int Get() const {return(Count);}
	/* WRAPPER_OPENMAX_API */operator unsigned int () const {return(Count);}
	/* WRAPPER_OPENMAX_API */unsigned int Ref() {++Count; return(Count); }
	/* WRAPPER_OPENMAX_API */unsigned int UnRef() { WOM_ASSERT(Count !=0); if (Count >0) --Count; return(Count); }
protected:
	unsigned int Count;
};

WRAPPER_OPENMAX_API int GetExtraData(const OMX_BUFFERHEADERTYPE* pOmxBufHdr, const int aStructType, char *&pStruct, size_t *size=NULL);



#if ( defined (__SYMBIAN32__) && !defined(GET_AND_SET_TRACE_LEVEL))
	#define GET_AND_SET_TRACE_LEVEL(a)
#endif

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
// Generic part not OMX dependant


#if defined(__DEBUG)
	#ifdef _MSC_VER
		#define WOM_BREAKPOINT _asm int 3;
	#elif defined(WORKSTATION)
		#define WOM_BREAKPOINT asm ("int $3;");
	#else
		#define WOM_BREAKPOINT ;
	#endif
#else
	#define WOM_BREAKPOINT ;
#endif

WRAPPER_OPENMAX_API size_t RoundAddress(size_t val, size_t PowOf2);

#endif // _WRAPPER_OPENMAX_TOOLS_H_
