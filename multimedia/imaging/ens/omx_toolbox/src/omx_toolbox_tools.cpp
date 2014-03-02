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
#include <omxil/OMX_Index.h>
#include <omxil/OMX_Core.h>
#ifndef __SYMBIAN32__
#include <OMX_Symbian_IVCommonExt_Ste.h>
#else
#include <OMX_Symbian_IVCommonExt.h>
#endif

#include "omx_toolbox_shared.h"
#include "omx_toolbox_tools.h"

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


const _sImageFormatDescription* GetImageFormatDescription(int format)
//*************************************************************************************************************
{ // Return the struct associated to the given format
	const _sImageFormatDescription *ptr=OMX_ImageFormatDescription;
	size_t Nbr=SIZE_OF_ARRAY(OMX_ImageFormatDescription);
	while(Nbr > 0)
	{
		if (ptr->ColorFormat ==format)
			return(ptr);
		++ptr;
		--Nbr;
	}
	return(NULL);
}

int ComputeImageSize(const int format, const size_t stride, const size_t height, size_t &ComputeSize)
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

int FormatUserComponentName(char *&aUserName, const char *aRoot, const char *aName)
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
	OTB_ASSERT(len1 < (size_t)maxsize);
	return(S_OK);
}

const _tTableEnumToString tableOmxStateName[] = 
{
	{ ENUM_AND_STRING(OMX_StateInvalid)         },
	{ ENUM_AND_STRING(OMX_StateLoaded)          },
	{ ENUM_AND_STRING(OMX_StateIdle)            },
	{ ENUM_AND_STRING(OMX_StateExecuting)       },
	{ ENUM_AND_STRING(OMX_StatePause)           },
	{ ENUM_AND_STRING(OMX_StateWaitForResources)},
}; //[OMX_StateMax];


const char *GetStringOmxState(int aState)
//*******************************************************************************
{
	const _tTableEnumToString  *pTable= tableOmxStateName;
	unsigned int size=SIZE_OF_ARRAY(tableOmxStateName);
	for (unsigned int i=0; i< size; ++i)
	{
		if (pTable->Value == aState)
			return(pTable->Name);
		++pTable;
	}
	return "Out of range value for OMX_STATETYPE";
}

const _tTableEnumToString tableOmxEventName[] =
{
	{ ENUM_AND_STRING(OMX_EventCmdComplete)               },
	{ ENUM_AND_STRING(OMX_EventError)                     },
	{ ENUM_AND_STRING(OMX_EventMark)                      },
	{ ENUM_AND_STRING(OMX_EventPortSettingsChanged)       },
	{ ENUM_AND_STRING(OMX_EventBufferFlag)                },
	{ ENUM_AND_STRING(OMX_EventResourcesAcquired)         },
	{ ENUM_AND_STRING(OMX_EventComponentResumed)          },
	{ ENUM_AND_STRING(OMX_EventDynamicResourcesAvailable) },
	{ ENUM_AND_STRING(OMX_EventPortFormatDetected)        },
//	{ ENUM_AND_STRING(OMX_EventCmdReceived)               },
//	{ ENUM_AND_STRING(OMX_EventResourcesReceived)         },
}; //[OMX_EventMax];

const char *GetStringOmxEvent(int aEvent)
//*******************************************************************************
{
	const _tTableEnumToString  *pTable= tableOmxEventName;
	unsigned int size=SIZE_OF_ARRAY(tableOmxEventName);
	for (unsigned int i=0; i< size; ++i)
	{
		if (pTable->Value == aEvent)
			return(pTable->Name);
		++pTable;
	}
	return "Out of range value for OMX_EVENTTYPE";
}

const _tTableEnumToString tableOmxCommandName[] =
{
	{ ENUM_AND_STRING(OMX_CommandStateSet)    },
	{ ENUM_AND_STRING(OMX_CommandFlush)       },
	{ ENUM_AND_STRING(OMX_CommandPortDisable) },
	{ ENUM_AND_STRING(OMX_CommandPortEnable)  },
	{ ENUM_AND_STRING(OMX_CommandMarkBuffer)  },
};

