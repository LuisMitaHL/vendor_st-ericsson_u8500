/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SW_3A_INTERNAL_HPP__
#define __SW_3A_INTERNAL_HPP__

// Traces
#define OMX_TRACE_UID 0x8
#define OMXCOMPONENT "SW_3A"
#include "osi_trace.h"

// Programming interface
#include "OMX_Image.h"
#include "IFM_Types.h"

//STE3A
#include "ste3a.h"
#include "fgal.h"
#include "cmal.h"
#include "ppal.h"

#include "sw_3A_types.hpp"
#include "sw_3A_list.hpp"
#include "sw_3A_trace.hpp"
#include "VhcElementDefs.h"
#include "mmhwbuffer.h"         // ISP/SW shared memory allocation
#include "pictor.h"             // Gamma LUT size

#include <cm/inc/cm_macros.h>
#include <cm/inc/cm.h>

#ifdef DEBUG
#    include "ENS_DBC.h"
#    define SW3A_ASSERT(_a) DBC_ASSERT((_a))
#else /* DEBUG */
#    define SW3A_ASSERT(_a)
#endif /* DEBUG */

#ifdef __DBGLOG
#    warning "los isn't supported in production components, This component has to be cleaned"
#    include <los/api/los_api.h>
#    include "omxilosalservices.h"
#    define IMG_LOG(a,b,c,d,e,f,g) { \
                OMX_U64 u64_time = omxilosalservices::OmxILOsalTimer::GetSystemTime(); \
                LOS_Log("[3A_TIME : %u : %u :] : ", (unsigned long)(u64_time & 0xFFFFFFFF), (unsigned long)((u64_time - iSw3AlibCtxt->u64_time_previous) & 0xFFFFFFFF)); \
                iSw3AlibCtxt->u64_time_previous = u64_time; \
                LOS_Log(a,b,c,d,e,f,g); \
                }
#else
#    define IMG_LOG(a,b,c,d,e,f,g)
#endif

#define SW3A_EXPECTED_STE3A_REVISION_MAJOR	0
#define SW3A_EXPECTED_STE3A_REVISION_MINOR	9

#include "nmx.h"
#define SW3A_EXPECTED_NMX_REVISION_MAJOR	0
#define SW3A_EXPECTED_NMX_REVISION_MINOR	4

// Generic
#define SW3A_BRACKETING_VALUES_MAX_NUMBER   3   // 3 static values at OMX level. The OMX type definition does not allow max number negotiation.

#define SW3A_GLACE_GRID_WIDTH           72
#define SW3A_GLACE_GRID_HEIGHT          54
#define SW3A_AF_GRID_WINDOW_MAX_NUMBER  10

#define SW3A_EXPTIME_MAX_VIDEO_DEFAULT  33000  // 30 fps for streaming modes; should be got from the omx camera.
#define SW3A_EXPTIME_STEP_DEFAULT          25  // 25us, as observed on Swordfish in VGA.
#define SW3A_AGAIN_MAX               (256 * 8) //  x8, as supported by the ISP.
#define SW3A_AGAIN_STEP                  1
#define SW3A_FRAMERATE_DEFAULT          30     // Adapted to video mode

// STE3A specifics
#define STE3A_TRACE_BUFFER_SIZE       1024*100
#define STE3A_MAKERNOTE_BUFFER_SIZE   1024*24  // TBV Using the size from NIPS. The actual size to be verified

#define PECOUNT_INCR_CHECK(_count_) if ( ((_count_) + 1) < iPeListMaxSize) { (_count_) = (_count_) + 1; } else { DBC_ASSERT(0); }
#define SW3A_MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define SW3A_MIN3(_a, _b, _c) (SW3A_MIN((_a), SW3A_MIN((_b), (_c))))
#define SW3A_MAX(_a, _b) (((_a) < (_b)) ? (_b) : (_a))
#define SW3A_MAX3(_a, _b) (SW3A_MAX((_a), SW3A_MAX((_b), (_c))))
#define SW3A_AVG(_a, _b) (((_a) + (_b)) / 2)
#define SW3A_AVG3(_a, _b, _c) (((_a) + (_b) + (_c)) / 3)
#define SW3A_OMX_TO_PHYSICAL_LENS_POS(omx_index, omx_range, macro_pos, inf_pos) ((((float)(omx_index)) / (omx_range)) * ((inf_pos) - (macro_pos)) + (macro_pos))
#define SW3A_PHYSICAL_LENS_POS_TO_OMX(log_val, omx_range,macro_pos,inf_pos)(((float(log_val))-(macro_pos))/(inf_pos-macro_pos)*omx_range)
#define SW3A_FRAMERATE_TO_FRAMEPERIOD_US(fps)   ((unsigned long)(1000000.0 / fps))
/* Workaround for [PictorBug #107541] ISP return Error when framerate is set with 1/100 fps precision
 * Round framerate to 1/10 Hz. */
