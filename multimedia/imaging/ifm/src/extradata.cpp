/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "OMX_Core.h"
#include "IFM_Index.h"
#include "extradata.h"
#include "exif.h"
#include "ENS_DBC.h"
#include <string.h>
#include "ImgConfig.h"
#include "mmhwbuffer_ext.h"

OMX_U32 Extradata::GetExtradataSize(t_Component eComponent, OMX_U32 nPort)
{
	OMX_U32 nExtradataSize = 0;

	/* camera */
	if (Component_Camera == eComponent)
	{
		/* VPB0 */
		if (0 == nPort)
		{
			/* debug PE list not supported anymore */
			nExtradataSize += EXTRADATA_VIDEOPACKSIZE;
		}
		/* VPB1 */
		else if (1 == nPort)
		{
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		/* VPB2 */
		else if (2 == nPort)
		{
			nExtradataSize += EXTRADATA_VIDEOPACKSIZE;
		}
		else
		{
			DBC_ASSERT(0);
		}
	}
	/* ARM iv proc */
	else if (Component_ARMivProc == eComponent)
	{
		/* no extradata */
		/* patch untill buffer sharing is implemented : still pack on all ports */
		//nExtradataSize += 0;
		nExtradataSize += EXTRADATA_STILLPACKSIZE;
	}
	/* splitter */
	else if (Component_Splitter == eComponent)
	{
		nExtradataSize += 0;
	}
	/* Analyzers (OT, AMBR, FD) */
	else if ((Component_OT_Analyzer == eComponent) || (Component_AMBR_Analyzer == eComponent) || (Component_FD_Analyzer == eComponent))
	{
		nExtradataSize += EXTRADATA_VIDEOPACKSIZE;
	}
	/* ISP Proc */
	else if (Component_ISPProc == eComponent)
	{
		/* input port */
		if (0 == nPort)
		{
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		else
		{
			/* no extradata */
			/* patch until buffer sharing is implemented : still pack on all ports */
			//nExtradataSize += 0;
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
	}
	/* RED */
	else if (Component_RED == eComponent)
	{
		/* imput port */
		if (0 == nPort)
		{
			nExtradataSize += EXTRADATA_REDEYEPACKSIZE;
		}
		/* output port */
		else if (1 == nPort)
		{
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		else
		{
			DBC_ASSERT(0);
		}
	}
	/* REC */
	else if (Component_REC == eComponent)
	{
		/* imput port 0 */
		if (0 == nPort)
		{
			nExtradataSize += EXTRADATA_REDEYEPACKSIZE;
		}
		/* input port 1 */
		else if (1 == nPort)
		{
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		/* output port */
		else if (2 == nPort)
		{
			/* no extradata */
			/* patch untill buffer sharing is implemented : still pack */
			//nExtradataSize += 0;
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		else
		{
			DBC_ASSERT(0);
		}
	}
	/* Norcos (Still YUV correction) */
	else if (Component_Norcos == eComponent)
	{
		/* imput port */
		if (0 == nPort)
		{
			nExtradataSize += EXTRADATA_STILLPACKSIZE;
		}
		/* output port */
		else if (1 == nPort)
		{
			nExtradataSize += 0;
		}
		else
		{
			DBC_ASSERT(0);
		}
	}
	else
	{
		/* unknown component */
		DBC_ASSERT(0);
	}

	return (nExtradataSize);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_BMS_CAPTURE_CONTEXT* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)IFM_BMS_CAPTURE_CONTEXT_TYPE, pOmxBufHdr, (void*)pExtradataData, sizeof(IFM_BMS_CAPTURE_CONTEXT), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCameraUserSettings, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_DAMPERS_DATA* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)IFM_DAMPERS_DATA_TYPE, pOmxBufHdr, (void*)pExtradataData, sizeof(IFM_DAMPERS_DATA), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCaptureParameters, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFrameDescription, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_ROITYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataROI, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_ROITYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE* pExtradataData, OMX_U32 nExtradataSize, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_Symbian_CameraExtraDataAFStatus, pOmxBufHdr, (void*)pExtradataData, nExtradataSize, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE* pFeedbackExtradata, OMX_SYMBIAN_FEEDBACKTYPE nFeedbackType, OMX_U32 nFeedbackExtradataSize, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback, nFeedbackType, pOmxBufHdr, pFeedbackExtradata, nFeedbackExtradataSize, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_EXIF_SETTINGS* pExifSettings, OMX_VERSIONTYPE nVersion)
{
	return GetExifField(pOmxBufHdr, pExifSettings, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_STE_PRODUCTIONTESTTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataField((OMX_EXTRADATATYPE)OMX_ExtraDataCameraProductionFlashTest, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_STE_PRODUCTIONTESTTYPE), nVersion);
}

//Added for extra data capture paramenter
OMX_ERRORTYPE Extradata::GetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version)
{
       return GetExtradataField((OMX_EXTRADATATYPE)OMX_STE_ExtraData_CaptureParameters, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE), version);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_BMS_CAPTURE_CONTEXT* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)IFM_BMS_CAPTURE_CONTEXT_TYPE, pOmxBufHdr, (void*)pExtradataData, sizeof(IFM_BMS_CAPTURE_CONTEXT), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCameraUserSettings, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_DAMPERS_DATA* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)IFM_DAMPERS_DATA_TYPE, pOmxBufHdr, (void*)pExtradataData, sizeof(IFM_DAMPERS_DATA), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCaptureParameters, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFrameDescription, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_ROITYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataROI, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_ROITYPE), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE* pExtradataData, OMX_U32 nExtradataSize, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_Symbian_CameraExtraDataAFStatus, pOmxBufHdr, (void*)pExtradataData, nExtradataSize, nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE* pFeedbackExtradata, OMX_SYMBIAN_FEEDBACKTYPE nFeedbackType, OMX_U32 nFeedbackExtradataSize, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback, nFeedbackType, pOmxBufHdr, pFeedbackExtradata, nFeedbackExtradataSize, nVersion);
}

