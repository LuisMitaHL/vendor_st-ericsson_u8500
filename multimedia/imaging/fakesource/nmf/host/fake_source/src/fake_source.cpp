/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#include <fake_source.nmf>
#include "OMX_Core.h"
#include "ENS_Component_Fsm.h"
#include "cm/inc/cm_macros.h"
#include "OMX_Index.h"
#include "OMX_Core.h"
#include "armnmf_dbc.h"

#ifdef __SYMBIAN32__
	#include <string.h>
	#include <openmax/il/shai/OMX_Symbian_IVCommonExt.h>
	#include <openmax/il/shai/OMX_Symbian_CameraExt.h>
	#include <stdlib.h> // for atoi
#else
	#include <OMX_Symbian_CameraExt_Ste.h>
#endif

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x30
#undef OMXCOMPONENT
#define OMXCOMPONENT "FAKE_SOURCE_HOST"

#include "osi_trace.h"
#include <los/api/los_api.h>

#include <stdio.h>


//#define FAKE_SOURCE_TRACE_SLEEP
//#define FAKE_SOURCE_TRACE_RETURNBUFFER

void fake_source::start(void)
{
	IN0("");
	//LOS_Log("fake_source::start \n", NULL , NULL, NULL, NULL, NULL, NULL);
	fifoSize = 0;
	bufferCount = 0;
	Width		= 0;
	Height		= 0;
	SliceHeight	= 0;
	Stride		= 0;
	Format		= 0;
	BufferSize	= 0;
	FrameSize	= 0;
	FrameSizeUpdateDueToStatusLine = 0;
	Domaine		= 0;

	fileName_data[0] = 0;
	fileHandle_data = NULL;
	whole_file_data_handle = 0;
	fileSize_data = 0;
	fileSizeRead_data = 0;
	memory_handle_extradata = 0;
	fileSize_extradata = 0;
	extraDataPointer = NULL;

	fileName_extradata[0] = 0;
	fileHandle_extradata = NULL;
	pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE = NULL;
    pElement_IFM_DAMPERS_DATA_TYPE = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription = NULL;
	pElement_OMX_SYMBIAN_ExtraDataVideoStabilization = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings = NULL;
	pElement_OMX_ExtraDataCameraProductionFlashTest = NULL;
	pElement_OMX_SYMBIAN_CameraExtraData3AVendorSpecific = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataFeedback = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataHistogram = NULL;
	pElement_OMX_SYMBIAN_CameraExtraDataROI = NULL;
	pElement_OMX_STE_ExtraData_CaptureParameters = NULL;
	pElement_OMX_Symbian_CameraExtraDataAFStatus = NULL;
	pElement_root = NULL;
	pElement_current = NULL;
	pAttribute_current = NULL;
	pElement_xml = NULL;
	pElement_streaming_param = NULL;
	frameNumberExtraData = -1;
	memset(&ED_Detection, 0, sizeof(TFakeSource_ExtraData_Detection));

	bFunction_process_already_called = OMX_FALSE;

	bEOS = OMX_FALSE;
	xEncodeFramerate = 0x00000; // coded in Q16
	bContinuous = OMX_FALSE;
	bFrameLimited = OMX_FALSE;
	nFrameLimit = 0;
	bCapturing = OMX_FALSE;
	bAutoPause = OMX_FALSE;

	beginTimeMicroS = 0;
	currentTimeMicroS = 0;
	previsousTimeStampMS = 0;
	currentTimeDiffMS = 0;
	expectedInterFrameInMs = 0;
	sleepInMs = 0;

	pWrittingExtraData = NULL;
	cumulatedSizeWrittenExtraData = 0;
	currentSizeWrittenExtraData = 0;
	extradataHeader.nVersion.nVersion = (OMX_U32)1;
	extradataHeader.nPortIndex = 0;
	extradataHeader.eType = (OMX_EXTRADATATYPE)OMX_ExtraDataNone;
	extradataHeader.nDataSize = 0;
	extradataHeader.data[0] = (OMX_U8)0;

	s_ExtraDataQuantization[0] = 200;
	s_ExtraDataQuantization[1] = 201;
	s_ExtraDataQuantization[2] = 202;
	s_ExtraDataQuantization[3] = 202;

	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nExtraDataVersion.nVersion = 2;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationNotSpecified;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nMotionLevel = 200;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nLuxAmbient = 201;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nTargetTotalExposure = 302;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nExposureStatus = 0;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xAnalogGain = 204;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xDigitalGain = 205;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nExposureTime = 206;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xApertureFNumber = 207;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bNDFilter = OMX_FALSE;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSensitivity = 309;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bFlashFired = OMX_FALSE;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFlashPowerDuringCapture = 211;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFlashCtrlPulseInUsec = 212;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGamma = 213;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nGammaLUT[0] = 0;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[0] = 215;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[1] = 216;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[2] = 217;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[3] = 218;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[4] = 219;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[5] = 220;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[6] = 221;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[7] = 222;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion[8] = 223;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainG = 224;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainR = 225;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainB = 226;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetG = 227;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetR = 228;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetB = 229;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nColorTemperature = 230;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xRG = 231;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xBG = 232;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nContrast = 233;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nBrightness = 234;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSaturation = 235;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSharpness = 236;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nTimestamp = 0;
	s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFrameNum = 0;

	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nX =100;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nY =101;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nValue =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nMin =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nMax =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nValue =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nMin =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nMax =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bOneShot =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bContinuous =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bFrameLimited =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFrameLimit =102;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFramesBefore =103;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset =OMX_SYMBIAN_SceneAuto;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl =OMX_WhiteBalControlOff;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl =OMX_ExposureControlOff;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl =OMX_IMAGE_FlashControlOff;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl =OMX_IMAGE_FocusControlOff;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFocusSteps =104;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFocusStepIndex =105;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nLockingStatus =106;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering =OMX_MeteringModeAverage;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xEVCompensation =107;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nApertureFNumber =108;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoAperture =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nShutterSpeedMsec =109;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoShutterSpeed =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSensitivity =110;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoSensitivity =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter =OMX_ImageFilterNone;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nContrast =111;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nBrightness =112;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSaturation =113;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nBrightness =114;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bStab =OMX_FALSE;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint =OMX_SYMBIAN_DoFNotSpecified;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl =OMX_SYMBIAN_FocusRegionAuto;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sTopLeft.nX =115;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sTopLeft.nY =116;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sSize.nWidth =117;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sSize.nHeight =118;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sReference.nWidth =119;
	s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sReference.nHeight =120;

	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nExtraDataVersion.nVersion = 4;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nPixelFormat = 50;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nDataEndianess = 51;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.eRawPreset = OMX_SYMBIAN_RawImageUnprocessed;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nColorOrder = 52;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nBitDepth = 53;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nDataPedestal = 54;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nFrameCounter = 55;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nImageDataReadoutOrder = 56;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nIslLinesTop = 57;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nIslLinesBottom = 58;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisColsStart = 59;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisCols = 60;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisRowsStart = 61;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisRows = 62;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nSpatialSampling = 63;
	s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nScalingMode = 64;

#ifdef NO_DEP_WITH_IFM
#else
	s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_s16_CenterOffsetX = 1;
	s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_s16_CenterOffsetY= 2;
	s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_f_SetFOVX = 3;
	s_IFM_BMS_CAPTURE_CONTEXT.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = 4;
	s_IFM_BMS_CAPTURE_CONTEXT.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = 5;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_MaxOPXOutputSize = 6;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_MaxOPYOutputSize = 7;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTXAddrMin = 8;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTYAddrMin = 9;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTXAddrMax = 10;
	s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTYAddrMax = 11;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_f_PreScaleFactor = 12;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTXAddrStart = 13;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTYAddrStart = 14;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTXAddrEnd = 15;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTYAddrEnd = 16;
	s_IFM_BMS_CAPTURE_CONTEXT.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun = 17;
	s_IFM_BMS_CAPTURE_CONTEXT.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun = 18;
	s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u8_NumberOfStatusLines = 0; //CurrentFrameDimension_u8_NumberOfStatusLines
	s_IFM_BMS_CAPTURE_CONTEXT.SystemConfig_Status_e_PixelOrder = 0; //CurrentFrameDimension_u8_NumberOfStatusLines

    s_IFM_DAMPERS_DATA.BasesValues.IntegrationTime = 1;
    s_IFM_DAMPERS_DATA.BasesValues.AnalogGain = 2;
    s_IFM_DAMPERS_DATA.BasesValues.PostDgMinimumGain = 3;
    s_IFM_DAMPERS_DATA.BasesValues.PostDgAverageGain = 4;
    s_IFM_DAMPERS_DATA.BasesValues.PostErMinimumGain = 5;
    s_IFM_DAMPERS_DATA.BasesValues.PostErAverageGain = 6;
    s_IFM_DAMPERS_DATA.BasesValues.PostCsMinimumGain = 7;
    s_IFM_DAMPERS_DATA.BasesValues.PostCsAverageGain = 8;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevel = 9;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelRed = 10;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelGreen = 11;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelBlue = 12;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnr = 13;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrRed = 14;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrGreen = 15;
    s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrBlue = 16;
    s_IFM_DAMPERS_DATA.Other.AverageDigitalGain = 17;

	s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.bState = OMX_TRUE;
	s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nTopLeftCropVectorX = 102;
	s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nTopLeftCropVectorY = 103;
	s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nMaxOverscannedWidth = 104;
	s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nMaxOverscannedHeight = 105;

	s_OMX_STE_PRODUCTIONTESTTYPE.nIlluminanceValue = 2;
	s_OMX_STE_PRODUCTIONTESTTYPE.nFlashChromaPointX = 3;
	s_OMX_STE_PRODUCTIONTESTTYPE.nFlashChromaPointY = 4;

	s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidGamma = OMX_TRUE;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidSFX = OMX_TRUE;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidEffect = OMX_TRUE;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nRedGammaLUT[0] = 100;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nRedGammaLUT[1] = 101;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nGreenGammaLUT[0] = 100;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nGreenGammaLUT[1] = 101;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nBlueGammaLUT[0] = 100;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nBlueGammaLUT[1] = 101;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nColorMatrix[0] = 100;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nColorMatrix[1] = 101;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetR = 205;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetG = 206;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetB = 207;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nSFXSolarisControl = 208;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.nSFXNegativeControl = 208;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKEmbossEffectCtrl = 208;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKFlipperEffectCtrl = 208;
	s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKGrayBackEffectCtrl = 208;

#endif

	s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nOriginatorTimestamp = 5;
	s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nOriginatorFrameNum = 6;
	s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackFaceTracker;
	s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackValueSize = 7;
	s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackValue[0] = 8;

	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nRed[0] =  15;
	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nRed[1] =  15;
	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nGreen[0] =  25;
	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nGreen[1] =  25;
	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nBlue[0] =  35;
	s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nBlue[1] =  35;

	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nTimestamp =3;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nFrameNum =3;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.bFocused =OMX_FALSE;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nAFAreas =0;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nReferenceWindow.nWidth =30;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nReferenceWindow.nHeight =31;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nRect.sTopLeft.nX =32;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nRect.sTopLeft.nY =33;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nRect.sSize.nWidth =34;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].nRect.sSize.nHeight =35;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].xFocusDistance =36;
	s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.sAFROIs[0].eFocusStatus =OMX_FocusStatusUnableToReach;

	size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus = 0;
	memory_handle_OMX_Symbian_CameraExtraDataAFStatus = 0;
	p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL;

	OUT0("");
}