//#define SW3A_FRAMEPERIOD_US_TO_FRAMERATE(p)     (1000000.0/(float)(p - (p%10)))
#define SW3A_FRAMEPERIOD_US_TO_FRAMERATE(p)     (((float)(10000000/p))/10.0)




/** Memory layout for statistics buffers
 *
 * ---------------------------------
 *   Red grid
 *   Green grid
 *   Blue grid
 *   Saturation grid
 * ---------------------------------
 *   StatsBufferStorage_t
 *    - the three histograms
 *    - Glace_Statistics_ts XP70 exchange structure
 *    ..
 *    - FrameParamStatus_ts XP70 exchange structure
 *    ..
 *    - AF statistics XP70 exchange structures
 * ---------------------------------
 */
#define SW3A_HISTO_BIN_NB               256
#define SW3A_ISP_GAMMA_LUT_SAMPLE_COUNT CE1_SHARP_MEM_LUT_GREEN_ELT_DIMENSION // 128
#define STE3A_MAKERNOTE_BUFFER_SIZE   1024*24  // Eventual target is 24kb max for R&D purpose, 4kb in the product..
#define STE3A_EXTRADATA_BUFFER_SIZE   1024*100



typedef struct {
    // AEW
    t_uint8 *pGridR;                // Virtual memory address
    t_uint8 *pGridG;                // Virtual memory address
    t_uint8 *pGridB;                // Virtual memory address
    t_uint8 *pGridS;                // Virtual memory address
    t_uint32 apHistR[SW3A_HISTO_BIN_NB];
    t_uint32 apHistG[SW3A_HISTO_BIN_NB];
    t_uint32 apHistB[SW3A_HISTO_BIN_NB];
    Glace_Statistics_ts glaceStats; // All included addresses are expressed in ISP address space
    t_uint32 ispHistAddr_R;         // Isp memory space address
    t_uint32 ispHistAddr_G;         // Isp memory space address
    t_uint32 ispHistAddr_B;         // Isp memory space address
    t_uint32 ispGlaceStatsAddr;     // Isp memory space address
    // Sensor parameters applied on frame
    FrameParamStatus_ts frameParamStatus;
    FrameParamStatus_Extn_ts frameParamStatusExtn;
    t_uint32 ispSensorParamAddr;    // Isp memory space address
    // Auto-focus
    AFStats_HostZoneConfigPercentage_ts afConfig[SW3A_AF_GRID_WINDOW_MAX_NUMBER];
    t_uint32 ispAfConfigAddr;       // Isp memory space address
    AFStats_HostZoneStatus_ts afStats[SW3A_AF_GRID_WINDOW_MAX_NUMBER];
    t_uint32 afStatsValid;
    t_uint32 afStatsLensPos;
    t_uint32 afFrameId;
    t_uint32 ispAfStatsAddr;        // Isp memory space address
} StatsBuffersStorage_t;

typedef enum {
    SW3A_SET_AEW,
    SW3A_SET_AF,
    SW3A_SET_SENSOR_PARAMETERS,
    SW3A_SET_ROI_COORDINATES_AF,
    SW3A_SET_ROI_COORDINATES_AEW,
    SW3A_SET_USER_SETTINGS,
    SW3A_CALCULATE_ISP_SETTINGS_AEW,
    SW3A_CALCULATE_ISP_SETTINGS_AF,
    SW3A_CALCULATE_HISTOGRAM_SETTINGS,
    SW3A_SET_FLASH_SETTINGS,
    SW3A_RUN_AEC,
    SW3A_RUN_AWB,
    SW3A_RUN_AF,
    SW3A_PING,
    SW3A_CHANGE_TUNING_OPERATING_MODE,
    SW3A_UPDATE_TUNING_CONFIG,
    SW3A_ACTION_MAXINDEX
} sw3A_action_t;

/** Functional operating phase, deciding upon which 3A lib calls should be done upon
 *  statistics notification.
 */
typedef enum {
    SW3A_PHASE_INIT,
    SW3A_PHASE_INITED,
    SW3A_PHASE_VF,
    SW3A_PHASE_PREFLASH,
    SW3A_PHASE_PRESTILL,
    SW3A_PHASE_MAINSTILL,
    SW3A_PHASE_TIME_NUDGE,
    SW3A_PHASE_MAXINDEX
} sw3A_phase_t;