/* Caution ; this function is defined again in stab; please update it when updating this one */
OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, IFM_EXIF_SETTINGS* pExifSettings, OMX_VERSIONTYPE nVersion)
{
	return SetExifField(pOmxBufHdr, pExifSettings, nVersion);
}

OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_STE_PRODUCTIONTESTTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return SetExtradataField((OMX_EXTRADATATYPE)OMX_ExtraDataCameraProductionFlashTest, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_STE_PRODUCTIONTESTTYPE), nVersion);
}

//Added for extra data capture paramenter - 3 channel gamma value
OMX_ERRORTYPE Extradata::SetExtradataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version)
{
       return SetExtradataField((OMX_EXTRADATATYPE)OMX_STE_ExtraData_CaptureParameters, pOmxBufHdr, (void*)pExtradataData, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE), version);
}


/* Caution ; this function is defined again in stab; please update it when updating this one */
OMX_ERRORTYPE Extradata::ClearAllExtradata(OMX_BUFFERHEADERTYPE* pOmxBufHdr)
{
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no extradata */
		return OMX_ErrorNone;
	}

	/* get extradata start pointer */
	OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pCur+ 3) & ~0x3);

	/* check that there is enough space for an extradata header */
	if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra + sizeof(OMX_OTHER_EXTRADATATYPE))
	{
		return OMX_ErrorInsufficientResources;
	}

	/* add extradata terminator */
	FillExtradataTerminator(pCurExtra);

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::ClearAllExtradata(OMX_U8* pMem)
{
	/* get extradata start pointer */
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pMem+ 3) & ~0x3);

	/* add extradata terminator */
	FillExtradataTerminator(pCurExtra);

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::CopyExtradata_Buffer2Mem(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_U8* pMem, OMX_U32 nExtradataDataSize)
{
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no extradata */
		return OMX_ErrorNone;
	}

	/* get extradata start pointer */
	OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pCur+ 3) & ~0x3);

	/* check that there can be enough data in buffer */
	if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra + nExtradataDataSize)
	{
		return OMX_ErrorInsufficientResources;
	}

	pMem = (OMX_U8*)(((OMX_U32)pMem + 3) & ~0x3);
	memcpy (pMem, (void*)pCurExtra, nExtradataDataSize);

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::CopyExtradata_Mem2Buffer(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_U8* pMem, OMX_U32 nExtradataDataSize)
{
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no extradata */
		return OMX_ErrorNone;
	}

	/* get extradata start pointer */
	OMX_U8* pCur = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pCur+ 3) & ~0x3);

	/* check that there is enough space in buffer for extradatas */
	if ((OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen < (OMX_U8*)pCurExtra + nExtradataDataSize)
	{
		return OMX_ErrorInsufficientResources;
	}

	pMem = (OMX_U8*)(((OMX_U32)pMem + 3) & ~0x3);
	memcpy ((void*)pCurExtra, pMem, nExtradataDataSize);

	/* set flag */
	pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, IFM_BMS_CAPTURE_CONTEXT* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)IFM_BMS_CAPTURE_CONTEXT_TYPE, pMem, (void*)pExtradataData, sizeof(IFM_BMS_CAPTURE_CONTEXT), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, IFM_DAMPERS_DATA* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)IFM_DAMPERS_DATA_TYPE, pMem, (void*)pExtradataData, sizeof(IFM_DAMPERS_DATA), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCaptureParameters, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFrameDescription, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE), nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE* pFeedbackExtradata, OMX_SYMBIAN_FEEDBACKTYPE nFeedbackType, OMX_U32 nFeedbackExtradataSize, OMX_VERSIONTYPE nVersion)
{
	return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback, nFeedbackType, pMem, (void*)pFeedbackExtradata, nFeedbackExtradataSize, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, IFM_EXIF_SETTINGS* pExifSettings, OMX_VERSIONTYPE nVersion)
{
	return GetExifFieldFromMem( pMem, pExifSettings, nVersion);
}

