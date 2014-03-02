/*
Nokia Corporation

Filename:       cam_drv.h


---------------------------------------------------------------------------
LEGAL NOTICE
The contents of this document are proprietary and confidential property of Nokia. This document is provided
subject to confidentiality obligations of the applicable agreement(s).

This document is intended for use of Nokia’s customers and collaborators only for the purpose for which this
document is submitted by Nokia. No part of this document may be reproduced or made available to the public
or to any third party in any form or means without the prior written permission of Nokia. This document is to be
used by properly trained professional personnel. Any use of the contents in this document is limited strictly to
the use(s) specifically authorized in the applicable agreement(s) under which the document is submitted. The
user of this document may voluntarily provide suggestions, comments or other feedback to Nokia in respect of
the contents of this document ("Feedback"). Such Feedback may be used in Nokia products and related
specifications or other documentation. Accordingly, if the user of this document gives Nokia Feedback on the
contents of this document, Nokia may freely use, disclose, reproduce, license, distribute and otherwise
commercialize the Feedback in any Nokia product, technology, service, specification or other documentation.

Nokia operates a policy of ongoing development. Nokia reserves the right to make changes and improvements
to any of the products and/or services described in this document or withdraw this document at any time without
prior notice.

The contents of this document are provided "as is". Except as required by applicable law, no warranties of any
kind, either express or implied, including, but not limited to, the implied warranties of merchantability and fitness
for a particular purpose, are made in relation to the accuracy, reliability or contents of this document. NOKIA
SHALL NOT BE RESPONSIBLE IN ANY EVENT FOR ERRORS IN THIS DOCUMENT or for any loss of data
or income or any special, incidental, consequential, indirect or direct damages howsoever caused, that might
arise from the use of this document or any contents of this document.

This document and the product(s) it describes are protected by copyright according to the applicable laws.

Nokia is a registered trademark of Nokia Corporation. Other product and company names mentioned herein
may be trademarks or trade names of their respective owners.

Copyright © Nokia Corporation 2010. All rights reserved.
---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
*   @1 ABSTRACT
*-----------------------------------------------------------------------------*/
/**   @file         cam_drv.h
*     @brief        This header file gives interface to camera driver.
*/
/*----------------------------------------------------------------------------*/

#ifndef CAM_DRV_H
#define CAM_DRV_H

#include "cam_drv_platform_defs.h"
#include "cam_drv_nvm_SMIApp.h"
#include "cam_drv_version.h"

/*----------------------------------------------------------------------------*/
/*  @LEVEL2

    @2 @CONTENTS

     1 ABSTRACT
     2 CONTENTS
     3 CONSTANTS
     4 MACROS
     5 DATA STRUCTURES
     6 DATA STRUCTURES - Sensor
     7 DATA STRUCTURES - Lens
     9 DATA STRUCTURES - Shutter
     0 DATA STRUCTURES - ND filter
     10 DATA STRUCTURES - NVM
     11 DATA STRUCTURES - Common
     12 FUNCTION PROTOTYPES
@                                                                             */
/*============================================================================*/

/*------------------------------------------------------------------------------
*  @3 CONSTANTS
*-----------------------------------------------------------------------------*/
#define CAM_DRV_OS_TIMER_ID_FIRST            0    /**< ID of the first OS timer used by camera driver. */
#define CAM_DRV_OS_TIMER_NUMBER              1    /**< Number of OS timers used by camera driver. */
#define CAM_DRV_MAX_NUMBER_OF_OUTPUT_MODES  80    /**< Defines the maximum number of output modes camera driver returns. */
#define CAM_DRV_MAX_SIZE_OF_NVM            512    /**< Defines the maximum size of the camera non-volatile memory. */
#define CAM_DRV_LENGTH_OF_ID_STRING          8    /**< Length of ID string of the camera module in letters. */
#define CAM_DRV_NUMBER_OF_SERIAL_NUMBERS    16    /**< Maximum number of serial numbers reported back from the camera module */
#define CAM_DRV_LENGTH_OF_FLASH_DETAILS_IN_DCC_STRING 2 /**< Length of Flash details in the DCC string of the camera module in letters. */

/* Define special DCC commands that the driver might use */
#define CAM_DRV_DCC_COMMAND_DELAY_IN_MICROSEC   0xFFFD
#define CAM_DRV_DCC_COMMAND_DELAY_IN_MILLISEC   0xFFFE
#define CAM_DRV_DCC_COMMAND_DELAY_IN_SEC        0xFFFF
#define CAM_DRV_DCC_COMMAND_APPLY_SETTINGS_OF_BLOCK_ID 0xFFFC

/** A special "register address" in CAM_DRV_DCC_REGISTER_BLOCK_ACTUATOR_IDLE
    to hold the ID of actuator driver IC ID for which block data is valid.
    A CAM_DRV_DCC_REGISTER_BLOCK_ACTUATOR_IDLE block can have data for multiple
    driver ICs. Therefore CAM_DRV_DCC_INDEX_ACTUATOR_ID can appear multiple
    times in CAM_DRV_DCC_REGISTER_BLOCK_ACTUATOR_IDLE. Each entry marks the
    start of register entries valid for this IC. End is indicated either
    by next CAM_DRV_DCC_INDEX_ACTUATOR_ID entry or by end of block.
    Value of this index can be either and exact ID of IC (value from register 0x00)
    or 0xFF which is treated as a generic match. Generic match is valid for all
    actuator ICs unless an exact match is presnet in
    CAM_DRV_DCC_REGISTER_BLOCK_ACTUATOR_IDLE. */
#define CAM_DRV_DCC_INDEX_ACTUATOR_ID           0xFFFB
/*************************************/
/* Clock requirements of the cameras */
/*************************************/

/* PRIMARY CAMERA CLOCK REQUIREMENT */
#define CAM_DRV_CONF_PRIMARY_CAMERA_MIN_CLOCK       600
#define CAM_DRV_CONF_PRIMARY_CAMERA_TARGET_CLOCK    960
#define CAM_DRV_CONF_PRIMARY_CAMERA_MAX_CLOCK       2700

/* SECONDARY CAMERA CLOCK REQUIREMENT */
#define CAM_DRV_CONF_SECONDARY_CAMERA_MIN_CLOCK     600
#define CAM_DRV_CONF_SECONDARY_CAMERA_TARGET_CLOCK  960
#define CAM_DRV_CONF_SECONDARY_CAMERA_MAX_CLOCK     2700

/* CAMERA DRIVER VERSION */
#define CAM_DRV_API_VERSION_MAJOR       3
#define CAM_DRV_API_VERSION_MINOR       1
#if 1

#define CAM_DRV_CONF_SECONDARY_CAM_IN_USE (TRUE)
#define CAM_DRV_CONF_SMIA_SUPPORT         (TRUE)

#define CAM_DRV_CONF_SMIAPP_SUPPORT   (FALSE)

#else
#define CAM_DRV_CONF_SECONDARY_CAM_IN_USE (TRUE)
#define CAM_DRV_CONF_SMIA_SUPPORT         (FALSE)

#define CAM_DRV_CONF_SMIAPP_SUPPORT   (TRUE)
#endif

/*---------------------------------------------------------------------------*/
/** @name Enumerations - Common to all components */
/*---------------------------------------------------------------------------*/

/** External events generated by camera driver. */
typedef enum {
    CAM_DRV_AF_LENS_MOVED,             /**< AF lens has been moved succesfully to the wanted position. */
    CAM_DRV_AF_LENS_POSITION_MEASURED, /**< Position of the AF lens has been measured succesfully */
    CAM_DRV_LENS_STOPPED,              /**< Lenses have been stopped succesfully */
    CAM_DRV_ZOOM_OK,                   /**< Wanted optical zoom level has been attained succesfully */
    CAM_DRV_REGISTER_FOR_NEXT_FSC,     /**< Camera driver requires next FSC events to be passed to it */
    CAM_DRV_REGISTER_FOR_NEXT_FEC,     /**< Camera driver requires next FEC events to be passed to it */
    CAM_DRV_EVENT_ERROR,               /**< Error has occurred */
    CAM_DRV_REGISTER_FOR_FSC,          /**< Camera driver requires all FSC events to be passed to it untill informed via CAM_DRV_UNREGISTER_FOR_FSC */
    CAM_DRV_REGISTER_FOR_FEC,          /**< Camera driver requires all FEC events to be passed to it untill informed via CAM_DRV_UNREGISTER_FOR_FEC */
    CAM_DRV_UNREGISTER_FOR_FSC,        /**< Camera driver no longer requires to receive FSC events */
    CAM_DRV_UNREGISTER_FOR_FEC,        /**< Camera driver no longer requires to receive FEC events */
} CAM_DRV_EVENT_E;

/** When event "Error" happens the error code is one of these. */
typedef enum {
    CAM_DRV_EVENT_ERROR_SENSOR,         /**< There is an error with the sensor */
    CAM_DRV_EVENT_ERROR_LENS,           /**< There is an error with the lens */
    CAM_DRV_EVENT_ERROR_SHUTTER,        /**< There is an error with the shutter */
    CAM_DRV_EVENT_ERROR_ND,             /**< There is an error with the ND filter */
    CAM_DRV_EVENT_ERROR_FLASH_STROBE,   /**< There is an error with flash strobe handling */
} CAM_DRV_EVENT_ERROR_E;

/** List of OS timer ID:s needed by camera driver. */
typedef enum {
    CAM_DRV_OS_TIMER_GENERAL = CAM_DRV_OS_TIMER_ID_FIRST /**< General purpose OS timer */
} CAM_DRV_OS_TIMER_E;

/** List of interrupt timers needed by camera driver. */
typedef enum {
    CAM_DRV_INT_TIMER_1, // used by sensor driver
    CAM_DRV_INT_TIMER_2, // used by lens driver
} CAM_DRV_INT_TIMER_E;

/** List of OS resource ID:s needed by Camera driver. */
typedef enum {
    CAM_DRV_OS_RESOURCE_GENERAL, /**< General purpose OS resource */
    CAM_DRV_OS_RESOURCE_I2C      /**< OS resource used for I2C communication */
} CAM_DRV_OS_RESOURCE_E;

/** List of message ID:s needed by camera driver.
    cam_drv_handle_msg() is always called in context of the thread in which camera driver executes*/
typedef enum {
    CAM_DRV_MSG_AF_LENS_MOVED,             /**< AF lens has been moved to wanted position */
    CAM_DRV_MSG_AF_LENS_POSITION_MEASURED, /**< Position of the AF lens has been measured succesfully */
    CAM_DRV_MSG_AF_LENS_TIMER_EXPIRED,     /**< Timer for AF lens movement expired */
    CAM_DRV_MSG_LENS_STOPPED,              /**< All lens movements have been stopped */
    CAM_DRV_MSG_FSC,                       /**< FSC received. */
    CAM_DRV_MSG_FEC,                       /**< FEC received. */
    CAM_DRV_MSG_DUMP_REGISTERS,            /**< Dump Camera Registers. */
} CAM_DRV_MSG_E;