typedef struct {
    t_uint32 expTimeMin;
    t_uint32 expTimeMax;
    t_uint32 expTimeStep;
    t_uint32 analogGainMin;
    t_uint32 analogGainMax;
    t_uint32 analogGainStep;
    t_sint32 readoutTime;
} sw3A_sensorParms_t;

//Flash Modes
typedef enum {
    SW3A_FLASHMODE_OFF,
    SW3A_FLASHMODE_ON,
    SW3A_FLASHMODE_AUTO,
    SW3A_FLASHMODE_REDEYE,
    SW3A_FLASHMODE_FILLIN,
    SW3A_FLASHMODE_TORCH,
    SW3A_FLASHMODE_MAXINDEX
} sw3A_flashmode_t;

//Camera Modes
typedef enum {
    SW3A_CAMERAMODE_STILLVF,
    SW3A_CAMERAMODE_VIDEOVF,
    SW3A_CAMERAMODE_MAINSTILL,
    SW3A_CAMERAMODE_STILL_IN_VIDEO,
    SW3A_CAMERAMODE_HDR_STILL,
    SW3A_CAMERAMODE_VIDEO,
    SW3A_CAMERAMODE_MAXINDEX
} sw3A_cameramode_t;

//Analog Gain Quantization Model
typedef struct {
    sw3A_uint8_t SmiaModel;
    sw3A_float_t SmiaM0;
    sw3A_float_t SmiaC0;
    sw3A_float_t SmiaM1;
    sw3A_float_t SmiaC1;
    sw3A_int16_t SmiaMinCode;
    sw3A_int16_t SmiaMaxCode;
    sw3A_float_t LinearMin;
    sw3A_float_t LinearMax;
    sw3A_float_t LinearBase;
    sw3A_float_t LinearStep;
} sw3A_analogquanmodel_t;

//Exposure Time Quantization Model
typedef struct {
    sw3A_uint32_t ExposureTimeMin;
    sw3A_uint32_t ExposureTimeMax;
    sw3A_uint32_t ExposureTimeBase;
    sw3A_uint32_t ExposureTimeStep;    
} sw3A_exposuretimequanmodel_t;

//Metering States
typedef enum {
    SW3A_METERING_NONE,
    SW3A_METERING_START,
    SW3A_METERING_AEW_FLASH,
    SW3A_METERING_AFC_START,
    SW3A_METERING_AFC_RUNNING,
    SW3A_METERING_AEW_AFTER_AFC,
    SW3A_METERING_AEW_DONE,
    SW3A_METERING_AEW_MAIN,
    SW3A_METERING_AEW_MAIN_DONE,
    SW3A_METERING_MAXINDEX
} sw3A_metering_t;

/** Statistics container management class */
class CSw3AStats
{
    public:
        CSw3AStats();
        ~CSw3AStats();
        sw3A_error_t createStatsBuffer(t_sw3A_ISPSharedBuffer *pSharedBuffer);
        void destroyStatsBuffer();
        //void static setFakeGrid(CNips3AAPI::TGrid* pGrid);
        //void static setFakeHisto(CNips3AAPI::THist* pHist);
        void cacheInvalidate(bool bAfData);
        void cacheClean();
        void setAewArea(t_uint32 addr, t_uint32 size);
        void setAfArea(t_uint32 addr, t_uint32 size);
        sw3A_error_t saveStatsBuffer();
        StatsBuffersStorage_t *pStatsBufferStorage; // Reference to the statistics buffer shared with ISP and allocated by hw_buffer.
	bool bAewInitialized;
	bool bAfInitialized;

    private:
        sw3A_uint32_t   frameCount;
        //MMHwBuffer     *statsBufferPoolId; //Fix for Bug 862 in coverity run
        MMHwBuffer     *pStatsHwBuffer;
        t_uint32        aewAreaAddr;
        t_uint32        aewAreaSize;
        t_uint32        afAreaAddr;
        t_uint32        afAreaSize;
        //CSw3ATraceFile *iHistoFile;
        //CSw3ATraceFile *iGridFile;
};

/** Core 3A library context class */
class CSw3AlibCtxt
{
    private:
        void setTodo(sw3A_phase_t phase, bool aActions[SW3A_ACTION_MAXINDEX]);

    public:
        CSw3AlibCtxt();
        ~CSw3AlibCtxt();
        void refreshSettings();
        void convertGammaLutFromNipsToISP(sw3A_uint32_t *aIspLut, sw3A_uint8_t *aNipsLut);
        bool isTodo(sw3A_action_t action);
        void setForcedTodoAction(sw3A_action_t action);
        void clearForcedTodoAction(sw3A_action_t action);
        void clearForcedTodo();
        void setSkippedTodoAction(sw3A_action_t action);
        void clearSkippedTodoAction(sw3A_action_t action);
        void clearSkippedTodo();
        void updateOMXFocusStatus(OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE * pExtFocusStatus, OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE *pFocusControl, OMX_SYMBIAN_CONFIG_ROITYPE *pROI);

