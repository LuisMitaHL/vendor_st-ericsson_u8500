/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SW3A_COMPONENT_MANAGER_EXTENSION_H_
#define _SW3A_COMPONENT_MANAGER_EXTENSION_H_
#include "sw3A_component_manager.h"
#include "OMX_3A_OtherExt.h"
#include "OMX_3A_CameraExt.h"
#include "Queue.h"
#include "3A_ExtAPI.h"
#include "VhcElementDefs.h"

#define LOCAL_PELIST_MAX_SIZE     100

#define OMX_3A_OTHER_SW3A_HISTO_BIN_NB (256)

typedef struct
{
  /// current Host Programmed Start X with in WOI X size .
  float f_HostAFZoneStartX_PER_wrt_WOIWidth;
  /// current Host Programmed Start Y with in WOI Y size .
  float f_HostAFZoneStartY_PER_wrt_WOIHeight;
  /// Width of the zone selected by the host .
  float f_HostAFZoneEndX_PER_wrt_WOIWidth;
  /// Height of the zone selected by the host .
  float f_HostAFZoneEndY_PER_wrt_WOIHeight;
  /// enable disable host zone
  OMX_BOOL bo_Enabled;
} OMX_3A_OTHER_AFSTATS_HOSTZONECONFIG_PERCENTAGE;

typedef struct {
  // AEW
  OMX_U8* pGridR;                // Virtual memory address
  OMX_U8* pGridG;                // Virtual memory address
  OMX_U8* pGridB;                // Virtual memory address
  OMX_U8* pGridS;                // Virtual memory address
  OMX_U32 apHistR[OMX_3A_OTHER_SW3A_HISTO_BIN_NB];
  OMX_U32 apHistG[OMX_3A_OTHER_SW3A_HISTO_BIN_NB];
  OMX_U32 apHistB[OMX_3A_OTHER_SW3A_HISTO_BIN_NB];
  Glace_Statistics_ts glaceStats; // All included addresses are expressed in ISP address space
  OMX_U32 ispHistAddr_R;         // Isp memory space address
  OMX_U32 ispHistAddr_G;         // Isp memory space address
  OMX_U32 ispHistAddr_B;         // Isp memory space address
  OMX_U32 ispGlaceStatsAddr;     // Isp memory space address
  // Sensor parameters applied on frame
  FrameParamStatus_ts frameParamStatus;
  FrameParamStatus_Extn_ts frameParamStatusExtn;
  OMX_U32 ispSensorParamAddr;    // Isp memory space address
  // Auto-focus
  // 10 windows: hard-coded in NIPS API
  OMX_3A_OTHER_AFSTATS_HOSTZONECONFIG_PERCENTAGE afConfig[OMX_3A_OTHER_STATSAF_WINDOW_NUMBER_MAX];
  OMX_U32 ispAfConfigAddr;       // Isp memory space address
  // 10 windows: hard-coded in NIPS API
  OMX_3A_OTHER_STATSAF_HOSTZONESTATUS afStats[OMX_3A_OTHER_STATSAF_WINDOW_NUMBER_MAX];
  OMX_U32 afStatsValid;
  OMX_U32 afStatsLensPos;
  OMX_U32 afFrameId;
  OMX_U32 ispAfStatsAddr;        // Isp memory space address
} OMX_3A_OTHER_STATSBUFFERSTORAGE;

typedef enum  {
  SWSEMC_Flash_Off,
  SWSEMC_Flash_AFAssist,
  SWSEMC_Flash_Preflash,
  SWSEMC_Flash_Mainflash,
  SWSEMC_Flash_VideoLed,
  SWSEMC_Flash_Indicator,
  SWSEMC_Flash_3ABeforeFlash
} OMX_3A_OTHER_FLASHSATATE;

typedef struct {
  OMX_3A_OTHER_FLASHSATATE flashState;
  OMX_U16 flashPower;
  OMX_U16 flashCtrlPulse;
  OMX_U32 ExposureTimeUs;
} OMX_3A_OTHER_FLASHDIRECTIVE;


typedef enum Sw3AMode {
  Sw3AMode_Preview,
  Sw3AMode_Video,
  Sw3AMode_Snapshot,
} Sw3AMode;

typedef enum Sw3AConfigMode {
  Sw3AConfigMode_Static,
  Sw3AConfigMode_Dynamic,
} Sw3AConfigMode;