/* List of signal IDs needed by camera driver.
   cam_drv_signal is always called in interrupt context or at least in context of a higher priority thread than camera driver thread*/
typedef enum {
    CAM_DRV_SIGNAL_FSC,             /**< Signal to inform that FS code happened. */
    CAM_DRV_SIGNAL_FEC,             /**< Signal to inform that FE code happened. */
    CAM_DRV_SIGNAL_OTHER,           /**< Signal to inform that other configurable event happened. */
    CAM_DRV_SIGNAL_TIMER_1_EXPIRED, /**< Timer started upon CAM_DRV_REQUEST_START_TIMER_1 event has expired
                                         i.e. requested time amount has been elapsed */
    CAM_DRV_SIGNAL_TIMER_2_EXPIRED, /**< Timer started upon CAM_DRV_REQUEST_START_TIMER_2 event has expired
                                         i.e. requested time amount has been elapsed */
} CAM_DRV_SIGNAL_E;

/** Return values of camera driver's callback function. */
typedef enum {
    CAM_DRV_FN_OK,  /**< OK */
    CAM_DRV_FN_FAIL /**< FAILED */
} CAM_DRV_FN_RETVAL_E;

/** This enum is used for camera selection (Primary or Secondary) */
typedef enum {
    CAM_DRV_CAMERA_PRIMARY,  /**< Primary camera */
    CAM_DRV_CAMERA_SECONDARY /**< Secondary camera */
} CAM_DRV_SELECT_CAMERA_E;

/** This enum is used to select the CCP receiver where the camera module is connected */
typedef enum {
    CAM_DRV_CCP_A, /**< Selects CCP receiver A */
    CAM_DRV_CCP_B  /**< Selects CCP receiver B */
} CAM_DRV_USED_CCP_E;

/** This enum used to select the external clock where the camera module is connected */
typedef enum {
    CAM_DRV_CLOCK_A, /**< Selects clock A */
    CAM_DRV_CLOCK_B  /**< Selects clock B */
} CAM_DRV_USED_CLOCK_E;

/** Enum that defines all Camera Model ID's for all of the supported
    Camera Models. Format is CAM_DRV_NAME_MANUFACTURER
    where NAME is the name commonly used inside NOKIA.  */
typedef enum {
    CAM_DRV_CAMERA_MODEL_SMIA65_2M_HP_FOXCONN = 2,    /*  2 */
    CAM_DRV_CAMERA_MODEL_ACME_LITE_ST = 5,            /*  5 */
    CAM_DRV_CAMERA_MODEL_CARP_FOXCONN = 7,            /*  7 */
    CAM_DRV_CAMERA_MODEL_SMIA65_3M_EDOF_ST = 9,       /*  9 */
    CAM_DRV_CAMERA_MODEL_ACME_MINI_ST = 11,            /* 11 */
    CAM_DRV_CAMERA_MODEL_GANDALF_2MP_ST = 14,          /* 14 */
    CAM_DRV_CAMERA_MODEL_SARUMAN_3MP_EDOF_ST = 15,     /* 15 */
    CAM_DRV_CAMERA_MODEL_SWORDFISH_8MP_TOSHIBA = 20,
    CAM_DRV_CAMERA_MODEL_GAMBINO_8MP_TOSHIBA = 21,
    CAM_DRV_CAMERA_MODEL_BATRAY_TOSHIBA,
    CAM_DRV_CAMERA_MODEL_ACME_MINI_TOSHIBA,
    CAM_DRV_CAMERA_MODEL_LADA_TOSHIBA,
    CAM_DRV_CAMERA_MODEL_LADA_FUJINON,
    CAM_DRV_CAMERA_MODEL_UNKNOWN                  /* Always the last field in the list */
} CAM_DRV_CAMERA_MODEL_E;

/** Enum for defining different requirements for AWB data */
typedef enum {
    CAM_DRV_AWB_REQUIRED_NOT,      /**< AWB data not required at all */
    CAM_DRV_AWB_REQUIRED_CAPTURE,  /**< AWB data only required when capture is performed */
    CAM_DRV_AWB_REQUIRED_ALWAYS    /**< AWB data always required (i.e. updates also required during VF) */
} CAM_DRV_AWB_REQUIREMENTS_E;

/** Enum for defining different requirements for zoom ROI data */
typedef enum {
    CAM_DRV_ZOOM_ROI_REQUIRED_NOT,
    CAM_DRV_ZOOM_ROI_REQUIRED_ALWAYS
} CAM_DRV_ZOOM_ROI_REQUIREMENTS_E;

/** Enum for defining different physical pins of camera module that are used in POWER ON/OFF */
typedef enum {
    CAM_DRV_PHYSICAL_PIN_VOLTAGES,  /**< Refers to voltage pins - Vana and Vdig */
    CAM_DRV_PHYSICAL_PIN_CLOCK,     /**< Refers to Clock pin */
    CAM_DRV_PHYSICAL_PIN_XSHUTDOWN  /**< Refers to XSHUTDOWN pin */
} CAM_DRV_PHYSICAL_PINS_E;

/** Enum for defining additional physical pins which might be controlled by driver */
typedef enum {
    CAM_DRV_PHYSICAL_PIN_FLASH,     /**< Refers to pin that controls firing of flash */
} CAM_DRV_OPTIONAL_PHYSICAL_PINS_E;

/** Enum for defining actions that can be performed on the physical pins - activate/deactivate */
typedef enum {
   CAM_DRV_PIN_ACTION_ACTIVATE,     /**< Activate the pin - The interpretation is:
                                         For Voltage and Clock pins - Enable or apply them
                                         For XSHUTDOWN pin - Drive the pin to logic level high (1) */

   CAM_DRV_PIN_ACTION_DEACTIVATE,   /**< Deactivate the pin - The interpretation is:
                                         For Voltage and Clock pins - Disable them
                                         For XSHUTDOWN pin - Drive the pin to logic level low (0) */

   CAM_DRV_PIN_ACTION_INIT          /**< Initialise the pin to default state. At start up, if required, the clock and XSHUTDOWN pins can be driven to an initial state */
} CAM_DRV_PIN_ACTION_E;

/** Enum for defining the width of register address to be read from / written to via I2C */
typedef enum {
    CAM_DRV_I2C_REG_SIZE_0 = 0,
    CAM_DRV_I2C_REG_SIZE_8BIT,      /**< 1 byte register address e.g: 0xF0. Lens/Falsh driver chips or non-SMIA registers mostly have 1 byte register addresses */
    CAM_DRV_I2C_REG_SIZE_16BIT      /**< 2 byte register address e.g: 0xF0FF. All SMIA/SMIA++ register addresses are 2 byte */
} CAM_DRV_I2C_REG_SIZE_E;

/** Enum for defining the order of data to be read from / written to a register via I2C */
typedef enum {
    CAM_DRV_I2C_BYTE_ORDER_NORMAL = 0,     /**< Use data as it is - Big endian */
    CAM_DRV_I2C_BYTE_ORDER_REVERSED,       /**< Reverse all of the data - Little endian */
    CAM_DRV_I2C_BYTE_ORDER_REVERSED_16BIT  /**< Reverse every 16bit field - To write larger amounts of 16bit data to a device from little endian system */
} CAM_DRV_I2C_BYTE_ORDER_E;

/*---------------------------------------------------------------------------*/
/** @name Enumerations - Sensor specific */
/*---------------------------------------------------------------------------*/
/** This enum is used to select whether the sensor (or particular property of the sensor) conforms to SMIA standard */
typedef enum {
    CAM_DRV_SENS_TYPE_SMIA,    /**< Sensor (or particular property of the sensor, e.g. woi setting) conforms to SMIA standard */
    CAM_DRV_SENS_TYPE_NON_SMIA /**< Sensor (or particular property of the sensor) does not conform to SMIA standard */
} CAM_DRV_SENS_TYPE_E;

/** Enum of all possible (down)scaling modes supported by the sensor */
typedef enum {
    CAM_DRV_SENS_SCALING_NONE,          /**< Sensor does not support scaling */
    CAM_DRV_SENS_SCALING_HORIZONTAL,    /**< Sensor only supports horizontal scaling */
    CAM_DRV_SENS_SCALING_VERTICAL,      /**< Sensor only supports vertical scaling */
    CAM_DRV_SENS_SCALING_FULL_SEPARATE, /**< Sensor supports both horizontal and vertical scaling.
                                             Scaling factors in both directions can be programmed independently */
    CAM_DRV_SENS_SCALING_FULL_COMBINED  /**< Sensor supports both horizontal and vertical scaling.
                                             Scaling factors in both directions are always the same */
} CAM_DRV_SENS_SCALING_MODE_E;

/** Enum for supported gain settings of the sensor */
typedef enum {
    CAM_DRV_SENS_GAIN_NONE,    /**< Sensor does not support configurable gain */
    CAM_DRV_SENS_GAIN_GLOBAL,  /**< Sensor has one global (common) gain factor for every Bayer channel
                                    (Red, Blue, Green in Red and Green in Blue) */
    CAM_DRV_SENS_GAIN_SEPARATE /**< Sensor has separate gain factors (four in total) for every Bayer channel
                                    (Red, Blue, Green in Red and Green in Blue) */
} CAM_DRV_SENS_GAIN_TYPE_E;

/** Enum of all possible bayer data pixel orders */
typedef enum {
    CAM_DRV_SENS_PIXEL_ORDER_GrRBGb, /**< The "standard" pixel order in SMIA sensors*/
    CAM_DRV_SENS_PIXEL_ORDER_RGrGbB, /**< The "mirrored" pixel order in SMIA sensors*/
    CAM_DRV_SENS_PIXEL_ORDER_BGbGrR, /**< The "flipped" pixel order in SMIA sensors*/
    CAM_DRV_SENS_PIXEL_ORDER_GbBRGr  /**< The "mirrored and flipped" pixel order in SMIA sensors*/
} CAM_DRV_SENS_PIXEL_ORDER_E;

/** Enum that tells whether black pixels are output from the sensor */
typedef enum {
    CAM_DRV_SENS_NO_BLACK_PIXELS,       /**< Black pixels are never output from the sensor */
    CAM_DRV_SENS_ALWAYS_BLACK_PIXELS,   /**< Black pixels are always output from the sensor */
    CAM_DRV_SENS_ON_DEMAND_BLACK_PIXELS /**< Black pixels can be output from the sensor if it is wanted.
                                             However, by default they are not output. */
} CAM_DRV_SENS_BLACK_PIXEL_E;

