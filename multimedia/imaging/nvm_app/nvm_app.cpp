/******************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file    nvm_app.cpp
 *
 * \brief   This module generates rawbayer data, nvm data, JPEG 
 *          and BMP of rawbayer data which can be used in camera
 *          calibration.
 *
 * \author  ST-Ericsson
 * \author  ST-Ericsson
 **/
/******************************************************************************/
#include "IFM_Types.h"
#include "IFM_Index.h"
#include "OMX_Symbian_CameraExt_Ste.h"
#include "nvm_app.h"
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
extern "C"
{
    #include "semaphore.h"
}

/* Hardcoded default input values */
#define NVMAPP_DEFAULT_RAW_CNT 1
#define NVMAPP_DEFAULT_BAYER_ORDER "Gr"
#define NVMAPP_DEFAULT_EXPOSURE 30
#define NVMAPP_DEFAULT_SENSITIVITY 200
#define NVMAPP_DEFAULT_TOLERANCE 0.29
#define NVMAPP_DEFAULT_LSC_MIN_AVG_MINMAXRATIO 0.0
#define NVMAPP_DEFAULT_LSC_MIN_NORMCROSSCORR 0.0
#define NVMAPP_DEFAULT_LSC_MIN_SIGMA_MINMAXRATIO 0.0
#define NVMAPP_DEFAULT_SENSOR_RESOLUTION "8MP"
#define NVMAPP_DEFAULT_APP_MODE "CALIBRATION"
#define NVMAPP_DEFAULT_RAW_IMAGE NVMAPP_FALSE
#define NVMAPP_DEFAULT_JPEG NVMAPP_TRUE
#define NVMAPP_DEFAULT_COLOR_FMT "12BIT"
#define NVMAPP_DEFAULT_ISP_DEBUG NVMAPP_FALSE
#define NVMAPP_DEFAULT_ISP_LOGLVL 255
#define NVMAPP_DEFAULT_FLASH NVMAPP_FALSE
#define NVMAPP_DEFAULT_FRAME_RATE 5
#define NVMAPP_DEFAULT_MIRROR "NONE"
#define NVMAPP_DEFAULT_RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK NVMAPP_FALSE
#define NVMAPP_DEFAULT_FILE_PATH "/data"
#define NVMAPP_DEFAULT_FOCUS_VCM_TOTALSTEPS 0
#define NVMAPP_DEFAULT_FOCUS_VCM_STARTSTEPS 0
#define NVMAPP_DEFAULT_FOCUS_VCM_STEPSIZE 0
#define NVMAPP_DEFAULT_SAVE_SENSOR_NVM_DATA NVMAPP_FALSE
#define NVMAPP_DEFAULT_SENSOR_NVM_DATA_PATH "/data/nvm_sensorData.dat"

/* Default input to application, can be overwridden by (in given order):
   - Config File
   - CMDLine input
*/
NVMAPPInputParams InputParams = {
    NVMAPP_DEFAULT_RAW_CNT,
    NVMAPP_DEFAULT_BAYER_ORDER,
    NVMAPP_DEFAULT_EXPOSURE,
    NVMAPP_DEFAULT_SENSITIVITY,
    NVMAPP_DEFAULT_TOLERANCE,
    NVMAPP_DEFAULT_LSC_MIN_AVG_MINMAXRATIO,
    NVMAPP_DEFAULT_LSC_MIN_NORMCROSSCORR,
    NVMAPP_DEFAULT_LSC_MIN_SIGMA_MINMAXRATIO,
    NVMAPP_DEFAULT_SENSOR_RESOLUTION,
    NVMAPP_DEFAULT_APP_MODE,
    NVMAPP_DEFAULT_RAW_IMAGE,
    NVMAPP_DEFAULT_JPEG,
    NVMAPP_DEFAULT_COLOR_FMT,
    NVMAPP_DEFAULT_ISP_DEBUG,
    NVMAPP_DEFAULT_ISP_LOGLVL,
    NVMAPP_DEFAULT_FLASH,
    NVMAPP_DEFAULT_FRAME_RATE,
    NVMAPP_DEFAULT_MIRROR,
    NVMAPP_DEFAULT_RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK,
    NVMAPP_DEFAULT_FILE_PATH,
    NVMAPP_DEFAULT_FOCUS_VCM_TOTALSTEPS,
    NVMAPP_DEFAULT_FOCUS_VCM_STARTSTEPS,
    NVMAPP_DEFAULT_FOCUS_VCM_STEPSIZE,
    NVMAPP_DEFAULT_SAVE_SENSOR_NVM_DATA,
    NVMAPP_DEFAULT_SENSOR_NVM_DATA_PATH
};

/* Max line length restriction for config file parser */
#define NVMAPP_MAX_LINE_LEN 500

/* Max size limitation for file names */
#define NVMAPP_MAX_FILE_NAME_LEN 128

/* Hardcoded PEDESTAL and CLIPPING values */
#define NVMAPP_DEFAULT_PEDESTAL_VALUE 256
#define NVMAPP_DEFAULT_CLIPPING_VALUE 4095

/* Hardcoded JPEG Quality factor */
#define OMX_DEFAULT_JPEG_QUALITY_FACTOR 90

/* Calibration feedback to be dumped in file */ 
CalibrationFileDataStructType CalibrationFeedback;

/* App private data */
NVMAPPPrivateDataType* AppPrivData = NULL;

/* Input properties, initialized w/ default values */
unsigned int AppMode = NVMAPPMode_CALIBRATION;
NVMAPPMirroring Mirroring = NVMAPPMirroring_None;
NVMAPPRAWPresetMode CurrentMode = NVMAPPRAWPresetMode_UnProcessed;
unsigned int RAWCaptureIndex = 0;
OMX_COLOR_FORMATTYPE ColorFormat = 
    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit;
unsigned int FrameWidth = SENSOR_X_SIZE_8MP;
unsigned int FrameHeight = SENSOR_Y_SIZE_8MP;
unsigned int StrideValue = ((FrameWidth * 3) / 2);

/* OMX related data */
OMX_PARAM_PORTDEFINITIONTYPE ISPProcOMXPortDef;
OMX_SYMBIAN_CONFIG_HWINFOTYPE SensorFuseIDInfo;

/* Macros to evaluate various conditions */
#define NVMAPP_IS_RAWFILE_REQUIRED (NVMAPP_TRUE == InputParams.RAW_IMAGE)
#define NVMAPP_IS_JPEG_REQUIRED (NVMAPP_TRUE == InputParams.JPEG)
#define NVMAPP_IS_CALIBRATION_REQUIRED (NVMAPPMode_CALIBRATION == AppMode)
#define NVMAPP_IS_FIRST_RAW_CAPTURE (0 == RAWCaptureIndex)
#define NVMAPP_IS_STATUS_SUCCESS(status) (NVMAPPErrorStatus_SUCCESS == status)
#define NVMAPP_IS_OMX_STATUS_SUCCESS(status) (OMX_ErrorNone == status)

/* Image descriptor required by STECC and related data */
stecc_bayer_image_descriptor NVMAPPImageDescriptor;
unsigned int RAWImageSize = 0;
unsigned int TMPDataBufSize = 0;
unsigned int CALDataBufSize = 0;
unsigned int TotalDataBufSize = 0;
void* pDataBuffer = NULL;
void* pTMPDataBuf = NULL;
void* pCALDataBufCurrent = NULL;
void* pCALDataBufRef = NULL;
void** pRAWImageArray = NULL;
unsigned char* pRAWImageBuf = NULL;

/* HSMCAM semaphores */
/* Used to synchronize app execution wrt FillBufferDone Evt */
sem_t SemHSMCAMFBDEvt;
/* Used to synchronize app execution wrt PortConfig Evt */
sem_t SemHSMCAMPortEvt;
/* Used to synchronize app execution wrt StateChange Evt */
sem_t SemHSMCAMCMDDoneEvt;

/* ISPPROC semaphores */
/* Used to synchronize app execution wrt FillBufferDone Evt */
sem_t SemISPProcFBDEvt;
/* Used to synchronize app execution wrt EmptyBufferDone Evt */
sem_t SemISPProcEBDEvt;
/* Used to synchronize app execution wrt PortConfig Evt */
sem_t SemISPProcPortEvt;
/* Used to synchronize app execution wrt StateChange Evt */
sem_t SemISPPROCCMDDoneEvt;

/* JPEGENC semaphores */
/* Used to synchronize app execution wrt EmptyBufferDone Evt */
sem_t SemJPEGEBDEvt;
/* Used to synchronize app execution wrt FillBufferDone Evt */
sem_t SemJPEGFBDEvt;
/* Used to synchronize app execution wrt StateChange Evt */
sem_t SemJPEGENCCMDDoneEvt;

/* Golden data file ptr */
FILE* fpGoldenData = NULL;

/* BufHdrs required for various components for datapath */
#if PREVIEW_CODE
OMX_BUFFERHEADERTYPE* HSMCAMBufHdr_VPB0 = NULL;
#endif 
OMX_BUFFERHEADERTYPE* HSMCAMBufHdr_VPB1 = NULL;
OMX_BUFFERHEADERTYPE* ISPPROCBufHdr_VPB0 = NULL;
OMX_BUFFERHEADERTYPE* ISPPROCBufHdr_VPB2 = NULL;
OMX_BUFFERHEADERTYPE* JPEGENCBufHdr_VPB0 = NULL;
OMX_BUFFERHEADERTYPE* JPEGENCBufHdr_VPB1 = NULL;


/** NVMAPPWriteRAWBufferToFile: If RAW images are required to be generated
    then dumps RAW image buffer to file system. Executed in both UnProcessed
    and Processed modes.

    @param [in] pBuffer
        Buffer pointer pointing to RAW image data buffer.
    @param [in] CurrentMode
        Current mode of execution.
    @return None
 */
void NVMAPPWriteRAWBufferToFile(OMX_BUFFERHEADERTYPE* pBuffer,
                                NVMAPPRAWPresetMode CurrentMode)
{
    char RAWFileName[NVMAPP_MAX_FILE_NAME_LEN];
    unsigned int offset = 0;

    if(NVMAPP_IS_RAWFILE_REQUIRED)
    {
        /* reconstruct the RAW file name */
        strncpy(RAWFileName, InputParams.FILE_PATH, sizeof(RAWFileName));
        offset = strlen(RAWFileName);
        
        if(NVMAPPRAWPresetMode_UnProcessed == CurrentMode)
        {
            sprintf(&RAWFileName[offset], 
                    NVMAPP_PATH_RAW_UNPROCESSED, 
                    RAWCaptureIndex);
        }
        else
        {
            sprintf(&RAWFileName[offset], 
                    NVMAPP_PATH_RAW_PROCESSED, 
                    RAWCaptureIndex);
        }
            
        FILE* fpRAWFile = fopen(RAWFileName, "wb");
        if(NULL == fpRAWFile)
        {
            ReleasePrint(stdout, 
                         "NVM_APP: Failed to open %s file.\n", 
                         RAWFileName);
        }
            
        fwrite((pBuffer->pBuffer + pBuffer->nOffset), 
               1, 
               pBuffer->nFilledLen,
               fpRAWFile);
            
        DebugPrint(stdout, 
                   "NVM_APP: %s written.\n", RAWFileName);
            
        fclose(fpRAWFile);
    }
}

/** NVMAPPWriteJPEGBufferToFile: If JPEG images are required to be generated
    then dumps JPEG image buffer to file system. Executed for both UnProcessed
    and Processed modes.

    @param [in] pBuffer
        Buffer pointer pointing to JPEG image data buffer.
    @param [in] CurrentMode
        Current mode of execution.
    @return None
 */
void NVMAPPWriteJPEGBufferToFile(OMX_BUFFERHEADERTYPE* pBuffer,
                                 NVMAPPRAWPresetMode CurrentMode)
{           
    char JPEGFileName[NVMAPP_MAX_FILE_NAME_LEN];
    unsigned int offset = 0;

    if(NVMAPP_IS_JPEG_REQUIRED)
    {
        /* reconstruct the JPEG file name */
        strncpy(JPEGFileName, InputParams.FILE_PATH, sizeof(JPEGFileName));
        offset = strlen(JPEGFileName);
        
        if(NVMAPPRAWPresetMode_UnProcessed == CurrentMode)
        {
            sprintf(&JPEGFileName[offset], 
                    NVMAPP_PATH_JPEG_UNPROCESSED, 
                    RAWCaptureIndex);
        }
        else
        {
            sprintf(&JPEGFileName[offset], 
                    NVMAPP_PATH_JPEG_PROCESSED, 
                    RAWCaptureIndex);
        }
            
        FILE* fpJPEGFile = fopen(JPEGFileName, "wb");
        if(NULL == fpJPEGFile)
        {
            ReleasePrint(stdout, 
                         "NVM_APP: Failed to open %s file.\n", 
                         JPEGFileName);
        }
            
        fwrite((pBuffer->pBuffer + pBuffer->nOffset), 
               1, 
               pBuffer->nFilledLen,
               fpJPEGFile);
            
        DebugPrint(stdout, "NVM_APP: %s written.\n", JPEGFileName);
            
        fclose(fpJPEGFile);
    }
}

/** NVMAPPHSMCAMEventHandler: HSMCAMERA Event hander which implements 
    the event handling mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] eEvent
        Event to notify.
    @param [in] nData1
        OMX_ERRORTYPE for an error event
        OMX_COMMANDTYPE for a command complete event
        OMX_INDEXTYPE for a OMX_PortSettingsChanged event.
    @param [in] nData2
        nData2 holda further info related to the event.
    @param [in] pEventData
        Pointer to additional event-specific data.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPHSMCAMEventHandler(OMX_HANDLETYPE hComponent,
                                       OMX_PTR pAppData,
                                       OMX_EVENTTYPE eEvent,
                                       OMX_U32 nData1,
                                       OMX_U32 nData2,
                                       OMX_PTR pEventData)
{
    switch(eEvent)
    {
        case OMX_EventCmdComplete:
        {
            OMX_COMMANDTYPE CMD = (OMX_COMMANDTYPE)nData1;

            if(OMX_CommandStateSet == CMD)
            {
                OMX_STATETYPE NewState = (OMX_STATETYPE)nData2;

                switch(NewState)
                {
                    case OMX_StateLoaded:
                        sem_post(&SemHSMCAMCMDDoneEvt);
                        break;
                    case OMX_StateIdle:
                        sem_post(&SemHSMCAMCMDDoneEvt);
                        break;
                    case OMX_StateExecuting:
                        sem_post(&SemHSMCAMCMDDoneEvt);
                        break;
                    case OMX_StatePause:
                        break;
                    case OMX_StateInvalid:
                        break;
                    default:
                        break;
                }
            }
            else if(OMX_CommandPortEnable == CMD)
            {
                sem_post(&SemHSMCAMPortEvt);
            }
            else if(OMX_CommandPortDisable == CMD)
            {
                sem_post(&SemHSMCAMPortEvt);
            }
            break;
        }
        default:
            break;
    }
    
    return OMX_ErrorNone;
}

/** NVMAPPISPPROCEventHandler: ISPPROC Event hander which implements 
    the event handling mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] eEvent
        Event to notify.
    @param [in] nData1
        OMX_ERRORTYPE for an error event
        OMX_COMMANDTYPE for a command complete event
        OMX_INDEXTYPE for a OMX_PortSettingsChanged event.
    @param [in] nData2
        nData2 holda further info related to the event.
    @param [in] pEventData
        Pointer to additional event-specific data.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPISPPROCEventHandler(OMX_HANDLETYPE hComponent,
                                        OMX_PTR pAppData,
                                        OMX_EVENTTYPE eEvent,
                                        OMX_U32 nData1,
                                        OMX_U32 nData2,
                                        OMX_PTR pEventData)
{   
    switch(eEvent)
    {
        case OMX_EventCmdComplete:
        {
            OMX_COMMANDTYPE CMD = (OMX_COMMANDTYPE)nData1;
            if(OMX_CommandStateSet == CMD)
            {
                OMX_STATETYPE NewState = (OMX_STATETYPE)nData2;
                switch(NewState)
                {
                    case OMX_StateLoaded:
                        sem_post(&SemISPPROCCMDDoneEvt);
                        break;
                    case OMX_StateIdle:
                        sem_post(&SemISPPROCCMDDoneEvt);
                        break;
                    case OMX_StateExecuting:
                        sem_post(&SemISPPROCCMDDoneEvt);
                        break;
                    case OMX_StatePause:
                        break;
                    case OMX_StateInvalid:
                        break;
                    default:
                        break;
                }
            }
            else if(OMX_CommandPortEnable == CMD)
            {
                sem_post(&SemISPProcPortEvt);
            }
            else if(OMX_CommandPortDisable == CMD)
            {
                sem_post(&SemISPProcPortEvt);
            }
            break;
        }
        default:
            break;
    }
    
    return OMX_ErrorNone;
}

/** NVMAPPJPEGENCEventHandler: JPEGENC Event hander which implements 
    the event handling mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] eEvent
        Event to notify.
    @param [in] nData1
        OMX_ERRORTYPE for an error event
        OMX_COMMANDTYPE for a command complete event
        OMX_INDEXTYPE for a OMX_PortSettingsChanged event.
    @param [in] nData2
        nData2 holda further info related to the event.
    @param [in] pEventData
        Pointer to additional event-specific data.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPJPEGENCEventHandler(OMX_HANDLETYPE hComponent,
                                        OMX_PTR pAppData,
                                        OMX_EVENTTYPE eEvent,
                                        OMX_U32 nData1,
                                        OMX_U32 nData2,
                                        OMX_PTR pEventData)
{   
    switch(eEvent)
    {
        case OMX_EventCmdComplete:
        {
            OMX_COMMANDTYPE CMD = (OMX_COMMANDTYPE)nData1;
            if(OMX_CommandStateSet == CMD)
            {
                OMX_STATETYPE NewState = (OMX_STATETYPE)nData2;
                switch(NewState)
                {
                    case OMX_StateLoaded:
                        sem_post(&SemJPEGENCCMDDoneEvt);
                        break;
                    case OMX_StateIdle:
                        sem_post(&SemJPEGENCCMDDoneEvt);
                        break;
                    case OMX_StateExecuting:
                        sem_post(&SemJPEGENCCMDDoneEvt);
                        break;
                    case OMX_StatePause:
                        break;
                    case OMX_StateInvalid:
                        break;
                    default:
                        break;
                }
            }
            else if(OMX_CommandPortEnable == CMD)
            {

            }
            else if(OMX_CommandPortDisable == CMD)
            {

            }
            break;
        }
        default:
            break;
    }
    
    return OMX_ErrorNone;
}

/** NVMAPPHSMCAMFillBufferDone: HSMCAMERA FillBufferDone Event hander 
    which implements the RAW buffer processing mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] pBuffer
        RAW data buffer pointer.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPHSMCAMFillBufferDone(OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    DebugPrint(stdout, "NVM_APP: HSMCAM FillBufferDone received.\n");
    
    if(OMX_CAMPORT_INDEX_VPB1 == pBuffer->nOutputPortIndex)
    {
        /* RAW frame */
        if(0 != (pBuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) )
        {
            pBuffer->nFlags = 0;
        }
        if(NVMAPP_IS_JPEG_REQUIRED)
        {
            /* Store the HSMCAMERA bufHdr info to ISPPROC bufHdr info as the 
               same buffer need to be passed to ISPPROC for YUV->JPEG encoding */
            ISPPROCBufHdr_VPB0->nOffset = pBuffer->nOffset;
            ISPPROCBufHdr_VPB0->nFilledLen = pBuffer->nFilledLen;
        }

        if(NVMAPPRAWPresetMode_UnProcessed == CurrentMode)
        {
            /* Copy the RAW frame to RAWImageArray at current capture index 
               for calibration computation */ 
            memcpy((char*)pRAWImageArray[RAWCaptureIndex], 
                   (pBuffer->pBuffer + pBuffer->nOffset),
                   ((pBuffer->nFilledLen <= RAWImageSize) ? 
                    pBuffer->nFilledLen : RAWImageSize));
        }
        
        NVMAPPWriteRAWBufferToFile(pBuffer, CurrentMode);
        
        sem_post(&SemHSMCAMFBDEvt);
        
    }
    