const char *GetStringOmxCommand(int aCmd)
//*******************************************************************************
{
	unsigned int size=SIZE_OF_ARRAY(tableOmxCommandName);
	const _tTableEnumToString  *pTable= tableOmxCommandName;
	for (unsigned int i=0; i< size; ++i)
	{
		if (pTable->Value == aCmd)
			return(pTable->Name);
		++pTable;
	}
	return "Out of range value for OMX_COMMANDTYPE";
}

static const char * const s_ErrorName[] =
{
	//  "OMX_ErrorNone",
	"OMX_ErrorInsufficientResources",
	"OMX_ErrorUndefined",
	"OMX_ErrorInvalidComponentName",
	"OMX_ErrorComponentNotFound",
	"OMX_ErrorInvalidComponent",
	"OMX_ErrorBadParameter",
	"OMX_ErrorNotImplemented",
	"OMX_ErrorUnderflow",
	"OMX_ErrorOverflow",
	"OMX_ErrorHardware",
	"OMX_ErrorInvalidState",
	"OMX_ErrorStreamCorrupt",
	"OMX_ErrorPortsNotCompatible",
	"OMX_ErrorResourcesLost",
	"OMX_ErrorNoMore",
	"OMX_ErrorVersionMismatch",
	"OMX_ErrorNotReady",
	"OMX_ErrorTimeout",
	"OMX_ErrorSameState",
	"OMX_ErrorResourcesPreempted",
	"OMX_ErrorPortUnresponsiveDuringAllocation",
	"OMX_ErrorPortUnresponsiveDuringDeallocation",
	"OMX_ErrorPortUnresponsiveDuringStop",
	"OMX_ErrorIncorrectStateTransition",
	"OMX_ErrorIncorrectStateOperation",
	"OMX_ErrorUnsupportedSetting",
	"OMX_ErrorUnsupportedIndex",
	"OMX_ErrorBadPortIndex",
	"OMX_ErrorPortUnpopulated",
	"OMX_ErrorComponentSuspended",
	"OMX_ErrorDynamicResourcesUnavailable",
	"OMX_ErrorMbErrorsInFrame",
	"OMX_ErrorFormatNotDetected",
	"OMX_ErrorContentPipeOpenFailed",
	"OMX_ErrorContentPipeCreationFailed",
	"OMX_ErrorSeperateTablesUsed",
	"OMX_ErrorTunnelingUnsupported"
};

const char *GetStringOmxError(OMX_ERRORTYPE aError)
//*******************************************************************************
{
	OTB_ASSERT( ((unsigned int)OMX_ErrorInsufficientResources + (SIZE_OF_ARRAY(s_ErrorName)) )== (unsigned int)(OMX_ErrorTunnelingUnsupported+1));

	if ( ((unsigned int)aError >= (unsigned int)OMX_ErrorInsufficientResources) &&
		 ((unsigned int)aError <= (unsigned int)OMX_ErrorTunnelingUnsupported))
		return(s_ErrorName[aError - OMX_ErrorInsufficientResources]);

	if (aError == OMX_ErrorNone)
		return "OMX_ErrorNone";
	else if (aError > OMX_ErrorTunnelingUnsupported)
		return "Unknow OMX_ERRORTYPE (too big)";
	else if (aError < OMX_ErrorInsufficientResources)
		return "Unknow OMX_ERRORTYPE (too small)";
	else
		return "Unknow OMX_ERRORTYPE (strange case)";
}

/**
Return the ExtraData structur associated to given index
*/
int GetExtraData(const OMX_BUFFERHEADERTYPE* pOmxBufHdr, const int aStructType, char *&pStruct, size_t *size)
//*************************************************************************************************************
{	/* get extradata start pointer */
	OMX_U8* pEnd                       = pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;
	OMX_U8* pExtraData                 = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtraData+ 3) & ~0x3);
	while ( ((OMX_U8*)pCurExtra < pEnd) &&  (pCurExtra->nSize>0) && (pCurExtra->eType!=OMX_ExtraDataNone) )
	{
		if( pCurExtra->eType==  (OMX_EXTRADATATYPE)aStructType)
		{ //found
			unsigned int ComputeSize=(pCurExtra->nDataSize + sizeof(OMX_OTHER_EXTRADATATYPE) -1 + 3) &~3;
			if ( ComputeSize > pCurExtra->nSize)
			{ //Corrupted metadata
				// MSG2("*** CORRUPTED metadata, sizes are not equal %d %ld\n",  ComputeSize, pCurExtra->nSize);
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


