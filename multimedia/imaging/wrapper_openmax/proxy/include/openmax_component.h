/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef __OPENMAX_COMPONENT_H__
#define __OPENMAX_COMPONENT_H__

#include <ENS_Component.h>
#include <ENS_Port.h>
#include <ENS_Nmf.h>
#include <ENS_List.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_type.h>

// Define when to send a callback
#define WOM_BUFFERFLAG_CALLBACK_MASK   (OMX_BUFFERFLAG_EOS )

class ENS_Component;
class EnsCommonPortData;
class OpenMax_Component;

typedef struct
{
	unsigned int NbBufferInProcess; // Number of buffers currently processed
	unsigned int NbSkipBuffer;      // Number of buffers skipped
	bool         bEnable;           // if skipping is enable
	int Enable(bool enable=true) { bEnable=enable; NbSkipBuffer=0; NbBufferInProcess=0; return(S_OK); }
	int Init() { bEnable=false; NbSkipBuffer=0; NbBufferInProcess=0; return(S_OK); }
} _tBufferSkipping;



class PortOther: public ENS_Port
//*************************************************************************************************************
{
public:
	WRAPPER_OPENMAX_API PortOther(const EnsCommonPortData& commonPortData, ENS_Component& ensComp);
	virtual WRAPPER_OPENMAX_API ~PortOther();
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;
	virtual WRAPPER_OPENMAX_API const _tImageInfo& GetImageInfo() const {return(m_ImageInfo);}

	WRAPPER_OPENMAX_API OMX_PARAM_PORTDEFINITIONTYPE&  getParamPortDefinition() { return(mParamPortDefinition); }

	WRAPPER_OPENMAX_API OMX_VIDEO_CODINGTYPE  Get_CompressionFormat() const;
	WRAPPER_OPENMAX_API OMX_COLOR_FORMATTYPE  Get_ColorFormat() const;
	WRAPPER_OPENMAX_API OMX_NATIVE_WINDOWTYPE Get_NativeWindow() const;
	WRAPPER_OPENMAX_API OMX_U32               Get_Bitrate() const ;
	WRAPPER_OPENMAX_API OMX_BOOL              Get_FlagErrorConcealment() const;
	WRAPPER_OPENMAX_API OMX_U32               Get_Framerate() const;

	WRAPPER_OPENMAX_API bool GetReserveExtradataForInput() const { return m_bReserveExtradataForInput; }
	WRAPPER_OPENMAX_API void SetReserveExtradataForInput(bool reserve) { m_bReserveExtradataForInput=reserve; }
protected:
	MMHwBuffer *buffersPool;
	OpenMax_Component& m_Omx_Component;

	_tImageInfo m_ImageInfo;
	bool        m_bReserveExtradataForInput; // false by default

	//size_t    mImageSize; //Size of image in bytes
#if 0 
	//Not enable for the moment
	_tBufferSkipping   m_Skipper;
public:
	WRAPPER_OPENMAX_API _tBufferSkipping &Get_BufferSkipping(void) { return(m_Skipper); }
	WRAPPER_OPENMAX_API bool                  Get_SkipBufferEnable() const { return(m_Skipper.bEnable); }
	WRAPPER_OPENMAX_API void                  Set_SkipBufferEnable(bool enable) { m_Skipper.bEnable=enable; }
#endif
};


class PortVideo: public PortOther
//*************************************************************************************************************
{
public:
	WRAPPER_OPENMAX_API PortVideo(const EnsCommonPortData& commonPortData, ENS_Component& ensComp);
	virtual WRAPPER_OPENMAX_API ~PortVideo();
	//virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;
	//virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE doBufferAllocation(OMX_U32 nBufferIndex,OMX_U32 nSizeBytes, OMX_U8 **ppData, void **ppBufferMetaData);
	//virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE doBufferDeAllocation(OMX_U32 nBufferIndex,void *pBufferMetaData);
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;

protected:
};

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

class OpenMax_RDB : public ENS_ResourcesDB
//*************************************************************************************************************
{
public:
	OpenMax_RDB();
};

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

/**
@class Extend ENS_Component class
*/
class openmax_processor;
class OpenMax_Proxy;
class OpenMax_Component : public ENS_Component
//*************************************************************************************************************
{
public:
	WRAPPER_OPENMAX_API OpenMax_Component(const char *name=NULL);
	virtual WRAPPER_OPENMAX_API ~OpenMax_Component();
	virtual int Construct()=0; //Make the real constuction
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE setConfig    (OMX_INDEXTYPE nIndex, OMX_PTR pStructure);
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE getConfig    (OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStructure) const;
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE setParameter (OMX_INDEXTYPE nParamIndex,  OMX_PTR pParameterStructure);
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE getParameter (OMX_INDEXTYPE nParamIndex,  OMX_PTR pParameterStructure) const;
	virtual WRAPPER_OPENMAX_API void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

	int GetLastError() const { return m_LastError;}

	enum
	{
		eError_NoError=0,
		eError_CannotCreateEnsComponent,
		eError_CannotCreateInputPort,
		eError_CannotCreateOutputPort,
		eError_NullPointer,
		eError_Bad_getConfigParameter,
		eError_Bad_setConfigParameter,
		eError_UnsupportedDomain,

		eError_LastEnum,
	};

	virtual WRAPPER_OPENMAX_API int ReportError(int error, const char *format, ...);

	virtual WRAPPER_OPENMAX_API int NMFConfigure(); /// call just after the constuction of internal structur, and nmf binding

	const char * GetComponentName() const { return(m_ComponentName);}

	virtual WRAPPER_OPENMAX_API int PortFormatChanged(ENS_Port &);/// call when a set param has been done on a port
        WRAPPER_OPENMAX_API OpenMax_Proxy & GetProxy();

protected:
	virtual WRAPPER_OPENMAX_API OMX_ERRORTYPE createResourcesDB();
	virtual WRAPPER_OPENMAX_API RM_STATUS_E getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData);
	virtual WRAPPER_OPENMAX_API RM_STATUS_E getCapabilities (OMX_INOUT OMX_U32 &pCapBitmap);
	virtual WRAPPER_OPENMAX_API int createPort (const EnsCommonPortData &aPortData);
	int          m_LastError;
	const char * m_ComponentName;
	bool         m_bUseBufferSharing;
};

#endif // __OPENMAX_COMPONENT_H__