typedef enum Sw3AState {
  Sw3AState_Stopped,
  Sw3AState_Stopping,
  Sw3AState_Running,
} Sw3AState;

class Sw3AProcessingCompExt : public Sw3AProcessingComp
{
public:
  Sw3AProcessingCompExt(TraceObject *traceobj);
  virtual OMX_ERRORTYPE instantiate(OMX_HANDLETYPE omxhandle)
  {
    return OMX_ErrorNone;
  }
  virtual OMX_ERRORTYPE deInstantiate(OMX_HANDLETYPE omxhandle)
  {
    return OMX_ErrorNone;
  }

  virtual OMX_ERRORTYPE open(enumCameraSlot camSlot, CFlashDriver *pFlashDriver);

  virtual OMX_ERRORTYPE close();
  virtual OMX_ERRORTYPE start()
  {
    return OMX_ErrorNone;
  }
  virtual OMX_ERRORTYPE stop()
  {
    return OMX_ErrorNone;
  }

  virtual OMX_ERRORTYPE start3A();
  virtual OMX_ERRORTYPE stop3A();

  virtual Sw3AAecConvergenceStatus_t getAecStateConverged()
  {
    return SW3A_AEC_CONVERGENCE_STATUS_CONVERGED;
  };

  virtual OMX_ERRORTYPE setMode(
    t_sw3A_LoopState loopState,
    t_uint32 numberOfStats);

  virtual OMX_ERRORTYPE setConfig(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure);
  virtual OMX_ERRORTYPE getConfig(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure);
  virtual OMX_ERRORTYPE run3A(
    enum e_ispctlInfo info);

  virtual OMX_S32 getMakernotesAllocSize();
  virtual OMX_S32 getExtradataAllocSize();

  // SEMC extensions
  static Sw3AProcessingCompExt* getInstance()
  {
    return pThis;
  };
  void setCallback(void* ud, OMX_ERRORTYPE (*func)(
    void* userData,
    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer))
  {
    StatsDone = func;
    userData  = ud;
  }
  virtual OMX_ERRORTYPE fillThisBufferAewb(
    OMX_BUFFERHEADERTYPE* pBuffer);
  virtual OMX_ERRORTYPE fillThisBufferAf(
    OMX_BUFFERHEADERTYPE* pBuffer);
  virtual void FlushAewbPort();
  virtual void FlushAfPort();
  virtual void SetLensIsReadyFlg(
    bool flg);

  virtual void SetNVMData(
    t_uint8* data);
  virtual bool SetLscConfigtoIsp();
  virtual bool SetLinearizerConfigtoIsp();
  OMX_U32 *pCastPos0GridsData;
  OMX_U32 *pCastPos1GridsData;
  OMX_U32 *pCastPos2GridsData;
  OMX_U32 *pCastPos3GridsData;

  OMX_U32 *pSdlR;
  OMX_U32 *pSdlGr;
  OMX_U32 *pSdlGb;
  OMX_U32 *pSdlB;

  bool bValidLsc;
  bool bValidLinearizer;

  bool bSensorModechanged;
private:
  static Sw3AProcessingCompExt* pThis;

  typedef union
  {
    float    f;
    t_sint32 s32;
  } val32_t;

