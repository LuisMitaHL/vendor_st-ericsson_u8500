/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPCTL_COMPONENT_MANAGER_H_
#define _ISPCTL_COMPONENT_MANAGER_H_

#include "OMX_Core.h" /*needed for error types*/

#include "client.h"
#include "deferredeventmgr.h"
#include "ispctl_eventclbk.h"
#include "host/ispctl/api/cmd.hpp"
#include "host/ispctl/api/cfg.hpp"
#include "host/ispctl/api/alert.hpp"

#include "host/api/set_debug.hpp"

#include "ENS_Component.h"
#include "mmhwbuffer.h"
#include "error.h"
class CScfStateMachine;

#define ICTL_FIFO_COMS_SIZE  16

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CIspctlComponentManager);
#endif
class CIspctlComponentManager:public ispctl_api_alertDescriptor
{

public :
    CIspctlComponentManager(t_sia_client_id clientId, ENS_Component &enscomp):mENSComponent(enscomp)
    {
        mSia_client_id =clientId;
        mPanic=OMX_FALSE;
        isfwloaded = 0;
        mIsp_fw_size = 0;
        mIsp_fw_sizeData = 0;
        mIsp_fw_sizeExt = 0;
        mIsp_fw_sizeExtDdr = 0;
        mNmfISPCtl = (t_cm_instance_handle)(NULL);
        mOSWrapper = (OMX_HANDLETYPE)(NULL);
        mSM = (CScfStateMachine*)(NULL);
        mXP70_esram_handle = (t_cm_memory_handle)(NULL);
        mXP70_sdram_handle = (t_cm_memory_handle)(NULL);
        mpIsp_firmware = (t_uint8*)(NULL);
        mpIsp_firmwareData = (t_uint8*)(NULL);
        mpIsp_firmwareExt = (t_uint8*)(NULL);
        mpIsp_firmwareExtDdr = (t_uint8*)(NULL);
    };
	OMX_ERRORTYPE instantiateNMF();
	OMX_ERRORTYPE configureNMF();
	OMX_ERRORTYPE startNMF() ;
	OMX_ERRORTYPE stopNMF() ;
	void setPanic(OMX_BOOL panic){mPanic=panic;};
	OMX_ERRORTYPE forcedStopNMF() ;
	OMX_ERRORTYPE deinstantiateNMF() ;
	void setFWSplit(t_uint32 size, t_uint8 *pFirmware);
	void setFWData(t_uint32 size, t_uint8 *pFirmware);
	void setFWExt(t_uint32 size, t_uint8 *pFirmware);
	void setFWExtDdr(t_uint32 size, t_uint8 *pFirmware);

	Iispctl_api_cmd getIspctlCmd(void);
	Iispctl_api_cfg getIspctlCfg(void);
	Iapi_set_debug getIspctlDebug(void);

    void setOMXHandle(OMX_HANDLETYPE aOSWrapper);
    void setSM(CScfStateMachine * sm);

    CIspctlEventClbks mEventClbks;

    void getFwXp70Info(t_uint32 * paddr, t_uint32* psize);


    virtual void info(enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp) ;
    virtual void infoList(enum e_ispctlInfo info, ts_PageElement listvalue[ISPCTL_SIZE_TAB_PE], t_uint16 nb_of_pe, t_uint32 timestamp) ;
    virtual void error(enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
    virtual void debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);

    t_sia_client_id getClientId(void) {return mSia_client_id;};
    void setClientId(t_sia_client_id client_id) {mSia_client_id = client_id;};

private :
    t_sia_client_id mSia_client_id;

    OMX_U32 mIsp_fw_size;
    t_uint8 *mpIsp_firmware;

    OMX_U32 mIsp_fw_sizeData;
    t_uint8 *mpIsp_firmwareData;

    OMX_U32 mIsp_fw_sizeExt;
    OMX_U32 mIsp_fw_sizeExtDdr;

    t_uint8 *mpIsp_firmwareExt;
    t_uint8 *mpIsp_firmwareExtDdr;

    t_cm_memory_handle mXP70_esram_handle;
    MMHwBuffer *mXP70_sdram_handle;

    OMX_HANDLETYPE   mOSWrapper;
    ENS_Component&   mENSComponent;
    CScfStateMachine * mSM;

    t_cm_instance_handle mNmfISPCtl;

    Iispctl_api_cfg mIspctlConfigure;
    Iispctl_api_cmd mIspctlCommand;
    Iapi_set_debug mIspctlDebug;
    t_uint16 isfwloaded;
    OMX_BOOL mPanic;
};



#endif /**/
