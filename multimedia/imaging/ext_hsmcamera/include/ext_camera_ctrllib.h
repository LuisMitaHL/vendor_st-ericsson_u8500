/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_CAMERA_CTRLLIB_H_
#define _EXT_CAMERA_CTRLLIB_H_

#include <los/api/file.h>
#include <los/api/time.h>
#include <los/api/thread.h>
#include <los/api/memory.h>
#include <MMIO_Camera.h>
#include <i2c_lib.h>

#ifndef MMIO_ENABLE_NEW_IF
#include <mmio_linux_api.h>
#endif //MMIO_ENABLE_NEW_IF

#include "string.h"
#include "stdio.h"
#include "ee/api/ee.h"
#include <cm/inc/cm.hpp>
#include "ENS_Nmf.h"
#include "ENS_Component_Fsm.h"

/*************************************************************************
 * Local types and file global variables
 *************************************************************************/
#define SUPPORT_OTP_CAMERA 1
#define TRUE 1
#define FALSE 0
#define CAMCTRL_NAME_BUFFER_SIZE 50
#define CAMCTRL_VALUE_BUFFER_SIZE 12000
#define CAMCTRL_IOBUFFER_SIZE 255
#define CAMCTRL_WRITEBUFFER_SIZE 100
#define CAMCTRL_MAX_MACRO_DEPTH 10
#define CAMCTRL_MAX_NUMBER_PROPERTIES 48
#define CAMCTRL_SETUP_FILE_PRINTF_BUFFER_SIZE 100
//Maximum character that can get appended in one iterate.
#define CAMCTRL_MAX_CHAR_PER_PASS  12

//Defines for handling of I2C errors
//----------------------------------
//When an acknowledge isn't received, the command is repeated the
//number of times defined in
#define CAMCTRL_I2C_RETRIES (2)    //respectively.
//Between each retry, a wait of
#define CAMCTRL_I2C_RETRY_DELAY (2)
//milliseconds is performed.
//If still no acknowledge is received, the process is repeated
#define CAMCTRL_I2C_REPEAT_RETRIES (2)
//times after waiting for
#define CAMCTRL_I2C_REPEAT_DELAY (10)
//milliseconds. NOTE: THE REPEAT DELAY MUST BE GREATER THAN THE RETRY DELAY!!!


/*Selected Camera*/
typedef enum
{
    CAMCTRL_CAM_SEL_PRIMARY   = 0,
    CAMCTRL_CAM_SEL_SECONDARY = 1
}CamCrtl_SelectedCam_t;

/*Selected data interface*/
typedef enum
{
    CAMCTRL_CAM_DATA_INTF_CCP2 = 0,
    CAMCTRL_CAM_DATA_INTF_CSI2 = 1
}CamCtrl_DataIntf_t;

/*MIPI DPHY Specification*/
typedef enum
{
    CAMCTRL_DPHYSPEC_V0_81B = 0,
    CAMCTRL_DPHYSPEC_V0_90  = 1
}CamCtrl_DphySpec_t;

typedef enum
{
    CAMCTRL_EOTBYPASS_DIS = 0,
    CAMCTRL_EOTBYPASS_EN  = 1
}CamCtrl_EotBypass_t;

/*Configuration for CSI infterface*/
typedef struct
{
    CamCtrl_DphySpec_t  Mipi_Specs_90_81b;
    CamCtrl_EotBypass_t Eot_Bypass;
    int                 CSI_bitrate_mbps;
    t_uint8             CSI_DataType0;
    t_uint8             CSI_DataType0_PixWidth;
    t_uint8             CSI_DataType1;
    t_uint8             CSI_DataType1_PixWidth;
    t_uint8             CSI_DataType2;
    t_uint8             CSI_DataType2_PixWidth;
}CamCtrl_CsiIntf_Config_t;