/** Enum that tells the position of the dummy pixels in relation to the black pixels */
typedef enum {
    CAM_DRV_SENS_DUMMY_PIXELS_NONE,   /**< Dummy pixels are not available */
    CAM_DRV_SENS_DUMMY_PIXELS_INSIDE, /**< Dummy pixels are inside (located closer to the center of the image) when compared to black pixels */
    CAM_DRV_SENS_DUMMY_PIXELS_OUTSIDE /**< Dummy pixels are outside (located closer to the borders of the image) when compared to black pixels */
} CAM_DRV_SENS_DUMMY_PIXEL_POS_E;

/** Enum of all the possible modes supported by the sensor */
typedef enum {
    CAM_DRV_SENS_MODE_STILL,        /**< Normal mode, when used with cam_drv_start_vf() it means still VF (not still capture)*/
    CAM_DRV_SENS_MODE_VIDEO,        /**< Video mode. Can not be selected for still capture. */
    CAM_DRV_SENS_MODE_STILL_NIGHT,  /**< Night mode, when used with cam_drv_start_vf() it means still VF (not still capture) */
    CAM_DRV_SENS_MODE_STILL_SEQ,    /**< Multi-shot mode */
    CAM_DRV_SENS_MODE_FLASH,        /**< Flash mode. */
    CAM_DRV_SENS_MODE_AF,           /**< Autofocus (AF) mode. Can not be selected for still capture */
    CAM_DRV_SENS_MODE_HQ_VIDEO,   	/**< High quality video */
    CAM_DRV_SENS_MODE_HS_VIDEO,   	/**< High frame rate video */
    CAM_DRV_SENS_MODE_STILL_CAPTURE,/**< To be used if cam_drv_start_vf() is used for capturing still image frame*/
} CAM_DRV_SENS_MODE_E;

/** Enum for specifying output data format of the sensor when sensor is configured. */
typedef enum {
    CAM_DRV_SENS_FORMAT_RAW10,         /**< Output data format of the sensor is RAW10 */
    CAM_DRV_SENS_FORMAT_RAW8,          /**< Output data format of the sensor is RAW8 */
    CAM_DRV_SENS_FORMAT_RAW8_DPCM,     /**< Output data format of the sensor is RAW8 compressed with DPCM/PCM encoder with simple predictor */
    CAM_DRV_SENS_FORMAT_RAW10TO6_DPCM, /**< Output data format of the sensor is RAW6 compressed from RAW10 with DPCM/PCM encoder with simple predictor */
} CAM_DRV_SENS_FORMAT_E;
#define CAM_DRV_MAX_NUMBER_OF_DATA_FORMATS  4

/** Enum for specifying the used CCP mode of the sensor. */
typedef enum {
    CAM_DRV_SENS_CCP_MODE_CLOCK,  /**< Sensor uses data/clock signalling */
    CAM_DRV_SENS_CCP_MODE_STROBE  /**< Sensor uses data/strobe signalling */
} CAM_DRV_SENS_CCP_MODE_E;

/** Enum for specifying the test picture mode */
typedef enum {
    CAM_DRV_SENS_TEST_PICTURE_MODE_NONE,
    CAM_DRV_SENS_TEST_PICTURE_MODE_SOLID_COLOUR,
    CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_SOLID,
    CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_FADE,
    CAM_DRV_SENS_TEST_PICTURE_MODE_PSEUDO_RANDOM,
    CAM_DRV_SENS_TEST_PICTURE_MODE_SCALE,
    CAM_DRV_SENS_TEST_PICTURE_MODE_RAMP,
    CAM_DRV_SENS_TEST_PICTURE_MODE_FIX_DATA
} CAM_DRV_SENS_TEST_PICTURE_MODE_E;

/** Enum for Sensor Image Data Interface signalling option. Possible options are CCP2 and CSI-2 */
typedef enum {
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_CLOCK,  /**< Use CCP2 DATA/CLOCK signalling */
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CCP2_DATA_STROBE, /**< Use CCP2 DATA/STROBE signalling */
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_CSI2              /**< Use CSI-2 signalling */
} CAM_DRV_SENS_IMAGE_DATA_INTERFACE_OPTIONS_E;

/** Enum for CSI-2 lane modes. This defines the number of lanes used when using CSI-2 as Image data interface */
typedef enum {
    CAM_DRV_SENS_CSI2_LANE_MODE_1,   /**< Use 1 lane */
    CAM_DRV_SENS_CSI2_LANE_MODE_2,   /**< Use 2 lane */
    CAM_DRV_SENS_CSI2_LANE_MODE_3,   /**< Use 3 lane */
    CAM_DRV_SENS_CSI2_LANE_MODE_4,   /**< Use 4 lane */
} CAM_DRV_SENS_CSI2_LANE_MODE_SELECT_E;

/*---------------------------------------------------------------------------*/
/** @name Enumerations - Lens specific */
/*---------------------------------------------------------------------------*/
/** Enum for specifying the type of the camera's lens system when it is not fixed. */
typedef enum {
    CAM_DRV_LENS_AF,         /**< Lens system has focusing capability */
    CAM_DRV_LENS_ZOOM,       /**< Lens system can be used for optical zooming */
    CAM_DRV_LENS_AF_AND_ZOOM /**< Lens system can be used for both focusing and optical zooming */
} CAM_DRV_LENS_TYPE_E;

/** Enum for specifying whether lens is stationary or moving */
typedef enum {
    CAM_DRV_LENS_IDLE,   /**< Lens is idle */
    CAM_DRV_LENS_ACTIVE  /**< Lens is active (e.g. moving) */
} CAM_DRV_LENS_STATUS_E;

/** Enum for defining the actions that can be performed for sensor modes using the API */
typedef enum {
    CAM_DRV_MODE_CONTROL_SET,  /**< Switches the sensor to the specified mode. */
    CAM_DRV_MODE_CONTROL_GET,  /**< Retrieves the current sensor mode. */
} CAM_DRV_MODE_CONTROL_E;

/** Enum for defining the actions that can be performed for aperture using the API */
typedef enum {
    CAM_DRV_APERTURE_VALUE_SET,  /**< Set new aperture value */
    CAM_DRV_APERTURE_VALUE_GET,  /**< Get current aperture value */
} CAM_DRV_APERTURE_CONTROL_E;

/** Enum for defining the actions that can be performed for ND filter using the API */
typedef enum {
 	CAM_DRV_ND_STATE_SET,
	CAM_DRV_ND_STATE_GET,
} CAM_DRV_ND_CONTROL_E ;


/** Enum for defining the possible sensor modes that can be controlled. */
typedef enum {
    CAM_DRV_MODE_STREAMING,                /**< Streaming mode - denotes sensor is streaming. */
    CAM_DRV_MODE_SW_STANDBY,               /**< SW STANDBY mode - denotes sensor is not streaming.
                                     When used to CAM_DRV_MODE_CONTROL_SET, fast SW standby is used if supported by HW*/
    CAM_DRV_MODE_SW_STANDBY_NO_TRUNCATION, /**< SW STANDBY mode - denotes sensor is not streaming.
                                     When used to CAM_DRV_MODE_CONTROL_SET, normal standby is used even if HW supports fast SW standby*/
} CAM_DRV_MODES_E;

typedef enum {
	CAM_DRV_BINNING_NONE,
	CAM_DRV_BINNING_AVERAGE,
	CAM_DRV_BINNING_SUMMED,
	CAM_DRV_BINNING_BAYER_CORRECTED,
	CAM_DRV_BINNING_OTHER,
} CAM_DRV_BINNING_TYPE_E;

typedef enum {
	CAM_DRV_SCALING_NONE,
	CAM_DRV_SCALING_BAYER_SAMPLED,
	CAM_DRV_SCALING_COSITED_3,
	CAM_DRV_SCALING_COSITED_4,
} CAM_DRV_SCALING_TYPE_E;

typedef enum{
	CAM_DRV_FLASH_STROBE_AT_EXPOSURE_START,
	CAM_DRV_FLASH_STROBE_AT_READOUT_START,
	} CAM_DRV_FLASH_STROBE_START_POINT_E;

/** CAM_DRV_USAGE_MODE_E, check usage_restriction_bitmask field in CAM_DRV_SENS_OUTPUT_MODE_T
    for mor information */
typedef enum {
  CAM_DRV_USAGE_MODE_VF,
  CAM_DRV_USAGE_MODE_AF,
  CAM_DRV_USAGE_MODE_STILL_CAPTURE,
  CAM_DRV_USAGE_MODE_NIGHT_STILL_CAPTURE,
  CAM_DRV_USAGE_MODE_STILL_SEQ_CAPTURE,
  CAM_DRV_USAGE_MODE_VIDEO_CAPTURE,
  CAM_DRV_USAGE_MODE_NIGHT_VIDEO_CAPTURE,
  CAM_DRV_USAGE_MODE_HQ_VIDEO_CAPTURE,
  CAM_DRV_USAGE_MODE_HS_VIDEO_CAPTURE, // high frame rate video
} CAM_DRV_USAGE_MODE_E;


/*------------------------------------------------------------------------------
*  @4 MACROS
*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
*  @5 DATA STRUCTURES - Sensor
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Sensor specific */

/** Structure for specifying resolution of the sensor */
typedef struct {
    uint16 width;  /**< Width in pixels */
    uint16 height; /**< Height in pixels */
} CAM_DRV_SENS_RESOLUTION_T;

/** Structure for specifying output data format capabilities of the sensor */
typedef struct {
    unsigned int raw10    : 1; /**< Sensor can output data in RAW10 format if this bit is set */
    unsigned int raw8     : 1; /**< Sensor can output data in RAW8 format if this bit is set */
    unsigned int raw8dpcm : 1; /**< Sensor can output data in RAW10 to RAW8 DPCM/PCM compressed format using simple predictor if this bit is set */
    unsigned int raw10to6dpcm : 1; /**< Sensor can output data in RAW10 to RAW6 DPCM/PCM compressed format using simple predictor if this bit is set */
} CAM_DRV_SENS_FORMAT_CAPABILITY_T;

/** Structure for specifying supported test picture outputs */
typedef struct {
    unsigned int solid_colour_mode       : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_SOLID_COLOUR mode */
    unsigned int colour_bar_solid_mode   : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_SOLID mode */
    unsigned int colour_bar_fade_mode    : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_COLOUR_BARS_FADE mode */
    unsigned int pseudo_random_mode      : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_PSEUDO_RANDOM mode */
    unsigned int scale_mode              : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_SCALE mode */
    unsigned int ramp_mode               : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_RAMP mode */
    unsigned int fix_data_mode           : 1; /**< driver supports CAM_DRV_SENS_TEST_PICTURE_MODE_FIX_DATA mode */
} CAM_DRV_SENS_TEST_PICTURE_CAPABILITY_T;

/** Structure that defines one output mode.
    Frame rate given is the maximum possible frame rate for that
    mode, i.e. lower frame rates can be programmed. */
