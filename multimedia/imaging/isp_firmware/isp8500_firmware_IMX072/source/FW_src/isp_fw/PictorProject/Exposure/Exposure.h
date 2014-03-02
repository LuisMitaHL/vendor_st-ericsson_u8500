/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \defgroup ExpCtrl Exposure Control Module
 * \brief This Module deals with the Exposure Algorithm applied on to the Image.
*/

/**

 * \file      Exposure.h
 * \brief     Header File containing external function declarations and defines for Exposure Algorithm and Exposure Compiler
 * \ingroup   ExpCtrl

*/
#ifndef EXPOSURE_H_
#define EXPOSURE_H_

// Includes
#include "Platform.h"
#include "ExposurePlatformSpecific.h"

//global variables

/**

 * \enum 	  AlgorithmUpdate_te
 * \brief	  Enum to give the Status of the Algorithm Part of Exposure Module.
 * \details   According to the status, the Compiler Part & the Sensor updation Part of the Exposure module are called.
 * \ingroup   ExpCtrl

*/
typedef enum
{
    /// The default value of the Algorithm Update Status.
	AlgorithmUpdate_e_Default               = 0x0,

    /// This attribute informs that only COMPILATION is needed by the Exposure Module.
	AlgorithmUpdate_e_Compile               = 0x1,

    /// This attribute informs that only SENSOR UPDATE is needed by the Exposure Module.
	AlgorithmUpdate_e_UpdateSensor          = 0x2,

    /// This attribute informs that only COMPILATION as well as SENSOR UPDATE is needed by the Exposure Module.
	AlgorithmUpdate_e_CompileAndUpdateSensor= 0x3
} AlgorithmUpdate_te;



/**

 * \enum 	  Exposure_Metering_te
 * \brief	  Enum for indicating the manner in which the zone gains are applied
 * \ingroup   ExpCtrl

*/
typedef enum
{
    /// Flat weighted accumulation done - Uniform gain associated with all pixels
	Exposure_Metering_e_Flat,

    /// Backlit weighted accumulation done
	Exposure_Metering_e_Backlit,

    /// Center weighted accumulation done - more gain associated with centre pixels
	Exposure_Metering_e_Centred,

    /// Manual Mode Accumulation, Host provides with the zone gains to be applied.
	Exposure_Metering_e_Exp_Manual,

    /// Skintone Metering for enhancing the Skin Portions of the Image
	Exposure_Metering_e_Skintone
} Exposure_Metering_te;

/**

 * \enum 	  ExposureMovement_te
 * \brief	  Enum for the Status of the movement of Desired Exposure Time in the Automatic Mode of Exposure.
 * \ingroup   ExpCtrl

*/
typedef enum
{
    /// If desired exposure is rising
	ExposureMovement_e_Rising,

    /// If desired exposure is falling
	ExposureMovement_e_Falling,

    /// If desired exposure is constant
	ExposureMovement_e_Unchanged
} ExposureMovement_te;

/**

 * \enum 	  AnalogGainMovement_te
 * \brief	  Enum for the Status of the movement of the Analog Gain.
 * \ingroup   ExpCtrl

*/
typedef enum
{
    /// If the Analog Gian value incresed from its previous value.
	AnalogGainMovement_e_Increased,

    /// If the Analog Gian value decresed from its previous value.
	AnalogGainMovement_e_Decreased,

    /// If the Analog Gian value does not change.
	AnalogGainMovement_e_NotChanged
} AnalogGainMovement_te;


/**

 * \struct 	  Exposure_CompilerStatus_ts
 * \brief	  Status Page for the Exposure Compiler.
 * \ingroup   ExpCtrl

*/
typedef struct
{
    /// Analog Gain calculated by the compiler - to be applied to the sensor.
    float_t     f_AnalogGainPending;

    /// Digital Gain calculated by the compiler - to be applied to the pixel pipe - this is
    /// multiplied with each of the channel gains as computed by the white balance module
    float_t     f_DigitalGainPending;

    /// Exposure Time as cal. by the Exposure Compiler taking the present frame rate into account.
    float_t     f_CompiledExposureTime_us;

    /// Total Current Integration Time = composite of fine integration pixels
    /// and coarse integration lines.
    uint32_t	u32_TotalIntegrationTimePending_us;

    /// Coarse Integration calculated by Exposure Compiler in terms of number of lines
    uint16_t    u16_CoarseIntegrationPending_lines;

    /// Fine Integration calculated by Exposure Compiler in terms of number of pixels
    uint16_t    u16_FineIntegrationPending_pixels;


    /// Coded Analog Gain
    uint16_t    u16_AnalogGainPending_x256;

    /// framerate   
    uint16_t    u16_frameRate_x100; 

} Exposure_CompilerStatus_ts;