void fake_source::fsmInit(fsmInit_t inits)
{
	IN0("");
	//LOS_Log("fake_source::fsmInit \n", NULL , NULL, NULL, NULL, NULL, NULL);
	OMX_BUFFERHEADERTYPE * mBuff_list[FAKE_FIFO_SIZE] ;
	for(int i = 0; i < FAKE_FIFO_SIZE; i++)
	{
		mBuff_list[i] = &mBuff[i] ;
	}

	LOS_Log("FAKE_SOURCE : fifoSize that has been negociated and should be used: %d\n", fifoSize , NULL, NULL, NULL, NULL, NULL);
	if(fifoSize > FAKE_FIFO_SIZE)
	{
		LOS_Log("ERROR: hardcoded fifosize is not enough : %d > FAKE_FIFO_SIZE = %d \n", fifoSize , FAKE_FIFO_SIZE, NULL, NULL, NULL, NULL);
	}

	setTraceInfo(inits.traceInfoAddr, inits.id1);

	mPort[0].init(OutputPort, // Direction
		false,  // bufferSupplier
		true, // isHWPort
		0, // sharingPort
		mBuff_list,
		fifoSize, // bufferCount (FIFO size )
		&fillbufferdone, // NMF 'return' interface
		0, // port index
		inits.portsDisabled & (1 << 0), // this is a mask
		inits.portsTunneled & (1 << 0), // this is a mask
		this // owner
		);

	init(1, mPort, &proxy, &me, false);

	OUT0("");
}

void fake_source::setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled)
{
	IN0("");
	ARMNMF_DBC_ASSERT(portIdx < 1);
	if(portIdx==-1) {
		mPort[0].setTunnelStatus(isTunneled & 0x1);
	}
	else
		mPort[portIdx].setTunnelStatus((isTunneled>>portIdx) & 0x1);
	OUT0("");
}

void fake_source::reset(void)
{
	IN0("");
	LOS_Log("fake_source::reset \n", NULL , NULL, NULL, NULL, NULL, NULL);
	OUT0("");
}

void fake_source::process()
{
	IN0("");
	//LOS_Log("fake_source::process \n", NULL , NULL, NULL, NULL, NULL, NULL);
	//LOS_Log("fake_source::process : bCapturing = %d bAutoPause = %d\n", bCapturing , bAutoPause, NULL, NULL, NULL, NULL);

	bFunction_process_already_called = OMX_TRUE;

	if(bAutoPause == OMX_FALSE)
	{
		if(bEOS == OMX_FALSE)
		{
			if (mPort[0].queuedBufferCount())
			{
				OMX_BUFFERHEADERTYPE * mBuff = mPort[0].dequeueBuffer();

				myProcessBuffer(mBuff);

				if(beginTimeMicroS == 0)
				{
					beginTimeMicroS = LOS_getSystemTime();
					((OMX_BUFFERHEADERTYPE*)mBuff)->nTimeStamp = 0;
					previsousTimeStampMS = 0;
				}
				else
				{
					if(xEncodeFramerate != 0)
					{
						expectedInterFrameInMs = (1000 * 0x10000)/ xEncodeFramerate; // xEncodeFramerate is coded in Q16
						currentTimeDiffMS = (LOS_getSystemTime() - beginTimeMicroS) /1000;

						sleepInMs = (previsousTimeStampMS + expectedInterFrameInMs) - currentTimeDiffMS;
						if(sleepInMs > 0)
						{
#ifdef FAKE_SOURCE_TRACE_SLEEP
							LOS_Log("\t fake_source : LOS_Sleep( %d ms )\n", sleepInMs);
#endif
							LOS_Sleep(sleepInMs);
						}
						else
						{
#ifdef FAKE_SOURCE_TRACE_SLEEP
							LOS_Log("\t fake_source : LOS_Sleep 1 ms case xEncodeFramerate != 0\n");
#endif
							LOS_Sleep(1);
						}
						previsousTimeStampMS += expectedInterFrameInMs;
					}
					else
					{
						// sleep is needed in order to give hand to other threads
						// apparently need to be done before returnBuffer(), otherwise it does'nt work
#ifndef __SYMBIAN32__
#ifdef FAKE_SOURCE_TRACE_SLEEP
						LOS_Log("\t fake_source : LOS_Sleep 1 ms case xEncodeFramerate == 0\n");
#endif
						LOS_Sleep(1);
#endif
					}

					currentTimeMicroS = LOS_getSystemTime();
					mBuff->nTimeStamp = currentTimeMicroS - beginTimeMicroS;
				}
				//LOS_Log("\t fake_source : previsousTimeStampMS is now = %lld ms\n", previsousTimeStampMS);
				//LOS_Log("\t fake_source : set nTimeStamp              = %lld ms\n", ((OMX_BUFFERHEADERTYPE*)mBuff)->nTimeStamp /1000);
				if(((OMX_BUFFERHEADERTYPE*)mBuff)->nFlags & OMX_BUFFERFLAG_EOS)
				{
					proxy.eventHandler(OMX_EventBufferFlag, 0, ((OMX_BUFFERHEADERTYPE*)mBuff)->nFlags);
				}
#ifdef FAKE_SOURCE_TRACE_RETURNBUFFER
				LOS_Log("fake_source::process : returnBuffer\n");
#endif
				mPort[0].returnBuffer((OMX_BUFFERHEADERTYPE*)mBuff);
			}

			if (mPort[0].queuedBufferCount())
			{
				scheduleProcessEvent();
			}
		}
		else
		{
			LOS_Log("\t fake_source : bEOS = OMX_TRUE\n");
		}
	}
	else
	{
		//LOS_Log("fake_source::process : case bAutoPause = %d\n", bAutoPause);
		if(bCapturing == OMX_FALSE)
		{
			//LOS_Log("case bCapturing = %d nothing to do\n", bCapturing);
		}
		else
		{
			//LOS_Log("case bCapturing = %d action is to push a buffer\n", bCapturing);
			if(bEOS == OMX_FALSE)
			{
				if (mPort[0].queuedBufferCount())
				{
					OMX_BUFFERHEADERTYPE * mBuff = mPort[0].dequeueBuffer();

					myProcessBuffer(mBuff);

					mBuff->nTimeStamp = 0;

					if(((OMX_BUFFERHEADERTYPE*)mBuff)->nFlags & OMX_BUFFERFLAG_EOS)
					{
						proxy.eventHandler(OMX_EventBufferFlag, 0, ((OMX_BUFFERHEADERTYPE*)mBuff)->nFlags);
					}
					LOS_Log("fake_source::process : frame by frame returnBuffer\n");
					mPort[0].returnBuffer((OMX_BUFFERHEADERTYPE*)mBuff);
					bCapturing = OMX_FALSE;
					//LOS_Log("fake_source::process : intern reset bCapturing %d\n", bCapturing);
				}

				if (mPort[0].queuedBufferCount())
				{
					//LOS_Log("fake_source::process : scheduleProcessEvent\n");
					scheduleProcessEvent();
				}
			}
			else
			{
				LOS_Log("\t fake_source : bEOS = OMX_TRUE\n");
			}
		}
	}
	OUT0("");
}

void fake_source::finish(void)
{
	IN0("");
	LOS_Log("fake_source::finish \n", NULL , NULL, NULL, NULL, NULL, NULL);
	if(whole_file_data_handle != 0)
	{
		LOS_Free(whole_file_data_handle);
		whole_file_data_handle = 0;
	}
	if(fileHandle_data != NULL)
	{
		//LOS_Log("fake_source::finish LOS_fclose previous fileHandle_data\n", NULL, NULL, NULL, NULL, NULL, NULL);
		LOS_fclose((t_los_file*)fileHandle_data);
		fileHandle_data = NULL;
	}
	if(fileHandle_extradata != NULL)
	{
		//LOS_Log("fake_source::finish LOS_fclose previous fileHandle_extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
		LOS_fclose((t_los_file*)fileHandle_extradata);
		fileHandle_extradata = NULL;
	}
	if(memory_handle_extradata != 0)
	{
		LOS_Free(memory_handle_extradata);
		memory_handle_extradata = 0;
	}
	if(pElement_current)
	{
		McbDeleteElement(pElement_current);
		pElement_current=NULL;
	}
	if(pElement_streaming_param)
	{
		McbDeleteElement(pElement_streaming_param);
		pElement_streaming_param=NULL;
	}
	if(pElement_xml)
	{
		McbDeleteElement(pElement_xml);
		pElement_xml=NULL;
	}
	if(pElement_root)
	{
		McbDeleteElement(pElement_root);
		pElement_root=NULL;
	}
	if(memory_handle_OMX_Symbian_CameraExtraDataAFStatus != 0)
	{
		LOS_Free(memory_handle_OMX_Symbian_CameraExtraDataAFStatus);
		memory_handle_OMX_Symbian_CameraExtraDataAFStatus = 0;
		p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL;
	}

	OUT0("");
}


void fake_source::print_color_format(t_uint32 format)
{
	LOS_Log("fake_source::print_color_format format = %d\n", format);
	switch(format)
	{
	case OMX_COLOR_FormatUnused :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatUnused\n");break;
	case OMX_COLOR_FormatMonochrome :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatMonochrome\n");break;
	case OMX_COLOR_Format8bitRGB332 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format8bitRGB332\n");break;
	case OMX_COLOR_FormatRawBayer8bit :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatRawBayer8bit\n");break;
	case OMX_COLOR_Format12bitRGB444 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format12bitRGB444\n");break;
	case OMX_COLOR_Format16bitARGB4444 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format16bitARGB4444\n");break;
	case OMX_COLOR_Format16bitARGB1555 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format16bitARGB1555\n");break;
	case OMX_COLOR_Format16bitRGB565 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format16bitRGB565\n");break;
	case OMX_COLOR_Format16bitBGR565 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format16bitBGR565\n");break;
	case OMX_COLOR_FormatYCbYCr :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYCbYCr\n");break;
	case OMX_COLOR_FormatYCrYCb :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYCrYCb\n");break;
	case OMX_COLOR_FormatCbYCrY :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatCbYCrY\n");break;
	case OMX_COLOR_FormatCrYCbY :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatCrYCbY\n");break;
	case OMX_COLOR_Format18bitRGB666 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format18bitRGB666\n");break;
	case OMX_COLOR_Format18bitARGB1665 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format18bitARGB1665\n");break;
	case OMX_COLOR_Format19bitARGB1666 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format19bitARGB1666\n");break;
	case OMX_COLOR_Format24bitRGB888 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format24bitRGB888\n");break;
	case OMX_COLOR_Format24bitBGR888 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format24bitBGR888\n");break;
	case OMX_COLOR_Format24bitARGB1887 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format24bitARGB1887\n");break;
	case OMX_COLOR_Format25bitARGB1888 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format25bitARGB1888\n");break;
	case OMX_COLOR_Format32bitBGRA8888 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format32bitBGRA8888\n");break;
	case OMX_COLOR_Format32bitARGB8888 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format32bitARGB8888\n");break;
	case OMX_COLOR_FormatYUV411Planar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV411Planar\n");break;
	case OMX_COLOR_FormatYUV411PackedPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV411PackedPlanar\n");break;
	case OMX_COLOR_FormatYUV422Planar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV422Planar\n");break;
	case OMX_COLOR_FormatYUV422PackedPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV422PackedPlanar\n");break;
	case OMX_COLOR_FormatYUV422SemiPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV422SemiPlanar\n");break;
	case OMX_COLOR_FormatYUV422PackedSemiPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV422PackedSemiPlanar\n");break;
	case OMX_COLOR_FormatYUV444Interleaved :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV444Interleaved\n");break;
	case OMX_COLOR_FormatRawBayer10bit :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatRawBayer10bit\n");break;
	case OMX_SYMBIAN_COLOR_FormatRawBayer12bit :
		LOS_Log("fake_source::print_color_format format OMX_SYMBIAN_COLOR_FormatRawBayer12bit\n");break;
	case OMX_COLOR_FormatRawBayer8bitcompressed :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatRawBayer8bitcompressed\n");break;
	case OMX_COLOR_FormatYUV420PackedSemiPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV420PackedSemiPlanar\n");break;
	case OMX_COLOR_FormatYUV420Planar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV420Planar\n");break;
	case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
		LOS_Log("fake_source::print_color_format format OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar\n");break;
	case OMX_COLOR_FormatYUV420PackedPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV420PackedPlanar\n");break;
	case OMX_COLOR_FormatYUV420SemiPlanar :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatYUV420SemiPlanar\n");break;
	case OMX_COLOR_Format18BitBGR666 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format18BitBGR666\n");break;
	case OMX_COLOR_Format24BitARGB6666 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format24BitARGB6666\n");break;
	case OMX_COLOR_Format24BitABGR6666 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_Format24BitABGR6666\n");break;
	case OMX_COLOR_FormatL2 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL2\n");break;
	case OMX_COLOR_FormatL4 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL4\n");break;
	case OMX_COLOR_FormatL8 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL8\n");break;
	case OMX_COLOR_FormatL16 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL16\n");break;
	case OMX_COLOR_FormatL24 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL24\n");break;
	case OMX_COLOR_FormatL32 :
		LOS_Log("fake_source::print_color_format format OMX_COLOR_FormatL32\n");break;
	default :
		LOS_Log("fake_source::print_color_format format default : %d\n", format);break;
	}
}