typedef struct {
    CAM_DRV_SENS_RESOLUTION_T   woi_res;                    /**< WOI resolution of the mode */
    CAM_DRV_SENS_RESOLUTION_T   output_res;                 /**< Output resolution of the mode */
    CAM_DRV_SENS_FORMAT_E       data_format;                /**< Data format of this mode */
    uint32                      usage_restriction_bitmask;  /**< Indicates if this mode is NOT to be used in some of CAM_DRV_USAGE_MODE_E.
                                                                 A zero value of usage_restriction_bitmask means that mode can be used with
                                                                 all CAM_DRV_USAGE_MODE_E. A set bit in usage_restriction_bitmask indicates
                                                                 the CAM_DRV_USAGE_MODE_E for which this configuration cannot be used.
                                                                 For example, a mode that cannot be used for still capture has
                                                                 (usage_restriction_bitmask = (1 << CAM_DRV_USAGE_MODE_STILL_CAPTURE))*/
    uint16                      max_frame_rate_x100;        /**< Maximum frame rate of this mode. Slower frame rates are also possible. */
} CAM_DRV_SENS_OUTPUT_MODE_T;

/** Structure for specifying all of the supported output modes.
 *  First cell of the array is always the maximum resolution.
 *  Modes are arranged: 1) Descending from largest WOI res 2)
 *  Descending from largest output res */
typedef struct {
    uint16                       number_of_modes; /**< States how many modes are defined in the modes array */
    CAM_DRV_SENS_OUTPUT_MODE_T  *p_modes;         /**< Array of available modes */
} CAM_DRV_SENS_OUTPUT_MODES_T;

/** Structure that contains gain capabilities of the sensor */
typedef struct {
    CAM_DRV_SENS_GAIN_TYPE_E    gain_type;       /**< Defines type of the gain (none, one gain for all channels or separate gains for every channel) */
    uint16                      min_gain_x1000;  /**< Minimum gain * 1000 (e.g. gain of x1 = 1000) */
    uint16                      max_gain_x1000;  /**< Maximum gain * 1000 (e.g. gain of x8 = 8000) */
    uint16                      gain_step_x1000; /**< Minimum step size of the gain * 1000 (e.g. step size of 0.02 = 20)*/
} CAM_DRV_SENS_GAIN_T;

/** Structure used to present exposure time capabilities of the sensor */
typedef struct {
    uint32 min_exp_time_us;    /**< Minimum exposure time of the sensor in microseconds */
    uint32 max_exp_time_us;    /**< Maximum exposure time of the sensor in microseconds */
    uint32 exp_time_step;      /**< Minimum step size of the exposure time in microseconds */
} CAM_DRV_SENS_EXPOSURE_T;

/** Structure for specifying orientation of the image */
typedef struct {
    unsigned int mirror : 1; /**< Image is horizontally mirrored if this bit is set */
    unsigned int flip   : 1; /**< Image is vertically flipped if this bit is set */
} CAM_DRV_SENS_ORIENTATION_T;

/** Structure for specifying pixel orders in the image when mirror or/and flip is applied */
typedef struct {
    CAM_DRV_SENS_PIXEL_ORDER_E  normal;   /**< Pixel order when neither image nor flip is applied */
    CAM_DRV_SENS_PIXEL_ORDER_E  mirrored; /**< Pixel order when image is horizontally mirrored */
    CAM_DRV_SENS_PIXEL_ORDER_E  flipped;  /**< Pixel order when image is vertically flipped */
    CAM_DRV_SENS_PIXEL_ORDER_E  mirrored_and_flipped; /**< Pixel order when image is mirrored and flipped */
} CAM_DRV_SENS_PIXEL_ORDERS_T;

/** Structure containing information about the format of the output frame. */
typedef struct {
    uint8                           anc_lines_top;        /**< number of ancillary data lines, top */
    uint8                           anc_lines_bottom;     /**< number of ancillary data lines, bottom */
    CAM_DRV_SENS_BLACK_PIXEL_E      black_pixels_capability; /**< Tells how/when sensor outputs black pixels.
                                                                  If black_pixels are output ON_DEMAND then default is that they are not output. */
    uint8                           black_pixels_left;    /**< number of black pixels, left  (if black pixels are output) */
    uint8                           black_pixels_right;   /**< number of black pixels, right (if black pixels are output) */
    uint8                           black_pixels_top;     /**< number of black pixels, top   (if black pixels are output) */
    uint8                           black_pixels_bottom;  /**< number of black pixels, bottom(if black pixels are output) */
    CAM_DRV_SENS_DUMMY_PIXEL_POS_E  dummy_pixel_position; /**< Tells whether dummy pixels are "before" or "after" black pixels */
    uint8                           dummy_pixels_left;    /**< number of dummy pixels, left */
    uint8                           dummy_pixels_right;   /**< number of dummy pixels, right */
    uint8                           dummy_pixels_top;     /**< number of dummy pixels, top */
    uint8                           dummy_pixels_bottom;  /**< number of dummy pixels, bottom */
} CAM_DRV_SENS_FRAME_FORMAT_T;

/** Structure for defining any delays that there might be after sensor is configured.
    All of the values are given as frames. */
typedef struct {
    uint8   start_vf;        /**< Delay associated with starting VF */
    uint8   capture_rolling; /**< Delay associated with starting capture with electrical rolling shutter */
    uint8   capture_global;  /**< Delay associated with starting capture with global reset enabled */
    uint8   config_woi;      /**< Delay associated with configuration of scaling factors and woi */
    uint8   config_af;       /**< Delay associated with AF configuration (exp. time, gains and framerate) */
    uint8   config_other;    /**< Delay associated with other configurations */
} CAM_DRV_SENS_CONFIG_DELAYS_T;

/** Structure to report CSI2 data type identifiers for each data format that driver may support. Non-compressed RAW data formats
    have standard identifiers specified MIPI CSI2 specifications. Compressed data formats can use any identifier in range of 0x30-0x37.
    In implementation, identifiers for only supported formats are compulsory to be filled. Driver may or may not fill in identifiers
    for non-supported data formats. Similarly driver may or may not fill these details if CSI2 is not supported at all.
    */
typedef struct {
    uint8 raw_8_dt;                                   /**< 0x2A */
    uint8 raw_10_dt;                                  /**< 0x2B */
    uint8 dpcm_10_to_8_dt;                            /**< custom e.g. 0x30 */
    uint8 dpcm_10_to_6_dt;                            /**< custom e.g. 0x32 */
    uint8 ancillary_data_dt;                          /**< typically 0x12 */
    uint8 black_pixels_dt;                            /**< Custom or possibly 0x00 to indicate that black pixels use same dt as visible pixels */
    uint8 dummy_pixels_dt;                            /**< Custom or possibly 0x00 to indicate that black pixels use same dt as visible pixels */
} CAM_DRV_SENS_CSI2_DT_IDENTIFIERS_T;

/** Structure that defines the Image data interface, CSI (CCP2 or CSI2) capabilities. In addition, fast standby control (switching to standby
    from streaming without having to wait for current frame completion) capability and Software configurable CCI address capability are also defined.
    These are bit-fields implying 1 -supported; 0 - un-supported   */

typedef struct {
    /** DHPY control capability register bits */
    unsigned int dphy_ctrl_automatic_supported: 1;
    unsigned int dphy_ctrl_UI_based_supported: 1;
    unsigned int dphy_ctrl_manual_time_and_UI_1_register_supported: 1;
    unsigned int dphy_ctrl_manual_time_and_UI_2_register_supported: 1;
    unsigned int dphy_ctrl_manual_time_register_supported: 1;

    /** CSI2 lane mode capability register bits */
    unsigned int csi2_1_lane_supported: 1;
    unsigned int csi2_2_lane_supported: 1;
    unsigned int csi2_3_lane_supported: 1;
    unsigned int csi2_4_lane_supported: 1;

    /** CSI signalling mode capability register bits */
    unsigned int csi_signalling_ccp2_data_clock_mode_supported: 1;
    unsigned int csi_signalling_ccp2_data_strobe_mode_supported: 1;
    unsigned int csi_signalling_csi2_mode_supported: 1;

    /** Fast standby capability register bits */
    unsigned int fast_standby_frame_truncation_supported: 1;

    /** CCI address control capability register bits. Allows multiple cameras to be attached to the same CCI (I2C) bus by having diffent CCI addresses */
    unsigned int sw_changeable_CCI_address_supported: 1;

    /**< Data type identifiers in CSI2 data transmission */
    CAM_DRV_SENS_CSI2_DT_IDENTIFIERS_T csi2_data_type_ids;
} CAM_DRV_SENS_CSI_SIGNALLING_OPTIONS_T;

/** Structure that defines the Ideal RAW data capabilities of the sensor */

