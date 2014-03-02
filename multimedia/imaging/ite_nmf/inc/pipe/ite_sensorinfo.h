/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_SENSORINFO_H_
#define ITE_SENSORINFO_H_

//#include "share/inc/type.h"
#include "ite_boardinfo.h"

#define TRIGGER TRUE+1
#define NAB TRIGGER+1
#define MAXSENSORSCOUNT 2



// defines precise sensor identity
typedef struct sensorRevision {
   char * manufacture; // sensor maker
   char * internalName; // development/prototyping codename, ex: S850, S555
   char * productName; // market product name
//  sensor revision, apply exponent alignment
//  ex. Revision 2.56 = 256, 1 = 100, 2.34.11 = 23411
   int number;
   int factory; // production factory code
   int identity; // sensor i2c numeric identity, 0x2ee, 0x352
} sensRev, *sensRevPtr;

// defines compatibility between sensor and famous standards
typedef struct sensorCompatibility {
//  SMIA standard compiance,  apply exponent alignment
//  ex. Version 2.56 = 256, 1 = 100, 2.34.11 = 23411
   int versionCompliantSMIA;
   char * eWarpVersion; // version of eWarp compatible with sensor
   char * eWarpNick; // eWarp nick name dependency line
   char * sensortype; // sensortype CCP or CSI2
   int datalanes;  // number of datalanes (for CSI only)
} sensCompat, *sensCompatPtr;

// defines current sensor configuration
typedef struct sensorConfig {
   int frameRate; // frames per second
   int colorDepth; // bits per pixel, ex. 10bpp, 8bpp
   int inputImageSizeX; // pixels per line
   int inputImageSizeY; // lines per frame
   int testMode; // sensor test mode
   int socket; // socket to where sensor attached
   int addrI2C; // sensor i2c address
   t_uint8 littleEndian; // sensor little Endian i2c address mode
   int fieldOfViewX; // viewable area dimention X
   int fieldOfViewY; // viewable area dimention Y
   char * videomaxsize; // max size for Video/stream usecase (NMF test)
   char * stillmaxsize; // max size for still capture usecase (NMF test)
} sensConf, *sensConfPtr;

// defines overal sensor metrics limits
typedef struct sensorMetricLimits {
   int maxFrameRate; // frames per second
   int maxImageSizeX; // pixels per line
   int maxImageSizeY; // lines per frame
   int minImageSizeX; // pixels per line
   int minImageSizeY; // lines per frame
   int maxDataStrobe; // maximum speed based on datastrobe
   int maxDataClock; // maximum speed based on dataclock
   int maxAnalogGain; // maximum gain of analog amplifier
   int pixOffsetX; // bayer data alignment on X
   int pixOffsetY; // bayer data alignment on Y
} sensMetrLim, *sensMetrLimPtr;

// defines features supported by sensor
typedef struct sensorFeatures {
   t_uint8 autofocus; // focusing automatically
   t_uint8 flashLight; // flash light availability
   t_uint8 zoomMech; // mechanical zoom
   t_uint8 zoomDig; // digital zoom, internal
   t_uint8 frameCut; // dimension/position cutting out frame
   t_uint8 nightVision; // infrared sensitivity
   t_uint8 nightLight; // infrared led
   t_uint8 requireVPIP; // sensor operates with help of VPIP
   t_uint8 ccirInput; // operates over CCIR protocol
   t_uint8 clockedge; // triggered by clockedge signal
   t_uint8 dataclock; // triggered by dataclock signal
   t_uint8 datastrobe; // triggered by datastrobe signal
   t_uint8 lowPowerStreaming; //streaming while low power mode
} sensFea, *sensFeaPtr;

// defines sensor specific procedures
typedef struct sensorHanding {
   char * initSequence; // sensor specific initialisation table
   void * deinitProc;
   char * colorMatrix; // sensor specific color conversion table
   char * initAutoFocus; // sensor specific autofocus initialisation table
   void * streamParser;
   char * pathGRID;    // path to GRID tables
} sensHand, *sensHandPtr;

