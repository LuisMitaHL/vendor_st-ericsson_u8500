/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _fakesource_H_
#define _fakesource_H_

#include "ENS_Component.h"
#include "ENS_Port.h"
#include "ENS_Nmf.h"
#include "OMX_Index.h"
#include "OMX_Core.h"
#include <cm/inc/cm.hpp>
#include <los/api/los_api.h>

#include "host/sendcommand.hpp"
#include "host/eventhandler.hpp"
#include "NmfHost_ProcessingComponent.h"
#include "fake_source_wrp.hpp"

#ifdef NO_DEPENDENCY_ON_IFM_NMF
#else
#include "ENS_Component_Fsm.h"
#include "cm/inc/cm_macros.h"
#include "IFM_HostNmfProcessingComp.h"
#endif

#ifdef NO_DEP_WITH_IFM
#else
#include "extradata.h"
#endif

#define FAKESOURCE_VERSION 3

#define FAKESOURCE_PATH_PREFIXE_EXTRADATA "extradat_path_detection:"


OMX_ERRORTYPE fakesourceFactoryMethod(ENS_Component_p * ppENSComponent);


class fakesourceport: public ENS_Port
{
public:
	fakesourceport(const EnsCommonPortData& commonPortData, ENS_Component& ensComp);      	      
	virtual ~fakesourceport();

	virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;		
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;
	OMX_PARAM_PORTDEFINITIONTYPE getParamPortDefinition(void) const {return mParamPortDefinition;}
	static const int DEFAULT_WIDTH = 640;
	static const int DEFAULT_HEIGHT = 480;
	static const OMX_COLOR_FORMATTYPE DEFAULT_COLOR_FORMAT = OMX_COLOR_FormatCbYCrY;
	static const int DEFAULT_FRAMERATE = 6;
	virtual float getPixelDepth(OMX_U32 format);
private:
	float	mBytesPerPixel;
};


class fakesource_RDB : public ENS_ResourcesDB 
{
public:
	fakesource_RDB();
};

class fakesource : public ENS_Component 
{

	virtual OMX_ERRORTYPE createResourcesDB();
public:

	fakesource();
	virtual ~fakesource();

	OMX_ERRORTYPE createPort (OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_U32 nFifoBufferSize);

	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure) const;
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const;
//	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
	virtual char* my_strstr (char * str1, const char * str2 );
    virtual RM_STATUS_E getCapabilities (OMX_INOUT OMX_U32 &pCapBitmap)
    {
      pCapBitmap |= RM_CAP_DISTUSESIA;
      return RM_E_NONE; 
    }

	OMX_CONFIG_FRAMERATETYPE	iConfig_FRAMERATETYPE;
	OMX_CONFIG_CAPTUREMODETYPE	iConfig_CAPTUREMODETYPE;
	OMX_CONFIG_BOOLEANTYPE		iConfig_CAPTURING;
	OMX_CONFIG_BOOLEANTYPE		iConfig_AUTOPAUSE;

	t_los_memory_handle memory_handle_iParam_CONTENTURITYPE_data_extended;
	OMX_PARAM_CONTENTURITYPE	*iParam_CONTENTURITYPE_data;
	t_los_memory_handle memory_handle_iParam_CONTENTURITYPE_extradata_extended;
	OMX_PARAM_CONTENTURITYPE	*iParam_CONTENTURITYPE_extradata;
};

#ifdef NO_DEPENDENCY_ON_IFM_NMF
class FakeSourceProcessingComp : public NmfHost_ProcessingComponent 
#else
class FakeSourceProcessingComp : public IFM_HostNmfProcessingComp 
#endif
{
public:

	FakeSourceProcessingComp(ENS_Component &enscomp);

	virtual OMX_ERRORTYPE instantiate();
	virtual OMX_ERRORTYPE start() ;
	virtual OMX_ERRORTYPE stop() ;
	virtual OMX_ERRORTYPE configure();
	virtual OMX_ERRORTYPE deInstantiate() ;

	virtual OMX_PTR retriveStructureFromProxy(OMX_INDEXTYPE nParamIndex, OMX_U32 count);
	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nIndex,OMX_PTR pStructure);
	Ifake_source_api_setConfiguration setyourconfigitf;
	Ifake_source_api_setParam	setparamitf;
	Ifake_source_api_synchroneApi synchroneyourapiitf;

	virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferredEvent){
		return OMX_ErrorNone;
	}

	virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);

	virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);

	virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);

protected :
	fakesourceport *mFakePort;
	fake_source_wrp * mNmfSource;    
};


#endif // _fakesource_H_