#if PREVIEW_CODE
    if(OMX_CAMPORT_INDEX_VPB0 == pBuffer->nOutputPortIndex)
    {
        /* Preview frame */
        HSMCAMBufHdr_VPB0->nOffset = 0;
        HSMCAMBufHdr_VPB0->nFilledLen = 0;
        
        (void)OMX_FillThisBuffer(AppPrivData->hHSMCAM, HSMCAMBufHdr_VPB0);
    }
#endif

    return OMX_ErrorNone;
}

/** NVMAPPISPPROCFillBufferDone: ISPPROC FillBufferDone Event hander 
    which implements the intermediate format buffer processing mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] pBuffer
        data buffer pointer.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPISPPROCFillBufferDone(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_BUFFERHEADERTYPE* pBuffer)
{
    DebugPrint(stdout, "NVM_APP: ISPPROC FillBufferDone received.\n");
    
    if(OMX_ISPPORT_INDEX_VPB2 == pBuffer->nOutputPortIndex)	
    {
        /* Store the ISPPROC bufHdr info to JPEGENC bufHdr info as the 
           same buffer need to be passed to JPEGENC for JPEG encoding */
        JPEGENCBufHdr_VPB0->nOffset 	= pBuffer->nOffset;
        JPEGENCBufHdr_VPB0->nFilledLen = pBuffer->nFilledLen;
        sem_post(&SemISPProcFBDEvt);
    }
    
    return OMX_ErrorNone;
}

/** NVMAPPISPPROCEmptyBufferDone: ISPPROC EmptyBufferDone Event hander 
    which is notified upon buffer consumption.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] pBuffer
        data buffer pointer.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPISPPROCEmptyBufferDone(OMX_HANDLETYPE hComponent,
                                           OMX_PTR pAppData,
                                           OMX_BUFFERHEADERTYPE* pBuffer)
{
    DebugPrint(stdout, "NVM_APP: ISPPROC EmptyBufferDone received.\n");
    
    sem_post(&SemISPProcEBDEvt);  

    return OMX_ErrorNone;
}

/** NVMAPPJPEGENCFillBufferDone: JPEGENC FillBufferDone Event hander 
    which implements the JPEG buffer processing mechanism.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] pBuffer
        jpeg data buffer pointer.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPJPEGENCFillBufferDone(OMX_HANDLETYPE hComponent,
                                          OMX_PTR pAppData,
                                          OMX_BUFFERHEADERTYPE* pBuffer)
{
    DebugPrint(stdout, "NVM_APP: JPEGENC FillBufferDone received.\n");
    
    if(OMX_JPGPORT_INDEX_VPB1 == pBuffer->nOutputPortIndex)
    {
        /* JPEG ENCODED frame received */
        NVMAPPWriteJPEGBufferToFile(pBuffer, CurrentMode);
        sem_post(&SemJPEGFBDEvt);
    }

    return OMX_ErrorNone;
}

/** NVMAPPJPEGENCEmptyBufferDone: JPEGENC EmptyBufferDone Event hander 
    which is notified upon buffer consumption.

    @param [in] hComponent
        handle of the component.
    @param [in] pAppData
        App context.
    @param [in] pBuffer
        data buffer pointer.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPJPEGENCEmptyBufferDone(OMX_HANDLETYPE hComponent,
                                           OMX_PTR pAppData,
                                           OMX_BUFFERHEADERTYPE* pBuffer)
{
    DebugPrint(stdout, "NVM_APP: JPEGENC EmptyBufferDone received.\n");
    
    sem_post(&SemJPEGEBDEvt);

    return OMX_ErrorNone;
}

/** NVMAPPPrintTagAndValue: Prints the final <TAG=VALUE> input which is 
    interpreted by application based on default/Config/CMDLine inputs.
    
    @return None
 */
void NVMAPPPrintTagAndValue(void)
{
    DebugPrint(stdout, "\n\n");
    
    DebugPrint(stdout, "NVM_APP: Final input to application:\n"); 
    DebugPrint(stdout, "NVM_APP: Based on default_values => configfile " 
                       "=> cmdline_input.\n");
    
    DebugPrint(stdout, "NVM_APP: <RAW_CNT=%d>\n", InputParams.RAW_CNT);
    DebugPrint(stdout, "NVM_APP: <BAYER_ORDER=%s>\n", InputParams.BAYER_ORDER);
    DebugPrint(stdout, "NVM_APP: <EXPOSURE=%d>\n", InputParams.EXPOSURE);
    DebugPrint(stdout, "NVM_APP: <SENSITIVITY=%d>\n", InputParams.SENSITIVITY);
    DebugPrint(stdout, "NVM_APP: <TOLERANCE=%f>\n", InputParams.TOLERANCE);
    DebugPrint(stdout, 
               "NVM_APP: <LSC_MIN_AVG_MINMAXRATIO=%f>\n", 
               InputParams.LSC_MIN_AVG_MINMAXRATIO);
    DebugPrint(stdout, 
               "NVM_APP: <LSC_MIN_NORMCROSSCORR=%f>\n", 
               InputParams.LSC_MIN_NORMCROSSCORR);
    DebugPrint(stdout, 
               "NVM_APP: <LSC_MIN_SIGMA_MINMAXRATIO=%f>\n", 
               InputParams.LSC_MIN_SIGMA_MINMAXRATIO);
    DebugPrint(stdout, 
               "NVM_APP: <SENSOR_RESOLUTION=%s>\n", 
               InputParams.SENSOR_RESOLUTION);
    DebugPrint(stdout, "NVM_APP: <APP_MODE=%s>\n", InputParams.APP_MODE);
    DebugPrint(stdout, 
               "NVM_APP: <RAW_IMAGE=%s>\n", 
               ((NVMAPP_TRUE == InputParams.RAW_IMAGE) ? "TRUE" : "FALSE"));
    DebugPrint(stdout, 
               "NVM_APP: <JPEG=%s>\n",
               ((NVMAPP_TRUE == InputParams.JPEG) ? "TRUE" : "FALSE"));
    DebugPrint(stdout, "NVM_APP: <COLOR_FMT=%s>\n", InputParams.COLOR_FMT);
    DebugPrint(stdout, 
               "NVM_APP: <ISP_DEBUG=%s>\n",
               ((NVMAPP_TRUE == InputParams.ISP_DEBUG) ? "TRUE" : "FALSE"));
    DebugPrint(stdout, "NVM_APP: <ISP_LOGLVL=%d>\n", InputParams.ISP_LOGLVL);
    DebugPrint(stdout, 
               "NVM_APP: <FLASH=%s>\n",
               ((NVMAPP_TRUE == InputParams.FLASH) ? "TRUE" : "FALSE"));
    DebugPrint(stdout, "NVM_APP: <FRAME_RATE=%d>\n", InputParams.FRAME_RATE);
    DebugPrint(stdout, "NVM_APP: <MIRROR=%s>\n", InputParams.MIRROR);
    DebugPrint(stdout, 
               "NVM_APP: <RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK=%s>\n",
               ((NVMAPP_TRUE == InputParams.RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK) ? 
               "TRUE" : "FALSE"));
    DebugPrint(stdout, "NVM_APP: <FILE_PATH=%s>\n", InputParams.FILE_PATH);

    DebugPrint(stdout, "\n\n");
}

