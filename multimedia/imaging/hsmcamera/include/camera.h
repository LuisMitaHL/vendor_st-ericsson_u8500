/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <sys/time.h>

#include "tuning.h"
#include "tuning_data_base.h"
#include "tuning_loader_manager.h"

#include "ENS_DBC.h"
#include "OMX_Index.h"
#include "IFM_Types.h"

#include "IFM_HostNmfProcessingComp.h"

#include "resource_sharer_manager.h"
#include "ispctl_component_manager.h"

#include "sw3A_component_manager.h"

#include "sia_platform_manager.h"
#include "grabctl.h"
#include "grab.h"
#include "zoom.h"
#include "trace.h"

#include "img_network.hpp"
#include "grabctl/api/acknowledges.hpp"
#include "grabctl/api/error.hpp"
#include "grabctl/api/configure.hpp"
#include "grab/api/alert.hpp"
#include "timer/timerWrapped.hpp"
#include "asyncflash_wrp.hpp"

#include "deferredeventmgr.h"
#include "opmodemgr.h"

#include "hostnmf_cb.h"
#include "hsmcam.h"

#include "primary_sensor.h"
#include "secondary_sensor.h"

#include "IFM_Index.h"
#include "IFM_Types.h"
#include "IFM_Trace.h"

#include "picture_settings.h"
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
#include "3A_ExtAPI.h"
#endif // CAMERA_ENABLE_OMX_STATS_PORT
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
#include "isp_dampers_ext.h"
#include "sw3A_component_manager_extension.h"
#include "framerate_ext.h"
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 8
#define PERF_TRACE_PROP "ste.hsmcamera.stillnvideo.perf"
#define DIPLAY_720_PROP "ste.hsmcamera.display.720p"

#define PERF_TRACE_LAT(msg,lastTime) \
{ \
GET_PROPERTY(PERF_TRACE_PROP, value, 0); \
int mDBGT =0; \
mDBGT=strtoul(value,NULL,16); \
if(mDBGT&0x1) \
{ \
struct timeval tempo; \
gettimeofday(&tempo, NULL); \
LOGD("PERF: "msg": TimeTaken: %ld:%ld \n", tempo.tv_sec,tempo.tv_usec); \
if(mDBGT & 0x2) \
{ \
LOGD("PERF: "msg": TimeTaken: %ld:%ld \n", tempo.tv_sec-lastTime.tv_sec,tempo.tv_usec-lastTime.tv_usec); \
} \
lastTime=tempo ;\
} \
} 


/*Resolution > XGA with STAB related changes*/
#define XGA_WIDTH 1024
#define XGA_HEIGHT 768
#define CAM_DISPLAY_RES_720p "persist.hsmcamera.display.720"
/*START of #defines for Damper configurations dependent on specific OMX indexes*/
#define OMX_NUM_INDEXES_FOR_DAMPER_CONFIG 8 //number of indexes which are used for Damper configuration

//index definitions for OMX indexes used for Damper configuration
#define OMX_DAMPER_CONFIG_INDEX_BASE 		0
#define OMX_DAMPER_CONFIG_WHITE_BAL			(OMX_DAMPER_CONFIG_INDEX_BASE + 0)
#define OMX_DAMPER_CONFIG_EXPOSURE_VAL 		(OMX_DAMPER_CONFIG_INDEX_BASE + 1)
#define OMX_DAMPER_CONFIG_EXPOSURE 			(OMX_DAMPER_CONFIG_INDEX_BASE + 2)
#define OMX_DAMPER_CONFIG_IMAGE_FILTER 		(OMX_DAMPER_CONFIG_INDEX_BASE + 3)
#define OMX_DAMPER_CONFIG_FOCUS_REGION 		(OMX_DAMPER_CONFIG_INDEX_BASE + 4)
#define OMX_DAMPER_CONFIG_FOCUS_RANGE 		(OMX_DAMPER_CONFIG_INDEX_BASE + 5)
#define OMX_DAMPER_CONFIG_SCENE_MODE 		(OMX_DAMPER_CONFIG_INDEX_BASE + 6)
#define OMX_DAMPER_CONFIG_FLICKER_REMOVAL 	(OMX_DAMPER_CONFIG_INDEX_BASE + 7)

typedef struct
{
	t_bool bIndexToBeConfigured;
	OMX_INDEXTYPE nIndex;
} camDamperConfig_t;
/*END of #defines for Damper configurations dependent on specific OMX indexes*/

