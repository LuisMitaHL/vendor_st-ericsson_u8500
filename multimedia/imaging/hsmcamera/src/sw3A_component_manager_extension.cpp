/*
 * copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "sw3A_component_manager_extension.h"
#include "camera.h"
#include "hsmcam.h"
#include "MMIO_Camera.h"    // Gamma LUT programming
#include "pictor.h"         // Gamma LUT programming
#include "ImgConfig.h"
#include "IFM_Trace.h"

#undef OMXCOMPONENT
#define OMXCOMPONENT "SW3A_COMP_MGR_EXT"
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8
extern "C"{
#include "osi_trace.h"
}

#define LOG_ENABLE_CONFIGFOCUS
#undef  LOG_ENABLE_CONFIGFOCUS

#define OMX_CONF_INIT_STRUCT(_s_, _name_)         \
  (_s_).nSize                    = sizeof(_name_);         \
  (_s_).nVersion.s.nVersionMajor = 0x1;                    \
  (_s_).nVersion.s.nVersionMinor = 0x1;                    \
  (_s_).nVersion.s.nRevision     = 0x2;                    \
  (_s_).nVersion.s.nStep         = 0x0

#define OMX_CONF_Q24 (1 << 24)
#define OMX_CONF_Q20 (1 << 20)
#define OMX_CONF_Q8  (1 <<  8)
#define OMX_CONF_U_TO_F(u, b)   (((float)(u)) / (b))
#define OMX_CONF_S_TO_F(u, b)   (((float)((t_sint32)u)) / (b))

#define SW3A_EXPOSURE_LINE_MARGIN_US (200)

#define SW3A_ARM_TO_ISP_TRANSLATE(_a_) \
  ((t_uint32) tempChunk.ispLogicalAddress + (t_uint32) (_a_) - (t_uint32) pData);

// Local prototypes
static void callBackLensStop(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd);
static void sw3A_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd);

Sw3AProcessingCompExt* Sw3AProcessingCompExt::pThis = NULL;

Sw3AProcessingCompExt::Sw3AProcessingCompExt(TraceObject* traceobj)
: Sw3AProcessingComp(traceobj)
{
  MSG0("Sw3AProcessingCompExt::Sw3AProcessingCompExt \n");

  pThis = this;

  userData = NULL;
  StatsDone = NULL;
  systemCoin = 0;
  afZoneCoin = 0;
  afStatsExportCoin = 0;
  focusCtrlCoin = 0;
  histCoin = 0;
  runmodeCoin = 0;
  sensorCoin = 0;
  ispCoin = 0;
//  glaceControlUpdateCount = 0;
  glaceParamUpdateCount = 0;
  m3Amode = Sw3AMode_Preview;
  m3Aconfigmode = Sw3AConfigMode_Static;
  m3Astate = Sw3AState_Stopped;
  bIspParmInit = false;
  bExposureParmInit = false;
  bGammaParmInit = false;
  bFramerateInit = false;
  bLscInit = false;
  bLinearizerInit = false;
  bValidLsc = OMX_FALSE;
  bValidLinearizer = OMX_FALSE;
  bFramerateUpdate = false;
  bSensorModechanged = false;
  memset(&ispParams, 0, sizeof(OMX_3A_CONFIG_ISPPARAMETERSTYPE));
  // additional FwStatus reading
  fwStatus.a_list[fwStatus.size++].pe_addr = Glace_Status_u8_ControlUpdateCount_Byte0;
  fwStatus.a_list[fwStatus.size++].pe_addr = HistStats_Status_e_CoinStatus_Byte0;
  fwStatus.a_list[fwStatus.size++].pe_addr = RunMode_Control_e_Coin_Ctrl_Byte0;
  fwStatus.a_list[fwStatus.size++].pe_addr = FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0;
  fwStatus.a_list[fwStatus.size++].pe_addr = SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0;
  fwStatus.a_list[fwStatus.size++].pe_addr = SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0;
  pCastPos0GridsData = NULL;
  pCastPos1GridsData = NULL;
  pCastPos2GridsData = NULL;
  pCastPos3GridsData = NULL;
  pSdlR  = NULL;
  pSdlGr = NULL;
  pSdlGb = NULL;
  pSdlB  = NULL;

  OMX_CONF_INIT_STRUCT(ispParams, OMX_3A_CONFIG_ISPPARAMETERSTYPE);
  OMX_CONF_INIT_STRUCT(exposureParam, OMX_3A_CONFIG_EXPOSURETYPE);
  OMX_CONF_INIT_STRUCT(gammaTable, OMX_3A_CONFIG_GAMMATABLE_TYPE);
  OMX_CONF_INIT_STRUCT(lscTable, OMX_3A_CONFIG_LSCTABLE_TYPE);
  OMX_CONF_INIT_STRUCT(linearizerTable, OMX_3A_CONFIG_LINEARIZERTABLE_TYPE);
  OMX_CONF_INIT_STRUCT(focusParam, OMX_3A_CONFIG_FOCUSTYPE);
  OMX_CONF_INIT_STRUCT(flashSync, OMX_3A_CONFIG_FLASHSYNCHROTYPE);
  OMX_CONF_INIT_STRUCT(sensorMode, OMX_3A_CONFIG_SENSORMODETYPE);

  focusParam.lensIsReady = TRUE;
  flashSync.bSync = OMX_FALSE;

  MSG1("Sw3AProcessingCompExt::Sw3AProcessingCompExt %d.\n", m3Aconfigmode);
}

OMX_ERRORTYPE Sw3AProcessingCompExt::setConfig(
  OMX_INDEXTYPE nParamIndex,
  OMX_PTR pComponentParameterStructure)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;

  switch (nParamIndex) {
    case OMX_3A_IndexConfigIspParameters:
    {
      MSG0("setConfig::OMX_3A_IndexConfigIspParameters(Extension).\n");
      if (m3Astate == Sw3AState_Running)
      {
        SetIspParamtersConfig((OMX_3A_CONFIG_ISPPARAMETERSTYPE*)pComponentParameterStructure,
          SW3A_COMMIT_LIST);
      }
      else
      {
        ispParams = *((OMX_3A_CONFIG_ISPPARAMETERSTYPE*)pComponentParameterStructure);
      }
      if (!bIspParmInit)
      {
        bIspParmInit = true;
      }
      break;
    }
    case OMX_3A_IndexConfigExposure:
    {
      MSG0("setConfig::OMX_3a_IndexConfigExposure(Extension).\n");
      SetExposureConfig((OMX_3A_CONFIG_EXPOSURETYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      break;
    }
    break;
  case OMX_3A_IndexConfigGamma:
    {
      MSG0("setConfig::OMX_3a_IndexConfigGamma(Extension).\n");
      SetGammaConfig((OMX_3A_CONFIG_GAMMATABLE_TYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      bGammaParmInit = true;
    }
    break;
  case OMX_3A_IndexConfigLsc:
    {
      MSG0("setConfig::OMX_3a_IndexConfigLsc(Extension).\n");
      SetLscConfig((OMX_3A_CONFIG_LSCTABLE_TYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      bLscInit = true;
    }
    break;
  case OMX_3A_IndexConfigLinearizer:
    {
      MSG0("setConfig::OMX_3a_IndexConfigLinearizer(Extension).\n");
      SetLinearizerConfig((OMX_3A_CONFIG_LINEARIZERTABLE_TYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      bLinearizerInit = true;
    }
    break;
  case OMX_3A_IndexConfigFocus:
    {
      MSG0("setConfig::OMX_3a_IndexConfigFocus(Extension).\n");
      SetFocusConfig((OMX_3A_CONFIG_FOCUSTYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      break;
    }
    case OMX_3A_IndexConfigMaxFramerate:
    {
      MSG0("setConfig::OMX_3A_IndexConfigMaxFramerate(Extension).\n");
      SetFramerateConfig((OMX_3A_CONFIG_MAXFRAMERATETYPE*)pComponentParameterStructure,
        SW3A_COMMIT_LIST);
      break;
    }
    case OMX_3A_IndexConfigFlashSynchro:
    {
      // TODO: Need to confirm how to handle this flash syncro.
      flashSync = *((OMX_3A_CONFIG_FLASHSYNCHROTYPE*)pComponentParameterStructure);
      break;
    }
    case OMX_3A_IndexConfigSensorMode:
    {
      // This should apply next startPreview timing
      if (sensorMode.sensorMode != ((OMX_3A_CONFIG_SENSORMODETYPE*)pComponentParameterStructure)->sensorMode)
      {
        MSG2("sensorMode is changed from %d to %d\n",
             sensorMode.sensorMode,
             ((OMX_3A_CONFIG_SENSORMODETYPE*)pComponentParameterStructure)->sensorMode);
        sensorMode = *((OMX_3A_CONFIG_SENSORMODETYPE*)pComponentParameterStructure);
        ((CFramerateExt*)(mSM->pFramerate))->setSensorUsageMode(sensorMode.sensorMode);
        bSensorModechanged = true;
      }
      break;
    }
    case OMX_3A_IndexConfigEEPROMData:
    {
      // EEPROM data is read only configuration
      break;
    }
    default :
//       err = Sw3AProcessingComp::setConfig(nParamIndex,
//         pComponentParameterStructure);
      break;
  }
  return err;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::getConfig(
  OMX_INDEXTYPE nParamIndex,
  OMX_PTR pComponentParameterStructure)
{
  IN0("");
  OMX_ERRORTYPE err = OMX_ErrorNone;
  switch (nParamIndex)
  {
    case OMX_3A_IndexConfigIspParameters:
    {
      OMX_3A_CONFIG_ISPPARAMETERSTYPE* pParms;
      pParms = (OMX_3A_CONFIG_ISPPARAMETERSTYPE*)pComponentParameterStructure;

      *pParms = ispParams;
      break;
    }
    case OMX_3A_IndexConfigFlashSynchro:
    {
      OMX_3A_CONFIG_FLASHSYNCHROTYPE* pParms;
      pParms = (OMX_3A_CONFIG_FLASHSYNCHROTYPE*)pComponentParameterStructure;
      *pParms = flashSync;
      break;
    }
    case OMX_3A_IndexConfigExposure:
    {
      OMX_3A_CONFIG_EXPOSURETYPE* pParms;
      pParms = (OMX_3A_CONFIG_EXPOSURETYPE*)pComponentParameterStructure;
      *pParms = exposureParam;
      break;
    }
    case OMX_3A_IndexConfigSensorMode:
    {
      OMX_3A_CONFIG_SENSORMODETYPE* pParms;
      pParms = (OMX_3A_CONFIG_SENSORMODETYPE*)pComponentParameterStructure;
      *pParms = sensorMode;
      break;
    }
    case OMX_3A_IndexConfigMaxFramerate:
    {
      OMX_3A_CONFIG_MAXFRAMERATETYPE* pParms;
      pParms = (OMX_3A_CONFIG_MAXFRAMERATETYPE*)pComponentParameterStructure;
      *pParms = framerateParam;
      break;
    }
    case OMX_3A_IndexConfigFocus:
    {
#ifdef LOG_ENABLE_CONFIGFOCUS
      MSG0("getConfig::OMX_3a_IndexConfigFocus(Extention)\n");
      MSG1("lensPos = %d | lensIsReady = %d\n", focusParam.lensPos, focusParam.lensIsReady);
#endif // LOG_ENABLE_CONFIGFOCUS

      OMX_3A_CONFIG_FOCUSTYPE* pFocustype;
      pFocustype = (OMX_3A_CONFIG_FOCUSTYPE*)pComponentParameterStructure;
      *pFocustype = focusParam;
      break;
    }
    case OMX_3A_IndexConfigEEPROMData:
    {
      OMX_3A_CONFIG_EEPROMDATATYPE* pEepromType;
      pEepromType = (OMX_3A_CONFIG_EEPROMDATATYPE*)pComponentParameterStructure;
      pEepromType->nSizeOfData = 2048;
      memcpy(&pEepromType->eepromData,
        &nvmData,
        sizeof(nvmData));
      break;
    }

    default :
      err = Sw3AProcessingComp::getConfig(nParamIndex,
        pComponentParameterStructure);
      break;
  }
  OUT0("");
  return err;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::setMode(
  t_sw3A_LoopState loopState,
  t_uint32 numberOfStats)
{
  MSG1("Sw3AProcessingCompExt::setMode %d.\n", loopState);
  if (loopState == SW3A_MODE_SET_STILL)
  {
    m3Amode = Sw3AMode_Snapshot;
    bStillSynchro_StatsReceived = false;
    bStillSynchro_GrabAllowed = false;
    statsReceived = false;
    bFramerateUpdate = true;
  }
  else if (loopState == SW3A_MODE_SET_STILLVF)
  {
    m3Amode = Sw3AMode_Preview;
    bFramerateUpdate = true;
  }
  else if (loopState == SW3A_MODE_SET_VIDEO ||
           loopState == SW3A_MODE_SET_VIDEOVF)
  {
    m3Amode = Sw3AMode_Preview;
    bFramerateUpdate = false;
  }
  return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::open(enumCameraSlot camSlot, CFlashDriver *pFlashDriver)
{
  IN0("");
  MSG0("Sw3AProcessingCompExt::open.\n");

//  Sw3AProcessingComp::open(camSlot, pFlashDriver);


  CreateStatsBuffer();

  mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP,
    (callBackLensStop),
    (t_ispctlEventClbkCtxtHnd) this);
  mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_GLACE_STATS_READY, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);
  mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_AUTOFOCUS_STATS_READY, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);
  mIspCtlEventClbks->setEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP, (sw3A_ispctlInfo), (t_ispctlEventClbkCtxtHnd) this);

  // Allocate and initialize the local copy of dampers, makernotes and sw3A extradata.
  t_sw3A_Buffer setBuf;
  t_sw3A_Buffer *ptBuf = NULL;
  // makernote buffer (not be used)
  ptBuf = makernotesBuf.create(getMakernotesAllocSize());
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of makernotes.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }

  // extradata buffer (not be used)
  ptBuf = extradataBuf.create(getExtradataAllocSize());
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of extradata.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }

  // captureParametersBuf (be used for ISPProc)
  ptBuf = captureParametersBuf.create(sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE));
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of CaptureParameters.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }
  else
  {
    OMX_SYMBIAN_CAPTUREPARAMETERSTYPE initBuf;
    memset(&initBuf, 0, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE));
    setBuf.pData = (t_uint8*)&initBuf;
    setBuf.size = sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE);
    captureParametersBuf.set(&setBuf);
  }

  // damperDataBuf (not be used)
  ptBuf = dampersDataBuf.create(sizeof(IFM_DAMPERS_DATA));
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of dampersData.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }

  // flashStatusBuf (not be used)
  ptBuf = flashStatusBuf.create(sizeof(OMX_STE_PRODUCTIONTESTTYPE));
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of flashStatus.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }

  // captureParametersBuf (be used for Gamma and ISPProc)
  ptBuf = steExtraCaptureParametersBuf.create(sizeof(OMX_STE_CAPTUREPARAMETERSTYPE));
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of steExtraCaptureParametersBuf.\n");
    OUT0("\n");
    return OMX_ErrorInsufficientResources;
  }
  else
  {
    OMX_STE_CAPTUREPARAMETERSTYPE initBuf;
    memset(&initBuf, 0, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE));
    setBuf.pData = (t_uint8*)&initBuf;
    setBuf.size = sizeof(OMX_STE_CAPTUREPARAMETERSTYPE);
    steExtraCaptureParametersBuf.set(&setBuf);
  }

  // focusStatusBuf (not be used)
  ptBuf = focusStatusBuf.create(IFM_CONFIG_EXTFOCUSSTATUS_SIZE(IFM_MAX_NUMBER_OF_FOCUS_STATUS_ROIS));
  if (ptBuf == NULL) {
    MSG0("Could not allocate local copy of focusStatus.\n");
    OUT0("\n"); return OMX_ErrorInsufficientResources;
  }

  // To avoid some assert of flash related(we don't use this flash driver.
  mFlashController.setFlashDriver(pFlashDriver);

  OUT0("\n");

  t_sw3A_Msg msg = SW3A_MSG_OPEN_COMPLETE;
  info(msg, NULL);
  return OMX_ErrorNone;
}
OMX_ERRORTYPE Sw3AProcessingCompExt::close()
{
  IN0("\n");
  OMX_ERRORTYPE err = OMX_ErrorNone;
  // Check that there is no pending setConfig() in NMF queue.
  // It is not necessary to check the config shadow storage status as, by design, the usedCount cannot be zero when the shadow storage is not empty.
  if (mConfigQueue_usedCount > 0) {
    MSG0("There are pending setConfig() in SW3A NMF queue. Posponing the close().\n");
    bClosePending = true;
  } else {
    makernotesBuf.destroy();
    extradataBuf.destroy();
    captureParametersBuf.destroy();
    dampersDataBuf.destroy();
    flashStatusBuf.destroy();
    steExtraCaptureParametersBuf.destroy();
    focusStatusBuf.destroy();

    mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_GLACE_STATS_READY);
    mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_AUTOFOCUS_STATS_READY);
    mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP);
    mIspCtlEventClbks->clearEventClbk(SIA_CLIENT_CAMERA, ISP_FLADRIVER_LENS_STOP);
    camSharedMemError_t csm_err = mSharedMemory->releaseChunk(CAM_SHARED_MEM_CHUNK_3A_STATS);
    // post-condition: CamSharedMemory::destroy();
    if (csm_err != CAM_SHARED_MEM_ERR_NONE) {
      MSG0("Could not release 3A shared memory chunk.\n");
      err = OMX_ErrorUndefined;
    }
  }
  OUT0("\n");
  return err;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::start3A()
{
  MSG0("Kicking the 3A loop(Extension).\n");
  // First to get fwStatus
  GetFwStatus();

  MSG2("Sw3AProcessingCompExt::start3A %d %d.\n", m3Aconfigmode, bIspParmInit);
  if (bExposureParmInit)
  {
    SetExposureConfig(&exposureParam, SW3A_ENQUEUE_LIST);
  }
  if (bIspParmInit)
  {
    SetIspParamtersConfig(&ispParams, SW3A_ENQUEUE_LIST);
  }
  else
  {
    ispParams.nispparam_aewb_static.nHGridSize = OMX_3A_OTHER_GLACE_GRID_MAX_WIDTH;
    ispParams.nispparam_aewb_static.nVGridSize = OMX_3A_OTHER_GLACE_GRID_MAX_HEIGHT;
    ispParams.nispparam_aewb_static.nHBlockSizeFraction = 1.0;
    ispParams.nispparam_aewb_static.nVBlockSizeFraction = 1.0;
    ispParams.nispparam_aewb_static.nHROIStartFraction = 0.0;
    ispParams.nispparam_aewb_static.nVROIStartFraction = 0.0;
    ispParams.nispparam_aewb_static.nRedSaturationLevel = 255;
    ispParams.nispparam_aewb_static.nGreenSaturationLevel = 255;
    ispParams.nispparam_aewb_static.nBlueSaturationLevel = 255;
    SetIspparamAewb_Static(&ispParams.nispparam_aewb_static);

    bool bAfStats = false;
    OMX_3A_ISPPARAM_STATIC_AFTYPE ispAfStatic;
    ispAfStatic.nCoringValue = 0;
// TODO: Because a value is not decided, I invalidate it.
//    ispAfStatic.nHRatioNum = 1;
//    ispAfStatic.nHRatioDen = 6;
//    ispAfStatic.nVRatioNum = 1;
//    ispAfStatic.nVRatioDen = 7;
    ispAfStatic.nHostActiveZonesCount = 10;
    ispAfStatic.nFlag_AbsSquareEnabled = 0;
    ispAfStatic.nFlag_ReducedZoneSetup = 0;

    SetIspparamAf_Static(&ispAfStatic);

    OMX_3A_ISPPARAM_DYNAMIC_AFTYPE ispAfDynamic;
    ispAfDynamic.nHostAFZone[0].nHostAFZoneStartX = 42 * OMX_CONF_Q8;
    ispAfDynamic.nHostAFZone[0].nHostAFZoneStartY = 39 * OMX_CONF_Q8;
    ispAfDynamic.nHostAFZone[0].nHostAFZoneEndX = 58 * OMX_CONF_Q8;
    ispAfDynamic.nHostAFZone[0].nHostAFZoneEndY = 60 * OMX_CONF_Q8;
    ispAfDynamic.nHostAFZone[0].nEnable = TRUE;

    SetIspparamAf_Dynamic(&ispAfDynamic);
  }
  if (bGammaParmInit)
  {
    SetGammaConfigtoIsp(&gammaTable);
  }
  if (bLscInit)
  {
    pCastPos0GridsData = &lscTable.nCastPos0GridsData[0];
    pCastPos1GridsData = &lscTable.nCastPos1GridsData[0];
    pCastPos2GridsData = &lscTable.nCastPos2GridsData[0];
    pCastPos3GridsData = &lscTable.nCastPos3GridsData[0];
    SetLscConfigtoIsp();
  }
  if (bLinearizerInit)
  {
    pSdlR  = &linearizerTable.nSdlR[0];
    pSdlGr = &linearizerTable.nSdlGr[0];
    pSdlGb = &linearizerTable.nSdlGb[0];
    pSdlB  = &linearizerTable.nSdlB[0];
    SetLinearizerConfigtoIsp();
  }
  if (m3Amode == Sw3AMode_Snapshot)
  {
    // refreshing CaptureParameters
    // Scene
    OMX_SYMBIAN_CAPTUREPARAMETERSTYPE* pCaptureParameters;
    pCaptureParameters = getCaptureParameters(false);
    pCaptureParameters->eSceneOrientation =
      ((COmxCamera*)&mSM->mENSComponent)->mSceneOrientation.eOrientation;
    // WhiteBalance
    float temp = 0.0;
    temp = OMX_CONF_U_TO_F(ispParams.nispparam_wb_dynamic.nRedManualgain, OMX_CONF_Q24);
    pCaptureParameters->xGainR = (t_sint32)(temp * 0x10000);
    temp = OMX_CONF_U_TO_F(ispParams.nispparam_wb_dynamic.nGreenManualgain, OMX_CONF_Q24);
    pCaptureParameters->xGainG = (t_sint32)(temp * 0x10000);
    temp = OMX_CONF_U_TO_F(ispParams.nispparam_wb_dynamic.nBlueManualgain, OMX_CONF_Q24);
    pCaptureParameters->xGainB = (t_sint32)(temp * 0x10000);
//     pCaptureParameters->nGainR = FloatToI32(ispParams.nispparam_wb_dynamic.nRedManualgain);
//     pCaptureParameters->nGainG = FloatToI32(ispParams.nispparam_wb_dynamic.nGreenManualgain);
//     pCaptureParameters->nGainB = FloatToI32(ispParams.nispparam_wb_dynamic.nBlueManualgain);

    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInRed, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[0] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInGreen, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[1] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInBlue, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[2] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInRed, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[3] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInGreen, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[4] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInBlue, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[5] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInRed, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[6] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInGreen, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[7] = (t_sint32)(temp * 0x100);
    temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInBlue, OMX_CONF_Q24);
    pCaptureParameters->nRGB2RGBColorConversion[8] = (t_sint32)(temp * 0x100);
    pCaptureParameters->nOffsetR
      = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_R * 64);
    pCaptureParameters->nOffsetG
      = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_G * 64);
    pCaptureParameters->nOffsetB
      = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_B * 64);

    m3Aconfigmode = Sw3AConfigMode_Static;
    {
      OMX_STE_CAPTUREPARAMETERSTYPE* pExtraCaptureData;
      pExtraCaptureData = getSteExtraCaptureParameters(false);
      OMX_CONF_INIT_STRUCT((*pExtraCaptureData), OMX_STE_CAPTUREPARAMETERSTYPE);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInRed, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[0] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInGreen, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[1] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nRedInBlue, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[2] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInRed, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[3] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInGreen, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[4] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nGreenInBlue, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[5] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInRed, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[6] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInGreen, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[7] = (t_sint32)(temp * 0x100);
      temp = OMX_CONF_S_TO_F(ispParams.nispparam_colormatrix_dynamic.nBlueInBlue, OMX_CONF_Q24);
      pExtraCaptureData->nColorMatrix[8] = (t_sint32)(temp * 0x100);
      pExtraCaptureData->nOffsetR
        = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_R * 64);
      pExtraCaptureData->nOffsetG
        = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_G * 64);
      pExtraCaptureData->nOffsetB
        = (t_sint32)(- ispParams.nispparam_colormatrix_dynamic.nOffset_B * 64);
      pExtraCaptureData->bValidSFX = OMX_FALSE;
      pExtraCaptureData->bValidEffect = OMX_FALSE;
    }
  }
  else
  {
    // framerate parameter is only valid for Preview
    if (bFramerateUpdate)
    {
      SetFramerateConfig(&framerateParam, SW3A_ENQUEUE_LIST);
    }
    m3Aconfigmode = Sw3AConfigMode_Dynamic;
  }
  // Flash config is needed in case of we use Port2 for snapshot
  SetFlashConfig(flashSync.bSync, SW3A_ENQUEUE_LIST);

  if (m3Amode == Sw3AMode_Snapshot)
  {
    UpdateCoins(SW3A_SYNC_LIST,
                TRUE, // bSystem
                FALSE,  // bAfStats
                FALSE, // bHist
                FALSE, // bSensor
                FALSE); // bIsp
  }
  else
  {
    UpdateCoins(SW3A_SYNC_LIST,
                FALSE, // bSystem
                TRUE,  // bAfStats
                FALSE, // bHist
                FALSE, // bSensor
                TRUE); // bIsp
  }
  m3Astate = Sw3AState_Running;

  return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::stop3A()
{
  m3Astate = Sw3AState_Stopping;

  t_sw3A_Msg msg = SW3A_MSG_STOP_ACK;
  info(msg, NULL);
  // We can stop immediately
  m3Astate = Sw3AState_Stopped;
  m3Aconfigmode = Sw3AConfigMode_Static;
  msg = SW3A_MSG_STOP_COMPLETE;
  info(msg, NULL);

  return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::run3A(
  enum e_ispctlInfo info)
{
  MSG0("3A iteration(Extension).\n");
  OMX_ERRORTYPE err = OMX_ErrorNone;
  // NB: statistics and capture live parameters are passed directly in the FW/SW shared memory
  switch(info) {
  case ISP_GLACE_STATS_READY:
    if (userData && StatsDone && (m3Amode == Sw3AMode_Preview) && (m3Astate == Sw3AState_Running))
    {
      // pop pBuffer from queue
      OMX_BUFFERHEADERTYPE* pBuffer = (OMX_BUFFERHEADERTYPE*)mAewbQueue.dequeue();
      if (pBuffer)
      {
        OMX_3A_OTHER_STATSAEWBDATATYPE* pAewbBuffer;
        pAewbBuffer = (OMX_3A_OTHER_STATSAEWBDATATYPE*)pBuffer->pBuffer;
        if (pAewbBuffer)
        {
          // copy stats to pBuffer
          memcpy(&(pAewbBuffer->statsR[0]),
            pStatsBufferStorage->pGridR,
            OMX_3A_OTHER_STATSAEWB_DATASIZE);
          memcpy(&(pAewbBuffer->statsG[0]),
            pStatsBufferStorage->pGridG,
            OMX_3A_OTHER_STATSAEWB_DATASIZE);
          memcpy(&(pAewbBuffer->statsB[0]),
            pStatsBufferStorage->pGridB,
            OMX_3A_OTHER_STATSAEWB_DATASIZE);
          memcpy(&(pAewbBuffer->statsS[0]),
            pStatsBufferStorage->pGridS,
            OMX_3A_OTHER_STATSAEWB_DATASIZE);

          pBuffer->nFilledLen = sizeof(OMX_3A_OTHER_STATSAEWBDATATYPE);

          MSG1("StatsDone Aewb pBuffer=%p", pBuffer);
          StatsDone(userData, pBuffer);
        }
      }
    }
    AewbDone();
    break;
  case ISP_AUTOFOCUS_STATS_READY:
    if (userData && StatsDone && (m3Amode == Sw3AMode_Preview) && (m3Astate == Sw3AState_Running))
    {
      // pop pBuffer from queue
      OMX_BUFFERHEADERTYPE* pBuffer = (OMX_BUFFERHEADERTYPE*)mAfQueue.dequeue();
      if (pBuffer)
      {
        OMX_3A_OTHER_STATSAFDATATYPE* pAfBuffer = (OMX_3A_OTHER_STATSAFDATATYPE*)pBuffer->pBuffer;
        if (pAfBuffer)
        {
          // copy stats to pBuffer
          memcpy(&(pAfBuffer->afStats[0]),
            &(pStatsBufferStorage->afStats[0]),
            sizeof(OMX_3A_OTHER_STATSAF_HOSTZONESTATUS) * OMX_3A_OTHER_STATSAF_WINDOW_NUMBER_MAX);

#ifdef LOG_ENABLE_CONFIGFOCUS
          MSG1("afStatsLensPos = %d\n", pStatsBufferStorage->afStatsLensPos);
#endif // LOG_ENABLE_CONFIGFOCUS

          pBuffer->nFilledLen = sizeof(OMX_3A_OTHER_STATSAFDATATYPE);

          MSG1("StatsDone Af pBuffer=%p", pBuffer);
          StatsDone(userData, pBuffer);

          // AF setting for ISP
          // TODO: Input a non-transitory value for Debug
          // I am going to delete it after official correspondence
          // LocalLoopPush(OMX_3A_OTHER_AF);
        }
      }
    }
    AfDone();
    break;

  default: // Unknown event. Do nothing.
    DBC_ASSERT(0);
    err = OMX_ErrorNotReady;
  }
  return err;
}

OMX_S32 Sw3AProcessingCompExt::getMakernotesAllocSize()
{
  return 0;
}

OMX_S32 Sw3AProcessingCompExt::getExtradataAllocSize()
{
  return 0;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::fillThisBufferAewb(
  OMX_BUFFERHEADERTYPE* pBuffer)
{
  MSG1("Sw3AProcessingCompExt::fillThisBuffer pBuffer = %p\n", pBuffer);
  mAewbQueue.enqueue(pBuffer);
  return OMX_ErrorNone;
}

OMX_ERRORTYPE Sw3AProcessingCompExt::fillThisBufferAf(
  OMX_BUFFERHEADERTYPE* pBuffer)
{
  MSG1("Sw3AProcessingCompExt::fillThisBuffer pBuffer = %p\n", pBuffer);
  mAfQueue.enqueue(pBuffer);
  return OMX_ErrorNone;
}

void Sw3AProcessingCompExt::FlushAfPort()
{
  MSG0("Sw3AProcessingCompExt::Flush AF port pBuffer\n");
  // TODO: dequeue mAfQueue and call fillThisBuffer with nFilledLen = 0
  OMX_BUFFERHEADERTYPE* pBuffer;
  while((pBuffer = (OMX_BUFFERHEADERTYPE*)mAfQueue.dequeue()))
  {
    pBuffer->nFilledLen = 0;
    MSG1("StatsDone Aewb pBuffer=%p", pBuffer);
    StatsDone(userData, pBuffer);
  }
}

void Sw3AProcessingCompExt::FlushAewbPort()
{
  MSG0("Sw3AProcessingCompExt::Flush AEWB port pBuffer\n");
  // TODO: dequeue mAewbQueue and call fillThisBuffer with nFilledLen = 0
  OMX_BUFFERHEADERTYPE* pBuffer;
  while((pBuffer = (OMX_BUFFERHEADERTYPE*)mAewbQueue.dequeue()))
  {
    pBuffer->nFilledLen = 0;
    MSG1("StatsDone Aewb pBuffer=%p", pBuffer);
    StatsDone(userData, pBuffer);
  }
}

void Sw3AProcessingCompExt::CreateStatsBuffer()
{
  t_uint32 gridWidth  = OMX_3A_OTHER_GLACE_GRID_MAX_WIDTH;
  t_uint32 gridHeight = OMX_3A_OTHER_GLACE_GRID_MAX_HEIGHT;
  // Configurable size for CNips3AAPI::TGrid
  t_uint32 bufferSize = gridWidth * gridHeight * 4 + sizeof(OMX_3A_OTHER_STATSBUFFERSTORAGE);

   MSG1("mSharedMemory(CreateStatsBuffer) %p .\n", mSharedMemory);
   // Memory layout setup
  camSharedMemChunk_t tempChunk;
  camSharedMemError_t csm_err_release = mSharedMemory->releaseChunk(CAM_SHARED_MEM_CHUNK_3A_STATS);
  camSharedMemError_t csm_err = mSharedMemory->getChunk(&tempChunk, CAM_SHARED_MEM_CHUNK_3A_STATS);
  if ((csm_err != CAM_SHARED_MEM_ERR_NONE)) {
     MSG2("Could not get 3A shared memory chunk(Extension). csm_err %d \n",
       csm_err);
     return;
  }

  OMX_U8 *pData = tempChunk.armLogicalAddress;
  pStatsBufferStorage = (OMX_3A_OTHER_STATSBUFFERSTORAGE *) (pData + 4 * gridWidth * gridHeight);
  pStatsBufferStorage->pGridR = pData;
  pStatsBufferStorage->pGridG = pData + gridWidth * gridHeight;
  pStatsBufferStorage->pGridB = pData + gridWidth * gridHeight * 2;
  pStatsBufferStorage->pGridS = pData + gridWidth * gridHeight * 3;

  // Address translation setup
  pStatsBufferStorage->glaceStats.u32_TargetRedAddress
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->pGridR);
  pStatsBufferStorage->glaceStats.u32_TargetGreenAddress
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->pGridG);
  pStatsBufferStorage->glaceStats.u32_TargetBlueAddress
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->pGridB);
  pStatsBufferStorage->glaceStats.u32_TargetSaturationAddress
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->pGridS);
  pStatsBufferStorage->glaceStats.u32_HGridSize
    = gridWidth;
  pStatsBufferStorage->glaceStats.u32_VGridSize
    = gridHeight;

  pStatsBufferStorage->ispHistAddr_R
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->apHistR);
  pStatsBufferStorage->ispHistAddr_G
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->apHistG);
  pStatsBufferStorage->ispHistAddr_B
    = SW3A_ARM_TO_ISP_TRANSLATE(pStatsBufferStorage->apHistB);

  pStatsBufferStorage->ispGlaceStatsAddr
    = SW3A_ARM_TO_ISP_TRANSLATE(&(pStatsBufferStorage->glaceStats));
  pStatsBufferStorage->ispSensorParamAddr
    = SW3A_ARM_TO_ISP_TRANSLATE(&(pStatsBufferStorage->frameParamStatus));
  pStatsBufferStorage->ispAfStatsAddr
    = SW3A_ARM_TO_ISP_TRANSLATE(&(pStatsBufferStorage->afStats));
  return;
}

static void setCallback(
  void* handle,
  void* ud,
  StatsCallbackFunc_t func)
{
  Sw3AProcessingCompExt* pThis = (Sw3AProcessingCompExt*)handle;
  pThis->setCallback(ud, func);
}

static OMX_ERRORTYPE fillThisBufferForStats(
  void* handle,
  OMX_BUFFERHEADERTYPE* pBuffer)
{
  Sw3AProcessingCompExt* pThis = (Sw3AProcessingCompExt*)handle;
  OMX_ERRORTYPE err = OMX_ErrorBadPortIndex;
  if (pBuffer->nOutputPortIndex == CAMERA_PORT_STATS_AEWB)
  {
    err = pThis->fillThisBufferAewb(pBuffer);
  }
  else if(pBuffer->nOutputPortIndex == CAMERA_PORT_STATS_AF)
  {
    err = pThis->fillThisBufferAf(pBuffer);
  }
  return err;
}

static void flushPort(
  void* handle,
  int nPortIndex)
{
  Sw3AProcessingCompExt* pThis = (Sw3AProcessingCompExt*)handle;
  if (nPortIndex == CAMERA_PORT_STATS_AEWB)
  {
    pThis->FlushAewbPort();
  }
  else if (nPortIndex == CAMERA_PORT_STATS_AF)
  {
    pThis->FlushAfPort();
  }
}

static OMX3A_ExtInterface_t itf;

OMX3A_ExtInterface_t* Get3AExtInterface()
{
  itf.handler     = Sw3AProcessingCompExt::getInstance();
  itf.SetCallback    = setCallback;
  itf.FillThisBuffer = fillThisBufferForStats;
  itf.FlushPort      = flushPort;
  return &itf;
}

// should make at least one coin update.
void Sw3AProcessingCompExt::UpdateCoins(t_sw3A_LoopState eState,
  bool bSystem,
  bool bAfStats,
  bool bHist,
  bool bSensor,
  bool bIsp)
{
  bool bUpdate = false;
  t_sw3A_PageElementList coinPeList;
  ClearPageElement(&coinPeList);
  if (bSystem)
  {
    systemCoin = 1 - systemCoin;
    PushPageElement(
      &coinPeList,
      SystemSetup_e_Coin_Ctrl_Byte0,
      systemCoin);
    bUpdate = true;
  }
  if (bAfStats)
  {
    afStatsExportCoin = 1 - afStatsExportCoin;
    PushPageElement(
      &coinPeList,
      AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0,
      afStatsExportCoin);
    bUpdate = true;
  }

  if (bSensor)
  {
    sensorCoin = 1 - sensorCoin;
    PushPageElement(
      &coinPeList,
      SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0,
      sensorCoin);
    bUpdate = true;
  }
  if (bIsp)
  {
    ispCoin = 1 - ispCoin;
    PushPageElement(
      &coinPeList,
      SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0,
      ispCoin);
    bUpdate = true;
  }

  if (!bUpdate)
  {
    PushPageElement(
      &coinPeList,
      DeviceParameters_u32_DeviceId_Byte0,
      (t_uint32)0);
  }
  done(coinPeList, eState, NULL, NULL, NULL);
}

void Sw3AProcessingCompExt::AewbDone()
{
  if((m3Astate == Sw3AState_Running))
  {
    if (m3Amode == Sw3AMode_Preview)
    {
//      UpdateCoins(SW3A_COMMIT_LIST, TRUE, FALSE, FALSE);
    }
    else
    {
      if (!statsReceived)
      {
        UpdateCoins(SW3A_COMMIT_LIST,
                    FALSE,  // bSystem
                    FALSE,  // bAfStats
                    FALSE,  // bHist
                    FALSE,  // bSensor
                    FALSE); // bIsp
        bStillSynchro_StatsReceived = true;
        bStillSynchro_GrabAllowed = true;
        statsReceived = true;
      }
    }
  }
}

void Sw3AProcessingCompExt::SetFrameRate()
{
  float f_rate;
  // Get Sensor maximum framerate
  f_rate = ((CFramerateExt*)(mSM->pFramerate))->getSensorMaxFrameRate();
  // Limit user setting framerate
  if (bFramerateInit)
  {
    float user_rate = (float)framerateParam.framerate / (1 << 16);
    if (user_rate < f_rate)
    {
      f_rate = user_rate;
    }
  }
  // Limit exposure framerate
  if (bExposureParmInit)
  {
    float exp_rate = (float)1000000 / (exposureParam.nTargetexposuretime + SW3A_EXPOSURE_LINE_MARGIN_US );
    if (exp_rate < f_rate)
    {
      f_rate = exp_rate;
    }
  }
  ClearPageElement(&frameratePeList);
  PushPageElement(
    &frameratePeList,
    VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0,
    f_rate);
}

bool Sw3AProcessingCompExt::SetExposureConfig(
  OMX_3A_CONFIG_EXPOSURETYPE* pExposuretype,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  // TODO: we need to consider about framerate on current release
  // EXPOSURE
  if (memcmp(pExposuretype,
        &exposureParam,
        sizeof(OMX_3A_CONFIG_EXPOSURETYPE)) != 0 ||
      m3Aconfigmode == Sw3AConfigMode_Static)
  {
    ClearPageElement(&exposurePeList);
    PushPageElement(
      &exposurePeList,
      Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0,
      pExposuretype->gain);
    PushPageElement(
      &exposurePeList,
      Exposure_DriverControls_u32_TargetExposureTime_us_Byte0,
      pExposuretype->nTargetexposuretime);
    PushPageElement(
      &exposurePeList,
      Exposure_DriverControls_e_Flag_NDFilter_Byte0,
      (float)1.0);
    done(exposurePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
    exposureParam = *pExposuretype;
    bExposureParmInit = true;
    if (m3Aconfigmode != Sw3AConfigMode_Static)
    {
      // TODO: need to consider this handling with Firmware.
      SetFrameRate();
      done(frameratePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
    }
    if (eState != SW3A_ENQUEUE_LIST)
    {
      UpdateCoins(eState,
                  FALSE,  // bSystem
                  FALSE,  // bAfStats
                  FALSE,  // bHist
                  TRUE,   // bSensor
                  FALSE); // bIsp
    }
    bUpdate = true;
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetFramerateConfig(
  OMX_3A_CONFIG_MAXFRAMERATETYPE* pMaxframerate,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  // TODO: this is currently fixed framerate setting.
  // Maxframerate
   if (memcmp(pMaxframerate,
         &framerateParam,
         sizeof(OMX_3A_CONFIG_MAXFRAMERATETYPE)) != 0 ||
     m3Aconfigmode == Sw3AConfigMode_Static)
   {
     float f_rate = (float)pMaxframerate->framerate/(1<<16);
     if (f_rate > ((CFramerateExt*)(mSM->pFramerate))->getSensorMaxFrameRate() ||
         f_rate == 0)
     {
       f_rate = ((CFramerateExt*)(mSM->pFramerate))->getSensorMaxFrameRate();
     }
     framerateParam = *pMaxframerate;
     bFramerateInit = true;
     SetFrameRate();
     done(frameratePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
     if (eState != SW3A_ENQUEUE_LIST)
     {
       UpdateCoins(eState,
                   FALSE,  // bSystem
                   FALSE,  // bAfStats
                   FALSE,  // bHist
                   TRUE,   // bSensor
                   FALSE); // bIsp
     }
     bUpdate = true;
   }
  return bUpdate;
}

// this is only called from start3A. because we need to synchronization when the mode is transit.
bool Sw3AProcessingCompExt::SetFlashConfig(
  OMX_BOOL bSync,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  MSG2("SetFlashConfig Flash sync is (%s) \n", bSync ? "TRUE" : "FALSE");
  ClearPageElement(&flashPeList);
  PushPageElement(
    &flashPeList,
    FlashControl_e_Flag_FlashMode_Byte0,
    (t_uint8)bSync);
  done(flashPeList, eState, NULL, NULL, NULL);
  bUpdate = true;
  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspParamtersConfig(
  OMX_3A_CONFIG_ISPPARAMETERSTYPE* pIspParams,
  t_sw3A_LoopState eState)
{
  MSG0("Sw3AProcessingCompExt::SetIspParamtersConfig\n");

  bool bUpdate = false;
  bool bGlace = false;
  bool bHist = false;
  bool bAfStats = false;
  // pushing Aewb Releated values in peListQueue(that are Exposure/WB/CM/StatsConfig)

  // BLACKLEVELCORRECTIONTYPE
  bUpdate |= SetIspparamBlackLevel_Static(&pIspParams->nispparam_black_static);

  //WHITEBALANCETYPE
  bUpdate |= SetIspparamWhiteBalance_Dynamic(&pIspParams->nispparam_wb_dynamic);

  //COLORCORRECTIONTYPE
  bUpdate |= SetIspparamColorCorrection_Dynamic(&pIspParams->nispparam_colormatrix_dynamic);

  //NOISEREDUCTIONTYPE
  bUpdate |= SetIspparamNoiseReduction_Static(&pIspParams->nispparam_noiser_static);
  bUpdate |= SetIspparamNoiseReduction_Dynamic(&pIspParams->nispparam_noiser_dynamic);

  //LENSSHADINGCORRECTIONTYPE
  bUpdate |= SetIspparamLensShading_Static(&pIspParams->nispparam_lensshad_static);
  bUpdate |= SetIspparamLensShading_Dynamic(&pIspParams->nispparam_lensshad_dynamic);

  //COLORCONVERSIONTYPE
  bUpdate |= SetIspparamColorConversion_Static(&pIspParams->nispparam_colorconversion_static);
  bUpdate |= SetIspparamColorConversion_Dynamic(&pIspParams->nispparam_colorconversion_dynamic);

  //EDGEENHANCEMENTTYPE
  bUpdate |= SetIspparamEdgeEnhance_Static(&pIspParams->nispparam_edgeEnhancement_static);
  bUpdate |= SetIspparamEdgeEnhance_Dynamic(&pIspParams->nispparam_edgeEnhancement_dynamic);

  //ISPSPECIFICTYPE
  bUpdate |= SetIspparamIspSpecific(&pIspParams->nispparam_ispSpecific);

  //AEWBTYPE
  bGlace = SetIspparamAewb_Static(&pIspParams->nispparam_aewb_static);

  //AFTYPE
  SetIspparamAf_Static(&pIspParams->nispparam_af_static);
  SetIspparamAf_Dynamic(&pIspParams->nispparam_af_dynamic);
  if (bUpdate && eState != SW3A_ENQUEUE_LIST)
  {
    UpdateCoins(eState,
                FALSE,    // bSystem
                FALSE,    // bAfStats
                FALSE,    // bHist
                FALSE,    // bSensor
                bUpdate); // bIsp
  }

  return bUpdate;
}

void Sw3AProcessingCompExt::AfDone()
{
  // pushing Af Releated values in peListQueue (that is FocusLensPos/FocusStats)
  if ((m3Amode == Sw3AMode_Preview) && (m3Astate == Sw3AState_Running))
  {
//     UpdateCoins(SW3A_COMMIT_LIST,
//                 FALSE,  // bSystem
//                 TRUE,   // bAfStats
//                 FALSE,  // bHist
//                 FALSE,  // bSensor
//                 FALSE); // bIsp
  }
}

void Sw3AProcessingCompExt::GetFwStatus()
{
  t_uint32 i;

  for (i = 0; i < fwStatus.size; i++)
  {
    switch (fwStatus.a_list[i].pe_addr)
    {
      case FLADriver_NVMStoredData_s32_NVMInfinityFarEndPos_Byte0:
      {
        fwStatusInfinityFarEnd = fwStatus.a_list[i].pe_data;
        break;
      }
      case FLADriver_NVMStoredData_s32_NVMMacroNearEndPos_Byte0:
      {
        fwStatusMacroNearEnd = fwStatus.a_list[i].pe_data;
        break;
      }

      case FLADriver_NVMStoredData_s32_NVMInfinityHorPos_Byte0:
      {
        fwStatusInfinityHor = fwStatus.a_list[i].pe_data;
        break;
      }

      case FLADriver_NVMStoredData_s32_NVMMacroHorPos_Byte0:
      {
        fwStatusMacroHor = fwStatus.a_list[i].pe_data;
        break;
      }

      case SystemConfig_Status_e_Coin_Status_Byte0:
      {
        systemCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case AFStats_Status_e_Coin_ZoneConfigStatus_Byte0:
      {
        afZoneCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case AFStats_Status_e_Coin_AFStatsExportStatus_Byte0:
      {
        afStatsExportCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case FocusControl_Controls_e_Coin_Control_Byte0:
      {
        focusCtrlCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case HistStats_Status_e_CoinStatus_Byte0:
      {
        histCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0:
      {
        sensorCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0:
      {
        ispCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case Glace_Status_u8_ParamUpdateCount_Byte0:
      {
        glaceParamUpdateCount = fwStatus.a_list[i].pe_data;
        break;
      }
//       case Glace_Status_u8_ControlUpdateCount_Byte0:
//       {
//         glaceControlUpdateCount = fwStatus.a_list[i].pe_data;
//         break;
//       }
      case RunMode_Control_e_Coin_Ctrl_Byte0:
      {
        runmodeCoin = fwStatus.a_list[i].pe_data;
        break;
      }
      case FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0:
      {
        focusParam.lensPos = fwStatus.a_list[i].pe_data;
        break;
      }
      default:
      {
        break;
      }
    }
  }

  fwStatusLensRange = fwStatusMacroNearEnd - fwStatusInfinityFarEnd;

// #ifdef LOG_ENABLE_CONFIGFOCUS
  MSG0("GetFwStatusLensPos\n");
  MSG2("infinityFarEnd = %ld | macroNearEnd = %ld\n", fwStatusInfinityFarEnd, fwStatusMacroNearEnd);
  MSG2("infinityHor = %ld | macroHor = %ld\n", fwStatusInfinityHor, fwStatusMacroHor);
  MSG1("fwStatusLensRange = %ld\n", fwStatusLensRange);
// #endif // LOG_ENABLE_CONFIGFOCUS
}

bool Sw3AProcessingCompExt::SetIspparamIspHistStat_Static(
  OMX_3A_ISPPARAM_STATIC_HISTSTATSTYPE* pIspParamHist)
{
  bool bUpdate = false;

  ClearPageElement(&histStatPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &histStatPeList,
      HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0,
      OMX_CONF_U_TO_F(pIspParamHist->nHistSizeRelativeToFOV_X, OMX_CONF_Q24));
    PushPageElement(
      &histStatPeList,
      HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0,
      OMX_CONF_U_TO_F(pIspParamHist->nHistSizeRelativeToFOV_Y, OMX_CONF_Q24));
    PushPageElement(
      &histStatPeList,
      HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0,
      OMX_CONF_U_TO_F(pIspParamHist->nHistOffsetRelativeToFOV_X, OMX_CONF_Q24));
    PushPageElement(
      &histStatPeList,
      HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0,
      OMX_CONF_U_TO_F(pIspParamHist->nHistOffsetRelativeToFOV_Y, OMX_CONF_Q24));
    PushPageElement(
      &histStatPeList,
      HistStats_Ctrl_e_HistInputSrc_Byte0,
      pIspParamHist->nHistInputSrc);

    PushPageElement(
      &aewbStatsPeList,
      HistStats_Ctrl_ptru32_HistRAddr_Byte0,
      pStatsBufferStorage->ispHistAddr_R);
    PushPageElement(
      &aewbStatsPeList,
      HistStats_Ctrl_ptru32_HistGAddr_Byte0,
      pStatsBufferStorage->ispHistAddr_G);
    PushPageElement(
      &aewbStatsPeList,
      HistStats_Ctrl_ptru32_HistBAddr_Byte0,
      pStatsBufferStorage->ispHistAddr_B);

    ispParams.nispparam_histstats_static = *pIspParamHist;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(histStatPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificScorpio_Static(
  OMX_3A_ISPPARAM_STATIC_SCORPIOTYPE* pIspParamScorpio)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &ispSpecPeList,
      Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0,
      pIspParamScorpio->nEnable);
    PushPageElement(
      &ispSpecPeList,
      Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0,
      pIspParamScorpio->nCoringLevel_Ctrl);

    ispParams.nispparam_ispSpecific.scorpio_static = *pIspParamScorpio;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificChannelGain_Static(
  OMX_3A_ISPPARAM_STATIC_CHANNELGAINTYPE* pIspParamChannelGain)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &ispSpecPeList,
      ChannelGains_combined_f_RedGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamChannelGain->nRedGain, OMX_CONF_Q24));
    PushPageElement(
      &ispSpecPeList,
      ChannelGains_combined_f_GreenInRedGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamChannelGain->nGreenInRedGain, OMX_CONF_Q24));
    PushPageElement(
      &ispSpecPeList,
      ChannelGains_combined_f_GreenInBlueGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamChannelGain->nGreenInBlueGain, OMX_CONF_Q24));
    PushPageElement(
      &ispSpecPeList,
      ChannelGains_combined_f_BlueGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamChannelGain->nBlueGain, OMX_CONF_Q24));

    ispParams.nispparam_ispSpecific.channelGain_static = *pIspParamChannelGain;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificBayerRepair_Static(
  OMX_3A_ISPPARAM_STATIC_BAYERREPAIRTYPE* pIspParamBayerRepair)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &ispSpecPeList,
      BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0,
      pIspParamBayerRepair->nEnable);

    ispParams.nispparam_ispSpecific.bayerRepair_static = *pIspParamBayerRepair;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificBabylon_Static(
  OMX_3A_ISPPARAM_STATIC_BABYLONTYPE* pIspParamBabylon)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &ispSpecPeList,
      Babylon_Ctrl_e_Flag_BabylonEnable_Byte0,
      pIspParamBabylon->nEnable);

    ispParams.nispparam_ispSpecific.babylon_static = *pIspParamBabylon;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificBabylon_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_BABYLONTYPE* pIspParamBabylon)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamBabylon,
         &ispParams.nispparam_ispSpecific.babylon_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_BABYLONTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &ispSpecPeList,
      Babylon_Ctrl_u8_ZipperKill_Byte0,
      pIspParamBabylon->nZipperKill);
    PushPageElement(
      &ispSpecPeList,
      Babylon_Ctrl_u8_Flat_Threshold_Byte0,
      pIspParamBabylon->nFlat_Threshold);

    ispParams.nispparam_ispSpecific.babylon_dynamic = *pIspParamBabylon;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecificSdl_Static(
  OMX_3A_ISPPARAM_STATIC_SDLTYPE* pIspParamSdl)
{
  bool bUpdate = false;

  ClearPageElement(&ispSpecPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &ispSpecPeList,
      SDL_Control_e_SDLMode_Control_Byte0,
      pIspParamSdl->nSDLMode_Control);

      ispParams.nispparam_ispSpecific.sdl_static = *pIspParamSdl;
      bUpdate = true;
  }

  if (bUpdate)
  {
    done(ispSpecPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamIspSpecific(
  OMX_3A_ISPPARAM_ISPSPECIFICTYPE* pIspParam)
{
  bool bUpdate = false;

  // BABYLONTYPE
  bUpdate |= SetIspparamIspSpecificBabylon_Static(&pIspParam->babylon_static);
  bUpdate |= SetIspparamIspSpecificBabylon_Dynamic(&pIspParam->babylon_dynamic);
  // BAYERREPAIRTYPE
  bUpdate |= SetIspparamIspSpecificBayerRepair_Static(&pIspParam->bayerRepair_static);
  // CHANNELGAINTYPE
  bUpdate |= SetIspparamIspSpecificChannelGain_Static(&pIspParam->channelGain_static);
  // SCORPIOTYPE
  bUpdate |= SetIspparamIspSpecificScorpio_Static(&pIspParam->scorpio_static);
  // SDLTYPE
  bUpdate |= SetIspparamIspSpecificSdl_Static(&pIspParam->sdl_static);

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamEdgeEnhance_Static(
  OMX_3A_ISPPARAM_STATIC_EDGEENHANCEMENTTYPE* pIspParamEdgeEnhance)
{
  bool bUpdate = false;

  ClearPageElement(&edgePeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0,
      pIspParamEdgeEnhance->nEnable);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0,
      pIspParamEdgeEnhance->nAdaptiveSharpeningEnable);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_e_Flag_Adsoc_RP_Enable_Byte0,
      pIspParamEdgeEnhance->nRadialPeakingEanble);

    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0,
      pIspParamEdgeEnhance->nEnable);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0,
      pIspParamEdgeEnhance->nAdaptiveSharpeningEnable);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_e_Flag_Adsoc_RP_Enable_Byte0,
      pIspParamEdgeEnhance->nRadialPeakingEanble);

    ispParams.nispparam_edgeEnhancement_static = *pIspParamEdgeEnhance;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(edgePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamEdgeEnhance_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_EDGEENHANCEMENTTYPE* pIspParamEdgeEnhance)
{
  bool bUpdate = false;

  ClearPageElement(&edgePeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamEdgeEnhance,
       &ispParams.nispparam_edgeEnhancement_dynamic,
       sizeof(OMX_3A_ISPPARAM_DYNAMIC_EDGEENHANCEMENTTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Gain_Byte0,
      pIspParamEdgeEnhance->nGain);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Coring_Level_Byte0,
      pIspParamEdgeEnhance->nCoringLevel);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0,
      pIspParamEdgeEnhance->nOverShootGainBright);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0,
      pIspParamEdgeEnhance->nOverShootGainDark);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u16_Lens_Centre_HOffset_Byte0,
      pIspParamEdgeEnhance->nLens_Centre_HOffset);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u16_Lens_Centre_VOffset_Byte0,
      pIspParamEdgeEnhance->nLens_Centre_VOffset);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef0_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Polycoef0);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef1_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Polycoef1);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_COF_Shift_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_COF_Shift);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Unity_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Unity);

    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0,
      pIspParamEdgeEnhance->nGain);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level_Byte0,
      pIspParamEdgeEnhance->nCoringLevel);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0,
      pIspParamEdgeEnhance->nOverShootGainBright);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0,
      pIspParamEdgeEnhance->nOverShootGainDark);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0,
      (t_uint8)0);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u16_Lens_Centre_HOffset_Byte0,
      pIspParamEdgeEnhance->nLens_Centre_HOffset);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u16_Lens_Centre_VOffset_Byte0,
      pIspParamEdgeEnhance->nLens_Centre_VOffset);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef0_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Polycoef0);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef1_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Polycoef1);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_COF_Shift_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_COF_Shift);
    PushPageElement(
      &edgePeList,
      Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Unity_Byte0,
      pIspParamEdgeEnhance->nRadial_Adsoc_RP_Unity);

    ispParams.nispparam_edgeEnhancement_dynamic = *pIspParamEdgeEnhance;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(edgePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamColorConversion_Static(
  OMX_3A_ISPPARAM_STATIC_COLORCONVERSIONTYPE* pIspParamColorConversion)
{
  bool bUpdate = false;

  ClearPageElement(&colorConvPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &colorConvPeList,
      CE_FadeToBlack_0_e_Flag_Disable_Byte0,
      pIspParamColorConversion->nDisable);

    PushPageElement(
      &colorConvPeList,
      CE_FadeToBlack_1_e_Flag_Disable_Byte0,
      pIspParamColorConversion->nDisable);

    ispParams.nispparam_colorconversion_static = *pIspParamColorConversion;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(colorConvPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamColorConversion_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_COLORCONVERSIONTYPE* pIspParamColorConversion)
{
  bool bUpdate = false;

  ClearPageElement(&colorConvPeList);
  if ((memcmp(pIspParamColorConversion,
         &ispParams.nispparam_colorconversion_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_COLORCONVERSIONTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_0_e_Transform_Type_Byte0,
      pIspParamColorConversion->nTransform_Type);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_0_u16_LumaExcursion_Byte0,
      pIspParamColorConversion->nLumaExcursion);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_0_u16_LumaMidpointTimes2_Byte0,
      pIspParamColorConversion->nLumaMidpointTimes2);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_0_u16_ChromaExcursion_Byte0,
      pIspParamColorConversion->nChromaCbExcursion);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_0_u16_ChromaMidpointTimes2_Byte0,
      pIspParamColorConversion->nChromaCbMidpointTimes2);
#if 0
    // New ISP setting (until a value is decided invalidity)
    PushPageElement(
      &colorConvPeList,
      ,
      pIspParamColorConversion->nChromaCrExcursion);
    PushPageElement(
      &colorConvPeList,
      ,
      pIspParamColorConversion->nChromaCrMidpointTimes2);
#endif
  PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_0_u8_Contrast_Byte0,
      pIspParamColorConversion->nContrast);
    PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_0_u8_ColourSaturation_Byte0,
      pIspParamColorConversion->nColourSaturation);
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_0_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_1_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_2_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_3_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_4_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_5_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_6_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_7_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_0_f_StockMatrix_8_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_0_e_BlackAndWhiteControl_Byte0,
      (uint8_t)BlackAndWhiteControl_Disable);
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_0_e_SepiaControl_Byte0,
      (uint8_t)SepiaControl_Disable);
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_0_e_SFXNegativeControl_Byte0,
      (uint8_t)SFXNegativeControl_Disable);

    PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_1_e_Transform_Type_Byte0,
      pIspParamColorConversion->nTransform_Type);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_1_u16_LumaExcursion_Byte0,
      pIspParamColorConversion->nLumaExcursion);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_1_u16_LumaMidpointTimes2_Byte0,
      pIspParamColorConversion->nLumaMidpointTimes2);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_1_u16_ChromaExcursion_Byte0,
      pIspParamColorConversion->nChromaCbExcursion);
    PushPageElement(
      &colorConvPeList,
      CE_CustomTransformOutputSignalRange_1_u16_ChromaMidpointTimes2_Byte0,
      pIspParamColorConversion->nChromaCbMidpointTimes2);
#if 0
    // New ISP setting (until a value is decided invalidity)
    PushPageElement(
      &colorConvPeList,
      ,
      pIspParamColorConversion->nChromaCrExcursion);
    PushPageElement(
      &colorConvPeList,
      ,
      pIspParamColorConversion->nChromaCrMidpointTimes2);
#endif
    PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_1_u8_Contrast_Byte0,
      pIspParamColorConversion->nContrast);
    PushPageElement(
      &colorConvPeList,
      CE_YUVCoderControls_1_u8_ColourSaturation_Byte0,
      pIspParamColorConversion->nColourSaturation);
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_0_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_1_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_2_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW0_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_3_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_4_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_5_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW1_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_6_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_0, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_7_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_1, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      CustomStockMatrix_1_f_StockMatrix_8_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorConversion->nW2_2, OMX_CONF_Q24));
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_1_e_BlackAndWhiteControl_Byte0,
      (uint8_t)BlackAndWhiteControl_Disable);
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_1_e_SepiaControl_Byte0,
      (uint8_t)SepiaControl_Disable);
    PushPageElement(
      &colorConvPeList,
      SpecialEffects_Control_1_e_SFXNegativeControl_Byte0,
      (uint8_t)SFXNegativeControl_Disable);

    ispParams.nispparam_colorconversion_dynamic = *pIspParamColorConversion;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(colorConvPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetGammaConfigtoIsp(
  OMX_3A_CONFIG_GAMMATABLE_TYPE* pGammaParams)
{
  OMX_ERRORTYPE e_ret = OMX_ErrorNone;
  IN0("");
  bool bHasErr = false;
  bool bUpdate = false;
  ClearPageElement(&gammaPeList);
  {
    // Accoding to sw3a_statemachine.cpp , there are bug #111096
      long * lutAddr = (long *) pGammaParams->nGammaG;
      int lutCount = pGammaParams->nsizeOfTable / sizeof(t_uint32) - 1;
      e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_RED_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_RED_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      t_uint32 *pu32_last = (t_uint32 *) &(pGammaParams->nGammaG[lutCount]);
      t_uint16 u16_last = *pu32_last;

      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Red_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Red_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Red_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Red_Byte0,
        u16_last);
      bUpdate = true;
  }
  {
    // Accoding to sw3a_statemachine.cpp , there are bug #111096
      long * lutAddr = (long *) pGammaParams->nGammaB;
      int lutCount = pGammaParams->nsizeOfTable / sizeof(t_uint32) - 1;
      e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_GREEN_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      t_uint32 *pu32_last = (t_uint32 *) &(pGammaParams->nGammaB[lutCount]);
      t_uint16 u16_last = *pu32_last;
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIR_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIR_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIR_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIR_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_GreenGIB_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_GreenGIB_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_GreenGIB_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_GreenGIB_Byte0,
        u16_last);
      bUpdate = true;
  }
  {
    // Accoding to sw3a_statemachine.cpp , there are bug #111096
      long * lutAddr = (long *) pGammaParams->nGammaR;
      int lutCount = pGammaParams->nsizeOfTable / sizeof(t_uint32) - 1;
      e_ret = MMIO_Camera::ispVectorWrite(CE0_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE0_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_SHARP_MEM_LUT_BLUE_ELT_OFFSET,   lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      e_ret = MMIO_Camera::ispVectorWrite(CE1_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET, lutAddr, lutCount);
        bHasErr |= (e_ret != OMX_ErrorNone);
      t_uint32 *pu32_last = (t_uint32 *) &(pGammaParams->nGammaR[lutCount]);
      t_uint16 u16_last = *pu32_last;
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_Sharp_Lst_Blue_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_0_u16_UnSharp_Lst_Blue_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_Sharp_Lst_Blue_Byte0,
        u16_last);
      PushPageElement(
        &gammaPeList,
        CE_GammaLastPixelValueControl_1_u16_UnSharp_Lst_Blue_Byte0,
        u16_last);
      bUpdate = true;
  }

  if (bUpdate)
  {
    PushPageElement(
      &gammaPeList,
      CE_GammaControl_0_e_GammaCurve_Byte0,
      (uint8_t)GammaCurve_Custom);
    PushPageElement(
      &gammaPeList,
      CE_GammaControl_1_e_GammaCurve_Byte0,
      (uint8_t)GammaCurve_Custom);
    done(gammaPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
    gammaTable = *pGammaParams;
  }
  OUT0("");

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetGammaConfig(
  OMX_3A_CONFIG_GAMMATABLE_TYPE* pGammaType,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  // GAMMA TABLE
  if(m3Aconfigmode == Sw3AConfigMode_Static)
  {
    bUpdate = SetGammaConfigtoIsp(pGammaType);
  }
  else if(m3Aconfigmode == Sw3AConfigMode_Dynamic)
  {
    OMX_STE_CAPTUREPARAMETERSTYPE *pSteExtraData;
    pSteExtraData = (OMX_STE_CAPTUREPARAMETERSTYPE *)getSteExtraCaptureParameters(false);
    memcpy(pSteExtraData->nGreenGammaLUT,
      pGammaType->nGammaG,
      (OMX_STE_GAMMA_LUT_SAMPLE_COUNT) * sizeof(t_uint32));
    memcpy(pSteExtraData->nBlueGammaLUT,
      pGammaType->nGammaB,
      (OMX_STE_GAMMA_LUT_SAMPLE_COUNT) * sizeof(t_uint32));
    memcpy(pSteExtraData->nRedGammaLUT,
      pGammaType->nGammaR,
      (OMX_STE_GAMMA_LUT_SAMPLE_COUNT) * sizeof(t_uint32));
    pSteExtraData->bValidGamma = OMX_TRUE;
    gammaTable = *pGammaType;
    bUpdate = true;
  }
  return bUpdate;
}

bool Sw3AProcessingCompExt::SetLscConfigtoIsp()
{
  bool bUpdate = false;

  IN0("");
  if((pCastPos0GridsData != NULL) ||
     (pCastPos1GridsData != NULL) ||
     (pCastPos2GridsData != NULL) ||
     (pCastPos3GridsData != NULL))
  {
    /* Write Gridiron grids to ISP memory */
    typedef struct
    {
        const unsigned long       GridironCastAddr;
        const unsigned long*      GridironSaveAddr;
    } t_GridironDesc;
    t_GridironDesc iGridironDesc[4] =
    {
        { GRIDIRON_MEM_ELT_CAST0_OFFSET, pCastPos0GridsData},
        { GRIDIRON_MEM_ELT_CAST1_OFFSET, pCastPos1GridsData},
        { GRIDIRON_MEM_ELT_CAST2_OFFSET, pCastPos2GridsData},
        { GRIDIRON_MEM_ELT_CAST3_OFFSET, pCastPos3GridsData}
    };

    for(int i=0; i<4; i++)
    {
        OMX_ERRORTYPE     ret = OMX_ErrorNone;
        CTuningBinData*   pGridironData = NULL;
        // Write Gridion grid to ISP
        ret = MMIO_Camera::ispVectorWrite(iGridironDesc[i].GridironCastAddr,
                                          (long int*)iGridironDesc[i].GridironSaveAddr,
                                          OMX_3A_CONFIG_LENS_HEIGHT * OMX_3A_CONFIG_LENS_WIDTH * 2);
        if (ret != OMX_ErrorNone) {
            MSG1("CAM_SM::Gridiron: failed to write grid to ISP memory err=%d\n", ret);
            DBC_ASSERT(0);
        }
    }
    pCastPos0GridsData = NULL;
    pCastPos1GridsData = NULL;
    pCastPos2GridsData = NULL;
    pCastPos3GridsData = NULL;
    bUpdate            = true;
  }
  OUT0("");
  return bUpdate;
}