/** NVMAPPProcessTagAndValue: Checks input TAG against valid TAGs and updates
    the corresponding TAG's value. Ignores invalid TAGs.

    @param [in] pContext
        Context in which fn executes. Can be called from Config processing or
        CMDLine processing.
    @param [in] pTag
        TAG to be processed.
    @param [in] pBuffer
        TAG value.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPProcessTagAndValue(char* pContext, char *pTag, char* pValue)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    
    if(NULL != pTag && NULL != pValue && NULL != pContext)
    {
        if(!strcmp("RAW_CNT", pTag))
        {
            InputParams.RAW_CNT = atoi(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <RAW_CNT=%d>\n", 
                       pContext, 
                       InputParams.RAW_CNT);
        }
        else if(!strcmp("BAYER_ORDER", pTag))
        {
            if(!strcmp("R", pValue) || 
               !strcmp("Gr", pValue) || 
               !strcmp("Gb", pValue) ||
               !strcmp("G", pValue))
            {
                strncpy((char*)InputParams.BAYER_ORDER, 
                        pValue, 
                        sizeof(InputParams.BAYER_ORDER));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <BAYER_ORDER=%s>\n", 
                           pContext, 
                           InputParams.BAYER_ORDER);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong BAYER_ORDER value = %s." 
                             "Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("EXPOSURE", pTag))
        {
            InputParams.EXPOSURE = atoi(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <EXPOSURE=%d>\n", 
                       pContext, 
                       InputParams.EXPOSURE);
        }
        else if(!strcmp("SENSITIVITY", pTag))
        {
            InputParams.SENSITIVITY = atoi(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <SENSITIVITY=%d>\n", 
                       pContext,
                       InputParams.SENSITIVITY);
        }
        else if(!strcmp("TOLERANCE", pTag))
        {
            InputParams.TOLERANCE = (float)atof(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <TOLERANCE=%f>\n", 
                       pContext,
                       InputParams.TOLERANCE);
        }
        else if(!strcmp("LSC_MIN_AVG_MINMAXRATIO", pTag))
        {
            InputParams.LSC_MIN_AVG_MINMAXRATIO = (float)atof(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <LSC_MIN_AVG_MINMAXRATIO=%f>\n", 
                       pContext,
                       InputParams.LSC_MIN_AVG_MINMAXRATIO);
        }
        else if(!strcmp("LSC_MIN_NORMCROSSCORR", pTag))
        {
            InputParams.LSC_MIN_NORMCROSSCORR = (float)atof(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <LSC_MIN_NORMCROSSCORR=%f>\n", 
                       pContext,
                       InputParams.LSC_MIN_NORMCROSSCORR);
        }
        else if(!strcmp("LSC_MIN_SIGMA_MINMAXRATIO", pTag))
        {
            InputParams.LSC_MIN_SIGMA_MINMAXRATIO = (float)atof(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <LSC_MIN_SIGMA_MINMAXRATIO=%f>\n", 
                       pContext,
                       InputParams.LSC_MIN_SIGMA_MINMAXRATIO);
        }
        else if(!strcmp("SENSOR_RESOLUTION", pTag))
        {
            if(!strcmp("5MP", pValue) ||
               !strcmp("5mp", pValue))
            {
                FrameWidth = SENSOR_X_SIZE_5MP;
                FrameHeight = SENSOR_Y_SIZE_5MP;
                strncpy((char*)InputParams.SENSOR_RESOLUTION, 
                        "5MP", 
                        sizeof(InputParams.SENSOR_RESOLUTION));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <SENSOR_RESOLUTION=%s>\n", 
                           pContext, 
                           InputParams.SENSOR_RESOLUTION);
            }
            else if(!strcmp("8MP", pValue) ||
                    !strcmp("8mp", pValue))
            {
                FrameWidth = SENSOR_X_SIZE_8MP;
                FrameHeight = SENSOR_Y_SIZE_8MP;
                strncpy((char*)InputParams.SENSOR_RESOLUTION, 
                        "8MP", 
                        sizeof(InputParams.SENSOR_RESOLUTION));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <SENSOR_RESOLUTION=%s>\n", 
                           pContext,
                           InputParams.SENSOR_RESOLUTION);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong SENSOR_RESOLUTION value = %s." 
                             "Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("APP_MODE", pTag))
        {   
            if(!strcmp("CALIBRATION", pValue))
            {
                AppMode = NVMAPPMode_CALIBRATION;
                strncpy((char*)InputParams.APP_MODE, 
                        pValue, 
                        sizeof(InputParams.APP_MODE));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <APP_MODE=%s>\n", 
                           pContext,
                           InputParams.APP_MODE);
            }
            else if(!strcmp("RAW", pValue))
            {
                AppMode = NVMAPPMode_RAW;
                strncpy((char*)InputParams.APP_MODE, 
                        pValue, 
                        sizeof(InputParams.APP_MODE));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <APP_MODE=%s>\n", 
                           pContext,
                           InputParams.APP_MODE);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong APP_MODE value = %s. Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("RAW_IMAGE", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.RAW_IMAGE = NVMAPP_TRUE;
                DebugPrint(stdout, "NVM_APP: %s: <RAW_IMAGE=TRUE>\n", pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.RAW_IMAGE = NVMAPP_FALSE;
                DebugPrint(stdout, "NVM_APP: %s: <RAW_IMAGE=FALSE>\n", pContext);
            }
            else
            {
               ReleasePrint(stdout, 
                            "NVM_APP: %s: Wrong RAW_IMAGE value = %s. Ignored.\n", 
                            pContext,
                            pValue);
            }
        }
        else if(!strcmp("JPEG", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.JPEG = NVMAPP_TRUE;
                DebugPrint(stdout, "NVM_APP: %s: <JPEG=TRUE>\n", pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.JPEG = NVMAPP_FALSE;
                DebugPrint(stdout, "NVM_APP: %s: <JPEG=FALSE>\n", pContext);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong JPEG value = %s. Ignored.\n", 
                             pContext, 
                             pValue);
            }
        }
        else if(!strcmp("COLOR_FMT", pTag))
        {   
            if(!strcmp("12BIT", pValue) ||
               !strcmp("12bit", pValue))
            {
                ColorFormat = 
                    (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatRawBayer12bit;
                StrideValue = (FrameWidth * 3) / 2;
                strncpy((char*)InputParams.COLOR_FMT, 
                        "12BIT", 
                        sizeof(InputParams.COLOR_FMT));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <COLOR_FMT=%s>\n", 
                           pContext,
                           InputParams.COLOR_FMT);
                
            }
            else if(!strcmp("8BIT", pValue) ||
                    !strcmp("8bit", pValue))
            {
                ColorFormat = OMX_COLOR_FormatRawBayer8bit;
                StrideValue = FrameWidth;
                strncpy((char*)InputParams.COLOR_FMT, 
                        "8BIT", 
                        sizeof(InputParams.COLOR_FMT));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <COLOR_FMT=%s>\n", 
                           pContext,
                           InputParams.COLOR_FMT);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong COLOR_FMT value = %s. Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("ISP_DEBUG", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.ISP_DEBUG = NVMAPP_TRUE;
                DebugPrint(stdout, "NVM_APP: %s: <ISP_DEBUG=TRUE>\n", pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.ISP_DEBUG = NVMAPP_FALSE;
                DebugPrint(stdout, "NVM_APP: %s: <ISP_DEBUG=FALSE>\n", pContext);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong ISP_DEBUG value = %s. Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("ISP_LOGLVL", pTag))
        {
            InputParams.ISP_LOGLVL = atoi(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <ISP_LOGLVL=%d>\n", 
                       pContext,
                       InputParams.ISP_LOGLVL);
        }
        else if(!strcmp("FLASH", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.FLASH = NVMAPP_TRUE;
                DebugPrint(stdout, "NVM_APP: %s: <FLASH=TRUE>\n", pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.FLASH = NVMAPP_FALSE;
                DebugPrint(stdout, "NVM_APP: %s: <FLASH=FALSE>\n", pContext);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong FLASH value = %s. Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("FRAME_RATE", pTag))
        {
            InputParams.FRAME_RATE = atoi(pValue);
            DebugPrint(stdout, 
                       "NVM_APP: %s: <FRAME_RATE=%d>\n", 
                       pContext,
                       InputParams.FRAME_RATE);
        }
        else if(!strcmp("MIRROR", pTag))
        {
            if(!strcmp("NONE", pValue))
            {
                Mirroring = NVMAPPMirroring_None;
                strncpy((char*)InputParams.MIRROR, 
                        pValue, 
                        sizeof(InputParams.MIRROR));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <MIRROR=%s>\n", 
                           pContext, 
                           InputParams.MIRROR);
            }
            else if (!strcmp("VERTICAL", pValue))
            {
                Mirroring = NVMAPPMirroring_Vertical;
                strncpy((char*)InputParams.MIRROR, 
                        pValue, 
                        sizeof(InputParams.MIRROR));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <MIRROR=%s>\n", 
                           pContext, 
                           InputParams.MIRROR);
            }
            else if (!strcmp("HORIZONTAL", pValue))
            {
                Mirroring = NVMAPPMirroring_Horizontal;
                strncpy((char*)InputParams.MIRROR, 
                        pValue, 
                        sizeof(InputParams.MIRROR));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <MIRROR=%s>\n", 
                           pContext, 
                           InputParams.MIRROR);
            }
            else if (!strcmp("BOTH", pValue))
            {
                Mirroring = NVMAPPMirroring_Both;
                strncpy((char*)InputParams.MIRROR, 
                        pValue, 
                        sizeof(InputParams.MIRROR));
                DebugPrint(stdout, 
                           "NVM_APP: %s: <MIRROR=%s>\n", 
                           pContext, 
                           InputParams.MIRROR);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong MIRROR value = %s. Ignored. \n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK = NVMAPP_TRUE;
                DebugPrint(stdout, 
                           "NVM_APP: %s: <RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK=TRUE>\n", 
                           pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK = NVMAPP_FALSE;
                DebugPrint(stdout, 
                           "NVM_APP: %s: <RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK=FALSE>\n", 
                           pContext);
            }
            else
            {
                ReleasePrint(stdout, 
                             "NVM_APP: %s: Wrong RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK "
                             "value = %s. Ignored.\n", 
                             pContext,
                             pValue);
            }
        }
        else if(!strcmp("FILE_PATH", pTag))
        {
            strncpy((char*)InputParams.FILE_PATH, 
                    pValue, 
                    sizeof(InputParams.FILE_PATH));
            DebugPrint(stdout, 
                       "NVM_APP: %s: <FILE_PATH=%s>\n", 
                       pContext, 
                       InputParams.FILE_PATH);
        }
        else if(!strcmp("FOCUS_VCM_TOTAL_STEPS", pTag))
        {
            InputParams.Focus_VCM_TotalSteps= atoi(pValue);
            DebugPrint(stdout,
                       "NVM_APP: %s: <Focus_VCM_TotalSteps=%d>\n",
                       pContext,
                       InputParams.Focus_VCM_TotalSteps);
        }
        else if(!strcmp("FOCUS_VCM_START_STEP", pTag))
        {
            InputParams.Focus_VCM_StartStep= atoi(pValue);
            DebugPrint(stdout,
                       "NVM_APP: %s: <Focus_VCM_StartStep=%d>\n",
                       pContext,
                       InputParams.Focus_VCM_StartStep);
        }
        else if(!strcmp("FOCUS_VCM_STEP_SIZE", pTag))
        {
            InputParams.Focus_VCM_StepSize= atoi(pValue);
            DebugPrint(stdout,
                       "NVM_APP: %s: <Focus_VCM_StepSize=%d>\n",
                       pContext,
                       InputParams.Focus_VCM_StepSize);
        }
        else if(!strcmp("SAVE_SENSOR_NVM_DATA", pTag))
        {
            if(!strcmp("TRUE", pValue))
            {
                InputParams.bSaveSensorNVMData = NVMAPP_TRUE;
                DebugPrint(stdout, "NVM_APP: %s: <bSaveSensorNVMData=TRUE>\n", pContext);
            }
            else if (!strcmp("FALSE", pValue))
            {
                InputParams.bSaveSensorNVMData = NVMAPP_FALSE;
                DebugPrint(stdout, "NVM_APP: %s: <bSaveSensorNVMData=FALSE>\n", pContext);
            }
        }
        else if(!strcmp("SENSOR_NVM_DATA_PATH", pTag))
        {
            strncpy((char*)InputParams.aSensorNVMDataPath, pValue, sizeof(InputParams.aSensorNVMDataPath));
            DebugPrint(stdout, "NVM_APP: %s: <FILE_PATH=%s>\n", pContext, InputParams.aSensorNVMDataPath);
        }
        else
        {
            ReleasePrint(stdout, 
                         "NVM_APP: %s: Undefined TAG = %s. Ignored.\n", 
                         pContext, pTag);
        }
    }
    else
    {
        status = NVMAPPErrorStatus_FAILURE;
    }

    return status;
}

/** NVMAPPTrimSpacesAndTabs: Remove all the leading and trailing spaces/tabs in
    a given string to make it easy to process.

    @param [in/out] pStr
        ptr to ptr to string to be processed.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPTrimSpacesAndTabs(char** pStr)
{
    char* pTMP = NULL;
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;

    if(NULL != pStr)
    {
        pTMP = *pStr;

        /* Remove any leading spaces/tabs */
        while(' ' == *pTMP || '\t' == *pTMP)
        {
            pTMP++;
        }
        *pStr = pTMP;

        /* Remove any trailing spaces/tabs */
        while(' ' != *pTMP && '\t' != *pTMP && '\0' != *pTMP )
        {
            pTMP++;
        }
        if(' ' == *pTMP || '\t' == *pTMP)
        {
            *pTMP = '\0';
        }
    }
    else
    {
        status = NVMAPPErrorStatus_FAILURE;
    }

    return status;
}

/** NVMAPPParseLine: Parse a given line and tokenize it as per predefined 
    input rules i.e. TAG=VALUE. Breaks the input into TAG and VALUE followed
    by processing them appropriately.

    @param [in] pContext
        Context in which fn executes. Can be called from Config processing or
        CMDLine processing.
    @param [in] pLine
        ptr to string to be parsed.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPParseLine(char* pContext, char *pLine)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    char* pTag = NULL;
    char* pValue = NULL;
    char* pInputLine = pLine;

    if(pInputLine != NULL)
    {
        /* Extract TAG */
        pTag = strtok(pInputLine, "=");
        if(NULL != pTag)
        {
            /* Trim leading/trailing spaces/tabs from the TAG */ 
            status = NVMAPPTrimSpacesAndTabs(&pTag);
            if(NVMAPP_IS_STATUS_SUCCESS(status))
            {
                /* Extract TAG's Value */
                pValue = strtok(NULL, "\r\n\0");
                if(NULL != pValue)
                {
                    /* Trim leading/trailing spaces/tabs from the TAG's Value */ 
                    status = NVMAPPTrimSpacesAndTabs(&pValue);
                    if(NVMAPP_IS_STATUS_SUCCESS(status))
                    {
                        /* Process TAG, VALUE pair */ 
                        status = NVMAPPProcessTagAndValue(pContext, pTag, pValue);
                    }
                }
                else
                {
                    ReleasePrint(stdout, "NVM_APP: Unxpected TAG Value for"
                                 "TAG = %s. Ignored.\n", pTag);
                }
            }
        }
        else
        {
            ReleasePrint(stdout, 
                         "NVM_APP: Unxpected input %s. Ignored.\n", 
                         pInputLine);
        }

    }
    else
    {
        status = NVMAPPErrorStatus_FAILURE;
    }

    return status;
}

/** NVMAPPLoadConfigFile: Looks for config.txt file in current working directory
    and if config file is present then processes the config data line by line
    and generates appropriate input which will override the default input.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPLoadConfigFile(void)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    char* pLine = NULL;
    char* pTMPLine = NULL;
    char Context[4] = "CFG";
    FILE* fpCfg = NULL;

    pLine = (char*)malloc(NVMAPP_MAX_LINE_LEN * sizeof(char));
    if(NULL != pLine)
    {
        fpCfg = fopen("config.txt", "r");
        if(NULL != fpCfg)
        {
            /* Seek to file start */
            fseek(fpCfg, 0, SEEK_SET);
            DebugPrint(stdout, "NVM_APP: Successfully opened the Cfg file.\n");
            while(fgets(pLine, NVMAPP_MAX_LINE_LEN, fpCfg))
            {   
                pTMPLine = pLine;
                /* Remove any leading spaces/tabs from current Line */
                while(' ' == *pTMPLine || '\t' == *pTMPLine)
                {
                    pTMPLine++;
                }
                
                /* Check for comments(starting w/ #)/blank lines and ignore 
                   such lines else proceed w/ parsing the line */
                if('#' != pTMPLine[0] && 
                   '\n' != pTMPLine[0] && 
                   '\r' != pTMPLine[0])
                {
                    status = NVMAPPParseLine(Context, pTMPLine);
                    if(!NVMAPP_IS_STATUS_SUCCESS(status))
                    {
                        break;
                    }
                }
            }

            fclose(fpCfg);
            DebugPrint(stdout, "NVM_APP: Successfully loaded config file.\n");
        }
        else
        {
            ReleasePrint(stdout, "NVM_APP: Failed to open config file, "
                                 "looking for cmdline input.\n");
        }
        free(pLine);
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Memory allocation failed.\n");
        ReleasePrint(stdout, "NVM_APP: Failed to load config file.\n");
        status = NVMAPPErrorStatus_FAILURE;
    }

    return status;
}

/** NVMAPPParseCmdLine: Processes the CMDLine input data argument by argument
    and generates appropriate input which will override the default/Config input.

    @param [in] argc
        Number of arguments passed to application
    @param [in] argv
        Array of arguments passed to application
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPParseCmdLine(int argc, char** argv)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    char Context[8] = "CMDLINE";
    int index = 0;

    DebugPrint(stdout, "NVM_APP: Parsing cmdline input.\n");

    for(index = 1; index < argc; index++)
    {
        /* Parse CMDine input once by one */
        status = NVMAPPParseLine(Context, argv[index]);
        if(!NVMAPP_IS_STATUS_SUCCESS(status))
        {
            break;
        }
    }
    
    if(NVMAPP_IS_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Successfully interpreted cmdline input.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to interpret cmdline input.\n");
    }

    return status;
}

/** NVMAPPInterpretInput: Interprets all forms of input i.e. Config file as well
    as CMDLine input. App default input will be overriden by Config file (if 
    present) which will be finally overriden by CMDLine input(if present). 
    At end, prints the final input to the application.

    @param [in] argc
        Number of arguments passed to application
    @param [in] argv
        Array of arguments passed to application
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPInterpretInput(int argc, char** argv)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;

    /* Load config file (if present) and override default values */
    status = NVMAPPLoadConfigFile();
    if(NVMAPP_IS_STATUS_SUCCESS(status))
    {
        if(argc > 1)
        {
            /* Override degault/config values w/ CMDLine input */
            status = NVMAPPParseCmdLine(argc, argv);
        }
    }

    /* Print the final input to application */
    NVMAPPPrintTagAndValue();

    return status;
}

/** NVMAPPPopulateISPProcInPortwrtHSMCAM_VPB1: Stores port configuration used
    by HSMCAM o/p port to be used for ISPPROC i/p port configuration.

    @param [in] pOMXPortDef
        ptr to OMX port defination configuration
        
    @return None
 */
void NVMAPPPopulateISPProcInPortwrtHSMCAM_VPB1(
    OMX_PARAM_PORTDEFINITIONTYPE* pOMXPortDef)
{
    ISPProcOMXPortDef.format.video.pNativeRender = 
        pOMXPortDef->format.video.pNativeRender;
    ISPProcOMXPortDef.format.video.nFrameWidth = 
        pOMXPortDef->format.video.nFrameWidth;
    ISPProcOMXPortDef.format.video.nFrameHeight = 
        pOMXPortDef->format.video.nFrameHeight;
    ISPProcOMXPortDef.format.video.nStride = pOMXPortDef->format.video.nStride;
    ISPProcOMXPortDef.format.video.nSliceHeight = 
        pOMXPortDef->format.video.nSliceHeight;
    ISPProcOMXPortDef.format.video.bFlagErrorConcealment = 
        pOMXPortDef->format.video.bFlagErrorConcealment;
    ISPProcOMXPortDef.format.video.eCompressionFormat = 
        pOMXPortDef->format.video.eCompressionFormat;
    ISPProcOMXPortDef.format.video.eColorFormat = 
        pOMXPortDef->format.video.eColorFormat;
    ISPProcOMXPortDef.format.video.pNativeWindow = 
        pOMXPortDef->format.video.pNativeWindow;
    ISPProcOMXPortDef.nBufferCountActual = pOMXPortDef->nBufferCountActual;
    ISPProcOMXPortDef.format.video.xFramerate = InputParams.FRAME_RATE;
    ISPProcOMXPortDef.nBufferSize = pOMXPortDef->nBufferSize;

    return;
}

/** NVMAPPConfigHSMCAMPort_VPB0: Configures HSMCAMERA VPB0 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigHSMCAMPort_VPB0(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_CAMPORT_INDEX_VPB0;

    status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: HSMCAM OMX_GetParameter for VPB0 done.\n");
        
        OMXPortDefinition.format.video.pNativeWindow = 0;
        OMXPortDefinition.nBufferCountActual = 1;
        OMXPortDefinition.format.video.xFramerate = InputParams.FRAME_RATE;
        
        status = OMX_SetParameter(AppPrivData->hHSMCAM, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: HSMCAM OMX_SetParameter for VPB0 done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: HSMCAM VPB0 configuration failed.\n");
    }

    return status;
    
}

/** NVMAPPConfigHSMCAMPort_VPB1: Configures HSMCAMERA VPB1 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigHSMCAMPort_VPB1(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_CAMPORT_INDEX_VPB1;

    status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: HSMCAM OMX_GetParameter for VPB1 done.\n");

        OMXPortDefinition.nBufferCountActual = 1;
        OMXPortDefinition.format.video.xFramerate = InputParams.FRAME_RATE;
        OMXPortDefinition.format.video.eColorFormat = ColorFormat;
        
        status = OMX_SetParameter(AppPrivData->hHSMCAM, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: HSMCAM OMX_SetParameter for VPB1 done.\n");

            status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                                      OMX_IndexParamPortDefinition, 
                                      &OMXPortDefinition);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, 
                           "NVM_APP: HSMCAM VPB1 nFrameWidth = %d\n", 
                           (int)OMXPortDefinition.format.video.nFrameWidth);
                DebugPrint(stdout, 
                           "NVM_APP: HSMCAM VPB1 nFrameHeight = %d\n", 
                           (int)OMXPortDefinition.format.video.nFrameHeight);
                DebugPrint(stdout,
                           "NVM_APP: HSMCAM VPB1 nStride = %d\n", 
                           (int)OMXPortDefinition.format.video.nStride);
                NVMAPPPopulateISPProcInPortwrtHSMCAM_VPB1(&OMXPortDefinition);
            }
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: HSMCAM VPB1 configuration failed.\n");
    }

    return status;
    
}

/*OMX_ERRORTYPE NVMAPPSetHSMCAM_VPB0_SensorMode(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_SENSORMODETYPE OMXSensorMode;

    OMXSensorMode.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXSensorMode.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXSensorMode.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXSensorMode.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXSensorMode.bOneShot = OMX_TRUE;
    OMXSensorMode.nPortIndex = OMX_CAMPORT_INDEX_VPB0;
    OMXSensorMode.nSize = sizeof(OMX_PARAM_SENSORMODETYPE);

    status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                              OMX_IndexParamCommonSensorMode, 
                              &OMXSensorMode);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM VPB0 OMX_GetParameter for sensor mode done.\n");

       	status = OMX_SetParameter(AppPrivData->hHSMCAM, 
                                  OMX_IndexParamCommonSensorMode, 
                                  &OMXSensorMode);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: HSMCAM VPB0 OMX_SetParameter for sensor mode done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, 
            "NVM_APP: HSMCAM VPB0 sensor mode config failed .\n");
    }

    return status;
    
}*/