/**
 * Picture size configurations
 *
 * @param ID Picture size id
 * @param Width Width in pixels of buffer containing image.
 * @param Height Height in pixels of buffer containing image.
 * @param FrameRate Number of images per 100 seconds.
 * @param CameraWidth Width in pixels of camera image.
 * @param CameraHeight Height in pixels of camera image.
 */
typedef struct
{
    int   ID;
    int  Width;
    int  Height;
    int Type;
    int  CameraWidth;
    int  CameraHeight;
} HAL_Gr_Camera_SizeConfig_t;


typedef struct
{
    int Length;
    t_uint8* String_p;
} ParsedValue_t;

typedef struct ParsedItem_t
{
    t_uint8* Name_p;
    int Id;
    ParsedValue_t Value;
    struct ParsedItem_t* Next_p;
} ParsedItem_t;

typedef struct
{
    int  Last;
    int LastIsValid;
} SavedCameraProperty_t;

typedef enum
{
    CAMCTRL_INFO_NAME = 0,
    CAMCTRL_INFO_VENDOR,
    CAMCTRL_INFO_MAJOR_VER,
    CAMCTRL_INFO_MINOR_VER,
    CAMCTRL_CAM_I2C_ADDRESS,
    CAMCTRL_CAM_INIT_RESULT,
    CAMCTRL_CAM_PING_RESULT,
    CAMCTRL_CAM_SS_FRAME_SKIP,
    CAMCTRL_CAM_VF_FRAME_SKIP,
    CAMCTRL_CAM_CC_FRAME_SKIP,
    CAMCTRL_CAM_WATCHDOG_TIMEOUT,
    CAMCTRL_CAM_INTERFACE,
    CAMCTRL_CAM_CCP2,
    CAMCTRL_CAM_CSI_BITRATE,
    CAMCTRL_CAM_CSI_DATATYPE0,
    CAMCTRL_CAM_CSI_DATATYPE1,
    CAMCTRL_CAM_CSI_DATATYPE2,
    CAMCTRL_CAM_CSI_DATATYPE0_PIXWIDTH,
    CAMCTRL_CAM_CSI_DATATYPE1_PIXWIDTH,
    CAMCTRL_CAM_CSI_DATATYPE2_PIXWIDTH,
    CAMCTRL_CAM_FOCUS_RESULT,
    CAMCTRL_SUP_SIZES,
    CAMCTRL_SEQ_INIT,
    CAMCTRL_SEQ_PING,
    CAMCTRL_SEQ_VF_START,
    CAMCTRL_SEQ_VF_STOP,
    CAMCTRL_SEQ_SS_START,
    CAMCTRL_SEQ_SS_STOP,
    CAMCTRL_SEQ_SS_TAKE,
    CAMCTRL_SEQ_SS_VERIFY,
    CAMCTRL_SEQ_FOCUS_STATUS,
    CAMCTRL_SEQ_DEINIT,
    CAMCTRL_SEQ_VF_RETURN,
    CAMCTRL_CAM_PIPE_NUMBER,
    CAMCTRL_CAM_H_VIEW_ANGLE,
    CAMCTRL_CAM_V_VIEW_ANGLE,
    CAMCTRL_CAM_FOCAL_LENGTH,
    CAMCTRL_EXIF_MAKER_NOTE,
    CAMCTRL_EXIF_X_RESOLUTION,
    CAMCTRL_EXIF_Y_RESOLUTION,
    CAMCTRL_EXIF_MAX_NUM_PIXELS_X,
    CAMCTRL_EXIF_ANALOG_GAIN,
    CAMCTRL_EXIF_DIGITAL_GAIN,
    CAMCTRL_CAM_EXPOSURE_LOCK,
    CAMCTRL_CAM_EXPOSURE_UNLOCK,
    CAMCTRL_CAM_AWB_LOCK,
    CAMCTRL_CAM_AWB_UNLOCK,
    CAMCTRL_AUTO_FLASH_ON_THRESHOLD,
    CAMCTRL_AUTO_FLASH_OFF_THRESHOLD,
    CAMCTRL_SEQ_SS_FLASH_ON,
    CAMCTRL_SEQ_SS_FLASH_OFF,
    CAMCTRL_TOUCHFOCUS_X_Y_MAXVALUE,
    CAMCTRL_MAX_ZOOM_PREVIEW_WIDTH,
    CAMCTRL_MAX_ZOOM_PREVIEW_HEIGHT,
    CAMCTRL_MAX_ZOOM_PICTURE_WIDTH,
    CAMCTRL_MAX_ZOOM_PICTURE_HEIGHT,
    CAMCTRL_SYNCHRO_FLASH_SUPPORTED,
    CAMCTRL_NUMBER_DEFINITIONS  //Always last!
} CamCtrl_DefinitionName_t;

