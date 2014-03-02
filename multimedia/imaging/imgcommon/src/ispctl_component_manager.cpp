/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* brief   This file contains the methods needed to instantiate the ISPCTL DSP NMF FW component. It depends on NMF Component Manager API
*       It :
*           - builds the NMF network to communicate with ISPCTL
*           - provides the interfaces needed to communicate with ISPCTL
*           - implements the callback functions binded on ISPTCL Alert/infos/debug functions.
*/

#define OMXCOMPONENT "ISPCTL_COMP_MGR"
#include "ispctl_component_manager.h"
#include "MMIO_Camera.h"

#include "ENS_Nmf.h"
#include <string.h>
#include "osi_trace.h"
#include "qhsm.h" // rename_me scf_api.h
#include "qevent.h" // rename_me scf_events.h
//#include "hw_buffer_lib.h"

#include "error.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_ispctl_component_managerTraces.h"
#endif

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#define CLIENT_NAME_FROM_ID(id) ((id)==SIA_CLIENT_CAMERA?"CAMERA":"ISPPROC")

void CIspctlComponentManager::setOMXHandle(OMX_HANDLETYPE aOSWrapper) {
	mOSWrapper = aOSWrapper;
}
void CIspctlComponentManager::setSM(CScfStateMachine * aSM) {
	mSM = aSM;
}
void CIspctlComponentManager::setFWSplit(t_uint32 size, t_uint8 *pFirmware) {
   mIsp_fw_size = size;
   mpIsp_firmware = pFirmware;
}

void CIspctlComponentManager::setFWData(t_uint32 size, t_uint8 *pFirmware) {
   mIsp_fw_sizeData = size;
   mpIsp_firmwareData = pFirmware;
}

void CIspctlComponentManager::setFWExt(t_uint32 size, t_uint8 *pFirmware) {
   mIsp_fw_sizeExt = size;
   mpIsp_firmwareExt = pFirmware;
}

void CIspctlComponentManager::setFWExtDdr(t_uint32 size, t_uint8 *pFirmware) {
   mIsp_fw_sizeExtDdr = size;
   mpIsp_firmwareExtDdr = pFirmware;
}

Iispctl_api_cmd CIspctlComponentManager::getIspctlCmd(void) {
	return mIspctlCommand;
}

Iispctl_api_cfg CIspctlComponentManager::getIspctlCfg(void) {
	return mIspctlConfigure;
}

Iapi_set_debug CIspctlComponentManager::getIspctlDebug(void) {
	return mIspctlDebug;
}


OMX_ERRORTYPE CIspctlComponentManager::instantiateNMF()
{

	OMX_ERRORTYPE error = OMX_ErrorNone;
	MSG1(" %s Instantiate  IspCtl\n",CLIENT_NAME_FROM_ID(mSia_client_id));
	OstTraceFiltStatic0(TRACE_DEBUG, "Instantiate  IspCtl", (&mENSComponent));

    OMX_U32 domainId = mENSComponent.getNMFDomainHandle(RM_NMFD_PROCSIA);

	/* Instantiate ISPCTL Component */
    t_cm_error cm_error = CM_InstantiateComponent(
        "ispctl",
        domainId,
        NMF_SCHED_NORMAL,
        "ispctl",
    	&mNmfISPCtl);
    if ( cm_error != CM_OK){
    	DBGT_ERROR("An error occured while instantiating ispctl : %d\n", cm_error);
    	OstTraceFiltStatic1(TRACE_ERROR, "An error occured while instantiating ispctl : %d", (&mENSComponent), cm_error);
        return OMX_ErrorHardware;
    }

	MSG0("Bind  IspCtl\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Bind  IspCtl", (&mENSComponent));
    error = ENS::bindComponentFromHost( mNmfISPCtl
            ,"iConfigure"
            ,(NMF::InterfaceReference *)&mIspctlConfigure
            , ICTL_FIFO_COMS_SIZE
            );

    if ( error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Bind Host->ispctl (iConfigure) 0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Bind Host->ispctl (iConfigure) 0x%x", (&mENSComponent),error);
        return error;
    }

    error = ENS::bindComponentFromHost( mNmfISPCtl
            ,"iCommand"
            ,(NMF::InterfaceReference *)&mIspctlCommand
            ,ICTL_FIFO_COMS_SIZE
            );

    error = ENS::bindComponentFromHost( mNmfISPCtl
            ,"iSetDebug"
            ,(NMF::InterfaceReference *)&mIspctlDebug
            ,ICTL_FIFO_COMS_SIZE
            );

    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Bind Host->ispctl (iCommand) 0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Bind Host->ispctl (iCommand) 0x%x", (&mENSComponent),error);
        return error;
    }

    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]", "iAlert[2]", "iAlert[3]"};

    DBC_ASSERT(mOSWrapper !=0);
    error = ENS::bindComponentToHost(
    		mOSWrapper,
    		mNmfISPCtl,
    		iAlertItfNamesArray[mSia_client_id],
             (NMF::InterfaceDescriptor *)this,
            ICTL_FIFO_COMS_SIZE);

    if(error != OMX_ErrorNone)
    {
        /*  Fatal error : Return directly */
        DBGT_ERROR("Error : Bind ispctl->Host (iAlert) 0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Bind ispctl->Host (iAlert) 0x%x", (&mENSComponent),error);
        return error;
    }


    return OMX_ErrorNone;

}