/** NVMAPPSetHSMCAM_VPB1_SensorMode: Configures HSMCAMERA VPB1 SensorMode.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPSetHSMCAM_VPB1_SensorMode(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_SENSORMODETYPE OMXSensorMode;

    OMXSensorMode.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXSensorMode.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXSensorMode.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXSensorMode.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXSensorMode.bOneShot = OMX_TRUE;
    OMXSensorMode.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXSensorMode.nSize = sizeof(OMX_PARAM_SENSORMODETYPE);

    status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                              OMX_IndexParamCommonSensorMode, 
                              &OMXSensorMode);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM VPB1 OMX_GetParameter for sensor mode done.\n");

        OMXSensorMode.bOneShot = OMX_TRUE;
       	status = OMX_SetParameter(AppPrivData->hHSMCAM, 
                                  OMX_IndexParamCommonSensorMode, 
                                  &OMXSensorMode);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: HSMCAM VPB1 OMX_SetParameter for sensor mode done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, 
            "NVM_APP: HSMCAM VPB1 sensor mode config failed .\n");
    }

    return status;
    
}

/** NVMAPPPrepareHSMCAMPorts: Prepares HSMCAMERA output ports.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPPrepareHSMCAMPorts(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = NVMAPPConfigHSMCAMPort_VPB0();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        status = NVMAPPConfigHSMCAMPort_VPB1();
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            /*status = NVMAPPSetHSMCAM_VPB0_SensorMode();*/
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                status = NVMAPPSetHSMCAM_VPB1_SensorMode();
            }
        }
    }

    return status;
    
}

/** NVMAPPConfigISPProcPort_VPB0: Configure ISPPROC VPB0 input port same as 
    HSMCAMERA VPB1 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigISPProcPort_VPB0(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_ISPPORT_INDEX_VPB0;

    status = OMX_GetParameter(AppPrivData->hISPPROC, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: ISPPROC OMX_GetParameter for VPB0 done.\n");
        
        OMXPortDefinition.format.video.pNativeRender = 
            ISPProcOMXPortDef.format.video.pNativeRender;
        OMXPortDefinition.format.video.nFrameWidth = 
            ISPProcOMXPortDef.format.video.nFrameWidth;
        OMXPortDefinition.format.video.nFrameHeight = 
            ISPProcOMXPortDef.format.video.nFrameHeight;
        OMXPortDefinition.format.video.nStride = 
            ISPProcOMXPortDef.format.video.nStride;
        OMXPortDefinition.format.video.nSliceHeight = 
            ISPProcOMXPortDef.format.video.nSliceHeight;
        OMXPortDefinition.format.video.bFlagErrorConcealment = 
            ISPProcOMXPortDef.format.video.bFlagErrorConcealment;
        OMXPortDefinition.format.video.eCompressionFormat = 
            ISPProcOMXPortDef.format.video.eCompressionFormat;
        OMXPortDefinition.format.video.eColorFormat = 
            ISPProcOMXPortDef.format.video.eColorFormat;
        OMXPortDefinition.format.video.pNativeWindow = 
            ISPProcOMXPortDef.format.video.pNativeWindow;
        OMXPortDefinition.nBufferCountActual = 
            ISPProcOMXPortDef.nBufferCountActual;
        OMXPortDefinition.format.video.xFramerate = InputParams.FRAME_RATE;
        OMXPortDefinition.nBufferSize = ISPProcOMXPortDef.nBufferSize;
        
        status = OMX_SetParameter(AppPrivData->hISPPROC, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: ISPPROC OMX_SetParameter for VPB0 done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: ISPPROC VPB0 configuration failed.\n");
    }

    return status;
    
}

/*OMX_ERRORTYPE NVMAPPConfigISPProcPort_VPB1(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_ISPPORT_INDEX_VPB1;

    status = OMX_GetParameter(AppPrivData->hISPPROC, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: ISPPROC OMX_GetParameter for VPB1 done.\n");
        
        status = OMX_SetParameter(AppPrivData->hISPPROC, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: ISPPROC OMX_SetParameter for VPB1 done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: ISPPROC VPB1 configuration failed.\n");
    }

    return status;
    
}*/

/** NVMAPPConfigISPProcPort_VPB2: Configure ISPPROC VPB2 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigISPProcPort_VPB2(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_ISPPORT_INDEX_VPB2;

    status = OMX_GetParameter(AppPrivData->hISPPROC, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: ISPPROC OMX_GetParameter for VPB2 done.\n");

        OMXPortDefinition.format.video.nFrameWidth = FrameWidth;
        OMXPortDefinition.format.video.nFrameHeight = FrameHeight;
        OMXPortDefinition.format.video.nStride = StrideValue;
        OMXPortDefinition.format.video.nSliceHeight = FrameHeight;
        OMXPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
        OMXPortDefinition.format.video.eCompressionFormat = 
            OMX_VIDEO_CodingUnused;
        OMXPortDefinition.format.video.eColorFormat = 
            (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
        OMXPortDefinition.format.video.pNativeWindow = 0;
        OMXPortDefinition.nBufferCountActual = 1;
        OMXPortDefinition.format.video.xFramerate = InputParams.FRAME_RATE;
        OMXPortDefinition.nBufferSize = 
            (OMXPortDefinition.format.video.nStride * 
            OMXPortDefinition.format.video.nFrameHeight);
        
        status = OMX_SetParameter(AppPrivData->hISPPROC, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: ISPPROC OMX_SetParameter for VPB2 done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: ISPPROC VPB2 configuration failed.\n");
    }

    return status;
    
}

/** NVMAPPPrepareISPProcPorts: Prepare ISPPROC input and output ports.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPPrepareISPProcPorts()
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = NVMAPPConfigISPProcPort_VPB0();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        /*status = NVMAPPConfigISPProcPort_VPB1();*/
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            status = NVMAPPConfigISPProcPort_VPB2();
        }
    }

    return status;
}

/** NVMAPPConfigJPEGQualityForPort_VPB1: Programs JPEGENC Quality factor for VPB1 
    output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigJPEGQualityForPort_VPB1(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_IMAGE_PARAM_QFACTORTYPE QFactorConfig;
    
    QFactorConfig.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    QFactorConfig.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    QFactorConfig.nVersion.s.nRevision = OMX_VERSION_REVISION;
    QFactorConfig.nVersion.s.nStep = OMX_VERSION_STEP;
    QFactorConfig.nSize = sizeof(OMX_IMAGE_PARAM_QFACTORTYPE);
    QFactorConfig.nPortIndex = OMX_JPGPORT_INDEX_VPB1;
    QFactorConfig.nQFactor = OMX_DEFAULT_JPEG_QUALITY_FACTOR;

    status = OMX_SetParameter(AppPrivData->hJPEG, 
                              OMX_IndexParamQFactor,
                              &QFactorConfig);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: JPEG QFactor set.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to set JPEG QFactor.\n");
    }

    return status;
    
}

/** NVMAPPConfigJPEGPort_VPB0: Configure JPEGENC VPB0 input port same as ISPPROC
    VPB2 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigJPEGPort_VPB0(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_JPGPORT_INDEX_VPB0;
    OMXPortDefinition.eDomain = OMX_PortDomainImage;
    OMXPortDefinition.format.image.cMIMEType = NULL;

    status = OMX_GetParameter(AppPrivData->hJPEG, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: JPEG OMX_GetParameter for VPB0 done.\n");
        
        OMXPortDefinition.nBufferCountActual = 1;
        OMXPortDefinition.format.image.cMIMEType = (OMX_STRING)"";
        OMXPortDefinition.format.image.pNativeRender = 
            (OMX_NATIVE_DEVICETYPE)NULL;
        OMXPortDefinition.format.image.nFrameWidth = FrameWidth;
        OMXPortDefinition.format.image.nFrameHeight = FrameHeight;
        OMXPortDefinition.format.image.bFlagErrorConcealment = 
            (OMX_BOOL)OMX_FALSE;
        OMXPortDefinition.format.image.eCompressionFormat = 
            (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingUnused;
        OMXPortDefinition.format.image.eColorFormat = 
            (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
        OMXPortDefinition.format.image.pNativeWindow = 
            (OMX_NATIVE_DEVICETYPE)NULL;
        
        status = OMX_SetParameter(AppPrivData->hJPEG, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: JPEG OMX_SetParameter for VPB0 done.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: JPEG VPB0 configuration failed.\n");
    }

    return status;
    
}

/** NVMAPPConfigJPEGPort_VPB1: Configure JPEGENC VPB1 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigJPEGPort_VPB1(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE OMXPortDefinition;
    
    OMXPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    OMXPortDefinition.nPortIndex = OMX_JPGPORT_INDEX_VPB1;
    OMXPortDefinition.eDomain = OMX_PortDomainImage;
    OMXPortDefinition.format.image.cMIMEType = NULL;

    status = OMX_GetParameter(AppPrivData->hJPEG, 
                              OMX_IndexParamPortDefinition, 
                              &OMXPortDefinition);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: JPEG OMX_GetParameter for VPB1 done.\n");
        
        OMXPortDefinition.nBufferCountActual = 1;
        OMXPortDefinition.format.image.cMIMEType = (OMX_STRING)"";
        OMXPortDefinition.format.image.pNativeRender = 
            (OMX_NATIVE_DEVICETYPE)NULL;
        OMXPortDefinition.format.image.nFrameWidth = 
            (OMX_U32)ISPProcOMXPortDef.format.video.nFrameWidth;
        OMXPortDefinition.format.image.nFrameHeight = 
            (OMX_U32)ISPProcOMXPortDef.format.video.nFrameHeight;
        OMXPortDefinition.format.image.bFlagErrorConcealment = 
            (OMX_BOOL)OMX_FALSE;
        OMXPortDefinition.format.image.eCompressionFormat = 
            (OMX_IMAGE_CODINGTYPE)OMX_IMAGE_CodingEXIF;
        OMXPortDefinition.format.image.eColorFormat = 
            (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
        OMXPortDefinition.format.image.pNativeWindow = 
            (OMX_NATIVE_DEVICETYPE)NULL;
        
        status = OMX_SetParameter(AppPrivData->hJPEG, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: JPEG OMX_SetParameter for VPB1 done.\n");

            status = NVMAPPConfigJPEGQualityForPort_VPB1();
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: JPEG VPB1 configuration failed.\n");
    }

    return status;
    
}

/** NVMAPPPrepareJPEGPorts: Prepare JPEGENC input and output ports.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPPrepareJPEGPorts()
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = NVMAPPConfigJPEGPort_VPB0();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        status = NVMAPPConfigJPEGPort_VPB1();
    }

    return status;
}

/* HSMCAMERA callback fn pointers */
OMX_CALLBACKTYPE NVMAPPHSMCamCallbacks = {
    NVMAPPHSMCAMEventHandler, 
    NULL, 
    NVMAPPHSMCAMFillBufferDone};

/* ISPPROC callback fn pointers */
OMX_CALLBACKTYPE NVMAPPISPProcCallbacks = {
    NVMAPPISPPROCEventHandler, 
    NVMAPPISPPROCEmptyBufferDone, 
    NVMAPPISPPROCFillBufferDone};

/* JPEGENC callback fn pointers */
OMX_CALLBACKTYPE NVMAPPJPEGCallbacks = {
    NVMAPPJPEGENCEventHandler, 
    NVMAPPJPEGENCEmptyBufferDone, 
    NVMAPPJPEGENCFillBufferDone};

/** NVMAPPDisableIrrelevantPorts: Bydefault all the ports are enabled for OMX 
    components hence desables all the irrelevant ports which are not required 
    for all the OMX components.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPDisableIrrelevantPorts(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
#if !PREVIEW_CODE
    /* Disable OMX_CAMPORT_INDEX_VPB0 if preview is not enabled */
    status = OMX_SendCommand(AppPrivData->hHSMCAM, 
                             OMX_CommandPortDisable, 
                             OMX_CAMPORT_INDEX_VPB0, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Waiting for HSMCAM VPB0 to be disabled.\n");
        sem_wait(&SemHSMCAMPortEvt);
        DebugPrint(stdout, "NVM_APP: Disabled HSMCAM VPB0.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to disable HSMCAM VPB0.\n");
    }
#endif

    /* Disable OMX_CAMPORT_INDEX_VPB2 as it is not required */
    status = OMX_SendCommand(AppPrivData->hHSMCAM, 
                             OMX_CommandPortDisable, 
                             OMX_CAMPORT_INDEX_VPB2, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Waiting for HSMCAM VPB2 to be disabled.\n");
        sem_wait(&SemHSMCAMPortEvt);
        DebugPrint(stdout, "NVM_APP: Disabled HSMCAM VPB2.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to disable HSMCAM VPB2.\n");
    }

    if(NVMAPP_IS_JPEG_REQUIRED)
    {
        /* Disable OMX_ISPPORT_INDEX_VPB1 as it is not required */
        status = OMX_SendCommand(AppPrivData->hISPPROC, 
                                 OMX_CommandPortDisable, 
                                 OMX_ISPPORT_INDEX_VPB1, 
                                 NULL);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: Waiting for ISPPROC VPB1 to be disabled.\n");
            sem_wait(&SemISPProcPortEvt);
            DebugPrint(stdout, "NVM_APP: Disabled ISPPROC VPB1.\n");
        }
        else
        {
            ReleasePrint(stdout, "NVM_APP: Failed to disable ISPPROC VPB1.\n");
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;

}

/** NVMAPPGetOMXComponentHandles: Gets OMX handles for all the required 
    components.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPGetOMXComponentHandles(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_GetHandle(&(AppPrivData->hHSMCAM),
                           (char* )"OMX.ST.HSMCamera", 
                           (void* )AppPrivData, 
                           &NVMAPPHSMCamCallbacks);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Got HSMCAM handle successfully.\n");

        if(NVMAPP_IS_JPEG_REQUIRED)
        {
            status = OMX_GetHandle(&(AppPrivData->hISPPROC), 
                                   (char* )"OMX.ST.IspProc", 
                                   (void* )AppPrivData, 
                                   &NVMAPPISPProcCallbacks);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, "NVM_APP: Got ISPPROC handle successfully.\n");
                
                status = OMX_GetHandle(&(AppPrivData->hJPEG), 
                                       (char* )"OMX.ST.VFM.JPEGEnc", 
                                       (void* )AppPrivData, 
                                       &NVMAPPJPEGCallbacks);
                
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    DebugPrint(stdout, "NVM_APP: Got JPEGEnc handle successfully.\n");
                }
                else
                {
                    returnStatus = NVMAPPErrorStatus_FAILURE;
                    ReleasePrint(stdout, "NVM_APP: Failed to get JPEGEnc handle.\n");   
                }
            }
            else
            {
                returnStatus = NVMAPPErrorStatus_FAILURE;
                ReleasePrint(stdout, "NVM_APP: Failed to get ISPPROC handle.\n"); 
            }
        }
    }
    else
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: Failed to get HSMCAM handle.\n");
    }

    return returnStatus;
}

/** NVMAPPPrepareOMXPorts: Prepares all the OMX component ports for required
    components.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPPrepareOMXPorts(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = NVMAPPPrepareHSMCAMPorts();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        if(NVMAPP_IS_JPEG_REQUIRED)
        {
            status = NVMAPPPrepareISPProcPorts();
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                status = NVMAPPPrepareJPEGPorts();
            }
        }
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPDisableIrrelevantPorts();
    }
    else
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
    
}

/** NVMAPPEnableXP70Traces: Enables Firmware trace logging using shared memory
    mechanism based on input.
        
    @return None
 */
void NVMAPPEnableXP70Traces(void)
{
    if(NVMAPP_TRUE == InputParams.ISP_DEBUG)
    {    
        /* Setup XP70 traces */
        IFM_PARAM_FIRMWARETRACETYPE xp70trace;

        xp70trace.bEnabled = OMX_TRUE;
        xp70trace.eTraceMechanism = IFM_FirmwareTraceMechanism_SharedMemory;
        xp70trace.nLogLevel = InputParams.ISP_LOGLVL;
        xp70trace.nSize = sizeof(IFM_PARAM_FIRMWARETRACETYPE);

        OMX_SetParameter(AppPrivData->hHSMCAM, 
                         (OMX_INDEXTYPE)IFM_IndexParamFirmwareTrace,
                         &xp70trace);
    }
}

