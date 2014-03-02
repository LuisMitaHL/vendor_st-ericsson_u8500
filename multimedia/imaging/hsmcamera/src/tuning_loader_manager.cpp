/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#undef OMXCOMPONENT
#define OMXCOMPONENT "TUNING_LOADER_MANAGER"

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x10

#include "tuning_loader_manager.h"
#include "osi_trace.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_tuning_loader_managerTraces.h"
#endif

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningLoaderManager);
#endif

/*
 * Constructor
 */
CTuningLoaderManager::CTuningLoaderManager(TraceObject *traceobj): mTraceObject(traceobj)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::CTuningLoaderManager", (mTraceObject));
    iSM = NULL;
    iNmfInstance = NULL;
    iOmxHandle = NULL;
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CTuningLoaderManager::CTuningLoaderManager", (mTraceObject));
}

/*
 * Destructor
 */
CTuningLoaderManager::~CTuningLoaderManager()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::~CTuningLoaderManager", (mTraceObject));
    if(iNmfInstance != NULL) {
        MSG0("NMF instance not destroyed => destroying\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "NMF instance not destroyed => destroying", (mTraceObject));
        if( destroy() != TUNING_LOADER_MGR_OK) {
            MSG0("TuningLoaderManager destruction failed\n");
            OstTraceFiltStatic0(TRACE_ERROR, "TuningLoaderManager destruction failed", (mTraceObject));
        }
    }
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CTuningLoaderManager::~CTuningLoaderManager", (mTraceObject));
}

/*
 * COM_SM Handle setter
 */
void CTuningLoaderManager::setSM(CScfStateMachine* aSM)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::setSM", (mTraceObject));
    iSM = aSM;
    MSG1("COM_SM Handle = %p\n", iSM);
    OstTraceFiltStatic1(TRACE_DEBUG, "COM_SM Handle = 0x%x", (mTraceObject), (t_uint32)iSM);
    OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CTuningLoaderManager::setSM", (mTraceObject));
}

/*
 * This function contructs and starts the NMF component
 */