void fake_source::setParam(t_uint32 portIndex, t_uint32 fifoSizeParam, t_uint32 direction, t_uint32 buffSupplierType, t_uint32 width, t_uint32 height, t_uint32 stride, t_uint32 sliceheight, t_uint32 colorFormat, t_uint32 buffersize, t_uint32 domaine)
{
	IN0("");
	LOS_Log("fake_source::setParam , portIndex = %d, fifoSize = %d, direction = %d, buffSupplierType = %d\n", portIndex , fifoSizeParam, direction, buffSupplierType);
	LOS_Log("fake_source::setParam , width = %d, height = %d, stride = %d, sliceheight = %d\n", width, height, stride, sliceheight);
	LOS_Log("fake_source::setParam , format = %d, buffersize = %d, domaine = %d\n", colorFormat, buffersize, domaine);
	fifoSize = fifoSizeParam;
	Width = width;
	Height = height;
	SliceHeight = sliceheight;
	Stride = stride;
	Format = colorFormat;
	BufferSize = buffersize;
	FrameSize = Stride * SliceHeight;
	if(OMX_COLOR_FormatYUV420PackedPlanar == Format)
	{
		LOS_Log("case OMX_COLOR_FormatYUV420PackedPlanar, FrameSize special computation as not a unique plan\n");
		FrameSize = (unsigned int)(FrameSize * 1.5);
	}
	LOS_Log("fake_source FrameSize = %d\n", FrameSize);

	Domaine = domaine;
	print_color_format(Format);

	OUT0("");
}

char* fake_source::my_strstr (char * str1, const char * str2 )
{
	int sizeString_1 = 0, sizeString_2 = 0;
	int count_1 = 0, count_2 = 0;

	sizeString_1 = strlen(str1);
	sizeString_2 = strlen(str2);
	if(sizeString_1 < sizeString_2)
		return (char*)NULL;

	while( count_1 <= sizeString_1 - sizeString_2)
	{
		count_2 = 0;
		while( (count_2 < sizeString_2) && (str1[count_1 + count_2] == str2[count_2]) )
		{
			count_2++;
		}
		if(count_2 == sizeString_2)
		{
			return (str1 + count_1);
		}
		count_1++;
	}
	return (char*)NULL;
}


void fake_source::Print_ED_Detection()
{
	if(ED_Detection.isThere_IFM_BMS_CAPTURE_CONTEXT_TYPE == 0)
		LOS_Log("  IFM_BMS_CAPTURE_CONTEXT_TYPE : no\n");
	else
		LOS_Log("  IFM_BMS_CAPTURE_CONTEXT_TYPE : yes\n");

	if(ED_Detection.isThere_IFM_DAMPERS_DATA_TYPE == 0)
		LOS_Log("  IFM_DAMPERS_DATA_TYPE : no\n");
	else
		LOS_Log("  IFM_DAMPERS_DATA_TYPE : yes\n");

	if(ED_Detection.isThere_OMX_ExtraDataQuantization == 0)
		LOS_Log("  OMX_ExtraDataQuantization : no\n");
	else
		LOS_Log("  OMX_ExtraDataQuantization : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCaptureParameters == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataCaptureParameters : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataCaptureParameters : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataEXIFAppMarker == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataEXIFAppMarker : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataEXIFAppMarker : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFrameDescription == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataFrameDescription : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataFrameDescription : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_ExtraDataVideoStabilization == 0)
		LOS_Log("  OMX_SYMBIAN_ExtraDataVideoStabilization : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_ExtraDataVideoStabilization : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCameraUserSettings == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataCameraUserSettings : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataCameraUserSettings : yes\n");
		
	if(ED_Detection.isThere_OMX_ExtraDataCameraProductionFlashTest == 0)
		LOS_Log("  OMX_ExtraDataCameraProductionFlashTest : no\n");
	else
		LOS_Log("  OMX_ExtraDataCameraProductionFlashTest : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraData3AVendorSpecific == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraData3AVendorSpecific : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraData3AVendorSpecific : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFeedback == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataFeedback : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataFeedback : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataHistogram == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataHistogram : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataHistogram : yes\n");

	if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataROI == 0)
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataROI : no\n");
	else
		LOS_Log("  OMX_SYMBIAN_CameraExtraDataROI : yes\n");

	if(ED_Detection.isThere_OMX_STE_ExtraData_CaptureParameters== 0)
		LOS_Log("  OMX_STE_ExtraData_CaptureParameters : no\n");
	else
		LOS_Log("  OMX_STE_ExtraData_CaptureParameters : yes\n");

	if(ED_Detection.isThere_OMX_Symbian_CameraExtraDataAFStatus == 0)
		LOS_Log("  OMX_Symbian_CameraExtraDataAFStatus : no\n");
	else
		LOS_Log("  OMX_Symbian_CameraExtraDataAFStatus : yes\n");

	LOS_Log("\n");
}

void fake_source::XML_Fill_Array(eFakeSource_Type type, void* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement, char* stringForSearchPrefixe)
{
	OMX_U32 index = 0;
	char stringForSearch[FAKE_SOURCE_STRING_SIZE_XML];
	OMX_U8 *pU8 = (OMX_U8 *)pArray;
	OMX_S8 *pS8 = (OMX_S8 *)pArray;
	OMX_U16 *pU16 = (OMX_U16 *)pArray;
	OMX_S16 *pS16 = (OMX_S16 *)pArray;
	OMX_U32 *pU32 = (OMX_U32 *)pArray;
	OMX_S32 *pS32 = (OMX_S32 *)pArray;

	for(index = 0; index < arrayNbElement; index++)
	{
		sprintf(stringForSearch, "%s_%d", stringForSearchPrefixe, (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			switch(type)
			{
				case eFakeSource_Type_U8: pU8[index] = atoi(pAttribute_current->lpszValue+1);break;
				case eFakeSource_Type_S8: pS8[index] = atoi(pAttribute_current->lpszValue+1);break;
				case eFakeSource_Type_U16: pU16[index] = atoi(pAttribute_current->lpszValue+1);break;
				case eFakeSource_Type_S16: pS16[index] = atoi(pAttribute_current->lpszValue+1);break;
				case eFakeSource_Type_U32: pU32[index] = atoi(pAttribute_current->lpszValue+1);break;
				case eFakeSource_Type_S32: pS32[index] = atoi(pAttribute_current->lpszValue+1);break;
				default: break;
			}
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
	}
}

void fake_source::XML_Fill_ROIOBJECTINFOTYPE(OMX_SYMBIAN_ROIOBJECTINFOTYPE* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement)
{
	OMX_U32 index = 0;
	char stringForSearch[FAKE_SOURCE_STRING_SIZE_XML];

	if(OMX_SYMBIAN_MAX_NUMBER_OF_ROIS < arrayNbElement)
	{
		LOS_Log("  Error ROIOBJECTINFOTYPE : OMX_SYMBIAN_MAX_NUMBER_OF_ROIS = %d < arrayNbElement = %d\n", OMX_SYMBIAN_MAX_NUMBER_OF_ROIS, arrayNbElement);
		return;
	}

	for(index = 0; index < arrayNbElement; index++)
	{
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sRect.sTopLeft.nX", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sRect.sTopLeft.nX = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sRect.sTopLeft.nY", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sRect.sTopLeft.nY = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sRect.sSize.nWidth", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sRect.sSize.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sRect.sSize.nHeight", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sRect.sSize.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sReference.nWidth", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sReference.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.sROI.sReference.nHeight", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].sROI.sReference.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.nROIID", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nROIID = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.nPriority", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nPriority = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.eObjectType", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RoiObjectNone"))
				pArray[index].eObjectType = OMX_SYMBIAN_RoiObjectNone;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RoiObjectTypeFace"))
				pArray[index].eObjectType = OMX_SYMBIAN_RoiObjectTypeFace;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RoiObjectTypeObject"))
				pArray[index].eObjectType = OMX_SYMBIAN_RoiObjectTypeObject;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RoiObjectVendorStartUnused"))
				pArray[index].eObjectType = OMX_SYMBIAN_RoiObjectVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RoiObjectMax"))
				pArray[index].eObjectType = OMX_SYMBIAN_RoiObjectMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.nOrientation.nYaw", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nOrientation.nYaw = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.nOrientation.nPitch", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nOrientation.nPitch = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sROIs_%d.nOrientation.nRoll", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nOrientation.nRoll = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
	}
}

void  fake_source::XML_Fill_AFROITYPE(OMX_SYMBIAN_AFROITYPE* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement)
{
	OMX_U32 index = 0;
	char stringForSearch[FAKE_SOURCE_STRING_SIZE_XML];

	for(index = 0; index < arrayNbElement; index++)
	{
		LOS_Log("\t %s filling cellule %d\n", __FUNCTION__, index);
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nReferenceWindow.nWidth", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nReferenceWindow.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nReferenceWindow.nHeight", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nReferenceWindow.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nRect.sTopLeft.nX", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nRect.sTopLeft.nX = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nRect.sTopLeft.nY", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nRect.sTopLeft.nY = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nRect.sSize.nWidth", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nRect.sSize.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.nRect.sSize.nHeight", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].nRect.sSize.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.xFocusDistance", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			pArray[index].xFocusDistance = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
// coverity[coverity_error_to_remove]
		sprintf(stringForSearch, "sAFROIs_%d.eFocusStatus", (int)index);
		pElement_current = McbFindElement(ptr_pElement, stringForSearch);
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusOff"))
				pArray[index].eFocusStatus = OMX_FocusStatusOff;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusRequest"))
				pArray[index].eFocusStatus = OMX_FocusStatusRequest;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusReached"))
				pArray[index].eFocusStatus = OMX_FocusStatusReached;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusUnableToReach"))
				pArray[index].eFocusStatus = OMX_FocusStatusUnableToReach;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusLost"))
				pArray[index].eFocusStatus = OMX_FocusStatusLost;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusKhronosExtensions"))
				pArray[index].eFocusStatus = OMX_FocusStatusKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusVendorStartUnused"))
				pArray[index].eFocusStatus = OMX_FocusStatusVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_FocusStatusMax"))
				pArray[index].eFocusStatus = OMX_FocusStatusMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
	}
}