typedef struct GrabErrorDetail {
    t_uint32 error_id;
    t_uint32 data;
} GrabErrorDetail;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Camera);
#endif
class Camera : public IFM_HostNmfProcessingComp,
	public grabctl_api_acknowledgesDescriptor,
	public grabctl_api_errorDescriptor,
	public grab_api_alertDescriptor,
	public timer_api_alarmDescriptor,
	public asyncflash_api_responseDescriptor
{

public:

  Camera(ENS_Component &enscomp, t_uint8 manufacturer, t_uint16 model, t_uint8 revisionNumber, enumCameraSlot cam=ePrimaryCamera);
	virtual ~Camera();
	virtual OMX_ERRORTYPE applyConfig(
		OMX_INDEXTYPE nIndex,
		OMX_PTR pStructure
		);

	virtual OMX_ERRORTYPE retrieveConfig(
		OMX_INDEXTYPE nConfigIndex,
		OMX_PTR pComponentConfigStructure
		);

	virtual OMX_ERRORTYPE instantiate();
	virtual OMX_ERRORTYPE start() ;
	virtual OMX_ERRORTYPE stop() ;

	virtual OMX_ERRORTYPE configure();

	virtual OMX_ERRORTYPE deInstantiate() ;

    virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE cmd, OMX_U32 nParam, OMX_BOOL &bDeferredCmd) ;

    virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &bDeferredEventHandler) ;

	OMX_ERRORTYPE setOperatingMode(t_operating_mode_camera aOpMode);
	void setExecutingStateStartTime();
	void setPipes(e_grabPipeID pipe4port0, e_grabPipeID pipe4port1, e_grabPipeID pipe4port2);
	OMX_TICKS getTimeElapsedSinceExecutingStateStart();

	CAM_SM* p_cam_sm;
	CopModeMgr iOpModeMgr;
	CTestMode iTestMode;
	CSensor* iSensor;
	OMX_BOOL iPanic;

    /* ARM-NMF fillBufferDone callback */
    virtual void fillBufferDone(OMX_BUFFERHEADERTYPE * pBuffer);
    virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

    OMX_ERRORTYPE suspendResources(void);
    OMX_ERRORTYPE unsuspendResources(void);
    void errorRecovery(void);

    Itimer_api_timer mTimer;
    Iasyncflash_api_request mAsyncflashRequest;

    /* high level latencies variables */
    t_uint64 latency_ShotToShotStart;
    t_uint64 latency_ShotToSnapshotStart;
    t_uint64 latency_TotalShutterLagStart;
    t_uint64 latency_VFOffToOnStart;
    t_uint64 latency_AFStart;
    t_uint64 latency_ShotToVF;
    OMX_STE_CONFIG_LENSPARAMETERTYPE mLensParameters;

    OMX_ERRORTYPE getSupportedResolutions(OMX_SYMBIAN_CONFIG_SUPPORTEDRESOLUTIONS *pRes);

    //for performance traces
    IFM_LATENCY_HSMCAMERA mlatency_Hsmcamera;	/*latency values for hsmcamera in this structure*/
    //for performance traces

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
    OMX_BOOL iSkipNVM;
#endif

    OMX_ERRORTYPE addIndexToDamperConfigList(OMX_INDEXTYPE nIndex);
    camDamperConfig_t isDamperConfigReqd[OMX_NUM_INDEXES_FOR_DAMPER_CONFIG];

    /** Dump OMXCamera internal state.
     *  @param apCtx pointer to OMX_OTHER_PARAM_PRIVATE_CONTEXT which provides
               OS independent logging function along with other info.
     */
    OMX_ERRORTYPE printPrivateContext(OMX_OTHER_PARAM_PRIVATE_CONTEXT *apCtx);

protected :
    void fillBufferDoneVPB1(OMX_BUFFERHEADERTYPE *pOMXBuffer);
    void fillBufferDoneVPB0(OMX_BUFFERHEADERTYPE *pOMXBuffer);
    void fillBufferDoneVPB2(OMX_BUFFERHEADERTYPE *pOMXBuffer);
#ifdef CAMERA_ENABLE_OMX_STATS_PORT
    void fillBufferDoneStatsAewb(OMX_BUFFERHEADERTYPE *pOMXBuffer);
    void fillBufferDoneStatsAf(OMX_BUFFERHEADERTYPE *pOMXBuffer);
    OMX3A_ExtInterface_t* pExtItf;
