/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ImgConfig.h"
#include "primary_sensor.h"
#include "baseline.h"
#include "extension.h"
#include "primary_PEList.h"
#include "SensorId.h"

#define BAYER_WIDTH_5MP 2608

#ifdef SIMULATE_NO_ISL
// For this feature, the sensor is programmed to no output its ISL (2 in numbers). With this the sensor would
// output 1960 lines of active data only; 2 out of these 1960 would be programmed in the ISP as ISLs
    #define BAYER_HEIGHT_5MP (1958+2)
#else
#define BAYER_HEIGHT_5MP (1960+2)
#endif

//Initial value of KEY_FOCAL_LENGTH was 40mm ,new value is 3.6 mm ( IEEE-754 HEX =40666666)
//Representation in Q16 format is 3.6*2^16 = 235929.6 => 235930
#define KEY_FOCAL_LENGTH 235930
#define KEY_HORIZONTAL_VIEW_ANGLE 45
#define KEY_VERTICAL_VIEW_ANGLE 31

/****************************************************************************/
/*
 * \brief   standard constructor
 * \author  monteilm
 * \Initialize properties to default values
 **/
/****************************************************************************/
CSensorPrimary::CSensorPrimary(): CSensor(ePrimaryCamera)
{
   t_uint32 i = 0;
   t_uint32 nb_element = iSensorPreBootNbElement;

   MSG0("Sensor IMX072 as Sensor0 constructor called\n");

   #define HOSTRXMAXPIXRATE 8 /*11Mpps*/
   #define SENSOR_FSENSOR_TYPE 0xE004
   #define HOSTCLOCK 200
   #define SENSORCLOCK 9.6
   #define DC_HOSTRXMAXDATARATE 400
   #define DS_HOSTRXMAXDATARATE 648.0
   #define SENSOR_START_DELAY 1000
   #define SENSOR_XSHUTDOWN_DELAY 0x100*4*4


    typedef union {
        float f_value;
        t_uint32 u32_value;
    }t_floatPE;
    t_floatPE dataConvertion;

    dataConvertion.f_value = HOSTCLOCK;
    iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_f_ClockHost_Mhz_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  dataConvertion.u32_value;

    dataConvertion.f_value = SENSOR_START_DELAY;
    iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_f_SensorStartDelay_us_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  dataConvertion.u32_value;

    dataConvertion.f_value = SENSORCLOCK;
    iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_f_SensorInputClockFreq_Mhz_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  dataConvertion.u32_value;

    dataConvertion.f_value = DS_HOSTRXMAXDATARATE;
    iSensorPreBootPEs[nb_element].pe_addr   =  VideoTimingHostInputs_f_HostRxMaxDataRate_Mbps_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  dataConvertion.u32_value;

    iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_u16_SensorXshutdownDelay_us_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  SENSOR_XSHUTDOWN_DELAY;  /* 3300 us */

    iSensorPreBootPEs[nb_element].pe_addr   =  VideoTimingHostInputs_u16_CsiRawFormat_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  0x0a0a;

    iSensorPreBootPEs[nb_element].pe_addr   =  SMS_Control_u16_CsiRawFormat_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  0x0a0a;

    iSensorPreBootPEs[nb_element].pe_addr   =  VideoTimingHostInputs_e_SensorBitsPerSystemClock_DataType_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data =  SensorBitsPerSystemClock_e_DATA_STROBE;

    iSensorPreBootPEs[nb_element].pe_addr =  SystemSetup_e_Flag_PerformIPPSetup_Byte0;
    iSensorPreBootPEs[nb_element++].pe_data = Flag_e_TRUE;


   iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_e_InputImageSource_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  InputImageSource_e_Sensor0;

   iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_e_InputImageInterface_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  InputImageInterface_CSI2_0;

   iSensorPreBootPEs[nb_element].pe_addr =    SystemSetup_u8_NumberOfCSI2DataLines_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data  =  0x2; // 2 data lane ! It's also hardecoded in siaplatformmanager.cpp

   iSensorPreBootPEs[nb_element].pe_addr   =  DataPathControl_e_Flag_Pipe0Enable_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  Flag_e_FALSE;

   iSensorPreBootPEs[nb_element].pe_addr   =  DataPathControl_e_Flag_Pipe1Enable_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  Flag_e_FALSE;

   iSensorPreBootPEs[nb_element].pe_addr   =  GPIOControl_u8_Sensor0XShutdownGPO_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x2;

   iSensorPreBootPEs[nb_element].pe_addr   =  GPIOControl_u8_Sensor1XShutdownGPO_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x3;

   #ifdef __UI_REFERENCE
   iSensorPreBootPEs[nb_element].pe_addr =  CSIControl_u16_DataLanesMapCSI2_0_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data   =  0xca;
   #endif

   iSensorPreBootPEs[nb_element].pe_addr   =  HostFrameConstraints_u8_MinimumPostScalar0LineBlanking_pixels_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x20;
   iSensorPreBootPEs[nb_element].pe_addr   =  HostFrameConstraints_u8_MinimumPostScalar1LineBlanking_pixels_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x20;

   iSensorPreBootPEs[nb_element].pe_addr   =  SystemSetup_u8_LinesToWaitAtRxStop_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x20;
   iSensorPreBootPEs[nb_element].pe_addr   =  HostFrameConstraints_u8_MinimumInterFrame_lines_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  0x21;

   iSensorPreBootPEs[nb_element].pe_addr   =  SensorPowerManagement_Control_e_Flag_EnableSMIAPP_PowerUpSequence_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  Flag_e_TRUE;

   iSensorPreBootPEs[nb_element].pe_addr   =  Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0;
   iSensorPreBootPEs[nb_element++].pe_data =  Flag_e_FALSE;

   iSensorPreBootNbElement = nb_element;
   DBC_ASSERT(iSensorPreBootNbElement < MAX_PREBOOT_PE);

   for(i=iSensorPreBootNbElement; i<MAX_PREBOOT_PE; i++)
   {
      iSensorPreBootPEs[i].pe_addr =  0;
      iSensorPreBootPEs[i].pe_data =  0;
   }

   nb_element = iSensorPostBootNbElement;

   iSensorPostBootPEs[nb_element].pe_addr = HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0;
   iSensorPostBootPEs[nb_element].pe_data = Flag_e_FALSE ;
   nb_element++;

   iSensorPostBootPEs[nb_element].pe_addr = Zoom_Control_e_Flag_PerformDynamicResolutionUpdate_Byte0;
   iSensorPostBootPEs[nb_element].pe_data = Flag_e_FALSE ;
   nb_element++;
   iSensorPostBootPEs[nb_element].pe_addr = VariableFrameRateControl_e_Flag_Byte0;
   iSensorPostBootPEs[nb_element].pe_data = Flag_e_TRUE ;
   nb_element++;

   iSensorPostBootPEs[nb_element].pe_addr = Exposure_DriverControls_u32_TargetExposureTime_us_Byte0;
   iSensorPostBootPEs[nb_element].pe_data = 40000 ;   //40ms try
   nb_element++;

   iSensorPostBootPEs[nb_element].pe_addr = Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0 ;
   iSensorPostBootPEs[nb_element].pe_data = 3*256 ;   // x3 gain
   nb_element++;

   iSensorPostBootPEs[nb_element].pe_addr = SystemSetup_e_Flag_abortRx_OnStop_Byte0 ;
   iSensorPostBootPEs[nb_element].pe_data = Flag_e_TRUE ;   // Flag_e_TRUE
   nb_element++;

   iSensorPostBootPEs[nb_element].pe_addr = SystemSetup_u8_NumOfFramesTobeSkipped_Byte0 ;
   iSensorPostBootPEs[nb_element].pe_data = 1 ;   // IMX072 give first frame as invalid, so we should skip it
   nb_element++;

   iSensorPostBootNbElement = nb_element;

   DBC_ASSERT(iSensorPostBootNbElement < MAX_POSTBOOT_PE);

   for(i=iSensorPostBootNbElement; i<MAX_POSTBOOT_PE; i++)
   {
       iSensorPostBootPEs[i].pe_addr =  0;
       iSensorPostBootPEs[i].pe_data =  0;
   }

   nb_element = iSensorSpecificNbElement;
   iSensorSpecificNbElement = nb_element;
   DBC_ASSERT(iSensorSpecificNbElement < MAX_SPECIFIC_PE);


   // SENSOR Tosh PATCH SEQUENCE TABLE:  Sensor I2C Register value to write
   // ********************************************************************
   nb_element = iSensorPatchSequenceNbElement;
   

   iSensorPatchSequenceNbElement = nb_element;

   DBC_ASSERT(nb_element < PATCH_SIZE_SEQUENCE);

   for(i=iSensorPatchSequenceNbElement; i<PATCH_SIZE_SEQUENCE; i++)
   {
      iSensorPatchSequence[i].I2CAddr =  0;
      iSensorPatchSequence[i].I2CValue =  0;
   }

   i_BayerWidth = BAYER_WIDTH_5MP;
   i_BayerHeight = BAYER_HEIGHT_5MP;

   iLensParameters.FocalLength = KEY_FOCAL_LENGTH;
   iLensParameters.HorizontolViewAngle = KEY_HORIZONTAL_VIEW_ANGLE;
   iLensParameters.VerticalViewAngle = KEY_VERTICAL_VIEW_ANGLE;
}

t_sint32 CSensorPrimary::GetSensorId(void)
{
	return LOW_LEVEL_API_SENSOR;
}

const char* CSensorPrimary::GetPeName(const t_uint32 aAddr)
{
    return primary_KPeNamesList[aAddr];
}

/****************************************************************************/
/**
 * \brief   Sensor class destructor: free memory allocated for storing PE and pages
  *
 **/
/****************************************************************************/
CSensorPrimary::~CSensorPrimary()
{
    MSG0("Sensor IMX072 on CSI destructor called\n");
}
