/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_DATA_BASE"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "tuning_data_base.h"
#include "omxilosalservices.h"
#include "osi_trace.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningDataBaseMutex);
#endif

class CTuningDataBaseMutex
{
    public:
        CTuningDataBaseMutex();
        ~CTuningDataBaseMutex();
        bool created();
        void lock();
        void unlock();
    private:
        omxilosalservices::OmxILOsalMutex* pMutex;
};

CTuningDataBaseMutex::CTuningDataBaseMutex()
{
    // Note: must not issue any printf/MSG calls here as this constructor
    // is called when the DLL is being loading
    pMutex = NULL;
    OMX_ERRORTYPE mutexErr = omxilosalservices::OmxILOsalMutex::MutexCreate(pMutex);
    if(mutexErr != OMX_ErrorNone)
        pMutex = NULL;
}

bool CTuningDataBaseMutex::created()
{
    MSG1("Mutex created: %s\n", pMutex==NULL ? "no" : "yes");
    return pMutex==NULL ? false : true;
}

void CTuningDataBaseMutex::lock()
{
    MSG0("Locking mutex\n");
    pMutex->MutexLock();
}

void CTuningDataBaseMutex::unlock()
{
    MSG0("Unlocking mutex\n");
    pMutex->MutexUnlock();
}

CTuningDataBaseMutex::~CTuningDataBaseMutex()
{
    MSG0("Freeing mutex\n");
    omxilosalservices::OmxILOsalMutex::MutexFree(pMutex);
}

// This global variable ensures that the mutext has been created and ready
// to be used (i.e. constructor was called) once the DLL is loaded.
static CTuningDataBaseMutex TheTuningDbMutex;

/*
 * Tuning Data Base Singleton
 */
CTuningDataBase* CTuningDataBase::pInstance = NULL;
int CTuningDataBase::iNbClients = 0;

CTuningDataBase::CTuningDataBase():
    iTuningStillPreview("Still-Preview"),
    iTuningStillCapture("Still-Capture"),
    iTuningVideoPreview("Video-Preview"),
    iTuningVideoRecord("Video-Record"),
    iTuningImageQualityData("Image-Quality-Data"),
    iTuningCharacterizationData("Characterization-Data"),
    iTuningCalibrationData("Calibration-Data"),
    iTuningFlashData("Flash-Data"),
    iTuningFirmwareData("Firmware-Data"),
    iTuningSensorNvmData("Sensor-NVM-Data"),
    iTuningFlashNvmData("Flash-NVM-Data"),
    iTuningGridiron("Gridiron"),
    iTuningLinearizationRR("Linearization-RR"),
    iTuningLinearizationGR("Linearization-GR"),
    iTuningLinearizationGB("Linearization-GB"),
    iTuningLinearizationBB("Linearization-BB")
{
    IN0(" ");
    OUT0(" ");
}

CTuningDataBase::~CTuningDataBase()
{
    IN0(" ");
    OUT0(" ");
}

EXPORT_C CTuningDataBase* CTuningDataBase::getInstance(const char* aClientName)
{
    IN0(" ");
    if(TheTuningDbMutex.created()==false)
    {
        // Something went wrong at mutex creation, we cannot go further
        MSG0("Mutex not created: could not create the TuningDataBase singleton\n");
        return NULL;
    }

    TheTuningDbMutex.lock();
    if (pInstance==NULL)
    {
        MSG0("Creating the TuningDataBase singleton\n");
        // Very first getInstace call: intantiate singleton
        pInstance = new CTuningDataBase;
        if(pInstance == NULL)
        {
            MSG0("TuningDataBase singleton instantiation failed\n");
            TheTuningDbMutex.unlock();
            return NULL;
        }
        MSG0("TuningDataBase singleton creation done\n");
    }
    CTuningDataBase::iNbClients++;
    TheTuningDbMutex.unlock();
    MSG2("Client [%s] bound to TuningDataBase=%p\n", aClientName, pInstance);
    MSG1("TuningDataBase: %d clients\n", CTuningDataBase::iNbClients);
    OUT0(" ");
    return pInstance;
}

EXPORT_C void CTuningDataBase::releaseInstance(const char* aClientName)
{
    IN0(" ");
    TheTuningDbMutex.lock();
    CTuningDataBase::iNbClients--;
    MSG1("Client [%s] unbound from TuningDataBase\n", aClientName);
    MSG1("TuningDataBase: %d clients\n", CTuningDataBase::iNbClients);
    if (CTuningDataBase::iNbClients<=0)
    {
        MSG1("No more clients, deleting TuningDataBase instance %p\n", pInstance);
        delete pInstance;
        pInstance = NULL;
    }
    TheTuningDbMutex.unlock();
    OUT0(" ");
}

EXPORT_C CTuning* CTuningDataBase::getObject(t_tuning_object aObjectId)
{
    CTuning* pTuning = NULL;
    switch(aObjectId)
    {
        case TUNING_OBJ_STILL_PREVIEW:
            pTuning = &iTuningStillPreview;
            break;
        case TUNING_OBJ_STILL_CAPTURE:
            pTuning = &iTuningStillCapture;
            break;
        case TUNING_OBJ_VIDEO_PREVIEW:
            pTuning = &iTuningVideoPreview;
            break;
        case TUNING_OBJ_VIDEO_RECORD:
            pTuning = &iTuningVideoRecord;
            break;
        default:
            pTuning = NULL;
            break;
    }
    MSG2("Object [%s] (%d)\n", pTuning==NULL?"Unknown":pTuning->GetInstanceName(), aObjectId);
    return pTuning;
}

EXPORT_C CTuningBinData* CTuningDataBase::getObject(t_tuning_bin_object aObjectId)
{
    CTuningBinData* pTuningBin = NULL;
    switch(aObjectId)
    {
        case TUNING_OBJ_IMAGE_QUALITY_DATA:
            pTuningBin = &iTuningImageQualityData;
            break;
        case TUNING_OBJ_CHARACTERIZATION_DATA:
            pTuningBin = &iTuningCharacterizationData;
            break;
        case TUNING_OBJ_CALIBRATION_DATA:
            pTuningBin = &iTuningCalibrationData;
            break;
        case TUNING_OBJ_FLASH_DATA:
            pTuningBin = &iTuningFlashData;
            break;
        case TUNING_OBJ_FIRMWARE_DATA:
            pTuningBin = &iTuningFirmwareData;
            break;
        case TUNING_OBJ_SENSOR_NVM_DATA:
            pTuningBin = &iTuningSensorNvmData;
            break;
        case TUNING_OBJ_FLASH_NVM_DATA:
            pTuningBin = &iTuningFlashNvmData;
            break;
        case TUNING_OBJ_GRIDIRON:
            pTuningBin = &iTuningGridiron;
            break;
        case TUNING_OBJ_LINEARIZATION_RR:
            pTuningBin = &iTuningLinearizationRR;
            break;
        case TUNING_OBJ_LINEARIZATION_GR:
            pTuningBin = &iTuningLinearizationGR;
            break;
        case TUNING_OBJ_LINEARIZATION_GB:
            pTuningBin = &iTuningLinearizationGB;
            break;
        case TUNING_OBJ_LINEARIZATION_BB:
            pTuningBin = &iTuningLinearizationBB;
            break;
        default:
            pTuningBin = NULL;
            break;
    }
    MSG2("Object [%s] (%d)\n", pTuningBin!=NULL?pTuningBin->getName():"Unknown", aObjectId);
    return pTuningBin;
}