typedef struct {
    unsigned int shading_correction_supported                                          :1; /**< Bit-field that indicates if shading correction is supported */
    unsigned int shading_correction_weight_adj_supported                               :1; /**< Bit-field that indicates if shading correction weight adjustment is supported */
    unsigned int green_imbalance_correction_supported                                  :1; /**< Bit-field that indicates if green imbalance correction is supported */
    unsigned int green_imbalance_correction_weight_adj_supported                       :1; /**< Bit-field that indicates if green imbalance correction weight adjustment is supported */
    unsigned int black_level_correction_supported                                      :1; /**< Bit-field that indicates if black level correction is supported */

    unsigned int module_mapped_couplet_correction_supported                            :1; /**< Bit-field that indicates if module mapped couplet correction is supported */
    unsigned int module_mapped_couplet_correction_weight_adj_manual_supported          :1; /**< Bit-field that indicates if module mapped couplet correction manual weight adjustment is supported */
    unsigned int module_mapped_couplet_correction_weight_adj_lim_auto_supported        :1; /**< Bit-field that indicates if module mapped couplet correction limited auto weight adjustment is supported */

    unsigned int module_dynamic_couplet_correction_supported                           :1; /**< Bit-field that indicates if module dynamic couplet correction is supported */
    unsigned int module_dynamic_couplet_correction_weight_adj_manual_supported         :1; /**< Bit-field that indicates if module dynamic couplet correction manual weight adjustment is supported */
    unsigned int module_dynamic_couplet_correction_weight_adj_lim_auto_supported       :1; /**< Bit-field that indicates if module dynamic couplet correction limited auto weight adjustmentis supported */

    unsigned int module_dynamic_single_pixel_correction_supported                      :1; /**< Bit-field that indicates if module dynamic single pixel correction is supported */
    unsigned int module_dynamic_single_pixel_correction_weight_adj_manual_supported    :1; /**< Bit-field that indicates if module dynamic single pixel correction manual weight adjustment is supported */
    unsigned int module_dynamic_single_pixel_correction_weight_adj_lim_auto_supported  :1; /**< Bit-field that indicates if module dynamic single pixel correction limited auto weight adjustment is supported */

    unsigned int module_dynamic_combined_correction_supported                          :1; /**< Bit-field that indicates if module dynamic combined correction is supported */
    unsigned int module_dynamic_combined_correction_weight_adj_manual_supported        :1; /**< Bit-field that indicates if module dynamic combined correction manual weight adjustment is supported */
    unsigned int module_dynamic_combined_correction_weight_adj_lim_auto_supported      :1; /**< Bit-field that indicates if module dynamic combined correction limited auto weight adjustment is supported */

    unsigned int module_mapped_line_correction_supported                               :1; /**< Bit-field that indicates if module mapped line correction is supported */
    unsigned int module_mapped_line_correction_weight_adj_manual_supported             :1; /**< Bit-field that indicates if module mapped line correction manual weight adjustment is supported */
    unsigned int module_mapped_line_correction_weight_adj_lim_auto_supported           :1; /**< Bit-field that indicates if module mapped line correction limited auto weight adjustment is supported */

    unsigned int module_mapped_triplet_correction_supported                            :1; /**< Bit-field that indicates if module mapped triplet correction is supported */
    unsigned int module_mapped_triplet_correction_weight_adj_manual_supported          :1; /**< Bit-field that indicates if module mapped triplet correction manual weight adjustment is supported */
    unsigned int module_mapped_triplet_correction_weight_adj_lim_auto_supported        :1; /**< Bit-field that indicates if module mapped triplet correction limited auto weight adjustment is supported */

    unsigned int module_dynamic_triplet_correction_supported                           :1; /**< Bit-field that indicates if module dynamic triplet correction is supported */
    unsigned int module_dynamic_triplet_correction_weight_adj_manual_supported         :1; /**< Bit-field that indicates if module dynamic triplet correction manual weight adjustment is supported */
    unsigned int module_dynamic_triplet_correction_weight_adj_lim_auto_supported       :1; /**< Bit-field that indicates if module dynamic triplet correction limited auto weight adjustment is supported */

    unsigned int module_dynamic_line_correction_supported                              :1; /**< Bit-field that indicates if module dynamic line correction is supported */
    unsigned int module_dynamic_line_correction_weight_adj_manual_supported            :1; /**< Bit-field that indicates if module dynamic line correction manual weight adjustment is supported */
    unsigned int module_dynamic_line_correction_weight_adj_lim_auto_supported          :1; /**< Bit-field that indicates if module dynamic line correction limited auto weight adjustment is supported */

    unsigned int module_specific_correction_supported                                  :1; /**< Bit-field that indicates if module specific correction is supported */
    unsigned int module_specific_correction_weight_adj_manual_supported                :1; /**< Bit-field that indicates if module specific correction manual weight adjustment is supported */
    unsigned int module_specific_correction_weight_adj_lim_auto_supported              :1; /**< Bit-field that indicates if module specific correction limited auto weight adjustment is supported */

} CAM_DRV_SENS_IDEAL_RAW_T;

/** Structure containing details of a sensor.  */
typedef struct {
    CAM_DRV_SENS_OUTPUT_MODES_T             output_mode_capability;   /**< Output mode capabilities of the sensor */
    CAM_DRV_SENS_FORMAT_CAPABILITY_T        format_capability;        /**< Output data format capabilities of the sensor */
    CAM_DRV_SENS_GAIN_T                     analog_gain_capability;   /**< Analog gain capabilities of the sensor */
    CAM_DRV_SENS_GAIN_T                     digital_gain_capability;  /**< Digital gain capabilities of the sensor */
    CAM_DRV_SENS_EXPOSURE_T                 exposure_time_capability; /**< Exposure time capabilities of the sensor */
    CAM_DRV_SENS_ORIENTATION_T              orientation_capability;   /**< Orientation capabilities of the sensor */
    CAM_DRV_SENS_PIXEL_ORDERS_T             pixel_orders;             /**< Describes the pixel order of the image when mirror or/and flip is applied */
    CAM_DRV_SENS_FRAME_FORMAT_T             frame_format;             /**< Format of the output frame (e.g. number of ancillary line, location of black pixels, etc.) */
    CAM_DRV_SENS_CONFIG_DELAYS_T            delays;                   /**< When sensor is configured any delays associated are given here */
    CAM_DRV_SENS_CSI_SIGNALLING_OPTIONS_T   csi_signalling_options;   /**< Details of the CSI data interface i.e, Image data interface signalling options*/
    CAM_DRV_SENS_IDEAL_RAW_T                ideal_raw_data_capability;/**< Details of Ideal RAW data capability */
    CAM_DRV_SENS_CCP_MODE_E                 ccp_mode;                 /**< The CCP mode that the sensor uses to send data to CCP receiver is defined here */
    CAM_DRV_AWB_REQUIREMENTS_E              awb_requirements;         /**< Tells AWB requirements of the camera */
    CAM_DRV_ZOOM_ROI_REQUIREMENTS_E         zoom_roi_requirements;    /**< Tells zoom ROI requirements of the camera */
    CAM_DRV_SENS_TEST_PICTURE_CAPABILITY_T  test_picture_capability;  /**< Supported Test picture modes */
} CAM_DRV_SENS_DETAILS_T;

/** Structure that holds the interpreted sensor settings of the current frame.\n
 *  This data is read from the ancillary data lines if they are available and directly from the sensor registers if ancillary lines are not available.*/
typedef struct {
    uint32  exposure_time_us;   /**< Exposure time of the current frame in microseconds*/
    uint16  analog_gain_x1000;  /**< Analog gain of the current frame * 1000*/
    uint16  digital_gain_x1000; /**< Digital gain of the current frame * 1000 */
	bool8   ms_used;
	bool8   flash_fired;
} CAM_DRV_SENS_SETTINGS_T;

/** Structure used to configure WOI of the sensor.\n
 *
 *  Usage examples with VGA sensor:\n
 *  1) Full 640x480    image output: output_res = {640, 480}, woi_res = {640, 480}\n
 *  2) Cropped 320x240 image output: output_res = {320, 240}, woi_res = {320, 240}\n
 *  3) Downscaled 640x480 -> 320x240 image output: output_res = {320, 240}, woi_res = {640, 480}\n
 *  4) Cropped 320x240 image downscaled to half of the original size (160x120): output_res = {160, 120}, woi_res = {320, 240}*/
typedef struct {
    /** Output frame resolution of the sensor (e.g. number of ouput visible pixels in horizontal and vertical direction).\n
     *  On top of this there are the ancillary lines, black pixels and dummy pixels as defined in CAM_DEV_SENS_FRAME_FORMAT_T.\n
     *  Please see CAM_DRV_CONFIG_VF_T for restrictions! */
    CAM_DRV_SENS_RESOLUTION_T     output_res;
    /** This is the cropped resolution from the center of the frame.
     *  If this is smaller than the maximum res then we are cropping from the center of the image.*/
    CAM_DRV_SENS_RESOLUTION_T     woi_res;
} CAM_DRV_SENS_CONFIG_WOI_T;

/** Structure used to configure gains and exposure time of the sensor. */
/* TODO: There is no possibility to set individual gains for different channels. Always just one global gain used. */
typedef struct {
    uint32   exposure_time_us;   /**< Exposure time of the sensor in microseconds */
    uint16   analog_gain_x1000;  /**< Analog gain of the sensor * 1000 */
    uint16   digital_gain_x1000; /**< Digital gain of the sensor * 1000 */
} CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T;

/** Structure used to configure frame rate of the sensor */
typedef struct {
    uint16   frame_rate_x100;    /**< Frame rate * 100 (e.g. 25fps = 2500) */
} CAM_DRV_SENS_CONFIG_FRAME_RATE_T;

typedef struct {
    uint16 red;
    uint16 greenR;
    uint16 blue;
    uint16 greenB;
} CAM_DRV_BAYER_COLOUR_T;

/** Structure used to configure the test picture of the sensor */
typedef struct {
    CAM_DRV_SENS_TEST_PICTURE_MODE_E   test_picture_mode;  /**< Test Picture Mode (e.g. Solid Colour, Colour Bars...) */
    CAM_DRV_BAYER_COLOUR_T             test_picture_colour; /**< Only valid SOLID_COLOUR mode*/
} CAM_DRV_SENS_CONFIG_TEST_PICTURE_T;

/** Structure used to define analog/digital gain more accurately. The gain is numerator/denominator. */
typedef struct {
    uint16 numerator;       /* Numerator of the gain */
    uint16 denominator;     /* Denominator of the gain */
} CAM_DRV_SENS_GAIN_VALUE_T;

/** Structure used to configure zoom ROI of the sensor */
typedef struct {
    uint16 startX;
    uint16 startY;
    uint16 endX;
    uint16 endY;
} CAM_DRV_SENS_CONFIG_ZOOM_ROI_T;

/** Structure to specify a single flash strobe - delay_from_start_point is in number of lines and strobe_length in micro seconds */
typedef struct {
	CAM_DRV_FLASH_STROBE_START_POINT_E 	strobe_start_point;
	int32					            delay_from_start_point; // number of lines
	uint32					            strobe_length;          // micro secs
} CAM_DRV_CONFIG_FLASH_STROBE_T;

/** Structure to specify flash strobe(s) configuration.
p_each_strobe_config points to list of CAM_DRV_CONFIG_FLASH_STROBE_T items.
Client must provide strobes_per_frame number of CAM_DRV_CONFIG_FLASH_STROBE_T at p_each_strobe_config.
frame_count is the number of frames for which flash strobe sequence needs to be repeated
If flash strobe is only needed for frame captured with mechanical shutter then set global_reset_frame_only.
If global_reset_frame_only is FALSE then flash strobe is triggered for next possible frame(s) whether it is
with or without mechanical shutter.
If flash strobe modulation is supported then it can be enabled by setting do_strobe_modulation.
*/
typedef struct {
	CAM_DRV_CONFIG_FLASH_STROBE_T		*p_each_strobe_config;
	uint8 					            strobes_per_frame;	// number of strobes per frame
	uint8					            frame_count;	    // repeat set of pulses for this many frames
	bool8					            global_reset_frame_only;
	bool8					            do_strobe_modulation;
} CAM_DRV_CONFIG_FLASH_STROBES_T;


/** Structure used to configure the sensor Image data interface signalling options. Initially only CCP2 is supported. CSI-2 parameters are added here for future use */
typedef struct {
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_OPTIONS_E mode; /**< CCP2 or CSI2 image data interface */
    uint8  channel_identifier;  /**< Used to identify different data channels from each other. With CCP2 it allows the DMA channel identifier within
                                    the CCP2 embedded synchronization codes to be programmed. With CSI-2, it will select the virtual channel */
    uint16 CCP2_speed_max_Mbps;     /**< Maximum CCP2 speed supported by the client in Mbps */
    CAM_DRV_SENS_CSI2_LANE_MODE_SELECT_E CSI2_lane_mode; /**< Number of lanes used in case of CSI-2 image data interface */
} CAM_DRV_SENS_IMAGE_DATA_INTERFACE_SIGNALLING_T;