void fake_source::Process_xml(int frameNumber)
{
	char string_IFM_BMS_CAPTURE_CONTEXT_TYPE[FAKE_SOURCE_STRING_SIZE_XML];
	char string_IFM_DAMPERS_DATA_TYPE[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataCaptureParameters[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataFrameDescription[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_ExtraDataVideoStabilization[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataCameraUserSettings[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_ExtraDataCameraProductionFlashTest[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraData3AVendorSpecific[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataFeedback[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataHistogram[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_SYMBIAN_CameraExtraDataROI[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_STE_ExtraData_CaptureParameters[FAKE_SOURCE_STRING_SIZE_XML];
	char string_OMX_Symbian_CameraExtraDataAFStatus[FAKE_SOURCE_STRING_SIZE_XML];
// coverity[coverity_error_to_remove]
	sprintf(string_IFM_BMS_CAPTURE_CONTEXT_TYPE, "IFM_BMS_CAPTURE_CONTEXT_TYPE_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_IFM_DAMPERS_DATA_TYPE, "IFM_DAMPERS_DATA_TYPE_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "OMX_SYMBIAN_CameraExtraDataCaptureParameters_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataFrameDescription, "OMX_SYMBIAN_CameraExtraDataFrameDescription_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_ExtraDataVideoStabilization, "OMX_SYMBIAN_ExtraDataVideoStabilization_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "OMX_SYMBIAN_CameraExtraDataCameraUserSettings_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_ExtraDataCameraProductionFlashTest, "OMX_ExtraDataCameraProductionFlashTest_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraData3AVendorSpecific, "OMX_SYMBIAN_CameraExtraData3AVendorSpecific_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataFeedback, "OMX_SYMBIAN_CameraExtraDataFeedback_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataHistogram, "OMX_SYMBIAN_CameraExtraDataHistogram_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_SYMBIAN_CameraExtraDataROI, "OMX_SYMBIAN_CameraExtraDataROI_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_STE_ExtraData_CaptureParameters, "OMX_STE_ExtraData_CaptureParameters_%d", frameNumber);
// coverity[coverity_error_to_remove]
	sprintf(string_OMX_Symbian_CameraExtraDataAFStatus, "OMX_Symbian_CameraExtraDataAFStatus_%d", frameNumber);

	LOS_Log("\n processing frame extradata number %d\n", frameNumber);
	memset(&ED_Detection, 0, sizeof(TFakeSource_ExtraData_Detection));

	pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE = McbFindElement(pElement_xml, string_IFM_BMS_CAPTURE_CONTEXT_TYPE);
	if(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE)
	{
#ifndef NO_DEP_WITH_IFM
		memset(&s_IFM_BMS_CAPTURE_CONTEXT, 0, sizeof(IFM_BMS_CAPTURE_CONTEXT));
#endif
		ED_Detection.isThere_IFM_BMS_CAPTURE_CONTEXT_TYPE = 1;
		LOS_Log("---> processing IFM_BMS_CAPTURE_CONTEXT_TYPE\n");

		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "Zoom_Control_s16_CenterOffsetX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_s16_CenterOffsetX = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "Zoom_Control_s16_CenterOffsetY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_s16_CenterOffsetY = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "Zoom_Control_f_SetFOVX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.Zoom_Control_f_SetFOVX = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "FrameDimensionStatus_u16_MaximumUsableSensorFOVX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.FrameDimensionStatus_u16_MaximumUsableSensorFOVX = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "FrameDimensionStatus_u16_MaximumUsableSensorFOVY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.FrameDimensionStatus_u16_MaximumUsableSensorFOVY = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_MaxOPXOutputSize");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_MaxOPXOutputSize = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_MaxOPYOutputSize");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_MaxOPYOutputSize = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_VTXAddrMin");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTXAddrMin = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_VTYAddrMin");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTYAddrMin = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_VTXAddrMax");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTXAddrMax = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SensorFrameConstraints_u16_VTYAddrMax");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SensorFrameConstraints_u16_VTYAddrMax = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_f_PreScaleFactor");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_f_PreScaleFactor = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_u16_VTXAddrStart");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTXAddrStart = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_u16_VTYAddrStart");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTYAddrStart = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_u16_VTXAddrEnd");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTXAddrEnd = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_u16_VTYAddrEnd");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u16_VTYAddrEnd = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "CurrentFrameDimension_u8_NumberOfStatusLines");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u8_NumberOfStatusLines = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE, "SystemConfig_Status_e_PixelOrder");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_BMS_CAPTURE_CONTEXT.SystemConfig_Status_e_PixelOrder = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE);pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE=NULL;
	}

	pElement_IFM_DAMPERS_DATA_TYPE = McbFindElement(pElement_xml, string_IFM_DAMPERS_DATA_TYPE);
	if(pElement_IFM_DAMPERS_DATA_TYPE)
	{
#ifndef NO_DEP_WITH_IFM
		memset(&s_IFM_DAMPERS_DATA, 0, sizeof(IFM_DAMPERS_DATA));
#endif
		ED_Detection.isThere_IFM_DAMPERS_DATA_TYPE = 1;
		LOS_Log("---> processing IFM_DAMPERS_DATA_TYPE\n");

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.IntegrationTime");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.IntegrationTime = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.AnalogGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.AnalogGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostDgMinimumGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostDgMinimumGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostDgAverageGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostDgAverageGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostErMinimumGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostErMinimumGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostErAverageGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostErAverageGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostCsMinimumGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostCsMinimumGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.PostCsAverageGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.PostCsAverageGain = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedPixelLevel");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevel = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedPixelLevelRed");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelRed = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedPixelLevelGreen");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelGreen = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedPixelLevelBlue");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedPixelLevelBlue = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedSnr");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnr = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedSnrRed");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrRed = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedSnrGreen");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrGreen = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "BasesValues.NomalizedSnrBlue");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.BasesValues.NomalizedSnrBlue = atof(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		pElement_current = McbFindElement(pElement_IFM_DAMPERS_DATA_TYPE, "Other.AverageDigitalGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_IFM_DAMPERS_DATA.Other.AverageDigitalGain = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_IFM_DAMPERS_DATA_TYPE);pElement_IFM_DAMPERS_DATA_TYPE=NULL;
        }

	pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataCaptureParameters);
	if(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters)
	{
		memset(&s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE, 0, sizeof(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCaptureParameters = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataCaptureParameters\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "eSceneOrientation");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationNotSpecified"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationNotSpecified;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowTopColumnLeft"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowTopColumnLeft;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowTopColumnRight"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowTopColumnRight;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowBottomColumnRight"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowBottomColumnRight;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowBottomColumnLeft"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowBottomColumnLeft;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowLeftColumnTop"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowLeftColumnTop;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowRightColumnTop"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowRightColumnTop;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowRightColumnBottom"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowRightColumnBottom;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_OrientationRowLeftColumnBottom"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.eSceneOrientation = OMX_SYMBIAN_OrientationRowLeftColumnBottom;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nMotionLevel");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nMotionLevel = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nLuxAmbient");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nLuxAmbient = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nTargetTotalExposure");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nTargetTotalExposure = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nExposureStatus");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nExposureStatus = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xAnalogGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xAnalogGain = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xDigitalGain");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xDigitalGain = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nExposureTime");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nExposureTime = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xApertureFNumber");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xApertureFNumber = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "bNDFilter");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bNDFilter = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bNDFilter = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nSensitivity");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSensitivity = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "bFlashFired");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bFlashFired = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.bFlashFired = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nFlashPowerDuringCapture");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFlashPowerDuringCapture = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nFlashCtrlPulseInUsec");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFlashCtrlPulseInUsec = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xGamma");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGamma = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		XML_Fill_Array(eFakeSource_Type_U8, (void*)&s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nGammaLUT, sizeof(s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nGammaLUT)/sizeof(OMX_U8), pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, (char*)"nGammaLUT");
		XML_Fill_Array(eFakeSource_Type_S32, (void*)&s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion, sizeof(s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nRGB2RGBColorConversion)/sizeof(OMX_S32), pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, (char*)"nRGB2RGBColorConversion");
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xGainG");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainG = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xGainR");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainR = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xGainB");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xGainB = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nOffsetG");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetG = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nOffsetR");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetR = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nOffsetB");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nOffsetB = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nColorTemperature");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nColorTemperature = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xRG");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xRG = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "xBG");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.xBG = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nContrast");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nContrast = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nBrightness");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nBrightness = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nSaturation");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSaturation = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nSharpness");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nSharpness = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nTimestamp");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nTimestamp = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters, "nFrameNum");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE.nFrameNum = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters);pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters=NULL;
	}

	pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataFrameDescription);
	if(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription)
	{
		memset(&s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE, 0, sizeof(OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFrameDescription = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataFrameDescription\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nPixelFormat");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nPixelFormat = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nDataEndianess");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nDataEndianess = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "eRawPreset");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RawImageUnprocessed"))
				s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.eRawPreset = OMX_SYMBIAN_RawImageUnprocessed;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_RawImageProcessed"))
				s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.eRawPreset = OMX_SYMBIAN_RawImageProcessed;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nColorOrder");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nColorOrder = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nBitDepth");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nBitDepth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nDataPedestal");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nDataPedestal = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nFrameCounter");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nFrameCounter = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nImageDataReadoutOrder");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nImageDataReadoutOrder = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nIslLinesTop");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nIslLinesTop = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nIslLinesBottom");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nIslLinesBottom = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nVisColsStart");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisColsStart = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nVisCols");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisCols = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nVisRowsStart");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisRowsStart = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nVisRows");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nVisRows = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nSpatialSampling");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nSpatialSampling = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription, "nScalingMode");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE.nScalingMode = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription);pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription=NULL;
	}

	pElement_OMX_SYMBIAN_ExtraDataVideoStabilization = McbFindElement(pElement_xml, string_OMX_SYMBIAN_ExtraDataVideoStabilization);
	if(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization)
	{
		memset(&s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE, 0, sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE));
		ED_Detection.isThere_OMX_SYMBIAN_ExtraDataVideoStabilization = 1;
		LOS_Log("---> processing OMX_SYMBIAN_ExtraDataVideoStabilization\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization, "bState");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.bState = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.bState = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization, "nTopLeftCropVectorX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nTopLeftCropVectorX = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization, "nTopLeftCropVectorY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nTopLeftCropVectorY = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization, "nMaxOverscannedWidth");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nMaxOverscannedWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization, "nMaxOverscannedHeight");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE.nMaxOverscannedHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_OMX_SYMBIAN_ExtraDataVideoStabilization);pElement_OMX_SYMBIAN_ExtraDataVideoStabilization=NULL;
	}

	pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataCameraUserSettings);
	 if(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings)
	 {
		memset(&s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE, 0, sizeof(OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCameraUserSettings = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataCameraUserSettings\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nX = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nY = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
 	
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xDigitalZoomFactor.nValue");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nValue = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xDigitalZoomFactor.nMin");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nMin = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xDigitalZoomFactor.nMax");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xDigitalZoomFactor.nMax = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xOpticalZoomFactor.nValue");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nValue = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xOpticalZoomFactor.nMin");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nMin = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xOpticalZoomFactor.nMax");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xOpticalZoomFactor.nMax = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
 		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bOneShot");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bOneShot = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bOneShot = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bContinuous");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bContinuous = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bContinuous = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bFrameLimited");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bFrameLimited = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bFrameLimited = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nFrameLimit");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFrameLimit = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nFramesBefore"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFramesBefore = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nSceneModePreset");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_ScenePortrait"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_ScenePortrait;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneLandscape"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneLandscape;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneNight"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneNight;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneNightPortrait"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneNightPortrait;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneSport"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneSport;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneMacro"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneMacro;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_SceneMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSceneModePreset = OMX_SYMBIAN_SceneMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eWhiteBalControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlOff"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlOff;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlSunLight"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlSunLight;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlCloudy"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlCloudy;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlShade"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlShade;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlTungsten"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlTungsten;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlFluorescent"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlFluorescent;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlIncandescent"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlIncandescent;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlFlash"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlFlash;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlHorizon"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlHorizon;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_WhiteBalControlMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eWhiteBalControl = OMX_WhiteBalControlMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eExposureControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlOff"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlOff;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlNight"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlNight;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlBackLight"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlBackLight;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlSpotLight"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlSpotLight;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlSports"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlSports;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlSnow"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlSnow;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlBeach"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlBeach;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlLargeAperture"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlLargeAperture;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlSmallApperture"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlSmallApperture;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_ExposureControlMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eExposureControl = OMX_ExposureControlMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eFlashControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlOn"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlOn;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlOff"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlOff;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlRedEyeReduction"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlRedEyeReduction;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlFillin"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlFillin;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlTorch"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlTorch;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FlashControlMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFlashControl = OMX_IMAGE_FlashControlMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eFocusControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlOn"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlOn;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlOff"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlOff;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlAutoLock"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlAutoLock;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_IMAGE_FocusControlMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusControl = OMX_IMAGE_FocusControlMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
    	pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nFocusSteps"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFocusSteps = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nFocusStepIndex"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nFocusStepIndex = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nLockingStatus"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nLockingStatus = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eMetering");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_MeteringModeAverage"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_MeteringModeAverage;
			if(strstr(pAttribute_current->lpszValue, "OMX_MeteringModeSpot"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_MeteringModeSpot;
			if(strstr(pAttribute_current->lpszValue, "OMX_MeteringModeMatrix"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_MeteringModeMatrix;
			if(strstr(pAttribute_current->lpszValue, "OMX_MeteringKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_MeteringKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_MeteringVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_MeteringVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_EVModeMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eMetering = OMX_EVModeMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "xEVCompensation"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.xEVCompensation = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nApertureFNumber"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nApertureFNumber = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bAutoAperture");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoAperture = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoAperture = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nShutterSpeedMsec"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nShutterSpeedMsec = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bAutoShutterSpeed");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoShutterSpeed = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoShutterSpeed = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nSensitivity"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSensitivity = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bAutoSensitivity");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoSensitivity = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bAutoSensitivity = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eImageFilter");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterNone"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterNone;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterNoise"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterNoise;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterEmboss"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterEmboss;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterNegative"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterNegative;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterSketch"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterSketch;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterOilPaint"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterOilPaint;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterHatch"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterHatch;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterGpen"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterGpen;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterAntialias"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterAntialias;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterDeRing"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterDeRing;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterSolarize"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterSolarize;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterKhronosExtensions"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterKhronosExtensions;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterVendorStartUnused"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterVendorStartUnused;
			if(strstr(pAttribute_current->lpszValue, "OMX_ImageFilterMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eImageFilter = OMX_ImageFilterMax;				
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nContrast"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nContrast = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nBrightness"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nBrightness = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nSaturation"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSaturation = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "nSharpness"); 
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.nSharpness = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		} 
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "bStab");
	 	if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bStab = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.bStab = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eDoFhint");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_DoFNotSpecified"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint = OMX_SYMBIAN_DoFNotSpecified;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_DoFSmall"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint = OMX_SYMBIAN_DoFSmall;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_DoFMedium"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint = OMX_SYMBIAN_DoFMedium;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_DoFLarge"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint = OMX_SYMBIAN_DoFLarge;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_DoFMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eDoFhint = OMX_SYMBIAN_DoFMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "eFocusRegionControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FocusRegionAuto"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl = OMX_SYMBIAN_FocusRegionAuto;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FocusRegionManual"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl = OMX_SYMBIAN_FocusRegionManual;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FocusRegionFacePriority"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl = OMX_SYMBIAN_FocusRegionFacePriority;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FocusRegionObjectPriority"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl = OMX_SYMBIAN_FocusRegionObjectPriority;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FocusRegionMax"))
				s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.eFocusRegionControl = OMX_SYMBIAN_FocusRegionMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
  		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sRect.sTopLeft.nX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sTopLeft.nX = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sRect.sTopLeft.nY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sTopLeft.nY = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sRect.sSize.nWidth");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sSize.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sRect.sSize.nHeight");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sRect.sSize.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sReference.nWidth");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sReference.nWidth = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings, "sFocusRegion.sReference.nHeight");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE.sFocusRegion.sReference.nHeight = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		
		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings);pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings=NULL;
	}

	pElement_OMX_ExtraDataCameraProductionFlashTest = McbFindElement(pElement_xml, string_OMX_ExtraDataCameraProductionFlashTest);
	if(pElement_OMX_ExtraDataCameraProductionFlashTest)
	{
#ifndef NO_DEP_WITH_IFM
		memset(&s_OMX_STE_PRODUCTIONTESTTYPE, 0, sizeof(OMX_STE_PRODUCTIONTESTTYPE));
#endif
		ED_Detection.isThere_OMX_ExtraDataCameraProductionFlashTest = 1;
		LOS_Log("---> processing OMX_ExtraDataCameraProductionFlashTest\n");

		pElement_current = McbFindElement(pElement_OMX_ExtraDataCameraProductionFlashTest, "nIlluminanceValue");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_PRODUCTIONTESTTYPE.nIlluminanceValue = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_ExtraDataCameraProductionFlashTest, "nFlashChromaPointX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_PRODUCTIONTESTTYPE.nFlashChromaPointX = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_ExtraDataCameraProductionFlashTest, "nFlashChromaPointY");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_PRODUCTIONTESTTYPE.nFlashChromaPointY = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
	
		McbDeleteElement(pElement_OMX_ExtraDataCameraProductionFlashTest);pElement_OMX_ExtraDataCameraProductionFlashTest=NULL;
	}

	pElement_OMX_SYMBIAN_CameraExtraDataFeedback = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataFeedback);
	if(pElement_OMX_SYMBIAN_CameraExtraDataFeedback)
	{
		memset(&s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE, 0, sizeof(OMX_SYMBIAN_CONFIG_FEEDBACKTYPE));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFeedback = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataFeedback\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFeedback, "nOriginatorTimestamp");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nOriginatorTimestamp = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFeedback, "nOriginatorFrameNum");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nOriginatorFrameNum = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFeedback, "nFeedbackType");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FeedbackFaceTracker"))
				s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackFaceTracker;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FeedbackObjectTracker"))
				s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackObjectTracker;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FeedbackMotionLevel"))
				s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackMotionLevel;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FeedbackVendorStart"))
				s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackVendorStart;
			if(strstr(pAttribute_current->lpszValue, "OMX_SYMBIAN_FeedbackMax"))
				s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackType = OMX_SYMBIAN_FeedbackMax;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataFeedback, "nFeedbackValueSize");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackValueSize = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		XML_Fill_Array(eFakeSource_Type_U8, (void*)&s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackValue, sizeof(s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE.nFeedbackValue)/sizeof(OMX_U8), pElement_OMX_SYMBIAN_CameraExtraDataFeedback, (char*)"nFeedbackValue");
	
		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataFeedback);pElement_OMX_SYMBIAN_CameraExtraDataFeedback=NULL;
	}

	pElement_OMX_SYMBIAN_CameraExtraDataHistogram = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataHistogram);
	if(pElement_OMX_SYMBIAN_CameraExtraDataHistogram)
	{
		memset(&s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM, 0, sizeof(OMX_SYMBIAN_CONFIG_RGBHISTOGRAM));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataHistogram = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataHistogram\n");

		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nRed, sizeof(s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nRed)/sizeof(OMX_U32), pElement_OMX_SYMBIAN_CameraExtraDataHistogram, (char*)"nRed");
		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nGreen, sizeof(s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nGreen)/sizeof(OMX_U32), pElement_OMX_SYMBIAN_CameraExtraDataHistogram, (char*)"nGreen");
		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nBlue, sizeof(s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM.nBlue)/sizeof(OMX_U32), pElement_OMX_SYMBIAN_CameraExtraDataHistogram, (char*)"nBlue");
	
		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataHistogram);pElement_OMX_SYMBIAN_CameraExtraDataHistogram=NULL;
	}

	pElement_OMX_SYMBIAN_CameraExtraDataROI = McbFindElement(pElement_xml, string_OMX_SYMBIAN_CameraExtraDataROI);
	if(pElement_OMX_SYMBIAN_CameraExtraDataROI)
	{
		memset(&s_OMX_SYMBIAN_ROITYPE, 0, sizeof(OMX_SYMBIAN_ROITYPE));
		ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataROI = 1;
		LOS_Log("---> processing OMX_SYMBIAN_CameraExtraDataROI\n");

		pElement_current = McbFindElement(pElement_OMX_SYMBIAN_CameraExtraDataROI, "nNumberOfROIs");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_ROITYPE.nNumberOfROIs = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		XML_Fill_ROIOBJECTINFOTYPE((OMX_SYMBIAN_ROIOBJECTINFOTYPE*)&s_OMX_SYMBIAN_ROITYPE.sROIs[0], s_OMX_SYMBIAN_ROITYPE.nNumberOfROIs, pElement_OMX_SYMBIAN_CameraExtraDataROI);

		McbDeleteElement(pElement_OMX_SYMBIAN_CameraExtraDataROI);pElement_OMX_SYMBIAN_CameraExtraDataROI=NULL;
	}

	pElement_OMX_STE_ExtraData_CaptureParameters = McbFindElement(pElement_xml, string_OMX_STE_ExtraData_CaptureParameters);
	if(pElement_OMX_STE_ExtraData_CaptureParameters)
	{
#ifndef NO_DEP_WITH_IFM
		memset(&s_OMX_STE_CAPTUREPARAMETERSTYPE, 0, sizeof(OMX_STE_CAPTUREPARAMETERSTYPE));
#endif
		ED_Detection.isThere_OMX_STE_ExtraData_CaptureParameters = 1;
		LOS_Log("---> processing OMX_STE_ExtraData_CaptureParameters\n");

		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "bValidGamma");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidGamma = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidGamma = OMX_FALSE;
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "bValidSFX");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidSFX = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidSFX = OMX_FALSE;
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "bValidEffect");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidEffect = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_STE_CAPTUREPARAMETERSTYPE.bValidEffect = OMX_FALSE;
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