#endif // CAMERA_ENABLE_OMX_STATS_PORT

    /* grabctl_api_acknowledges callbacks */
    virtual void configured(t_uint16 port_idx);
    virtual void bufferGrabbed(t_uint16 port_idx);
    virtual void clockrunning(t_uint16 port_idx);
    virtual void clockwaitingforstart(t_uint16 port_idx);
    virtual void grababorted(t_uint16 port_idx);
    /*grabctl_api_error callbacks */
    virtual void error(enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id) ;
    /* grab_api_alert callbacks */
    virtual void info(enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp);
    virtual void error(enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp);
    virtual void debug(enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
    /* timer_api_alarm callback */
    virtual void signal(void);
    /* asyncflash_api_response callback */
    void asyncFlashSelfTestResult(t_sint32 res);
    void asyncFlashReturnCode(t_sint32 ret);

    img_network * mpImgNetwork;
    timerWrapped* pTimer;
    asyncflash_wrp* pAsyncflash;
    Igrabctl_api_configure mIgrabctlConfigure;
    Istab_api_configure mIstabConfigure;

private :

	/* may be put in a specific object ?*/
	void queueRateTuningPE(void);

	void constructSM(ENS_Component &enscomp);
	void destroySM();

	/* check alive tracing enabling / disabling */
	void checkAliveStartStop();
	void printCheckAliveStatus();
	unsigned int iCheckIntervalMsec;
	bool iPrintStatus;
	bool iCheckAliveTimerRunning;
	unsigned int iNbAliveIterval500Msec;

public:
	t_uint32 mDisplay720p;
	unsigned int iNbLowPowerInterval500Msec;
	bool iCheckTimerRunning;
	int captureRequest;
	OMX_ERRORTYPE configureDamper(OMX_INDEXTYPE nIndex);
       OMX_TICKS iExifTimeStamp;
private:
	/* Delegates */
	CCamSharedMemory iSharedMemory;
	camSharedMemChunk_t iSensorModesShmChunk;
	camSharedMemChunk_t iNvmShmChunk;
	camSharedMemChunk_t iFwShmChunk;

	camSharedMemChunk_t iGammaSharpGreen_LR;
	camSharedMemChunk_t iGammaSharpRed_LR;
	camSharedMemChunk_t iGammaSharpBlue_LR;
	camSharedMemChunk_t iGammaUnSharpGreen_LR;
	camSharedMemChunk_t iGammaUnSharpRed_LR;
	camSharedMemChunk_t iGammaUnSharpBlue_LR;

	camSharedMemChunk_t iGammaSharpGreen_HR;
	camSharedMemChunk_t iGammaSharpRed_HR;
	camSharedMemChunk_t iGammaSharpBlue_HR;
	camSharedMemChunk_t iGammaUnSharpGreen_HR;
	camSharedMemChunk_t iGammaUnSharpRed_HR;
	camSharedMemChunk_t iGammaUnSharpBlue_HR;
	COmxStateMgr iOmxStateMgr;

	CSiaPlatformManager iSiaPlatformManager;
	CIspctlComponentManager iIspctlComponentManager;
    CFlashDriver * iFlashDriver; 
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
	Sw3AProcessingCompExt i3AComponentManager;
public:
	CFramerateExt iFramerate;
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
	Sw3AProcessingComp i3AComponentManager;
public:
	CFramerate iFramerate;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
private:
	CIspctlCom iIspctlCom;
	CGrabControl iGrabControl;
	CGrab iGrab;
	CCapture_context iCapture_context;
	CZoom iZoom;
	CTrace iTrace;
	CResourceSharerManager iResourceSharerManager;
	CDefferedEventMgr iDeferredEventMgr;
	CHostNmf_CB iCallback;

	/* Tuning */
	CTuningDataBase* pTuningDataBase;
	CTuningLoaderManager iTuningLoaderManager;
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
	CIspDampersExt iIspDampers;
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
	CIspDampers iIspDampers;
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
	CPictureSettings iPictureSettings;

	/* Configuration at COM_SM construct level */
	/* Passed to the OpModeMgr instance */
	t_uint16 CapturePort;
	t_uint16 RecordPort;
	t_uint16 RawCapturePort;
	CSelfTest iSelfTest;
	OMX_TICKS iExecutingStateStartTime;
	/*For Shutterlag*/	
	t_sw3a_stop_req_status Sw3aStopReqStatus;
	t_grab_abort_status GrabAbortStatus;
    GrabErrorDetail grabErrorDetail[CAMERA_NB_OUT_PORTS];
    e_grabPipeID grabPipeIDConfig[CAMERA_NB_OUT_PORTS];
};

#endif
