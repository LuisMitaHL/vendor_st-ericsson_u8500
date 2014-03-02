
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2010 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_SensorPowerMode_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_SensorPowerMode_api.h 1.1 2010/09/06 15:03:28Z pfleming Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_SENSOR_POWERMODE_H
#define GN_SENSOR_POWERMODE_H

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
//
//  GN Sensor Power Saving Mode API Definitions
//
//*****************************************************************************

#include "gps_ptypes.h"


//*****************************************************************************
// This function is called to enable the Sensor Power Saving mode of operation.
void GN_GPS_SensorPowerMode_On(
         BL AutoConfig,         // If set to FALSE the algorithms will be configured according to
                                //    the remaining input arguments. If set to TRUE the remaining
                                //    arguments will be ignored.
         BL InComa,             // If set to FALSE the GPS ME will be set to Sleep mode when stationary.
                                // If set to TRUE the GPS ME will be set to Coma mode when stationary.
         U2 MaxComa,            // This is the maximum period (in seconds) the GNS7560 will remain in Coma
                                // or Sleep  mode, even if the platform is thought to be stationary.
                                //    The default value of 60 seconds is invoked by setting this parameter to 0.
         U2 MaxAttempt,         // This is the maximum duration (in seconds) the GNS7560 will attempt to obtain
                                //    a fix for when in StationaryAwake mode. The default value of 60 seconds is
                                //    invoked by setting this parameter to 0.
         U2 Delay );            // This is the delay (in seconds) between the sensor indicating that the platform is
                                //    stationary and the PE software transitioning to the Stationary state.


//*****************************************************************************
// This function is called to disable the Sensor Power Saving mode.
void GN_GPS_SensorPowerMode_Off( void );

//*****************************************************************************
// This function must be called whenever it is detected that the unit has stopped moving. That is,
// it alerts the GPS PE that the unit is now stationary.
void GN_GPS_SensorPowerMode_Static( void );

//*****************************************************************************
// This function must be called whenever it is detected that the unit has started moving. That is,
// it alerts the GPS PE that the unit is no longer stationary.
void GN_GPS_SensorPowerMode_Motion( void );

//*****************************************************************************
// This function may be called by the host, for it to determine if the GPS measurements indicate
// that the unit is now stationary. That is, in one implementation, the host might poll the GPS
// PE by calling this function after every fix and, if the function returns TRUE, the host might
// then call the function GN_GPS_SensorPowerMode_Static.
BL   GN_GPS_SensorPowerMode_IfStat( void );

//*****************************************************************************
// This function returns the state the library believes the chip is currently in:
//   NOT_SET:    This indicates an error case
//   RUNNING:    The chip should be searching for and tracking SVs
//   SENT_SLEEP: The chip was sent to Sleep mode (but it might have transitioned to Coma)
//   SENT_COMA:  The chip was sent to Coma mode
//*****************************************************************************

typedef enum GN_SPM_State    //
{
   GN_SPM_NOTSET,            // This indicates an error case
   GN_SPM_RUNNING,           // The chip should be searching for and tracking SVs
   GN_SPM_SENT_SLEEP,        // The chip was sent to Sleep mode (but it might have transitioned to Coma)
   GN_SPM_SENT_COMA          // The chip was sent to Coma mode

}  e_GN_SPM_State;          //

e_GN_SPM_State   GN_GPS_SensorPowerMode_Get_State( void );


#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_AGPS_FREQAID_H
