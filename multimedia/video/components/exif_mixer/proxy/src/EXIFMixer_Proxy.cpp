/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_EXIFMixer
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_exif_mixer_proxy_src_EXIFMixer_ProxyTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "exif_mixer"
#include<cutils/log.h>
#endif


#include "EXIFMixer_factory.h"
#include "EXIFMixer_Proxy.h"
#include "EXIFMixer_Port.h"

void EXIFMixer_Proxy::exif_mixer_proxy_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal)
{
    if (!condition)
    {
        OstTraceFiltInst2(TRACE_ERROR, "EXIFMIXER_Proxy: errorType : 0x%x error line %d\n", errorType,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}

EXIFMixer_Proxy::EXIFMixer_Proxy():VFM_Component(&mParam),mParam(this),processingComponent(*this)
{
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Proxy : In Constructor < Line no : %d >",__LINE__);
    size_jpeg = 0;
	mSendParamToARMNMF.set();
}

OMX_ERRORTYPE EXIFMixer_Proxy::notifyportsetting(OMX_U32 size)
{
    OstTraceFiltInst1(TRACE_API,"In EXIFMixer_Proxy::In notifyportsetting line no : %d \n",__LINE__);
    OMX_ERRORTYPE err = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE outPortDef;

    outPortDef.nPortIndex = 1;
    outPortDef.nVersion.s.nVersionMajor    = 1;
    outPortDef.nVersion.s.nVersionMinor    = 1;
    outPortDef.nVersion.s.nRevision	= 1;
    outPortDef.nVersion.s.nStep		= 0;
    outPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    if(size_jpeg > size)
    {
        err = ((EXIFMixer_Port *)getPort(2))->getParameter(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
        if(err != OMX_ErrorNone) 
        {
            OstTraceFiltInst0(TRACE_ERROR,"EXIFMixer_Proxy::notifyportsetting :: Error while getting settings for output port \n");
            exif_mixer_proxy_assert((0==1),err,__LINE__,OMX_TRUE);
            return err;
        }
        outPortDef.nBufferSize = size_jpeg;
        outPortDef.nPortIndex = 2;
        OstTraceFiltInst2(TRACE_FLOW,"EXIFMixer_Proxy Updating settings for output port with nBuffer Size = 0x%x at line no : %d \n",size_jpeg, __LINE__);
		err = ((EXIFMixer_Port *)getPort(2))->updateSettings(OMX_IndexParamPortDefinition,(OMX_PTR)&outPortDef);
		if(err != OMX_ErrorNone) 
		{
			OstTraceFiltInst0(TRACE_ERROR,"EXIFMixer_Proxy : Error while Updating settings for output port \n");
            exif_mixer_proxy_assert((0==1),err,__LINE__,OMX_TRUE);			
            return err;
		}
    }
    OstTraceFiltInst1(TRACE_FLOW,"EXIFMixer_Proxy PortSettingChanged Checked at line no : %d \n", __LINE__);
    OstTraceFiltInst1(TRACE_API,"In EXIFMixer_Proxy::notifyportsetting DONE line no : %d \n",__LINE__);
    return err;   
}

OMX_ERRORTYPE EXIFMixer_Proxy::construct()
{
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Proxy : In Construct < Line no : %d >",__LINE__);
	RETURN_OMX_ERROR_IF_ERROR(VFM_Component::construct(3));
    setProcessingComponent(&processingComponent);
    processingComponent.setPortCount(3);
    processingComponent.isSetConfigDone = OMX_FALSE;

	processingComponent.gpsLocationValues.nLatitudeDegrees = 0;
	processingComponent.gpsLocationValues.nLatitudeMinutes = 0;
	processingComponent.gpsLocationValues.nLatitudeSeconds = 0;
	processingComponent.gpsLocationValues.nLongitudeDegrees = 0;
	processingComponent.gpsLocationValues.nLongitudeMinutes = 0;
	processingComponent.gpsLocationValues.nLongitudeSeconds = 0;
	processingComponent.gpsLocationValues.nAltitudeMeters = 0;
	processingComponent.gpsLocationValues.bLatitudeRefNorth = OMX_FALSE;
	processingComponent.gpsLocationValues.bLongitudeRefEast = OMX_FALSE;
	processingComponent.gpsLocationValues.bAltitudeRefAboveSea = OMX_FALSE;
	processingComponent.gpsLocationValues.bLocationKnown = OMX_FALSE;
    processingComponent.gpsLocationValues.nHours = 0;
	processingComponent.gpsLocationValues.nMinutes = 0;
	processingComponent.gpsLocationValues.nSeconds = 0;
	/* ER 429860 */
	processingComponent.gpsLocationValues.nYear = 0;
	processingComponent.gpsLocationValues.nMonth = 0;
	processingComponent.gpsLocationValues.nDay = 0;
	/* ER 429860 */
    processingComponent.gpsLocationValues.processingValue = (OMX_KEY_GPS_PROCESSING_METHODTYPE)0;
    for(int i=0;i<32;i++)
        processingComponent.gpsLocationValues.nPrMethodDataValue[i] = 0;

	ENS_Port * portInput1= newPort(EnsCommonPortData(IPB+0,OMX_DirInput,1,0,OMX_PortDomainImage,OMX_BufferSupplyInput), *this);
	ENS_Port * portInput2= newPort(EnsCommonPortData(IPB+1,OMX_DirInput,1,0,OMX_PortDomainImage,OMX_BufferSupplyInput), *this);

	if ((portInput1 == NULL) || (portInput2 == NULL))
	{
        OstTraceInt0(TRACE_ERROR, "EXIFMIXER_Proxy: Error while creating Input Ports \n");
		exif_mixer_proxy_assert((0==1), (OMX_U32)OMX_ErrorInsufficientResources, __LINE__, OMX_FALSE);
		return OMX_ErrorInsufficientResources;
	}

	ENS_Port * portOutput= newPort(EnsCommonPortData(IPB+2,OMX_DirOutput,1,0,OMX_PortDomainImage,OMX_BufferSupplyOutput), *this);
	if (portOutput == NULL)
	{
        OstTraceInt0(TRACE_ERROR, "EXIFMIXER_Proxy: Error while creating Output Ports \n");
		delete portInput1;
		delete portInput2;
		exif_mixer_proxy_assert((0==1), (OMX_U32)OMX_ErrorInsufficientResources, __LINE__, OMX_FALSE);
		return OMX_ErrorInsufficientResources;
	}
    addPort(portInput1);
    addPort(portInput2);
	addPort(portOutput);
    setFrameSize(IPB+0, 640, 480);
    setFrameSize(IPB+1, 640, 480);
    setFrameSize(IPB+2, 640, 480);

    OstTraceInt1(TRACE_API, "EXIFMIXER: Proxy construct complete line no : %d \n",__LINE__);

	return OMX_ErrorNone;
}


EXIFMixer_Proxy::~EXIFMixer_Proxy()
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Proxy : In Destructor < Line no : %d >",__LINE__);
    setProcessingComponent(0);

}


VFM_Port *EXIFMixer_Proxy::newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp)
{
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Proxy : Creating New Port of size : %d",sizeof(EXIFMixer_Port));
	return new EXIFMixer_Port(commonPortData, enscomp);
}


// Mandatory to register the component
EXPORT_C OMX_ERRORTYPE EXIFMixerFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Proxy : In EXIFMixerFactoryMethod < Line no : %d >",__LINE__);
    OMX_ERRORTYPE error;
    EXIFMixer_Proxy *comp = new EXIFMixer_Proxy;
    RETURN_XXX_IF_WRONG(comp!=NULL, OMX_ErrorInsufficientResources);
	error = comp->construct();
	if (error != OMX_ErrorNone)
	{
        OstTraceInt2(TRACE_ERROR, "EXIFMIXER_Proxy: in EXIFMixerFactoryMethod errorType : 0x%x error line %d\n", error,__LINE__);
		delete comp;
		return error;
	}
    *ppENSComponent = comp;
	OstTraceInt1(TRACE_API, "In EXIFMIXER_Proxy : EXIFMixerFactoryMethod Completes < Line no : %d >",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE EXIFMixer_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Proxy : In setConfig  < Line no : %d >",__LINE__);
	OMX_ERRORTYPE omxError;
	OMX_CONFIG_GPSLOCATIONTYPE *pt = 0x0;
    switch ((int)nParamIndex)
    {
    	case OMX_IndexConfigImageGPSLocation:
    		pt = (OMX_CONFIG_GPSLOCATIONTYPE *)pComponentParameterStructure;

    		processingComponent.isSetConfigDone = OMX_TRUE;
    		processingComponent.gpsLocationValues.nLatitudeDegrees = pt->nLatitudeDegrees;
    		processingComponent.gpsLocationValues.nLatitudeMinutes = pt->nLatitudeMinutes;
    		processingComponent.gpsLocationValues.nLatitudeSeconds = pt->nLatitudeSeconds;
    		processingComponent.gpsLocationValues.nLongitudeDegrees = pt->nLongitudeDegrees;
    		processingComponent.gpsLocationValues.nLongitudeMinutes = pt->nLongitudeMinutes;
    		processingComponent.gpsLocationValues.nLongitudeSeconds = pt->nLongitudeSeconds;
    		processingComponent.gpsLocationValues.nAltitudeMeters = pt->nAltitudeMeters;
    		processingComponent.gpsLocationValues.bLatitudeRefNorth = pt->bLatitudeRefNorth;
    		processingComponent.gpsLocationValues.bLongitudeRefEast = pt->bLongitudeRefEast;
    		processingComponent.gpsLocationValues.bAltitudeRefAboveSea = pt->bAltitudeRefAboveSea;
    		processingComponent.gpsLocationValues.bLocationKnown = pt->bLocationKnown;
            processingComponent.gpsLocationValues.nHours = pt->nHours;
            processingComponent.gpsLocationValues.nMinutes = pt->nMinutes;
            processingComponent.gpsLocationValues.nSeconds = pt->nSeconds;
            /* ER 429860 */
            processingComponent.gpsLocationValues.nYear = pt->nYear;
            processingComponent.gpsLocationValues.nMonth = pt->nMonth;
            processingComponent.gpsLocationValues.nDay = pt->nDay;
            /* ER 429860 */
            processingComponent.gpsLocationValues.processingValue = pt->processingValue;
            for(int i=0;i<32;i++)
                   processingComponent.gpsLocationValues.nPrMethodDataValue[i] = pt->nPrMethodDataValue[i];
            omxError = OMX_ErrorNone;
			break;


	    default:
	        omxError = VFM_Component::setConfig(nParamIndex, pComponentParameterStructure);
	        break;
    }

	if (OMX_ErrorNone != omxError)
		exif_mixer_proxy_assert((0==1), (OMX_U32)omxError, __LINE__, OMX_FALSE);

	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Proxy : setConfig DONE < Line no : %d >",__LINE__);
    return omxError;
}


OMX_ERRORTYPE EXIFMixer_Proxy::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentParameterStructure) const
{
	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Proxy : In getConfig line no : %d ",__LINE__);
	OMX_ERRORTYPE omxError;
	OMX_CONFIG_GPSLOCATIONTYPE *pt = 0x0;
    switch ((int)nConfigIndex)
    {
    	case OMX_IndexConfigImageGPSLocation:
    		pt = (OMX_CONFIG_GPSLOCATIONTYPE *)pComponentParameterStructure;

    		pt->nLatitudeDegrees = processingComponent.gpsLocationValues.nLatitudeDegrees;
    		pt->nLatitudeMinutes = processingComponent.gpsLocationValues.nLatitudeMinutes;
    		pt->nLatitudeSeconds = processingComponent.gpsLocationValues.nLatitudeSeconds;
    		pt->nLongitudeDegrees = processingComponent.gpsLocationValues.nLongitudeDegrees;
    		pt->nLongitudeMinutes = processingComponent.gpsLocationValues.nLongitudeMinutes;
    		pt->nLongitudeSeconds = processingComponent.gpsLocationValues.nLongitudeSeconds;
    		pt->nAltitudeMeters = processingComponent.gpsLocationValues.nAltitudeMeters;
    		pt->bLatitudeRefNorth = processingComponent.gpsLocationValues.bLatitudeRefNorth;
    		pt->bLongitudeRefEast = processingComponent.gpsLocationValues.bLongitudeRefEast;
    		pt->bAltitudeRefAboveSea = processingComponent.gpsLocationValues.bAltitudeRefAboveSea;
    		pt->bLocationKnown = processingComponent.gpsLocationValues.bLocationKnown;
            pt->nHours = processingComponent.gpsLocationValues.nHours;
            pt->nMinutes = processingComponent.gpsLocationValues.nMinutes;
            pt->nSeconds = processingComponent.gpsLocationValues.nSeconds;
            /* ER 429860 */
            pt->nYear = processingComponent.gpsLocationValues.nYear;
            pt->nMonth = processingComponent.gpsLocationValues.nMonth;
            pt->nDay = processingComponent.gpsLocationValues.nDay;
            /* ER 429860 */
            pt->processingValue = processingComponent.gpsLocationValues.processingValue;
            for(int i=0;i<32;i++)
                   pt->nPrMethodDataValue[i] = processingComponent.gpsLocationValues.nPrMethodDataValue[i];
    		omxError = OMX_ErrorNone;
			break;


	    default:
	        omxError = VFM_Component::getConfig(nConfigIndex, pComponentParameterStructure);
	        break;
    }

	OstTraceFiltInst1(TRACE_API, "In EXIFMIXER_Proxy : getConfig DONE line no : %d ",__LINE__);
    return omxError;
}