//Added for extra data capture paramenter - 3 channel gamma value
OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem(OMX_U8* pMem, OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version)
{
       return GetExtradataFieldFromMem((OMX_EXTRADATATYPE)OMX_STE_ExtraData_CaptureParameters, pMem, (void*)pExtradataData, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE), version);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, IFM_BMS_CAPTURE_CONTEXT* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)IFM_BMS_CAPTURE_CONTEXT_TYPE, pMem, (void*)pExtradataData, sizeof(IFM_BMS_CAPTURE_CONTEXT), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCameraUserSettings, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, IFM_DAMPERS_DATA* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)IFM_DAMPERS_DATA_TYPE, pMem, (void*)pExtradataData, sizeof(IFM_DAMPERS_DATA), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCaptureParameters, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFrameDescription, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization, pMem, (void*)pExtradataData, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, IFM_EXIF_SETTINGS* pExifSettings, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExifFieldInMem(pMem, pExifSettings, nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_STE_PRODUCTIONTESTTYPE* pExtradataData, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_ExtraDataCameraProductionFlashTest, pMem, (void*)pExtradataData, sizeof(OMX_STE_PRODUCTIONTESTTYPE), nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE* pFeedbackExtradata, OMX_SYMBIAN_FEEDBACKTYPE nFeedbackType, OMX_U32 nFeedbackExtradataSize, OMX_VERSIONTYPE nVersion, OMX_U32 nPortIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback, nFeedbackType, pMem, (void*)pFeedbackExtradata, nFeedbackExtradataSize, nVersion, nPortIndex);
}

//Added for extra data capture paramenter - 3 channel gamma value
OMX_ERRORTYPE Extradata::SetExtradataFieldInMem(OMX_U8* pMem, OMX_STE_CAPTUREPARAMETERSTYPE* pExtradataData, OMX_VERSIONTYPE version, OMX_U32 portIndex)
{
	return SetExtradataFieldInMem((OMX_EXTRADATATYPE)OMX_STE_ExtraData_CaptureParameters, pMem, (void*)pExtradataData, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE), version, portIndex);
}

/*
 *
 * Below functions are the private methods that implement the actual extradata management
 *
 */

