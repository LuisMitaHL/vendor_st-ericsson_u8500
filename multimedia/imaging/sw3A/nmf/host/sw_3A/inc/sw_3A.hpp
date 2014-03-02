/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SW_3A_HPP__
#define __SW_3A_HPP__

// Programming interface
#include "IFM_Index.h"
#include "IFM_Types.h"
#include "OMX_CoreExt.h"
#include "OMX_IndexExt.h"
#include "OMX_Symbian_CameraExt_Ste.h"

// Internal definitions
#include "tuning.h"             // For tuning PE list storage
#include <tuning_data_base.h>
#include "tuning_params.h"
#include "damper_base.h"
#include "ImgConfig.h"
#include "ste3a_ppal_types.h"
#include "ste3a_awb_types.h"
#include "ste3a_cmal_types.h"
#include "ppal.h"
#include "OMX_Component.h"


typedef void * CSw3AlibCtxt_p;   // Handle to the private 3A lib context

// References for opaque members
class CSw3AlibCtxt;
class CpeList;

//  ARM NMF component class

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(sw_3A);
#endif

/** 3A library state, for detecting stateflow infringements) */
typedef enum {
        SW3A_LIBSTATE_PREINIT,
        SW3A_LIBSTATE_CLOSED,   /**< sw3A wrapper resources allocated */
        SW3A_LIBSTATE_OPEN,     /**< sw3A core library open, ready for system interaction */
        SW3A_LIBSTATE_RUNNING   /**< sw3A component alive: continuous flow of statistics in and control out */
} sw3A_libState_t;

class sw_3A_ClbkReq
{
    public:
        sw_3A_ClbkReq();
        void set(bool bValue);
        void setMsg(t_sw3A_Msg msg, t_uint32 data,  Isw_3A_api_done *pDone);
        void notify();   // Auto-clears if the notification is done.
    private:
        bool ibLock;
        t_sw3A_Msg iMsg;
        t_uint32 iData; // Storage for an OMX index
        Isw_3A_api_done *ipDone;
};

class C3AExposure
{
    public:
        C3AExposure();

        /* From user*/
        OMX_CONFIG_EXPOSUREVALUETYPE value;
        OMX_EXPOSURECONTROLTYPE control;
};

class C3AWhiteBalance
{
    public:
        C3AWhiteBalance();

        /* From user*/
        OMX_CONFIG_WHITEBALCONTROLTYPE value;
        OMX_WHITEBALCONTROLTYPE control;
};

class C3AAutoFocus
{
    public:
        C3AAutoFocus();

        /* From user*/
        OMX_CONFIG_FOCUSREGIONTYPE value;
        OMX_FOCUSSTATUSTYPE control;
};

#define IQ_MODE_NUM 20
#define ROI_REF_WIDTH 100
#define ROI_REF_HEIGHT 100
#define SCALE_STE3A_RANGE 0.01f
#define SCALE_FW_RANGE 100

#define SFXSolarisControl_0_Addr 0x3300
#define SFXSolarisControl_1_Addr 0x3340
#define SFXNegativeControl_0_Addr 0x3301
#define SFXNegativeControl_1_Addr 0x3341

#define Adsoc_RP_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Addr 0x3046
#define Adsoc_RP_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Addr 0x3086
#define Adsoc_RP_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Addr 0x3047
#define Adsoc_RP_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Addr 0x3087
#define Adsoc_RP_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Addr 0x3048
#define Adsoc_RP_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Addr 0x3088

class sw_3A: public sw_3ATemplate
{
    public:
        sw_3A();
        virtual void open(t_uint32 peListMaxSize, t_sw3A_ISPSharedBuffer sharedBuf, t_sw3A_FlashModeSet flashModesSet);
        virtual void close();
        virtual void startLoop(t_sint32 bNotifyCompletion, t_sw3A_PageElementList *pFwState);
        virtual void stopLoop(t_sint32 bNotifyCompletion);
        virtual void setMode(t_sw3A_LoopState loopState, t_uint32 numberOfStats);
		virtual void setFrameRate(t_uint32 framerate_x100, t_uint32 framerate_min_x100, t_sint32 bFixedFramerate);	
        virtual void setConfig(t_uint32 pIndex, void * pp);
        virtual void process(t_sw3A_StatEvent statEvent);
	
		virtual void stop(void);
		virtual void start(void);
		
        virtual t_sint32 isConfigAvailable(t_uint32 pIndex, void* pp);
		virtual t_sint32 getConfig(t_uint32 nIndex, void* pp);
		virtual t_sint32 getMakenotesAllocSize(void);
        virtual t_sint32 getExtradataAllocSize(void);
        virtual t_sint16 getTorchPower(void);
		virtual t_sw3A_FlashModeSet translateFlashModes(t_sw3A_FlashModeSet flashModes); 	