        CSw3AStats iStatistics;
        sw3A_phase_t curPhase;
        sw3A_phase_t pendingPhase;
        sw3A_phase_t lastPhase;

        bool todoLUT[SW3A_ACTION_MAXINDEX][SW3A_PHASE_MAXINDEX];
        bool forcedActionLUT[SW3A_ACTION_MAXINDEX];
        bool skippedActionLUT[SW3A_ACTION_MAXINDEX];

        // STE3A Handle and Callbacks    
        ste3a_handle                    ste3a_instance;
        ste3a_callbacks                 callbacks;

        // Capture mode settings from user side
        t_uint32                      aUserSettingsExposureTime;
        t_uint16                      aUserSettingsAnalogGain;
        t_uint16                      aUserSettingsEVShift;        
        t_uint32                      aUserSettingsISOValue;
        bool                          aUserSettingsNDFilter;
       
        // AF user controls: Should be a user setting
        t_sint32 aAfRangeFarPosLimit;
        t_sint32 aAfRangeFarPosDefault;
        t_sint32 aAfRangeNearPosDefault;
        t_sint32 aAfRangeNearPosLimit;

        // NVM range backup values, used to recover from a manual focus
        t_sint32 aAfRangeFarPosLimitBackup;
        t_sint32 aAfRangeFarPosDefaultBackup;
        t_sint32 aAfRangeNearPosDefaultBackup;
        t_sint32 aAfRangeNearPosLimitBackup;

        t_uint16 aMotionMeasure;

        //For exposure calibration data
        //t_uint32 aAecMin_ExposureTime;
        //t_uint32 aAecMax_ExposureTime;
        //t_uint32 aAecBase_ExposureTime;
        //t_uint32 aAecStep_ExposureTime;        

        // Settings calculated by STE3A: statistics blocks configuration
        // Grid & Histogram Statistics
        ste3a_stats_descriptor          stats_descriptor;
        ste3a_histograms_stats_status   stats_hist;
        ste3a_grid_stats_status         stats_grid;
        ste3a_histograms_stats_geometry stats_hist_geometry;
        ste3a_grid_stats_geometry       stats_grid_geometry;

        // AF Statistics 
        ste3a_focus_stats_descriptor    stats_focus_descriptor;
        ste3a_focus_stats_status        stats_focus;
        ste3a_focus_stats_zone_geometry stats_focus_zone_geometry;
        ste3a_focus_stats_geometry      stats_focus_geometry;

        // STE3A Tuning configuration
        ste3a_aec_tuning                aec_tuning;
        ste3a_awb_tuning                awb_tuning;
        ste3a_afc_tuning                afc_tuning;
        ste3a_art_tuning                art_tuning;

        ste3a_bool                      aec_stable;
        ste3a_bool                      awb_stable;
        ste3a_bool                		afc_stable;
        ste3a_bool                		afc_running_in_single_shot;
        // Tone mapping structure coming from ART
        ste3a_isp_tm_config				isp_tm_config; 
        
        // ISP gamma coding descriptor
        ste3a_isp_gc_config             isp_gc_config;

        // ISP specfice structres
		ste3a_ppal_8500V2_cg_config	    x8500V2_cg_config;
		ste3a_ppal_8500V2_cm_config	    x8500V2_cm_config;

		// For Gamma Correction
		ste3a_ppal_8500V2_gc_config     x8500V2_gc_config;
        
        //For internal flash state
        sw3A_flashmode_t                aFlashMode;

        //For internal camera mode state
        sw3A_cameramode_t               aCameraMode;

        // Settings calculated by NIPS3A: control
        t_sw3A_FlashDirective           flashDirectives; // duplicate in an other format a subset of iAECSettings
        t_sw3A_FlashDirective           syncFlashDirectives; // duplicate in an other format a subset of iAECSettings
        
        // Mirroring of StatsBuffersStorage_t contents for NIPS3A parameter passing.
        
        // Gamma LUT in ISP format. Reference to be passed to hsmcamera for writing to ISP memory.
        sw3A_uint32_t aGammaLUT[SW3A_ISP_GAMMA_LUT_SAMPLE_COUNT];
        