/** Structure used to inform the sensor about operating voltages used by the client */
typedef struct {
    uint16 Vana_x100;   /**< Analog voltage; typical value - 2.5V expressed as 250 */
    uint16 Vdig_x100;   /**< Digital voltage: typical value - 1.2V or 1.8V expressed as 120 or 180 respectively */
    uint16 Vio_x100;    /**< IO voltage: typical value - 1.2V or 1.8V expressed as as 120 or 180 respectively */
} CAM_DRV_SENS_OPERATING_VOLTAGES_T;
/*------------------------------------------------------------------------------
*  @6 DATA STRUCTURES - Lens
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Lens specific */

/** Structure containing important AF lens positions */
typedef struct {
    int32  rest;       /**< Default rest position of the lens which is normally used by still VF & video. Usually also consumes least energy. */
    int32  far_end;    /**< Position of the mechanical far end (infinity end) */
    int32  infinity;   /**< Lens position when camera is focused to infinity */
    int32  hyperfocal; /**< Lens position with greatest depth of field. May be sharper in hyperfocal than "rest" position but also may consumes more energy and therefore is only used in image capture. */
    int32  macro;      /**< Lens position when camera is focused to 10cm from the camera */
    int32  near_end;   /**< Position of the mechanical near end (macro end) */
} CAM_DRV_LENS_POSITIONS_T;

/* Structure containing AF lens movement times. This will be expanded if e.g. orientation demands for finer control.
   These times are for moving AF lens one measurable unit (defined in CAM_DRV_LENS_POSITIONS_T).
   -> Moving from far_end to macro will require (macro - far_end) * horizontal_us amount of time. */
typedef struct {
    uint32  horizontal_us;    /**< AF lens movement time in microseconds in default horizontal orientation for one measurable unit. */
} CAM_DRV_LENS_MOVEMENT_TIMES_T;

/** Structure containing details of a lens */
typedef struct {
    CAM_DRV_LENS_TYPE_E            type;            /**< Type of the lens system */
    CAM_DRV_LENS_POSITIONS_T       positions;       /**< Important lens positions */
    CAM_DRV_LENS_MOVEMENT_TIMES_T  movement_times;  /**< AF lens movement times */
    bool8                          af_lens_pos_measure_ability;
} CAM_DRV_LENS_DETAILS_T;

/*------------------------------------------------------------------------------
*  @8 DATA STRUCTURES - Shutter
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Shutter specific */

/** Structure containing details of a shutter */
typedef struct {
	unsigned int ms_single_capture_supported: 1;
	unsigned int ms_continuous_capture_supported: 1;
} CAM_DRV_SHUTTER_DETAILS_T;

/*------------------------------------------------------------------------------
*  @8 DATA STRUCTURES - Aperture
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Aperture specific */

/** Structure containing details of a aperture */
typedef struct {
	const uint16  *p_f_number_x_100; // number_of_supported_apertures entries, each entry is an f# x 100
	uint8   number_of_supported_apertures;
} CAM_DRV_APERTURE_DETAILS_T;

/*------------------------------------------------------------------------------
*  @9 DATA STRUCTURES - ND filter
*-----------------------------------------------------------------------------*/
/** @name Data Structures - ND filter specific */

/** Structure containing details of a ND filter
A value of 0 meaning that ND filter is not supported. Other values indicate
the transparency (Signal Level with ND / signal level without ND) in percentage.
Provided value is 100x actual value. For example, a provided value of 1250 means
a transparency of 12.5%.
*/
typedef struct {
    uint16 nd_filter_transparency_x_100;
} CAM_DRV_ND_DETAILS_T;

/*------------------------------------------------------------------------------
*  @9 DATA STRUCTURES - Flash Strobe
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Flash strobe specific */

/** Structure containing details of flash strobe support. Strobe length values are in micro seconds */
typedef struct {
	uint32 min_strobe_length;
	uint32 max_strobe_length;
	uint32 strobe_length_step;
	uint8  number_of_strobe_per_frame;
	bool8  strobe_modulation_support;
} CAM_DRV_FLASH_STROBE_DETAILS_T;

/*------------------------------------------------------------------------------
*  @11 DATA STRUCTURES - Common
*-----------------------------------------------------------------------------*/
/** @name Data Structures - Common to all components */

/** Structure for passing AWB data of the scene to the camera */
typedef struct {
    uint16 green_gain_x1000;  /**< Used gain for green pixels x 1000 */
    uint16 red_gain_x1000;    /**< Used gain for red pixels x 1000 */
    uint16 blue_gain_x1000;   /**< Used gain for green pixels x 1000 */
    uint16 color_temperature; /**< Color temperature of the scene in Kelvins */
} CAM_DRV_AWB_DATA_T;

typedef struct {
	unsigned int capture_with_ms_closed: 1; /**< if true: capture image with shutter closed i.e. a dark frame */
	unsigned int capture_continuous: 1;     /**< if true: capture continuously with global reset till stopped using VF request
	                                             or via changing power state e.g. power off. */
} CAM_DRV_SHUTTER_CONTROL_T;


/** Structure that contains identification parameters of the camera. */
typedef struct {

    char id_string[CAM_DRV_LENGTH_OF_ID_STRING];/**< Identification string for the camera. All of this information is read directly from the camera registers.\n
                                                This string can be used to exactly tell which camera module is in use.\n
                                                The format is MMIIIIRR, where
                                                MM   = Module Manufacturer ID (
                                                IIII = Module ID
                                                RR   = Module Revision.
                                                Clients should use this parameter to identify the camera sensor currently in use. */
    char camera_serial_number[CAM_DRV_NUMBER_OF_SERIAL_NUMBERS]; /* Serial number of the camera module in ASCII (or if not available then something similar) */

    CAM_DRV_CAMERA_MODEL_E camera_model; /**< This is an enumeration derived from sensor manufacturer ID and sensor ID. The client should avoid using this as it will
    not be updated for a new sensor unless the internal enum is updated accordingly. Instead id_string should be used for identification of module. This is kept here
    to satisfy legacy code */
    uint8 camera_register_map_version; /**< Register map version of the camera module */

} CAM_DRV_CAMERA_TYPE_T;

/** This union is used when events happen. It has the structure needed by the event which is defined in CAM_DRV_EVENT_E. */
typedef union {
    int32 lens_position;            /**< Data type used for CAM_DRV_LENS_MOVED and CAM_DRV_AF_LENS_POSITION_MEASURED events.
                                         The final position of the lens is given here.*/
    CAM_DRV_EVENT_ERROR_E error_id; /**< Data type used for CAM_DRV_EVENT_ERROR event.*/
} CAM_DRV_EVENT_INFO_U;

/** This union contains the structures of different messages used in the callback function for sending OS messages.\n
 *  This message type is defined in CAM_DRV_MSG_E. */
typedef union {
    uint8 dummy_bit; /**< Dummy for compilation. Filled later */
} CAM_DRV_MSG_INFO_U;

/** Structure used to configure camera driver in the initialisation. */
typedef struct {
    /** Callback function to be used by Camera driver for memory allocation.\n
     *  Function should return either pointer to allocated block or NULL if allocation failed.\n
     *  block_size - size of block to be allocated in bytes. */
    void * (*cam_drv_block_alloc_no_wait_fn)(uint32 block_size);

    /** Callback function to be used by Camera driver for memory deallocation.\n
     *  p_block: pointer to block to be deallocated. */
    void   (*cam_drv_block_dealloc_fn)(void *p_block);

    /** Callback function to be used by Camera driver for external events.\n
     *  event_code: event which occurred.\n
     *  p_event_info: pointer structure containing information about the event.\n
     *  isr_event: if set to TRUE, event occurred in interrupt context. */
    void   (*cam_drv_event_fn)(CAM_DRV_EVENT_E event_code, CAM_DRV_EVENT_INFO_U *p_event_info, bool8 isr_event);

    /** Callback function to be used by Camera driver for sending OS message.\n
     *  This function is used if Camera driver wants to send a message to itself. After client has sent the OS message, it should
     *  call cam_drv_handle_msg, where the message is handled.\n */
    CAM_DRV_FN_RETVAL_E (*cam_drv_msg_send_fn)(CAM_DRV_MSG_E msg_id, void *p_msg_info, uint16 msg_size);

    /** Callback function to be used by Camera driver for starting OS timer.\n
     *  Client should call cam_drv_handle_timer_timeout when the timer has expired. */
    void   (*cam_drv_timer_start_fn)(CAM_DRV_OS_TIMER_E timer_id, uint32 timer_ticks);

    /** Callback function to be used by Camera driver for stopping OS timer. */
    void   (*cam_drv_timer_stop_fn)(CAM_DRV_OS_TIMER_E timer_id);

    /** Callback function to be used by Camera driver for handling power actions - Activating / Deactivating Voltage, Clock and XSHUTDOWN pins */
    void   (*cam_drv_control_physical_pins_fn) (CAM_DRV_SELECT_CAMERA_E cam_id, CAM_DRV_PHYSICAL_PINS_E pin_name, CAM_DRV_PIN_ACTION_E action);

    /** Callback function to be used by Camera driver for handling I2C read requests */
    CAM_DRV_FN_RETVAL_E (*cam_drv_common_i2c_read_fn) (uint8 i2c_addr, uint16 reg_addr, CAM_DRV_I2C_REG_SIZE_E reg_addr_size, uint16 nr_of_bytes_to_read, CAM_DRV_I2C_BYTE_ORDER_E byte_order, uint8 *p_data);

    /** Callback function to be used by Camera driver for handling I2C write requests */
    CAM_DRV_FN_RETVAL_E (*cam_drv_common_i2c_write_fn) (uint8 i2c_addr, uint16 reg_addr, CAM_DRV_I2C_REG_SIZE_E reg_addr_size, uint16 nr_of_bytes_to_write, CAM_DRV_I2C_BYTE_ORDER_E byte_order, uint8 *p_data);

    /** Callback function to be used by Camera driver for notifying client that it has decided to stop (pause streaming) the sensor and hence the sensor clock lane */
    void (*cam_drv_sensor_about_to_stop_fn)();

    /** Callback function to be used by Camera driver for notifying client that it has (re-)started the sensor and hence the sensor clock lane */
    void (*cam_drv_sensor_started_fn)();

    /** Start a timer of time_in_10us duration, call cam_drv_signal() with CAM_DRV_SIGNAL_TIMER_1_EXPIRED or CAM_DRV_SIGNAL_TIMER_2_EXPIRED
         at timer expiry. When called with time_in_10us = 0 then purpose is to stop previously started timer timer_id. */
    void (*cam_drv_int_timer_fn)(CAM_DRV_INT_TIMER_E timer_id, uint32 time_in_10us );

    /** Blocking timer. Driver calls this function when it needs a busy-wait loop. Function returns after required time (= time_in_10us) elapsed. */
    void (*cam_drv_blocking_timer_fn)( uint32 time_in_10us );

    /** Callback function to be used by Camera driver to control some additional GPIOs. This control is optional. Client can pass a NULL pointer
        if there is no additional GPIO to be controlled by driver or the pointed function can return an error to indicate that requested
        CAM_DRV_OPTIONAL_PHYSICAL_PINS_E is not supported in client's implementation */
    CAM_DRV_FN_RETVAL_E (*cam_drv_control_additional_physical_pins_fn) (CAM_DRV_SELECT_CAMERA_E cam_id, CAM_DRV_OPTIONAL_PHYSICAL_PINS_E pin_name, CAM_DRV_PIN_ACTION_E action);

} CAM_DRV_CONFIG_INIT_T;