typedef enum
{
    CAMCTRL_EXPOSURE               = 0,
    CAMCTRL_WHITE_BALANCE          = 1,
    CAMCTRL_FLICKER_MODE           = 2,
    CAMCTRL_VERTICAL_MIRROR        = 3,
    CAMCTRL_HORIZONTAL_MIRROR      = 4,
    CAMCTRL_JPEG_COMPRESSION_LEVEL = 5,
    CAMCTRL_FOCUS_MODES            = 6,
    CAMCTRL_FOCUS_MACRO            = 7,
    CAMCTRL_MANUAL_FOCUS_CONTROL   = 8,
    CAMCTRL_OPTICAL_ZOOM           = 9,
    CAMCTRL_IMAGE_EFFECT           = 10,
    CAMCTRL_CONTRAST_MODE          = 11,
    CAMCTRL_CONTRAST               = 12,
    CAMCTRL_BRIGHTNESS_MODE        = 13,
    CAMCTRL_BRIGHTNESS             = 14,
    CAMCTRL_GAMMA_MODE             = 15,
    CAMCTRL_GAMMA                  = 16,
    CAMCTRL_FLASH_LIGHT            = 17,
    CAMCTRL_MECHANICAL_SHUTTER     = 18,
    CAMCTRL_SATURATION             = 19,
    CAMCTRL_EXPOSURE_COMPENSATION  = 20,
    CAMCTRL_SCENE_MODE             = 21,
    CAMCTRL_SHARPNESS              = 22,
    CAMCTRL_ISO                    = 23,
    CAMCTRL_METERING_EXPOSURE      = 24,
    CAMCTRL_FOCUS_RANGE            = 25,
    CAMCTRL_FRAMERATE              = 26,
    CAMCTRL_TOUCHAF_REGION         = 27,
    CAMCTRL_GET_EXPOSURE_TIME      = 28,
    CAMCTRL_GET_LIGHT_VALUE        = 29,

} CamCtrl_Property;

typedef enum
{
    CAMCTRL_ISO_AUTO = 0,
    CAMCTRL_ISO_100,
    CAMCTRL_ISO_200,
    CAMCTRL_ISO_400,
    CAMCTRL_ISO_800,
    CAMCTRL_ISO_1600,
    CAMCTRL_ISO_3200
} CamCtrl_Iso;

typedef enum
{
  CAMCTRL_IMAGE_EFFECT_NONE = 0,
  CAMCTRL_IMAGE_EFFECT_GRAYSCALE,
  CAMCTRL_IMAGE_EFFECT_NEGATIVE,
  CAMCTRL_IMAGE_EFFECT_SOLARIZE,
  CAMCTRL_IMAGE_EFFECT_SEPIA,
  CAMCTRL_IMAGE_EFFECT_POSTERIZE,
  CAMCTRL_IMAGE_EFFECT_WHITEBOARD,
  CAMCTRL_IMAGE_EFFECT_BLACKBOARD,
  CAMCTRL_IMAGE_EFFECT_AQUA
} CamCtrl_ImageFilter;