OMX_ERRORTYPE Extradata::GetExtradataField( OMX_EXTRADATATYPE     nExtradataType,
                                            OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                            void*                 pExtradataData,
                                            OMX_U32               nExtradataDataSize,
                                            OMX_VERSIONTYPE       nVersion)
{
	return GetExtradataField(nExtradataType, 0, pOmxBufHdr, pExtradataData, nExtradataDataSize, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataField( OMX_EXTRADATATYPE     nExtradataType,
                                            OMX_U32               nExtradataSubType,
                                            OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                            void*                 pExtradataData,
                                            OMX_U32               nExtradataDataSize,
                                            OMX_VERSIONTYPE       nVersion)
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataBoundary = NULL;

	/* preamble */
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no need to add extradata */
		memset (pExtradataData, 0, nExtradataDataSize);
		return OMX_ErrorNone;
	}

	/* get extradata boundaries */
	err = GetExtradataBoundaries( pOmxBufHdr, &pExtradataStart, &pExtradataBoundary);
	if( err != OMX_ErrorNone || pExtradataStart == NULL || pExtradataBoundary == NULL )
	{
		/* failed to get extradata boundaries from buffer header */
		return OMX_ErrorInsufficientResources;
	}

	/* get extradata field */
	err = DoGetExtradataField( pExtradataStart, pExtradataBoundary, nExtradataType, nExtradataSubType, pExtradataData, nExtradataDataSize, nVersion);
	if( err != OMX_ErrorNone )
	{
		/* failed to get extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::GetExifField( OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                       IFM_EXIF_SETTINGS*    pExifSettings,
                                       OMX_VERSIONTYPE       nVersion)
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataBoundary = NULL;

	/* preamble */
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no need to add extradata */
		memset (pExifSettings, 0, sizeof(*pExifSettings));
		return OMX_ErrorNone;
	}

	/* get extradata boundaries */
	err = GetExtradataBoundaries( pOmxBufHdr, &pExtradataStart, &pExtradataBoundary);
	if( err != OMX_ErrorNone || pExtradataStart == NULL || pExtradataBoundary == NULL )
	{
		/* failed to get extradata boundaries from buffer header */
		return OMX_ErrorInsufficientResources;
	}

	/* get exif field */
	err = DoGetExifField( pExtradataStart, pExtradataBoundary, pExifSettings, nVersion);
	if( err != OMX_ErrorNone )
	{
		/* failed to get exif extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem( OMX_EXTRADATATYPE nExtradataType,
                                                   OMX_U8*           pMem,
                                                   void*             pExtradataData,
                                                   OMX_U32           nExtradataDataSize,
                                                   OMX_VERSIONTYPE   nVersion)
{
    return GetExtradataFieldFromMem( nExtradataType, 0, pMem, pExtradataData, nExtradataDataSize, nVersion);
}

OMX_ERRORTYPE Extradata::GetExtradataFieldFromMem( OMX_EXTRADATATYPE nExtradataType,
                                                   OMX_U32           nExtradataSubType,
                                                   OMX_U8*           pMem,
                                                   void*             pExtradataData,
                                                   OMX_U32           nExtradataDataSize,
                                                   OMX_VERSIONTYPE   nVersion)
{
	/* get extradata start pointer */
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pMem + 3) & ~0x3);
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* get extradata field */
	err = DoGetExtradataField( pExtradataStart, NULL, nExtradataType, nExtradataSubType, pExtradataData, nExtradataDataSize, nVersion);
	if( err != OMX_ErrorNone )
	{
		/* failed to get extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::GetExifFieldFromMem( OMX_U8*            pMem,
                                              IFM_EXIF_SETTINGS* pExifSettings,
                                              OMX_VERSIONTYPE    nVersion)
{
	/* get extradata start pointer */
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pMem + 3) & ~0x3);
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* get exif field */
	err = DoGetExifField( pExtradataStart, NULL, pExifSettings, nVersion);
	if( err != OMX_ErrorNone )
	{
		/* failed to get exif extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::DoGetExtradataField( OMX_OTHER_EXTRADATATYPE* pExtradataStart,
                                              OMX_OTHER_EXTRADATATYPE* pExtradataBoundary,
                                              OMX_EXTRADATATYPE        nExtradataType,
                                              OMX_U32                  nExtradataSubType,
                                              void*                    pExtradataData,
                                              OMX_U32                  nExtradataDataSize,
                                              OMX_VERSIONTYPE          nVersion)
{
	OMX_OTHER_EXTRADATATYPE* pExtradataField = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataTerminator = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* search extradata field */
	err = DoSearchExtradataField( pExtradataStart, pExtradataBoundary, nExtradataType, nExtradataSubType, &pExtradataField, &pExtradataTerminator);
	if( err != OMX_ErrorNone || ( pExtradataField == NULL && pExtradataTerminator == NULL) )
	{
		/* failed to search extradata */
		return OMX_ErrorInsufficientResources;
	}

	/* Check if extradata was found */
	if ( pExtradataField != NULL )
	{
		/* extradata was found: copy it */
		if ( pExtradataField->nDataSize > nExtradataDataSize )
		{
			return OMX_ErrorInsufficientResources;
		}
		memcpy ( pExtradataData, pExtradataField->data, nExtradataDataSize);
	}
	else
	{
		/* extradata was not found: zeroefy extradata data */
		memset (pExtradataData, 0, nExtradataDataSize);

	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::SetExtradataField( OMX_EXTRADATATYPE     nExtradataType,
                                            OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                            void*                 pExtradataData,
                                            OMX_U32               nExtradataDataSize,
                                            OMX_VERSIONTYPE       nVersion)
{
	return SetExtradataField(nExtradataType, 0, pOmxBufHdr, pExtradataData, nExtradataDataSize, nVersion);
}

/* Caution ; this function is defined again in stab; please update it when updating this one */
OMX_ERRORTYPE Extradata::SetExtradataField( OMX_EXTRADATATYPE     nExtradataType,
                                            OMX_U32               nExtradataSubType,
                                            OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                            void*                 pExtradataData,
                                            OMX_U32               nExtradataDataSize,
                                            OMX_VERSIONTYPE       nVersion)
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataBoundary = NULL;

	/* preamble */
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no need to add extradata */
		return OMX_ErrorNone;
	}

	/* get extradata boundaries */
	err = GetExtradataBoundaries( pOmxBufHdr, &pExtradataStart, &pExtradataBoundary);
	if( err != OMX_ErrorNone || pExtradataStart == NULL || pExtradataBoundary == NULL )
	{
		/* failed to get extradata boundaries from buffer header */
		return OMX_ErrorInsufficientResources;
	}

	/* set extradata field */
	err = DoSetExtradataField( pExtradataStart, pExtradataBoundary, nExtradataType, nExtradataSubType, pExtradataData, nExtradataDataSize, nVersion, pOmxBufHdr->nOutputPortIndex);
	if( err != OMX_ErrorNone )
	{
		/* failed to set extradata */
		return err;
	}

	/* done */
	pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::SetExifField( OMX_BUFFERHEADERTYPE* pOmxBufHdr,
                                       IFM_EXIF_SETTINGS*    pExifSettings,
                                       OMX_VERSIONTYPE       nVersion)
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataBoundary = NULL;

	/* preamble */
	if ((0 == pOmxBufHdr) || (0 == pOmxBufHdr->pBuffer) || (0 == pOmxBufHdr->nAllocLen))
	{
		return OMX_ErrorInsufficientResources;
	}
	else if (0 == pOmxBufHdr->nFilledLen)
	{
		/* no image in buffer => no need to add extradata */
		return OMX_ErrorNone;
	}

	/* get extradata boundaries */
	err = GetExtradataBoundaries( pOmxBufHdr, &pExtradataStart, &pExtradataBoundary);
	if( err != OMX_ErrorNone || pExtradataStart == NULL || pExtradataBoundary == NULL )
	{
		/* failed to get extradata boundaries from buffer header */
		return OMX_ErrorInsufficientResources;
	}

	/* set extradata field */
	err = DoSetExifField( pExtradataStart,pExtradataBoundary, pExifSettings, nVersion, pOmxBufHdr->nOutputPortIndex);
	if( err != OMX_ErrorNone )
	{
		/* failed to set exif extradata */
		return err;
	}

	/* done */
	pOmxBufHdr->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem( OMX_EXTRADATATYPE nExtradataType,
                                                 OMX_U8*           pMem,
                                                 void*             pExtradataData,
                                                 OMX_U32           nExtradataDataSize,
                                                 OMX_VERSIONTYPE   nVersion,
                                                 OMX_U32           nPortIndex)
{
	return SetExtradataFieldInMem( nExtradataType, 0, pMem, pExtradataData, nExtradataDataSize, nVersion, nPortIndex);
}

OMX_ERRORTYPE Extradata::SetExtradataFieldInMem( OMX_EXTRADATATYPE nExtradataType,
                                                 OMX_U32           nExtradataSubType,
                                                 OMX_U8*           pMem,
                                                 void*             pExtradataData,
                                                 OMX_U32           nExtradataDataSize,
                                                 OMX_VERSIONTYPE   nVersion,
                                                 OMX_U32           nPortIndex)
{
	/* get extradata start pointer */
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pMem + 3) & ~0x3);
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* set extradata field */
	err = DoSetExtradataField( pExtradataStart, NULL, nExtradataType, nExtradataSubType, pExtradataData, nExtradataDataSize, nVersion, nPortIndex);
	if( err != OMX_ErrorNone )
	{
		/* failed to set extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}


OMX_ERRORTYPE Extradata::SetExifFieldInMem( OMX_U8*            pMem,
                                            IFM_EXIF_SETTINGS* pExifSettings,
                                            OMX_VERSIONTYPE    nVersion,
                                            OMX_U32            nPortIndex)
{
	/* get extradata start pointer */
	OMX_OTHER_EXTRADATATYPE* pExtradataStart = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pMem + 3) & ~0x3);
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* set exif field */
	err = DoSetExifField( pExtradataStart, NULL, pExifSettings, nVersion, nPortIndex);
	if( err != OMX_ErrorNone )
	{
		/* failed to set exif extradata */
		return err;
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::DoSetExtradataField( OMX_OTHER_EXTRADATATYPE* pExtradataStart,
                                              OMX_OTHER_EXTRADATATYPE* pExtradataBoundary,
                                              OMX_EXTRADATATYPE        nExtradataType,
                                              OMX_U32                  nExtradataSubType,
                                              void*                    pExtradataData,
                                              OMX_U32                  nExtradataDataSize,
                                              OMX_VERSIONTYPE          nVersion,
                                              OMX_U32                  nPortIndex)
{
	OMX_OTHER_EXTRADATATYPE* pExtradataField = NULL;
	OMX_OTHER_EXTRADATATYPE* pExtradataTerminator = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* search extradata field */
	err = DoSearchExtradataField( pExtradataStart, pExtradataBoundary, nExtradataType, nExtradataSubType, &pExtradataField, &pExtradataTerminator);
	if( err != OMX_ErrorNone || ( pExtradataField == NULL && pExtradataTerminator == NULL) )
	{
		/* failed to search extradata */
		return OMX_ErrorInsufficientResources;
	}

	/* Check if extradata was found */
	if ( pExtradataField != NULL )
	{
		/* extradata was found: overwrite it */
		/* check if there is enough space in this extradata field */
		if ( nExtradataDataSize > pExtradataField->nDataSize )
		{
			return OMX_ErrorInsufficientResources;
		}
		memcpy ((void*)pExtradataField->data, pExtradataData, nExtradataDataSize);
	}
	else
	{
		/* extradata does not exist yet, append new extradata */
		pExtradataField = pExtradataTerminator;

		/* check if there is enough space for new extradata */
		if(    pExtradataBoundary != NULL
		    && EnoughSpaceForExtradata(pExtradataBoundary, pExtradataField, nExtradataDataSize) != OMX_ErrorNone )
		{
			return OMX_ErrorInsufficientResources;
		}

		/* fill header */
		FillExtradataHeader( pExtradataField, nVersion, nPortIndex, (OMX_EXTRADATATYPE)nExtradataType, nExtradataDataSize);

		/* copy extradata */
        if (NULL == pExtradataData) {
            return (OMX_ErrorBadParameter);
        }
		memcpy ((void*)pExtradataField->data, pExtradataData, nExtradataDataSize);

		/* add extradata terminator */
		pExtradataField = NextExtradataHeader( pExtradataField );
		FillExtradataTerminator( pExtradataField );
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::DoSetExifField( OMX_OTHER_EXTRADATATYPE* pExtradataStart,
                                         OMX_OTHER_EXTRADATATYPE* pExtradataBoundary,
                                         IFM_EXIF_SETTINGS*       pExifSettings,
                                         OMX_VERSIONTYPE          nVersion,
                                         OMX_U32                  nPortIndex)
{
    OMX_OTHER_EXTRADATATYPE* pExtradataField = NULL;
    OMX_OTHER_EXTRADATATYPE* pExtradataTerminator = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* search extradata field */
	err = SearchExtradataField( pExtradataStart, pExtradataBoundary, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker, &pExtradataField, &pExtradataTerminator);
	if( err != OMX_ErrorNone || ( pExtradataField == NULL && pExtradataTerminator == NULL) )
	{
		/* failed to serach extradata */
		return OMX_ErrorInsufficientResources;
	}

	/* Check if extradata was found */
	if ( pExtradataField != NULL )
	{
		/* exif extradata was found: overwrite it */
		if ( sizeof(EXTRADATA_EXIFMARKERCONTAINERTYPE) > pExtradataField->nDataSize )
		{
			return OMX_ErrorInsufficientResources;
		}

		/* format exif data */
		err = FormatExifData( (EXTRADATA_EXIFMARKERCONTAINERTYPE*)pExtradataField->data, pExifSettings, nVersion);
		if( err != OMX_ErrorNone )
		{
			/* failed to format exif data */
			return err;
		}
	}
	else
	{
		/* exif extradata does not exist yet, append new extradata */
		pExtradataField = pExtradataTerminator;

		/* check if there is enough space for new extradata */
		if(     pExtradataBoundary != NULL
		    && EnoughSpaceForExtradata(pExtradataBoundary, pExtradataField, sizeof(EXTRADATA_EXIFMARKERCONTAINERTYPE)) != OMX_ErrorNone)
		{
			return OMX_ErrorInsufficientResources;
		}

		/* fill header */
		FillExtradataHeader( pExtradataField, nVersion, nPortIndex, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker, sizeof(EXTRADATA_EXIFMARKERCONTAINERTYPE));

		/* format exif data */
		err = FormatExifData( (EXTRADATA_EXIFMARKERCONTAINERTYPE*)pExtradataField->data, pExifSettings, nVersion);
		if( err != OMX_ErrorNone )
		{
			return err;
		}

		/* add extradata terminator */
		pExtradataField = NextExtradataHeader( pExtradataField );
		FillExtradataTerminator( pExtradataField );
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::DoGetExifField( OMX_OTHER_EXTRADATATYPE* pExtradataStart,
                                         OMX_OTHER_EXTRADATATYPE* pExtradataBoundary,
                                         IFM_EXIF_SETTINGS*       pExifSettings,
                                         OMX_VERSIONTYPE          nVersion)
{
    OMX_OTHER_EXTRADATATYPE* pExtradataField = NULL;
    OMX_OTHER_EXTRADATATYPE* pExtradataTerminator = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* search extradata field */
	err = SearchExtradataField( pExtradataStart, pExtradataBoundary, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataEXIFAppMarker, &pExtradataField, &pExtradataTerminator);
	if( err != OMX_ErrorNone || ( pExtradataField == NULL && pExtradataTerminator == NULL) )
	{
		/* failed to serach extradata */
		return OMX_ErrorInsufficientResources;
	}

	/* Check if extradata was found */
	if ( pExtradataField != NULL )
	{
		/* unformat exif data */
		err = UnFormatExifData( (EXTRADATA_EXIFMARKERCONTAINERTYPE*)pExtradataField->data, pExifSettings, nVersion);
		if( err != OMX_ErrorNone )
		{
			/* failed to unformat exif data */
			return err;
		}
	}
	else
	{
		/* exif extradata does not exist */
		memset(pExifSettings, 0, sizeof(*pExifSettings));
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::GetExtradataBoundaries( OMX_BUFFERHEADERTYPE*     pOmxBufHdr,
                                                 OMX_OTHER_EXTRADATATYPE** ppExtradataStart,
                                                 OMX_OTHER_EXTRADATATYPE** ppExtradataBoundary)
{
	OMX_U8* pEndOfBufferFilled = (OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_U8* pEndOfBufferActual = (OMX_U8*)pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;
	*ppExtradataStart = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pEndOfBufferFilled + 3) & ~0x3);
	*ppExtradataBoundary = (OMX_OTHER_EXTRADATATYPE*)pEndOfBufferActual;
	return OMX_ErrorNone;
}

void Extradata::FillExtradataHeader( OMX_OTHER_EXTRADATATYPE* pExtradataField,
                                     OMX_VERSIONTYPE          nVersion,
                                     OMX_U32                  nPortIndex,
                                     OMX_EXTRADATATYPE        eExtratadaType,
                                     OMX_U32                  nDataSize)
{
	pExtradataField->nSize = (sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + nDataSize + 3) & ~0x3;
	pExtradataField->nVersion = nVersion;
	pExtradataField->nPortIndex = nPortIndex;
	pExtradataField->eType = eExtratadaType;
	pExtradataField->nDataSize = nDataSize;
}

void Extradata::FillExtradataTerminator( OMX_OTHER_EXTRADATATYPE* pExtradataField )
{
	pExtradataField->nSize = (sizeof(OMX_OTHER_EXTRADATATYPE)+ 3) & ~0x3;
	pExtradataField->nVersion.nVersion = 0;
	pExtradataField->nPortIndex = 0;
	pExtradataField->eType = OMX_ExtraDataNone;
	pExtradataField->nDataSize = 0;
}

OMX_ERRORTYPE Extradata::EnoughSpaceForExtradata( OMX_OTHER_EXTRADATATYPE* pExtradataBoundary,
                                                  OMX_OTHER_EXTRADATATYPE* pExtradataField,
                                                  OMX_U32                  nExtradataDataSize)
{
	if ( (OMX_U8*)pExtradataBoundary < (OMX_U8*)pExtradataField + ((sizeof(OMX_OTHER_EXTRADATATYPE)
	    - sizeof(OMX_U8) + nExtradataDataSize + 3) & ~0x3) + sizeof(OMX_OTHER_EXTRADATATYPE))
	{
		return  OMX_ErrorInsufficientResources;
	}
	return OMX_ErrorNone;
}

OMX_OTHER_EXTRADATATYPE* Extradata::NextExtradataHeader( OMX_OTHER_EXTRADATATYPE* pExtradataField)
{
	return (OMX_OTHER_EXTRADATATYPE*)((OMX_U8*)pExtradataField + pExtradataField->nSize);
}


OMX_ERRORTYPE Extradata::SearchExtradataField( OMX_OTHER_EXTRADATATYPE*  pExtradataStart,
                                               OMX_OTHER_EXTRADATATYPE*  pExtradataBoundary,
                                               OMX_EXTRADATATYPE         nExtradataType,
                                               OMX_OTHER_EXTRADATATYPE** ppExtradataField,
                                               OMX_OTHER_EXTRADATATYPE** ppExtradataTerminator)
{
	return DoSearchExtradataField( pExtradataStart, NULL, nExtradataType, 0, ppExtradataField, ppExtradataTerminator);
}

OMX_ERRORTYPE Extradata::DoSearchExtradataField( OMX_OTHER_EXTRADATATYPE*  pExtradataStart,
                                                 OMX_OTHER_EXTRADATATYPE*  pExtradataBoundary,
                                                 OMX_EXTRADATATYPE         nExtradataType,
                                                 OMX_U32                   nExtradataSubType,
                                                 OMX_OTHER_EXTRADATATYPE** ppExtradataField,
                                                 OMX_OTHER_EXTRADATATYPE** ppExtradataTerminator)
{
	bool bFound = false;
	bool bEndOfExtradata = false;
	OMX_OTHER_EXTRADATATYPE* pCurrentExtradata = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtradataStart+ 3) & ~0x3);

	/* search extradata:                                                                          */
	/* if extradata is found: ppExtradataField is set to extradata field header address           */
	/* if not found and terminator is reached: ppExtradataTerminator is set to terminator address */
	while( !bFound && !bEndOfExtradata )
	{
		/* if searching a feedback, then check feedback type */
		if (   (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback == nExtradataType
		    && (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback == pCurrentExtradata->eType
		    && (OMX_SYMBIAN_FEEDBACKTYPE)nExtradataSubType == ((OMX_SYMBIAN_EXTRADATA_FEEDBACKTYPE*)(pCurrentExtradata->data))->nFeedbackType)
		{
			*ppExtradataField = pCurrentExtradata;
			*ppExtradataTerminator = NULL;
			bFound = true;
		}
		/* for any other extradata, only check extradata type */
		else if ((OMX_EXTRADATATYPE)nExtradataType == pCurrentExtradata->eType)
		{
			*ppExtradataField = pCurrentExtradata;
			*ppExtradataTerminator = NULL;
			bFound = true;
		}
		/* else check if we reached the end of extradata marker */
		else if ( OMX_ExtraDataNone == pCurrentExtradata->eType )
		{
			*ppExtradataField = NULL;
			*ppExtradataTerminator = pCurrentExtradata;
			bEndOfExtradata = true;
		}
		/* try to reach next extradata header */
		else if( pExtradataBoundary == NULL )
		{
			/* function caller does not provide the pointer to end of extradata area */
			/* we have no way to check that end of extradata area is reached */
			pCurrentExtradata = NextExtradataHeader(pCurrentExtradata);
		}
		else
		{
			/* function caller provides the pointer to end of extradata area */
			/* we can whether end of extradata area is reached */
			if ((OMX_U8*)pCurrentExtradata + pCurrentExtradata->nSize + sizeof(OMX_OTHER_EXTRADATATYPE) < (OMX_U8*)pExtradataBoundary )
			{
				pCurrentExtradata = NextExtradataHeader(pCurrentExtradata);
			}
			else
			{
				/* reached end of extradata but end of extradata marker not found */
				*ppExtradataField = NULL;
				*ppExtradataTerminator = NULL;
				return OMX_ErrorInsufficientResources;
			}
		}
	}

	/* done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::FormatExifData( EXTRADATA_EXIFMARKERCONTAINERTYPE* pExifMarkerContainer,
                                         IFM_EXIF_SETTINGS*                 pExifSettings,
                                         OMX_VERSIONTYPE                    aVersion)
{
	CExifMetadata* pExifMetadata = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* Allocate a EXIF metadata object */
	pExifMetadata = new CExifMetadata;
	if(pExifMetadata==NULL) {
		/* Allocation failed */
		return OMX_ErrorInsufficientResources;
	}

	/* Write the EXIF-formatted metadata to the EXIF section of extradata */
	err = pExifMetadata->serialize(pExifSettings, pExifMarkerContainer->sExifMarker.nAppMarkerData, EXIF_APPMARKER_SIZE);
	if(err!=OMX_ErrorNone) {
		/* Something went wrong when serializing */
		delete pExifMetadata;
		return err;
	}

#if defined(DISPLAY_EXIF_METADATA)
	// For debug purpose, print the EXIF metadata
	CExifReader::print(pExifMarkerContainer->sExifMarker.nAppMarkerData);
#endif

	/* Fill the APP Marker header */
	pExifMarkerContainer->sExifMarker.nSize = pExifMetadata->size();
	pExifMarkerContainer->sExifMarker.nExtraDataVersion = aVersion;

	/* serialization done */
	delete pExifMetadata;

	/* Done */
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Extradata::UnFormatExifData( EXTRADATA_EXIFMARKERCONTAINERTYPE* pExifMarkerContainer,
                                           IFM_EXIF_SETTINGS*                 pExifSettings,
                                           OMX_VERSIONTYPE                    aVersion)
{
	CExifMetadata* pExifMetadata = NULL;
	OMX_ERRORTYPE err = OMX_ErrorNone;

	/* Allocate a EXIF metadata object */
	pExifMetadata = new CExifMetadata;
	if(pExifMetadata==NULL) {
		/* Allocation failed */
		return OMX_ErrorInsufficientResources;
	}

	/* Deserialize the EXIF-formatted metadata */
	err = pExifMetadata->deserialize(pExifSettings, pExifMarkerContainer->sExifMarker.nAppMarkerData, EXIF_APPMARKER_SIZE);
	if(err!=OMX_ErrorNone) {
		/* Something went wrong when deserializing */
		delete pExifMetadata;
		return err;
	}

	/* Done */
	delete pExifMetadata;
	return OMX_ErrorNone;
}

/****************************************************************************/
/**
 * \brief: return the index to be used for an OMX extension from its string name
 * \in cParameterName string name to be search for
 * \out pIndexType Index found for the string
 * \return error code
 **/
/****************************************************************************/
OMX_ERRORTYPE CExtension::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) {
	// IN0("\n");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_IMAGE_MAXJPEGSIZE_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigImageMaxJpegSize;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERAFUNCTESTSELECT_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraFuncTestSelect;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERASELFTESTSELECT_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraSelfTestSelect;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERATESTING_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraTesting;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SCENEMODE_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSceneMode;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_RGBHISTOGRAM_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRGBHistogram;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HISTOGRAMCONTROL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHistogramControl;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_XENONLIFECOUNTER_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigXenonLifeCounter;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ROISELECTION_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigROISelection;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_REDCOMPLEXITY_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigREDComplexity;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_REDEYEREMOVAL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRedEyeRemoval;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ORIENTATIONSCENE_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigOrientationScene;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_PARAM_COMMON_REVERTASPECTRATIO_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexParamRevertAspectRatio;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_NDFILTERCONTROL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigNDFilterControl;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXPOSURELOCK_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureLock;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_WHITEBALANCELOCK_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigWhiteBalanceLock;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSLOCK_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusLock;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ALLLOCK_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAllLock;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CENTERFIELDOFVIEW_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCenterFieldOfView;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTDIGITALZOOM_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtDigitalZoom;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTOPTICALZOOM_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtOpticalZoom;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSRANGE_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRange;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTFOCUSSTATUS_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtFocusStatus;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLICKERREMOVAL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlickerRemoval;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLASHCONTROL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashControl;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_AFASSISTANTLIGHT_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAFAssistantLight;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HINTPOWERVSQUALITY_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHintPowerVsQuality;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_HINTDEPTHOFFIELD_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigHintDepthOfField;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSREGION_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFocusRegion;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ROI_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigROI;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_CAMERASENSORINFO_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraSensorInfo;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLASHGUNINFO_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashGunInfo;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_MOTIONLEVEL_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigMotionLevel;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_SHARPNESS_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSharpness;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTURING_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCommonExtCapturing;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTUREMODE_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExtCaptureMode;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_XENONFLASHSTATUS_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigXenonFlashStatus;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_PRECAPTUREEXPOSURETIME_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigPreCaptureExposureTime;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXPOSUREINITIATED_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigExposureInitiated;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_BRACKETING_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigBracketing;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_RAWPRESET_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRAWPreset;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_APPLYUSERSETTINGS_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigApplyUserSettings;
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_VIDEOLIGHT_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigVideoLight;
	}
	else if (0 == strcmp(cParameterName, OMX_SYMBIAN_INDEX_CONFIG_SHAREDCHUNKMETADATA_NAME))
	{
#ifdef __SYMBIAN32__
		*pIndexType = (OMX_INDEXTYPE)OMX_MMHwBufferIndexConfigSharedChunkMetadata;
#else
		*pIndexType = (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata;
#endif
	}
	else if (0 == strcmp(cParameterName,OMX_SYMBIAN_INDEX_CONFIG_COMMON_ANALYZERFEEDBACK_NAME))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAnalyzerFeedback;
	}
	else
	{
		err = OMX_ErrorNotImplemented;
		//err = MMHwBuffer::GetIndexExtension(cParameterName, pIndexType);
	}

	// OUTR("%d\n"("",(err));
	return err;
}