/** Structure that defines the interface requirements of a camera module. */
typedef struct {
    uint32               min_clock_freq_x100;    /**< Minimum external clock frequency in MHz * 100 (e.g. 9.6MHz = 960) */
    uint32               target_clock_freq_x100; /**< Target (wanted/ideal) external clock frequency in MHz * 100 (e.g. 19.6MHz = 1960) */
    uint32               max_clock_freq_x100;    /**< Maximum external clock frequency in MHz * 100 (e.g. 27MHz = 2700) */
    CAM_DRV_USED_CCP_E   ccp_receiver_used;      /**< Tells to which CCP receiver (A or B) the camera module is connected to */
    CAM_DRV_USED_CLOCK_E clock_used;             /**< Tells to which external clock output (A or B) the camera module is connected to */
} CAM_DRV_INTERFACE_T;

/** Structure that defines the interface requirements of the whole camera system (up to 2 cameras) */
typedef struct {
    CAM_DRV_INTERFACE_T *p_primary_camera_interface;   /**< Interface of the primary camera.
                                                            If pointer is NULL then that interface/camera is not in use. */
    CAM_DRV_INTERFACE_T *p_secondary_camera_interface; /**< Interface of the secondary camera.
                                                            If pointer is NULL then that interface/camera is not in use. */
} CAM_DRV_INTERFACE_REQ_T;

/** Structure used for configuring camera driver when camera is turned on */
typedef struct {
    uint32 ext_clock_freq_x100;            /**< External clock freq set by client * 100 (e.g 15MHz = 1500) */
    CAM_DRV_SELECT_CAMERA_E selected_cam;  /**< Camera that is to be turned on */
    CAM_DRV_SENS_IMAGE_DATA_INTERFACE_SIGNALLING_T image_data_interface_settings; /**< Image data interface settings */
    CAM_DRV_SENS_OPERATING_VOLTAGES_T operating_voltages; /**< Operating voltages i.e., Vana, Vdig and Vio of the sensor provided by the client in Volts.
                                                               All values are expressed as multiplied by 100. For eg: 1.8V = 180 */
    uint32 requested_link_bit_rate_mbps;    /**< Target bitrate for CSI-2 transmission. Used in cases where the camera module requires knowledge of the
                                                 requested bitrate from the sensor. Ignored in case of CCP2 */
} CAM_DRV_CONFIG_ON_T;

/** Structure used to configure VF when it is started. \n
 *  The following restrictions should be taken into account by the client:
 *  1) output_res can not be larger than woi_res because it would mean upscaling.
 *  2) There is a restriction in CCP2 that the number of pixels between the LS and LE sync codes must be multiple of 4 pixels for RAW8 modes
 *     and multiple of 16 pixels for RAW10 mode. When output_res is programmed this needs to be taken into account by the client.
 *     e.g. if there is 2 dummy columns and wanted resolution would be 1600x1200 then the client should program width of 1602 pixels
 *     for RAW8 and 1614 pixels for RAW10 in order to fulfill this requirement. If this is not done then camera driver automatically does
 *     this and reports the programmed output size to the client in CAM_DRV_CONFIG_FEEDBACK_T structure. */
typedef struct {
    /** Resolution of the frames (only visible pixels) to be output from the sensor.\n
     *  On top of this there are the ancillary lines, black pixels and dummy pixels as defined in CAM_DEV_SENS_FRAME_FORMAT_T. */
    CAM_DRV_SENS_RESOLUTION_T   output_res;
    /** Resolution of the WOI (calculated from max sensor resolution). Camera driver
     *  assumes that cropping is done from the centre of the sensor. */
    CAM_DRV_SENS_RESOLUTION_T   woi_res;
    CAM_DRV_SENS_FORMAT_E       format;             /**< Output data format of the frame */
    CAM_DRV_SENS_MODE_E         sensor_mode;        /**< Sensor mode (normal, video, night or AF) */
    CAM_DRV_SENS_ORIENTATION_T  orientation;        /**< Orientation of the image */
    uint32                      min_line_length_pck;/**< Minimum line length pixel clock required by client, set 0 if client does not have minimum line length requirement*/
    uint32                      exposure_time_us;   /**< Wanted exposure time */
    uint16                      analog_gain_x1000;  /**< Wanted analog gain * 1000 */
    uint16                      digital_gain_x1000; /**< Wanted digital gain * 1000 */
    uint16                      frame_rate_x100;    /**< Wanted frame rate * 100. So e.g. 9.75 fps = 975. */
    uint8                       ac_frequency;       /**< AC frequency that is in use. This is used for anti-mains flicker.
                                                         If this is 0 then anti-mains flicker is disabled. */
    bool8                       trial_only;         /**< if true: driver only returns CAM_DRV_CONFIG_FEEDBACK_T
                                                         and nothing is applied to HW */
} CAM_DRV_CONFIG_VF_T;

/** Structure used to configure capture when still image capture is started. */
typedef struct {
    /** Resolution of the frames (only visible pixels) to be output from the sensor.\n
     *  On top of this there are the ancillary lines, black pixels and dummy pixels as defined in CAM_DEV_SENS_FRAME_FORMAT_T.\n
     *  Please see CAM_DRV_CONFIG_VF_T for restrictions! */
    CAM_DRV_SENS_RESOLUTION_T  output_res;
    /** Resolution of WOI (calculated from max sensor resolution). Camera driver
     *  assumes that cropping is done from the centre of the sensor. */
    CAM_DRV_SENS_RESOLUTION_T  woi_res;
    CAM_DRV_SENS_FORMAT_E      format;                 /**< Output data format of the frame */
    CAM_DRV_SENS_MODE_E        sensor_mode;            /**< Sensor mode (normal, night or flash) */
    CAM_DRV_SENS_ORIENTATION_T orientation;            /**< Wanted orientation of the image */
    CAM_DRV_AWB_DATA_T         awb_data;               /**< AWB data of the scene */
    CAM_DRV_SHUTTER_CONTROL_T  mechanical_shutter_ctrl;/**< Details of MS usage, only applicable if use_mechanical_shutter */
    CAM_DRV_CONFIG_FLASH_STROBES_T config_flash_strobe; /**< Flash strobe config if used */
    uint32                     exposure_time_us;       /**< Wanted exposure time */
    uint16                     analog_gain_x1000;      /**< Wanted analog gain * 1000 */
    uint16                     digital_gain_x1000;     /**< Wanted digital gain * 1000 */
    bool8                      use_mechanical_shutter; /**< Option to use mechanical shutter if it is supported by the camera */
    bool8                      use_flash_strobe;       /**< Option to use flash strobe */
} CAM_DRV_CONFIG_CAPTURE_T;

/** Structure that is used to show camera run-time configuration settings that are included*/
typedef struct {
    unsigned int config_woi               : 1; /**< Window of interest (WOI) */
    unsigned int config_gain_and_exp_time : 1; /**< Exposure time and both analog and digital gain */
    unsigned int config_frame_rate        : 1; /**< Frame rate */
    unsigned int config_test_picture      : 1; /**< Test Picture */
    unsigned int config_camera_data       : 1; /**< Set camera configuration data */
    unsigned int config_awb_data          : 1; /**< AWB data of the scene */
    unsigned int config_zoom_roi          : 1; /**< Zoom ROI */
    unsigned int config_flash_strobe      : 1; /**< Flash Strobe */
    unsigned int config_sensor_mode       : 1; /**< CAM_DRV_SENS_MODE_E */
} CAM_DRV_CONFIG_CONTENTS_T;

/** Structure used to pass certain properties of the camera / sensor to camera driver as flags */
typedef struct {
    unsigned int sens_invert      : 1;  /**< Sensor needs to be inverted */
    unsigned int sens_hscale      : 1;  /**< Sensor reports no scaling but has horizontal */
    unsigned int sens_idle_stream : 1;  /**< Sensor needs to be set to streaming after idle settings */
    unsigned int sens_disable_aec : 1;  /**< Disable AEC for Sensor */
} CAM_DRV_CAMERA_FLAGS_FLAGS_T;

/** Structure used to pass configurations from the client to the camera.\n
    Configuration of the sensor is only supported at the moment.
    Last entry has to be {0, 0, 0}. */
typedef struct {
    uint8 reg_addr_hi; /**< High byte of the register address */
    uint8 reg_addr_lo; /**< Low byte of the register address */
    uint8 reg_value;   /**< Value to write to the register */
} CAM_DRV_CAMERA_CONFIGS_T;

/** Structure that holds all of the camera configuration data.
    If pointer is NULL then that data is not available. */
typedef struct {
    CAM_DRV_CAMERA_FLAGS_FLAGS_T configuration_flags;   /**< Camera / sensor specific flags */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_idle;     /**< Config data for idle mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_still_vf;                 /**< Config data for still_vf mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_video_vf;                 /**< Config data for video_vf mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_video_rec;                /**< Config data for video_rec mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_capture;                  /**< Config data for capture mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_override;                 /**< Config data that is used to override certain register reads */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_vf_night_mode;            /**< Config data for vf_night mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_capture_night_mode;       /**< Config data for capture_night mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_prod_test_mode;           /**< Config data for prod_test mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_seq_shot_mode;            /**< Configs data for seq_shot mode */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_actuator_idle;            /**< Initial settings for "lens" actuator */
    const CAM_DRV_CAMERA_CONFIGS_T * p_config_sw_config_data;           /**< SW config data */
    const void* p_config_data_all;                                      /**< All configuration data in CDCC format */
} CAM_DRV_CAMERA_CONFIGURATION_DATA_T;
/** Enumeration for all the register setting blocks in DCC that might be required by sensor driver. Note that these do not contain the binary data blocks
    in the DCC as the driver only does I2C writes which are effectively the register setting blocks of DCC. */
typedef enum
{
    CAM_DRV_DCC_REGISTER_BLOCK_IDLE = 0,
    CAM_DRV_DCC_REGISTER_BLOCK_STILL_VF,
    CAM_DRV_DCC_REGISTER_BLOCK_VIDEO_VF,
    CAM_DRV_DCC_REGISTER_BLOCK_VIDEO_REC,
    CAM_DRV_DCC_REGISTER_BLOCK_CAPTURE,
    CAM_DRV_DCC_REGISTER_BLOCK_OVERRIDE,
    CAM_DRV_DCC_REGISTER_BLOCK_STILL_VF_NIGHT_MODE,
    CAM_DRV_DCC_REGISTER_BLOCK_STILL_CAPTURE_NIGHT_MODE,
    CAM_DRV_DCC_REGISTER_BLOCK_PROD_TEST_MODE,
    CAM_DRV_DCC_REGISTER_BLOCK_SEQ_SHOT_MODE,
    CAM_DRV_DCC_REGISTER_BLOCK_STANDBY_MODE,
    CAM_DRV_DCC_REGISTER_BLOCK_ACTUATOR_IDLE, //AnttiP
    CAM_DRV_DCC_REGISTER_BLOCK_SW_CONFIG_DATA,
    MAX_NO_OF_DCC_REGISTER_BLOCKS
} CAM_DRV_DCC_REGISTER_BLOCKS_E;