#ifndef NO_DEP_WITH_IFM
		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_STE_CAPTUREPARAMETERSTYPE.nRedGammaLUT, sizeof(s_OMX_STE_CAPTUREPARAMETERSTYPE.nRedGammaLUT)/sizeof(OMX_U32), pElement_OMX_STE_ExtraData_CaptureParameters, (char*)"nRedGammaLUT");
		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_STE_CAPTUREPARAMETERSTYPE.nGreenGammaLUT, sizeof(s_OMX_STE_CAPTUREPARAMETERSTYPE.nGreenGammaLUT)/sizeof(OMX_U32), pElement_OMX_STE_ExtraData_CaptureParameters, (char*)"nGreenGammaLUT");
		XML_Fill_Array(eFakeSource_Type_U32, (void*)&s_OMX_STE_CAPTUREPARAMETERSTYPE.nBlueGammaLUT, sizeof(s_OMX_STE_CAPTUREPARAMETERSTYPE.nBlueGammaLUT)/sizeof(OMX_U32), pElement_OMX_STE_ExtraData_CaptureParameters, (char*)"nBlueGammaLUT");
		XML_Fill_Array(eFakeSource_Type_S32, (void*)&s_OMX_STE_CAPTUREPARAMETERSTYPE.nColorMatrix, sizeof(s_OMX_STE_CAPTUREPARAMETERSTYPE.nColorMatrix)/sizeof(OMX_S32), pElement_OMX_STE_ExtraData_CaptureParameters, (char*)"nColorMatrix");