OMX_ERRORTYPE CIspctlComponentManager::configureNMF()
{
    MSG1("%s Configure  IspCtl OK\n",CLIENT_NAME_FROM_ID(mSia_client_id));
    OstTraceFiltStatic0(TRACE_DEBUG, "Configure  IspCtl OK", (&mENSComponent));
	OMX_ERRORTYPE omxerr=OMX_ErrorNone;
    return omxerr;
}

OMX_ERRORTYPE CIspctlComponentManager::stopNMF()
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	mPanic = OMX_TRUE; /* to reject any callback that could come after this point */
	MSG1("%s Stop  IspCtl ...\n",CLIENT_NAME_FROM_ID(mSia_client_id));
	OstTraceFiltStatic0(TRACE_DEBUG, "Stop  IspCtl ...", (&mENSComponent));
    error = ENS::stopNMFComponent(mNmfISPCtl);
	if(error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Fail to stop ispctl component  (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Fail to stop ispctl component  (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    MSG0("Stop IspCtl OK \n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Stop IspCtl OK ", (&mENSComponent));

	MSG0("Unload FW \n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Unload FW ", (&mENSComponent));
    if(mSia_client_id == SIA_CLIENT_CAMERA)
    {
		t_cm_error cmError=CM_FreeMpcMemory(mXP70_esram_handle);
		if (cmError != CM_OK) {
			DBGT_ERROR("CM_FreeMpcMemory error= %d\n",cmError);
			OstTraceFiltStatic1(TRACE_ERROR, "CM_FreeMpcMemory error= %d", (&mENSComponent),cmError);
			DBC_ASSERT(0);
			return OMX_ErrorHardware;
		}
		error=MMHwBuffer::Destroy(mXP70_sdram_handle);
		if (error != OMX_ErrorNone) {
		    DBGT_ERROR("CM_FreeMpcMemory error= %08x\n",error);
		    OstTraceFiltStatic1(TRACE_ERROR, "CM_FreeMpcMemory error= %08x", (&mENSComponent),error);
		    DBC_ASSERT(0);
		    return error;
		}
		isfwloaded=0;
	}
    return error;
}

/* 
  called from NmfPanicCallback in case of NMF panic on SIA.
  Brutally destroy the whole NMF network on DSP side
  Note that in this function we FORCE all components to stop and destroy 
    whatever the error code returned by NMF functions !
*/
OMX_ERRORTYPE CIspctlComponentManager::forcedStopNMF()
{
    OMX_ERRORTYPE error = OMX_ErrorUndefined;

    error = ENS::stopNMFComponent(mNmfISPCtl);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CIspctlComponentManager::forcedStopNMF (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CIspctlComponentManager::forcedStopNMF (error =0x%x)", (&mENSComponent),error);
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CIspctlComponentManager::deinstantiateNMF()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
	MSG1("%s Deinstantiate  ISPTCL ...\n",CLIENT_NAME_FROM_ID(mSia_client_id));
	OstTraceFiltStatic0(TRACE_DEBUG, "Deinstantiate  ISPTCL ...", (&mENSComponent));
    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference *)&mIspctlDebug );
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Unbind Host->ispctl (ispctlDebug) (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Unbind Host->ispctl (ispctlDebug) (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference *)&mIspctlCommand );
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Unbind Host->ispctl (ispctlCommand) (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Unbind Host->ispctl (ispctlCommand) (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference *)&mIspctlConfigure );
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Unbind Host->ispctl (ispctlConfigure)   (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Unbind Host->ispctl (ispctlConfigure)   (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]", "iAlert[2]", "iAlert[3]"};
    error = ENS::unbindComponentToHost( mOSWrapper, mNmfISPCtl, iAlertItfNamesArray[mSia_client_id]);
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : ispctl->Host (iAlert)  (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : ispctl->Host (iAlert)  (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    /* Destroy ISPTCL component */
    error = ENS::destroyNMFComponent( mNmfISPCtl );
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Destroy ispctl (error =0x%x)\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Destroy ispctl (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    MSG0("Deinstantiate  IspCtl OK \n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Deinstantiate  IspCtl OK ", (&mENSComponent));
    return OMX_ErrorNone;
}

#define XP70_SIZE_ESRAM16 60*1024 /* 60kb */
#define XP70_SIZE_SDRAM 64*1024 /* 64kb */
OMX_ERRORTYPE CIspctlComponentManager::startNMF()
{
    mPanic = OMX_FALSE;

    OMX_ERRORTYPE error = OMX_ErrorNone;
    MSG0("%s Start IspCtl\n",CLIENT_NAME_FROM_ID(mSia_client_id));
    OstTraceFiltStatic0(TRACE_DEBUG, "Start IspCtl", (&mENSComponent));
    error = ENS::startNMFComponent(mNmfISPCtl);
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : Fail to start mNmfISPCtl component (error = %d)\n", error );
        OstTraceFiltStatic1(TRACE_ERROR, "Error : Fail to start mNmfISPCtl component (error = %d)", (&mENSComponent), error );
        return error;
    }

    // Set OST trace filtering infos to MPC, once it has started
    if (NULL != mENSComponent.getSharedTraceInfoPtr()) {
        TraceInfo_t trace_info = *(mENSComponent.getSharedTraceInfoPtr());
        //trace_info.traceEnable |= 0x8; // activate TRACE_DEBUG
        mIspctlCommand.traceInit(trace_info, mENSComponent.getId1());
    }
    else {
        DBGT_ERROR("CIspctlComponentManager::startNMF : getSharedTraceInfoPtr returns NULL value\n");
        OstTraceFiltStatic0(TRACE_ERROR, "CIspctlComponentManager::startNMF : returns NULL value", (&mENSComponent));
        return OMX_ErrorUndefined;
    }

    if(mSia_client_id == SIA_CLIENT_CAMERA)
    {
        MSG0("Load ISP FW\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Load ISP FW", (&mENSComponent));
        OMX_ERRORTYPE err=OMX_ErrorNone;
        t_cm_error cmError;
        t_cm_system_address XP70_esram;
        t_uint32 physAddr, logAddr;

        /* ESRAM section */
        if (mIsp_fw_sizeExt == 64*1024) {
            /* firmware doesn't skip first 4k, can't use correct RME domains for it. Hack it into another area */ 
            MSG0("WARNING: using an old ISP firmware that doesn't fit in its dedicated ESRAM domain!\n");
            OstTraceFiltStatic0(TRACE_WARNING, "WARNING: using an old ISP firmware that doesn't fit in its dedicated ESRAM domain!", (&mENSComponent));
            OMX_U32 domainId = mENSComponent.getNMFDomainHandle();

            cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_ESRAM16, 128*1024/sizeof(t_uint16), CM_MM_ALIGN_NONE, &mXP70_esram_handle);
            if (cmError != CM_OK) {
                DBGT_ERROR("CM_AllocMpcMemory for 128k of ESRAM failed = %d\n",error);
                OstTraceFiltStatic1(TRACE_ERROR, "CM_AllocMpcMemory for 128k of ESRAM failed = %d", (&mENSComponent),error);
                DBC_ASSERT(0);
                return OMX_ErrorInsufficientResources;
            }

            cmError=CM_GetMpcMemorySystemAddress (mXP70_esram_handle, &XP70_esram);
            if (cmError != CM_OK) {
                DBGT_ERROR("CM_GetMpcMemorySystemAddress failed = %d\n",cmError);
                OstTraceFiltStatic1(TRACE_ERROR, "CM_GetMpcMemorySystemAddress failed = %d", (&mENSComponent),cmError);
                DBC_ASSERT(0);
                return OMX_ErrorHardware;
            }

            /* align on 64 kb boundary */
            physAddr = (XP70_esram.physical & 0xFFFF0000) + 0x00010000;
            t_uint32 offset = physAddr - XP70_esram.physical;
            logAddr = XP70_esram.logical + offset;

            memcpy((void *)(logAddr), (void *)mpIsp_firmwareExt, mIsp_fw_sizeExt);

        } else {
            /* this is a 60k firmware which fits in the dedicated RME domain */
            DBC_ASSERT(mIsp_fw_sizeExt <= 60*1024);

            OMX_U32 domainId = mENSComponent.getNMFDomainHandle(RM_NMFD_HWXP70SIA);

            cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_ESRAM16, 60*1024/sizeof(t_uint16), CM_MM_ALIGN_NONE, &mXP70_esram_handle);
            if (cmError != CM_OK) {
                DBGT_ERROR("CM_AllocMpcMemory of 60k in ESRAM failed = %d\n",error);
                OstTraceFiltStatic1(TRACE_ERROR, "CM_AllocMpcMemory of 60k in ESRAM failed = %d", (&mENSComponent),error);
                DBC_ASSERT(0);
                return OMX_ErrorInsufficientResources;
            }

            cmError=CM_GetMpcMemorySystemAddress (mXP70_esram_handle, &XP70_esram);
            if (cmError != CM_OK) {
                DBGT_ERROR("CM_GetMpcMemorySystemAddress failed = %d\n",cmError);
                OstTraceFiltStatic1(TRACE_ERROR, "CM_GetMpcMemorySystemAddress failed = %d", (&mENSComponent),cmError);
                DBC_ASSERT(0);
                return OMX_ErrorHardware;
            }

            if (((XP70_esram.physical-(4*1024)) & 0x0000FFFFU) != 0) {
                // The buffer is not 64KB aligned.
                DBC_ASSERT(0);
                return OMX_ErrorInsufficientResources;
            }

            physAddr = XP70_esram.physical;
            memcpy((void *) (XP70_esram.logical), (void *) mpIsp_firmwareExt, mIsp_fw_sizeExt);
        }

        // DDR section

        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        poolAttrs.iBuffers = 1;
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
        poolAttrs.iDomainID = 0;                              /* don't care, (only requested for MPC memory type) */
        if (mIsp_fw_sizeExtDdr<=64*1024)
                poolAttrs.iSize = 2*XP70_SIZE_SDRAM;                  /* we ask twice the size to align on 64kb inside */
        else    poolAttrs.iSize = 1*1024*1024 + 256*1024;             /* we ask 256k more for 1M size alignement */
        poolAttrs.iAlignment = 256;                           /* maximum alignement. We'll have to align on 64kb ourselves after */ 
        poolAttrs.iCacheAttr = MMHwBuffer::ENormalUnCached;

        mXP70_sdram_handle = NULL;
        error = MMHwBuffer::Create(poolAttrs, NULL, mXP70_sdram_handle);
        if (error != OMX_ErrorNone) {
            DBGT_ERROR("Unable to SDRAM for XP70 firmware (OMX err %d)\n", error);
            OstTraceFiltStatic1(TRACE_ERROR, "Unable to SDRAM for XP70 firmware (OMX err %d)", (&mENSComponent), error);
            return error;
        }

        /* retrieve physical and logical addresses to pass up */
        MMHwBuffer::TBufferInfo xp70_sdram;
        error = mXP70_sdram_handle->BufferInfo(0, xp70_sdram);
        if (error != OMX_ErrorNone) {
            DBGT_ERROR("Unable to retrive physical address of SDRAM for XP70 firmware (err %d)\n", error);
            OstTraceFiltStatic1(TRACE_ERROR, "Unable to retrive physical address of SDRAM for XP70 firmware (err %d)", (&mENSComponent), error);
            return error;
        }

        /* align */
        t_uint32 physAddrDdr;
        if (mIsp_fw_sizeExtDdr<=64*1024)
                physAddrDdr= (xp70_sdram.iPhyAddr & 0xFFFF0000) + 0x00010000; /* on 64kb */
        else    physAddrDdr= (xp70_sdram.iPhyAddr & 0xFFF00000) + 0x00100000; /* on 1Mb */
        t_uint32 offsetDdr = physAddrDdr - xp70_sdram.iPhyAddr;
        t_uint32 logAddrDdr = xp70_sdram.iLogAddr + offsetDdr;

        memcpy((void *) logAddrDdr, (void *) mpIsp_firmwareExtDdr, mIsp_fw_sizeExtDdr);

        err = MMIO_Camera::loadXP70FW(
                (void*)physAddrDdr, mIsp_fw_sizeExtDdr,
                (void*)physAddr, mIsp_fw_sizeExt,
                mpIsp_firmware, mIsp_fw_size,
                mpIsp_firmwareData, mIsp_fw_sizeData);

        if (err != OMX_ErrorNone) {
            DBGT_ERROR("MMIO_Camera::loadXP70FW error= %08x\n",err);
            OstTraceFiltStatic1(TRACE_ERROR, "MMIO_Camera::loadXP70FW error= %08x", (&mENSComponent),err);
            DBC_ASSERT(0);
            return OMX_ErrorHardware;
        }
        isfwloaded=1;
    }
    return OMX_ErrorNone;
}

void CIspctlComponentManager::getFwXp70Info(t_uint32 * paddr, t_uint32* psize)
{	/* does not have to be called */
	DBC_ASSERT(0);
}

void CIspctlComponentManager::info( enum e_ispctlInfo info_id, t_uint32 value, t_uint32 timestamp)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspctlComponentManager::info", (&mENSComponent));
    if(mPanic==OMX_TRUE)return;
    switch (info_id) {
//        case ISP_CDCC_AVAILABLE:
//        case ISP_POWER_NOTIFICATION:
        case ISP_SMS_NOTIFICATION:
            MSG2("\t  CIspctlComponentManager::Callback_ispctlInfo, Event info_id 0x%x (%s) ignored because it is not supported yet.\n", info_id, CError::stringIspctlInfo(info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlInfo, Event info_id 0x%x ignored because it is not supported yet.", (&mENSComponent), info_id);
            break;
        case ISP_GLACE_STATS_READY:
        case ISP_HISTOGRAM_STATS_READY:
        case ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED:
        case ISP_AUTOFOCUS_STATS_READY:
        case ISP_FLADRIVER_LENS_STOP:
		case ISP_VALID_BMS_FRAME_NOTIFICATION:
		case ISP_SENSOR_COMMIT_NOTIFICATION:
		case ISP_ISP_COMMIT_NOTIFICATION:
           case ISP_LR_GAMMA_UPDATE_COMPLETE:
            case ISP_HR_GAMMA_UPDATE_COMPLETE:
            if (mSia_client_id == SIA_CLIENT_CAMERA) {
                t_ispctlEventClbk clbk;
                t_ispctlEventClbkCtxtHnd context;
                t_ispctlEventClbkErr err = mEventClbks.getEventClbk(mSia_client_id, info_id, &clbk, &context);
                MSG2("\t  CIspctlComponentManager::Callback_ispctlInfo, info_id 0x%x (%s)\n", info_id, CError::stringIspctlInfo(info_id));
                OstTraceFiltStatic1(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlInfo, info_id 0x%x", (&mENSComponent), info_id);
                if ((err == ISPCTL_EVENTCLBK_ERR_NONE) && (clbk != NULL)) {
                    clbk(info_id, context);
                }
                else {
                    MSG2("\t  CIspctlComponentManager::Callback_ispctlInfo, Event info_id 0x%x (%s) ignored because no callback was registered.\n", info_id, CError::stringIspctlInfo(info_id));
                    OstTraceFiltStatic1(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlInfo, Event info_id 0x%x ignored because no callback was registered.", (&mENSComponent), info_id);
                }
            }
            break;
        default:
        {
            s_scf_event event;
            event.sig = EVT_ISPCTL_INFO_SIG;
            event.type.ispctlInfo.info_id = info_id;
            event.type.ispctlInfo.value = value;
            event.type.ispctlInfo.timestamp = timestamp;
            event.type.ispctlInfo.number_of_pe = 0;
            MSG2("\t  CIspctlComponentManager::Callback_ispctlInfo,event.type.ispctlInfo.info_id 0x%x (%s)\n", event.type.ispctlInfo.info_id, CError::stringIspctlInfo(info_id));
            OstTraceFiltStatic1(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlInfo,event.type.ispctlInfo.info_id 0x%x", (&mENSComponent), event.type.ispctlInfo.info_id);
            mSM->ProcessEvent(&event);
        }
    }
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspctlComponentManager::info", (&mENSComponent));
}

void CIspctlComponentManager::infoList( enum e_ispctlInfo info_id, ts_PageElement Listdata[], t_uint16 number_of_pe, t_uint32 timestamp)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspctlComponentManager::infoList", (&mENSComponent));
    //CIspctlComponentManager *mgr = (CIspctlComponentManager *) ap_this;
    if(mPanic==OMX_TRUE)return;
    s_scf_event event;
    event.sig = EVT_ISPCTL_LIST_INFO_SIG;
    event.type.ispctlListInfo.info_id = info_id;
    event.type.ispctlListInfo.timestamp = timestamp;
    for(int i=0; i<ISPCTL_SIZE_TAB_PE ;i++)
    {
		event.type.ispctlInfo.Listvalue[i] = Listdata[i];
    }
    event.type.ispctlInfo.number_of_pe = number_of_pe;


	MSG2("\t  CIspctlComponentManager::Callback_ispctlListInfo,event.type.ispctlListInfo.info_id 0x%x (%s)\n", event.type.ispctlListInfo.info_id, CError::stringIspctlInfo(info_id));
	OstTraceFiltStatic1(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlListInfo,event.type.ispctlListInfo.info_id 0x%x", (&mENSComponent), event.type.ispctlListInfo.info_id);

    mSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspctlComponentManager::infoList", (&mENSComponent));
}

void CIspctlComponentManager::error( enum e_ispctlError error_id, t_uint32 value, t_uint32 timestamp)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspctlComponentManager::error", (&mENSComponent));
    //CIspctlComponentManager *mgr = (CIspctlComponentManager *) ap_this;
    if(mPanic==OMX_TRUE)return;
	s_scf_event event;
	event.sig = EVT_ISPCTL_ERROR_SIG;
	event.type.ispctlError.error_id = error_id;
	event.type.ispctlError.last_PE_data_polled = value; //depends on error_id,  last_PE_data_polled or number_of_pe is the value
	event.type.ispctlError.number_of_pe = value;
	event.type.ispctlError.timestamp = timestamp;

	MSG3("\t  CIspctlComponentManager::Callback_ispctlError,event.type.ispctlError.error_id 0x%x (%s), last_PE_data_polled =0x%lx\n", event.type.ispctlError.error_id, CError::stringIspctlError(error_id),value);
	OstTraceFiltStatic2(TRACE_DEBUG, "CIspctlComponentManager::Callback_ispctlError,event.type.ispctlError.error_id 0x%x, last_PE_data_polled =0x%lx", (&mENSComponent), event.type.ispctlError.error_id,value);

	mSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspctlComponentManager::error", (&mENSComponent));
}

/*
void CIspctlComponentManager::Callback_ispctlUnexpectedError( void *ap_this, t_eStateId State_id, t_eSignal data, t_uint32 timestamp)
{
   ASYNC_IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspctlComponentManager::Callback_ispctlUnexpectedError", (&mENSComponent));
   ASYNC_OUT0("\n");
}
*/

void CIspctlComponentManager::debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp)
{
	ASYNC_IN0("\n");
	OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspctlComponentManager::debug", (&mENSComponent));
	/*
	s_scf_event event;
    event.sig = EVT_ISPCTL_DEBUG_SIG;
    event.type.ispctlDebug.debug_id = debug_id;
    event.type.ispctlDebug.data1 = data1;
    event.type.ispctlDebug.data2 = data2;
    event.type.ispctlDebug.timestamp = timestamp;*/
	MSG1("Callback_ispctlDebug (debug_id =0x%x)\n",debug_id );
	OstTraceFiltStatic1(TRACE_DEBUG, "Callback_ispctlDebug (debug_id =0x%x)", (&mENSComponent),debug_id );
	MSG1("Callback_ispctlDebug (data1 =0x%x)\n",data1);
	OstTraceFiltStatic1(TRACE_DEBUG, "Callback_ispctlDebug (data1 =0x%x)", (&mENSComponent),data1);
	MSG1("Callback_ispctlDebug (data2 =0x%x)\n",data2);
	OstTraceFiltStatic1(TRACE_DEBUG, "Callback_ispctlDebug (data2 =0x%x)", (&mENSComponent),data2);
	
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspctlComponentManager::debug", (&mENSComponent));
}