typedef enum
{
  CAMCTRL_SCENE_MODE_AUTO = 0,
  CAMCTRL_SCENE_MODE_PORTRAIT,
  CAMCTRL_SCENE_MODE_LANDSCAPE,
  CAMCTRL_SCENE_MODE_NIGHT,
  CAMCTRL_SCENE_MODE_NIGHT_PORTRAIT,
  CAMCTRL_SCENE_MODE_SPORT,
  CAMCTRL_SCENE_MODE_BEACH,
  CAMCTRL_SCENE_MODE_SNOW,
  CAMCTRL_SCENE_MODE_PARTY,
  CAMCTRL_SCENE_MODE_ACTION,
  CAMCTRL_SCENE_MODE_THEATRE,
  CAMCTRL_SCENE_MODE_SUNSET,
  CAMCTRL_SCENE_MODE_STEADY_PHOTO,
  CAMCTRL_SCENE_MODE_FIREWORKS,
  CAMCTRL_SCENE_MODE_CANDLELIGHT,
  CAMCTRL_SCENE_MODE_BARCODE
} CamCtrl_SceneMode;

typedef struct
{
    int Min;
    int Max;
    int Default;
    int Last;
    int* Values_p;
    int  NumValuesStoredInValuesList;
    int Supported;
    int LastIsValid;
} PropertyProperty_t;

/**
 * Return status information.
 *
 * @param HAL_GR_I2C_SUCCESS
 * @param HAL_GR_I2C_ERROR
 */
typedef enum
{
    HAL_GR_I2C_SUCCESS = 0,
    HAL_GR_I2C_ERROR
} HAL_Gr_I2C_ErrorStatus_t;

typedef enum
{
    COMP_TYPE_EQUAL,
    COMP_TYPE_GREATER_THAN,
    COMP_TYPE_SMALLER_THAN,
    COMP_TYPE_BIT_EQUAL
} CompType_t;

//Internal struct with global driver data
typedef struct
{
    t_uint16            I2C_WriteAddress;
    t_uint16            I2C_ReadAddress;
    int                 Registers[32];
    ParsedItem_t*       ItemList_p;
    ParsedValue_t*      Definitions[CAMCTRL_NUMBER_DEFINITIONS];
    PropertyProperty_t  PropertyProperties[CAMCTRL_MAX_NUMBER_PROPERTIES];
    t_uint8             CurrentSizeId;  //For $z command
} CamCtrl_DriverData_t;

//Data for macro expansion
typedef struct
{
    t_uint8          Depth;
    ParsedValue_t    Param[16];
    t_uint8          NumParams;
} CamCtrl_MacroData_t;

//Struct to manage IO buffer
typedef struct
{
    int         CurrentByte;
    int         BytesInBuffer;
    t_uint8*    Buffer;
} CamCtrl_IO_t;

//Data needed when executing sequence; input to ExecuteSequencePart
typedef struct
{
    t_uint8*             ParamArray_p;
    CamCtrl_MacroData_t  MacroData;
    int*                 DelayTime_p;
    t_uint8              WriteBuffer[CAMCTRL_WRITEBUFFER_SIZE];
    t_uint8              WriteBufferIndex;
} CamCtrl_SequenceData_t;

/**
 * Camera property identifier
 */
typedef t_uint8 HAL_Gr_Camera_Property_t;

//Register description (for GC tester).
/**
 * Register description (for GCTester/XGAMTester).
 * @param Name Register name
 * @param ReadOnly TRUE if register only can be read
 * @param NumberOfBits Number of bits; 8/16/32
 */
typedef struct
{
    t_uint8   Name[30];
    int       ReadOnly;
    t_uint8   NumberOfBits;
} HAL_Gr_Camera_RegisterDescription_t;


/*************************************************************************
 * Forward declaration of file local functions
 *************************************************************************/
class Camera_Ctrllib
{

public:
    /* constructor */
    Camera_Ctrllib(
            bool is_primary_camera);

    void Camera_CtrlLib_ModuleInit(
            void);

    int  Camera_CtrlLib_LoadDriver(
            OMX_HANDLETYPE omxhandle);

    void LoadDriver_Continued(
            void);