#endif

		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "nOffsetR");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetR = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "nOffsetG");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetG = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "nOffsetB");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.nOffsetB = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "nSFXSolarisControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.nSFXSolarisControl = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "nSFXNegativeControl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.nSFXNegativeControl = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "uAdsocPKEmbossEffectCtrl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKEmbossEffectCtrl = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "uAdsocPKFlipperEffectCtrl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKFlipperEffectCtrl = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_STE_ExtraData_CaptureParameters, "uAdsocPKGrayBackEffectCtrl");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
#ifndef NO_DEP_WITH_IFM
			s_OMX_STE_CAPTUREPARAMETERSTYPE.uAdsocPKGrayBackEffectCtrl = atoi(pAttribute_current->lpszValue+1);
#endif
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		McbDeleteElement(pElement_OMX_STE_ExtraData_CaptureParameters);pElement_OMX_STE_ExtraData_CaptureParameters=NULL;
	}

	pElement_OMX_Symbian_CameraExtraDataAFStatus = McbFindElement(pElement_xml, string_OMX_Symbian_CameraExtraDataAFStatus);
	if(pElement_OMX_Symbian_CameraExtraDataAFStatus)
	{
		memset(&s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE, 0, sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE));
		ED_Detection.isThere_OMX_Symbian_CameraExtraDataAFStatus = 1;
		LOS_Log("---> processing OMX_Symbian_CameraExtraDataAFStatus\n");

		pElement_current = McbFindElement(pElement_OMX_Symbian_CameraExtraDataAFStatus, "nTimestamp");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nTimestamp = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_Symbian_CameraExtraDataAFStatus, "nFrameNum");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nFrameNum = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_Symbian_CameraExtraDataAFStatus, "bFocused");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
				s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.bFocused = OMX_TRUE;
			if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
				s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.bFocused = OMX_FALSE;
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}
		pElement_current = McbFindElement(pElement_OMX_Symbian_CameraExtraDataAFStatus, "nAFAreas");
		if(pElement_current)
		{
			pAttribute_current = McbFindAttribute(pElement_current, "value");
			LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);
			s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nAFAreas = atoi(pAttribute_current->lpszValue+1);
			McbDeleteElement(pElement_current);pElement_current=NULL;
		}

		// be carfull to have enough allocated memory to fill the array
#ifdef WIN32
		if(p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE != (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL)
		{
			free(p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE);
			p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL;
		}
#else
		if(memory_handle_OMX_Symbian_CameraExtraDataAFStatus != 0)
		{
			LOS_Free(memory_handle_OMX_Symbian_CameraExtraDataAFStatus);
			memory_handle_OMX_Symbian_CameraExtraDataAFStatus = 0;
			p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL;
		}
#endif		
		size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus = sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE) + (s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE.nAFAreas - 1) * sizeof(OMX_SYMBIAN_AFROITYPE);
#ifdef WIN32
		p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)malloc(size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus);
#else
		memory_handle_OMX_Symbian_CameraExtraDataAFStatus  = LOS_Alloc(size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus, 4, LOS_MAPPING_DEFAULT);
		if(memory_handle_OMX_Symbian_CameraExtraDataAFStatus != 0)
		{
			p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE = (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)LOS_GetLogicalAddress(memory_handle_OMX_Symbian_CameraExtraDataAFStatus);
		}
#endif
		if(p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE == (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL)
		{
			LOS_Log("Error memory allocation\n");
			return;
		}
				
		if(p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE != (OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE*)NULL)
		{
			memcpy(p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE, &s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE, sizeof(OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE));
			XML_Fill_AFROITYPE(&p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE->sAFROIs[0], p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE->nAFAreas, pElement_OMX_Symbian_CameraExtraDataAFStatus);
		}

		McbDeleteElement(pElement_OMX_Symbian_CameraExtraDataAFStatus);pElement_OMX_Symbian_CameraExtraDataAFStatus=NULL;
	}

// end of parsing
	Print_ED_Detection();
}


void fake_source::setMyConfig(t_uint32 index, void* opaque_ptr)
{
	IN0("");
	char* startOfFileName = (char*)NULL;
	//LOS_Log("fake_source::setMyConfig \n", NULL , NULL, NULL, NULL, NULL, NULL);
	switch (index)
	{
	case OMX_IndexParamContentURI :
		{
			unsigned int sizeString = 0;
			LOS_Log("fake_source::setMyConfig OMX_IndexParamContentURI\n", NULL, NULL, NULL, NULL, NULL, NULL);
			OMX_PARAM_CONTENTURITYPE *pFakesource_config = (OMX_PARAM_CONTENTURITYPE *)opaque_ptr;
			if(my_strstr((char*)pFakesource_config->contentURI, (char*)FAKE_SOURCE_PATH_PREFIXE_EXTRADATA))
			{
				if(fileHandle_extradata != (t_los_file*)NULL)
				{
					//LOS_Log("fake_source::setMyConfig OMX_IndexParamContentURI LOS_fclose previous fileHandle_extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
					LOS_fclose(fileHandle_extradata);
					fileHandle_extradata = (t_los_file*)NULL;
				}
				if(memory_handle_extradata != 0)
				{
					LOS_Free(memory_handle_extradata);
					memory_handle_extradata = 0;
				}
// coverity[coverity_error_to_remove]
				startOfFileName = (char*) pFakesource_config->contentURI + strlen((char*)FAKE_SOURCE_PATH_PREFIXE_EXTRADATA);
				sizeString = strlen(startOfFileName);
				if(sizeString < sizeof(fileName_extradata)/ sizeof(OMX_U8))
				{
					strncpy((char*) fileName_extradata, startOfFileName , sizeString + 1);
					LOS_Log("LOS_fopen fileName_extradata = %s\n", fileName_extradata , NULL, NULL, NULL, NULL, NULL);
					fileHandle_extradata = LOS_fopen((char*)fileName_extradata, "rb");
					if(fileHandle_extradata == NULL)
					{
						LOS_Log("ERROR : Can not open file\n", NULL, NULL, NULL, NULL, NULL, NULL);
					}
					else
					{
						LOS_fseek(fileHandle_extradata, 0, LOS_SEEK_END);
						fileSize_extradata = LOS_ftell(fileHandle_extradata);
						LOS_fseek(fileHandle_extradata, 0, LOS_SEEK_SET);
						memory_handle_extradata = LOS_Alloc(fileSize_extradata, 4, LOS_MAPPING_DEFAULT);
						if(memory_handle_extradata != 0)
						{
							extraDataPointer = (char*)LOS_GetLogicalAddress(memory_handle_extradata);
							//LOS_Log("\t call to LOS_fread fileHandle_extradata\n");
							LOS_fread(extraDataPointer, sizeof(char), fileSize_extradata, fileHandle_extradata);

							pResults.error = eXMLErrorNone;
							pResults.nLine = 0;
							pResults.nColumn = 0;
							pElement_root = McbParseXML((LPCTSTR) extraDataPointer, &pResults);
							if(pResults.error != eXMLErrorNone)
							{
								LOS_Log("\n XML error print : %s\n", McbGetError(pResults.error));
								return;
							}
							if(pElement_root)
							{
								pElement_xml = McbFindElement(pElement_root, "xml");
								if(pElement_xml)
								{
									pElement_streaming_param = McbFindElement(pElement_xml, "streaming_param");
									if(pElement_streaming_param)
									{
										pElement_current = McbFindElement(pElement_streaming_param, "parameters_for_all_frames");
										if(pElement_current)
										{
											pAttribute_current = McbFindAttribute(pElement_current, "value");
											LOS_Log("\t %s %s %s\n", pElement_current->lpszName, pAttribute_current->lpszName, pAttribute_current->lpszValue);

											if(strstr(pAttribute_current->lpszValue, "OMX_TRUE"))
											{
												McbDeleteElement(pElement_current);pElement_current=NULL;
												frameNumberExtraData = 0;
												Process_xml(frameNumberExtraData);
											}
											else
											{
												if(strstr(pAttribute_current->lpszValue, "OMX_FALSE"))
												{
													McbDeleteElement(pElement_current);pElement_current=NULL;
													frameNumberExtraData = 1;
												}
												else
												{
													LOS_Log("\t case bad setting\n");
												}
											}
										}
									}
								}
							}
						}
						else
						{
							LOS_Log("ERROR : allocation problem\n", NULL, NULL, NULL, NULL, NULL, NULL);
						}
					}
				}
				else
				{
					LOS_Log("ERROR fileName_extradata is to long = %s\n", pFakesource_config->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}
			else
			{
				if(fileHandle_data != NULL)
				{
					//LOS_Log("fake_source::setMyConfig OMX_IndexParamContentURI LOS_fclose previous fileHandle_data\n", NULL, NULL, NULL, NULL, NULL, NULL);
					LOS_fclose(fileHandle_data);
					fileHandle_data = NULL;
				}


				startOfFileName = (char*) pFakesource_config->contentURI;
				sizeString = strlen(startOfFileName);
				if(sizeString < sizeof(fileName_data)/ sizeof(OMX_U8))
				{
					strncpy((char*) fileName_data, startOfFileName , sizeString + 1);
					LOS_Log(" LOS_fopen fileName_data = %s\n", fileName_data , NULL, NULL, NULL, NULL, NULL);
					fileHandle_data = LOS_fopen((char*)fileName_data, "rb");
					if(fileHandle_data == NULL)
					{
						LOS_Log("ERROR : Can not open file\n", NULL, NULL, NULL, NULL, NULL, NULL);
					}
					else
					{
						LOS_fseek(fileHandle_data, 0, LOS_SEEK_END);
						fileSize_data = LOS_ftell(fileHandle_data);
						LOS_fseek(fileHandle_data, 0, LOS_SEEK_SET);

						LOS_Log("fileSize_data is detected as = %d\n", fileSize_data , NULL, NULL, NULL, NULL, NULL);
					}
				}
				else
				{
					LOS_Log("ERROR fileName_data is to long = %s\n", pFakesource_config->contentURI , NULL, NULL, NULL, NULL, NULL);
				}
			}
			break;
		}
	case OMX_IndexConfigCaptureMode :
		{
			LOS_Log("fake_source::setMyConfig  OMX_IndexConfigCaptureMode\n", NULL , NULL, NULL, NULL, NULL, NULL);
			OMX_CONFIG_CAPTUREMODETYPE *pFakesource_config = (OMX_CONFIG_CAPTUREMODETYPE *)opaque_ptr;
			LOS_Log("bContinuous = %d, bFrameLimited = %d, pFakesource_config->nFrameLimit = %d\n", pFakesource_config->bContinuous, pFakesource_config->bFrameLimited, pFakesource_config->nFrameLimit, NULL, NULL, NULL);
			bContinuous = pFakesource_config->bContinuous;
			bFrameLimited = pFakesource_config->bFrameLimited;
			nFrameLimit = pFakesource_config->nFrameLimit;
			break;
		}
	case OMX_IndexConfigCapturing :
		{
			LOS_Log("fake_source::setMyConfig  OMX_IndexConfigCapturing\n", NULL , NULL, NULL, NULL, NULL, NULL);
			OMX_CONFIG_BOOLEANTYPE *pFakesource_config = (OMX_CONFIG_BOOLEANTYPE *)opaque_ptr;
			LOS_Log("bEnabled = %d\n", pFakesource_config->bEnabled);
			bCapturing = pFakesource_config->bEnabled;
			if(bFunction_process_already_called == OMX_TRUE)
			{
				//LOS_Log("fake_source::setMyConfig : call scheduleProcessEvent as bFunction_process_already_called = OMX_TRUE\n");
				scheduleProcessEvent();
			}
			else
			{
				//LOS_Log("fake_source::setMyConfig : no call scheduleProcessEvent as bFunction_process_already_called = OMX_FALSE\n");
			}
			break;
		}
	case OMX_IndexAutoPauseAfterCapture :
		{
			LOS_Log("fake_source::setMyConfig  OMX_IndexAutoPauseAfterCapture\n", NULL , NULL, NULL, NULL, NULL, NULL);
			OMX_CONFIG_BOOLEANTYPE *pFakesource_config = (OMX_CONFIG_BOOLEANTYPE *)opaque_ptr;
			LOS_Log("bEnabled = %d\n", pFakesource_config->bEnabled);
			bAutoPause = pFakesource_config->bEnabled;
			break;
		}
	case OMX_IndexConfigVideoFramerate:
		{
			LOS_Log("fake_source::setMyConfig OMX_IndexConfigVideoFramerate\n", NULL, NULL, NULL, NULL, NULL, NULL);
			OMX_CONFIG_FRAMERATETYPE *pFakesource_config = (OMX_CONFIG_FRAMERATETYPE *)opaque_ptr;
			LOS_Log("xEncodeFramerate = %d <=>  %d fps\n", pFakesource_config->xEncodeFramerate, pFakesource_config->xEncodeFramerate / 0x10000, NULL, NULL, NULL, NULL);
			LOS_Log("interframe should be = %d ms\n", 1000 * 0x10000 / pFakesource_config->xEncodeFramerate, NULL, NULL, NULL, NULL, NULL);
			xEncodeFramerate = pFakesource_config->xEncodeFramerate;
			break;
		}
	default :
		break;
	}
	OUT0("");
}

void fake_source::fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)
{
	IN0("");
	//LOS_Log("fake_source::fillThisBuffer \n", NULL , NULL, NULL, NULL, NULL, NULL);
	Component::deliverBuffer(0, buffer);
	OUT0("");
}

void fake_source::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
	//LOS_Log("\t fake_source::sendCommand \n");
	switch (cmd)
	{
	case OMX_CommandStateSet:
		//LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_CommandStateSet\n");
		switch(param)
		{
		case OMX_StateInvalid:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateInvalid\n");
			break;
		case OMX_StateLoaded:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateLoaded\n");
			beginTimeMicroS = 0; // reset reference time in order to start in good conditions
			break;
		case OMX_StateIdle:
			LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateIdle\n");
			beginTimeMicroS = 0; // reset reference time in order to start in good conditions
			if(whole_file_data_handle != 0)
			{
				LOS_Free(whole_file_data_handle);
				whole_file_data_handle = 0;
			}
			if(fileHandle_data != NULL)
			{
				//LOS_Log("fake_source::sendCommand OMX_StateIdle LOS_fclose previous fileHandle_data\n", NULL, NULL, NULL, NULL, NULL, NULL);
				LOS_fclose(fileHandle_data);
				fileHandle_data = NULL;
			}
			if(fileHandle_extradata != NULL)
			{
				//LOS_Log("fake_source::sendCommand OMX_StateIdle LOS_fclose previous fileHandle_extradata\n", NULL, NULL, NULL, NULL, NULL, NULL);
				LOS_fclose(fileHandle_extradata);
				fileHandle_extradata = NULL;
			}
			if(memory_handle_extradata != 0)
			{
				LOS_Free(memory_handle_extradata);
				memory_handle_extradata = 0;
			}

			bEOS = OMX_FALSE;
			if(fileHandle_data != NULL)
			{
				LOS_fseek(fileHandle_data, 0, LOS_SEEK_SET);
				LOS_Log("\t fake_source::sendCommand -> place file reading pointer at beginning\n");
			}
			//LOS_Log("\t fake_source::sendCommand reset fileSizeRead_data to 0\n");
			fileSizeRead_data = 0;
			//LOS_Log("\t fake_source::sendCommand reset beginTimeMicroS currentTimeMicroS previsousTimeStampMSto 0\n");
			beginTimeMicroS = 0;
			currentTimeMicroS = 0;
			previsousTimeStampMS = 0;
			break;
		case OMX_StateExecuting:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateExecuting\n");
			break;
		case OMX_StatePause:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StatePause\n");
			beginTimeMicroS = 0; // reset reference time in order to start in good conditions
			break;
		case OMX_StateTransientToIdle:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateTransientToIdle\n");
			break;
		case OMX_StateWaitForResources:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateWaitForResources\n");
			break;
		case OMX_StateTransient:
			//LOS_Log("\t tracing state: OMX_StateTransient\n");
			break;
		case OMX_StateLoadedToIdleHOST:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateLoadedToIdleHOST\n");
			break;
		case OMX_StateLoadedToIdleDSP:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateLoadedToIdleDSP\n");
			break;
		case OMX_StateIdleToLoaded:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateIdleToLoaded\n");
			break;
		case OMX_StateIdleToExecuting:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateIdleToExecuting\n");
		break;
		case OMX_StateIdleToPause:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateIdleToPause\n");
			break;
		case OMX_StateExecutingToIdle:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateExecutingToIdle\n");
			break;
		case OMX_StateExecutingToPause:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StateExecutingToPause\n");
			break;
		case OMX_StatePauseToIdle:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StatePauseToIdle\n");
			break;
		case OMX_StatePauseToExecuting:
			//LOS_Log("\t fake_source::sendCommand tracing state: OMX_StatePauseToExecuting\n");
			break;
		default:
			LOS_Log("\t fake_source::sendCommand tracing state: OMX_State ... unknown = %d\n", param);
			break;
		}
		break;
	case OMX_CommandFlush:
		//LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_CommandFlush\n");
		break;
	case OMX_CommandPortDisable:
		//LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_CommandPortDisable\n");
		break;
	case OMX_CommandPortEnable:
		//LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_CommandPortEnable\n");
		break;
	case OMX_CommandMarkBuffer:
		//LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_CommandMarkBuffer\n");
		break;
	default:
		LOS_Log("\t fake_source::sendCommand tracing cmd: OMX_Command... unknown\n");
		break;
	}

	Component::sendCommand(cmd, param);
}

