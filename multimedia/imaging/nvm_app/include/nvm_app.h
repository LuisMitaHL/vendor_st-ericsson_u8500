#ifndef _NVM_APP_H_
#define _NVM_APP_H_

#include "OMX_Core.h"
#include "pthread.h"
#include "stecc.h"
#include "assert.h"
#include "OMX_IVCommon.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#include "mmhwbuffer_ext.h"
#include "OMX_Symbian_IVCommonExt_Ste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "IFM_Index.h"

#define Debug

/* Logging mechanism */
#ifdef Debug
#define DebugPrint(stdout, formate, args...) fprintf(stdout, formate,##args)
#else
#define DebugPrint(stdout, formate, ... ) ((void)0)
#endif

#define ReleasePrint(stdout, formate, args...) fprintf(stdout, formate,##args)


/* Sensor resolution - 8MP */
#define SENSOR_X_SIZE_8MP 3280
#define SENSOR_Y_SIZE_8MP 2464

/* Sensor resolution - 5MP */
#define SENSOR_X_SIZE_5MP 2608
#define SENSOR_Y_SIZE_5MP 1960

/* Preview frames required */
#define PREVIEW_CODE 1

/* NVM_APP/STECC Revision */
#define NVM_APP_REVISION_BUILD	120312
#define EXPECTED_STECC_REVISION_MAJOR	1
#define EXPECTED_STECC_REVISION_MINOR	1

/* File names */
#define NVMAPP_PATH_RAW_UNPROCESSED "/UnProcessed%d.raw"
#define NVMAPP_PATH_RAW_PROCESSED "/Processed%d.raw"
#define NVMAPP_PATH_JPEG_PROCESSED "/Processed%d.jpg"
#define NVMAPP_PATH_JPEG_UNPROCESSED "/UnProcessed%d.jpg"

#define PATH_CALIBRATION_FILE_TO_DELETE "/CALIBRATION_%.2X%.4X.cal"
#define PATH_NVM_OUT_FILE_TO_DELETE "/NVM_%.2X%.4X.cal"
#define PATH_NVM_OUT_FILE_RENAME "/NVM_%.2X%.4X.cal"
#define PATH_CALIBRATION_FILE_RENAME "/CALIBRATION_%.2X%.4X.cal"
#define PATH_GOLDEN_SAMPLE_FILE "/GSAMPLE_%.2X%.4X.cal"

/* OMXPort definations */
typedef enum CAMPort
{
    OMX_CAMPORT_INDEX_VPB0=0, 
    OMX_CAMPORT_INDEX_VPB1,      
    OMX_CAMPORT_INDEX_VPB2,   
    OMX_CAMPORT_INDEX_MAX
}CAMPort;

typedef enum ISPPort
{
    OMX_ISPPORT_INDEX_VPB0=0, 
    OMX_ISPPORT_INDEX_VPB1,      
    OMX_ISPPORT_INDEX_VPB2,   
    OMX_ISPPORT_INDEX_MAX
}ISPPort;

typedef enum JPEGPort	
{
    OMX_JPGPORT_INDEX_VPB0=0, 
    OMX_JPGPORT_INDEX_VPB1,         
    OMX_JPGPORT_INDEX_MAX
}JPEGPort;

/* NVMAPP error status */
typedef enum NVMAPPErrorStatus
{
    NVMAPPErrorStatus_SUCCESS = 0,
    NVMAPPErrorStatus_FAILURE
}NVMAPPErrorStatus;

/* OMX Preset mode */
typedef enum NVMAPPRAWPresetMode
{
    NVMAPPRAWPresetMode_Processed = 0,
    NVMAPPRAWPresetMode_UnProcessed
}NVMAPPRAWPresetMode;

/* NVMAPP private data */
typedef struct NVMAPPPrivateDataType
{
    OMX_HANDLETYPE hHSMCAM;
    OMX_HANDLETYPE hISPPROC;
    OMX_HANDLETYPE hJPEG;
}NVMAPPPrivateDataType;

/* Calibration feedback datatype */
typedef struct CalibrationFileDataStructType
{
    char passfail[30];
    char Tolerance[30];
    char GoldenDataR_r[50];
    char GoldenDataR_g[50];
    char GoldenDataR_b[50];
    char ComputeNvmDataC_r[50];
    char ComputeNvmDataC_g[50];
    char ComputeNvmDataC_b[50];
    char VariationVRg[30];
    char VariationVBg[30];
    char Fuse_ID1[30];
    char Fuse_ID2[30];
    char Fuse_ID3[30];
    char Fuse_ID4[30];
    CalibrationFileDataStructType()
    {
        passfail[0]='\0';
        Tolerance[0]='\0';
        GoldenDataR_r[0]='\0';
        GoldenDataR_g[0]='\0';
        GoldenDataR_b[0]='\0';
        ComputeNvmDataC_r[0]='\0';
        ComputeNvmDataC_g[0]='\0';
        ComputeNvmDataC_b[0]='\0';
        VariationVRg[0]='\0';
        VariationVBg[0]='\0';
        Fuse_ID1[0]='\0';
        Fuse_ID2[0]='\0';
        Fuse_ID3[0]='\0';
        Fuse_ID4[0]='\0';
    }
}CalibrationFileDataStructType;

/* Bool datatype */
typedef enum NVMAPPBoolean	
{
    NVMAPP_FALSE = 0,
    NVMAPP_TRUE
}NVMAPPBoolean;

/* NVMAPP mode of operation */
typedef enum NVMAPPMode	
{
    NVMAPPMode_RAW = 0,
    NVMAPPMode_CALIBRATION
}NVMAPPMode;

/* Mirroring datatype */
typedef enum NVMAPPMirroring	
{
    NVMAPPMirroring_None = 0,
    NVMAPPMirroring_Vertical,
    NVMAPPMirroring_Horizontal,
    NVMAPPMirroring_Both
}NVMAPPMirroring;

/* Input params struct datatype */
typedef struct NVMAPPInputParams
{
    unsigned char RAW_CNT;
    char BAYER_ORDER[3];
    int EXPOSURE;
    int SENSITIVITY;
    float TOLERANCE;
    float LSC_MIN_AVG_MINMAXRATIO;
    float LSC_MIN_NORMCROSSCORR;
    float LSC_MIN_SIGMA_MINMAXRATIO;
    char SENSOR_RESOLUTION[4];
    char APP_MODE[12];
    unsigned char RAW_IMAGE;
    unsigned char JPEG;
    char COLOR_FMT[6];
    unsigned char ISP_DEBUG;
    unsigned char ISP_LOGLVL;
    unsigned char FLASH;
    unsigned char FRAME_RATE;
    char MIRROR[11];
    unsigned char RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK;
    char FILE_PATH[20];
    unsigned int Focus_VCM_TotalSteps;
    unsigned int Focus_VCM_StartStep;
    unsigned int Focus_VCM_StepSize;
    unsigned char bSaveSensorNVMData;
    char aSensorNVMDataPath[50];
}NVMAPPInputParams;
	

/************************************ Function Declarations **************************************/
NVMAPPErrorStatus NVMAPPSaveSensorNVMData(void);

#endif

