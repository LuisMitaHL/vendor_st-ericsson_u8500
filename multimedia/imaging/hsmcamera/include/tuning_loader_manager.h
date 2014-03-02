/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_LOADER_MANAGER_H_
#define _TUNING_LOADER_MANAGER_H_

#include "OMX_Types.h"
#include "qhsm.h"
#include "tuning.h"
#include "sensor.h"
#include "tuningloader_wrp.hpp"

/*
 * Tuning Loader Manager error codes
 */
typedef enum
{
    TUNING_LOADER_MGR_OK,
    TUNING_LOADER_MGR_ALREADY_CONSTRUCTED,
    TUNING_LOADER_MGR_NOT_CONSTRUCTED,
    TUNING_LOADER_MGR_BAD_ARGUMENT,
    TUNING_LOADER_MGR_NMF_ERROR
} TuningLoaderMgrError_t;

/*
 * Tuning Loader Manager class.
 */
class CTuningLoaderManager: public tuningloader_api_acknowledgeDescriptor
{
    public:
        CTuningLoaderManager(TraceObject *traceobj);
        ~CTuningLoaderManager();
        void setSM(CScfStateMachine*);
        TuningLoaderMgrError_t construct(OMX_HANDLETYPE);
        TuningLoaderMgrError_t destroy();
        TuningLoaderMgrError_t start();
        TuningLoaderMgrError_t stop();
        bool                   hasFirmwareTuningData();
        bool                   hasNvmTuningData();
        bool                   hasImageQualityTuningData();
        bool                   hasFlashTuningData();
        TuningLoaderMgrError_t loadFirmwareTuningData(const t_camera_info*, const t_fw_blocks_info*);
        TuningLoaderMgrError_t loadNvmTuningData(const t_camera_info*);
        TuningLoaderMgrError_t saveNvmTuningData(const t_camera_info*,unsigned char*, t_uint32);
        TuningLoaderMgrError_t loadImageQualityTuningData(const t_camera_info*);
        TuningLoaderMgrError_t loadFlashTuningData(const t_flash_info*);
        TuningLoaderMgrError_t getCameraConfigFileName(const char**);
        TuningLoaderMgrError_t getFlashConfigFileName(const char**);
        static const char*     errorCode2String(TuningLoaderMgrError_t);
        const char*            loaderErrorCode2String(t_sint32);

    protected:
        // Overload NMF callback
        virtual void response(t_tuningloader_error);

    private:
        OMX_HANDLETYPE            iOmxHandle;
        CScfStateMachine*         iSM;
        tuningloader_wrp*         iNmfInstance;
        Ituningloader_api_query   iNmfQueryIntf;
        Ituningloader_api_sync    iNmfSyncIntf;
    public:
	    TraceObject*              mTraceObject;
};

#endif /* _TUNING_LOADER_MANAGER_H_ */
