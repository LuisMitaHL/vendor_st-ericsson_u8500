/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define DBGT_PREFIX "CFlashSequencer"


#include "hsmcam.h"
#include "ImgConfig.h"
#include "flash.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_flashTraces.h"
#endif

CFlashSequencer::CFlashSequencer(TraceObject *traceobj):mTraceObject(traceobj)
{
    mController = NULL;
    mpClbk = NULL;
    mctxtHnd = NULL;
    mpTimerStartClbk = NULL;
    mpTimerStopClbk = NULL;
    mTimerctxtHnd = NULL; 
    curSeq = NONE_SEQ;
    i32_remSteps = 0;
}


cam_flash_err_e CFlashSequencer::setTimerCallbacks(flashSeqTimerStartClbk_t pTimerStart, flashSeqTimerStopClbk_t pTimerStop, flashSeqClbkCtxtHnd_t ctxtHnd) {
    if ((pTimerStart == NULL) || (pTimerStop == NULL)) {
        return CAM_FLASH_ERR_BAD_PARAMETER;
    }
    mpTimerStartClbk = pTimerStart;
    mpTimerStopClbk = pTimerStop;
    mTimerctxtHnd = ctxtHnd;   
    return CAM_FLASH_ERR_NONE;
}

cam_flash_err_e CFlashSequencer::startSeq(sequenceID_t sequence, CFlashController *pController, flashSeqClbk_t pClbk, flashSeqClbkCtxtHnd_t ctxtHnd) {
    IN0("\n");
    if (pController == NULL) {      // Error: a valid controller handle must be passed.
        OUT0("\n");
        return CAM_FLASH_ERR_BAD_PARAMETER;
    }
    if (curSeq != NONE_SEQ) {   // A running sequence cannot be interrupted
        OUT0("\n");
        return CAM_FLASH_ERR_BAD_PRECONDITION; 
    }
    curSeq = sequence;
    switch (sequence) {
        case NONE_SEQ:
            i32_remSteps = 0;
            break;
        case RED_EYE_REMOVAL_SEQ:
            i32_remSteps = sizeof(a_RER_sequence) / sizeof(cam_flashSeq_atom_t);
            break;
        case PRIVACY_SEQ:
            i32_remSteps = sizeof(a_PI_sequence) / sizeof(cam_flashSeq_atom_t);
            break;
    }
    mController = pController;
    mpClbk = pClbk;
    mctxtHnd = ctxtHnd;
    sigTimer(); // To be called after delay by the chosen timing service.
    OUT0("\n");
    return CAM_FLASH_ERR_NONE;
}

void CFlashSequencer::sigTimer() {
    IN0("\n");
    cam_flashSeq_atom_t const *pSeq = NULL;
    if (i32_remSteps > 0) { // Some flash action still to be done
        switch (curSeq) {
            default:
            case NONE_SEQ:
                DBGT_ERROR("CFlashSequencer::sigTimer - Error: NONE_SEQ cannot be ticked.\n");
                OstTraceFiltStatic0(TRACE_ERROR, "CFlashSequencer::sigTimer - Error: NONE_SEQ cannot be ticked.", (mTraceObject));
                break;
            case RED_EYE_REMOVAL_SEQ:
                pSeq = &a_RER_sequence[sizeof(a_RER_sequence) / sizeof(cam_flashSeq_atom_t) - i32_remSteps];
                break;
            case PRIVACY_SEQ:
                pSeq = &a_PI_sequence[sizeof(a_PI_sequence) / sizeof(cam_flashSeq_atom_t) - i32_remSteps];
                break; 
        }
        if (NULL != pSeq) {
            doAtom(pSeq);
        }
        i32_remSteps--;
    } else {            // End of sequence: call the user back if requested, and clean the request.
        MSG1("CFlashSequencer::sigTimer - Sequence %d completed.\n", curSeq);
        curSeq = NONE_SEQ;
        if (mpClbk != NULL) {
            mpClbk(mctxtHnd);
            mpClbk = NULL;
            mctxtHnd = NULL;
        }
    }
    OUT0("\n");
}