    private:
        void processAew(t_sw3A_StatEvent statEvent);
        void processAf(t_sw3A_StatEvent statEvent);
        CpeList *iPeList;
        //CTuningDataBase* pTuningDataBase;
        sw3A_libState_t libState;
        t_sw3A_PageElementList iFwStatus;
        t_sw3A_StillSynchro mStillSynchro;
        t_uint8 checkAllLockNotify();

        // OMX structures that are updated according to 3A decisions. To be copied as getConfig() output.
        typedef struct  
            {
            OMX_CONFIG_EXPOSURECONTROLTYPE exposureControl;
            OMX_CONFIG_EXPOSUREVALUETYPE exposureValue;
            OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE NDFilterControl;
            OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE flickerRemoval;
            OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE *pExtFocusStatus;
            OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE focusRegion;
            OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE focusRange;
            OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE focusControl;
            OMX_SYMBIAN_CONFIG_ROITYPE roi;
            OMX_SYMBIAN_CONFIG_RGBHISTOGRAM rgbHistogram;  // NB: having a copy here will not be required when flip/flop buffers are used for statistics export.
            OMX_SYMBIAN_CONFIG_LOCKTYPE exposureLock;
            OMX_SYMBIAN_CONFIG_LOCKTYPE whiteBalanceLock;
            OMX_SYMBIAN_CONFIG_LOCKTYPE focusLock;
            OMX_SYMBIAN_CONFIG_LOCKTYPE allLock;
            OMX_CONFIG_BRIGHTNESSTYPE   brightness;
            OMX_CONFIG_CONTRASTTYPE     contrast;
            OMX_CONFIG_SATURATIONTYPE   saturation;
            OMX_SYMBIAN_CONFIG_S32TYPE  sharpness;
			OMX_SYMBIAN_CONFIG_BRACKETINGTYPE *pBracketing;
            OMX_CONFIG_BOOLEANTYPE      meteringControl;
            } sw_3A_OMXConfig;

        sw_3A_OMXConfig iOMXConfig;
        bool b_SyncModeSet;
        bool b_startPending;
        bool b_stopPending;
        bool b_stopAfterAEW;            // Only compute AEW settings from current statistics. Do not apply new settings and automatically exit RUNNING state.
        bool b_aewLoopStopped;          // Transition status record when stopping: AEW loop
        bool b_afLoopStopped;           // Transition status record when stopping: AF loop
        bool b_startNotificationPending;
        bool b_stopNotificationPending;
        bool b_exposureLock;
        bool b_whiteBalanceLock;
        bool b_focusLock;
        bool b_allLock;
        bool b_exposureLockAtCapture;
        bool b_whiteBalanceLockAtCapture;
        bool b_focusLockAtCapture;
        bool b_allLockAtCapture;
        bool b_fixedFramerate;
        bool b_FlashNeeeded;
		float logicalDistance_persistent;

/*preflash torch*/
//#if IMG_CONFIG_PREFLASH_USING_TORCHMODE
	 bool b_discardPartialLitFrameStats;
//#endif

        float f_maxFramerateValue;
        float f_minFramerateValue;

        sw_3A_ClbkReq iExposureLockClbkReq;
        sw_3A_ClbkReq iWhiteBalanceLockClbkReq;
        sw_3A_ClbkReq iFocusLockClbkReq;
        sw_3A_ClbkReq iAllLockClbkReq;
        sw_3A_ClbkReq iExtFocusStatusClbkReq;
        sw_3A_ClbkReq iPreCaptureExposureTimeClbkReq;
        sw_3A_ClbkReq iMeteringPreFlashClbkReq;

        CTuning *iTuning;
	    CTuningDataBase* pTuningDataBase;
        CTuningBinData*   pSw3aCharactData;
        CTuningBinData*   pSw3aCalibData;
	    e_iqset_id iqset;
        //t_xp70_pe *a_tuningPeList;     // Used for getting IqSets from the tuning class

        C3AExposure iExposure;
        C3AWhiteBalance iWhiteBalance;
	    C3AAutoFocus iFocus;
        OMX_WHITEBALCONTROLTYPE WhiteBalanceControlType;


        t_uint32 pIndexConfig[IQ_MODE_NUM];
	    t_uint32 omx_config_valid[IQ_MODE_NUM];
	    t_uint32 *omx_config_ptr[IQ_MODE_NUM];
		t_uint32 omx_config_valid_opMode[IQ_MODE_NUM]; // Used to absorb opmode specific IQSet's. 
	 	
        

