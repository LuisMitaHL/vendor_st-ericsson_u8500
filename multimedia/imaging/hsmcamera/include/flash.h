/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#include "flash_api.h"

/** Callback context handle passed back to the caller */
typedef void * flashSeqClbkCtxtHnd_t;

/** End of sequence callback function pointer; to be registered from sequencer client */
typedef void (* flashSeqClbk_t)(flashSeqClbkCtxtHnd_t ctxtHnd);

/** Timer services implemented by the sequencer client: Callback functions pointers */
typedef void (* flashSeqTimerStartClbk_t)(t_uint32 sleepTime_ms, flashSeqClbkCtxtHnd_t ctxtHnd);
typedef void (* flashSeqTimerStopClbk_t)(flashSeqClbkCtxtHnd_t ctxtHnd);    // TOTO: the stop callback may not be needed as we can start for one time only.

typedef enum {
    CAM_FLASH_ERR_NONE = 0,
    CAM_FLASH_ERR_BAD_PARAMETER, 
    CAM_FLASH_ERR_BAD_PRECONDITION,
    CAM_FLASH_ERR_DRIVER
} cam_flash_err_e;

typedef struct {
    t_sw3A_FlashState flashState;
    t_uint32          flashDuration_ms;
    t_uint32          flashPower_per;
} cam_flashSeq_atom_t;


/** Red-eye removal pre-flash sequence */
static const cam_flashSeq_atom_t a_RER_sequence[] = { 
    {SW3A_FLASH_VIDEOLED, 100, 100},
    {SW3A_FLASH_OFF, 100, 0},
    {SW3A_FLASH_VIDEOLED, 100, 20},   // {SW3A_FLASH_INDICATOR, 100, 100},
    {SW3A_FLASH_OFF, 100, 0},
    {SW3A_FLASH_VIDEOLED, 100, 20},   // {SW3A_FLASH_INDICATOR, 100, 100},
    {SW3A_FLASH_OFF, 100, 0},
    {SW3A_FLASH_VIDEOLED, 100, 20},   // {SW3A_FLASH_INDICATOR, 100, 100},
    {SW3A_FLASH_OFF, 200, 0}
}; 

/** Privacy indicator sequence */
static const cam_flashSeq_atom_t a_PI_sequence[] = {
    {SW3A_FLASH_VIDEOLED, 60, 10},  // {SW3A_FLASH_INDICATOR, 100, 100},
    {SW3A_FLASH_OFF, 1, 0}, //  Leave the flash off after the post-capture flash sequence.
};  


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CFlashController);
#endif

class CFlashController 
{
public:
    CFlashController(TraceObject *traceobj);
    void setFlashDriver(CFlashDriver *pFlashDriver) { mFlashDriver = pFlashDriver; }
    void updateSupportedFlashModes(enumCameraSlot camSlot);
    t_sw3A_FlashModeSet getSupportedFlashModes() { return mFlashModesSet; }
    bool isFlashModeSupported() { return (mDriverSupportedFlashModes != 0) ? true : false; }
   void setDirective(t_sw3A_FlashDirective *pFlashDirective);
    t_sw3A_FlashDirective getDirective() { return mFlashDirective_cur; }
    bool isDirectiveNewAClear() { bool bRet = mbNewDirective; mbNewDirective = false; return bRet; }
    cam_flash_err_e configure(t_sw3A_FlashDirective *pFlashDirective);
    cam_flash_err_e unConfigure();

    void inhibate(bool bInhib) { mbDisabled = bInhib; }
//    static t_sw3A_FlashModeSet translateSupportedFlashModes(TFlashMode flashModes);

    TraceObject* mTraceObject; // TO: check why this should be public.
    
private:
    static t_uint32 computeIntensity(t_sw3A_FlashDirective *pFlashDirective, TFlashDetails *pFlashDriverModeDetails);
    static t_sw3A_FlashModeSet translateFlashModes(TFlashMode flashModes);
    
    CFlashDriver *mFlashDriver;
    TCameraId mFlashDriverCamId;
    t_sw3A_FlashModeSet   mFlashModesSet;
    t_sw3A_FlashDirective mFlashDirective_cur;  // Used only as temporary storage between the SW3A callback and the SW3A state machine data usage.
    t_sw3A_FlashDirective mFlashDirective_pre;  // Used to block setting the flash driver if the previous setting is same as old.
	
    TFlashMode mDriverSupportedFlashModes;
    TFlashMode mFlashDriverMode;
	TFlashDetails mFlashDriverModeDetails;
    bool mbDisabled;
    bool mbNewDirective;
};

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CFlashSequencer);
#endif

class CFlashSequencer 
{
public:
    typedef enum {
        NONE_SEQ = 0,
        RED_EYE_REMOVAL_SEQ,
        PRIVACY_SEQ
    } sequenceID_t;
    
    CFlashSequencer(TraceObject *traceobj);
    cam_flash_err_e setTimerCallbacks(flashSeqTimerStartClbk_t pTimerStart, flashSeqTimerStopClbk_t pTimerStop, flashSeqClbkCtxtHnd_t ctxtHnd);
    /** To be called:
     *   - RED_EYE_REMOVAL_SEQ: after entering still capture mode, and before the SW3A startLoop() feeback 
     *      is used to configure the flash driver for the pre-flashes.
     *   - PRIVACY_SEQ: after an end of BMS event. 
     * @param sequence: flash sequence identifier
     * @param pController:  reference to a live flash controller. The controller shall not be destroyed until
     *                  potentially on-going sequences are finished (i.e. until the callback is called).
     * @param pClbk:    Callback function pointer. Called once the requested sequence is complete.
     * @param ctxtHnd:  Callback context handle, passed back as a callback function parameter.
     * */
    cam_flash_err_e startSeq(sequenceID_t sequence, CFlashController *pController, flashSeqClbk_t pClbk, flashSeqClbkCtxtHnd_t ctxtHnd);
    cam_flash_err_e cancelSeq();
    void sigTimer();
    
    TraceObject* mTraceObject; // Check why this should be public.
private:
    void doAtom(cam_flashSeq_atom_t const *pAtom);

    CFlashController *mController;
    flashSeqClbk_t mpClbk;
    flashSeqClbkCtxtHnd_t mctxtHnd;
    flashSeqTimerStartClbk_t mpTimerStartClbk;
    flashSeqTimerStopClbk_t mpTimerStopClbk;
    flashSeqClbkCtxtHnd_t mTimerctxtHnd;
    
    sequenceID_t curSeq;
    t_sint32 i32_remSteps;
};



#endif /* __FLASH_H__ */