/** NVMAPPSetupFlashSupport: Configures flash to be force lit based on input.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPSetupFlashSupport(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_SYMBIAN_CONFIG_FLASHCONTROLTYPE FlashConfig;

    if(NVMAPP_TRUE == InputParams.FLASH)
    {    
        FlashConfig.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
        status = OMX_GetConfig(AppPrivData->hHSMCAM, 
                               (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashControl, 
                               &FlashConfig);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            FlashConfig.eFlashControl = OMX_IMAGE_FlashControlOn;
            status = OMX_SetConfig(AppPrivData->hHSMCAM, 
                                   (OMX_INDEXTYPE)OMX_Symbian_IndexConfigFlashControl, 
                                   &FlashConfig);
        }
        if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            ReleasePrint(stdout, "NVM_APP: Flash configuration failed.\n");
            returnStatus = NVMAPPErrorStatus_FAILURE;
        }
    }

    return returnStatus;
}   

/** NVMAPPSetupMirroringSupport: Configures Mirroring effect based on input.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPSetupMirroringSupport(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_CONFIG_MIRRORTYPE MirroringConfig;

    if(NVMAPPMirroring_None != Mirroring)
    {    
        MirroringConfig.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
        status = OMX_GetConfig(AppPrivData->hHSMCAM, 
                               OMX_IndexConfigCommonMirror, 
                               &MirroringConfig);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            MirroringConfig.eMirror = (OMX_MIRRORTYPE)Mirroring;
            status = OMX_SetConfig(AppPrivData->hHSMCAM, 
                                   OMX_IndexConfigCommonMirror, 
                                   &MirroringConfig);
        }
        if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            ReleasePrint(stdout, "NVM_APP: Mirroring configuration failed.\n");
            returnStatus = NVMAPPErrorStatus_FAILURE;
        }
    }

    return returnStatus;
} 

/** NVMAPPGetSensorInfo: Gets sensor specific information (fuse id) based on 
    which app executes.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPGetSensorInfo(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    SensorFuseIDInfo.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    SensorFuseIDInfo.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    SensorFuseIDInfo.nVersion.s.nRevision = OMX_VERSION_REVISION;
    SensorFuseIDInfo.nVersion.s.nStep = OMX_VERSION_STEP;
    SensorFuseIDInfo.nSize = sizeof(OMX_SYMBIAN_CONFIG_HWINFOTYPE);
    SensorFuseIDInfo.cInfoString = NULL;
    status = OMX_GetConfig(AppPrivData->hHSMCAM,
                           (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCameraSensorInfo,
                           &SensorFuseIDInfo);
    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: Failed to get CameraSensorInfo.\n");
    }

    return status;
}

/** NVMAPPSetupOMXComponents: Sets-up all the OMX components required for 
    execution. Also enables firmware logging mechanism, configures 
    flash/mirroring and gets sensor specific hw information.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPSetupOMXComponents(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    
    returnStatus = NVMAPPGetOMXComponentHandles();
    if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
    {    
        NVMAPPEnableXP70Traces();
        returnStatus = NVMAPPSetupFlashSupport();
        if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
        {
            returnStatus = NVMAPPSetupMirroringSupport();
            if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
            {
                returnStatus = NVMAPPPrepareOMXPorts();
            }
        }
    }

    return returnStatus;
}

/** NVMAPPResetOMXBufHdrs: Resets OMX bufHdrs.
        
    @return None
 */
void NVMAPPResetOMXBufHdrs(void)
{
    HSMCAMBufHdr_VPB1 = NULL;
    ISPPROCBufHdr_VPB0 = NULL;
    ISPPROCBufHdr_VPB2 = NULL;
    JPEGENCBufHdr_VPB0 = NULL;
    JPEGENCBufHdr_VPB1 = NULL;
}

/** NVMAPPConfigHSMCAMRAWPreset: Configures HSMCAMERA to run in Processed/
    UnProcessed mode. In UnProcessed mode, none of the calibration blocks in 
    Pixel Pipe are enabled while as in Processed mode, all the calibration 
    blocks in Pixel Pipe are enabled.

    @param [in] mode
        Mode of operation to enable/disable calibration blocks in Pixel Pipe
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigHSMCAMRAWPreset(NVMAPPRAWPresetMode mode)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_SYMBIAN_CONFIG_RAWPRESETTYPE OMXModeConfigRAWPreset;
    
    OMXModeConfigRAWPreset.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXModeConfigRAWPreset.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXModeConfigRAWPreset.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXModeConfigRAWPreset.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXModeConfigRAWPreset.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXModeConfigRAWPreset.nSize = sizeof(OMX_SYMBIAN_CONFIG_RAWPRESETTYPE);

    if(NVMAPPRAWPresetMode_Processed == mode)
    {
        ReleasePrint(stdout, "NVM_APP: Running in RawImageProcessed mode.\n");
        OMXModeConfigRAWPreset.ePreset = OMX_SYMBIAN_RawImageProcessed;
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Running in RawImageUnProcessed mode.\n");
        OMXModeConfigRAWPreset.ePreset = OMX_SYMBIAN_RawImageUnprocessed;
    }
    
    status = OMX_SetConfig(AppPrivData->hHSMCAM, 
                           (OMX_INDEXTYPE)OMX_Symbian_IndexConfigRAWPreset, 
                           &OMXModeConfigRAWPreset);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Raw preset mode set.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to set raw preset mode.\n");
    }

    return status;
    
}

/** NVMAPPTransitHSMCAMLoadedToIdle: Transition HSMCAMERA from LOADED to IDLE 
    state. To move to IDLE state, HSMCAMERA will need all the buffers on it's 
    enabled ports as well as it will aquire FW resource. Once all the resources
    are aquired, it will move to IDLE state.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitHSMCAMLoadedToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_SendCommand(AppPrivData->hHSMCAM, 
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Requested HSMCAM to transit to IDLE state.\n");

        OMX_PARAM_PORTDEFINITIONTYPE OMXHSMCAMPortDefinition;
        OMXHSMCAMPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
        OMXHSMCAMPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
        OMXHSMCAMPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
        OMXHSMCAMPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
        OMXHSMCAMPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

#if PREVIEW_CODE	
        OMXHSMCAMPortDefinition.nPortIndex = OMX_CAMPORT_INDEX_VPB0;

        status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXHSMCAMPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            /* Provide buffer on OMX_CAMPORT_INDEX_VPB0 */
            HSMCAMBufHdr_VPB0 = NULL;
            status = OMX_AllocateBuffer(AppPrivData->hHSMCAM, 
                                        &HSMCAMBufHdr_VPB0, 
                                        OMX_CAMPORT_INDEX_VPB0, 
                                        NULL, 
                                        OMXHSMCAMPortDefinition.nBufferSize);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, 
                    "NVM_APP: Allocated buffer for HSMCAM VPB0.\n");
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: Failed to allocate buffer for HSMCAM VPB0.\n");
            }
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: OMX_GetParameter for HSMCAM VPB0 failed.\n");
        }
#endif
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            OMXHSMCAMPortDefinition.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
            status = OMX_GetParameter(AppPrivData->hHSMCAM, 
                                      OMX_IndexParamPortDefinition, 
                                      &OMXHSMCAMPortDefinition);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                /* Provide buffer on OMX_CAMPORT_INDEX_VPB1 */
                HSMCAMBufHdr_VPB1 = NULL;
                status = OMX_AllocateBuffer(AppPrivData->hHSMCAM, 
                                            &HSMCAMBufHdr_VPB1, 
                                            OMX_CAMPORT_INDEX_VPB1, 
                                            NULL, 
                                            OMXHSMCAMPortDefinition.nBufferSize);
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    DebugPrint(stdout, 
                        "NVM_APP: Allocated buffer for HSMCAM VPB1.\n");
                }
                else
                {
                    ReleasePrint(stdout, 
                        "NVM_APP: Failed to allocate buffer for HSMCAM VPB1.\n");
                }
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: OMX_GetParameter for HSMCAM VPB1 failed.\n");
            }
        }

    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request HSMCAM to transit to IDLE state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for HSMCAM to transit to IDLE state.\n");
        sem_wait(&SemHSMCAMCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM transitioned to IDLE state.\n");
    }
    
    return status;
}

/** NVMAPPTransitISPPROCLoadedToIdle: Transition ISPPROC from LOADED to IDLE 
    state. To move to IDLE state, ISPPROC will need all the buffers on it's 
    enabled ports as well as it will aquire FW resource. Once all the resources
    are aquired, it will move to IDLE state.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitISPPROCLoadedToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_SendCommand(AppPrivData->hISPPROC, 
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Requested ISPPROC to transit to IDLE state.\n");

        OMX_PARAM_PORTDEFINITIONTYPE OMXISPPROCPortDefinition;
        OMXISPPROCPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
        OMXISPPROCPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
        OMXISPPROCPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
        OMXISPPROCPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
        OMXISPPROCPortDefinition.nPortIndex = OMX_ISPPORT_INDEX_VPB0;
        OMXISPPROCPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            
        status = OMX_GetParameter(AppPrivData->hISPPROC, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXISPPROCPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            OMX_OSI_CONFIG_SHARED_CHUNK_METADATA OMXSharedChunkMetaData;
            OMXSharedChunkMetaData.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
            OMXSharedChunkMetaData.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
            OMXSharedChunkMetaData.nVersion.s.nRevision = OMX_VERSION_REVISION;
            OMXSharedChunkMetaData.nVersion.s.nStep = OMX_VERSION_STEP;
            OMXSharedChunkMetaData.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
            OMXSharedChunkMetaData.nSize = 
                sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);

            status = OMX_GetConfig(AppPrivData->hHSMCAM, 
                                   (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, 
                                   &OMXSharedChunkMetaData);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                OMXSharedChunkMetaData.nPortIndex = OMX_ISPPORT_INDEX_VPB0;
    
                status = OMX_SetConfig(AppPrivData->hISPPROC, 
                                       (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, 
                                       &OMXSharedChunkMetaData);
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    /* Provide buffer on OMX_ISPPORT_INDEX_VPB0 */
                    status = OMX_UseBuffer(AppPrivData->hISPPROC, 
                                           &ISPPROCBufHdr_VPB0, 
                                           OMX_ISPPORT_INDEX_VPB0,
                                           NULL, 
                                           HSMCAMBufHdr_VPB1->nAllocLen,
                                           HSMCAMBufHdr_VPB1->pBuffer);
                    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                    {
                        DebugPrint(stdout, 
                            "NVM_APP: OMX_UseBuffer for ISPPROC VPB0 done.\n");
                        
                        OMXISPPROCPortDefinition.nPortIndex =
                            OMX_ISPPORT_INDEX_VPB2;
                        
                        status = OMX_GetParameter(AppPrivData->hISPPROC, 
                                                  OMX_IndexParamPortDefinition, 
                                                  &OMXISPPROCPortDefinition);
                        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                        {
                            /* Provide buffer on OMX_ISPPORT_INDEX_VPB2 */
                            status = OMX_AllocateBuffer(AppPrivData->hISPPROC, 
                                                        &ISPPROCBufHdr_VPB2, 
                                                        OMX_ISPPORT_INDEX_VPB2, 
                                                        NULL, 
                                                        OMXISPPROCPortDefinition.nBufferSize);
                            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                            {
                                DebugPrint(stdout, 
                                    "NVM_APP: OMX_AllocateBuffer for ISPPROC VPB2 done.\n");
                            }
                            else
                            {
                                ReleasePrint(stdout, 
                                    "NVM_APP: OMX_AllocateBuffer for ISPPROC VPB2 failed.\n");
                            }
                        }
                        else
                        {
                            ReleasePrint(stdout, 
                                "NVM_APP: OMX_GetParameter for ISPPROC VPB2 failed.\n");
                        }
                    }
                    else
                    {
                        ReleasePrint(stdout, 
                            "NVM_APP: OMX_UseBuffer for ISPPROC VPB0 failed.\n");
                    }
                }
                else
                {
                    ReleasePrint(stdout, 
                        "NVM_APP: OMX_SetConfig for ISPPROC VPB0 for SHARED_CHUNK_METADATA failed.\n");
                }
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: OMX_GetConfig for HSMCAM VPB1 for SHARED_CHUNK_METADATA failed.\n");
            }
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: OMX_GetParameter for ISPPROC VPB0 failed.\n");
        }
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request ISPPROC to transit to IDLE state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for ISPPROC to transit to IDLE state.\n");
        sem_wait(&SemISPPROCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: ISPPROC transitioned to IDLE state.\n");
    }
    
    return status;
}

/** NVMAPPTransitJPEGLoadedToIdle: Transition JPEGENC from LOADED to IDLE 
    state. To move to IDLE state, JPEGENC will need all the buffers on it's 
    enabled ports. Once all the resources are aquired, it will move to IDLE 
    state.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitJPEGLoadedToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_SendCommand(AppPrivData->hJPEG, 
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        OMX_PARAM_PORTDEFINITIONTYPE OMXJPEGPortDefinition;
        OMXJPEGPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
        OMXJPEGPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
        OMXJPEGPortDefinition.nVersion.s.nRevision = OMX_VERSION_REVISION;
        OMXJPEGPortDefinition.nVersion.s.nStep = OMX_VERSION_STEP;
        OMXJPEGPortDefinition.nPortIndex = OMX_JPGPORT_INDEX_VPB0;
        OMXJPEGPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

        status = OMX_GetParameter(AppPrivData->hJPEG, 
                                  OMX_IndexParamPortDefinition, 
                                  &OMXJPEGPortDefinition);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            OMX_OSI_CONFIG_SHARED_CHUNK_METADATA OMXSharedChunkMetaData;
            OMXSharedChunkMetaData.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
            OMXSharedChunkMetaData.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
            OMXSharedChunkMetaData.nVersion.s.nRevision = OMX_VERSION_REVISION;
            OMXSharedChunkMetaData.nVersion.s.nStep = OMX_VERSION_STEP;
            OMXSharedChunkMetaData.nPortIndex = OMX_ISPPORT_INDEX_VPB2;
            OMXSharedChunkMetaData.nSize = 
                sizeof(OMX_OSI_CONFIG_SHARED_CHUNK_METADATA);

            status = OMX_GetConfig(AppPrivData->hISPPROC, 
                                   (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, 
                                   &OMXSharedChunkMetaData);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                OMXSharedChunkMetaData.nPortIndex = OMX_JPGPORT_INDEX_VPB0;
                
                status = OMX_SetConfig(AppPrivData->hJPEG, 
                                       (OMX_INDEXTYPE)OMX_OsiIndexConfigSharedChunkMetadata, 
                                       &OMXSharedChunkMetaData);
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    /* Provide buffer on OMX_JPGPORT_INDEX_VPB0 */
                    status = OMX_UseBuffer(AppPrivData->hJPEG, 
                                           &JPEGENCBufHdr_VPB0, 
                                           OMX_JPGPORT_INDEX_VPB0, 
                                           NULL,
                                           ISPPROCBufHdr_VPB2->nAllocLen,
                                           ISPPROCBufHdr_VPB2->pBuffer);
                    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                    {
                        DebugPrint(stdout, 
                            "NVM_APP: OMX_UseBuffer for JPEGENC VPB0 done.\n");

                        OMXJPEGPortDefinition.nPortIndex = OMX_JPGPORT_INDEX_VPB1;
                        status = OMX_GetParameter(AppPrivData->hJPEG, 
                                                  OMX_IndexParamPortDefinition, 
                                                  &OMXJPEGPortDefinition);
                        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                        {
                            /* Provide buffer on OMX_JPGPORT_INDEX_VPB1 */
                            status = OMX_AllocateBuffer(AppPrivData->hJPEG, 
                                                        &JPEGENCBufHdr_VPB1, 
                                                        OMX_JPGPORT_INDEX_VPB1, 
                                                        NULL, 
                                                        OMXJPEGPortDefinition.nBufferSize);
                            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                            {
                                DebugPrint(stdout, 
                                    "NVM_APP: OMX_AllocateBuffer for JPEGENC VPB1 done.\n");
                            }
                            else
                            {
                                ReleasePrint(stdout, 
                                    "NVM_APP: OMX_AllocateBuffer for JPEGENC VPB1 failed.\n");
                            }
                        }
                        else
                        {
                            ReleasePrint(stdout, 
                                "NVM_APP: OMX_GetParameter for JPEGENC VPB1 failed.\n");
                        }
                    }
                    else
                    {
                        ReleasePrint(stdout, 
                            "NVM_APP: OMX_UseBuffer for JPEGENC VPB0 failed.\n");
                    }
                }
                else
                {
                    ReleasePrint(stdout, 
                        "NVM_APP: OMX_SetConfig for JPEG VPB0 for SHARED_CHUNK_METADATA failed.\n");
                }
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: OMX_GetConfig for ISPPROC VPB2 for SHARED_CHUNK_METADATA failed.\n");
            }
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: OMX_GetParameter for JPEGENC VPB0 failed.\n");
        }
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request JPEGENC to transit to IDLE state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for JPEGENC to transit to IDLE state.\n");
        sem_wait(&SemJPEGENCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: JPEGENC transitioned to IDLE state.\n");
    }

    return status;
}