bool Sw3AProcessingCompExt::SetLscConfig(
  OMX_3A_CONFIG_LSCTABLE_TYPE* pLscParams,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  // LSC TABLE
  lscTable = *pLscParams;
  if(m3Aconfigmode == Sw3AConfigMode_Static)
  {
    pCastPos0GridsData = &lscTable.nCastPos0GridsData[0];
    pCastPos1GridsData = &lscTable.nCastPos1GridsData[0];
    pCastPos2GridsData = &lscTable.nCastPos2GridsData[0];
    pCastPos3GridsData = &lscTable.nCastPos3GridsData[0];
    bUpdate            = SetLscConfigtoIsp();
  }
  else if((m3Aconfigmode == Sw3AConfigMode_Dynamic) && (bLscInit == false))
  {
    pCastPos0GridsData = &lscTable.nCastPos0GridsData[0];
    pCastPos1GridsData = &lscTable.nCastPos1GridsData[0];
    pCastPos2GridsData = &lscTable.nCastPos2GridsData[0];
    pCastPos3GridsData = &lscTable.nCastPos3GridsData[0];
    bUpdate            = true;
    bValidLsc          = OMX_TRUE;
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetLinearizerConfigtoIsp()
{
  bool bUpdate = false;

  IN0("");
  if((pSdlR != NULL) || (pSdlGr != NULL) || (pSdlGb != NULL) || (pSdlB != NULL))
  {
    typedef struct
    {
      const unsigned long       IspLutAddr;
      const t_uint16            LastEltPE;
      const unsigned long*      IspLutSaveAddr;
    } t_LinearisationDesc;

    const t_LinearisationDesc iLinearizationDesc[4] =
    {
      { LINEAR_MEM_LUT_RED_ELT_OFFSET,  SDL_ELT_u16_LastElementinLUT_RED_Byte0,  pSdlR   },
      { LINEAR_MEM_LUT_GINR_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_GIR_Byte0,  pSdlGr  },
      { LINEAR_MEM_LUT_GINB_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_GIB_Byte0,  pSdlGb  },
      { LINEAR_MEM_LUT_BLUE_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_BLUE_Byte0, pSdlB   }
    };

    ClearPageElement(&sdlPeList);
    for(int i=0; i<4; i++)
    {
      OMX_ERRORTYPE     ret = OMX_ErrorNone;
      CTuningBinData*   pLinearizationData = NULL;
      t_uint32          iLastEltValue = 0;
      // Write Linearization LUT to ISP (Last element not written)
      ret = MMIO_Camera::ispVectorWrite( iLinearizationDesc[i].IspLutAddr,
                                         (long int*)iLinearizationDesc[i].IspLutSaveAddr,
                                         OMX_3A_CONFIG_SDL_SIZE - 1); // TODO:temporary
      if (ret != OMX_ErrorNone) {
        MSG1("CAM_SM::Linearization: failed write LUT to ISP memory err=%d\n", ret);
        DBC_ASSERT(0);
      }

      // Last element of LUT must be written to a Page Element
      iLastEltValue = *(t_uint32*)((t_uint32)iLinearizationDesc[i].IspLutSaveAddr +
        (OMX_3A_CONFIG_SDL_SIZE - 1) * sizeof(t_uint32));//TODO: temporary
      PushPageElement(
        &sdlPeList,
        iLinearizationDesc[i].LastEltPE,
        (t_uint16)iLastEltValue);
    }
    done(sdlPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
    pSdlR            = NULL;
    pSdlGr           = NULL;
    pSdlGb           = NULL;
    pSdlB            = NULL;
    bUpdate          = true;
  }
  OUT0("");
  return bUpdate;
}

bool Sw3AProcessingCompExt::SetLinearizerConfig(
  OMX_3A_CONFIG_LINEARIZERTABLE_TYPE* pLinearizerParams,
  t_sw3A_LoopState eState)
{
  bool bUpdate = false;
  // LINEARIZERTABLE TABLE
  linearizerTable = *pLinearizerParams;
  if(m3Aconfigmode == Sw3AConfigMode_Static)
  {
    pSdlR            = &linearizerTable.nSdlR[0];
    pSdlGr           = &linearizerTable.nSdlGr[0];
    pSdlGb           = &linearizerTable.nSdlGb[0];
    pSdlB            = &linearizerTable.nSdlB[0];
    bUpdate          = SetLinearizerConfigtoIsp();
  }
  else if((m3Aconfigmode == Sw3AConfigMode_Dynamic) && (bLinearizerInit == false))
  {
    pSdlR            = &linearizerTable.nSdlR[0];
    pSdlGr           = &linearizerTable.nSdlGr[0];
    pSdlGb           = &linearizerTable.nSdlGb[0];
    pSdlB            = &linearizerTable.nSdlB[0];
    bUpdate          = true;
    bValidLinearizer = OMX_TRUE;
  }

  return bUpdate;

}

bool Sw3AProcessingCompExt::SetIspparamLensShading_Static(
  OMX_3A_ISPPARAM_STATIC_LENSSHADINGCORRECTIONTYPE* pIspParamLensShading)
{
  bool bUpdate = false;

  ClearPageElement(&lensSharedPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    //add later
    PushPageElement(
      &lensSharedPeList,
      GridironControl_e_Flag_Enable_Byte0,
      pIspParamLensShading->nEnable);
    PushPageElement(
      &lensSharedPeList,
      GridironControl_u16_GridWidth_Byte0,
      pIspParamLensShading->nGridWidth);
    PushPageElement(
      &lensSharedPeList,
      GridironControl_u16_GridHeight_Byte0,
      pIspParamLensShading->nGridHeight);
    PushPageElement(
      &lensSharedPeList,
      GridironControl_e_PixelOrder_Byte0,
      pIspParamLensShading->nPixelOrder);
    PushPageElement(
      &lensSharedPeList,
      GridironControl_u8_Active_ReferenceCast_Count_Byte0,
      pIspParamLensShading->nActiveReferenceCastCount);
    PushPageElement(
      &lensSharedPeList,
      GridironControl_f_CastPosition0_Byte0,
      OMX_CONF_S_TO_F(pIspParamLensShading->nCastPosition0, OMX_CONF_Q20));
    PushPageElement(
      &lensSharedPeList,
      GridironControl_f_CastPosition1_Byte0,
      OMX_CONF_S_TO_F(pIspParamLensShading->nCastPosition1, OMX_CONF_Q20));
    PushPageElement(
      &lensSharedPeList,
      GridironControl_f_CastPosition2_Byte0,
      OMX_CONF_S_TO_F(pIspParamLensShading->nCastPosition2, OMX_CONF_Q20));
    PushPageElement(
      &lensSharedPeList,
      GridironControl_f_CastPosition3_Byte0,
      OMX_CONF_S_TO_F(pIspParamLensShading->nCastPosition3, OMX_CONF_Q20));

    ispParams.nispparam_lensshad_static = *pIspParamLensShading;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(lensSharedPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamLensShading_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_LENSSHADINGCORRECTIONTYPE* pIspParamLensShading)
{
  bool bUpdate = false;

  ClearPageElement(&lensSharedPeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamLensShading,
         &ispParams.nispparam_lensshad_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_LENSSHADINGCORRECTIONTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &lensSharedPeList,
      GridironControl_f_LiveCast_Byte0,
      OMX_CONF_S_TO_F(pIspParamLensShading->nLiveCast, OMX_CONF_Q20));

    ispParams.nispparam_lensshad_dynamic = *pIspParamLensShading;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(lensSharedPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamNoiseReduction_Static(
  OMX_3A_ISPPARAM_STATIC_NOISEREDUCTIONTYPE* pIspParamNoiseReduction)
{
  bool bUpdate = false;

  ClearPageElement(&noiserPeList);

  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &noiserPeList,
      DusterControl_e_Flag_DusterEnable_Byte0,
      pIspParamNoiseReduction->nEnable);

    ispParams.nispparam_noiser_static = *pIspParamNoiseReduction;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(noiserPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamNoiseReduction_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_NOISEREDUCTIONTYPE* pIspParamNoiseReduction)
{
  bool bUpdate = false;

  ClearPageElement(&noiserPeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamNoiseReduction,
         &ispParams.nispparam_noiser_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_NOISEREDUCTIONTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &noiserPeList,
      DusterControl_u16_FrameSigma_Byte0,
      pIspParamNoiseReduction->nFrameSigma);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_ScytheControl_hi_Byte0,
      pIspParamNoiseReduction->nScytheControl_hi);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_ScytheControl_lo_Byte0,
      pIspParamNoiseReduction->nScytheControl_lo);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_CenterCorrectionSigmaFactor_Byte0,
      pIspParamNoiseReduction->nCenterCorrectionSigmaFactor);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_RingCorrectionNormThr_Byte0,
      pIspParamNoiseReduction->nRingCorrectionNormThr);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_GaussianWeight_Byte0,
      (uint8_t)128);
    PushPageElement(
      &noiserPeList,
      DusterControl_u8_SigmaWeight_Byte0,
      (uint8_t)0);

    ispParams.nispparam_noiser_dynamic = *pIspParamNoiseReduction;
    bUpdate = true;
  }
  if (bUpdate)
  {
    done(noiserPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamColorCorrection_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_COLORCORRECTIONTYPE* pIspParamColorCorrection)
{
  bool bUpdate = false;

  ClearPageElement(&colorMatrixPeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamColorCorrection,
         &ispParams.nispparam_colormatrix_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_COLORCORRECTIONTYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    // setting ColorMatrix
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_RedInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_GreenInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_BlueInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_RedInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_RedInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0,
      pIspParamColorCorrection->nOffset_R);
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0,
      pIspParamColorCorrection->nOffset_G);
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0,
      pIspParamColorCorrection->nOffset_B);

    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_RedInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_GreenInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_BlueInRed_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInRed, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_RedInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInGreen, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_RedInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nRedInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nGreenInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0,
      OMX_CONF_S_TO_F(pIspParamColorCorrection->nBlueInBlue, OMX_CONF_Q24));
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0,
      pIspParamColorCorrection->nOffset_R);
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0,
      pIspParamColorCorrection->nOffset_G);
    PushPageElement(
      &colorMatrixPeList,
      CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0,
      pIspParamColorCorrection->nOffset_B);

    ispParams.nispparam_colormatrix_dynamic = *pIspParamColorCorrection;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(colorMatrixPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamWhiteBalance_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_WHITEBALANCETYPE* pIspParamWhiteBalance)
{
  bool bUpdate = false;

  ClearPageElement(&whiteBalancePeList);
  // If there is a difference share, dynamicParam is always set.
  if ((memcmp(pIspParamWhiteBalance,
         &ispParams.nispparam_wb_dynamic,
         sizeof(OMX_3A_ISPPARAM_DYNAMIC_WHITEBALANCETYPE)) != 0) ||
     (m3Aconfigmode == Sw3AConfigMode_Static))
  {
    PushPageElement(
      &whiteBalancePeList,
      WhiteBalanceControl_f_RedManualGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamWhiteBalance->nRedManualgain, OMX_CONF_Q24));
    PushPageElement(
      &whiteBalancePeList,
      WhiteBalanceControl_f_GreenManualGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamWhiteBalance->nGreenManualgain, OMX_CONF_Q24));
    PushPageElement(
      &whiteBalancePeList,
      WhiteBalanceControl_f_BlueManualGain_Byte0,
      OMX_CONF_U_TO_F(pIspParamWhiteBalance->nBlueManualgain, OMX_CONF_Q24));

    ispParams.nispparam_wb_dynamic = *pIspParamWhiteBalance;
    bUpdate = true;
  }

  if (bUpdate)
  {
    done(whiteBalancePeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamBlackLevel_Static(
  OMX_3A_ISPPARAM_STATIC_BLACKLEVELCORRECTIONTYPE* pIspParamBlackLevel)
{
  bool bUpdate = false;

  ClearPageElement(&blackLevelPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
      PushPageElement(
        &blackLevelPeList,
        RSO_Control_e_Flag_EnableRSO_Byte0,
        pIspParamBlackLevel->nEnable);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_XCoefGr_Byte0,
        pIspParamBlackLevel->nXCoefGr);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_YCoefGr_Byte0,
        pIspParamBlackLevel->nYCoefGr);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_XCoefR_Byte0,
        pIspParamBlackLevel->nXCoefR);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_YCoefR_Byte0,
        pIspParamBlackLevel->nYCoefR);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_XCoefB_Byte0,
        pIspParamBlackLevel->nXCoefB);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_YCoefB_Byte0,
        pIspParamBlackLevel->nYCoefB);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_XCoefGb_Byte0,
        pIspParamBlackLevel->nXCoefGb);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u32_YCoefGb_Byte0,
        pIspParamBlackLevel->nYCoefGb);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_DcTermGr_Byte0,
        pIspParamBlackLevel->nDcTermGr);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_DcTermR_Byte0,
        pIspParamBlackLevel->nDcTermR);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_DcTermB_Byte0,
        pIspParamBlackLevel->nDcTermB);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_DcTermGb_Byte0,
        pIspParamBlackLevel->nDcTermGb);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_XSlantOrigin_Byte0,
        pIspParamBlackLevel->nXSlantOrigin);
      PushPageElement(
        &blackLevelPeList,
        RSO_DataCtrl_u16_YSlantOrigin_Byte0,
        pIspParamBlackLevel->nYSlantOrigin);

      ispParams.nispparam_black_static = *pIspParamBlackLevel;
      bUpdate = true;
  }

  if (bUpdate)
  {
    done(blackLevelPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamAewb_Static(
  OMX_3A_ISPPARAM_STATIC_AEWBTYPE* pIspParamsAewb)
{
  bool bUpdate = false;

  ClearPageElement(&aewbStatsPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_HGridSize_Byte0,
      (t_uint32)pIspParamsAewb->nHGridSize);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_VGridSize_Byte0,
      (t_uint32)pIspParamsAewb->nVGridSize);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_e_GlaceOperationMode_Control_Byte0,
      (t_uint32)GlaceOperationMode_e_Continuous);

    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_ptrGlace_Statistics_Byte0,
      pStatsBufferStorage->ispGlaceStatsAddr);
    PushPageElement(
      &aewbStatsPeList,
      FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte0,
      pStatsBufferStorage->ispSensorParamAddr);

    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_f_HBlockSizeFraction_Byte0,
      OMX_CONF_U_TO_F(pIspParamsAewb->nHBlockSizeFraction, OMX_CONF_Q24));
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_f_VBlockSizeFraction_Byte0,
      OMX_CONF_U_TO_F(pIspParamsAewb->nVBlockSizeFraction, OMX_CONF_Q24));
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_f_HROIStartFraction_Byte0,
      OMX_CONF_U_TO_F(pIspParamsAewb->nHROIStartFraction, OMX_CONF_Q24));
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_f_VROIStartFraction_Byte0,
      OMX_CONF_U_TO_F(pIspParamsAewb->nVROIStartFraction, OMX_CONF_Q24));
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_RedSaturationLevel_Byte0,
      (t_uint32)pIspParamsAewb->nRedSaturationLevel);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_GreenSaturationLevel_Byte0,
      (t_uint32)pIspParamsAewb->nGreenSaturationLevel);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_BlueSaturationLevel_Byte0,
      (t_uint32)pIspParamsAewb->nBlueSaturationLevel);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_e_GlaceDataSource_Byte0,
      (t_uint32)pIspParamsAewb->nGlaceDataSource);
    // Update Glace (no needed ?)