void CFlashSequencer::doAtom(cam_flashSeq_atom_t const *pAtom) {
    IN0("\n");
    t_sw3A_FlashDirective flashDirective;
    flashDirective.flashState = pAtom->flashState;
    flashDirective.flashDurationUs = 0;    // Manually strobed, so there is no pre-defined duration.
    flashDirective.flashPower = pAtom->flashPower_per;
    flashDirective.bNeedPreCaptureFlash = false;
    flashDirective.bNeedPostCaptureFlash = false;
    if (mController != NULL) {
        cam_flash_err_e err = mController->configure(&flashDirective);
        if (CAM_FLASH_ERR_NONE != err) {
            DBGT_ERROR("Error: Flash configuration failed.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash configuration failed.", (mTraceObject));
            DBC_ASSERT(0);
        }
    } else {
        DBGT_ERROR("Error: Cannot run a flash sequence atom without a valid flash controller.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "Error: Cannot run a flash sequence atom without a valid flash controller.", (mTraceObject));
        DBC_ASSERT(0);
    }
    mpTimerStartClbk(pAtom->flashDuration_ms, mTimerctxtHnd);
    OUT0("\n");
}

cam_flash_err_e CFlashSequencer::cancelSeq() {
    IN0("\n");
    i32_remSteps = 0;
    cam_flash_err_e cam_err = mController->unConfigure();
    if (cam_err != CAM_FLASH_ERR_NONE) {
        MSG0("Error: Could not un-configure the flash driver.\n");
        OUT0("\n");
        return CAM_FLASH_ERR_DRIVER;
    }
    // WARNING: this is valid only if the end of sequence callback implementation can be called from the cancelSeq() caller stack.
    sigTimer();
    OUT0("\n");
    return CAM_FLASH_ERR_NONE;
}


TFlashReturnCode validateFlashDetails(TFlashDetails *pDetails) {
    if (((true == pDetails->IsIntensityProgrammable) && (pDetails->MinIntensity >= pDetails->MaxIntensity) && (pDetails->MaxIntensity == 0)) ||
		((false == pDetails->IsIntensityProgrammable) && (pDetails->MinIntensity != pDetails->MaxIntensity) && (pDetails->MaxIntensity == 0))) {
            return FLASH_RET_UNSUPPORTED_SETTING;
    } else {
        return FLASH_RET_NONE;
    }
}

CFlashController::CFlashController(TraceObject *traceobj):mTraceObject(traceobj)
{
    mFlashDriver = NULL;
    mFlashDriverCamId = EPrimary;
    mFlashModesSet = SW3A_FLASH_OFF;
    mFlashDirective_cur.flashState = SW3A_FLASH_OFF;
    mFlashDirective_cur.flashPower = 0;
    mFlashDirective_cur.flashDurationUs = 0;
    mFlashDirective_cur.bNeedPreCaptureFlash = false;
    mFlashDirective_cur.bNeedPostCaptureFlash = false;
    mDriverSupportedFlashModes = FLASH_MODE_NONE;
    mFlashDriverMode = FLASH_MODE_NONE;
    memset(&mFlashDriverModeDetails, 0, sizeof(TFlashDetails));
    mbDisabled = false;
    mbNewDirective = false;

    mFlashDirective_pre.flashState = SW3A_FLASH_OFF;
    mFlashDirective_pre.flashPower = 0;
    mFlashDirective_pre.flashDurationUs = 0;
    mFlashDirective_pre.bNeedPreCaptureFlash = false;
    mFlashDirective_pre.bNeedPostCaptureFlash = false;
}

void CFlashController::updateSupportedFlashModes(enumCameraSlot camSlot) {
    IN0("\n");
    if (mFlashDriver != NULL) {
        mFlashDriverCamId = (camSlot == ePrimaryCamera) ? EPrimary : ESecondary;
        mFlashDriver->GetSupportedFlashModes(mDriverSupportedFlashModes, mFlashDriverCamId);
    } else {
        DBGT_ERROR("updateSupportedFlashModes: No valid flash driver.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "updateSupportedFlashModes: No valid flash driver.", (mTraceObject));
        DBC_ASSERT(0);
        mDriverSupportedFlashModes = FLASH_MODE_NONE;
    }
    
    mFlashModesSet = translateFlashModes(mDriverSupportedFlashModes);

    MSG1("Flash Driver : Supported Flash modes =  %d\n", (int)mFlashModesSet);
    OUT0("\n");
};

void CFlashController::setDirective(t_sw3A_FlashDirective *pFlashDirective) { 
    if ((pFlashDirective->flashState    != mFlashDirective_cur.flashState) 
        || (pFlashDirective->flashPower      != mFlashDirective_cur.flashPower) 
        || (pFlashDirective->flashDurationUs != mFlashDirective_cur.flashDurationUs))
    {
        mbNewDirective = true;
    }
    mFlashDirective_cur = *pFlashDirective;
}

t_sw3A_FlashModeSet CFlashController::translateFlashModes(TFlashMode flashModes) {
    t_sw3A_FlashModeSet flashModesSet = 0;

    flashModesSet |= (flashModes & FLASH_MODE_XENON)                        ? SW3A_FLASH_MODE_XENON : 0;
    flashModesSet |= (flashModes & FLASH_MODE_XENON_EXTERNAL_STROBE)        ? SW3A_FLASH_MODE_XENON_EXTERNAL_STROBE : 0;
    flashModesSet |= (flashModes & FLASH_MODE_VIDEO_LED)                    ? SW3A_FLASH_MODE_VIDEO_LED : 0;
    flashModesSet |= (flashModes & FLASH_MODE_VIDEO_LED_EXTERNAL_STROBE)    ? SW3A_FLASH_MODE_VIDEO_LED_EXTERNAL_STROBE : 0;
    flashModesSet |= (flashModes & FLASH_MODE_STILL_LED)                    ? SW3A_FLASH_MODE_STILL_LED : 0;
    flashModesSet |= (flashModes & FLASH_MODE_STILL_LED_EXTERNAL_STROBE)    ? SW3A_FLASH_MODE_STILL_LED_EXTERNAL_STROBE : 0;
    flashModesSet |= (flashModes & FLASH_MODE_AF_ASSISTANT)                 ? SW3A_FLASH_MODE_AF_ASSISTANT : 0;
    flashModesSet |= (flashModes & FLASH_MODE_INDICATOR)                    ? SW3A_FLASH_MODE_INDICATOR : 0;
    flashModesSet |= (flashModes & FLASH_MODE_STILL_HPLED)                  ? SW3A_FLASH_MODE_STILL_HPLED : 0;
    flashModesSet |= (flashModes & FLASH_MODE_STILL_HPLED_EXTERNAL_STROBE)  ? SW3A_FLASH_MODE_STILL_HPLED_EXTERNAL_STROBE : 0;
    
    OUT0("\n");
    return flashModesSet;
}

// Compare with previous state, and update current and previous
cam_flash_err_e CFlashController::configure(t_sw3A_FlashDirective *pFlashDirective)
{
    IN0("\n");
    MSG3("CFlashController::configure State=%d Duration=%dus Power=%d%%\n", (int) pFlashDirective->flashState, (int) pFlashDirective->flashDurationUs, (int) pFlashDirective->flashPower);
    if (mFlashDriver == NULL) {
        DBGT_ERROR("configure: No valid flash driver.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "configure: No valid flash driver.", (mTraceObject));
        DBC_ASSERT(0);
        OUT0("\n");
        return CAM_FLASH_ERR_BAD_PRECONDITION;
    }
    if (mDriverSupportedFlashModes == 0)
    {
        MSG0("CFlash Driver : mDriverSupportedFlashModes 0 \n");
        OUT0("\n");
        return CAM_FLASH_ERR_NONE;
    } 
	
	if((mFlashDirective_pre.flashState != pFlashDirective->flashState)||
       (mFlashDirective_pre.flashPower != pFlashDirective->flashPower)||
       (mFlashDirective_pre.flashDurationUs != pFlashDirective->flashDurationUs))
        {
        mFlashDirective_pre.flashState = pFlashDirective->flashState;
        mFlashDirective_pre.flashPower = pFlashDirective->flashPower;
        mFlashDirective_pre.flashDurationUs = pFlashDirective->flashDurationUs;
        }
    else
        {
        MSG0("CFlash Driver : mFlashDirective_pre are same \n");
        return CAM_FLASH_ERR_NONE;
        }
	
	
	   
    bool bDoStrobe = false;
    TFlashMode refDriverMode = FLASH_MODE_NONE;
     t_uint32 duration = 0;
     duration = pFlashDirective->flashDurationUs;
    // flash is always disabled
    /* FIXME: possibly implement via inhibate(), or prevent flash requests from sw3A when in timenudge mode.
    if (OMX_TRUE == pOpModeMgr->IsTimeNudgeEnabled()) {
        flashState = SW3A_FLASH_OFF;
    }
     * */
    if (mbDisabled) {
        pFlashDirective->flashState = SW3A_FLASH_OFF;
    }

    switch(pFlashDirective->flashState) {
        case SW3A_FLASH_OFF:
            refDriverMode = FLASH_MODE_NONE;
            bDoStrobe = false;
            break;
        case SW3A_FLASH_PREFLASH:
//#if IMG_CONFIG_PREFLASH_USING_TORCHMODE	
//            refDriverMode = FLASH_MODE_VIDEO_LED;
//            bDoStrobe = true;
//            break;
//#endif /* IMG_CONFIG_PREFLASH_USING_TORCHMODE*/ 
        /* If pre-flash is not implemented by a torch mode, it is handled as a main flash. */
        case SW3A_FLASH_MAINFLASH:
            refDriverMode = FLASH_MODE_STILL_LED_EXTERNAL_STROBE;
	     duration=0; /*ER: 367887*/	
            bDoStrobe = false;
            break;
        case SW3A_FLASH_PREFLASH_HPLED:
        case SW3A_FLASH_MAINFLASH_HPLED:
            refDriverMode = FLASH_MODE_STILL_LED_EXTERNAL_STROBE;
            bDoStrobe = false;
            break;
        case SW3A_FLASH_AFASSIST:
            refDriverMode = FLASH_MODE_AF_ASSISTANT;
            bDoStrobe = true;
            break;
        case SW3A_FLASH_INDICATOR:
            refDriverMode = FLASH_MODE_INDICATOR;
            bDoStrobe = true;
            break;
        case SW3A_FLASH_VIDEOLED:
            refDriverMode = FLASH_MODE_VIDEO_LED;
            bDoStrobe = true;
            break;
        default:
            DBGT_ERROR("configureFlash: Error: Unknown flash type %d\n", pFlashDirective->flashState);
            OstTraceFiltStatic1(TRACE_ERROR, "configureFlash: Error: Unknown flash type %d", (mTraceObject), pFlashDirective->flashState);
            DBC_ASSERT(0);
            OUT0("\n");
            return CAM_FLASH_ERR_BAD_PARAMETER;
    }
    
    if ((refDriverMode != FLASH_MODE_NONE) && ((refDriverMode & mFlashModesSet) == 0)) {
        // The requested mode is not supported. Skip the configuration.
        MSG1("Warning: the requested flash mode (0x%08x) is not supported by the flash driver. Discard the configuration.\n", refDriverMode);
        OstTraceFiltStatic1(TRACE_WARNING, "Warning: the requested flash mode (0x%08x) is not supported by the flash driver. Discard the configuration.", (mTraceObject), refDriverMode);
        OUT0("\n");
        return CAM_FLASH_ERR_NONE;
    }
    
    t_uint32 intensity = 0;
    t_uint32 timeout = 0;
    TFlashReturnCode driver_err = FLASH_RET_NONE;
    TFlashDetails details;
    
    MSG2("CFlash Driver : refDriverMode=%d : mFlashDriverMode=%d\n", refDriverMode, mFlashDriverMode);
    if (refDriverMode != mFlashDriverMode) {
        MSG0("CFlash Driver : refDriverMode != mFlashDriverMode \n");
        cam_flash_err_e cam_err = unConfigure();
        if (cam_err != CAM_FLASH_ERR_NONE) {
            DBGT_ERROR("Error: Could not un-configure the flash driver.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Could not un-configure the flash driver.", (mTraceObject));
	    OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
		
	if (refDriverMode != FLASH_MODE_NONE) //ER: 406525
	{	MSG0("CFlash Driver : refDriverMode != FLASH_MODE_NONE \n");
		driver_err = mFlashDriver->GetFlashModeDetails(refDriverMode, details, mFlashDriverCamId);
		if (driver_err != FLASH_RET_NONE)
		{
			DBGT_ERROR("Error: Flash details could not be retrieved.\n");  // NB: might not work for NONE mode
			OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash details could not be retrieved.", (mTraceObject));  // NB: might not work for NONE mode
			OUT0("\n");
			return CAM_FLASH_ERR_DRIVER;
		}
	}
	
        /* No added value with this check, at least for the time being.
        driver_err = validateFlashDetails(&details);
        if (driver_err != FLASH_RET_NONE) {
            DBGT_ERROR("Error: Invalid flash details.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Invalid flash details.", (mTraceObject));
            OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
        */
        driver_err = mFlashDriver->EnableFlashMode(refDriverMode, NULL, NULL, mFlashDriverCamId);
        if (driver_err != FLASH_RET_NONE) {
            DBGT_ERROR("Error: Flash mode could not be enabled.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash mode could not be enabled.", (mTraceObject));
            OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
        mFlashDriverMode = refDriverMode;
        mFlashDriverModeDetails = details;
    }
    if (mFlashDriverMode != FLASH_MODE_NONE) {  // Configuring the NONE mode is not allowed by the driver.
        MSG0("CFlash Driver : computeIntensity \n");
        intensity = computeIntensity(pFlashDirective, &mFlashDriverModeDetails);
#if (IMG_CONFIG_SW_PLATFORM == 1)
        if(pFlashDirective->flashDurationUs == 0) {
            timeout = 0;
        }
        else {
            timeout = mFlashDriverModeDetails.MaxStrobeDuration;
        }
        driver_err = mFlashDriver->ConfigureFlashMode(mFlashDriverMode, duration, intensity, timeout, mFlashDriverCamId); //ER 365857
#else
        driver_err = mFlashDriver->ConfigureFlashMode(mFlashDriverMode, duration, intensity, timeout, mFlashDriverCamId);		
#endif
        if (driver_err != FLASH_RET_NONE) {
            DBGT_ERROR("Error: Flash mode could not be configured.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Flash mode could not be configured.", (mTraceObject));
            OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
    }
    if (bDoStrobe) {
        MSG0("CFlash Driver : bDoStrobe \n");
        driver_err = mFlashDriver->Strobe(mFlashDriverMode, true, mFlashDriverCamId);
        if (driver_err != FLASH_RET_NONE) {
            DBGT_ERROR("Error: Manual strobe failed.\n");
            OstTraceFiltStatic0(TRACE_ERROR, "Error: Manual strobe failed.", (mTraceObject));
	    OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
    }
    
    OUT0("\n");
    return CAM_FLASH_ERR_NONE;
}


cam_flash_err_e CFlashController::unConfigure()
{
    IN0("\n");
   if (mFlashDriver == NULL) {
        DBGT_ERROR("unConfigure: No valid flash driver.\n");
        OstTraceFiltStatic0(TRACE_ERROR, "unConfigure: No valid flash driver.", (mTraceObject));
        DBC_ASSERT(0);
        OUT0("\n");
        return CAM_FLASH_ERR_BAD_PRECONDITION;
    }
    if (mDriverSupportedFlashModes == 0)
    {
        OUT0("\n");
        return CAM_FLASH_ERR_NONE;
    }
    TFlashReturnCode driver_err = FLASH_RET_NONE;

    // User-controlled modes: switch off
    if ((FLASH_MODE_INDICATOR    == mFlashDriverMode)
     || (FLASH_MODE_AF_ASSISTANT == mFlashDriverMode)
     || (FLASH_MODE_VIDEO_LED    == mFlashDriverMode))
    {
        driver_err = mFlashDriver->Strobe(mFlashDriverMode, false, mFlashDriverCamId);
        if (driver_err != FLASH_RET_NONE) {
            DBGT_ERROR("unConfigure: Strobe error;\n");
            OstTraceFiltStatic0(TRACE_ERROR, "unConfigure: Strobe error;", (mTraceObject));
	    OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
    }

    // Any mode: un-configure
    if (FLASH_MODE_NONE != mFlashDriver)
    {
        driver_err = mFlashDriver->EnableFlashMode(FLASH_MODE_NONE, NULL, NULL, mFlashDriverCamId);
        if (driver_err != FLASH_RET_NONE)
        {
            DBGT_ERROR("unConfigure: EnableFlashMode error;\n");
            OstTraceFiltStatic0(TRACE_ERROR, "unConfigure: EnableFlashMode error;", (mTraceObject));
	    OUT0("\n");
            return CAM_FLASH_ERR_DRIVER;
        }
        mFlashDriverMode = FLASH_MODE_NONE;
    }
    OUT0("\n");
    return CAM_FLASH_ERR_NONE;
}

t_uint32 CFlashController::computeIntensity(t_sw3A_FlashDirective *pFlashDirective, TFlashDetails *pFlashDriverModeDetails)
{
    t_uint32 intensity = 0;
    /* Check flashDirective validity.
     * In case the desired flash mode does not allow to pass a power parameter, 0 should be used. */
    /*if ((pFlashDirective->flashPower < 0) || (pFlashDirective->flashPower > 100)) {
        MSG1("computeFlashIntensity - Error: out of range power received from SW3A: %d\n", pFlashDirective->flashPower);
        MSG0("computeFlashIntensity: defaulting to 100%% power\n");
        intensity = 100;
    } else {
        intensity = pFlashDirective->flashPower;
    }*/
	
    /*ER: 374656, ER 365857 */
    //intensity = ((pFlashDriverModeDetails->MaxIntensity - pFlashDriverModeDetails->MinIntensity) * intensity ) / 100 + pFlashDriverModeDetails->MinIntensity;
    
    intensity = (t_uint32)((100 * pFlashDirective->flashPower) / pFlashDriverModeDetails->MaxIntensity);

    return intensity;
}