/** Structure that holds DCC register block pointers and corresponding number of entries for the particular block. */
typedef struct
{
    CAM_DRV_CAMERA_CONFIGS_T * p_config_register_block;
    uint16 number_of_entries;
} CAM_DRV_DCC_REGISTER_BLOCKS_INFO_T;

/** Structure to report list of IDs of needed sub-blocks from configuration data */
typedef struct {
    const uint16*  identifiers;
	uint16 number_of_identifiers;
} CAM_DRV_CONFIG_DATA_IDENTIFIERS_T;

/** Structure used to configure the camera on the fly.*/
typedef struct {
    CAM_DRV_CONFIG_CONTENTS_T                 config_contents;           /**< Describes what configuration info is used. */
    CAM_DRV_SENS_CONFIG_WOI_T               * p_config_woi;              /**< Sensors WOI (scaling and cropping) configuration */
    CAM_DRV_SENS_CONFIG_GAIN_AND_EXP_TIME_T * p_config_gain_and_exp_time;/**< Sensors exposure time and gain configurations */
    CAM_DRV_SENS_CONFIG_FRAME_RATE_T        * p_config_frame_rate;       /**< Sensors frame rate configuration */
    CAM_DRV_SENS_CONFIG_TEST_PICTURE_T      * p_config_test_picture;     /**< Sensors test picture configuration */
    CAM_DRV_CAMERA_CONFIGURATION_DATA_T     * p_config_camera_data;      /**< Camera configuration data */
    CAM_DRV_AWB_DATA_T                      * p_config_awb_data;         /**< AWB data of the scene */
    CAM_DRV_SENS_CONFIG_ZOOM_ROI_T          * p_config_zoom_roi;         /**< Zoom ROI configuration */
    CAM_DRV_CONFIG_FLASH_STROBES_T 	        * p_config_flash_strobe;     /**< Flash strobe configuration */
    CAM_DRV_SENS_MODE_E                     * p_config_sensor_mode;      /**< Sensor mode (normal, video, night, AF etc) */
    uint32                                  min_line_length_pck;         /**< Minimum line length pixel clock required by client, set 0 if client does not have minimum line length requirement */
    bool8                                   trial_only;                  /**< if true: driver only returns CAM_DRV_CONFIG_FEEDBACK_T
                                                                              and nothing is applied to HW */
} CAM_DRV_CONFIG_T;

/** Structure that contains feedback after config has been done. */
typedef struct {
    CAM_DRV_SENS_RESOLUTION_T woi_res;      /**< Actual programmed woi_resolution of the sensor */
    CAM_DRV_SENS_RESOLUTION_T output_res;   /**< Actual programmed output_resolution of the sensor */
    CAM_DRV_SENS_GAIN_VALUE_T analog_gain;  /**< Exact analog gain configured to the sensor */
    CAM_DRV_SENS_GAIN_VALUE_T digital_gain; /**< Exact digital gain configured to the sensor */
	CAM_DRV_BINNING_TYPE_E 	  binning_type; /**< Applied binning mode. See CAM_DRV_BINNING_TYPE_E */
	CAM_DRV_SCALING_TYPE_E 	  scaling_type; /**< Applied scaling mode. See CAM_DRV_SCALING_TYPE_E */
	CAM_DRV_SENS_TEST_PICTURE_MODE_E test_pic_mode; /**< Applied test picture mode. CAM_DRV_SENS_TEST_PICTURE_MODE_NONE in non-test configuration */
	CAM_DRV_BAYER_COLOUR_T    test_picture_colour;  /**< Pixel values in SOLID_COLOUR and FIX_DATA modes */
    CAM_DRV_SENS_EXPOSURE_T   current_et_capability;/**< Exposure time capabilities of the sensor with current configuration */
    CAM_DRV_SENS_FRAME_FORMAT_T frame_format;       /**< Output frame format with current configuration */
    float  vt_pix_clk_freq;     		    /**< Video timing Pixel clock Freq */
    float  op_pix_clk_freq;     		    /**< Output Pixel clock Freq */
    uint32 line_length_pck;                 /**< Actual line length, can be equal or more than min_line_length_pck requested by client*/
	uint32 max_line_length_pck;	            /**< Max line length possible with requested configuration */
    uint32 line_readout_time_ns;            /**< Readout time of a line in ns */
    uint32 line_blanking_bits;              /**< Line blanking time in bits. This also includes sync codes, CRC code, etc. */
    uint32 ccp_speed_khz;                   /**< Configured CCP speed in kHz. */
    uint32 exposure_time_us;                /**< Actual exposure time configured to sensor in microseconds */
    uint16 exposure_time_margin_us;         /**< Amount of time that has to be left per frame "not exposing" with current settings in us */
    uint16 analog_gain_x1000;               /**< Actual analog gain configured to sensor * 1000 */
    uint16 digital_gain_x1000;              /**< Actual digital gain configured to sensor * 1000 */
    uint16 frame_rate_x100;                 /**< Actual frame rate configured to sensor * 100 */
    uint16 config_line_number;              /**< Sensor line number where config should be called next time */
    uint16 test_picture_fix_pattern_data;   /**< Output data when configured for CAM_DRV_SENS_TEST_PICTURE_MODE_FIX_DATA */
    uint16 focal_length_x100;               /**< Focal length of camera */
	uint8  binning_factor; 	                /**< 0xHV indicate HxV binning */
	uint8  sub_sampling_factor; 	        /**< 0xHV indicate H as X and V as Y increments */

} CAM_DRV_CONFIG_FEEDBACK_T;

/** Structure containing details of a NVM filter */
typedef struct {
  	uint16  size;        /**< Size of the NVM data in p_nvm_data */
	uint8  *p_nvm_data;  /**< Pointer to the NVM data. Contains all of the NVM contents as stored into the sensor. */
	uint16 parsed_nvm_size; /**< Size of the parsed NVM structure. Clients needs to pass a buffer of this size to cam_drv_get_parsed_nvm function */
} CAM_DRV_NVM_DETAILS_T;

/** Structure that gives full details of a camera module and other external components.\n
 *  If any of the pointers is NULL the the camera system does not have that functionality. */
typedef struct {
    CAM_DRV_SENS_DETAILS_T         * p_sensor_details;  /**< Details of the used sensor */
    CAM_DRV_LENS_DETAILS_T         * p_lens_details;    /**< Details of the used lens (used for autofocus/optical zoom lenses).*/
    CAM_DRV_SHUTTER_DETAILS_T      * p_shutter_details; /**< Details of global reset strobe capabilities . */
    CAM_DRV_ND_DETAILS_T           * p_nd_details;      /**< Details of the used ND filter. */
    CAM_DRV_NVM_DETAILS_T          * p_nvm_details;     /**< Details of the non-volatile memory */
    CAM_DRV_APERTURE_DETAILS_T     * p_aperture_details;/**< Details of the aperture options */
    CAM_DRV_FLASH_STROBE_DETAILS_T * p_flash_strobe_details; /**< Details of the flash strobe */
} CAM_DRV_CAMERA_DETAILS_T;

/** Structur is used for Driver/API version.
In the case of driver, major version part is incremented with every new release. New release
sets minor as 0. For a branch release (some modifications to be made on top of an older release) major part
does not change and minor is incremented.
In the case of API, minor version part is incremented every time API is changed such
that the change is backward compatible i.e. no change is needed to existing clients if those are not interested
In using newly added service. Major number is incremented every time API is changed such that existing
client code needs to be modified. If client code only needs to be re-compiled but not modified because
of API change, such change is to be considered backward compatible and only minor number should change.
*/
typedef struct {
uint32 major;
uint32 minor;
} CAM_DRV_VERSION_T;


/*------------------------------------------------------------------------------
*  @12 FUNCTION PROTOTYPES
*-----------------------------------------------------------------------------*/
CAM_DRV_RETVAL_E cam_drv_init(const CAM_DRV_CONFIG_INIT_T * p_init_config) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_get_camera_details(CAM_DRV_CAMERA_DETAILS_T * p_camera_details) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_on(const CAM_DRV_CONFIG_ON_T * p_config_on, CAM_DRV_CAMERA_TYPE_T * p_camera_type) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_off(void)TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_config_set(const CAM_DRV_CONFIG_T * p_config, CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback)TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_start_vf(const CAM_DRV_CONFIG_VF_T * p_vf_config, CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_capture(const CAM_DRV_CONFIG_CAPTURE_T * p_capture_config, CAM_DRV_CONFIG_FEEDBACK_T * p_config_feedback) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_interpret_sensor_settings(const uint8 * p_ancillary_lines, CAM_DRV_SENS_SETTINGS_T * p_sensor_settings);

CAM_DRV_RETVAL_E cam_drv_lens_move_to_pos(int32 position) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_lens_measure_pos(void)TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_lens_get_status(CAM_DRV_LENS_STATUS_E *p_status) TO_EXT_RAM2;
CAM_DRV_RETVAL_E cam_drv_lens_stop(void) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_handle_timer_timeout(CAM_DRV_OS_TIMER_E timer_id);
CAM_DRV_RETVAL_E cam_drv_handle_msg(CAM_DRV_MSG_E msg_id, const void *p_msg);
CAM_DRV_RETVAL_E cam_drv_signal(const CAM_DRV_SIGNAL_E signal_id);
CAM_DRV_RETVAL_E cam_drv_cache_nvm(void) TO_EXT_RAM2;

CAM_DRV_RETVAL_E  cam_drv_mode_control (const CAM_DRV_MODE_CONTROL_E action, CAM_DRV_MODES_E *mode_val) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_aperture_control( const CAM_DRV_APERTURE_CONTROL_E action, uint16* const p_f_number_x_100 ) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_nd_control( const CAM_DRV_ND_CONTROL_E action, bool8* const state ) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_get_config_data_identifiers(CAM_DRV_CONFIG_DATA_IDENTIFIERS_T* p_config_ids) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_get_parsed_nvm(CAM_DRV_NVM_T *p_nvm)TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_get_driver_version(CAM_DRV_VERSION_T* version) TO_EXT_RAM2;

CAM_DRV_RETVAL_E cam_drv_get_api_version(CAM_DRV_VERSION_T* version) TO_EXT_RAM2;


#endif /* CAM_DEV_CAM_H */