//     PushPageElement(
//       &aewbStatsPeList,
//       Glace_Control_u8_ControlUpdateCount_Byte0,
//       (t_uint32)++glaceControlUpdateCount);
    PushPageElement(
      &aewbStatsPeList,
      Glace_Control_u8_ParamUpdateCount_Byte0,
      (t_uint32)++glaceParamUpdateCount);
    ispParams.nispparam_aewb_static = *pIspParamsAewb;
    bUpdate = true;
    MSG3("SetIspparamAewb static HGridSize = %d | VGridSize = %d\n"
         , pIspParamsAewb->nHGridSize
         , pIspParamsAewb->nVGridSize);
  }

  if (bUpdate)
  {
    done(aewbStatsPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamAf_Static(
  OMX_3A_ISPPARAM_STATIC_AFTYPE* pIspParamsAf)
{
  bool bUpdate = false;

  ClearPageElement(&afStatsPeList);
  if((m3Aconfigmode == Sw3AConfigMode_Static) || (bIspParmInit == false))
  {
    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_e_AFStats_WindowsSystem_Control_Byte0,
      (t_uint32)AFStats_WindowsSystem_e_AF_HOST_SYSTEM);

    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_pu32_HostAssignedAddr_Byte0,
      (t_uint32) pStatsBufferStorage->ispAfStatsAddr);
    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0,
      (t_uint32)AFStats_HostCmd_e_REQ_STATS_CONTINUOUS_AND_VALID);

    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_u8_CoringValue_Byte0,
      (t_uint32)pIspParamsAf->nCoringValue);
