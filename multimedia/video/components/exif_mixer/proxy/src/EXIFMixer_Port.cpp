/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#define _CNAME_ Vfm_Exif_Mixer
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_exif_mixer_proxy_src_EXIFMixer_PortTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "exif_mixer"
#include<cutils/log.h>
#endif

#include "EXIFMixer_Port.h"

#define PORT_0 0 //for thumbnail JPEG data
#define PORT_1 1 //for EXIF data
#define PORT_2 2 //for output mixer data


void EXIFMixer_Port::exif_mixer_port_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal)
{
    if (!condition)
    {
        OstTraceFiltInst2(TRACE_ERROR, "EXIFMIXER_Port: errorType : 0x%x error line %d\n", errorType,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}



EXIFMixer_Port::~EXIFMixer_Port()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Port : Destructor < Line no : %d >",__LINE__);
}

OMX_U32 EXIFMixer_Port::getBufferSize() const
{
	OstTraceFiltInst1(TRACE_DEBUG, "In EXIFMIXER_Port : getBufferSize DONE < Line no : %d >",__LINE__);
	return 0;
}


EXIFMixer_Port::EXIFMixer_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
		: VFM_Port(commonPortData, enscomp)
{
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Port : COnstructor < Line no : %d >",__LINE__);
	mParamPortDefinition.format.image.cMIMEType = NULL;
}


OMX_ERRORTYPE EXIFMixer_Port::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
		OstTraceFiltInst1(TRACE_API,"In EXIFMIXER_Port : In setFormatInPortDefinition <Line : %d>",__LINE__);
		OMX_ERRORTYPE omxError;
		VFM_Component   *vfm_component = (VFM_Component *)(&(getENSComponent()));
		omxError = checkSetFormatInPortDefinition(&portDef);
		if (omxError != OMX_ErrorNone)
			exif_mixer_port_assert((0==1), (OMX_U32)omxError, __LINE__, OMX_FALSE);
		mParamPortDefinition.format.image = portDef.format.image;

		if(portDef.nPortIndex == PORT_2)
		{
				mParamPortDefinition.nBufferSize = (mParamPortDefinition.format.image.nFrameWidth*mParamPortDefinition.format.image.nFrameHeight*3*7)/(2*10) + 65536;
				OstTraceFiltInst2(TRACE_FLOW,"In EXIFMIXER_Port : setFormatInPortDefinition nBufferSize on Port 2 : %d <Line : %d>",mParamPortDefinition.nBufferSize,__LINE__);
		}
		omxError = vfm_component->reset();
		if (omxError != OMX_ErrorNone)
			exif_mixer_port_assert((0==1), (OMX_U32)omxError, __LINE__, OMX_FALSE);

		if(portDef.nPortIndex ==1)
		{
            vfm_component->pParam->getRecyclingDelay()->set(0);
        }

		OstTraceFiltInst1(TRACE_API,"In EXIFMIXER_Port : setFormatInPortDefinition DONE <Line : %d>",__LINE__);
		return omxError;
}


OMX_ERRORTYPE EXIFMixer_Port::checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef)
{
		OstTraceFiltInst1(TRACE_API,"In EXIFMIXER_Port : In checkSetFormatInPortDefinition  line : %d",__LINE__);
		OMX_IMAGE_PORTDEFINITIONTYPE *pt_image = (OMX_IMAGE_PORTDEFINITIONTYPE *)(&(portDef->format.image));
		if (PORT_0 == portDef->nPortIndex)
		{
			RETURN_XXX_IF_WRONG(OMX_IMAGE_CodingJPEG == pt_image->eCompressionFormat, OMX_ErrorBadParameter);
		}
		if ((PORT_1 == portDef->nPortIndex) || (PORT_2 == portDef->nPortIndex))
		{
			RETURN_XXX_IF_WRONG(OMX_IMAGE_CodingEXIF == pt_image->eCompressionFormat, OMX_ErrorBadParameter);
		}
		RETURN_XXX_IF_WRONG(1 <= pt_image->nFrameHeight && pt_image->nFrameHeight <= 8176, OMX_ErrorBadParameter);
		RETURN_XXX_IF_WRONG(1 <= pt_image->nFrameWidth && pt_image->nFrameWidth <= 8176, OMX_ErrorBadParameter);

		OstTraceFiltInst1(TRACE_API,"In EXIFMIXER_Port : checkSetFormatInPortDefinition done  line : %d",__LINE__);
		return OMX_ErrorNone;
}