    void Camera_CtrlLib_UnloadDriver(
            const CamCtrl_DriverData_t* DriverData_p,
            OMX_HANDLETYPE omxhandle);

    int  Camera_CtrlLib_IsAlive(
            const CamCtrl_DriverData_t* DriverData_p);

    void Camera_CtrlLib_FirmwareDownload();
#if SUPPORT_OTP_CAMERA
    void InitSensorForOTPMemRead5140();
    void OnReadOTPShading5140();
#endif

    int  Camera_CtrlLib_Initialize(
            const CamCtrl_DriverData_t* DriverData_p,
            int* DelayTime_p);

    int  Camera_CtrlLib_Deinitialize(
            const CamCtrl_DriverData_t* DriverData_p,
            int* DelayTime_p);

    int Camera_CtrlLib_Get_cam_interface_config(
            const CamCtrl_DriverData_t* DriverData_p,
            t_uint8* selCam,
            t_uint8* selOutputIntf,
            t_uint8 *numofLanes,
            CamCtrl_CsiIntf_Config_t *csiConfig);

    int Camera_CtrlLib_Get_ccp2_config(
            const CamCtrl_DriverData_t* DriverData_p,
            t_uint16* Ccp_Static_BPP,
            t_uint16* Ccp_Static_Data_Strobe);

    void Camera_CtrlLib_GetDriverInfo(
            const CamCtrl_DriverData_t* DriverData_p,
            const int         NameSize,
            const int         VendorSize,
            t_uint8*          Name_p,
            t_uint8*          Vendor_p,
            t_uint8*          MajorVersion_p,
            t_uint8*          MinorVersion_p,
            t_uint8*          CameraOrientation_p);

    int Camera_CtrlLib_GetSizeConfig(
            const CamCtrl_DriverData_t* DriverData_p,
            const t_uint8 Id,
            HAL_Gr_Camera_SizeConfig_t* SizeConfig_p);

    int Camera_CtrlLib_SetDigitalZoom(
            const CamCtrl_DriverData_t* DriverData_p,
            const int Value,
            int* DelayTime_p);

    int Camera_CtrlLib_SetProperty(
            const CamCtrl_DriverData_t* DriverData_p,
            const HAL_Gr_Camera_Property_t Prop,
            const int Value,
            int* const DelayTime_p);

    int Camera_CtrlLib_GetProperty(
            const CamCtrl_DriverData_t* DriverData_p,
            const HAL_Gr_Camera_Property_t Prop,
            int* const Value_p);

    int Camera_CtrlLib_GetWatchdogTimeout(
            const CamCtrl_DriverData_t* DriverData_p);

    int Camera_CtrlLib_GetPipeNumber(
            const CamCtrl_DriverData_t* DriverData_p);

    void Camera_CtrlLib_GetExifInfo(
            const CamCtrl_DriverData_t* Camera_h,
            const int         MakerNoteSize,
            t_uint8*          MakerNote_p,
            t_uint8*          XResolution_p,
            t_uint8*          YResolution_p,
            t_uint8*          MaxNumPixelsX_p,
            t_uint8*          AnalogGain_p,
            t_uint8*          DigitalGain_p);

    int Camera_CtrlLib_GetFocusStatus(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const Value_p);

    int Camera_CtrlLib_SetViewfindSize(
            const CamCtrl_DriverData_t* DriverData_p,
            const t_uint8 Id, int* const DelayTime_p);

    int Camera_CtrlLib_SetSnapshotSize(
            const CamCtrl_DriverData_t* DriverData_p,
            const t_uint8 Id,
            int* const DelayTime_p);