// TODO: Because a value is not decided, I invalidate it.
//    PushPageElement(
//      &afStatsPeList,
//      AFStats_Controls_u8_HRatioNum_Byte0,
//      (t_uint32)pIspParamsAf->nHRatioNum);
//    PushPageElement(
//      &afStatsPeList,
//      AFStats_Controls_u8_HRatioDen_Byte0,
//      (t_uint32)pIspParamsAf->nHRatioDen);
//    PushPageElement(
//      &afStatsPeList,
//      AFStats_Controls_u8_VRatioNum_Byte0,
//      (t_uint32)pIspParamsAf->nVRatioNum);
//    PushPageElement(
//      &afStatsPeList,
//      AFStats_Controls_u8_VRatioDen_Byte0,
//      (t_uint32)pIspParamsAf->nVRatioDen);
    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_u8_HostActiveZonesCounter_Byte0,
      (t_uint32)pIspParamsAf->nHostActiveZonesCount);
    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_e_Flag_AbsSquareEnabled_Byte0,
      (t_uint32)pIspParamsAf->nFlag_AbsSquareEnabled);
    PushPageElement(
      &afStatsPeList,
      AFStats_Controls_e_Flag_ReducedZoneSetup_Byte0,
      (t_uint32)pIspParamsAf->nFlag_ReducedZoneSetup);

    ispParams.nispparam_af_static = *pIspParamsAf;
    bUpdate = true;
    MSG0("SetIspParamAf static2\n");
  }

  if (bUpdate)
  {
    done(afStatsPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetIspparamAf_Dynamic(
  OMX_3A_ISPPARAM_DYNAMIC_AFTYPE* pIspParamsAf)
{
  bool bUpdate = false;

  // TODO: The invalid data do not set it.(TENTATIVE)
  OMX_3A_ISPPARAM_DYNAMIC_AFTYPE IspParamsAF_Null;

  memset(&IspParamsAF_Null, 0, sizeof(OMX_3A_ISPPARAM_DYNAMIC_AFTYPE));

  if (memcmp(pIspParamsAf, &IspParamsAF_Null, sizeof(OMX_3A_ISPPARAM_DYNAMIC_AFTYPE)) == 0)
  {
    MSG0("SetIspparamAf_Dynamic Data is NULL!!\n");
  }
  else
  {
    ClearPageElement(&afStatsPeList);
    // If there is a difference share, dynamicParam is always set.
    if ((memcmp(pIspParamsAf,
           &ispParams.nispparam_af_dynamic,
           sizeof(OMX_3A_ISPPARAM_DYNAMIC_AFTYPE)) != 0) ||
       (m3Aconfigmode == Sw3AConfigMode_Static))
    {
      t_uint32 i;
      t_uint16 afFrameAddrShift;
      t_uint16 result1, result2, result3, result4, result5;
      float startX, endX, startY, endY;
      // AF Frame Setting.
      for (i = 0; i < OMX_3A_OTHER_STATSAF_WINDOW_NUMBER_MAX; i++)
      {
        afFrameAddrShift = 0x40 * i;
        result1 = (AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartX_PER_wrt_WOIWidth_Byte0 +
          afFrameAddrShift);
        result2 = (AFStats_HostZoneConfigPercentage_0_f_HostAFZoneStartY_PER_wrt_WOIHeight_Byte0 + 
          afFrameAddrShift);
        result3 = (AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndX_PER_wrt_WOIWidth_Byte0 + 
          afFrameAddrShift);
        result4 = (AFStats_HostZoneConfigPercentage_0_f_HostAFZoneEndY_PER_wrt_WOIHeight_Byte0 +
          afFrameAddrShift);
        result5 = (AFStats_HostZoneConfigPercentage_0_e_Flag_Enabled_Byte0 + afFrameAddrShift);

        startX = OMX_CONF_U_TO_F(pIspParamsAf->nHostAFZone[i].nHostAFZoneStartX, OMX_CONF_Q8);
        startY = OMX_CONF_U_TO_F(pIspParamsAf->nHostAFZone[i].nHostAFZoneStartY, OMX_CONF_Q8);
        endX = OMX_CONF_U_TO_F(pIspParamsAf->nHostAFZone[i].nHostAFZoneEndX, OMX_CONF_Q8);
        endY = OMX_CONF_U_TO_F(pIspParamsAf->nHostAFZone[i].nHostAFZoneEndY, OMX_CONF_Q8);

        PushPageElement(
          &afStatsPeList,
          result1,
          startX);
        PushPageElement(
          &afStatsPeList,
          result2,
          startY);
        PushPageElement(
          &afStatsPeList,
          result3,
          endX);
        PushPageElement(
          &afStatsPeList,
          result4,
          endY);
        PushPageElement(
          &afStatsPeList,
          result5,
          (t_uint32)pIspParamsAf->nHostAFZone[i].nEnable);
      }

      PushPageElement(
        &afStatsPeList,
        AFStats_Controls_e_Flag_HostZoneSetupInPercentage_Byte0,
        (t_uint32)TRUE);

      afZoneCoin = 1 - afZoneCoin;
      PushPageElement(
        &afStatsPeList,
        AFStats_Controls_e_Coin_ZoneConfigCmd_Byte0,
        afZoneCoin);

      ispParams.nispparam_af_dynamic = *pIspParamsAf;
      bUpdate = true;
      MSG0("SetIspParamAf dynamic\n");
    }

    if (bUpdate)
    {
      done(afStatsPeList, SW3A_ENQUEUE_LIST, NULL, NULL, NULL);
    }
  }

  return bUpdate;
}

bool Sw3AProcessingCompExt::SetFocusConfig(
  OMX_3A_CONFIG_FOCUSTYPE* pFocusParams, t_sw3A_LoopState eState)
{
  bool bUpdate = false;

#ifdef LOG_ENABLE_CONFIGFOCUS
  MSG2("SetFocusConfig: Type = %d | Val = %d\n",
    pFocusParams->lensSettingType, pFocusParams->lensPos);
#endif // LOG_ENABLE_CONFIGFOCUS

  ClearPageElement(&focusPeList);
  PushPageElement(
    &focusPeList,
    FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0,
    (t_uint32)FALSE);

  if (pFocusParams->lensSettingType == OMX_3A_CONFIG_LENS_SET)
  {
#ifdef LOG_ENABLE_CONFIGFOCUS
    MSG0("OMX_3A_CONFIG_LENS_SET\n");
#endif // LOG_ENABLE_CONFIGFOCUS

    PushPageElement(
      &focusPeList,
      FLADriver_LLLCtrlStatusParam_u16_TarSetPos_Byte0,
      pFocusParams->lensPos);
    PushPageElement(
      &focusPeList,
      FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
      (t_uint32)FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION);
  }
  else
  {
    PushPageElement(
      &focusPeList,
      FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0,
      pFocusParams->lensPos);

    if (pFocusParams->lensSettingType == OMX_3A_CONFIG_LENS_MOVE_INF)
    {
#ifdef LOG_ENABLE_CONFIGFOCUS
      MSG0("OMX_3A_CONFIG_LENS_MOVE_INF\n");
#endif // LOG_ENABLE_CONFIGFOCUS
      PushPageElement(
        &focusPeList,
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        (t_uint32)FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY);
    }
    else
    {
#ifdef LOG_ENABLE_CONFIGFOCUS
      MSG0("OMX_3A_CONFIG_LENS_MOVE_MACRO\n");
#endif // LOG_ENABLE_CONFIGFOCUS
      PushPageElement(
        &focusPeList,
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        (t_uint32)FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO);
    }
  }
  focusCtrlCoin = 1 - focusCtrlCoin;
  PushPageElement(
    &focusPeList,
    FocusControl_Controls_e_Coin_Control_Byte0,
    focusCtrlCoin);

  // hold current state
  focusParam = *pFocusParams;
  focusParam.lensIsReady = FALSE; // Because I move a lens, I make Ready flag FALSE
#ifdef LOG_ENABLE_CONFIGFOCUS
  MSG2("current lensPos = %lu | lensIsReady = %d\n", focusParam.lensPos, focusParam.lensIsReady);
#endif // LOG_ENABLE_CONFIGFOCUS

  done(focusPeList, eState, NULL, NULL, NULL);
  bUpdate = true;

  return bUpdate;
}

void Sw3AProcessingCompExt::SetLensIsReadyFlg(
  bool flg)
{
#ifdef LOG_ENABLE_CONFIGFOCUS
  MSG2("SetLensIsReadyFlg lensIsReady current = %d | set = %d\n", focusParam.lensIsReady, flg);
#endif // LOG_ENABLE_CONFIGFOCUS

  focusParam.lensIsReady = flg;
}

void Sw3AProcessingCompExt::SetNVMData(t_uint8* data)
{
  memcpy(nvmData,
    data,
    sizeof(nvmData));
  // TODO: should use more proper operation
  if (strncmp((const char*)nvmData, "KMO08BN0", 8) == 0 ||
      strncmp((const char*)nvmData, "SOI08BN1", 8) == 0)
  {
    sensorName = OMX_3A_SENSOR_NAME_IMX111;
  }
  else if (strncmp((const char*)nvmData, "KMO05BN0", 8) == 0 ||
           strncmp((const char*)nvmData, "STW05BN0", 8) == 0)
  {
    sensorName = OMX_3A_SENSOR_NAME_IMX072;
  }
  ((CFramerateExt*)(mSM->pFramerate))->setSensorName(sensorName);
  ((CFramerateExt*)(mSM->pFramerate))->setSensorUsageMode(OMX_3A_SENSORMODE_USAGE_VF); // default VF
};

void Sw3AProcessingCompExt::ClearPageElement(
  t_sw3A_PageElementList* pList)
{
  pList->size = 0;
}

void Sw3AProcessingCompExt::PushPageElement(
  t_sw3A_PageElementList* pList,
  t_uint16 addr,
  t_uint32 data)
{
  pList->a_list[pList->size].pe_addr = addr;
  pList->a_list[pList->size].pe_data = data;
  pList->size++;
  return;
}

void Sw3AProcessingCompExt::PushPageElement(
  t_sw3A_PageElementList* pList,
  t_uint16 addr,
  t_sint16 data)
{
  pList->a_list[pList->size].pe_addr = addr;
  pList->a_list[pList->size].pe_data = data;
  pList->size++;
  return;
}

void Sw3AProcessingCompExt::PushPageElement(
  t_sw3A_PageElementList* pList,
  t_uint16 addr,
  t_uint16 data)
{
  pList->a_list[pList->size].pe_addr = addr;
  pList->a_list[pList->size].pe_data = data;
  pList->size++;
  return;
}

void Sw3AProcessingCompExt::PushPageElement(
  t_sw3A_PageElementList* pList,
  t_uint16 addr,
  t_uint8 data)
{
  pList->a_list[pList->size].pe_addr = addr;
  pList->a_list[pList->size].pe_data = data;
  pList->size++;
  return;
}

void Sw3AProcessingCompExt::PushPageElement(
  t_sw3A_PageElementList* pList,
  t_uint16 addr,
  float data)
{
  pList->a_list[pList->size].pe_addr = addr;
  pList->a_list[pList->size].pe_data = *((t_uint32*)&data);
  pList->size++;
  return;
}

/** ISP info callback hook implementation */
static void sw3A_ispctlInfo(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd)
{
    ASYNC_IN0("\n");
    Sw3AProcessingComp *sw3Acomp = (Sw3AProcessingComp *) ctxtHnd;
    CAM_SM *camSm = sw3Acomp->getSM();
    CSelfTest *pSelfTest = camSm->pSelfTest;
    if(OMX_TRUE == pSelfTest->pTesting->bEnabled) {
        if (infoID == ISP_FLADRIVER_LENS_STOP) {
            s_scf_event event;
            event.sig = EVT_ISPCTL_INFO_SIG;
            event.type.ispctlInfo.info_id = infoID;
            camSm->ProcessEvent(&event);
        }
    } else {
        if (infoID != ISP_FLADRIVER_LENS_STOP) {
        IMG_TIME_LOG((infoID == ISP_AUTOFOCUS_STATS_READY) ? IMG_TIME_3A_STATS_AF_READY : IMG_TIME_3A_STATS_AEW_READY);
            (void) sw3Acomp->run3A(infoID);
        }
    }
    ASYNC_OUT0("\n");
    return;
}
static void callBackLensStop(
  enum e_ispctlInfo infoID,
  t_ispctlEventClbkCtxtHnd ctxtHnd)
{
#ifdef LOG_ENABLE_CONFIGFOCUS
  MSG0("callBackLensStop(Extension)\n");
#endif // LOG_ENABLE_CONFIGFOCUS

  Sw3AProcessingCompExt *sw3AcompExt = (Sw3AProcessingCompExt *) ctxtHnd;

  sw3AcompExt->SetLensIsReadyFlg(TRUE);
}