void fake_source::add_ExtradatStructure(OMX_BUFFERHEADERTYPE* pBufferH, OMX_EXTRADATATYPE typeED, unsigned int sizeofED, void* bufferED)
{
	if(sizeofED > 0)
	{
		extradataHeader.nSize = sizeof(OMX_OTHER_EXTRADATATYPE) + sizeofED - sizeof(OMX_U8);
	}
	else
	{
		// case last structure to end extra data
		extradataHeader.nSize = sizeof(OMX_OTHER_EXTRADATATYPE);
	}

	if( extradataHeader.nSize % sizeof(OMX_U32) )
	{
		extradataHeader.nSize += ( sizeof(OMX_U32) - (extradataHeader.nSize % sizeof(OMX_U32) ) );
	}

	if(pWrittingExtraData == NULL)
	{
		pWrittingExtraData = (char*)pBufferH->pBuffer + pBufferH->nOffset + pBufferH->nFilledLen;
		cumulatedSizeWrittenExtraData = 0;
		currentSizeWrittenExtraData = 0;
	}

	if( pWrittingExtraData + extradataHeader.nSize > (char*)pBufferH->pBuffer + pBufferH->nAllocLen)
	{
		LOS_Log("\t fct %s : Error : not enough space to writte extra data\n", __FUNCTION__);
		LOS_Log("\t pWrittingExtraData = 0x%x, nSize = 0x%x, pBuffer = 0x%x, nAllocLen = 0x%x\n", (int)pWrittingExtraData, extradataHeader.nSize, (int)pBufferH->pBuffer, pBufferH->nAllocLen);
		return;
	}

	extradataHeader.nPortIndex = pBufferH->nOutputPortIndex;
	extradataHeader.eType = (OMX_EXTRADATATYPE)typeED;
	extradataHeader.nDataSize = sizeofED;
	extradataHeader.data[0] = (OMX_U8)0;
	currentSizeWrittenExtraData = sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8);
	cumulatedSizeWrittenExtraData += currentSizeWrittenExtraData;
	memcpy(pWrittingExtraData, &extradataHeader, currentSizeWrittenExtraData);

	if(sizeofED > 0)
	{
		OMX_OTHER_EXTRADATATYPE * pHeaderOtherExtraData = (OMX_OTHER_EXTRADATATYPE*) pWrittingExtraData;
		currentSizeWrittenExtraData = sizeofED;
		cumulatedSizeWrittenExtraData += currentSizeWrittenExtraData;
		if(bufferED != (void*) NULL)
		{
			memcpy(pHeaderOtherExtraData->data , bufferED, currentSizeWrittenExtraData);
		}
	}

	pWrittingExtraData += extradataHeader.nSize;
}

void fake_source::add_IFM_BMS_CAPTURE_CONTEXT_TYPE(OMX_BUFFERHEADERTYPE* pBufferH)
{
#ifdef NO_DEP_WITH_IFM
#else
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)IFM_BMS_CAPTURE_CONTEXT_TYPE, sizeof(s_IFM_BMS_CAPTURE_CONTEXT), (void*)&s_IFM_BMS_CAPTURE_CONTEXT);
#endif
}

void fake_source::add_IFM_DAMPERS_DATA_TYPE(OMX_BUFFERHEADERTYPE* pBufferH)
{
#ifdef NO_DEP_WITH_IFM
#else
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)IFM_DAMPERS_DATA_TYPE, sizeof(s_IFM_DAMPERS_DATA), (void*)&s_IFM_DAMPERS_DATA);
#endif
}

void fake_source::add_OMX_ExtraDataQuantization(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_ExtraDataQuantization, sizeof(s_ExtraDataQuantization), (void*)&s_ExtraDataQuantization);
}

void fake_source::add_OMX_SYMBIAN_ExtraDataVideoStabilization(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization, sizeof(s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE), (void*)&s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE);
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataCameraUserSettings(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCameraUserSettings, sizeof(s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE), (void*)&s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE);
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataCaptureParameters(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataCaptureParameters, sizeof(s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE), (void*)&s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE);
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataEXIFAppMarker(OMX_BUFFERHEADERTYPE* pBufferH)
{
	//LOS_Log("add_OMX_SYMBIAN_CameraExtraDataEXIFAppMarker not yet implemented\n");
	//pWrittingExtraData += extradataHeader.nSize;
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataFrameDescription(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFrameDescription, sizeof(s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE), (void*)&s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE);
}

void fake_source::add_OMX_ExtraDataCameraProductionFlashTest(OMX_BUFFERHEADERTYPE* pBufferH)
{
#ifdef NO_DEP_WITH_IFM
#else
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_ExtraDataCameraProductionFlashTest, sizeof(OMX_STE_PRODUCTIONTESTTYPE), (void*)&s_OMX_STE_PRODUCTIONTESTTYPE);
#endif
}

void fake_source::add_OMX_SYMBIAN_CameraExtraData3AVendorSpecific(OMX_BUFFERHEADERTYPE* pBufferH)
{
	//LOS_Log("add_OMX_SYMBIAN_CameraExtraData3AVendorSpecific not yet implemented\n");
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataFeedback(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataFeedback, sizeof(s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE), (void*)&s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE);
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataHistogram(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataHistogram, sizeof(s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM), (void*)&s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM);
}

void fake_source::add_OMX_SYMBIAN_CameraExtraDataROI(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_SYMBIAN_CameraExtraDataROI, sizeof(s_OMX_SYMBIAN_ROITYPE), (void*)&s_OMX_SYMBIAN_ROITYPE);
}

void fake_source::add_OMX_STE_ExtraData_CaptureParameters(OMX_BUFFERHEADERTYPE* pBufferH)
{
#ifdef NO_DEP_WITH_IFM
#else
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_STE_ExtraData_CaptureParameters, sizeof(s_OMX_STE_CAPTUREPARAMETERSTYPE), (void*)&s_OMX_STE_CAPTUREPARAMETERSTYPE);
#endif
}