// defines page element direct programmation
typedef struct irpInitElementPage {
   // common page elements
   // common page elements
   int page_AntiFlickerExposureControls_fGuaranteeStaticFlickerFrameLength;
   int page_ColourEngine0_FadeToBlack_fDisable;
   int page_AutomaticFrameRateControl_bMode;
   int page_StaticFrameRateControl_bDesiredFrameRate_Den;
   int page_StaticFrameRateControl_uwDesiredFrameRate_Num_MSByte;
   int page_ZoomMgrCtrl_bMagFactor;
   int page_ZoomMgrCtrl_fChgOverForbidden;
   int page_ZoomMgrCtrl_fSetAlternateInitWOI;
   int page_JackFilterControls_fDisablePromotingLow;
   int page_JackFilterControls_fDisablePromotingHigh;
   int page_JackFilterControls_bMaxWeightLow;
   int page_JackFilterControls_bMaxWeightHigh;
   int page_NoraControls_bMaximumValue;
   int page_NoraControls_DamperLowThreshold_MSByte;
   int page_NoraControls_DamperHighThreshold_MSByte;
   int page_NoraControls_fDisable;
   int page_NoraControls_fDisableNoraPromoting;
   int page_MinWeightedWBControls_fDisable;
   int page_FlashManagerControl_bMode;
   int page_JackFilterControls_fDisableFilter;
   int page_VfpnControls_fEnableCorrection;
   int page_AntiVignetteControls_fDisableFilter;
   int page_ColourEngine0_OutputCoderControls_bContrast;
   int page_ColourEngine0_OutputCoderControls_bColourSaturation;
   int page_ColourEngine0_GammaCorrection_SharpRed;
   int page_ColourEngine0_GammaCorrection_SharpGreen;
   int page_ColourEngine0_GammaCorrection_SharpBlue;
   int page_ColourEngine0_GammaCorrection_SoftRed;
   int page_ColourEngine0_GammaCorrection_SoftGreen;
   int page_ColourEngine0_GammaCorrection_SoftBlue;
   int page_ColourEngine0_ApertureCorrectionControls_bMinimumCoringThreshold;
   int page_ColourEngine0_ApertureCorrectionControls_bMaxGain;
   int page_ColourEngine0_ColourMatrixDamperControl_DamperLowThreshold_MSByte;
   int page_ColourEngine0_ColourMatrixDamperControl_DamperHighThreshold_MSByte;
   int page_ColourEngine0_ColourMatrixDamperControl_MinimumDamperOutput_MSByte;
   int page_ColourEngine0_ApertureCorrectionControls_bMinimumHighThreshold;
   int page_ColourEngine0_ApertureCorrectionControls_DamperLowThreshold_Coring_MSByte;
   int page_ColourEngine0_ApertureCorrectionControls_DamperHighThreshold_Coring_MSByte;
   int page_ColourEngine0_ApertureCorrectionControls_MinimumDamperOutput_Coring_MSByte;
   int page_ColourEngine0_ApertureCorrectionControls_fDisableGainDamping;
   int page_ColourEngine0_ApertureCorrectionControls_fDisableCoringDamping;
   int page_ColourEngine0_ApertureCorrectionControls_fDisableCorrection;
   int page_ColourEngine0_ColourMatrixDamperControl_fDisableMatrixDamping;
   int page_ColourEngine0_RadialApertureCorrectionControl_fEnableCorrection;
   int page_ColourEngine1_OutputCoderControls_bContrast;
   int page_ColourEngine1_OutputCoderControls_bColourSaturation;
   int page_ColourEngine1_GammaCorrection_SharpRed;
   int page_ColourEngine1_GammaCorrection_SharpGreen;
   int page_ColourEngine1_GammaCorrection_SharpBlue;
   int page_ColourEngine1_GammaCorrection_SoftRed;
   int page_ColourEngine1_GammaCorrection_SoftGreen;
   int page_ColourEngine1_GammaCorrection_SoftBlue;
   int page_ColourEngine1_ApertureCorrectionControls_bMinimumCoringThreshold;
   int page_ColourEngine1_ApertureCorrectionControls_bMaxGain;
   int page_ColourEngine1_ColourMatrixDamperControl_DamperLowThreshold_MSByte;
   int page_ColourEngine1_ColourMatrixDamperControl_DamperHighThreshold_MSByte;
   int page_ColourEngine1_ColourMatrixDamperControl_MinimumDamperOutput_MSByte;
   int page_ColourEngine1_ApertureCorrectionControls_bMinimumHighThreshold;
   int page_ColourEngine1_ApertureCorrectionControls_DamperLowThreshold_Coring_MSByte;
   int page_ColourEngine1_ApertureCorrectionControls_DamperHighThreshold_Coring_MSByte;
   int page_ColourEngine1_ApertureCorrectionControls_MinimumDamperOutput_Coring_MSByte;
   int page_ColourEngine1_ApertureCorrectionControls_fDisableGainDamping;
   int page_ColourEngine1_ApertureCorrectionControls_fDisableCoringDamping;
   int page_ColourEngine1_ApertureCorrectionControls_fDisableCorrection;
   int page_ColourEngine1_ColourMatrixDamperControl_fDisableMatrixDamping;
   int page_ColourEngine1_RadialApertureCorrectionControl_fEnableCorrection;
   int page_ScytheFilterControls_bMaxWeightLow; 
   int page_ScytheFilterControls_bMaxWeightHigh;
   int page_ScytheFilterControls_fpDamperLowThresholdLow_MSByte;
   int page_ScytheFilterControls_fpDamperLowThresholdHigh_MSByte;
   int page_ScytheFilterControls_fpMinimumDamperOutputLow_MSByte;
   int page_ScytheFilterControls_fpMinimumDamperOutputHigh_MSByte;
   int page_ScytheFilterControls_fpDamperHighThresholdLow_MSByte;
   int page_ScytheFilterControls_fpDamperHighThresholdHigh_MSByte;
   int page_ScytheFilterControls_fDisablePromotingLow;
   int page_ScytheFilterControls_fDisablePromotingHigh;
   int page_ScytheFilterControls_fDisableFilter;
   int page_WhiteBalanceControls_bMode;
   int page_WhiteBalanceConstrainerControls_fpRedA_MSByte;
   int page_WhiteBalanceConstrainerControls_fpBlueA_MSByte;
   int page_WhiteBalanceConstrainerControls_fpRedB_MSByte;
   int page_WhiteBalanceConstrainerControls_fpBlueB_MSByte;
   int page_WhiteBalanceConstrainerControls_fpMaximumDistanceAllowedFromLocus_MSByte;
   int page_WhiteBalanceStatisticsControls_bLowThreshold;
   int page_WhiteBalanceConstrainerControls_fEnableConstrainedWhiteBalance;
   int page_ExposureControls_bMiscSettings;

   // sensor 0 page elements
   int page_Sensor0FrameConstraints_uwVTXAddrMin_MSByte;
   int page_Sensor0FrameConstraints_uwVTYAddrMin_MSByte;
   int page_Sensor0FrameConstraints_uwVTXAddrMax_MSByte;
   int page_Sensor0FrameConstraints_uwVTYAddrMax_MSByte;
   int page_Sensor0Capabilities_uwSensorIntegrationTimeCapability_MSByte;
   int page_Sensor0Capabilities_fpSensorAnalogGainConstM1_MSByte;
   int page_Sensor0FrameConstraints_uwMaxVTLineLengthPck_MSByte;
   int page_Sensor0FrameConstraints_uwMaxVTFrameLengthLines_MSByte;
   int page_Sensor0FrameConstraints_uwMinVTFrameLengthLines_MSByte;
   int page_Sensor0FrameConstraints_uwMinVTFrameBlanking_MSByte;
   int page_Sensor0FrameConstraints_uwMinVTLineBlankingPck_MSByte;

   // sensor1 page elements
   int page_Sensor1FrameConstraints_uwVTXAddrMin_MSByte;
   int page_Sensor1FrameConstraints_uwVTYAddrMin_MSByte;
   int page_Sensor1FrameConstraints_uwVTXAddrMax_MSByte;
   int page_Sensor1FrameConstraints_uwVTYAddrMax_MSByte;
   int page_Sensor1Capabilities_uwSensorIntegrationTimeCapability_MSByte;
   int page_Sensor1Capabilities_fpSensorAnalogGainConstM1_MSByte;
   int page_Sensor1FrameConstraints_uwMaxVTLineLengthPck_MSByte;
   int page_Sensor1FrameConstraints_uwMaxVTFrameLengthLines_MSByte;
   int page_Sensor1FrameConstraints_uwMinVTFrameLengthLines_MSByte;
   int page_Sensor1FrameConstraints_uwMinVTFrameBlanking_MSByte;
   int page_Sensor1FrameConstraints_uwMinVTLineBlankingPck_MSByte;
    
} pageInitVPIP, *pageInitVPIPPtr;

// overal sensor information holder
typedef struct sensorInfoPage {
   //     t_bool present; // preserve memory deallocation for present sensor info
   sensRev revision;
   sensCompat compat;
   sensConf config;
   sensMetrLim metrics;
   sensFea features;
   sensHand handling;
   pageInitVPIP init;
} ts_sensInfo, *tps_sensInfo;

#ifdef __cplusplus
extern "C"
{
#endif

void ITE_initSensorsInfo(char * ap_ccp0_options, char * ap_ccp1_options);
int ITE_autoDetectSensors(void);

#ifdef __cplusplus
}
#endif

#endif /*ITE_SENSORINFO_H_*/
