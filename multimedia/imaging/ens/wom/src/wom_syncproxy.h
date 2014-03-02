/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_SYNCPROXY_H_
#define _WOM_SYNCPROXY_H_

class WOM_API Interface_ProcessorToComponent
//*************************************************************************************************************
{ // Interface for ENS component, Coming from Img_ProcessingComponent
public:
	virtual int SendOmxEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)=0;
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)                    =0;
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)                    =0;
};

class WOM_API Sync_Proxy: public Img_ProcessingComponent
//*************************************************************************************************************
{
public:
	Sync_Proxy(const char *name, ImgEns_Component &comp, Interface_ProcessorToComponent &IProcessorToComponent);
	virtual ~Sync_Proxy();

	virtual OMX_ERRORTYPE construct(void)/*  = 0 */;
	virtual OMX_ERRORTYPE destroy(void)/*  = 0 */;

	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)/*  = 0 */;

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;
	virtual OMX_ERRORTYPE fillThisBuffer (OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);
	virtual OMX_ERRORTYPE useBuffer     (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);
	virtual OMX_ERRORTYPE freeBuffer    (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);
	virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;

	virtual OMX_ERRORTYPE getMMHWBufferInfo(OMX_U32 nPortIndex, OMX_U32 nSizeBytes, OMX_U8 *pData, void **bufferAllocInfo, void **portPrivateInfo);
	virtual OMX_ERRORTYPE freeSharedBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);

	// New function
	virtual const char *GetComponentName() const {return(m_Name);}
	virtual const char *GetUserName() const {return(m_UserName);}

	void Debug(const char *, ...) const;
protected:
	const char            * m_Name;
	const char            * m_UserName; //for trace functions
protected:
	Interface_ProcessorToComponent &mI_ProcessorToComponent;
	//specific new framework
};


#endif // _WOM_SYNCPROXY_H_