TuningLoaderMgrError_t CTuningLoaderManager::construct(OMX_HANDLETYPE aOmxHandle)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::construct", (mTraceObject));
    t_nmf_error nmfErr = NMF_OK;
    bool nmfConstructed = false;
    bool queryBound = false;

    if(iNmfInstance != NULL) {
        MSG0("NMF instance already constructed\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "NMF instance already constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_ALREADY_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::construct (%d)", (mTraceObject), TUNING_LOADER_MGR_ALREADY_CONSTRUCTED);
        return TUNING_LOADER_MGR_ALREADY_CONSTRUCTED;
    }

    if(aOmxHandle == NULL) {
        MSG1("Bad argument aOmxHandle=%p\n",aOmxHandle);
        OstTraceFiltStatic1(TRACE_ERROR, "Bad argument aOmxHandle=0x%x", (mTraceObject), (t_uint32)aOmxHandle);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::construct (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    MSG0("NMF instantiation\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "NMF instantiation", (mTraceObject));
    iNmfInstance = tuningloader_wrpCreate();
    if(iNmfInstance == NULL) {
        MSG0("NMF instantiation failed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "NMF instantiation failed", (mTraceObject));
        goto failed;
    }

    MSG0("NMF construction\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "NMF construction", (mTraceObject));
    nmfErr = iNmfInstance->construct();
    if (nmfErr != NMF_OK) {
        MSG1("NMF construction failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF construction failed (NmfErr=%d)", (mTraceObject), nmfErr);
        goto failed;
    }
    nmfConstructed = true;

    MSG0("Binding NMF 'query' interface\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Binding NMF 'query' interface", (mTraceObject));
    nmfErr = iNmfInstance->bindFromUser( "query", 2, &iNmfQueryIntf);
    if(nmfErr != NMF_OK) {
        MSG1("NMF 'query' interface binding failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF 'query' interface binding failed (NmfErr=%d)", (mTraceObject), nmfErr);
        goto failed;
    }
    queryBound = true;

    MSG0("Getting NMF 'sync' synchronous interface\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Getting NMF 'sync' synchronous interface", (mTraceObject));
    nmfErr = iNmfInstance->getInterface("sync", &iNmfSyncIntf) ;
    if (nmfErr != NMF_OK) {
        MSG1("Could not get NMF 'cfgfile' interface (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "Could not get NMF 'cfgfile' interface (NmfErr=%d)", (mTraceObject), nmfErr);
        goto failed;
    }

    MSG0("Binding NMF 'acknowledge' interface\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Binding NMF 'acknowledge' interface", (mTraceObject));
    nmfErr = EnsWrapper_bindToUser( aOmxHandle, iNmfInstance, "acknowledge", (tuningloader_api_acknowledgeDescriptor*)this, 2);
    if (nmfErr != NMF_OK) {
        MSG1("NMF 'acknowledge' interface binding failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF 'acknowledge' interface binding failed (NmfErr=%d)", (mTraceObject), nmfErr);
        goto failed;
    }

    // Contruction done
    iOmxHandle = aOmxHandle;
    MSG0("Construction done\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Construction done", (mTraceObject));
    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::construct (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;

failed:
    if(queryBound == true) {
        MSG0("Unbinding NMF 'query' interface\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Unbinding NMF 'query' interface", (mTraceObject));
        nmfErr = iNmfInstance->unbindFromUser("query");
        if (nmfErr != NMF_OK) {
            MSG1("NMF 'query' interface unbinding failed (NmfErr=%d)\n", nmfErr);
            OstTraceFiltStatic1(TRACE_ERROR, "NMF 'query' interface unbinding failed (NmfErr=%d)", (mTraceObject), nmfErr);
        }
    }
    if(nmfConstructed == true) {
        MSG0("Destroying NMF component\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Destroying NMF component", (mTraceObject));
        nmfErr = iNmfInstance->destroy();
        if (nmfErr != NMF_OK) {
            MSG1("NMF component destruction failed (NmfErr=%d)\n", nmfErr);
            OstTraceFiltStatic1(TRACE_ERROR, "NMF component destruction failed (NmfErr=%d)", (mTraceObject), nmfErr);
        }
    }
    if(iNmfInstance != NULL) {
        MSG0("Destroying NMF instance\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Destroying NMF instance", (mTraceObject));
        tuningloader_wrpDestroy(iNmfInstance);
        iNmfInstance = NULL;
    }
    MSG0("Construction failed\n");
    OstTraceFiltStatic0(TRACE_ERROR, "Construction failed", (mTraceObject));
    OUTR(" ",TUNING_LOADER_MGR_NMF_ERROR);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::construct (%d)", (mTraceObject), TUNING_LOADER_MGR_NMF_ERROR);
    return TUNING_LOADER_MGR_NMF_ERROR;
}

/*
 * This function starts NMF
 */
TuningLoaderMgrError_t CTuningLoaderManager::start()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::start", (mTraceObject));
    if(iNmfInstance == NULL) {
        MSG0("NMF instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "NMF instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::start (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }
    MSG0("Starting NMF component\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Starting NMF component", (mTraceObject));
    iNmfInstance->start();
    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::start (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * This function stops NMF
 */
TuningLoaderMgrError_t CTuningLoaderManager::stop()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::stop", (mTraceObject));
    if(iNmfInstance == NULL) {
        MSG0("NMF instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "NMF instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::stop (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }
    MSG0("Stopping NMF component\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Stopping NMF component", (mTraceObject));
    iNmfInstance->stop_flush();
    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::stop (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * This function stops and destroys NMF resources
 */
TuningLoaderMgrError_t CTuningLoaderManager::destroy()
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::destroy", (mTraceObject));
    t_nmf_error nmfErr = NMF_OK;
    bool nmfErrOccured = false;
    TuningLoaderMgrError_t err = TUNING_LOADER_MGR_OK;

    if(iNmfInstance == NULL) {
        MSG0("NMF instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "NMF instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::destroy (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    MSG0("Unbinding NMF 'query' interface\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Unbinding NMF 'query' interface", (mTraceObject));
    nmfErr = iNmfInstance->unbindFromUser("query");
    if (nmfErr != NMF_OK) {
        MSG1("NMF 'query' interface unbinding failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF 'query' interface unbinding failed (NmfErr=%d)", (mTraceObject), nmfErr);
        nmfErrOccured = true;
    }

    MSG0("Unbinding NMF 'acknowledge' interface\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Unbinding NMF 'acknowledge' interface", (mTraceObject));
    nmfErr = EnsWrapper_unbindToUser( iOmxHandle, iNmfInstance, "acknowledge");
    if (nmfErr != NMF_OK) {
        MSG1("NMF 'acknowledge' interface unbinding failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF 'acknowledge' interface unbinding failed (NmfErr=%d)", (mTraceObject), nmfErr);
        nmfErrOccured = true;
    }

    MSG0("Destroying NMF component\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Destroying NMF component", (mTraceObject));
    nmfErr = iNmfInstance->destroy();
    if (nmfErr != NMF_OK) {
        MSG1("NMF component destruction failed (NmfErr=%d)\n", nmfErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF component destruction failed (NmfErr=%d)", (mTraceObject), nmfErr);
        nmfErrOccured = true;
    }

    MSG0("Destroying NMF instance\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Destroying NMF instance", (mTraceObject));
    tuningloader_wrpDestroy(iNmfInstance);

    // Destruction done
    iOmxHandle = NULL;
    iNmfInstance = NULL;
    err = nmfErrOccured ? TUNING_LOADER_MGR_NMF_ERROR : TUNING_LOADER_MGR_OK;
    MSG1("Destruction %s\n", err==TUNING_LOADER_MGR_OK ?"done":"failed");
    if (err==TUNING_LOADER_MGR_OK){
        OstTraceFiltStatic0(TRACE_DEBUG, "Destruction done", (mTraceObject));
	}
    else{ 
		OstTraceFiltStatic0(TRACE_DEBUG, "Destruction failed", (mTraceObject));
	}
    OUTR(" ",err);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::destroy (%d)", (mTraceObject), err);
    return err;
}

/*
 * This function launches the loading of the firmware tuning data
 */
TuningLoaderMgrError_t CTuningLoaderManager::loadFirmwareTuningData(const t_camera_info*    pCamInfo,
                                                                    const t_fw_blocks_info* pFwBlocksInfo)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::loadFirmwareTuningData", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFirmwareTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Aguments sanity check
    if( pCamInfo == NULL || pFwBlocksInfo == NULL)
    {
        MSG2("Bad argument: pCamInfo=%p, pFwBlocksInfo=%p\n", pCamInfo, pFwBlocksInfo);
        OstTraceFiltStatic2(TRACE_ERROR, "Bad argument: pCamInfo=0x%x, pFwBlocksInfo=0x%x", (mTraceObject), (t_uint32)pCamInfo, (t_uint32)pFwBlocksInfo);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFirmwareTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    // Send command to NMF
    MSG0("Send 'loadFirmwareTuning' command to NMF\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Send 'loadFirmwareTuning' command to NMF", (mTraceObject));
    iNmfQueryIntf.loadFirmwareTuning(*pCamInfo, *pFwBlocksInfo);

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFirmwareTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * This function launches the loading of NVM data
 */
TuningLoaderMgrError_t CTuningLoaderManager::loadNvmTuningData(const t_camera_info* pCamInfo)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::loadNvmTuningData", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Aguments sanity check
    if( pCamInfo == NULL)
    {
        MSG1("Bad argument: pCamInfo=%p\n", pCamInfo);
        OstTraceFiltStatic1(TRACE_ERROR, "Bad argument: pCamInfo=0x%x", (mTraceObject), (t_uint32)pCamInfo);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    // Send command to NMF
    MSG0("Send 'loadNvmTuning' command to NMF\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Send 'loadNvmTuning' command to NMF", (mTraceObject));
    iNmfQueryIntf.loadNvmTuning(*pCamInfo);

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}


TuningLoaderMgrError_t CTuningLoaderManager::saveNvmTuningData(const t_camera_info* pCamInfo,unsigned char* pNvm, t_uint32 aSize)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::saveNvmTuningData", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Aguments sanity check
    if( (pCamInfo == NULL) || (pNvm ==NULL) || (aSize == 0))
    {
        MSG1("Bad argument: pCamInfo=%x, pNvm=0x%x, aSize=%x\n", (unsigned int)pCamInfo,(unsigned int)pNvm,(unsigned int)aSize);
        OstTraceFiltStatic3(TRACE_ERROR, "Bad argument: pCamInfo=0x%x pNvm=0x%x, aSize=%d", (mTraceObject), (t_uint32)pCamInfo,(t_uint32)pNvm,aSize);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    // Send command to NMF
    MSG0("Send 'SaveNvmTuning' command to NMF\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Send 'loadNvmTuning' command to NMF", (mTraceObject));
    iNmfQueryIntf.saveNvmTuning(*pCamInfo,(t_uint32)pNvm,aSize);

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadNvmTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}


/*
 * This function launches the loading of the image quality tuning data
 */
TuningLoaderMgrError_t CTuningLoaderManager::loadImageQualityTuningData(const t_camera_info* pCamInfo)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::loadImageQualityTuningData", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadImageQualityTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Aguments sanity check
    if( pCamInfo == NULL)
    {
        MSG1("Bad argument: pCamInfo=%p\n", pCamInfo);
        OstTraceFiltStatic1(TRACE_ERROR, "Bad argument: pCamInfo=0x%x", (mTraceObject), (t_uint32)pCamInfo);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadImageQualityTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    // Send command to NMF
    MSG0("Send 'loadImageQualityTuning' command to NMF\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Send 'loadImageQualityTuning' command to NMF", (mTraceObject));
    iNmfQueryIntf.loadImageQualityTuning(*pCamInfo);

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadImageQualityTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * This function launches the loading of the flash tuning data
 */
TuningLoaderMgrError_t CTuningLoaderManager::loadFlashTuningData(const t_flash_info* pFlashInfo)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::loadFlashTuningData", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFlashTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Aguments sanity check
    if( pFlashInfo == NULL)
    {
        MSG1("Bad argument: pFlashInfo=%p\n", pFlashInfo);
        OstTraceFiltStatic1(TRACE_ERROR, "Bad argument: pFlashInfo=0x%x", (mTraceObject), (t_uint32)pFlashInfo);
        OUTR(" ",TUNING_LOADER_MGR_BAD_ARGUMENT);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFlashTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_BAD_ARGUMENT);
        return TUNING_LOADER_MGR_BAD_ARGUMENT;
    }

    // Send command to NMF
    MSG0("Send 'loadFlashTuning' command to NMF\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Send 'loadFlashTuning' command to NMF", (mTraceObject));
    iNmfQueryIntf.loadFlashTuning(*pFlashInfo);

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::loadFlashTuningData (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * The following functions tell whether some features are supported
 */
bool CTuningLoaderManager::hasFirmwareTuningData()
{
    return (bool)iNmfSyncIntf.hasFirmwareTuning();
}

bool CTuningLoaderManager::hasNvmTuningData()
{
    return (bool)iNmfSyncIntf.hasNvmTuning();
}

bool CTuningLoaderManager::hasImageQualityTuningData()
{
    return (bool)iNmfSyncIntf.hasImageQualityTuning();
}

bool CTuningLoaderManager::hasFlashTuningData()
{
    return (bool)iNmfSyncIntf.hasFlashTuning();
}

/*
 * This function get the config file name of the camera
 */
TuningLoaderMgrError_t CTuningLoaderManager::getCameraConfigFileName(const char** pCameraConfigFileName)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::getCameraConfigFileName", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::getCameraConfigFileName (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Ask file name to NMF component
    *pCameraConfigFileName = iNmfSyncIntf.cameraConfigFileName();

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::getCameraConfigFileName (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * This function get the config file name of the flash
 */
TuningLoaderMgrError_t CTuningLoaderManager::getFlashConfigFileName(const char** pFlashCfgFileName)
{
    IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::getFlashConfigFileName", (mTraceObject));

    // Check if constructed
    if(iNmfInstance == NULL) {
        MSG0("Instance not constructed\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Instance not constructed", (mTraceObject));
        OUTR(" ",TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::getFlashConfigFileName (%d)", (mTraceObject), TUNING_LOADER_MGR_NOT_CONSTRUCTED);
        return TUNING_LOADER_MGR_NOT_CONSTRUCTED;
    }

    // Ask file name to NMF component
    *pFlashCfgFileName = iNmfSyncIntf.flashConfigFileName();

    OUTR(" ",TUNING_LOADER_MGR_OK);
    OstTraceFiltStatic1(TRACE_FLOW, "Exit CTuningLoaderManager::getFlashConfigFileName (%d)", (mTraceObject), TUNING_LOADER_MGR_OK);
    return TUNING_LOADER_MGR_OK;
}

/*
 * NMF callback
 */
void CTuningLoaderManager::response(t_tuningloader_error aErr)
{
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CTuningLoaderManager::response", (mTraceObject));
    s_scf_event event;
    if(aErr==TUNING_LOADER_OK) {
        // NMF operation done: prepare COM_SM event
        MSG0("NMF operation completed\n");
        OstTraceFiltStatic0(TRACE_DEBUG, "NMF operation completed", (mTraceObject));
        event.sig = TUNING_LOADER_OPERATION_DONE_SIG;
        event.type.tuningLoaderEvent.error = TUNING_LOADER_OK;
    }
    else {
        // NMF operation failed: prepare COM_SM event
        MSG1("NMF operation failed: err=%d\n", aErr);
        OstTraceFiltStatic1(TRACE_ERROR, "NMF operation failed: err=%d", (mTraceObject), aErr);
        // Note: here the function iNmfSyncIntf.errorCode2String() cannot be called
        // as CTuningLoaderManager::response callback is called in the context of the ENS
        // Wrapper which has no reference to iNmfErrorIntf
        event.sig = TUNING_LOADER_OPERATION_FAILED_SIG;
        event.type.tuningLoaderEvent.error = aErr;
    }
    iSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CTuningLoaderManager::response", (mTraceObject));
}

/*
 * This function converts an enum error code into a human readable string
 */
const char* CTuningLoaderManager::errorCode2String(TuningLoaderMgrError_t aErrCode)
{
     switch(aErrCode)
     {
         case TUNING_LOADER_MGR_OK:                  return "TUNING_LOADER_MGR_OK";
         case TUNING_LOADER_MGR_ALREADY_CONSTRUCTED: return "TUNING_LOADER_MGR_ALREADY_CONSTRUCTED";
         case TUNING_LOADER_MGR_NOT_CONSTRUCTED:     return "TUNING_LOADER_MGR_NOT_CONSTRUCTED";
         case TUNING_LOADER_MGR_BAD_ARGUMENT:        return "TUNING_LOADER_MGR_BAD_ARGUMENT";
         case TUNING_LOADER_MGR_NMF_ERROR:           return "TUNING_LOADER_MGR_NMF_ERROR";
     }
     return "unrecognized CTuningLoaderManager error code";
}

/*
 * This function converts an enum error code into a human readable string
 */
const char* CTuningLoaderManager::loaderErrorCode2String(t_sint32 aErr)
{
    // synchronous call of the tuning loader errorCode2String() function
    return iNmfSyncIntf.errorCode2String((t_tuningloader_error)aErr);
}