/** NVMAPPTransitHSMCAMIdleToExecuting: Transition HSMCAMERA from IDLE to 
    EXECUTING state. In EXECUTING state, component will start processing data 
    buffers.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitHSMCAMIdleToExecuting(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hHSMCAM,
                             OMX_CommandStateSet, 
                             OMX_StateExecuting, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for HSMCAM to transit to EXECUTING state.\n");
        sem_wait(&SemHSMCAMCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM transitioned to EXECUTING state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request HSMCAM to transit to EXECUTING state.\n");
    }

    return status;
}

/** NVMAPPTransitISPPROCIdleToExecuting: Transition ISPPROC from IDLE to 
    EXECUTING state. In EXECUTING state, component will start processing data 
    buffers.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitISPPROCIdleToExecuting(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_SendCommand(AppPrivData->hISPPROC,
                             OMX_CommandStateSet, 
                             OMX_StateExecuting, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for ISPPROC to transit to EXECUTING state.\n");
        sem_wait(&SemISPPROCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: ISPPROC transitioned to EXECUTING state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request ISPPROC to transit to EXECUTING state.\n");
    }

    return status;
}

/** NVMAPPTransitJPEGIdleToExecuting: Transition JPEGENC from IDLE to 
    EXECUTING state. In EXECUTING state, component will start processing data 
    buffers.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitJPEGIdleToExecuting(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;

    status = OMX_SendCommand(AppPrivData->hJPEG,
                             OMX_CommandStateSet, 
                             OMX_StateExecuting, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for JPEGENC to transit to EXECUTING state.\n");
        sem_wait(&SemJPEGENCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: JPEGENC transitioned to EXECUTING state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request JPEGENC to transit to EXECUTING state.\n");
    }

    return status;
}

/** NVMAPPApplyExposure: Applies input exposure value to be used for RAW capture.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPApplyExposure(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_CONFIG_EXPOSUREVALUETYPE OMXConfigExposure;
    
    OMXConfigExposure.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXConfigExposure.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXConfigExposure.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXConfigExposure.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXConfigExposure.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXConfigExposure.nSize = sizeof(OMX_CONFIG_EXPOSUREVALUETYPE);
    
    status = OMX_GetConfig(AppPrivData->hHSMCAM, 
                           OMX_IndexConfigCommonExposureValue, 
                           &OMXConfigExposure);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        OMXConfigExposure.xEVCompensation = 0;
        OMXConfigExposure.nShutterSpeedMsec = InputParams.EXPOSURE;
        OMXConfigExposure.bAutoShutterSpeed = OMX_FALSE;
        OMXConfigExposure.nSensitivity = InputParams.SENSITIVITY;
        OMXConfigExposure.bAutoSensitivity = OMX_FALSE;
        
        status = OMX_SetConfig(AppPrivData->hHSMCAM, 
                               OMX_IndexConfigCommonExposureValue, 
                               &OMXConfigExposure);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: Exposure value applied.\n");
        }
    }
    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: Failed to apply exposure.\n");
    }

    return status;
}

/** NVMAPPApplyWhiteBalance: Applies input white balance value to be used for 
    RAW capture.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPApplyWhiteBalance(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_CONFIG_WHITEBALCONTROLTYPE OMXConfigWhiteBalance;
    
    OMXConfigWhiteBalance.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXConfigWhiteBalance.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXConfigWhiteBalance.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXConfigWhiteBalance.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXConfigWhiteBalance.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXConfigWhiteBalance.nSize = sizeof(OMX_CONFIG_WHITEBALCONTROLTYPE);
    OMXConfigWhiteBalance.eWhiteBalControl = (OMX_WHITEBALCONTROLTYPE)0;
        
    status = OMX_SetConfig(AppPrivData->hHSMCAM,
                           OMX_IndexConfigCommonWhiteBalance,
                           &OMXConfigWhiteBalance);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: White Balance applied.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to apply White Balance.\n");
    }

    return status;
}


/** NVMAPPApplyFocusControl: configure lens position used for RAW capture.

    @return OMX_ERRORTYPE
 */
 OMX_ERRORTYPE NVMAPPApplyFocusControl(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE  OMXConfigFocusControl;

    OMXConfigFocusControl.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXConfigFocusControl.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXConfigFocusControl.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXConfigFocusControl.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXConfigFocusControl.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXConfigFocusControl.nSize = sizeof(OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE);

    OMXConfigFocusControl.eFocusControl = OMX_IMAGE_FocusControlOff;	//Manual Focus
    OMXConfigFocusControl.nFocusSteps = InputParams.Focus_VCM_TotalSteps;
    OMXConfigFocusControl.nFocusStepIndex =
        InputParams.Focus_VCM_StartStep + (InputParams.Focus_VCM_StepSize * RAWCaptureIndex);

    if(OMXConfigFocusControl.nFocusStepIndex > InputParams.Focus_VCM_TotalSteps) {
         DebugPrint(stdout, "NVM_APP: Warning: nFocusStepIndex was more that Focus_VCM_TotalSteps !!!\n");
        OMXConfigFocusControl.nFocusStepIndex = InputParams.Focus_VCM_TotalSteps;
    }

    DebugPrint(stdout, "NVM_APP: nFocusStepIndex = %lu, RAWCaptureIndex = %u.\n",
        OMXConfigFocusControl.nFocusStepIndex, RAWCaptureIndex);

    status = OMX_SetConfig(AppPrivData->hHSMCAM,
                           OMX_IndexConfigFocusControl,
                           &OMXConfigFocusControl);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Focus Control applied.\n");
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to apply Focus Control.\n");
    }

    if(InputParams.Focus_VCM_StepSize != 0) {
        sleep(1); //wait provided for lens movement finish
    }

    return status;
}


/** NVMAPPConfigHSMCAMCapture_VPB1: Enable start/stop capture on HSMCAMERA VPB1
    output port.
    
    @param [in] ConfigValue
        ConfigValue to specify start/stop capturing RAW frames.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPConfigHSMCAMCapture_VPB1(OMX_BOOL ConfigValue)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    OMX_SYMBIAN_CONFIG_BOOLEANTYPE OMXConfigCapturing;
        
    OMXConfigCapturing.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    OMXConfigCapturing.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    OMXConfigCapturing.nVersion.s.nRevision = OMX_VERSION_REVISION;
    OMXConfigCapturing.nVersion.s.nStep = OMX_VERSION_STEP;
    OMXConfigCapturing.nPortIndex = OMX_CAMPORT_INDEX_VPB1;
    OMXConfigCapturing.bEnabled = ConfigValue;
    OMXConfigCapturing.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
        
    status = OMX_SetConfig(AppPrivData->hHSMCAM,
                           (OMX_INDEXTYPE)OMX_Symbian_IndexConfigCommonExtCapturing,
                           &OMXConfigCapturing);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
                   "NVM_APP: HSMCAM VPB1 configured to %s capture.\n", 
                   ((ConfigValue == OMX_TRUE) ? "start" : "stop"));
    }
    else
    {
        ReleasePrint(stdout, 
                     "NVM_APP: Failed to configure HSMCAM VPB1 to %s capture.\n",
                     ((ConfigValue == OMX_TRUE) ? "start" : "stop"));
    }

    return status;
}

/** NVMAPPCaptureHSMCAMRawFrame_VPB1: [BMS operation] Capture one RAW frame on 
    HSMCAMERA VPB1 output port and then disable capturing.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPCaptureHSMCAMRawFrame_VPB1(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    HSMCAMBufHdr_VPB1->nOffset 	= 0;
    HSMCAMBufHdr_VPB1->nFilledLen = 0;
    
    status = OMX_FillThisBuffer(AppPrivData->hHSMCAM, 
                                HSMCAMBufHdr_VPB1);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Buffer pushed for HSMCAM VPB1.\n");

        status = NVMAPPConfigHSMCAMCapture_VPB1(OMX_TRUE);
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to push buffer for HSMCAM VPB1.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Waiting for RAW buffer on HSMCAM VPB1.\n");
        sem_wait(&SemHSMCAMFBDEvt);
        DebugPrint(stdout, "NVM_APP: Received RAW buffer on HSMCAM VPB1.\n");

        status = NVMAPPConfigHSMCAMCapture_VPB1(OMX_FALSE);
    }

    return status;
}

/** NVMAPPTransitHSMCAMExecutingToIdle: Transitions HSMCAMERA from EXECUTING
    to IDLE state. To move to IDLE state, HSMCAMERA stops data buffer processing.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitHSMCAMExecutingToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hHSMCAM,
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for HSMCAM to transit to IDLE state.\n");
        sem_wait(&SemHSMCAMCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM transitioned to IDLE state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request HSMCAM to transit to IDLE state.\n");
    }

    return status;
}

/** NVMAPPTransitISPPROCExecutingToIdle: Transitions ISPPROC from EXECUTING
    to IDLE state. To move to IDLE state, ISPPROC stops data buffer processing.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitISPPROCExecutingToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hISPPROC,
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for ISPPROC to transit to IDLE state.\n");
        sem_wait(&SemISPPROCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: ISPPROC transitioned to IDLE state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request ISPPROC to transit to IDLE state.\n");
    }

    return status;
}

/** NVMAPPTransitJPEGExecutingToIdle: Transitions JPEGENC from EXECUTING
    to IDLE state. To move to IDLE state, JPEGENC stops data buffer processing.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitJPEGExecutingToIdle(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hJPEG,
                             OMX_CommandStateSet, 
                             OMX_StateIdle, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for JPEGENC to transit to IDLE state.\n");
        sem_wait(&SemJPEGENCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: JPEGENC transitioned to IDLE state.\n");
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request JPEGENC to transit to IDLE state.\n");
    }

    return status;
}

/** NVMAPPTransitJPEGIdleToLoaded: Transitions JPEGENC from IDLE to LOADED state.
    To move to LOADED state, componet releases all it's resources.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitJPEGIdleToLoaded(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hJPEG,
                             OMX_CommandStateSet, 
                             OMX_StateLoaded, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Requested JPEGENC to transit to LOADED state.\n");

        /* Release buffer on OMX_JPGPORT_INDEX_VPB0 */
        status = OMX_FreeBuffer(AppPrivData->hJPEG, 
                                OMX_JPGPORT_INDEX_VPB0, 
                                JPEGENCBufHdr_VPB0);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: Requested free buffer for JPEGENC VPB0.\n");

             /* Release buffer on OMX_JPGPORT_INDEX_VPB1 */
            status = OMX_FreeBuffer(AppPrivData->hJPEG, 
                                    OMX_JPGPORT_INDEX_VPB1, 
                                    JPEGENCBufHdr_VPB1);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, 
                    "NVM_APP: Requested free buffer for JPEGENC VPB1.\n");
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: Failed to request free buffer for JPEGENC VPB1.\n");
            }
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: Failed to request free buffer for JPEGENC VPB0.\n");
        }
            
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request JPEGENC to transit to LOADED state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for JPEGENC to transit to LOADED state.\n");
        sem_wait(&SemJPEGENCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: JPEGENC transitioned to LOADED state.\n");
    }

    return status;
}

/** NVMAPPTransitISPPROCIdleToLoaded: Transitions ISPPROC from IDLE to LOADED 
    state. To move to LOADED state, componet releases all it's resources.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitISPPROCIdleToLoaded(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hISPPROC,
                             OMX_CommandStateSet, 
                             OMX_StateLoaded, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Requested ISPPROC to transit to LOADED state.\n");

        /* Release buffer on OMX_ISPPORT_INDEX_VPB0 */
        status = OMX_FreeBuffer(AppPrivData->hISPPROC, 
                                OMX_ISPPORT_INDEX_VPB0, 
                                ISPPROCBufHdr_VPB0);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: Requested free buffer for ISPPROC VPB0.\n");

            /* Release buffer on OMX_ISPPORT_INDEX_VPB2 */
            status = OMX_FreeBuffer(AppPrivData->hISPPROC, 
                                    OMX_ISPPORT_INDEX_VPB2, 
                                    ISPPROCBufHdr_VPB2);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, 
                    "NVM_APP: Requested free buffer for ISPPROC VPB2.\n");
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: Failed to request free buffer for ISPPROC VPB2.\n");
            }
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: Failed to request free buffer for ISPPROC VPB0.\n");
        }
            
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request ISPPROC to transit to LOADED state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for ISPPROC to transit to LOADED state.\n");
        sem_wait(&SemISPPROCCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: ISPPROC transitioned to LOADED state.\n");
    }

    return status;
}

/** NVMAPPTransitHSMCAMIdleToLoaded: Transitions HSMCAMERA from IDLE to LOADED 
    state. To move to LOADED state, componet releases all it's resources.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPTransitHSMCAMIdleToLoaded(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    status = OMX_SendCommand(AppPrivData->hHSMCAM,
                             OMX_CommandStateSet, 
                             OMX_StateLoaded, 
                             NULL);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Requested HSMCAM to transit to LOADED state.\n");
        
#if PREVIEW_CODE
        /* Release buffer on OMX_CAMPORT_INDEX_VPB0 */
        status = OMX_FreeBuffer(AppPrivData->hHSMCAM, 
                                OMX_CAMPORT_INDEX_VPB0, 
                                HSMCAMBufHdr_VPB0);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: Requested free buffer for HSMCAM VPB0.\n");
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: Failed to request free buffer for HSMCAM VPB0.\n");
        }
#endif
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            /* Release buffer on OMX_CAMPORT_INDEX_VPB1 */
            status = OMX_FreeBuffer(AppPrivData->hHSMCAM, 
                                    OMX_CAMPORT_INDEX_VPB1, 
                                    HSMCAMBufHdr_VPB1);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, 
                    "NVM_APP: Requested free buffer for HSMCAM VPB1.\n");
            }
            else
            {
                ReleasePrint(stdout, 
                    "NVM_APP: Failed to request free buffer for HSMCAM VPB1.\n");
            }
        }            
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request HSMCAM to transit to LOADED state.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for HSMCAM to transit to LOADED state.\n");
        sem_wait(&SemHSMCAMCMDDoneEvt);
        DebugPrint(stdout, 
            "NVM_APP: HSMCAM transitioned to LOADED state.\n");
    }

    return status;
}

/** NVMAPPProcessISPPROCRawFrame: [BML operation] Reconstructs YUV image using 
    Pixel Pipe from the RAW frame received from HSMCAMERA VPB1 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPProcessISPPROCRawFrame(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    
    /* Send RAW buffer to ISPPROC input */
    status = OMX_EmptyThisBuffer(AppPrivData->hISPPROC, ISPPROCBufHdr_VPB0);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Empty this buffer requested for ISPPROC VPB0.\n");
        
        ISPPROCBufHdr_VPB2->nOffset = 0;
        ISPPROCBufHdr_VPB2->nFilledLen = 0;
        /* Provide empty buffer at ISPPROC output */
        status = OMX_FillThisBuffer(AppPrivData->hISPPROC, ISPPROCBufHdr_VPB2);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: Fill this buffer requested for ISPPROC VPB2.\n");
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: Failed to request fill this buffer for ISPPROC VPB2.\n");
        }
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request empty this buffer for ISPPROC VPB0.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for RAW buffer to be processed by ISPPROC.\n");
        sem_wait(&SemISPProcFBDEvt);
        sem_wait(&SemISPProcEBDEvt);
        DebugPrint(stdout, 
            "NVM_APP: RAW buffer processed by ISPPROC.\n");
    }

    return status;
}

/** NVMAPPProcessJPEGENCCurrentFrame: [JPEG encoding] JPEG encode the given
    YUV frame received from ISPPROC VPB2 output port.
        
    @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE NVMAPPProcessJPEGENCCurrentFrame(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;          

    /* Send YUV buffer to JPEGENC input */
    status = OMX_EmptyThisBuffer(AppPrivData->hJPEG, JPEGENCBufHdr_VPB0);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Empty this buffer requested for JPEGENC VPB0.\n");
        
        JPEGENCBufHdr_VPB1->nOffset = 0;
        JPEGENCBufHdr_VPB1->nFilledLen = 0;
        /* Provide empty buffer at JPEGENC output */
        status = OMX_FillThisBuffer(AppPrivData->hJPEG, JPEGENCBufHdr_VPB1);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, 
                "NVM_APP: Fill this buffer requested for JPEGENC VPB1.\n");
        }
        else
        {
            ReleasePrint(stdout, 
                "NVM_APP: Failed to request fill this buffer for JPEGENC VPB1.\n");
        }
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: Failed to request empty this buffer for JPEGENC VPB0.\n");
    }

    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, 
            "NVM_APP: Waiting for JPEG buffer from JPEGENC.\n");
        sem_wait(&SemJPEGFBDEvt);
        sem_wait(&SemJPEGEBDEvt);
        DebugPrint(stdout, 
            "NVM_APP: JPEG buffer received from JPEGENC.\n");
    }

    return status;
}

/** NVMAPPPerformJPEGEnc: Trigger JPEG encoding of the captured RAW frame from 
    HSMCAMERA.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPPerformJPEGEnc(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;  

    /* Do BML operation on captured RAW frame to get YUV frame */
    status = NVMAPPProcessISPPROCRawFrame();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        /* Do JPEGENC on the current YUV frame to get JPEG frame */
        status = NVMAPPProcessJPEGENCCurrentFrame();
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
}