void fake_source::add_OMX_Symbian_CameraExtraDataAFStatus(OMX_BUFFERHEADERTYPE* pBufferH)
{
	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_Symbian_CameraExtraDataAFStatus, size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus, (void*)p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE);
}

void fake_source::add_EndExtraData(OMX_BUFFERHEADERTYPE* pBufferH)
{
	if(pWrittingExtraData == NULL)
	{
		pWrittingExtraData = (char*)pBufferH->pBuffer + pBufferH->nOffset + pBufferH->nFilledLen;
		cumulatedSizeWrittenExtraData = 0;
		currentSizeWrittenExtraData = 0;
		//LOS_Log("fake_source::add_EndExtraData case no extra data cumulated \n", NULL , NULL, NULL, NULL, NULL, NULL);
		pBufferH->nFlags &= ~OMX_BUFFERFLAG_EXTRADATA;
	}
	else
	{
		pBufferH->nFlags |= OMX_BUFFERFLAG_EXTRADATA;
	}

	add_ExtradatStructure(pBufferH, (OMX_EXTRADATATYPE)OMX_ExtraDataNone, 0, (void*)NULL);

	pWrittingExtraData = NULL;
	//LOS_Log("cumulatedSizeWrittenExtraData = %d \n", cumulatedSizeWrittenExtraData);
	cumulatedSizeWrittenExtraData = 0;
	currentSizeWrittenExtraData = 0;
}


void fake_source::myProcessBuffer(OMX_BUFFERHEADERTYPE* pBufferH)
{
	OMX_U32 size = 0;
	//LOS_Log("fake_source::myProcessBuffer \n", NULL , NULL, NULL, NULL, NULL, NULL);

	pBufferH->nOffset = 0;
	if(fileHandle_data == NULL)
	{
		if(fileName_data[0] != 0)
		{
			LOS_Log("fake_source::myProcessBuffer trying opening file : %s\n", fileName_data, NULL, NULL, NULL, NULL, NULL);
			fileHandle_data = LOS_fopen((char*)fileName_data, "rb");
			if(fileHandle_data == NULL)
			{
				LOS_Log("ERROR : fake_source::myProcessBuffer Can not open file : %s\n", fileName_data, NULL, NULL, NULL, NULL, NULL);
				pBufferH->nFilledLen = 0;
				return;
			}
		}
		else
		{
			LOS_Log("ERROR : fake_source::myProcessBuffer bad file name : not initialized\n", NULL, NULL, NULL, NULL, NULL, NULL);
			return;
		}
	}

	if(   (bContinuous == OMX_TRUE) || // loop mode
		( (bFrameLimited == OMX_FALSE) && (fileSizeRead_data < fileSize_data) ) || // read whole file once
		( (bFrameLimited == OMX_TRUE) && (nFrameLimit != 0) && (fileSizeRead_data < nFrameLimit*FrameSize) ) // read just N frames of the file
		)
	{
		//LOS_Log("\t reso = %dx%d BufferSize %d FrameSize%d\n", Width, Height, BufferSize, FrameSize);
		//print_color_format(Format);

		if(frameNumberExtraData >= 0)
		{
			if(frameNumberExtraData == 0)
			{
				//LOS_Log("\t nothing to do, parsing is already done previsouly\n");
			}
			else
			{
				LOS_Log("\t retrive extradata for frame number %d\n", frameNumberExtraData);
				Process_xml(frameNumberExtraData);
				frameNumberExtraData++;
			}

			if(0 == FrameSizeUpdateDueToStatusLine)
			{
				if(ED_Detection.isThere_IFM_BMS_CAPTURE_CONTEXT_TYPE)
				{
					LOS_Log("\t presence IFM_BMS_CAPTURE_CONTEXT_TYPE\n");
					LOS_Log("\t CurrentFrameDimension_u8_NumberOfStatusLines = %d\n", s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u8_NumberOfStatusLines);
					if(OMX_COLOR_FormatRawBayer8bit == Format)
					{
						FrameSize += Stride * s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u8_NumberOfStatusLines;
						LOS_Log("\t update FrameSize = %d\n", FrameSize);
					}
					else if(OMX_SYMBIAN_COLOR_FormatRawBayer12bit == Format)
					{
						FrameSize += Stride * s_IFM_BMS_CAPTURE_CONTEXT.CurrentFrameDimension_u8_NumberOfStatusLines;
						LOS_Log("\t update FrameSize = %d\n", FrameSize);
					}
					else
					{
						//LOS_Log("\t no need to add StatusLines\n");
					}
				}
				else
				{
					//LOS_Log("\t nopresence IFM_BMS_CAPTURE_CONTEXT_TYPE\n");
				}
				FrameSizeUpdateDueToStatusLine++;
			}
		}
		else
		{
			//LOS_Log("\t no extradta to add\n");
		}

		//LOS_Log("\t call to LOS_fread fileHandle_data with FrameSize = %d\n", FrameSize);
		size = LOS_fread(pBufferH->pBuffer, 1, FrameSize, fileHandle_data);
		if(size != FrameSize)
		{
			LOS_Log("fake_source::myProcessBuffer warning Unable to read %ld , red size = %ld\n", FrameSize, size);
		}
		fileSizeRead_data += size;
		//LOS_Log("\t fileSizeRead_data = %d\n", fileSizeRead_data);
		if(bContinuous == OMX_TRUE)
		{
			//LOS_Log("\t fread %d bytes with fileSize_data = %d \n", size, fileSize_data);
		}
		else
		{
			if(bFrameLimited == OMX_TRUE)
			{
				//LOS_Log("\t fread %d bytes with fileSizeRead_data = %d fileSize_data = %d (case nFrameLimit == %d != 0)\n", size, fileSizeRead_data, fileSize_data, nFrameLimit);
				//LOS_Log("\t limit is nFrameLimit*FrameSize %d \n", nFrameLimit*FrameSize);
			}
			else
			{
				//LOS_Log("\t fread %d bytes with fileSizeRead_data = %d fileSize_data = %d (case nFrameLimit == 0)\n", size, fileSizeRead_data, fileSize_data);
			}
		}
		pBufferH->nFilledLen = size;

		if(frameNumberExtraData >= 0)
		{

			//LOS_Log("same extradata for all frames\n");
			if(ED_Detection.isThere_IFM_BMS_CAPTURE_CONTEXT_TYPE)
			{
				//LOS_Log("fake_source adding IFM_BMS_CAPTURE_CONTEXT_TYPE\n");
				add_IFM_BMS_CAPTURE_CONTEXT_TYPE(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding IFM_BMS_CAPTURE_CONTEXT_TYPE\n");
			}
			if(ED_Detection.isThere_IFM_DAMPERS_DATA_TYPE)
			{
				//LOS_Log("fake_source adding IFM_DAMPERS_DATA_TYP\n");
				add_IFM_DAMPERS_DATA_TYPE(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding IFM_DAMPERS_DATA_TYP\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCaptureParameters)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataCaptureParameters\n");
				add_OMX_SYMBIAN_CameraExtraDataCaptureParameters(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraDataCaptureParameters\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFrameDescription)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataFrameDescription\n");
				add_OMX_SYMBIAN_CameraExtraDataFrameDescription(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraDataFrameDescription\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_ExtraDataVideoStabilization)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_ExtraDataVideoStabilization\n");
				add_OMX_SYMBIAN_ExtraDataVideoStabilization(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_ExtraDataVideoStabilization\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataCameraUserSettings)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataCameraUserSettings\n");
				add_OMX_SYMBIAN_CameraExtraDataCameraUserSettings(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraDataCameraUserSettings\n");
			}
			if(ED_Detection.isThere_OMX_ExtraDataCameraProductionFlashTest)
			{
				//LOS_Log("fake_source adding OMX_ExtraDataCameraProductionFlashTest\n");
				add_OMX_ExtraDataCameraProductionFlashTest(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_ExtraDataCameraProductionFlashTest\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraData3AVendorSpecific)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraData3AVendorSpecific\n");
				add_OMX_SYMBIAN_CameraExtraData3AVendorSpecific(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraData3AVendorSpecific\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataFeedback)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataFeedback\n");
				add_OMX_SYMBIAN_CameraExtraDataFeedback(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraDataFeedback\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataHistogram)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataHistogram\n");
				add_OMX_SYMBIAN_CameraExtraDataHistogram(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding OMX_SYMBIAN_CameraExtraDataHistogram\n");
			}
			if(ED_Detection.isThere_OMX_SYMBIAN_CameraExtraDataROI)
			{
				//LOS_Log("fake_source adding OMX_SYMBIAN_CameraExtraDataROI\n");
				add_OMX_SYMBIAN_CameraExtraDataROI(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding isThere_OMX_SYMBIAN_CameraExtraDataROI\n");
			}
			if(ED_Detection.isThere_OMX_STE_ExtraData_CaptureParameters)
			{
				//LOS_Log("fake_source adding OMX_STE_ExtraData_CaptureParameters\n");
				add_OMX_STE_ExtraData_CaptureParameters(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding isThere_OMX_STE_ExtraData_CaptureParameters\n");
			}
			if(ED_Detection.isThere_OMX_Symbian_CameraExtraDataAFStatus)
			{
				//LOS_Log("fake_source adding OMX_Symbian_CameraExtraDataAFStatus\n");
				add_OMX_Symbian_CameraExtraDataAFStatus(pBufferH);
			}
			else
			{
				//LOS_Log("fake_source not adding isThere_OMX_Symbian_CameraExtraDataAFStatus\n");
			}

			add_EndExtraData(pBufferH);
		}
		else
		{
			//LOS_Log("\t no extradta to add\n");
		}
		
		if(fileSizeRead_data % fileSize_data == 0)
		{
			LOS_fseek(fileHandle_data, 0, LOS_SEEK_SET);
			LOS_Log("\t fake_source::myProcessBuffer -> place file reading pointer at beginning\n");
			if( bContinuous == OMX_TRUE)
			{
				//LOS_Log("\t fake_source::myProcessBuffer reading looping file -> reset fileSizeRead_data to 0\n");
				fileSizeRead_data = 0;
			}
		}

		if( bContinuous == OMX_FALSE)
		{
			if(
				( (bFrameLimited == OMX_FALSE) && (fileSizeRead_data == fileSize_data) ) || // read whole file once
				( (bFrameLimited == OMX_TRUE) && (nFrameLimit != 0) && (fileSizeRead_data == nFrameLimit*FrameSize) )// read just N frames of the file
			)
			{

				LOS_Log("\t fake_source::myProcessBuffer reading NO bContinuous : setting flag EOS \n");
				// make a |= instead of = in order to keep flag for extradata
				pBufferH->nFlags |= OMX_BUFFERFLAG_EOS ;
				if(fileHandle_data)
				{
					//LOS_Log("fake_source::myProcessBuffer bFrameLimited == OMX_TRUE LOS_fclose previous file\n", NULL, NULL, NULL, NULL, NULL, NULL);
					LOS_fclose(fileHandle_data);
					fileHandle_data = (t_los_file *)NULL;
				}
				else
				{
					LOS_Log("Warning fake_source::myProcessBuffer fileHandle_data is NULL (line %d)\n", __LINE__);
				}
				bEOS = OMX_TRUE;
			}
		}
	}
	else
	{
		LOS_Log("\t fake_source::myProcessBuffer NO READ TO DO : bContinuous = %d, bFrameLimited = %d, nFrameLimit = %d, \n", bContinuous, bFrameLimited, nFrameLimit);
		LOS_Log("\t fake_source::myProcessBuffer NO READ TO DO : fileSizeRead_data = %d, fileSize_data = %d, nFrameLimit*FrameSize = %d\n", fileSizeRead_data, fileSize_data, nFrameLimit*FrameSize);
	}
}