/**

 * \struct 	  Exposure_ParametersApplied_ts
 * \brief	  Status Page to show the parameters applied to the Sensor/Image by the
              exposure module after it was successful in grabbing the masteri2c channel.
 * \ingroup   ExpCtrl

*/
typedef struct
{
    /// Digital Gain in coherence with the above three values which would be used for the digital gain calculations;
    /// to be applied on the pixel pipe when a frame comes with the below mentioned integration time and analog gain.
    float_t     f_DigitalGain;

    // total Integ time
    uint32_t	u32_TotalIntegrationTime_us;

    /// Coarse Integration Lines programmed on the sensor
    uint16_t    u16_CoarseIntegration_lines;

    /// Fine Integration Pixels programmed on the sensor
    uint16_t    u16_FineIntegration_pixels;

    /// Analog Gain programmed on the sensor
    uint16_t    u16_AnalogGain_x256;
} Exposure_ParametersApplied_ts;

/**

 * \struct 	Exposure_ErrorControl_ts
 * \brief	Control Page to handle any Error in the Exposure Working.
 * \ingroup ExpCtrl

*/
typedef struct
{
    /// Number of frames for which the system should wait for the applied analog gain
    /// and integration time to appear. If they do not appear within this number of frames,
    /// an error case is generated and g_fInputProcUpdateDelayed is set to FALSE forcibly.
    uint8_t u8_MaximumNumberOfFrames;
} Exposure_ErrorControl_ts;

/**

 * \struct 	  Exposure_ErrorStatus_ts
 * \brief	  Status Page to show various error conditions in the Exposure Module.
 * \ingroup   ExpCtrl

*/
typedef struct
{
    /// Gives the number of times Input Proc has been forcibly updated. Integration Time and
    /// Analog Gain applied to the sensor have not appeared in a frame for a consecutive
    /// number of frames which is as specified by the Control Page.
    uint8_t u8_NumberOfForcedInputProcUpdates;

    /// Gives the number of consecutive frames for which the analog gain and integration
    /// time have not appeared in a frame after they have been applied on the sensor.
    /// In an ideal scenario, they appear in the second frame after they are applied.
    uint8_t u8_NumberOfConsecutiveDelayedFrames;

    /// Gives the total count of frames for which the analog gain and integration
    /// time were out of sync
    uint8_t u8_ExposureSyncErrorCount;

    /// Flag which indicates to the SOF isr that it has to do the forced updation
    /// of Input Proc and let the exposure control start running again even though
    /// due to some error analog gain and integration time applied on the sensor
    /// have not appeared in the MaximumNumberOfFrames as specified by the control page
    uint8_t e_Flag_ForceInputProcUpdation;
} Exposure_ErrorStatus_ts;


/**

 * \struct 	  Exposure_DriverControls_ts
 * \brief	  Control page for the programming of Exposure Values for the SensorDriver.
 * \ingroup   ExpCtrl

*/
typedef struct
{
    /// Total Target Exposure Time to be applied on to the Sensor Driver
    uint32_t u32_TotalTargetExposureTime_us;

    /// Exposure Time to be applied on to the Sensor Driver
    uint32_t u32_TargetExposureTime_us;

    /// Analog Gain to be applied on to the Sensor Driver
    uint16_t u16_TargetAnalogGain_x256;

    /// Aperture to be applied on to the Sensor Driver
    uint16_t u16_Aperture;

    /// FlashState to be applied on to the Sensor Driver
    uint8_t u8_FlashState;

    /// DistanceFromConvergence
    uint8_t u8_DistanceFromConvergence;

    /// Flag depicting whether NDFilter should be used
    uint8_t e_Flag_NDFilter;

    /// Flag for AEC Convergence
    uint8_t e_Flag_AECConverged;
} Exposure_DriverControls_ts;


// Global Variables exported from ExposureControl.c
// to indicate that the frame is a first one for exposure control and so exposure algorithm should proceed accordingly
//extern bool_t gbo_Exposure_FirstFrame;
// Page variables  exported from Exposure.c


extern Exposure_CompilerStatus_ts       g_Exposure_CompilerStatus;
extern Exposure_ParametersApplied_ts    g_Exposure_ParametersApplied;
extern Exposure_DriverControls_ts       g_Exposure_DriverControls;
//for error management
extern Exposure_ErrorStatus_ts          g_Exposure_ErrorStatus;
extern Exposure_ErrorControl_ts         g_Exposure_ErrorControl;


//#define ClipValue(value, min, max)			    (((value) <= (min))? (min) : (((value) >= (max))? (max) : (value)))
#define ClipValue(value, min, max)			    Max((min),Min((value), (max)))
#define ClipValueFloat(value, min, max)         (value = __builtin_fpx_fmax(min, __builtin_fpx_fmin(value, max)))


// ClipValueWithRespectToMax means   Min(value, max)
#define ClipValueWithRespectToMax(value, max) 	(((value) >= (max))? (max) : (value))
// ClipValueWithRespectToMin means   Max(value,  min)
#define ClipValueWithRespectToMin(value, min) 	(((value) <= (min))? (min) : (value))
#define Min(a,b)					            (((a) >= (b))? (b) : (a))
#define Max(a,b)					            (((a) >= (b))? (a) : (b))
//#define MaxFloat(a,b)

#define EXPOSURE_STATS_PROCESSOR_6x8_ZONES PLATFORM_STATS_PROCESSOR_6x8_ZONES

#endif