  inline t_sint32 FloatToI32(const float fval)
  {
    val32_t v;
    v.f = fval;
    return v.s32;
  }

protected:
  // SEMC callbacks
  void * userData;
  OMX_ERRORTYPE (*StatsDone)(
    void* userData,
    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

  // OMX_BUFFERHEADERTYPE queue class
  Queue mAewbQueue;
  Queue mAfQueue;

  Sw3AMode m3Amode;
  Sw3AConfigMode m3Aconfigmode;
  Sw3AState m3Astate;

  virtual void CreateStatsBuffer();
  virtual void GetFwStatus();
  virtual void SetFrameRate();
  virtual bool SetIspparamBlackLevel_Static(
    OMX_3A_ISPPARAM_STATIC_BLACKLEVELCORRECTIONTYPE* pIspParamBlackLevel);  //Static Only
  virtual bool SetIspparamWhiteBalance_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_WHITEBALANCETYPE* pIspParamWhiteBalance);       //Dynamic Only
  virtual bool SetIspparamColorCorrection_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_COLORCORRECTIONTYPE* pIspParamColorCorrection); //Dynamic Only
  virtual bool SetIspparamNoiseReduction_Static(
    OMX_3A_ISPPARAM_STATIC_NOISEREDUCTIONTYPE* pIspParamNoiseReduction);
  virtual bool SetIspparamNoiseReduction_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_NOISEREDUCTIONTYPE* pIspParamNoiseReduction);
  virtual bool SetIspparamLensShading_Static(
    OMX_3A_ISPPARAM_STATIC_LENSSHADINGCORRECTIONTYPE* pIspParamLensShading);
  virtual bool SetIspparamLensShading_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_LENSSHADINGCORRECTIONTYPE* pIspParamLensShading);
  virtual bool SetIspparamColorConversion_Static(
    OMX_3A_ISPPARAM_STATIC_COLORCONVERSIONTYPE* pIspParamColorConversion);
  virtual bool SetIspparamColorConversion_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_COLORCONVERSIONTYPE* pIspParamColorConversion);
  virtual bool SetIspparamEdgeEnhance_Static(
    OMX_3A_ISPPARAM_STATIC_EDGEENHANCEMENTTYPE* pIspParamEdgeEnhance);
  virtual bool SetIspparamEdgeEnhance_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_EDGEENHANCEMENTTYPE* pIspParamEdgeEnhance);
  virtual bool SetIspparamAewb_Static(
    OMX_3A_ISPPARAM_STATIC_AEWBTYPE* pIspParamsAewb);
  virtual bool SetIspparamAf_Static(
    OMX_3A_ISPPARAM_STATIC_AFTYPE* pIspParamsAf);
  virtual bool SetIspparamAf_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_AFTYPE* pIspParamsAf);
  virtual bool SetIspparamIspSpecific(
    OMX_3A_ISPPARAM_ISPSPECIFICTYPE* pIspParam);
  virtual bool SetIspparamIspSpecificBabylon_Static(
    OMX_3A_ISPPARAM_STATIC_BABYLONTYPE* pIspParamBabylon);
  virtual bool SetIspparamIspSpecificBabylon_Dynamic(
    OMX_3A_ISPPARAM_DYNAMIC_BABYLONTYPE* pIspParamBabylon);
  virtual bool SetIspparamIspSpecificBayerRepair_Static(
    OMX_3A_ISPPARAM_STATIC_BAYERREPAIRTYPE* pIspParamBayerRepair);           //Static Only
  virtual bool SetIspparamIspSpecificChannelGain_Static(
    OMX_3A_ISPPARAM_STATIC_CHANNELGAINTYPE* pIspParamChannelGain);           //Static Only
  virtual bool SetIspparamIspSpecificScorpio_Static(
    OMX_3A_ISPPARAM_STATIC_SCORPIOTYPE* pIspParamScorpio);                   //Static Only
  virtual bool SetIspparamIspSpecificSdl_Static(
    OMX_3A_ISPPARAM_STATIC_SDLTYPE* pIspParamSdl);                           //Static Only
  virtual bool SetIspparamIspHistStat_Static(
    OMX_3A_ISPPARAM_STATIC_HISTSTATSTYPE* pIspParam);                        //Static Only

  virtual bool SetIspParamtersConfig(
    OMX_3A_CONFIG_ISPPARAMETERSTYPE* pIspParams,
    t_sw3A_LoopState eState);
  virtual bool SetExposureConfig(
    OMX_3A_CONFIG_EXPOSURETYPE* pExposureParams,
    t_sw3A_LoopState eState);
  virtual bool SetGammaConfig(
    OMX_3A_CONFIG_GAMMATABLE_TYPE* pGammaParams,
    t_sw3A_LoopState eState);
  virtual bool SetGammaConfigtoIsp(
    OMX_3A_CONFIG_GAMMATABLE_TYPE* pGammaParams);
  virtual bool SetLscConfig(
    OMX_3A_CONFIG_LSCTABLE_TYPE* pLscParams,
    t_sw3A_LoopState eState);
  virtual bool SetLinearizerConfig(
    OMX_3A_CONFIG_LINEARIZERTABLE_TYPE* pLinearizerParams,
    t_sw3A_LoopState eState);
  virtual bool SetFocusConfig(
    OMX_3A_CONFIG_FOCUSTYPE* pFocusParams,
    t_sw3A_LoopState eState);
  virtual bool SetFramerateConfig(
    OMX_3A_CONFIG_MAXFRAMERATETYPE* pFrameRate,
    t_sw3A_LoopState eState);
  virtual bool SetFlashConfig(
    OMX_BOOL flashSync,
    t_sw3A_LoopState eState);
  virtual void ClearPageElement(
    t_sw3A_PageElementList* pList);
  virtual void PushPageElement(
    t_sw3A_PageElementList* pList,
    t_uint16 addr,
    t_uint32 data);
  virtual void PushPageElement(
    t_sw3A_PageElementList* pList,
    t_uint16 addr,
    t_sint16 data);
  virtual void PushPageElement(
    t_sw3A_PageElementList* pList,
    t_uint16 addr,
    t_uint16 data);
  virtual void PushPageElement(
    t_sw3A_PageElementList* pList,
    t_uint16 addr,
    t_uint8 data);
  virtual void PushPageElement(
    t_sw3A_PageElementList* pList,
    t_uint16 addr,
    float data);

  virtual void UpdateCoins(t_sw3A_LoopState eState,
                           bool bSystem,
                           bool bAfStats,
                           bool bHist,
                           bool bSensor,
                           bool bIsp);

  // Aewb component
  OMX_3A_CONFIG_ISPPARAMETERSTYPE ispParams;
  OMX_3A_CONFIG_EXPOSURETYPE      exposureParam;
  OMX_3A_CONFIG_GAMMATABLE_TYPE   gammaTable;
  OMX_3A_CONFIG_LSCTABLE_TYPE     lscTable;
  OMX_3A_CONFIG_LINEARIZERTABLE_TYPE     linearizerTable;
  bool bIspParmInit;
  bool bExposureParmInit;
  bool bGammaParmInit;
  bool bLscInit;
  bool bLinearizerInit;
  bool bFramerateInit;
  bool bFramerateUpdate;
  t_sw3A_PageElementList aewbStatsPeList;
  t_sw3A_PageElementList afStatsPeList;

  t_sw3A_PageElementList exposurePeList;
  t_sw3A_PageElementList blackLevelPeList;
  t_sw3A_PageElementList whiteBalancePeList;
  t_sw3A_PageElementList colorMatrixPeList;
  t_sw3A_PageElementList dpcPeList;
  t_sw3A_PageElementList noiserPeList;
  t_sw3A_PageElementList lensSharedPeList;
  t_sw3A_PageElementList gammaPeList;
  t_sw3A_PageElementList colorConvPeList;
  t_sw3A_PageElementList edgePeList;
  t_sw3A_PageElementList defectPixelPeList;
  t_sw3A_PageElementList ispSpecPeList;
  t_sw3A_PageElementList histStatPeList;
  t_sw3A_PageElementList sdlPeList;
  t_sw3A_PageElementList flashPeList;
  t_sw3A_PageElementList frameratePeList;
  virtual void AewbDone();

  // Af component
  OMX_3A_CONFIG_FOCUSTYPE focusParam;
  t_sw3A_PageElementList focusPeList;
  virtual void AfDone();
  // other sensor related
  OMX_3A_CONFIG_MAXFRAMERATETYPE framerateParam;
  OMX_3A_CONFIG_FLASHSYNCHROTYPE flashSync;
  OMX_3A_CONFIG_SENSORMODETYPE sensorMode;

  // Reference to the statistics buffer shared with ISP and allocated by hw_buffer.
  OMX_3A_OTHER_STATSBUFFERSTORAGE *pStatsBufferStorage;
//  OMX_SYMBIAN_CAPTUREPARAMETERSTYPE CaptureParametersType;
  // Update Mechanism
  t_uint32 systemCoin;
  t_uint32 afZoneCoin;
  t_uint32 afStatsExportCoin;
  t_uint32 focusCtrlCoin;
  t_uint32 histCoin;
  t_uint32 sensorCoin;
  t_uint32 ispCoin;
//  t_uint32 glaceControlUpdateCount;
  t_uint32 glaceParamUpdateCount;
  t_uint32 runmodeCoin;

  // Lens position infomation
  t_uint32 fwStatusInfinityFarEnd;
  t_uint32 fwStatusMacroNearEnd;
  t_uint32 fwStatusInfinityHor;
  t_uint32 fwStatusMacroHor;
  t_uint32 fwStatusLensRange;
  bool statsReceived;
  // EEPROMdata
  t_uint8 nvmData[2048];
  OMX_3A_SENSOR_NAME_TYPE sensorName;
};
#endif // _SW3A_COMPONENT_MANAGER_EXTENSION_H_
