/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdlib.h>
#include <stdarg.h>
#include "ImgEns_Shared.h"
#include "omxil/OMX_Types.h"

static const OMX_VERSIONTYPE g_Img_OmxVersion= {{OMX_VERSION_MAJOR, OMX_VERSION_MINOR, OMX_VERSION_REVISION, OMX_VERSION_STEP}};

/// Function used to indicate the Open Max specification pVersion.
void getImgOmxIlSpecVersion(OMX_VERSIONTYPE *pVersion)
//***************************************************************************************
{
	if (pVersion==NULL)
		return;
	*pVersion = g_Img_OmxVersion;
/*	pVersion->s.nVersionMajor = OMX_VERSION_MAJOR;
	pVersion->s.nVersionMinor = OMX_VERSION_MINOR;
	pVersion->s.nRevision     = OMX_VERSION_REVISION;
	pVersion->s.nStep         = OMX_VERSION_STEP; */
}

const OMX_VERSIONTYPE &getImgOmxIlSpecVersion()
//***************************************************************************************
{
	return(g_Img_OmxVersion);
}

#if 0
ImgEns_Component * getImgEnsComponent(OMX_HANDLETYPE /*hComp*/)
//***************************************************************************************
{
	IMGENS_ASSERT(0); //to be managed
	return(NULL);
}
#endif


int ImgEns_SetTraceCallback(void (*logfn)(const char *name, unsigned int level))
//***************************************************************************************
{
	IMGENS_ASSERT(0); //to be managed
	return(eImgEns_NoError);
}

int ImgEns_SetTraceLevel(unsigned int /*nTraceLevel*/)
//***************************************************************************************
{
	IMGENS_ASSERT(0); //to be managed
	return(eImgEns_NoError);
}

int ImgEns_GetTraceLevel(unsigned int * /*pTraceLevel*/)
//***************************************************************************************
{
	IMGENS_ASSERT(0); //to be managed
	return(eImgEns_NoError);
}

int ImgEns_Report(int error, char const *filename, unsigned int line,char const * msg,...)
//***************************************************************************************
{
	if (error || filename || line || msg)
	{
	}
	printf("\n**** ImgEns ERROR %d in %s:%d ", error, filename, line);
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
	return(0);
}

//***************************************************************************************
//***************************************************************************************
//***************************************************************************************
//***************************************************************************************
//***************************************************************************************
#include <stdarg.h>
#include <string.h>
#include "osi_toolbox_lib.h"

void ost_printf(const char *name, const char *format, ...)
//***************************************************************************************
{
	va_list arg;
	va_start(arg, format);
	if (strcmp(name, TRACE_ERROR)==0)
	{
		OTB_ASSERT(0);
	}
	printf("\nost[%s] ", name);
	vprintf(format, arg);
	va_end(arg);
}

#ifdef _MSC_VER

//#include "mmhwbuffer.h"

OMX_ERRORTYPE MMHwBuffer::GetIndexExtension(OMX_STRING /*aParameterName*/, OMX_INDEXTYPE* /*apIndexType*/) 
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::GetPortIndexExtension(enum OMX_INDEXTYPE,void *,unsigned long &)
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::Open(void *,enum OMX_INDEXTYPE,void *,class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE  MMHwBuffer::Destroy(class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::Close(class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::SetConfigExtension(OMX_HANDLETYPE aTunneledComponent, OMX_U32 aPortIndex)
{
	return(OMX_ErrorNone);
}

#endif
