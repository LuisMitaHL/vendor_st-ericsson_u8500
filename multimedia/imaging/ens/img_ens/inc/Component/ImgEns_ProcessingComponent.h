/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_PROCESSING_COMPONENT_H_
#define _IMG_PROCESSING_COMPONENT_H_

#include "ImgEns_Component.h"

class IMGENS_API Img_ProcessingComponent: public Img_TraceObject
{
public:
	Img_ProcessingComponent(ImgEns_Component &enscomp)
		: mENSComponent(enscomp)
	{
	}

	virtual ~Img_ProcessingComponent(void) {}

	virtual OMX_ERRORTYPE construct(void) = 0;
	virtual OMX_ERRORTYPE destroy(void)   = 0;

	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE /*nConfigIndex*/, OMX_PTR /*pComponentConfigStructure*/)
	{
		return OMX_ErrorUnsupportedIndex;
	}

	virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE /*nConfigIndex*/, OMX_PTR /*pComponentConfigStructure*/)
	{
		return OMX_ErrorUnsupportedIndex;
	}

	virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData) = 0;

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer) = 0;
	virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)  = 0;
	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo) = 0;

	virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir,OMX_BUFFERHEADERTYPE* pBuffer) = 0;
	virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo) = 0;
	virtual OMX_ERRORTYPE freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo) = 0;

	virtual OMX_ERRORTYPE getMMHWBufferInfo(OMX_U32 nPortIndex, OMX_U32 nSizeBytes, OMX_U8 *pData, void **bufferAllocInfo, void **portPrivateInfo)=0;

	virtual OMX_ERRORTYPE freeSharedBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo) =0;

protected:
	ImgEns_Component &mENSComponent;

};

#endif // _IMG_PROCESSING_COMPONENT_H_