/** NVMAPPPerformRAWCapture: Apply exposure/whitebalance and trigger RAW 
    capture from HSMCAMERA.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPPerformRAWCapture(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;    

    status = NVMAPPGetSensorInfo();
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        status = NVMAPPApplyExposure();
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            status = NVMAPPApplyWhiteBalance();
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                //Check if Manual Focus is required or not
                if((InputParams.Focus_VCM_StepSize != 0) && (InputParams.Focus_VCM_TotalSteps != 0))
                {
                    status = NVMAPPApplyFocusControl();
                }
            }
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                status = NVMAPPCaptureHSMCAMRawFrame_VPB1();
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    status = NVMAPPTransitHSMCAMExecutingToIdle();
                }
            }
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
}

/** NVMAPPAquireReqComponents: Aquire required components i.e. move all the 
    components to EXECUTING state.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPAquireReqComponents(NVMAPPRAWPresetMode mode)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;    

    status = NVMAPPConfigHSMCAMRAWPreset(mode);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        status = NVMAPPTransitHSMCAMLoadedToIdle();
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            if(NVMAPP_IS_JPEG_REQUIRED)
            {
                status = NVMAPPTransitISPPROCLoadedToIdle();
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    status = NVMAPPTransitJPEGLoadedToIdle();
                }
            }
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                status = NVMAPPTransitHSMCAMIdleToExecuting();
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    if(NVMAPP_IS_JPEG_REQUIRED)
                    {
                        status = NVMAPPTransitISPPROCIdleToExecuting();
                        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                        {
                            status = NVMAPPTransitJPEGIdleToExecuting();
                        }
                    }
                }
            }
        }
    }

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
}

/** NVMAPPReleaseReqComponents: Release required components i.e. move all the 
    components to LOADED state.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPReleaseReqComponents(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;  

    if(NVMAPP_IS_JPEG_REQUIRED)
    {
        status = NVMAPPTransitISPPROCExecutingToIdle();
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            status = NVMAPPTransitJPEGExecutingToIdle();
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                status = NVMAPPTransitJPEGIdleToLoaded();
                if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
                {
                    status = NVMAPPTransitISPPROCIdleToLoaded();
                }
            }
        }
    }

    status = NVMAPPTransitHSMCAMIdleToLoaded();

    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
}

/** NVMAPPPrepareCalibrationFiles: Remove the existing calibration files (if 
    present) before executing the application for generating new calibration data.
    Prepare golden data file for validating the newly generated calibration data.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPPrepareCalibrationFiles(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    int status = 0;
    unsigned int offset = 0;
    char CalFileName[NVMAPP_MAX_FILE_NAME_LEN];
    char NVMFileName[NVMAPP_MAX_FILE_NAME_LEN];
    char GoldenDataFile[NVMAPP_MAX_FILE_NAME_LEN];

    /* reconstruct the feedback file name */
    strncpy(CalFileName, InputParams.FILE_PATH, sizeof(CalFileName));
    offset = strlen(CalFileName);

    sprintf(&CalFileName[offset],
            PATH_CALIBRATION_FILE_TO_DELETE,
            (unsigned int)(SensorFuseIDInfo.nVersion1 & 0xFF),
            (unsigned int)(SensorFuseIDInfo.nVersion1 >> 16));

    /* reconstruct the nvm data file name */
    strncpy(NVMFileName, InputParams.FILE_PATH, sizeof(NVMFileName));
    offset = strlen(NVMFileName);

    sprintf(&NVMFileName[offset],
            PATH_NVM_OUT_FILE_TO_DELETE,
            (unsigned int)(SensorFuseIDInfo.nVersion1 & 0xFF),
            (unsigned int)(SensorFuseIDInfo.nVersion1 >> 16));

    /* reconstruct the refrence golden file name */
    strncpy(GoldenDataFile, InputParams.FILE_PATH, sizeof(GoldenDataFile));
    offset = strlen(GoldenDataFile);

    sprintf(&GoldenDataFile[offset],
            PATH_GOLDEN_SAMPLE_FILE,
            (unsigned int)(SensorFuseIDInfo.nVersion1 & 0xFF),
            (unsigned int)(SensorFuseIDInfo.nVersion1 >> 16));
    
    status = remove(CalFileName);
    if(0 == status)
    {
        DebugPrint(stdout, 
                   "NVM_APP: %s file deleted successfully.\n", 
                   CalFileName);
    }
    else
    {
        DebugPrint(stdout, 
                   "NVM_APP: Unable to delete the file %s." 
                   "Ignored as the file may not exist.\n", 
                   CalFileName);
    }

    status = remove(NVMFileName);
    if(0 == status)
    {
        DebugPrint(stdout, 
                   "NVM_APP: %s file deleted successfully.\n", 
                   NVMFileName);
    }
    else
    {
        DebugPrint(stdout, 
                   "NVM_APP: Unable to delete the file %s."
                   "Ignored as the file may not exist.\n",
                   NVMFileName);
    }

    DebugPrint(stdout, 
               "NVM_APP: Golden sample file path - %s\n", 
               GoldenDataFile);
    if(NULL != (fpGoldenData = fopen(GoldenDataFile, "rb")))
    {
        /* Seek to start of file. */
        fseek(fpGoldenData, 0, SEEK_SET);
    }
    else
    {
        ReleasePrint(stdout, 
                     "NVM_APP: Failed to open %s\n", 
                     GoldenDataFile);
        returnStatus = NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
    
}

/** NvmCalibrationStore: Stores the calibration feedback to the file.
        
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NvmCalibrationStore(void)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    unsigned int offset = 0;
    char NVMExecutionStatusFile[NVMAPP_MAX_FILE_NAME_LEN];
    FILE* fpNVMExecutionStatus = NULL;

    /* reconstruct the feedback file name */
    strncpy(NVMExecutionStatusFile, 
            InputParams.FILE_PATH, 
            sizeof(NVMExecutionStatusFile));
    offset = strlen(NVMExecutionStatusFile);
    
    sprintf(&NVMExecutionStatusFile[offset],
            PATH_CALIBRATION_FILE_RENAME,
            (unsigned int)(SensorFuseIDInfo.nVersion1 & 0xFF), 
            (unsigned int)(SensorFuseIDInfo.nVersion1 >> 16));

    if(NULL == (fpNVMExecutionStatus = fopen(NVMExecutionStatusFile, "wb")))
    {
        ReleasePrint(stdout, "NVM_APP: Failed to open Calibration file.\n");
        status = NVMAPPErrorStatus_FAILURE;
    }
    else
    {
        fwrite(&CalibrationFeedback.passfail, 
               strlen(CalibrationFeedback.passfail), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.GoldenDataR_r, 
               strlen(CalibrationFeedback.GoldenDataR_r), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.GoldenDataR_g, 
               strlen(CalibrationFeedback.GoldenDataR_g), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.GoldenDataR_b, 
               strlen(CalibrationFeedback.GoldenDataR_b), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.ComputeNvmDataC_r, 
               strlen(CalibrationFeedback.ComputeNvmDataC_r), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.ComputeNvmDataC_b, 
               strlen(CalibrationFeedback.ComputeNvmDataC_b), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.ComputeNvmDataC_g, 
               strlen(CalibrationFeedback.ComputeNvmDataC_g), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.VariationVRg, 
               strlen(CalibrationFeedback.VariationVRg), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.VariationVBg, 
               strlen(CalibrationFeedback.VariationVBg), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.Fuse_ID1, 
               strlen(CalibrationFeedback.Fuse_ID1), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.Fuse_ID2, 
               strlen(CalibrationFeedback.Fuse_ID2), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.Fuse_ID3, 
               strlen(CalibrationFeedback.Fuse_ID3), 
               1, 
               fpNVMExecutionStatus);
        fwrite(&CalibrationFeedback.Fuse_ID4, 
               strlen(CalibrationFeedback.Fuse_ID4), 
               1, 
               fpNVMExecutionStatus);

        fclose(fpNVMExecutionStatus);
    }

    return status;
}

/** NVMAPPCamputeCalibration: Camputes calibration data based on the RAW 
    UnProcessed image data captured using STECC lib. Validates the generated 
    calibration data against golden refrence data to check if it is w/i 
    tolerence range provided as input. 
    Calibration is successful is the generated data is w/i tolerence else 
    it fails. 
    Feedback data is polulated accordingly and dumped in file.
    Calibration data is stored in file if calibration was done successfully else
    ignored.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPCamputeCalibration(void)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_SUCCESS;
    char tempstr[50] = {0};
    unsigned char* bufferR = NULL;
    char NVMDataFile[NVMAPP_MAX_FILE_NAME_LEN];
    FILE* fpNVMData = NULL;
     
    sprintf(&tempstr[0], "Tolerance:%f\n", (float)InputParams.TOLERANCE);
    strcpy(CalibrationFeedback.Tolerance, tempstr);

    sprintf(&tempstr[0], "Fuse_ID[1]=0x%x\n", 0);
    strcpy(CalibrationFeedback.Fuse_ID1, tempstr);
    
    sprintf(&tempstr[0], "Fuse_ID[2]=0x%x\n", 0);
    strcpy(CalibrationFeedback.Fuse_ID2, tempstr);
    
    sprintf(&tempstr[0], "Fuse_ID[3]=0x%x\n", 0);
    strcpy(CalibrationFeedback.Fuse_ID3, tempstr);
    
    sprintf(&tempstr[0], "Fuse_ID[4]=0x%x\n", (unsigned int)SensorFuseIDInfo.nVersion2);

    strcpy(CalibrationFeedback.Fuse_ID4, tempstr);

    if(stecc_compute(NVMAPPImageDescriptor, 
                     pRAWImageArray, 
                     InputParams.RAW_CNT,
                     pTMPDataBuf,
                     pCALDataBufCurrent))
    {
        stecc_check_parameters check_parameters;
        stecc_check_intermediates check_intermediates;
        unsigned int code = 0;
        int readvalue = 0;
        unsigned int offset = 0;

        /* Load reference golden calibration data */
        status = NVMAPPPrepareCalibrationFiles();
        bufferR = (unsigned char*)malloc(CALDataBufSize);
        if(NULL != bufferR && NVMAPP_IS_STATUS_SUCCESS(status))
        {
            readvalue = fread(bufferR, 1, CALDataBufSize, fpGoldenData);
            if(-1 != readvalue)
            {
                /* Populate the parameters to validate the computed calibration 
                   data */
                switch(InputParams.RADIAL_THRESHOLD_FOR_SENSITIVITY_CHECK)
                {
                    case NVMAPP_TRUE:
                        check_parameters.channels_sensitivities_check_method = 
                            stecc_channels_sensitivities_check_method_1x2d;
                        break;

                    case NVMAPP_FALSE:
                    default:
                        check_parameters.channels_sensitivities_check_method = 
                            stecc_channels_sensitivities_check_method_2x1d;
                        break;
                }
                check_parameters.channels_sensitivities_tolerance = 
                    (float)InputParams.TOLERANCE;
                check_parameters.lens_shading_min_average_minmaxratio = 
                    (float)InputParams.LSC_MIN_AVG_MINMAXRATIO;
                check_parameters.lens_shading_min_normcrosscorr = 
                    (float)InputParams.LSC_MIN_NORMCROSSCORR;
                check_parameters.lens_shading_min_sigma_minmaxratio = 
                    (float)InputParams.LSC_MIN_SIGMA_MINMAXRATIO;

                /* Check the generated calibration data and store only if 
                   generated calibration data is w/i tolerance i.e. valid */            
                if(stecc_check(NVMAPPImageDescriptor, 
                               check_parameters, 
                               (void *)bufferR, 
                               pCALDataBufCurrent, 
                               check_intermediates, 
                               code))
                {
                    ReleasePrint(stdout, 
                        "NVM_APP: Successfully computed VALID calibration data.\n");

                    /* reconstruct the nvm data file name */
                    strncpy(NVMDataFile, 
                            InputParams.FILE_PATH, 
                            sizeof(NVMDataFile));
                    offset = strlen(NVMDataFile);

                    sprintf(&NVMDataFile[offset], 
                            PATH_NVM_OUT_FILE_RENAME,
                            (unsigned int)(SensorFuseIDInfo.nVersion1 & 0xFF),
                            (unsigned int)(SensorFuseIDInfo.nVersion1 >> 16));

                    if(NULL != (fpNVMData = fopen(NVMDataFile, "wb")))
                    {
                        if(1 == fwrite(pCALDataBufCurrent, 
                                       CALDataBufSize, 
                                       1, 
                                       fpNVMData))
                        {
                            ReleasePrint(stdout, 
                                "NVM_APP: Successfully written NVM output file.\n");

                            sprintf(&tempstr[0], "Test:Success\n");
                            strcpy(CalibrationFeedback.passfail, tempstr);
                        }
                        else
                        {
                            ReleasePrint(stdout,
                                "NVM_APP: Failed to write NVM output file.\n");
                            status = NVMAPPErrorStatus_FAILURE;
                        }

                    }
                    else
                    {
                        ReleasePrint(stdout,
                            "NVM_APP: Failed to open NVM output file.\n");
                        status = NVMAPPErrorStatus_FAILURE;
                    }
                }
                else
                {
                    ReleasePrint(stdout, 
                        "NVM_APP: Failed to compute VALID calibration data.\n");
                    status = NVMAPPErrorStatus_FAILURE;
                }
          
                sprintf(&tempstr[0], 
                        "VariationVRg:%f\n",
                        check_intermediates.var_rg); 
                strcpy(CalibrationFeedback.VariationVRg, tempstr);
                
                sprintf(&tempstr[0], 
                        "VariationVBg:%f\n",
                        check_intermediates.var_bg); 
                strcpy(CalibrationFeedback.VariationVBg, tempstr);

                sprintf(&tempstr[0], 
                        "GoldenDataR_r:%f\n",
                        check_intermediates.ref_r); 
                strcpy(CalibrationFeedback.GoldenDataR_r, tempstr);
                
                sprintf(&tempstr[0], 
                        "GoldenDataR_g:%f\n",
                        check_intermediates.ref_g); 
                strcpy(CalibrationFeedback.GoldenDataR_g, tempstr);
                
                sprintf(&tempstr[0], 
                        "GoldenDataR_b:%f\n",
                        check_intermediates.ref_b); 
                strcpy(CalibrationFeedback.GoldenDataR_b, tempstr);

                sprintf(&tempstr[0], 
                        "ComputeNvmDataC_r:%f\n",
                        check_intermediates.cur_r); 
                strcpy(CalibrationFeedback.ComputeNvmDataC_r, tempstr);
                
                sprintf(&tempstr[0], 
                        "ComputeNvmDataC_g:%f\n",
                        check_intermediates.cur_g); 
                strcpy(CalibrationFeedback.ComputeNvmDataC_g, tempstr);
                
                sprintf(&tempstr[0], 
                        "ComputeNvmDataC_b:%f\n",
                        check_intermediates.cur_b); 
                strcpy(CalibrationFeedback.ComputeNvmDataC_b, tempstr);
            }
            else
            {
                ReleasePrint(stdout, "NVM_APP: Failed to read golden data.\n");
                status = NVMAPPErrorStatus_FAILURE;
            }
        }
        else if(NULL == bufferR)
        {
            ReleasePrint(stdout, "NVM_APP: Failed to allocate buffer.\n");
            status = NVMAPPErrorStatus_FAILURE;
        }

        if(!NVMAPP_IS_STATUS_SUCCESS(status))
        {
            sprintf(&tempstr[0],"Test:Fail\n");
            strcpy(CalibrationFeedback.passfail, tempstr);
        }

        NvmCalibrationStore();
    }
    else
    {
        ReleasePrint(stdout, 
            "NVM_APP: STECC Failed to compute calibration data.\n");
        status = NVMAPPErrorStatus_FAILURE;
    }

    if(NULL != fpNVMData)
    {
        fclose(fpNVMData);
        fpNVMData = NULL;
    }
    
    if(NULL != bufferR)
    {
        free(bufferR);
        bufferR = NULL;
    }

    return status;
    
}

/** NVMAPPPrepareImageDescriptor: Prepares image descriptor as per STECC 
    requirements. Required during UnProcessed mode of operation only.
    Prepares buffer layout required for calibration process.

    @return NVMAPPErrorStatus

    Buffer layout:
                              --------------------
                              |                  |
                              |         ---------|---------------------
                              |         |        |                    |
                              |         |        V                    V
    ############################################################################
    #       #       #       #   |     |   #              |     |               #
    # TMP   # CAL   # CAL   #   |     |   #              |     |               #
    # DATA  # DATA  # DATA  # 1 | ... | N #    IMAGE-1   | ... | IMAGE-N       #
    # BUF   # BUF   # BUF   #   |     |   #              |     |               #
    #       # CURR  # REF   #   |     |   #              |     |               #
    #       #       #       #   |     |   #              |     |               #
    ############################################################################
    <-------><------><------><------------><----------------------------------->
    TMP DATA  CURR    REF    RawImageArray,    RawImageBuf, contains actual
    Buffer    CAL     CAL    Contains PTR      Raw image data.
    req for   DATA    DATA   to RAW images.
    CALIB     BUFF    BUFF

    * N => RAW_CNT : No of raw images required.
    
 */
