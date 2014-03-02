/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _IMG_ENS_IOMX_H_
#define _IMG_ENS_IOMX_H_

#include "ImgEns_Shared.h"
#include <omxil/OMX_Core.h>

class IMGENS_API ImgEns_IOmx
//***************************************************************************************
{ ///  Virtual interface for all OMX component
public:

	ImgEns_IOmx(void);
	virtual ~ImgEns_IOmx(void);

	virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData) = 0;
	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure) = 0;
	virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure) = 0;

	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) = 0;
	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) = 0;

	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) = 0;
	virtual OMX_ERRORTYPE GetState(OMX_STATETYPE *pState) = 0;
	virtual OMX_ERRORTYPE ComponentTunnelRequest(OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) = 0;
	virtual OMX_ERRORTYPE SetCallbacks(const OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) = 0;

	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer) = 0;
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer) = 0;
	virtual OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) = 0;
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) = 0;
	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) = 0;

	virtual OMX_ERRORTYPE UseEGLImage(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) = 0;

	virtual OMX_ERRORTYPE GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) = 0;

	virtual OMX_ERRORTYPE ComponentDeInit() = 0;
	virtual OMX_ERRORTYPE ComponentRoleEnum(OMX_U8 *cRole, OMX_U32 nIndex) = 0;

	virtual const char *getName() const;
	virtual void setName(const char *cName);

	virtual OMX_VERSIONTYPE getVersion(void) const;
	virtual unsigned short getTraceEnable() const = 0;

	virtual OMX_U32 getUidTopDictionnary(void) = 0;

	virtual OMX_ERRORTYPE construct(void) = 0;
	virtual OMX_ERRORTYPE eventHandlerCB(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData=NULL) = 0;

	// New functionnality
	virtual void SetWrapper(OMX_PTR pWrapper) { m_pWrapper=pWrapper; }
	virtual OMX_PTR GetWrapper() const        { return (m_pWrapper); }
	virtual OMX_ERRORTYPE  setOMXHandle(OMX_HANDLETYPE hComp);
	virtual OMX_HANDLETYPE getOMXHandle() const { return m_OmxHandle;}

	virtual OMX_U32 getPortCount(void) const { return mPortCount; }

protected:
	OMX_HANDLETYPE   m_OmxHandle;
	OMX_PTR          m_pWrapper;
	const char     * m_Name; //Doesn't allocate memory for the moment just keep the address
	OMX_U32          mPortCount;
};

#endif      // _IMG_ENS_IOMX_H_