        //ste3a_afc_actuator_type         afc_actuator_type;
        ste3a_afc_mode                  aAFMode;
        ste3a_zoom_factor               dzoom;
        ste3a_cammod_focus_status       focus_status;
        ste3a_cammod_focus_config       focus_config;
        ste3a_afc_state                 focus_state;
	    ste3a_afc_state                 previous_focus_state;	
        bool                            bFocusManual;
        float                           fManualFocusDistance;
        OMX_SYMBIAN_LOCKTYPE            focus_LockType;
        bool                            bFocusControlTurnedOn; //For Region Selection

	    unsigned int                    ino_of_rois;

        ste3a_isp_cg_config             isp_cg_config; // controls to ISP: RGB gains
        ste3a_isp_cm_config             isp_cm_config; // controls to ISP: color matrix
		ste3a_awb_scene_info            awb_scene_info; // scene informations from the AWB algorithm
        //ste3a_awb_intermediates         awb_intermediates;
        ste3a_scene_status              awb_scene;

        ste3a_cammod_exposure_config    cam_config; // controls to sensor: AG, ET, IG, NG
        ste3a_isp_dg_config             isp_dg_config; // controls to ISP: DG
		ste3a_isp_er_config				isp_er_config; 
	    ste3a_isp_cs_config				isp_cs_config;
		ste3a_damper_bases				damper_bases;
	    ste3a_aec_scene_info			aec_scene_info;

	    ste3a_bool                      aec_flash;

        ste3a_acquisition_mode          acquisition_mode;
  	    //ste3a_aec_intermediates         aec_intermediates;
        ste3a_afc_scene_info            afc_scene_info;
        ste3a_flash_exposure_config     flashExpconfig;

        bool        bNeedGammaLUTUpdate;
        bool        bAutoAperture;
        bool        bAutoShutterSpeed;
        bool        bforceIspSettingsAF;
        t_uint16    aEVShift;
        t_uint32    q16manualApertureFNumber;
        t_uint16    numPreflashFired;
		bool		b_focusCallbackNotify; //To send AF lock after receiving valid stats
        bool        b_shakeDetection;
        bool        b_extFocusStatus;

        // Trace buffers for debug and extradata
        //TNUint8     a_extradataBuf[NIPS3A_EXTRADATA_BUFFER_SIZE];
        sw3A_uint8_t  a_traceBuf[STE3A_TRACE_BUFFER_SIZE];
        sw3A_uint32_t iExtraDebugSize;
        //TNUint8     a_makerNoteBuf[NIPS3A_MAKERNOTE_BUFFER_SIZE];

        // Trace files for debug and extradata
        CSw3ATraceFile *iExtradataFile;
        CSw3ATraceFile *iTraceFile;
        CSw3ATraceFile *iMakernotesFile;
		//For exposure region
		ste3a_aec_dynamic_weighting_geometry dynamic_weighting_geometry;

        //For Time debug traces
        OMX_U64 u64_time_previous;

        //For shake detection
        OMX_SYMBIAN_PRECAPTUREEXPOSURETIMETYPE iPreCapExpTime;
        OMX_SYMBIAN_PRECAPTUREEXPOSURETIMETYPE iCurCapExpTime;
        sw3A_uint8_t iShortTimeThreshold;
        sw3A_uint8_t iLongTimeThreshold;

        //For WhitrBalance off mode
        bool                    b_whiteBalanceOff;
        ste3a_isp_cg_config	    wboff_cg_config;
		ste3a_isp_cm_config	    wboff_cm_config;

        //For Flash Control
        sw3A_uint8_t            iFlashPowerModeSelection;
        sw3A_uint16_t           iFlashMaxPower;
        sw3A_uint16_t           iFlashMaxPreflashes;

        //For Auto Senstivity, for exif fix.
        OMX_U32                 iSensitivity;

        //To support Analog gain quantization model
        sw3A_analogquanmodel_t agQuanModel;

        //To support Exposure time quantization model
        sw3A_exposuretimequanmodel_t etQuanModel;

	    //To support MakerNotes
	    t_uint8  pMakerNoteBufData[STE3A_MAKERNOTE_BUFFER_SIZE];
	    t_uint32 makerNoteSize;		

        ste3a_timestamp         iSte3aTimestamp;
        //To support metering in half press mode
        bool                    meteringOn;
        sw3A_metering_t         iMeteringState;
        bool                    meteringFlashNeeded;
        bool                    meteringFlashFire;
        t_uint32                meteringCounter;

        //To support Flash LED current level and avoid calling Flash LED in every iteration
        t_uint16                torchCurrentFlashLED;
        t_uint16                flashCurrentFlashLED;
        t_uint16                afCurrentFlashLED;

};

#endif /* __SW3A_INTERNAL_HPP__ */