NVMAPPErrorStatus NVMAPPPrepareImageDescriptor()
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;

    if(NVMAPP_IS_FIRST_RAW_CAPTURE)
    {
        /* Image width and height */
        NVMAPPImageDescriptor.width = 
            ISPProcOMXPortDef.format.video.nFrameWidth;
        NVMAPPImageDescriptor.height = 
            ISPProcOMXPortDef.format.video.nFrameHeight;

        /* raw bayer order of RAW capture */
        if(!strcmp(InputParams.BAYER_ORDER, "R"))
        {
            NVMAPPImageDescriptor.phase = stecc_bayer_phase_first_r;
        }
        else if(!strcmp(InputParams.BAYER_ORDER, "Gr"))
        {
            NVMAPPImageDescriptor.phase = stecc_bayer_phase_first_gr;
        }
        else if(!strcmp(InputParams.BAYER_ORDER, "Gb"))
        {
            NVMAPPImageDescriptor.phase = stecc_bayer_phase_first_gb;
        }
        else if(!strcmp(InputParams.BAYER_ORDER, "B"))
        {
            NVMAPPImageDescriptor.phase = stecc_bayer_phase_first_b;
        }
        
        NVMAPPImageDescriptor.stride = ISPProcOMXPortDef.format.video.nStride;
        NVMAPPImageDescriptor.pedestal_value = NVMAPP_DEFAULT_PEDESTAL_VALUE;
        NVMAPPImageDescriptor.clipping_value = NVMAPP_DEFAULT_CLIPPING_VALUE;

        /* RAW image buffer size */
        RAWImageSize = NVMAPPImageDescriptor.stride * 
            NVMAPPImageDescriptor.height;

        /* Get calibration buffer requirements */
        stecc_get_buffers_size(NVMAPPImageDescriptor, 
                               TMPDataBufSize, 
                               CALDataBufSize);

        /* Total buffer size required for calibration process */
        TotalDataBufSize = TMPDataBufSize + 
                           (2 * CALDataBufSize) + 
                           (InputParams.RAW_CNT * (RAWImageSize + sizeof(void*)));
        
        pDataBuffer = malloc(TotalDataBufSize);
        
        if(NULL == pDataBuffer)
        {
            returnStatus = NVMAPPErrorStatus_FAILURE;
            ReleasePrint(stdout, "NVM_APP: Memory allocation failed.\n");
        }
        else
        {
            /* Arrage the single allocated buffer as per requirement. 
               Please refer fn header for detailed layout. */
            pTMPDataBuf = (unsigned char *)pDataBuffer + 0;
            pCALDataBufCurrent = (unsigned char *)pDataBuffer + TMPDataBufSize;
            pCALDataBufRef = 
                (unsigned char *)pDataBuffer + TMPDataBufSize + CALDataBufSize;

            pRAWImageArray = (void**)((unsigned char *)pDataBuffer + 
                                      TMPDataBufSize + 
                                      (2 * CALDataBufSize));
            pRAWImageBuf = (unsigned char *) pDataBuffer + 
                           TMPDataBufSize + 
                           (2 * CALDataBufSize) + 
                           (InputParams.RAW_CNT * sizeof(void*));
        }
    }

    if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
    {
        /* Populate RAWImageArray w/ the image buffer pointers.
           Please refer fn header for detailed layout. */
        pRAWImageArray[RAWCaptureIndex] = 
            (void*)(pRAWImageBuf + (RAWImageSize * RAWCaptureIndex));
    }

    return returnStatus;
}

/** NVMAPPSemInit: Create/initialize all the required semaphores.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPSemInit(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    
    if(-1 == sem_init(&SemHSMCAMFBDEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemHSMCAMFBDEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemHSMCAMCMDDoneEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemHSMCAMCMDDoneEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemISPProcFBDEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemISPProcFBDEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemISPProcEBDEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemISPProcEBDEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemISPPROCCMDDoneEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemISPPROCCMDDoneEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemHSMCAMPortEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemHSMCAMPortEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemISPProcPortEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemISPProcPortEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemJPEGEBDEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemJPEGEBDEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemJPEGFBDEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemJPEGFBDEvt init failed.\n");
    }
    else if(-1 == sem_init(&SemJPEGENCCMDDoneEvt, 0, 0))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: SemJPEGENCCMDDoneEvt init failed.\n");
    }

    return returnStatus;

}

/** NVMAPPSemDestroy: Destroy all the required semaphores.
    
    @return None
 */
void NVMAPPSemDestroy(void)
{
    sem_destroy(&SemHSMCAMFBDEvt);
    sem_destroy(&SemHSMCAMCMDDoneEvt);
    sem_destroy(&SemISPProcFBDEvt);
    sem_destroy(&SemISPProcEBDEvt);
    sem_destroy(&SemISPPROCCMDDoneEvt);
    sem_destroy(&SemHSMCAMPortEvt);
    sem_destroy(&SemISPProcPortEvt);
    sem_destroy(&SemJPEGEBDEvt);
    sem_destroy(&SemJPEGFBDEvt);
    sem_destroy(&SemJPEGENCCMDDoneEvt);
    
    return;
}

/** NVMAPPClean: Release/free all the aquired resources.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPClean(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;

    if(NULL != AppPrivData->hHSMCAM)
    {
        status = OMX_FreeHandle(AppPrivData->hHSMCAM);
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: Freed HSMCAM handle.\n");
        }
        else
        {
            ReleasePrint(stdout, "NVM_APP: Failed to free HSMCAM handle.\n");
        }
    }

    if(NVMAPP_IS_JPEG_REQUIRED)
    {
        if(NULL != AppPrivData->hISPPROC)
        {
            status = OMX_FreeHandle(AppPrivData->hISPPROC);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, "NVM_APP: Freed ISPPROC handle.\n");
            }
            else
            {
                ReleasePrint(stdout, "NVM_APP: Failed to free ISPPROC handle.\n");
            }
        }

        if(NULL != AppPrivData->hJPEG)
        {
            status = OMX_FreeHandle(AppPrivData->hJPEG);
            if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
            {
                DebugPrint(stdout, "NVM_APP: Freed JPEGENC handle.\n");
            }
            else
            {
                ReleasePrint(stdout, "NVM_APP: Failed to free JPEGENC handle.\n");
            }
        }
    }

    if(NULL != fpGoldenData)
    {
        fclose(fpGoldenData);
        fpGoldenData = NULL;
    }

    if(AppPrivData)
    {
        free(AppPrivData);
    }
    
    if(pDataBuffer)
    {
        free(pDataBuffer);
    }
    
    status = OMX_Deinit();

    NVMAPPSemDestroy();
    
    if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        returnStatus = NVMAPPErrorStatus_FAILURE;
        ReleasePrint(stdout, "NVM_APP: clean-up failed.\n");
    }

    return returnStatus;
}

/** NVMAPPDisplayFinalResult: Displays the final application execution status.
    Also displays calibration status if calibration data was generated 
    successfully w/i tolerance range.

    @param [in] status
        final app execution status
    
    @return None
 */
void NVMAPPDisplayFinalResult(NVMAPPErrorStatus status)
{
    ReleasePrint(stdout, "\n#############################################\n\n");
    if(NVMAPP_IS_STATUS_SUCCESS(status))
    {
        ReleasePrint(stdout, "NVM_APP: Successfully executed.\n");
        if(NVMAPP_IS_CALIBRATION_REQUIRED)
        {
            ReleasePrint(stdout, 
                         "NVM_APP: CALIBRATION STATUS: %s\n", 
                         CalibrationFeedback.passfail);
        }
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to executed.\n");
    }
    ReleasePrint(stdout, "\n#############################################\n\n");
}

/** NVMAPPExecute: Main NVM_APP application execution body. Runs for required 
    number of RAW_CNT in Unprocessed and Processed mode to capture RAW and 
    JPEG image.

    @param [in] mode
        mode of current execution
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPExecute(NVMAPPRAWPresetMode mode)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    unsigned int NumImages = InputParams.RAW_CNT;

    CurrentMode = mode;
    RAWCaptureIndex = 0;

    while(NumImages)
    {
        if(NVMAPPRAWPresetMode_UnProcessed == mode)
        {
            /* Prepare image descriptor to be used during calibration 
               computation. */
            returnStatus = NVMAPPPrepareImageDescriptor();
        }
        returnStatus = NVMAPPAquireReqComponents(mode);
        if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
        {
            if((RAWCaptureIndex == 0) && (InputParams.bSaveSensorNVMData == NVMAPP_TRUE))
            {
                returnStatus = NVMAPPSaveSensorNVMData();
            }
        }
        if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
        {
            returnStatus = NVMAPPPerformRAWCapture();
            if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
            {
                if(NVMAPP_IS_JPEG_REQUIRED)
                {
                    returnStatus = NVMAPPPerformJPEGEnc();
                }
                if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
                {       
                    RAWCaptureIndex++;
                    NumImages--;
                    DebugPrint(stdout, "NVM_APP: %d RAW images captured.\n", 
                               RAWCaptureIndex);
                }
            }
            returnStatus = NVMAPPReleaseReqComponents();
            NVMAPPResetOMXBufHdrs();
        }
        if(!NVMAPP_IS_STATUS_SUCCESS(returnStatus))
        {
            break;
        }
    }

    return returnStatus;
}

/** NVMAPPRun: Contains NVM_APP execution logic. 1st of all it runs in 
    Unprocessed mode to capture RAW and JPEG images. Followed by generating
    calibration data based on captured RAW images. 
    If the generated calibration data is valid then executes in Processed mode
    to capture RAW and JPEG images again else exits w/o runing in Processed 
    mode.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPRun(void)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_FAILURE;

    /* 1. Execute in UnProcessed mode */
    status = NVMAPPExecute(NVMAPPRAWPresetMode_UnProcessed);
    if(NVMAPP_IS_STATUS_SUCCESS(status))
    {
        if(NVMAPP_IS_CALIBRATION_REQUIRED)
        {
            /* 2. Generate calibration data */
            status = NVMAPPCamputeCalibration();
            if(NVMAPP_IS_STATUS_SUCCESS(status))
            {
                /* 3. Execute in Processed mode */
                status = NVMAPPExecute(NVMAPPRAWPresetMode_Processed);
            }
        }
    }
    
    return status;
}

/** NVMAPPInit: Initializes all the required resources.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPInit(void)
{
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;
    OMX_ERRORTYPE OMXStatus = OMX_ErrorNone;

    returnStatus = NVMAPPSemInit();
    if(NVMAPP_IS_STATUS_SUCCESS(returnStatus))
    {
        OMXStatus = OMX_Init();
        if(NVMAPP_IS_OMX_STATUS_SUCCESS(OMXStatus))
        {
            DebugPrint(stdout, "NVM_APP: OMX_Init success.\n");
            AppPrivData = 
                (NVMAPPPrivateDataType*)malloc(sizeof(NVMAPPPrivateDataType));
            if(NULL == AppPrivData)
            {
                returnStatus = NVMAPPErrorStatus_FAILURE;
                ReleasePrint(stdout, "NVM_APP: Memory Allocation failed.\n");
            }
            else
            {
                memset(AppPrivData, 0, sizeof(NVMAPPPrivateDataType));
                RAWCaptureIndex = 0;
                returnStatus = NVMAPPSetupOMXComponents();
            }
        }
        else
        {
            returnStatus = NVMAPPErrorStatus_FAILURE;
            ReleasePrint(stdout, "NVM_APP: OMX_Init failed.\n");
        }
    }

    return returnStatus;
    
}

/** NVMAPPCheckCompatibility: Checks the compatibility b/t NVM_APP and STECC
    library and executes only if both are compatible else exits immediatly.
    
    @return NVMAPPErrorStatus
 */
NVMAPPErrorStatus NVMAPPCheckCompatibility(void)
{
    NVMAPPErrorStatus status = NVMAPPErrorStatus_FAILURE;
    unsigned int STECCRevMajor = 0;
    unsigned int STECCRevMinor = 0;
    unsigned int STECCRevBuild = 0;
    
    ReleasePrint(stdout, 
                 "\nNVM_APP: BUILD_REVISION = %d\n", 
                 NVM_APP_REVISION_BUILD);
    ReleasePrint(stdout, 
                 "NVM_APP: STECC version = %d.%d\n", 
                 EXPECTED_STECC_REVISION_MAJOR, 
                 EXPECTED_STECC_REVISION_MINOR);
        
    stecc_revision(STECCRevMajor, STECCRevMinor, STECCRevBuild);
    if((EXPECTED_STECC_REVISION_MAJOR == STECCRevMajor) &&
       (EXPECTED_STECC_REVISION_MINOR == STECCRevMinor))
    {
        status = NVMAPPErrorStatus_SUCCESS;
        DebugPrint(stdout, "\nNVM_APP: Compatibility check passed.\n");
    }
    else
    {
        ReleasePrint(stdout, "\nNVM_APP: Compatibility check failed.\n");
    }

    return status;
}

int main(int argc, char **argv)
{
    pid_t pid, wpid;
    int status;
    
    pid = fork();
    if(pid < 0)
    {
        DebugPrint(stdout, "\nNVM_APP: Fork failed!\n\n");
    }
    else if(0 == pid)
    {
        NVMAPPErrorStatus status = NVMAPPErrorStatus_FAILURE;

        /* Check compatibility of NVM_APP and STECC library. */
        status = NVMAPPCheckCompatibility();
        if(NVMAPP_IS_STATUS_SUCCESS(status))
        {
            /* Interpret input from ConfigFile/CMDLine */
            status = NVMAPPInterpretInput(argc, argv);
            if(NVMAPP_IS_STATUS_SUCCESS(status))
            {
                /* Initialize resources */
                status = NVMAPPInit();
                if(NVMAPP_IS_STATUS_SUCCESS(status))
                {
                    /* Run NVM_APP */
                    status = NVMAPPRun();
                }
                /* Release resources */
                NVMAPPClean();
            }
        }
        /* Display final execution/calibration result. */
        NVMAPPDisplayFinalResult(status);
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, 0);
        }
        while((WIFEXITED(status) == 0) &&
              (WIFSIGNALED(status) == 0) &&
              (WIFSTOPPED(status) == 0));
    }
    
    return 0;
    
}


NVMAPPErrorStatus NVMAPPSaveSensorNVMData(void)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    NVMAPPErrorStatus returnStatus = NVMAPPErrorStatus_SUCCESS;

    OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE SensorNvmData;
    unsigned int totalSize = 0;
    unsigned char *pData = NULL;
    OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE *pSensorNVMData;
    FILE *fp = NULL;
    int num = 0;

    DebugPrint(stdout, "NVM_APP: NVMAPPSaveSensorNVMData\n");

    SensorNvmData.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
    SensorNvmData.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
    SensorNvmData.nVersion.s.nRevision = OMX_VERSION_REVISION;
    SensorNvmData.nVersion.s.nStep = OMX_VERSION_STEP;
    SensorNvmData.nSize = sizeof(OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE);
    SensorNvmData.nNVMBytes = 0;

    status = OMX_GetConfig(AppPrivData->hHSMCAM, (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSensorNvmData, &SensorNvmData);
    if(NVMAPP_IS_OMX_STATUS_SUCCESS(status))
    {
        DebugPrint(stdout, "NVM_APP: Success. SensorNvmData.nNVMBytes = %lu\n", SensorNvmData.nNVMBytes);

        totalSize = sizeof(OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE) + SensorNvmData.nNVMBytes;
        pData = (unsigned char *) calloc(totalSize, sizeof(unsigned char));

        if(pData == NULL)
        {
            DebugPrint(stdout, "NVM_APP: Not able to allocate memory of size %d\n", totalSize);
            return NVMAPPErrorStatus_FAILURE;
        }

        DebugPrint(stdout, "NVM_APP: Allocate memory of size %d\n", totalSize);

        pSensorNVMData = (OMX_SYMBIAN_CONFIG_SENSORNVMDATATYPE *)pData;
        pSensorNVMData->nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
        pSensorNVMData->nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
        pSensorNVMData->nVersion.s.nRevision = OMX_VERSION_REVISION;
        pSensorNVMData->nVersion.s.nStep = OMX_VERSION_STEP;
        pSensorNVMData->nSize = totalSize;
        pSensorNVMData->nNVMBytes = SensorNvmData.nNVMBytes;

        status = OMX_GetConfig(AppPrivData->hHSMCAM, (OMX_INDEXTYPE)OMX_Symbian_IndexConfigSensorNvmData, pSensorNVMData);
        if(!NVMAPP_IS_OMX_STATUS_SUCCESS(status))
        {
            DebugPrint(stdout, "NVM_APP: Not able to read Sensor NVM Data by getConfig\n");
            free(pData);
            return NVMAPPErrorStatus_FAILURE;
        }

        fp = fopen(InputParams.aSensorNVMDataPath, "w+");
        if(fp != NULL)
        {
            num = fwrite((void *)(pSensorNVMData->NVMData), sizeof(OMX_U8), pSensorNVMData->nNVMBytes, fp);
            DebugPrint(stdout, "NVM_APP: File written ... bytes = %d \n", num);
            fclose(fp);
        }
        else
        {
            DebugPrint(stdout, "NVM_APP: Not able to open file %s\n", InputParams.aSensorNVMDataPath);
            free(pData);
            return NVMAPPErrorStatus_FAILURE;
        }

        free(pData);
    }
    else
    {
        ReleasePrint(stdout, "NVM_APP: Failed to read nNVMBytes.\n");
        return NVMAPPErrorStatus_FAILURE;
    }

    return returnStatus;
}