	/*local structures for glue logic*/
	 OMX_CONFIG_WHITEBALCONTROLTYPE		*whitebalConfig;
	 OMX_SYMBIAN_CONFIG_SCENEMODETYPE		*scenemodeConfig;
	 OMX_SYMBIAN_CONFIG_FLICKERREMOVALTYPE *flickerConfig;
	 OMX_CONFIG_EXPOSURECONTROLTYPE		 *exposureControl;
	 OMX_CONFIG_EXPOSUREVALUETYPE			*exposurevalueConfig;
	 OMX_CONFIG_BRIGHTNESSTYPE				*brightnessConfig;
	 OMX_CONFIG_SATURATIONTYPE				*saturationConfig;	 
	 OMX_CONFIG_CONTRASTTYPE				*contrastConfig;
 	 OMX_SYMBIAN_CONFIG_FOCUSRANGETYPE       *focusrangeConfig;
	 OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE			*focuscontrolConfig;
	 OMX_SYMBIAN_CONFIG_FOCUSREGIONTYPE		*focusregionConfig;
	 OMX_CONFIG_IMAGEFILTERTYPE				*imagefilterConfig;
     OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE    *flashcontrolConfig;   
	 /*added for sharpness */
	  OMX_SYMBIAN_CONFIG_S32TYPE *sharpnessConfig;
	 /*added for AFassist */
	 OMX_SYMBIAN_CONFIG_HIGHLEVELCONTROLTYPE *afAssistConfig;
	 
	 typedef struct  { 
		t_uint32 flashChromaPointX; 
   		t_uint32 flashChromaPointY;
		t_sint16 flashPower;		
	 	}sw_3A_flashlib_values ; //added for chroma values from flash library
		sw_3A_flashlib_values	flashlibValues;
        // 3A lib dependant data
        CSw3AlibCtxt *iSw3AlibCtxt;
	IFM_CONFIG_EXPOSURE_REGION *exposureRegionConfig;

        // Status of FW coins required for synchronizing without polling
        int glaceParamCount;
        int systemSetupCoinCount;
        int hdrControlCoin;
       // int systemSetupCoinGlaceHistoCtrlCount;
        int afCoinCount;
        int afZoneConfigCount;
        int focusControlCoinCount;
	    int flashStatusCoinCount;
        bool bLensActuator;

        // Control the dump to file of 3A development traces
        bool b_dbg_stats_dump;

        // Temporary storages required for NMF calls.

        t_sw3A_Buffer makernotes;
        t_sw3A_metadata metadata;
		
		// Supported Flash Modes
		t_sw3A_FlashModeSet mFlashModesSet ; 
	
	OMX_SYMBIAN_CAPTUREPARAMETERSTYPE  extradataCaptureParameters;
    IFM_DAMPERS_DATA dampersData;
    OMX_STE_PRODUCTIONTESTTYPE sw3AFlashStatusData; 
    OMX_STE_CAPTUREPARAMETERSTYPE steExtraDataCaptureParameters;
	
	// Number of stats that we need to receive before stopping to call process
	// we do not count preflashes
	// 0 => infinite
	t_uint32 numberOfStatsToReceive_pending;
	t_uint32 numberOfStatsToReceive;
	// Number of stats already received
	t_uint32 numberOfStatsReceived;
	OMX_SYMBIAN_CONFIG_ROITYPE	*pFocusROI; //To keep the value for OMX_Symbian_IndexConfigFocusRegion modes, set to NULL after consumed.

	t_tuning_object tuning_state;
    OMX_ERRORTYPE setConfig(t_uint32 pIndex, void * pp, bool b_testOnly);
    OMX_ERRORTYPE applyConfig(t_uint32 pIndex, void*pp);
    void setPeListFromIqSet(CpeList *peList, e_iqset_id IqSetId);
    void traceToFile();
    void setFwStatus(t_sw3A_PageElementList *fwStatus);
    t_uint32 getFwStatus(t_uint16 peAddr);
    void formatCaptureParameters(void);
    void formatDampersData(void);

    OMX_ERRORTYPE UpdateTuningList(const e_iqset_id  aIqSetId);
    OMX_ERRORTYPE Weighting_Glue(OMX_CONFIG_EXPOSUREVALUETYPE *exposurevalueConfig);
	void EV_Glue(OMX_CONFIG_EXPOSUREVALUETYPE *exposurevalueConfig);
	void TimeRange_Glue(OMX_CONFIG_EXPOSUREVALUETYPE *exposurevalueConfig);
	void Priority_Glue(OMX_CONFIG_EXPOSUREVALUETYPE *exposurevalueConfig);
	void Gain_Glue(OMX_CONFIG_EXPOSUREVALUETYPE *exposurevalueConfig);
	void setAWBparameters(ste3a_ppal_8500V2_cg_config  const& cg_config, ste3a_ppal_8500V2_cm_config const& cm_config);
	void setAECparameters(ste3a_cammod_exposure_config const& exposure_config);
    void setFlashParameters();
    t_uint32 getSystemTimeMs();

	t_uint8 aec_stats_counter; //To send AEC convergence event to camera if it taking more time then expected

    float initial_lux;

	int bracketCount;   // Bracketing configuration counter: number of pending values in the bracket.
    t_sw3A_stat_status sw3A_stat_status;
};

#endif //__SW_3A_HPP__