    int Camera_CtrlLib_StartViewFind(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    void Camera_CtrlLib_StopViewFind(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_ReturnViewFind(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_StartSnapshot(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    void Camera_CtrlLib_StopSnapshot(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_TakeSnapshot(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_VerifySnapshot(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    t_uint8 Camera_CtrlLib_GetVfFrameSkip(
            const CamCtrl_DriverData_t* DriverHandle);

    t_uint8 Camera_CtrlLib_GetCcFrameSkip(
            const CamCtrl_DriverData_t* DriverHandle);

    t_uint8 Camera_CtrlLib_GetSsFrameSkip(
            const CamCtrl_DriverData_t* DriverHandle);

    int Camera_CtrlLib_GetNeededJPEG_BufferSize(
            const CamCtrl_DriverData_t* DriverData_p,
            const int SizeId,
            int* const BufferSize_p);

    int Camera_CtrlLib_Get_LensParameters(
            const CamCtrl_DriverData_t* Camera_h,
            int * h_angle,
            int * v_angle,
            int * focal_len);
    int Camera_CtrlLib_AwbLock(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);


    int Camera_CtrlLib_AwbUnLock(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_ExposureLock(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_ExposureUnLock(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_GET_MAX_TOUCH_X_Y_VALUES(
            const CamCtrl_DriverData_t* DriverHandle);

    CamCtrl_DriverData_t* Camera_CtrlLib_GetDriverHandle()
    {
        return iDriverData_p;
    }

    void Camera_CtrlLib_GetAutoFlashThreshold(
            const CamCtrl_DriverData_t* Camera_h,
            t_uint32*  pAutoFlashOnThreshold,
            t_uint32*  pAutoFlashOffThreshold);

    void Camera_CtrlLib_GetFlashSynchroSupport(
            const CamCtrl_DriverData_t* Camera_h,
            int*  pSynchroFlashSupported);

    int Camera_CtrlLib_StartFlash(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    void Camera_CtrlLib_StopFlash(
            const CamCtrl_DriverData_t* DriverData_p,
            int* const DelayTime_p);

    int Camera_CtrlLib_Get_Supported_Zoom_Resolutions
            (const CamCtrl_DriverData_t*  DriverData_p,
            int *supported_zoom_preview_width,
            int *supported_zoom_preview_height,
            int *supported_zoom_picture_width,
            int *supported_zoom_picture_height);

private:
    /*  Parsing function */
    int ParseFile(
            CamCtrl_DriverData_t* DriverData_p);

    int HandleParsedItem(
            CamCtrl_DriverData_t* DriverData_p,
            const t_uint8* name_p,
            int nlen,
            t_uint8*  value_p,
            int vlen,
            int id);

    void StorePropertyProperties(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8* props_p,
            int plen);

    int StorePropertyValues(
            CamCtrl_DriverData_t* DriverData_p,
            const int Prop,
            const t_uint8* Sequence_p);

    void ClearUsedProperties(
            CamCtrl_DriverData_t* DriverData_p);

    void FreeParsedItems(
            CamCtrl_DriverData_t* DriverData_p);

    int InsertIntoDefinitionArray(
            CamCtrl_DriverData_t* DriverData_p,
            const t_uint8* Name_p,
            const t_uint8* Value_p,
            int ValueLength,
            int* error_p);

    int FindItem(
            const CamCtrl_DriverData_t* DriverData_p,
            const t_uint8* Name_p,
            t_uint8** Value_pp,
            int* Length_p);

    int FindItemId(
            const CamCtrl_DriverData_t* DriverData_p,
            const t_uint8* Name_p,
            int* Id_p);

    int FindItemById(
            const CamCtrl_DriverData_t* DriverData_p,
            int Id,
            t_uint8** Value_pp,
            int* Length_p);

    int FindDefinition(
            const CamCtrl_DriverData_t* DriverData_p,
            CamCtrl_DefinitionName_t Definition,
            t_uint8** Value_pp,
            int* Length_p);

    const ParsedItem_t* FindNextMatchingItem(
            const CamCtrl_DriverData_t* DriverData_p,
            const ParsedItem_t* PrevItem_p,
            const t_uint8* PartialName_p);

    int GetNextDelimiterPos(
            const t_uint8* Sequence_p,
            t_uint8 Delimiter);

    int GetNextDelimiterPos2(
            const t_uint8* Sequence_p,
            const t_uint8 Delimiter1,
            const t_uint8 Delimiter2);

    int GetValuesFromSequence(
            const t_uint8* Sequence_p,
            const int NumToGet,
            int* Values_p);

    // tools
    int ExecuteSequence(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8* szSequence,
            int SequenceLength,
            const t_uint8* InByteArray_p,
            int* DelayTime_p);

    int ExecuteSequenceWithInbuffer(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8* szSequence,
            int SequenceLength,
            const t_uint8* InByteArray_p,
            int* DelayTime_p,
            t_uint8* InBuffer_p,
            int InBufferSize);

    int ExecuteSequencePart(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8* szSequence,
            int SequenceLength,
            CamCtrl_SequenceData_t* SeqData_p,
            t_uint8** InBuffer_pp,
            int* InBufferSize_p);

    void UpdateParameterId(
            t_uint8* Sequence_p,
            int Length,
            t_sint8 ValueToAdd);

    t_uint8 ReadNextChar(
            t_uint8** szSequence_InOut_pp,
            int* CharsLeft_Out_p);

    t_uint8 ConvertDigit(t_uint8 c);

    int ConvertNumber(
            const t_uint8* Sequence_p,
            int Length, t_uint8 Base);

    int IsRegister(
            t_uint8 Register,
            t_uint8* RegisterIndex_p);

    t_uint8 GetRegisterIndex(t_uint8 Register);

    int ComparePartialStrings(
            const t_uint8* string1_p,
            const t_uint8* string2_p,
            const int NumCharsToCompare);

    void DoDelay(
            int Time);

    int GetByte(
            CamCtrl_IO_t* const IO_p,
            t_uint8* c_p);

    int GetSizeConfig(
            t_uint8* Sequence_p,
            int Length,
            HAL_Gr_Camera_SizeConfig_t* SizeConfig_p,
            int* Offset_p);

    int GetI2CAddress(
            CamCtrl_DriverData_t* DriverData_p);

    int SeqCommand_a(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_b(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_c(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_d(
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_e(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_f(
            CamCtrl_DriverData_t* DriverData_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_m(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_p(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_r(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p,
            t_uint8** InBuffer_pp, int* InBufferSize_p);

    int SeqCommand_s(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_t(
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_u(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_w(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_z(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_lt(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_gt(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    int SeqCommand_aritm(
            t_uint8 op,
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p,
            CamCtrl_SequenceData_t* SeqData_p);

    int SeqCommand_pr(
            CamCtrl_DriverData_t* DriverData_p,
            t_uint8** szSequence_p,
            int* SequenceLength_p);

    // I2C tools
    HAL_Gr_I2C_ErrorStatus_t I2C_Write(
            const t_uint8 SlaveAddress,
            const t_uint16 BytesToTransmit,
            t_uint8* Data_p,
            const t_sint8 Retries,
            const t_sint8 RetryRepeats);

    HAL_Gr_I2C_ErrorStatus_t I2C_Read(
            const t_uint8 SlaveWriteAddress,
            const t_uint16 BytesToWrite,
            const t_uint8* const ReadCommand_p,
            const t_uint8 SlaveReadAddress,
            const t_uint16 BytesToRead,
            const t_sint8 Retries,
            const t_sint8 RetryRepeats,
            t_uint8 * const Dest_p);

    HAL_Gr_I2C_ErrorStatus_t I2C_SimpleRead(
            const t_uint8 SlaveReadAddress,
            const t_uint16 BytesToRead,
            const t_sint8 Retries,
            const t_sint8 RetryRepeats,
            t_uint8* const Dest_p);

private:
    //NB! The order of the definition strings below must be
    //    the same as the corresponding entries in the enum above!
    CamCtrl_DriverData_t*    iDriverData_p;
    i2clib_handle_t          i2c_handle;
    i2c_init_params_t        init_param;

    t_uint32                 fileSize;
    bool                     status_Camera_CtrlLib_Primary;
};

#endif /* _EXT_CAMERA_CTRLLIB_H_ */
