/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include "osi_trace.h"
#ifndef __SYMBIAN32__
#include <OMX_Symbian_IVCommonExt_Ste.h>
#else
#include <OMX_Symbian_IVCommonExt.h>
#endif

#include "../include/wrapper_openmax_tools.h"

const _sImageFormatDescription OMX_ImageFormatDescription[]=
{ // Supported formats
	// OMXColorFormat                                  ,OverallPixelDepth   , PixelDepth2
	//                                                       , NbPlan           , Name
	//                                                          , PixelDepth0
	//                                                                , PixelDepth1                                                 , Description
	{ OMX_COLOR_FormatUnused                           , 0.  , 0, 0.  ,  0.,  0., "OMX_COLOR_FormatUnused"                          , "Format unused, pixelDepth = 0. This should not occur !"},
	{ OMX_COLOR_FormatRawBayer8bit                     , 1.0 , 1, 1.0 , 0.0, 0.0, "OMX_COLOR_FormatRawBayer8bit"                    , "RawBayer 8bits"},
	{ OMX_COLOR_Format16bitRGB565                      , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_Format16bitRGB565"                     , "RGB565"},
	{ OMX_COLOR_Format16bitBGR565                      , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_Format16bitBGR565"                     , "BGR565"},
	{ OMX_COLOR_FormatYUV422PackedPlanar               , 2.0 , 3, 1.0 , 0.5, 0.5, "OMX_COLOR_FormatYUV422PackedPlanar"              , ""},
	{ OMX_COLOR_FormatYCbYCr                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYCbYCr"                          , "YCbYCr interlaced"},
	{ OMX_COLOR_FormatYCrYCb                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYCrYCb"                          , "YCrYCb interlaced"},
	{ OMX_COLOR_FormatCbYCrY                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatCbYCrY"                          , "CbYCrY interlaced"},
	{ OMX_COLOR_FormatCrYCbY                           , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatCrYCbY"                          , "CrYCbY interlaced"},
	{ OMX_COLOR_Format24bitRGB888                      , 3.0 , 1, 3.0 , 0.0, 0.0, "OMX_COLOR_Format24bitRGB888"                     , "RGB888 interlaced"},
	{ OMX_COLOR_Format24bitBGR888                      , 3.0 , 1, 3.0 , 0.0, 0.0, "OMX_COLOR_Format24bitBGR888"                     , "BGR888 interlaced"},
	{ OMX_COLOR_FormatYUV420PackedPlanar               , 1.5 , 3, 1.0 , .25, .25, "OMX_COLOR_FormatYUV420PackedPlanar"              , "YUV420MB old name for 420mb"},
	{ OMX_COLOR_FormatYUV420SemiPlanar                 , 1.5 , 2, 1.0 , 0.5, 0.0, "OMX_COLOR_FormatYUV420SemiPlanar"                , "NV21"},

	{ OMX_COLOR_FormatYUV422SemiPlanar                 , 2.0 , 1, 2.0 , 0.0, 0.0, "OMX_COLOR_FormatYUV422SemiPlanar"                , "mapped to STE 422mb"},
	{ OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar , 1.5 , 1, 1.5 , 0.0, 0.0, "YUV420MB"                                        , "STE 420MB"},
	{ OMX_SYMBIAN_COLOR_FormatRawBayer12bit            , 1.5 , 1, 1.5 , 0.0, 0.0, "OMX_SYMBIAN_COLOR_FormatRawBayer12bit"           , "RawBayer 12bits"},

	{ OMX_COLOR_Format32bitBGRA8888                    , 4.0 , 1, 4.0 , 0.0, 0.0, "OMX_COLOR_Format32bitBGRA8888"                   , "BGRA8888 32 bits"},
	{ OMX_COLOR_Format32bitARGB8888                    , 4.0 , 1, 4.0 , 0.0, 0.0, "OMX_COLOR_Format32bitARGB8888"                   , "ARGB8888 32 bits"},

	{ OMX_COLOR_FormatRawBayer10bit                    , 1.25, 1, 1.25,  0.,  0., "OMX_COLOR_FormatRawBayer10bit"                   , "RawBayer 10bits"},
	{ OMX_COLOR_Format12bitRGB444                      , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_Format12bitRGB444"                     , "12bits RGB444"},
	{ OMX_COLOR_Format16bitARGB4444                    , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_Format16bitARGB4444"                   , "16bits ARGB4444"},
	{ OMX_COLOR_Format16bitARGB1555                    , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_Format16bitARGB1555"                   , "16bits ARGB1555"},
	{ OMX_COLOR_FormatMonochrome                       , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_FormatMonochrome"                      , "Monochrome"},
	{ OMX_COLOR_Format8bitRGB332                       , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_Format8bitRGB332"                      , "8bit RGB332"},

	{ OMX_COLOR_Format18bitRGB666                      ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18bitRGB666"                     , "18bits RGB666"  },
	{ OMX_COLOR_Format18bitARGB1665                    ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18bitARGB1665"                   , "18bits ARGB1665"},
	{ OMX_COLOR_Format19bitARGB1666                    ,2.375, 1,2.375,  0.,  0., "OMX_COLOR_Format19bitARGB1666"                   , "19bits ARGB1666"},
	{ OMX_COLOR_Format24bitARGB1887                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24bitARGB1887"                   , "24bits ARGB1887"},
	{ OMX_COLOR_Format18BitBGR666                      ,2.25 , 1, 2.25,  0.,  0., "OMX_COLOR_Format18BitBGR666"                     , "18Bits BGR666"  },
	{ OMX_COLOR_Format24BitARGB6666                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24BitARGB6666"                   , "24Bits ARGB6666"},
	{ OMX_COLOR_Format24BitABGR6666                    ,3.   , 1, 3.  ,  0.,  0., "OMX_COLOR_Format24BitABGR6666"                   , "24Bits ABGR6666"},
	{ OMX_COLOR_Format25bitARGB1888                    ,3.125, 1,3.125,  0.,  0., "OMX_COLOR_Format25bitARGB1888"                   , "25bits ARGB1888"},

	{ OMX_COLOR_FormatRawBayer8bitcompressed           ,  1. , 1,  1. ,  0.,  0., "OMX_COLOR_FormatRawBayer8bitcompressed"          , "RawBayer 8bits compressed"},
/*
	{ OMX_COLOR_FormatYUV422Planar                     ,  2. , 1,  2. ,  0.,  0., "OMX_COLOR_FormatYUV422Planar"                    , ""},
	{ OMX_COLOR_FormatYUV422PackedSemiPlanar           ,  2. , 1,  2. ,  0.,  0., "OMX_COLOR_FormatYUV422PackedSemiPlanar"          , ""},

	{ OMX_COLOR_FormatYUV411Planar                     ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV411Planar"                    , ""},
	{ OMX_COLOR_FormatYUV411PackedPlanar               ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV411PackedPlanar"              , ""},
	{ OMX_COLOR_FormatYUV444Interleaved                ,  4. , 1,  4. ,  0.,  0., "OMX_COLOR_FormatYUV444Interleaved"               , ""},

	{ OMX_COLOR_FormatYUV420PackedSemiPlanar           , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_FormatYUV420PackedSemiPlanar"          , ""},
	{ OMX_COLOR_FormatYUV420Planar                     , 1.5 , 1, 1.5 ,  0.,  0., "OMX_COLOR_FormatYUV420Planar"                    , ""},
*/
	{ OMX_COLOR_FormatL2                               , 0.25, 1, 0.25,  0.,  0., "OMX_COLOR_FormatL2"                              , ""},
	{ OMX_COLOR_FormatL4                               , 0.5 , 1, 0.5 ,  0.,  0., "OMX_COLOR_FormatL4"                              , ""},
	{ OMX_COLOR_FormatL8                               , 1.  , 1, 1.  ,  0.,  0., "OMX_COLOR_FormatL8"                              , ""},
	{ OMX_COLOR_FormatL16                              , 2.  , 1, 2.  ,  0.,  0., "OMX_COLOR_FormatL16"                             , ""},
	{ OMX_COLOR_FormatL24                              , 3.  , 1, 3.  ,  0.,  0., "OMX_COLOR_FormatL24"                             , ""},
	{ OMX_COLOR_FormatL32                              , 4.  , 1, 4.  ,  0.,  0., "OMX_COLOR_FormatL32"                             , ""},
};

#define ARRAYSIZE(a) sizeof(a)/sizeof(*a)

WRAPPER_OPENMAX_API const _sImageFormatDescription* GetImageFormatDescription(int format)
//*************************************************************************************************************
{ // Return the struct associated to the given format
	const _sImageFormatDescription *ptr=OMX_ImageFormatDescription;
	size_t Nbr=ARRAYSIZE(OMX_ImageFormatDescription);
	while(Nbr > 0)
	{
		if (ptr->ColorFormat ==format)
			return(ptr);
		++ptr;
		--Nbr;
	}

	return(NULL);
}

WRAPPER_OPENMAX_API int ComputeImageSize(const int format, const size_t stride, const size_t height, size_t &ComputeSize)
//*************************************************************************************************************
{ // Return the compute size of the image for the given format and 
	const _sImageFormatDescription*pFormat= GetImageFormatDescription(format);
	if ( (pFormat==NULL) || (pFormat->PixelDepth0==0.) )
	{
		ComputeSize=0;
		return(-1);
	}
	ComputeSize= (size_t)((stride*height* pFormat->OverallPixelDepth) / pFormat->PixelDepth0);
	return(S_OK);
}


//Set Default reporting function
/*WRAPPER_OPENMAX_API*/ _tReportFunction _fnReport=_DefaultReport;

WRAPPER_OPENMAX_API void *SetReportingFunc(_tReportFunction ptr)
//*************************************************************************************************************
{ // Set the reporting function
	if (ptr==NULL)
		ptr=_DefaultReport;
	void * Oldfunc=(void *)_fnReport;
	_fnReport=ptr;
	return(Oldfunc);
}

WRAPPER_OPENMAX_API float ComputePixelDepth(OMX_U32 format)
//*************************************************************************************************************
{
	switch (format) 
	{
	case OMX_COLOR_FormatUnused :
		MSG0("Format unused, pixelDepth = 0. This should not occur !\n");
		return 0.;

	case OMX_COLOR_FormatMonochrome :
	case OMX_COLOR_Format8bitRGB332 :
	case OMX_COLOR_FormatRawBayer8bit :
		MSG0("Monochrome image format !\n");
		return 1.;

	case OMX_COLOR_Format12bitRGB444 :
	case OMX_COLOR_Format16bitARGB4444 :
	case OMX_COLOR_Format16bitARGB1555 :
	case OMX_COLOR_Format16bitRGB565 :
	case OMX_COLOR_Format16bitBGR565 :
	case OMX_COLOR_FormatYUV422Planar :
	case OMX_COLOR_FormatYUV422PackedPlanar :
	case OMX_COLOR_FormatYUV422SemiPlanar :
	case OMX_COLOR_FormatYUV422PackedSemiPlanar :
	case OMX_COLOR_FormatRawBayer10bit :
	case OMX_COLOR_FormatYCbYCr :
	case OMX_COLOR_FormatYCrYCb :
	case OMX_COLOR_FormatCbYCrY :
	case OMX_COLOR_FormatCrYCbY :
		MSG0("2 bytes per pixel used\n");
		return 2.;

	case OMX_COLOR_Format18bitRGB666 :
	case OMX_COLOR_Format18bitARGB1665 :
	case OMX_COLOR_Format19bitARGB1666 :
	case OMX_COLOR_Format24bitRGB888 :
	case OMX_COLOR_Format24bitBGR888 :
	case OMX_COLOR_Format24bitARGB1887 :
	case OMX_COLOR_Format18BitBGR666 :
	case OMX_COLOR_Format24BitARGB6666 :
	case OMX_COLOR_Format24BitABGR6666 :
		MSG0("3 bytes per pixel used\n");
		return 3.;

	case OMX_COLOR_Format25bitARGB1888 :
	case OMX_COLOR_Format32bitBGRA8888 :
	case OMX_COLOR_Format32bitARGB8888 :
	case OMX_COLOR_FormatYUV411Planar :
	case OMX_COLOR_FormatYUV411PackedPlanar :
	case OMX_COLOR_FormatYUV444Interleaved :
	case OMX_COLOR_FormatRawBayer8bitcompressed :
		MSG0("4 bytes per pixel used\n");
		return 4.;

	case OMX_COLOR_FormatYUV420PackedSemiPlanar :
	case OMX_COLOR_FormatYUV420Planar :
	case OMX_COLOR_FormatYUV420PackedPlanar :
	case OMX_COLOR_FormatYUV420SemiPlanar :
	case OMX_SYMBIAN_COLOR_FormatRawBayer12bit : // is used for raw12!!!
	case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
		MSG0("1.5 bytes per pixel used\n");
		return 1.5;

	case OMX_COLOR_FormatL2 :
	case OMX_COLOR_FormatL4 :
	case OMX_COLOR_FormatL8 :
	case OMX_COLOR_FormatL16 :
	case OMX_COLOR_FormatL24 :
	case OMX_COLOR_FormatL32 :
		MSG1("Could not evaluate pixel depth for format 0x%lx\n", format);
		return 4.;

	default :
		MSG0("Format Unkno, pixelDepth = 0. This should not occur !\n");
		return 0.;
	}
}

const char *Report_ErrorString[2]={"<ERROR>", "</ERROR>\n"};
const char *Report_WarningString[2]={"<WARNING>", "</WARNING>\n"};

EXPORT_C int _DefaultReport(int aWhat, const char *aStr,...)
//*************************************************************************************************************
{ // Centralized function for reporting info
	int res=S_OK;
	const char *PreString=NULL;
	const char *PostString=NULL;
	switch (aWhat & eReport_TypeMask)
	{
	case eReport_Error:
		res=-1;
		PreString =Report_ErrorString[0];
		PostString=Report_ErrorString[1];
		break;
	case eReport_Warning:
		res=S_OK;
		break;
	case eReport_Debug:
		res=S_OK;
		break;
	case eReport_Info:
		res=S_OK;
		break;
	case eReport_Msg:
		res=S_OK;
		break;
	case eReport_Event:
		res=S_OK;
		break;

	case eReport_Other:
	default:
		res=S_OK;
		break;
	}
	va_list list;
	va_start(list, aStr);
#ifndef __SYMBIAN32__
	if (PreString!=NULL)
		MSG0("%s", PreString);
	vfprintf(stdout, aStr, list);
	if (PostString!=NULL)
		MSG0("%s", PostString);
#else
	static char ErrorString[1024];
	if (PreString!=NULL)
		RDebug::Printf(PreString);
	vsprintf(ErrorString, aStr, list);
	RDebug::Printf(ErrorString);
	if (PostString!=NULL)
		RDebug::Printf(PostString);
#endif
	va_end(list);
	return(res);
}

WRAPPER_OPENMAX_API int FormatUserComponentName(char *&aUserName, const char *aRoot, const char *aName)
//*************************************************************************************************************
{ // Format user name for tracing. reallocate username for having sufficient space
	if (aUserName!=NULL)
		delete [] aUserName;
	size_t len1, len2;
	if (aName==NULL)
		aName="";
	if (aRoot==NULL)
		aRoot="";
	len1=strlen(aRoot);
	len2=strlen(aName);
	int maxsize=len1 +len2+3;
	aUserName=new char [ maxsize];
	if (len1!=0)
	{
		strncpy(aUserName, aRoot, len1);
		strncpy(aUserName+len1, "->", 2);
		len1+=2;
	}
	if (len2!=0)
	{
		strncpy(aUserName+len1, aName, len2);
		len1+=len2;
	}
	if ((len1==0) && (len2==0))
	{
		strncpy(aUserName, "?", 2);
		len1=2;
	}
	*(aUserName+len1)='\0';
	WOM_ASSERT(len1 < (size_t)maxsize);
	return(S_OK);
}

EXPORT_C int ReportError(int error, const char *format, ...)
//*************************************************************************************************************
{
	static int m_LastError=0;
	if (error != 0)
	{
		if (m_LastError ==0)
			m_LastError=error;  //Memorize the error
		va_list list;
		va_start(list, format);
#ifndef __SYMBIAN32__
		fprintf (stderr, "\n<%s Error %d >", "OpenMax_Component", error);
		vfprintf(stderr, format, list);
#else
		static char ErrorString[1024];
		vsprintf(ErrorString, format, list);
		RDebug::Printf("\nError %d: %s", error, ErrorString);
#endif
		va_end(list);
	}
	return(0);
}


EXPORT_C const char *GetStringOmxState(unsigned int aState)
//*******************************************************************************
{
	static const char * const StateName[] = {
		"OMX_StateInvalid",
		"OMX_StateLoaded",
		"OMX_StateIdle",
		"OMX_StateExecuting",
		"OMX_StatePause",
		"OMX_StateWaitForResources"
	}; //[OMX_StateMax];

	unsigned int size=sizeof(StateName)/sizeof(StateName[0]);
	if ( /*(0<=aState) &&*/ (aState<size) )
		return StateName[aState];
	else
		return "Out of range value for OMX_STATETYPE";
}


EXPORT_C const char *GetStringOmxEvent(unsigned int aEvent)
//*******************************************************************************
{
	static const char * const EventName[] = {
		"OMX_EventCmdComplete",
		"OMX_EventError",
		"OMX_EventMark",
		"OMX_EventPortSettingsChanged",
		"OMX_EventBufferFlag",
		"OMX_EventResourcesAcquired",
		"OMX_EventComponentResumed",
		"OMX_EventDynamicResourcesAvailable",
		"OMX_EventPortFormatDetected"
	}; //[OMX_EventMax];

	unsigned int size=sizeof(EventName)/sizeof(EventName[0]);
	if ( /*(0 <=aEvent) &&*/ (aEvent < size) )
		return EventName[aEvent];
	else
		return "Out of range value for OMX_EVENTTYPE";
}


static const char * const OmxCommandName[] = {
	"OMX_CommandStateSet",
	"OMX_CommandFlush",
	"OMX_CommandPortDisable",
	"OMX_CommandPortEnable",
	"OMX_CommandMarkBuffer",
};

EXPORT_C const char *GetStringOmxCommand(unsigned int aCmd)
//*******************************************************************************
{
	unsigned int size=sizeof(OmxCommandName)/sizeof(OmxCommandName[0]);
	if (aCmd < size)
		return OmxCommandName[aCmd];
	else
		return "Out of range value for OMX_COMMANDTYPE";
}

/**
Return the ExtraData structur associated to given index
*/
EXPORT_C int GetExtraData(const OMX_BUFFERHEADERTYPE* pOmxBufHdr, const int aStructType, char *&pStruct, size_t *size)
//*************************************************************************************************************
{	/* get extradata start pointer */
	OMX_U8* pEnd=pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;
	OMX_U8* pExtraData = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtraData+ 3) & ~0x3);
	while ( ((OMX_U8*)pCurExtra < pEnd) &&  (pCurExtra->nSize>0) && (pCurExtra->eType!=OMX_ExtraDataNone) )
	{
		if( pCurExtra->eType==  (OMX_EXTRADATATYPE)aStructType)
		{ //found 
			unsigned int ComputeSize=(pCurExtra->nDataSize + sizeof(OMX_OTHER_EXTRADATATYPE) -1 + 3) &~3;
			if ( ComputeSize > pCurExtra->nSize)
			{ //Corrupted metadata
				MSG2("*** CORRUPTED metadata, sizes are not equal %d %ld\n",  ComputeSize, pCurExtra->nSize);
				ReportError(eError_CorruptedMetada, "*** CORRUPTED metadata, sizes are not equal %d %d\n",  ComputeSize, pCurExtra->nSize);
			}
			pStruct=(char *)pCurExtra->data;
			if (size!=NULL)
				*size=(size_t)pCurExtra->nDataSize;
			return(S_OK);
		}
		pExtraData +=pCurExtra->nSize;
		//round pointer
		pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtraData+ 3) & ~0x3);
	}
	pStruct=NULL;
	if (size!=NULL)
		*size=0;
	return(-1);
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
// Generic part not OMX dependant

EXPORT_C size_t RoundAddress(size_t val, size_t PowOf2)
//*************************************************************************************************************
{
	if ( (sizeof(size_t) << 3) < PowOf2)
	{ //Shift is two big!
		MSG2("RoundAddress( %u %u) PowOf2 is too big!\n",  (unsigned int)val, (unsigned int)PowOf2);
		return(0);
	}
	size_t Rounding = (1 << PowOf2)-1;
	size_t Mask     = ~Rounding;
	return((val + Rounding) & Mask);
}
