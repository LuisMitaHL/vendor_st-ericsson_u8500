/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Should be remove when BML will be clean
#include "hi_register_acces.h"
#include "ipp.h"

//
#include "ite_nmf_standard_functions.h"

#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"
#include "ssc.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"

#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_focuscontrol_functions.h"
#include "ite_nmf_sensor_output_mode.h"

#include "ite_nmf_zoom_functions.h"
#include "ite_debug.h"


#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
#   include "ite_framerate.h"

//For Linux
#   include "OMX_Types.h"
#   include <unistd.h>
#   include <fcntl.h>
#   if !defined(__ARM_SYMBIAN)
#      include <linux/fb.h>
#      include <sys/mman.h>
#      include <sys/ioctl.h>
#   endif
#   include <assert.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include "mcde_ioctls.h"
#endif
#include <cm/inc/cm_macros.h>
#include <test/api/test.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "ite_recordsystem_time.h"
#include "ite_iniparser.h"
#include "ite_sia_buffer.h"
#include "ite_nmf_glace_geometry.h"

extern struct s_grabParams      grabparamsBMS;
extern struct s_grabParams      grabparamsHR;
extern struct s_grabParams      grabparamsLR;
extern volatile t_uint32        g_grabHR_infinite;
extern volatile t_uint32        g_grabLR_infinite;
extern ts_siapicturebuffer      GrabBufferLR[];
extern ts_siapicturebuffer      GrabBufferHR[];
extern ts_siapicturebuffer      GrabBufferBMS[];
extern ts_siapicturebuffer      LCDBuffer;
extern ts_siapicturebuffer      GamBuffer;
extern ts_sensInfo              SensorsInfo[2];
extern void                     ITE_NMF_FocusStatsPrepare (void);
extern void                     ITE_NMF_UpdateGlaceParam (void);
extern void                     ITE_NMF_UpdateHistoParam (void);
extern void                     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);
extern unsigned long long       ITE_NMF_GlaceAndHistoExpReqNoWait (void);

extern t_uint32                 ite_sensorselect;

extern t_uint32                 g_test_perf_status;
extern ts_stat_test_usecase 	Glace_uc;
extern StatsBuffersStorage_t    *g_pStatsBufferStorage;
#define SW3A_HISTO_BIN_NB       256	         //  Cheack the valus from SW_3A_INTERNAL_HPP    #define SW3A_HISTO_BIN_NB

/*
  #if !(defined(__PEPS8500_SIA) || defined(_SVP_) || defined(__ARM_LINUX))
  extern ts_b2r2_config g_B2R2_node;
  #endif*/
extern t_uint32                 g_B2R2HR_infinite;
extern t_uint32                 g_dplLR_infinite;
extern t_uint32                 g_dplHR_infinite;

extern ts_sia_usecase           usecase;

extern t_uint32                 smia_base_address;
extern int                      discoveredID, discoveredRevision;

extern t_cm_domain_id           domainId;
extern t_bool                   TraceBMLEvent;

extern tps_siaMetaDatabuffer    pStatsBuffer;
extern tps_siaNVMbuffer         pNVMStatsBuffer;
extern tps_siaMetaDatabuffer    pTraceBuffer;
extern tps_siaFocusStatbuffer   pFocusStatsBuffer;
extern tps_siaMetaDatabuffer    pOutpuModeBuffer;
extern tps_siaGammaLUTbuffer    pGammaLUTBuffer;

extern ts_siaFocusStatbuffer    AfFrameStatusBuffer;
extern tps_siaFocusStatbuffer   pAfFrameStatusBuffer;

extern dictionary               *ini;

ts_bmlinfo                      BmlInfo;

unsigned long long              BMLStartTime = 0;

t_bool                          g_nvm_raw_data = 0;

volatile t_uint32               g_frameparam_counter = 0;

typedef enum
{
    ///  Disable Pipe
    PipeControl_Disable = 0,

    ///  Enable Pipe
    PipeControl_Enable  = 1,
} PipeControl_e;

/*
 * Address Block : ISP_OPF0
 */

// mle #define pictor_in_ISP_OPF0_BASE_ADDR                        (0x1060)

/*
 * Address Block : ISP_SMIARX
 */

//mle #define pictor_in_ISP_SMIARX_BASE_ADDR                      (0x500)

/*
 * Address Block : ISP_OPF0
 */

//mle #define pictor_in_ISP_OPF0_BASE_ADDR                        (0x1060)

/*
 * Address Block : ISP_OPF1
 */

//mle #define pictor_in_ISP_OPF1_BASE_ADDR                        (0x1560)
void
ITE_InitBmlInfo(void)
{
    BmlInfo.s16_CenterOffsetX = ITE_readPE(Zoom_Control_s16_CenterOffsetX_Byte0);
    BmlInfo.s16_CenterOffsetY = ITE_readPE(Zoom_Control_s16_CenterOffsetY_Byte0);
    BmlInfo.f_SetFOVX = ITE_readPE(Zoom_Control_f_SetFOVX_Byte0);
    BmlInfo.u16_MaximumUsableSensorFOVX = ITE_readPE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0);
    BmlInfo.u16_MaximumUsableSensorFOVY = ITE_readPE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0);
    BmlInfo.u16_MaxOPXOutputSize = ITE_readPE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0);
    BmlInfo.u16_MaxOPYOutputSize = ITE_readPE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0);
    BmlInfo.u16_VTXAddrMin = ITE_readPE(SensorFrameConstraints_u16_VTXAddrMin_Byte0);
    BmlInfo.u16_VTYAddrMin = ITE_readPE(SensorFrameConstraints_u16_VTYAddrMin_Byte0);
    BmlInfo.u16_VTXAddrMax = ITE_readPE(SensorFrameConstraints_u16_VTXAddrMax_Byte0);
    BmlInfo.u16_VTYAddrMax = ITE_readPE(SensorFrameConstraints_u16_VTYAddrMax_Byte0);
    BmlInfo.f_PreScaleFactor = ITE_readPE(CurrentFrameDimension_f_PreScaleFactor_Byte0);
    BmlInfo.u16_VTXAddrStart = ITE_readPE(CurrentFrameDimension_u16_VTXAddrStart_Byte0);
    BmlInfo.u16_VTYAddrStart = ITE_readPE(CurrentFrameDimension_u16_VTYAddrStart_Byte0);
    BmlInfo.u16_VTXAddrEnd = ITE_readPE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0);
    BmlInfo.u16_VTYAddrEnd = ITE_readPE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0);
    BmlInfo.u16_NumberofNonActiveLinesAtTopEdge = ITE_readPE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0);
}


void
ITE_SendBmlInfo(
enum e_bmsmode  bmsmode)
{
    ITE_writePE(Zoom_Control_s16_CenterOffsetX_Byte0, BmlInfo.s16_CenterOffsetX);
    ITE_writePE(Zoom_Control_s16_CenterOffsetY_Byte0, BmlInfo.s16_CenterOffsetY);
    if (bmsmode == BMSSTILL)
    {
        ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, BmlInfo.Previous_f_SetFOVX);
    }
    else
    {
        ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, BmlInfo.f_SetFOVX);
    }


    ITE_writePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0, BmlInfo.u16_MaximumUsableSensorFOVX);
    ITE_writePE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0, BmlInfo.u16_MaximumUsableSensorFOVY);
    ITE_writePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0, BmlInfo.u16_MaxOPXOutputSize);
    ITE_writePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0, BmlInfo.u16_MaxOPYOutputSize);
    ITE_writePE(SensorFrameConstraints_u16_VTXAddrMin_Byte0, BmlInfo.u16_VTXAddrMin);
    ITE_writePE(SensorFrameConstraints_u16_VTYAddrMin_Byte0, BmlInfo.u16_VTYAddrMin);
    ITE_writePE(SensorFrameConstraints_u16_VTXAddrMax_Byte0, BmlInfo.u16_VTXAddrMax);
    ITE_writePE(SensorFrameConstraints_u16_VTYAddrMax_Byte0, BmlInfo.u16_VTYAddrMax);
    ITE_writePE(CurrentFrameDimension_f_PreScaleFactor_Byte0, BmlInfo.f_PreScaleFactor);
    ITE_writePE(CurrentFrameDimension_u16_VTXAddrStart_Byte0, BmlInfo.u16_VTXAddrStart);
    ITE_writePE(CurrentFrameDimension_u16_VTYAddrStart_Byte0, BmlInfo.u16_VTYAddrStart);
    ITE_writePE(CurrentFrameDimension_u16_VTXAddrEnd_Byte0, BmlInfo.u16_VTXAddrEnd);
    ITE_writePE(CurrentFrameDimension_u16_VTYAddrEnd_Byte0, BmlInfo.u16_VTYAddrEnd);
    ITE_writePE(
    CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0,
    BmlInfo.u16_NumberofNonActiveLinesAtTopEdge);
}


void
ITE_StoreCurrentFOVX(void)
{
    BmlInfo.Previous_f_SetFOVX = ITE_readPE(Zoom_Control_f_SetFOVX_Byte0);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testFirmwareRevision
   PURPOSE  : Test if GrabBufferBMS[0] buffer was filled;
   ------------------------------------------------------------------------ */
void
ITE_testFirmwareRevision(void)
{
    char        mess[256];
    char        testComment[200];
    char        pathname[200];
    char        sensorName[16];
    t_uint32    pe_value;
    t_uint32    rev0,
                rev1,
                rev2,
                rev3,
                rev4,
                rev5,
                rev6,
                rev7,
                rev8;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    sprintf(testComment, "%s_firmware", sensorName);

    //perform a "cd" to destination repository for test log
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg");

    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, "test firmware revision", g_out_path);

    rev0 = ITE_readPE(DeviceParameters_u32_DeviceId_Byte0);
    rev1 = ITE_readPE(DeviceParameters_u32_FirmwareVersionMajor_Byte0);
    rev2 = ITE_readPE(DeviceParameters_u32_FirmwareVersionMinor_Byte0);
    rev3 = ITE_readPE(DeviceParameters_u32_FirmwareVersionMicro_Byte0);
    rev4 = ITE_readPE(DeviceParameters_u32_LLA_Sensor_Byte0);
    rev5 = ITE_readPE(DeviceParameters_u32_LLA_MajorVersion_Byte0);
    rev6 = ITE_readPE(DeviceParameters_u32_LLA_MinorVersion_Byte0);
    rev7 = ITE_readPE(DeviceParameters_u32_LLCD_MajorVersion_Byte0);
    rev8 = ITE_readPE(DeviceParameters_u32_LLCD_MinorVersion_Byte0);
    LOS_Log(
    "ISP retrieved version: HW %d, FW_%d.%d.%d_SENSOR_%d_LLA_%d.%d_LLCD_%d.%d\n",
    ( int ) rev0,
    ( int ) rev1,
    ( int ) rev2,
    ( int ) rev3,
    ( int ) rev4,
    ( int ) rev5,
    ( int ) rev6,
    ( int ) rev7,
    ( int ) rev8);
    snprintf(
    mess,
    sizeof(mess),
    "Test FirmVer: HW %d, FW_%d.%d.%d_SENSOR_%d_LLA_%d.%d_LLCD_%d.%d\n",
    ( int ) rev0,
    ( int ) rev1,
    ( int ) rev2,
    ( int ) rev3,
    ( int ) rev4,
    ( int ) rev5,
    ( int ) rev6,
    ( int ) rev7,
    ( int ) rev8);
    mmte_testComment(mess);
    mmte_testNext(mess);

    // check sensor
    pe_value = ITE_readPE(SensorInformation_e_Flag_Available_Byte0);
    if (pe_value == 1)
    {
        LOS_Log("Sensor is present, The details => \n", NULL, NULL, NULL, NULL, NULL, NULL);

        rev1 = ITE_readPE(SensorInformation_u16_model_id_Byte0);
        LOS_Log("ModelID               : IIII => 0x%X\n", ( void * ) rev1, NULL, NULL, NULL, NULL, NULL);

        rev2 = ITE_readPE(SensorInformation_u8_revision_number_Byte0);
        LOS_Log("Module Revision       : RR   => 0x%X\n", ( void * ) rev2, NULL, NULL, NULL, NULL, NULL);

        rev3 = ITE_readPE(SensorInformation_u8_manufacturer_id_Byte0);
        LOS_Log("Manufacturer ID       : MM   => 0x%X\n", ( void * ) rev3, NULL, NULL, NULL, NULL, NULL);

        rev4 = ITE_readPE(SensorInformation_u8_smia_version_Byte0);
        LOS_Log("SMIA Version          : SM   => 0x%X\n", ( void * ) rev4, NULL, NULL, NULL, NULL, NULL);

        snprintf(
        mess,
        sizeof(mess),
        "Sensor is present, ModelID: IIII => 0x%x\t Module Revision : RR => 0x%x\t Manufacturer ID : MM   => 0x%x\t SMIA Version: SM   => 0x%x\t",
        ( unsigned int ) rev1,
        ( unsigned int ) rev2,
        ( unsigned int ) rev3,
        ( unsigned int ) rev4);
        mmte_testComment(mess);
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("Sensor not detected\n", NULL, NULL, NULL, NULL, NULL, NULL);
        snprintf(mess, sizeof(mess), " No sensor Detected \n");
        mmte_testComment(mess);
        mmte_testNext(mess);
        mmte_testResult(TEST_FAILED);
    }


    mmte_testEnd();

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/************************************************************************/

/* ITE_InitUseCase                                                      */

/* Initialyze usecase structure with specific usecase paramaters        */

/************************************************************************/
void
ITE_InitUseCase(
char            *ap_test_id,
char            *ap_grabvpip_options,
tps_sia_usecase p_sia_usecase)
{
    Result_te err = Result_e_Success;
    char tempFVholder[32];
    UNUSED(ap_test_id);

    p_sia_usecase->sensor = ITE_GetOptionValue(ap_grabvpip_options, "SENSOR", 0);

    discoveredID = ITE_GetOptionValue(ap_grabvpip_options, "ID", 0);
    discoveredRevision = ITE_GetOptionValue(ap_grabvpip_options, "REV", 0);

    /* Update also ite variable to be independant form ite_nmf_use_case */
    ite_sensorselect = p_sia_usecase->sensor;

    ITE_GetOptionString(tempFVholder, ap_grabvpip_options, "LRout", "");
    if (strlen(tempFVholder) == 0)
    {
        strcpy(tempFVholder, "VGA");
    }

    p_sia_usecase->LR_XSize = ITE_GiveXSize(PIPE_LR, tempFVholder, "RGB565");
    p_sia_usecase->LR_YSize = ITE_GiveYSize(PIPE_LR, tempFVholder, "RGB565");
    p_sia_usecase->LR_GrbFormat = GRBFMT_R5G6B5;

    ITE_GetOptionString(tempFVholder, ap_grabvpip_options, "HRout", "");
    if (strlen(tempFVholder) == 0)
    {
        strcpy(tempFVholder, "VGA");
    }

    p_sia_usecase->HR_XSize = ITE_GiveXSize(PIPE_HR, tempFVholder, "YUV420MB_D");
    p_sia_usecase->HR_YSize = ITE_GiveYSize(PIPE_HR, tempFVholder, "YUV420MB_D");
    p_sia_usecase->HR_GrbFormat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED;

    /* Select ewarp from pc or not */
    p_sia_usecase->flag_backup_fw = ITE_IsOption(ap_grabvpip_options, "BACKUPFW");

    /* Select if agilent test mode (no sensor test mode): */
    p_sia_usecase->flag_nosensor_testmode = ITE_IsOption(ap_grabvpip_options, "NOSENSOR");

    if (p_sia_usecase->sensor == 0)
    {
        /* For Primary sensor (8Mega) */
        p_sia_usecase->BMS_XSize = 3264;
        p_sia_usecase->BMS_YSize = 2448;
    }
    else
    {
        /* For Secondary (VGA) */
        p_sia_usecase->BMS_XSize = 640;
        p_sia_usecase->BMS_YSize = 480;
    }

    p_sia_usecase->BMS_Mode = BMSSTILL;
    p_sia_usecase->BMS_output = BAYERSTORE0;
    p_sia_usecase->BMS_GrbFormat = GRBFMT_FAST_RAW8;

    p_sia_usecase->BML_GrbFormat = GRBFMT_FAST_RAW8;

    err = ITE_InitBoard();

    if(err != Result_e_Success)
    {
        LOS_Log("ITE_InitUseCase ITE_InitBoard ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }

    CLI_disp_msg(" after Init Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());

    if (usecase.flag_nosensor_testmode == 0)
    {
        ITE_autoDetectSensors();
        ITE_SetActiveSensor((p_sia_usecase->sensor) + 1);
    }

    err = ITE_switchBoardIRPI2CMode();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_InitUseCase ITE_switchBoardIRPI2CMode ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }
}

/************************************************************************************************************************/

/* ITE_Start_Env                                                    */

/* Upto Boot Cmd ( autoDetect Sensors on board / NMF network build / MCDE management / load ewarp FW + Boot Command)    */

/* WARNING: use CCP + platform parameters from testargs file                                */

/************************************************************************************************************************/
void
ITE_Start_Env(
tps_sia_usecase p_sia_usecase,
char            *ap_grabvpip_options,
int             smiapp_power_sequence,
int             sensor_tunning_before_boot)
{
    t_uint32    pe_value;
    t_uint32    pe_value2;
    UNUSED(ap_grabvpip_options);

    /* Creating txt file for dumping traces*/
#ifdef DUMP_PERF_DATA_TO_FILE
    char        tmpMsg[255];
    if (strlen(g_trace_path) == 0)
    {
        snprintf(g_trace_path, sizeof(g_trace_path), IMAGING_PATH);
    }


    snprintf(tmpMsg, 255, "%s/ite_nmf_trace.txt", g_trace_path);
    g_trace_file = LOS_fopen(tmpMsg, "w+");
    if (g_trace_file == NULL)
    {
        LOS_Log("unable to open file\n");
    }


#endif
    if (TRUE == ITE_register_sia_components())
    {
        /* Log error messages if it is not possible */
        LOS_Log("Unable to instantiate and bind grab components\n", NULL, NULL, NULL, NULL, NULL, NULL);
        exit(1);
    }


    ITE_initEvent();    /*intialize event count array to 0 before every boot*/

    // include ITE_ISP_PowerOn : binary load and boot ewarp (set PC=0)
    // Send PreBoot page element sequence to ispctl then launch a "BOOT" ewarp command through page element
    if (ITE_ISP_Boot(p_sia_usecase->sensor, p_sia_usecase->flag_backup_fw, smiapp_power_sequence, sensor_tunning_before_boot))
    {
        exit(1);
    }


    // check sensor
    pe_value = ITE_readPE(SensorInformation_e_Flag_Available_Byte0);
    if (pe_value == 1)
    {
        LOS_Log("Sensor is present, The details => \n", NULL, NULL, NULL, NULL, NULL, NULL);

        pe_value = ITE_readPE(SensorInformation_u16_model_id_Byte0);
        LOS_Log("ModelID               : IIII => 0x%X\n", ( void * ) pe_value, NULL, NULL, NULL, NULL, NULL);

        pe_value2 = ITE_readPE(SensorInformation_u8_revision_number_Byte0);
        LOS_Log("Module Revision       : RR   => 0x%X\n", ( void * ) pe_value2, NULL, NULL, NULL, NULL, NULL);

        pe_value2 = ITE_readPE(SensorInformation_u8_manufacturer_id_Byte0);
        LOS_Log("Manufacturer ID       : MM   => 0x%X\n", ( void * ) pe_value2, NULL, NULL, NULL, NULL, NULL);

        pe_value = ITE_readPE(SensorInformation_u8_smia_version_Byte0);
        LOS_Log("SMIA Version          : SM   => 0x%X\n", ( void * ) pe_value, NULL, NULL, NULL, NULL, NULL);
    }
    else
    {
        LOS_Log("Sensor not detected\n", NULL, NULL, NULL, NULL, NULL, NULL);
    }


#if defined(__PEPS8500_SIA)
#   define DS_HOSTRXMAXDATARATE    160
    ITE_postBootPE_PEPS();
    LOS_Log("Launch PEPS PostBoot\n");

#elif defined(_SVP_)
    ITE_postBootPE_TLM();
    LOS_Log("Launch TLM PostBoot\n");
#else
    ITE_postBoot();
#endif

#if FLASH_DRIVER_INCLUDE
	// ER:408207: Flash driver integration in ITE NMF. 
	ITE_FlashDriverInit();

#endif //FLASH_DRIVER_INCLUDE

}


/********************************************************************************/

/* ITE_Clear_Env                                 */

/* clears the R,G,B,S memory buffer of GLACE content.          */
/* clears the R,G,B memory buffer of HISTO content          */


/********************************************************************************/

void
ITE_Clear_Env(char *data)
{

        int size;
    if( 0 == ITE_stricmp(data, "glace"))
    {
    /*Initial Status Buffer conatins the Glace R,G,B,S Data  */

        size = g_pStatsBufferStorage->glaceStats.u32_HGridSize  * g_pStatsBufferStorage->glaceStats.u32_VGridSize * 4;
        memset ((void *) (g_pStatsBufferStorage->pGridR),0xFF, size);
        LOS_Log("Glace memory clear addr %x size %d  \n",pStatsBuffer->logAddress,size);
    }
    else if( 0 == ITE_stricmp(data, "histo"))
    {
    /* Histo array size is fixed. Histo R,G,B Data present in StatsBuffersStorage_t */

        size = SW3A_HISTO_BIN_NB * 3;
        memset ((void *) (g_pStatsBufferStorage->apHistR), 0xFF ,(size*4));
        LOS_Log("Histo memory clear addr  %x  size %d  \n",g_pStatsBufferStorage->apHistR,(size*4));
    }
    else
    {
        LOS_Log("Error in ite_clear_env parameter \n");
    }
}



/********************************************************************************/

/* ITE_Stop_Env                                 */

/* destroy and unbind SIA HW component; DeAlloc memory buffer           */

/********************************************************************************/
void
ITE_Stop_Env(void)
{
    Result_te err = Result_e_Success ;

    ITE_Freefirmware_Ext();
    /* Destroy & unbind all grab related components (grab, ispctl, sia_rm & sia_adapters)  and their interfaces */
    if (TRUE == ITE_unregister_sia_components())
    {
        /* Log error messages if it is not possible */
        LOS_Log("Unable to destroy grab network\n", NULL, NULL, NULL, NULL, NULL, NULL);

        exit(1);
    }


    STSSC_Term(0);
    STSSC_Term(1);
    STSSC_Term(2);
    STSSC_Term(3);

    //Desallocate buffers
    if (LCDBuffer.address != 0)
    {
        ITE_FreeSiaPictureBuffer(&LCDBuffer);
    }


    //WARNING: check that no pb if not been allocated previously
    if (GrabBufferLR[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[0]));
    }


    if (GrabBufferLR[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[1]));
    }


    if (GrabBufferHR[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[0]));
    }


    if (GrabBufferHR[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[1]));
    }


    if (GrabBufferBMS[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferBMS[0]));
    }


    if (GrabBufferBMS[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferBMS[1]));
    }


    if (GamBuffer.address != 0)
    {
        ITE_FreeSiaPictureBuffer(&GamBuffer);
    }


    // US: 5/20 Memory Leak
    ITE_NMF_freeSensorOutputModeBuffer(pOutpuModeBuffer);
    ITE_NMF_freeStatsBuffer(pStatsBuffer);
    ITE_NMF_freeFocusStatsBuffer(pFocusStatsBuffer);
    ITE_NMF_freeFocusStatsBuffer(pAfFrameStatusBuffer);
    ITE_NMF_freeNVMBuffer(pNVMStatsBuffer);
    ITE_NMF_freeTraceBuffer(pTraceBuffer);
    ITE_NMF_freeGammaBuffer(pGammaLUTBuffer);

    if ((ite_sensorselect == 1))
    {
        err = ITE_DeInitSecondary();
    }
    else
    {
        err = ITE_DeInitPrimary();
    }

    if (err !=Result_e_Success)
    {
        LOS_Log("ITE_Stop_Env DeInit ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }

    //#endif
    // US: 5/2 Memory Leak Fix For INI file
    if (ini != NULL)
    {
        LOS_Log("ITE_Stop_Env: Freeing the Sensor Dictionary\n");
        iniparser_freedict(ini);
    }

#if FLASH_DRIVER_INCLUDE
	// ER:408207: Flash driver integration in ITE NMF. 
	ITE_FlashDriverDeInit();

#endif //FLASH_DRIVER_INCLUDE
	ITE_ExitEvent();

    /*Closing Trace Dump File*/
#ifdef DUMP_PERF_DATA_TO_FILE
    LOS_fclose(g_trace_file);
#endif

}


/****************************************************************/

/* ITE_ConfigureLRPipe                      */

/* Configure LR pipe:  LR output pipe config            */

/*                              */

/****************************************************************/

//ported on 8500
void
ITE_ConfigureLRPipe(
tps_siapicturebuffer    p_Buffer)
{
    // Check if p_Buffer is initialized
    if (p_Buffer->address == 0)
    {
        LOS_Log(
        "\nERROR Trying to configure LR Pipe but Buffer not Declared\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }
    else
    {
        //Set sensor as datapath input, ouput setted next
#ifdef _SVP_
        ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
#else
        if (ite_sensorselect == 0)
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor0);
        }
        else
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
        }


#endif

        //configure pipe[1] PE Group
        ITE_InitializeOutputPipe(
        GRBPID_PIPE_LR,
        p_Buffer->PipeOutputformat,
        p_Buffer->framesizeX,
        p_Buffer->framesizeY);

        ITE_SendOutputPipeConfig(GRBPID_PIPE_LR);

        // prepare Grab task
        ITE_setGrabParams(
        &grabparamsLR,
        p_Buffer->Grb_colorformat,
        p_Buffer->framesizeX,
        p_Buffer->framesizeY,
        p_Buffer->framesizeX,
        0,
        p_Buffer->address,
        (t_bool) TRUE,
        0);

    }
}


/********************************************************/

/* ITE_CheckStillPreviewUseCase             */

/* Check if Usecase format and size output pipe correct */

/********************************************************/
int
ITE_CheckStillPreviewUseCase(
tps_sia_usecase p_sia_usecase)
{
    int error = 0;

    switch (p_sia_usecase->LR_GrbFormat)
    {
        // Check Output pipe format (according to PAS Doc)
        // Allowed
        case (GRBFMT_YUV422_RASTER_INTERLEAVED):
        case (GRBFMT_A4R4G4B4):
        case (GRBFMT_A1R5G5B5):
        case (GRBFMT_R5G6B5):
        case (GRBFMT_R8G8B8):
        case (GRBFMT_A8R8G8B8):
            error = 0;
            break;

        // Not Allowed
        case (GRBFMT_YUV420_RASTER_PLANAR_I420):
        case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
        case (GRBFMT_YUV422_MB_SEMIPLANAR):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
        case (GRBFMT_RGB30):    // ????
        case (GRBFMT_RAW8):     // ????
        case (GRBFMT_RAW12):    // ????
        default:                // __NO_WARNING__
            error = 1;
            break;
    }


    if (error)
    {
        LOS_Log("\nERROR output LR Pipe Format not Correct \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    // Check LR Output pipe Size up to XGA (according to PAS Doc)
    if ((p_sia_usecase->LR_XSize) > 1024)
    {
        error = 1;
    }
    else
    {
        error = 0;
    }


    if (error)
    {
        LOS_Log("\nERROR output LR Pipe XSize Bigger than XGA \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    if ((p_sia_usecase->LR_YSize) > 768)
    {
        error = 1;
    }
    else
    {
        error = 0;
    }


    if (error)
    {
        LOS_Log("\nERROR output LR Pipe YSize Bigger than XGA \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    return (error);
}


/********************************************************/

/* ITE_StillPreview_Prepare             */

/* Initialized BufferLR Structure; Configure PipeLR */

/********************************************************/
void
ITE_StillPreview_Prepare(
tps_sia_usecase p_sia_usecase)
{
    ITE_CheckStillPreviewUseCase(p_sia_usecase);    // check if format and size are correct according to Architecture doc

    // Warning je ne sais pas encore comment je gere le fait que le Buffer de sortie du pipeLR
    // soit le meme que le LCD buffer
    ITE_createSiaPictureBuffer(
    &(GrabBufferLR[0]),
    p_sia_usecase->LR_XSize,
    p_sia_usecase->LR_YSize,
    p_sia_usecase->LR_GrbFormat,
    BUF_LR);

    //GrabBufferLR[0] = LCDBuffer;
    ITE_ConfigureLRPipe(&(GrabBufferLR[0]));
    p_sia_usecase->mode = STILLPREVIEW;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************/

/* ITE_StillPreview_Start           */

/* launch Grab Task; Enable PipeLR; START VPIP  */

/************************************************/
void
ITE_StillPreview_Start(
t_uint32    nbFrames)
{
    //tps_siapicturebuffer p_siapictbuffer;
    g_grabHR_infinite = nbFrames;

    grabparamsHR.output_format = GRBFMT_R8G8B8;

    //FIXME: windowing not yet managed
    grabparamsHR.x_window_size = 640;
    grabparamsHR.y_window_size = 480 - 2;
    grabparamsHR.x_window_offset = 0;
    grabparamsHR.y_window_offset = 0;
    grabparamsHR.x_frame_size = 640;
    grabparamsHR.y_frame_size = 480 - 2;
    grabparamsHR.dest_buf_addr = 0x01000000;    //destBufferAddr;
    grabparamsHR.buf_id = 0;
    grabparamsHR.disable_grab_cache = (t_bool) TRUE;
    grabparamsHR.cache_buf_addr = 0;

    //FIXME: stab not yet managed
    grabparamsHR.enable_stab = 0;

    //grabparamsHR.hr_enabled=0;
    //mle pGrabparams->stab_buf_addr=0;
    grabparamsHR.proj_h_buf_addr = 0;
    grabparamsHR.proj_v_buf_addr = 0;
    grabparamsHR.lr_alpha = 0;

    NMFCALL (grabCommand, execute) (GRBPID_PIPE_HR, grabparamsHR, GRBPID_PIPE_HR);

}


/************************************************************************/

/* ITE_StillPreview_Stop                        */

/* Stop Grab Task; Wait for completion; STOP VPIP; Disable PipeLR   */

/************************************************************************/
void
ITE_StillPreview_Stop(void)
{
    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    usecase.state = STATE_PREPARED;
}


/************************************************/

/* ITE_StillPreview_Free            */

/* DeAllocation of GrabBuffer_LR[]      */

/************************************************/
void
ITE_StillPreview_Free(void)
{
    // Warning GrabBuffer[0] is not allocated in memory but it is LCDBuffer
    //GrabBufferLR[0].address = NULL;
    //GrabBufferLR[1].address = NULL;
    if (GrabBufferLR[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[0]));
    }


    if (GrabBufferLR[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[1]));
    }


    usecase.mode = MODE_UNDEFINED;
    usecase.state = STATE_FREE;
}


/****************************************************************/

/* ITE_ConfigureHRPipe                      */

/* Configure HR pipe:  HR output pipe config            */

/****************************************************************/

//ported on 8500 V1
void
ITE_ConfigureHRPipe(
tps_siapicturebuffer    p_Buffer)
{
    t_cm_memory_handle  esramGrabCache;
    t_uint32            grabCacheWordSize = 0;
    t_cm_system_address GrabCacheAddr;
    t_cm_error          cmError;

    // Check if p_Buffer is initialized
    if (p_Buffer->address == 0)
    {
        LOS_Log(
        "\nERROR Trying to configure LR Pipe but Buffer not Declared\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }
    else
    {
        //Set sensor as datapath input, ouput setted next
#ifdef _SVP_
        ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
#else
        if (ite_sensorselect == 0)
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor0);
        }
        else
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
        }


#endif

        //configure pipe[0] PE Group
        ITE_InitializeOutputPipe(
        GRBPID_PIPE_HR,
        p_Buffer->PipeOutputformat,
        p_Buffer->framesizeX,
        p_Buffer->framesizeY);
        ITE_SendOutputPipeConfig(GRBPID_PIPE_HR);

        if (usecase.esramGrabCacheMemHandle != 0)
        {
            t_cm_error  cmError;

            cmError = CM_FreeMpcMemory(usecase.esramGrabCacheMemHandle);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR freeing grab cache buffer !!! \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }


            usecase.esramGrabCacheMemHandle = 0;
        }


        //enable use of grab cache buffer: for HR/MB format only
        //FIXME: use of grab cache: to be try after
        if
        (
            (p_Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED)
        ||  (p_Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED)
        ||  (p_Buffer->Grb_colorformat == GRBFMT_YUV422_MB_SEMIPLANAR)
        )
        {
            // t_cm_domain_memory mydomain;
            //  grabparamsHR.disable_grab_cache=FALSE;
            grabCacheWordSize = (p_Buffer->framesizeX * 2 * 16 * 2) / 2;    //2 * width of MB * octect nb per pixel (1.5 for YUV420 / 2 for YUV422)
            cmError = CM_AllocMpcMemory(
                domainId,
                CM_MM_MPC_ESRAM16,
                grabCacheWordSize,
                CM_MM_ALIGN_2WORDS,
                &esramGrabCache);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR allocating grab cache buffer\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }


            cmError = CM_GetMpcMemorySystemAddress(esramGrabCache, &GrabCacheAddr);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR retrieving grab cache buffer address\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }


            //   grabparamsHR.cache_buf_addr=GrabCacheAddr.physical;
            if (cmError == CM_OK)
            {
                usecase.esramGrabCacheMemHandle = esramGrabCache;
            }
            else
            {
                usecase.esramGrabCacheMemHandle = 0;
            }
        }


        // prepare Grab task
        if (usecase.esramGrabCacheMemHandle == 0)
        {
            ITE_setGrabParams(
            &grabparamsHR,
            p_Buffer->Grb_colorformat,
            p_Buffer->framesizeX,
            p_Buffer->framesizeY,
            p_Buffer->framesizeX,
            0,
            p_Buffer->address,
            (t_bool) TRUE,
            0);
        }
        else
        {
            ITE_setGrabParams(
            &grabparamsHR,
            p_Buffer->Grb_colorformat,
            p_Buffer->framesizeX,
            p_Buffer->framesizeY,
            p_Buffer->framesizeX,
            0,
            p_Buffer->address,
            (t_bool) FALSE,
            GrabCacheAddr.physical);
        }


        //grabparamsHR.cache_buf_addr);
    }
}

/****************************************************************/

/* ITE_ConfigureHRPipe   for Stripe                   */

/* Configure HR pipe:  HR output pipe config            */

/****************************************************************/

//ported on 8500 V1
void
ITE_ConfigureHRPipe_ForStripe(
tps_siapicturebuffer    p_Buffer,t_uint32     U8_StripeCount, t_uint32    stripeIndex)
{
    t_cm_memory_handle  esramGrabCache;
    t_uint32            grabCacheWordSize = 0;
    t_cm_system_address GrabCacheAddr;
    t_cm_error          cmError;
    t_uint32  xwindowSize, xwindowoffset;

    UNUSED(stripeIndex);

    // Check if p_Buffer is initialized
        if (p_Buffer->address == 0)
        {
            LOS_Log(
            "\nERROR Trying to configure LR Pipe but Buffer not Declared\n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
        }
        else
        {
            if(usecase.stripeIndex == 0)
            {
        //Set sensor as datapath input, ouput setted next
#ifdef _SVP_
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
#else
        if (ite_sensorselect == 0)
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor0);
        }
        else
        {
            ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
        }
#endif
        }
        //configure pipe[0] PE Group
            ITE_InitializeOutputPipe(
            GRBPID_PIPE_HR,
            p_Buffer->PipeOutputformat,
            p_Buffer->framesizeX,
            p_Buffer->framesizeY);
            ITE_SendOutputPipeConfig(GRBPID_PIPE_HR);
        if (usecase.esramGrabCacheMemHandle != 0)
        {
            t_cm_error  cmError;
            cmError = CM_FreeMpcMemory(usecase.esramGrabCacheMemHandle);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR freeing grab cache buffer !!! \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }
            usecase.esramGrabCacheMemHandle = 0;
        }
        //enable use of grab cache buffer: for HR/MB format only
        //FIXME: use of grab cache: to be try after
        if
        (
            (p_Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED)
        ||  (p_Buffer->Grb_colorformat == GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED)
        ||  (p_Buffer->Grb_colorformat == GRBFMT_YUV422_MB_SEMIPLANAR)
        )
        {
            // t_cm_domain_memory mydomain;
            grabCacheWordSize = (p_Buffer->framesizeX * 2 * 16 * 2) / 2;    //2 * width of MB * octect nb per pixel (1.5 for YUV420 / 2 for YUV422)
            cmError = CM_AllocMpcMemory(
            domainId,
            CM_MM_MPC_ESRAM16,
            grabCacheWordSize,
            CM_MM_ALIGN_2WORDS,
            &esramGrabCache);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR allocating grab cache buffer\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }
            cmError = CM_GetMpcMemorySystemAddress(esramGrabCache, &GrabCacheAddr);
            if (cmError != CM_OK)
            {
                LOS_Log("\nERROR retrieving grab cache buffer address\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            }
            //   grabparamsHR.cache_buf_addr=GrabCacheAddr.physical;
            if (cmError == CM_OK)
            {
                usecase.esramGrabCacheMemHandle = esramGrabCache;
            }
            else
            {
                usecase.esramGrabCacheMemHandle = 0;
            }
        }
        if( U8_StripeCount > 0 )
        {
            xwindowSize = p_Buffer->framesizeX/ U8_StripeCount;
            if(usecase.stripeIndex == 0)
                xwindowoffset = 0;
            else
                xwindowoffset = p_Buffer->framesizeX/ U8_StripeCount;
        }
        else
        {
            xwindowSize = p_Buffer->framesizeX;
            xwindowoffset = 0;
        }
        if (usecase.esramGrabCacheMemHandle == 0)
        {

            ITE_setGrabParams(
            &grabparamsHR,
            p_Buffer->Grb_colorformat,
            p_Buffer->framesizeX,
            p_Buffer->framesizeY,
            xwindowSize,
            xwindowoffset,
            p_Buffer->address,
            (t_bool) TRUE,
            0);
        }
        else
        {

            ITE_setGrabParams(
            &grabparamsHR,
            p_Buffer->Grb_colorformat,
            p_Buffer->framesizeX,
            p_Buffer->framesizeY,
            xwindowSize,
            xwindowoffset,
            p_Buffer->address,
            (t_bool) FALSE,
            GrabCacheAddr.physical);
        }
    }
}

/********************************************************/

/* ITE_CheckStillUseCase                */

/* Check if Usecase format and size output pipe correct */

/********************************************************/
int
ITE_CheckStillUseCase(
tps_sia_usecase p_sia_usecase)
{
    int error = 0;
    switch (p_sia_usecase->HR_GrbFormat)
    {
        // Check Output pipe format (according to PAS Doc)
        // Allowed
        case (GRBFMT_YUV422_RASTER_INTERLEAVED):
        case (GRBFMT_YUV420_RASTER_PLANAR_I420):
        case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
        case (GRBFMT_YUV422_MB_SEMIPLANAR):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
        case (GRBFMT_RGB30):    // not sure for following!
        case (GRBFMT_RAW8):
        case (GRBFMT_RAW12):
            error = 0;
            break;

        // Not Allowed
        case (GRBFMT_A4R4G4B4):
        case (GRBFMT_A1R5G5B5):
        case (GRBFMT_R5G6B5):
        case (GRBFMT_R8G8B8):
        case (GRBFMT_A8R8G8B8):
        default:                // __NO_WARNING__
            error = 1;
            break;
    }


    if (error)
    {
        LOS_Log("\nERROR output HR Pipe Format not Correct \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    // WARNING ACTUALLY SET FOR 3 MegaPel sensor 850
    // Check HR Output pipe Size up to 12.5(according to PAS Doc)
    if ((p_sia_usecase->HR_XSize) > 2048)
    {
        error = 1;
    }
    else
    {
        error = 0;
    }


    if (error)
    {
        LOS_Log("\nERROR output HR Pipe XSize Bigger than 12.5M \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    if ((p_sia_usecase->HR_YSize) > 1536)
    {
        error = 1;
    }
    else
    {
        error = 0;
    }


    if (error)
    {
        LOS_Log("\nERROR output HR Pipe YSize Bigger than 12.5M \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    return (error);
}


/********************************************************/

/* ITE_Still_Prepare                    */

/* Initialized BufferHR Structure; Configure PipeHR */

/********************************************************/
void
ITE_Still_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    UNUSED(nbFrames);
    
    ITE_CheckStillUseCase(p_sia_usecase);   // check if format and size are correct according to Architecture doc
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);
    ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
    p_sia_usecase->mode = STILL;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************************************/

/* ITE_Still_Start                          */

/* must Stop Grab and PipeLR                        */

/* launch Grab HR And LR Task; Enable PipeHR and PipeLR; START VPIP */

/************************************************************************/
int
ITE_Still_Start(
t_uint32    nbFrames)
{
    UNUSED(nbFrames);
    return (1);

    /*
            int error =0;

            if(ITE_IsGrabRunning(GRBPID_PIPE_LR)) {
            ITE_StopGrab(GRBPID_PIPE_LR);
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);}
            if(ITE_IsVPIPRunning())
            STOPVPIP();

            //ITE_InitDatapath(InputImageSource_Sensor, OutputImageDestination_PixelPipes);
            //ITE_SendDatapath();
            // Launch Grab task: Warning: launch Grab HR before LR as grabctl launch HR when a previous LR is available
            ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
            ITE_StartGrabNb(GRBPID_PIPE_HR,nbFrames);
            if (GrabBufferLR[0].address != NULL) {
            ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
            ITE_StartGrabNb(GRBPID_PIPE_LR,nbFrames);
            }
            ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
            STARTVPIP();
            ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
            usecase.state = STATE_RUNNING;
            //error = ITE_WaitEventTimeout(ITE_EVT_GRABHR_EVENTS, 2000);
            ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
            if (GrabBufferLR[0].address != NULL)
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);


            return(error);
          */
}


/****************************************/

/* ITE_Still_Stop           */

/*                  */

/****************************************/
void
ITE_Still_Stop(void)
{
    STOPVPIP();
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    usecase.state = STATE_PREPARED;
}


/************************************************/

/* ITE_Still_Free               */

/* DeAllocation of GrabBuffer_HR[]      */

/************************************************/
void
ITE_Still_Free(void)
{
    if (GrabBufferHR[0].address != 0)
    {
        // if GrabBufferHR[0] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[0].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[0]));
    }


    if (GrabBufferHR[1].address != 0)
    {
        // if GrabBufferHR[1] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[1].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[1]));
    }


    usecase.mode = MODE_UNDEFINED;
    usecase.state = STATE_FREE;
}


/****************************************************************/

/* ITE_ConfigureBMSPipe                     */

/* Configure BMS pipe:  BMS output pipe config          */

/****************************************************************/

//ported on 8500
void
ITE_ConfigureBMSPipe(
tps_siapicturebuffer    p_Buffer,
t_uint32                nbFrames)
{
 UNUSED(nbFrames);
    // Check if p_Buffer is initialized
    if (p_Buffer->address == 0)
    {
        LOS_Log(
        "\nERROR Trying to configure BMS Pipe but Buffer not Declared\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }
    else
    {
#ifdef _SVP_
        ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
#else
        if (usecase.BMS_Mode == BMSSTILL)   //sudeep added condition, to avoid execution in case of BMS VIDEO
        {
            //Set sensor0 as datapath input, ouput setted next
            if (ite_sensorselect == 0)
            {
                ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor0);
            }
            else
            {
                ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Sensor1);
            }
        }


#endif

        //no specific PE to configure BMS: size is full size + choicee between raw8 and raw12 setted by grab call (handle by DSP code)
        // prepare Grab task
        ITE_setGrabParams(
        &grabparamsBMS,
        p_Buffer->Grb_colorformat,
        usecase.BMS_XSize,                  //p_Buffer->framesizeX,
        p_Buffer->framesizeY,
        usecase.BMS_XSize,
        0,
        p_Buffer->address,
        (t_bool) TRUE,
        0);

        //Activate fast BMS (1st stage of BMS)
        switch (usecase.BMS_output)
        {
            case (BAYERSTORE0):
                ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, Flag_e_TRUE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, Flag_e_FALSE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, Flag_e_FALSE);
                break;

            case (BAYERSTORE1):
                ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, Flag_e_FALSE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, Flag_e_TRUE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, Flag_e_FALSE);
                break;

            case (BAYERSTORE2):
                ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, Flag_e_FALSE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, Flag_e_FALSE);
                ITE_writePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, Flag_e_TRUE);
                break;

            default:
                break;                      // __NO_WARNING__
        }


        if (usecase.BMS_Mode == BMSSTILL)
        {
            //FIXME
            //PATCH: one another pipe should be enabled for BMS to work properly
            //ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
            ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
            ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
        }
    }
}


/********************************************************/

/* ITE_CheckBMSUseCase                  */

/* Check if Usecase format and size output pipe correct */

/********************************************************/
int
ITE_CheckBMSUseCase(
tps_sia_usecase p_sia_usecase)
{
    int error = 0;
    switch (p_sia_usecase->BMS_GrbFormat)
    {
        // Check Output pipe format (according to PAS Doc)
        // Allowed
        case (GRBFMT_RAW8):
        case (GRBFMT_FAST_RAW8):
        case (GRBFMT_RAW12):
        case (GRBFMT_FAST_RAW12):
            error = 0;
            break;

        // Not Allowed
        case (GRBFMT_YUV422_RASTER_INTERLEAVED):
        case (GRBFMT_YUV420_RASTER_PLANAR_I420):
        case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
        case (GRBFMT_YUV422_MB_SEMIPLANAR):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
        case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
        case (GRBFMT_RGB30):
        case (GRBFMT_A4R4G4B4):
        case (GRBFMT_A1R5G5B5):
        case (GRBFMT_R5G6B5):
        case (GRBFMT_R8G8B8):
        case (GRBFMT_A8R8G8B8):
        default:
            error = 1;
            break;  // __NO_WARNING__
    }


    if (error)
    {
        LOS_Log("\nERROR output BMS Pipe Format not Correct \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return (error);
    }


    return (error);
}


/********************************************************/

/* ITE_BMS_Prepare                  */

/* Initialized BufferBMS Structure; Configure PipeBMS   */

/********************************************************/

//ported on 8500
void
ITE_BMS_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    ITE_CheckBMSUseCase(p_sia_usecase); // check if format is correct according to Architecture doc
    if (p_sia_usecase->BMS_Mode == BMSSTILL)
    {
        ITE_writePE(Pipe_RAW_u16_output_res_X_size_Byte0, p_sia_usecase->BMS_XSize);
        ITE_writePE(Pipe_RAW_u16_output_res_Y_size_Byte0, p_sia_usecase->BMS_YSize);

        ITE_writePE(Pipe_RAW_u16_woi_res_X_Byte0, p_sia_usecase->BMS_woi_X);
        ITE_writePE(Pipe_RAW_u16_woi_res_Y_Byte0, p_sia_usecase->BMS_woi_Y);

        ITE_writePE(VideoTimingHostInputs_u16_CsiRawFormat_Byte0, p_sia_usecase->data_format);
    }


    if (p_sia_usecase->BMS_Mode == BMSVIDEO)
    {                           // in this case BMS done with current Output sensor
        p_sia_usecase->BMS_XSize = ITE_readPE(CurrentFrameDimension_u16_OPXOutputSize_Byte0);
        p_sia_usecase->BMS_YSize = ITE_readPE(CurrentFrameDimension_u16_OPYOutputSize_Byte0);
    }


    ITE_createSiaPictureBuffer(
    &(GrabBufferBMS[0]),
    p_sia_usecase->BMS_XSize,   // alignement done into createSiaBuffer
    p_sia_usecase->BMS_YSize + ITE_readPE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0),
    p_sia_usecase->BMS_GrbFormat,
    BUF_BMS);
    GrabBufferBMS[0].nonactivelines = ITE_readPE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0);
    ITE_ConfigureBMSPipe(&(GrabBufferBMS[0]), nbFrames);
    p_sia_usecase->mode = BMS;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************************************/

/* ITE_BMS_Start                            */

/* must Stop Grab and PipeLR                        */

/* launch Grab BMS Task; Enable PipeBMS; START VPIP         */

/************************************************************************/

//ported on 8500
int
ITE_BMS_Start(
t_uint32    nbFrames)
{
    int                 error = 0;
    t_uint32            RunError;
    t_uint32            maxprescale;
#if STORE_GLACE_STATS_AND_BITMAP
    char                BitmapPathString[100];
    char                BitmapFileName[10];
#endif

#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("START", "BMS DATAPATH", "START_BMS_PIPE", MAX_BMS_START_TIME);
#endif
    if (usecase.BMS_Mode == BMSVIDEO)
    {               // VIDEO BMS mode; assuming Pipe are streaming
        if (ITE_IsVPIPStopped())
        {
            LOS_Log("Error BMS video cannot run, because pipe is stopped\n");
            error = 1;
        }
        else
        {
            // Launch Grab task:
            ITE_StartGrabNb(GRBPID_PIPE_RAW_OUT, nbFrames);

            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);  //[sudeep] waiting for GRBINF_GRAB_LAUNCHED
            if (usecase.BMS_output == BAYERSTORE2)
            {       // Stats request are mandatory if BAYERSTORE2
                if (pStatsBuffer->handle == 0)
                {   // if stat buffer doesn't exist, creat some dummy stats
                    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
                    ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
                    ITE_NMF_SetGlaceGrid(72, 54);
                    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

                    //ITE_NMF_createStatsBuffer(pStatsBuffer);
                    ITE_NMF_UpdateGlaceParam();
                    ITE_NMF_UpdateHistoParam();
                }


                ITE_NMF_GlaceHistoExpStatsRequestAndWait();
            }


            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);
            usecase.state = STATE_RUNNING;
            ITE_InitBmlInfo();
        }


        return (error);
    }
    else
    {               // STILL BMS mode;
        if (ITE_IsGrabRunning(GRBPID_PIPE_RAW_OUT))
        {
            ITE_StopGrab(GRBPID_PIPE_RAW_OUT);
            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);
        }


        if (ITE_IsVPIPRunning())
        {
            STOPVPIP();
        }


        if (usecase.BMS_output == BAYERSTORE2)
        {           // Stats request are mandatory if BAYERSTORE2
            if (pStatsBuffer->handle == 0)
            {       // if stat buffer doesn't exist, creat some dummy stats
                ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetGlaceGrid(72, 54);
                ITE_NMF_SetGlaceSatLevel(255, 255, 255);

                //ITE_NMF_createStatsBuffer(pStatsBuffer);
                ITE_NMF_UpdateGlaceParam();
                ITE_NMF_UpdateHistoParam();
            }

                ITE_NMF_GlaceAndHistoExpReqNoWait();
        }

        maxprescale = ITE_readPE(HostFrameConstraints_u8_MaximumPreScale_Byte0);
        ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, 1);  // No Prescale


        ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);

        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

        ITE_StartGrabNb(GRBPID_PIPE_RAW_OUT, nbFrames);
        ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);

        RunError = STARTVPIP();
        if (RunError != 0)
        {
            LOS_Log("\n ISPCTL ERROR!!\n");
            ITE_NMF_ASSERT(RunError);
        }

        ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);                          //comment this line for flash test
        usecase.state = STATE_RUNNING;
        ITE_InitBmlInfo();

        ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, maxprescale);

        if (usecase.BMS_output == BAYERSTORE2)
        {
#if 1
            LOS_Log("\nChecking glace event.. \n");
            ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
            error = ITE_GetEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
            LOS_Log("1 glace event(s) received.\n");

            // Get_Glace_Geometry();

#   if STORE_GLACE_STATS_AND_BITMAP
            // dump glace stats
            ITE_NMF_DumpGlaceBuffer();

            // dump frame
            strcpy(BitmapFileName, "BMS");

            //ITE_itoa(bms_counter, BitmapFileName+3, 10);   // leave 3 bytes for string BMS
            snprintf(BitmapPathString, sizeof(BitmapPathString), IMAGING_PATH "/bmp_%s", BitmapFileName);
            ITE_StoreinBMPFile(BitmapPathString, &(GrabBufferBMS[0]));
#   endif
#else
            ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
            if (newstatbuff == 1)
            {
                //Sudeep: commented as allocation is done only once at (post) BOOT time
                //TODO: find a suitable place to move this code.
#   if 0
                ITE_NMF_freeStatsBuffer(pStatsBuffer);
                ITE_NMF_ResetISPBufferPointers();
#   endif
            }


            newstatbuff = 0;
#endif
        }


        return (error);
    }


#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("STOP", "BMS DATAPATH", "START_BMS_PIPE", MAX_BMS_START_TIME);
#endif
}


/****************************************/

/* ITE_BMS_Stop             */

/*                  */

/****************************************/

//ported on 8500
void
ITE_BMS_Stop(void)
{
  if(ITE_IsVPIPRunning())
    {   
        // Nothing to do because we wait for end of grab into ITE_BMS_Start function
        // if(ITE_IsGrabRunning(GRBPID_PIPE_RAW_OUT)) {
        //    ITE_StopGrab(GRBPID_PIPE_RAW_OUT);
        //    ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);}
        // ITE_StopGrab(GRBPID_PIPE_RAW_OUT);
#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("START", "BMS DATAPATH", "STOP_BMS_PIPE", MAX_BMS_STOP_TIME);
#endif
        if (usecase.BMS_Mode == BMSSTILL)
        {
            STOPVPIP();
        }


        usecase.state = STATE_PREPARED;
        ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, Flag_e_FALSE);
        ITE_writePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, Flag_e_FALSE);
        ITE_writePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, Flag_e_FALSE);

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "BMS DATAPATH", "STOP_BMS_PIPE", MAX_BMS_STOP_TIME);
#endif
    }
    else
    {
         LOS_Log("BMS stopped\n");
    }
}


/************************************************/

/* ITE_BMS_Free                 */

/* DeAllocation of GrabBuffer_BMS[]     */

/************************************************/

//ported on 8500
void
ITE_BMS_Free(void)
{
    if (GrabBufferBMS[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferBMS[0]));
    }


    if (GrabBufferBMS[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferBMS[1]));
    }


    usecase.mode = MODE_UNDEFINED;
    usecase.state = STATE_FREE;
}


/****************************************/

/* ITE_BMS_Abort            */

/*                  */

/****************************************/
void
ITE_BMS_Abort(void)
{
    LOS_Log("\n>> ITE_BMS_Abort\n");

    NMFCALL (grabCommand, abort) (GRBPID_PIPE_RAW_OUT, GRBPID_PIPE_RAW_OUT);
    LOS_Log("\n<< ITE_BMS_Abort\n");
}


/************************************************************************/

/* ITE_BMS_Start_params                          */

/* must Stop Grab and PipeLR                        */

/* launch Grab BMS Task; Enable PipeBMS; START VPIP         */

/* This is different form ITE_BMS_START here we are not toggling coin as it is being toggled by verify_params_absorbed*/

/************************************************************************/

//ported on 8500
int
ITE_BMS_Start_Extention(
t_uint32 nbFrames , char* wait_param,char* toggle_param)
{
    int                 error = 0;
    t_uint32            RunError;
    t_uint32            maxprescale;
#if STORE_GLACE_STATS_AND_BITMAP
    char                BitmapPathString[100];
    char                BitmapFileName[10];
#endif

    if (usecase.BMS_Mode == BMSVIDEO)
    {               // VIDEO BMS mode; assuming Pipe are streaming
        if (ITE_IsVPIPStopped())
        {
            LOS_Log("Error BMS video cannot run, because pipe is stopped\n");
            error = 1;
        }
        else
        {
            // Launch Grab task:
            ITE_StartGrabNb(GRBPID_PIPE_RAW_OUT, nbFrames);

            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);  //[sudeep] waiting for GRBINF_GRAB_LAUNCHED
            if (usecase.BMS_output == BAYERSTORE2)
            {       // Stats request are mandatory if BAYERSTORE2
                if (pStatsBuffer->handle == 0)
                {   // if stat buffer doesn't exist, creat some dummy stats
                    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
                    ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
                    ITE_NMF_SetGlaceGrid(72, 54);
                    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

                    //ITE_NMF_createStatsBuffer(pStatsBuffer);
                    ITE_NMF_UpdateGlaceParam();
                    ITE_NMF_UpdateHistoParam();
                }

                if( 0 == ITE_stricmp(toggle_param, "toggle"))
                {
                       ITE_NMF_GlaceAndHistoExpReqNoWait();
                       LOS_Log("Toggled System Coin !!\n");
                }
               else
                {
                       LOS_Log("system coin toggling SKIPPED !!\n");
                }
            }


            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);
            usecase.state = STATE_RUNNING;
            ITE_InitBmlInfo();
        }


        return (error);
    }
    else
    {               // STILL BMS mode;
        if (ITE_IsGrabRunning(GRBPID_PIPE_RAW_OUT))
        {
            ITE_StopGrab(GRBPID_PIPE_RAW_OUT);
            ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);
        }


        if (ITE_IsVPIPRunning())
        {
            STOPVPIP();
        }


        if (usecase.BMS_output == BAYERSTORE2)
        {           // Stats request are mandatory if BAYERSTORE2
            if (pStatsBuffer->handle == 0)
            {       // if stat buffer doesn't exist, creat some dummy stats
                ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetGlaceGrid(72, 54);
                ITE_NMF_SetGlaceSatLevel(255, 255, 255);

                //ITE_NMF_createStatsBuffer(pStatsBuffer);
                ITE_NMF_UpdateGlaceParam();
                ITE_NMF_UpdateHistoParam();
            }

            if( 0 == ITE_stricmp(toggle_param,"toggle"))
                {
                       ITE_NMF_GlaceAndHistoExpReqNoWait();
                       LOS_Log("Toggled System Coin !!\n");
                }
            else
                {
                       LOS_Log("system coin toggling SKIPPED !!\n");
                }
        }

        maxprescale = ITE_readPE(HostFrameConstraints_u8_MaximumPreScale_Byte0);
        ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, 1);  // No Prescale

        ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);

        ITE_StartGrabNb(GRBPID_PIPE_RAW_OUT, nbFrames);

        ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);

        RunError = STARTVPIP();
        if (RunError != 0)
        {
            LOS_Log("\n ISPCTL ERROR!!\n");
            ITE_NMF_ASSERT(RunError);
        }

        ITE_WaitEvent(ITE_EVT_GRABBMS_EVENTS);  //why twice?                          //comment this line for flash test
        usecase.state = STATE_RUNNING;

        ITE_InitBmlInfo(); //why?

        ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, maxprescale);

        if (usecase.BMS_output == BAYERSTORE2)
        {
             if ( 0 == ITE_stricmp(wait_param,"wait"))
              {
                LOS_Log("\nChecking glace event.. \n");
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
                LOS_Log("1 glace event(s) received.\n");
              }
             else
              {
                LOS_Log("GLACE EVENT WAIT SKIPPED!!\n");
              }
        }
        
        return (error);
    }

}

/********************************************************/

/* ITE_BML_Prepare                  */

/* Initialized BufferHR Structure; Configure PipeHR */

/********************************************************/
void
ITE_BML_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    t_uint16    sizey;
    UNUSED(nbFrames);

    // ITE_CheckStillUseCase(p_sia_usecase); // check if format and size are correct according to Architecture doc
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);
    ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
    sizey = GrabBufferBMS[0].framesizeY - (BmlInfo.u16_NumberofNonActiveLinesAtTopEdge);
    ITE_setGrabParams(
    &grabparamsBMS,
    p_sia_usecase->BML_GrbFormat,
    usecase.BMS_XSize,                              //GrabBufferBMS[0].framesizeX,
    sizey,
    usecase.BMS_XSize,
    0,
    GrabBufferBMS[0].address,
    (t_bool) TRUE,
    0);

    p_sia_usecase->mode = BML;
    p_sia_usecase->state = STATE_PREPARED;
}

/********************************************************/

/* ITE_BML_stripe_Prepare                  */

/* Initialized BufferHR Structure; Configure PipeHR */

/********************************************************/
void
ITE_BML_stripe_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    t_uint16    sizey;
    UNUSED(nbFrames);
    UNUSED(sizey);

    // ITE_CheckStillUseCase(p_sia_usecase); // check if format and size are correct according to Architecture doc
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);


}

/************************************************************************/

/* ITE_BML_Start                            */

/* must Stop Grab and PipeLR                        */

/* launch Grab HR And LR Task; Enable PipeHR and PipeLR; START VPIP */

/************************************************************************/

// Be Carefull should be remove later
#define LINETYPE_SOF            0x1
#define LINETYPE_EOF            0x2
#define LINETYPE_BLANK          0x3
#define LINETYPE_BLACK          0x4
#define LINETYPE_DARK           0x8
#define LINETYPE_ACTIVE         0xD
#define LINETYPE_ACTIVE_LAST    0xE
#define LINETYPE_ACTIVE_SOF     (LINETYPE_ACTIVE | LINETYPE_SOF)

int
ITE_BML_Start(
t_uint32    nbFrames)
{
    int         streaming_error;
    t_uint16    tmp;
#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("START", "BML DATAPATH", "START_BML_PIPE", MAX_BML_START_TIME);
#endif
    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_StopGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        if((InputImageSource_e_BayerLoad1 == ITE_readPE(SystemSetup_e_InputImageSource_Byte0)) || ((InputImageSource_e_BayerLoad2 == ITE_readPE(SystemSetup_e_InputImageSource_Byte0))))
        {
            STOPVPIP_BML();
        }
	else
	{
            STOPVPIP();
        }
    }


    switch (usecase.BML_input)
    {
        case (BAYERLOAD1):  ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_BayerLoad1); break;
        case (BAYERLOAD2):  ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_BayerLoad2); break;
        case (RGBLOAD):     ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_RGBLoad); break;
        default:            LOS_Log_Err("usecase.BML_input case didn't match, BAYERSTORE0 etc\n"); break;
    }


    //ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, 0);
    //ITE_writePE(SensorFrameConstraints_u16_MaxVTFrameLengthLines_Byte0, 0x1000); //????
    ITE_SendBmlInfo(usecase.BMS_Mode);

    ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Manual);

    // should be remove when XP70 take in charge
    // with Byte stuffing or not???

    /*
            IPP_R_W(SD_STATIC_LOAD_WW,BmlInfo.u16_MaxOPXOutputSize);            // Here static programmation of the BML IPP registers
            IPP_R_W(SD_STATIC_LOAD_BML_PIXEL_ORDER,0x00);                   // GR:00, RG:01, BG:10, GB:11
            IPP_R_W(SD_STATIC_LOAD_LINETYPE0,(LINETYPE_SOF<<12)|1);             // 1 lines embedded data (never more)

            IPP_R_W(SD_STATIC_LOAD_LINETYPE1,(LINETYPE_BLANK<<12)|(BmlInfo.u16_NumberofNonActiveLinesAtTopEdge-1));     // + 1 ligne blank = 2 lines embedded dada
            IPP_R_W(SD_STATIC_LOAD_LINETYPE2,(LINETYPE_ACTIVE_SOF<<12)|(BmlInfo.u16_MaxOPYOutputSize-1));
            // WH-1 line of active data
            IPP_R_W(SD_STATIC_LOAD_LINETYPE3,(LINETYPE_ACTIVE_LAST<<12)|1);         // 1 line of active EOF data for go out blank ligne from BML
            // 15 lines of blank line for go out 4 last line in pipe, one more in VGA -> QCIF
            IPP_R_W(SD_STATIC_LOAD_LINETYPE4,(LINETYPE_SOF<<12)|0x1F);
            IPP_R_W(SD_STATIC_LOAD_LINETYPE5,0);
            IPP_R_W(SD_STATIC_LOAD_LINETYPE6,0);
            IPP_R_W(SD_STATIC_LOAD_LINETYPE7,0);
          */

    //Set BML specific grab structure (shared today with BMS)
    grabparamsBMS.bml_params.nb_linetypes = 5;
    grabparamsBMS.bml_params.line_type[0] = BML_LINETYPE_STARTOFFRAME;
    grabparamsBMS.bml_params.line_type_nb[0] = 1;   // 1 lines embedded data (never more)
    grabparamsBMS.bml_params.line_type[1] = BML_LINETYPE_BLANK;
    grabparamsBMS.bml_params.line_type_nb[1] = BmlInfo.u16_NumberofNonActiveLinesAtTopEdge - 1; // + 1 ligne blank = 2 lines embedded dada
    grabparamsBMS.bml_params.line_type[2] = BML_LINETYPE_ACTIVE;

    //grabparamsBMS.bml_params.line_type_nb[2]=BmlInfo.u16_MaxOPYOutputSize-1; // WH-1 line of active data
    grabparamsBMS.bml_params.line_type_nb[2] = grabparamsBMS.y_frame_size - 1;                  // WH-1 line of active data
    grabparamsBMS.bml_params.line_type[3] = BML_LINETYPE_ACTIVELAST;
    grabparamsBMS.bml_params.line_type_nb[3] = 1;   // 1 line of active EOF data for go out blank ligne from BML
    grabparamsBMS.bml_params.line_type[4] = BML_LINETYPE_BLANK;
    grabparamsBMS.bml_params.line_type_nb[4] = 32;       // FIDO ER # 407862
    grabparamsBMS.bml_params.interline_size = grabparamsBMS.x_frame_size / 2;

    //Read PE and Update Pixel order of buffer for BML to match with the pixel order expected by BMS. <Sandeep Sharma>
    tmp = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);
    switch (tmp)
    {
        case BML_GREENRED:
            grabparamsBMS.bml_params.pixel_order = BML_GREENRED;
            break;

        case BML_REDGREEN:
            grabparamsBMS.bml_params.pixel_order = BML_REDGREEN;
            break;

        case BML_BLUEGREEN:
            grabparamsBMS.bml_params.pixel_order = BML_BLUEGREEN;
            break;

        case BML_GREENBLUE:
            grabparamsBMS.bml_params.pixel_order = BML_GREENBLUE;
            break;

        default:
            LOS_Log(
            "grabparamsBMS.bml_params.pixel_order from SystemConfig_Status_e_PixelOrder_Byte0 is WARNING = %d\n", tmp);
            break;
    }


    grabparamsBMS.bml_params.nb_embedded_lines = BmlInfo.u16_NumberofNonActiveLinesAtTopEdge;

    // Launch Grab out task: Warning: launch Grab HR before LR as grabctl launch HR when a previous LR is available
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
    ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);
    if (GrabBufferLR[0].address != 0)
    {
        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
        ITE_StartGrabNb(GRBPID_PIPE_LR, nbFrames);
    }


    if (TraceBMLEvent)
    {
        BMLStartTime = LOS_getSystemTime();
        LOS_Log(
        "\n   -----------> BML start at %.3Lfs ( %9Ld us)\n",
        (BMLStartTime / ( double ) 1000000),
        BMLStartTime,
        NULL,
        NULL,
        NULL,
        NULL);

        //LOS_Log("(((( Time When starting BML: %llu ))))\n",BMLStartTime);
    }


    //[BG]set hostcontraints BML prescale factor to 2
    // <Hem> As discussed by Atul that following PE write should not be done.
    // But commenting below line is causing issue for zoom bml test. So not commenting it.
    // 11-January_2012 Tested OK with below change
    // ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, 2);

    streaming_error = STARTVPIP_BML();

    tmp = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
    if (tmp < GrabBufferBMS[0].framesizeX)
    {
        tmp = 0;
    }
    else
    {
        tmp = tmp - GrabBufferBMS[0].framesizeX;
    }


    //grabparamsBMS.bml_params.interline_size= tmp;
    if (TraceBMLEvent)
    {
        //DG: force interline to 0 for BML  performances evaluation
        grabparamsBMS.bml_params.interline_size = 0;
    }
    else
    {
        //        grabparamsBMS.bml_params.interline_size = 6 * GrabBufferBMS[0].framesizeX;
        //          grabparamsBMS.bml_params.interline_size = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
        //          grabparamsBMS.bml_params.interline_size = ITE_readPE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0);
    }


    grabparamsBMS.bml_params.interline_size = ITE_readPE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0) - usecase.BMS_XSize;

    LOS_Log("Requested Line Length = %d\n", ITE_readPE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0));

    LOS_Log("Programmed Line Length = %d\n", grabparamsBMS.bml_params.interline_size);

    // should be covered by grabparamsBMS.bml_params.interline_size ... TBC

    /*
          // minimum horizontal time between 2 lines
          tmp = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
          LOS_Log("VTLineLengthPck read = %d\n",tmp);
          if (tmp == 0)
          tmp = 215;
          else tmp = tmp - GrabBufferBMS[0].framesizeX;
          //FIXME: not handled through grab structure....
          IPP_R_W(SD_STATIC_LOAD_LINE_TIME_LSB_EN,((tmp<<1)|1)&0xffffU);
          IPP_R_W(SD_STATIC_LOAD_LINE_TIME_MSB, ((tmp<<1)>>16)&0x000f);
          */
    if (streaming_error == 0)
    {
        usecase.state = STATE_RUNNING;

        // Launch Grab input task:
        ITE_StartGrabNb(GRBPID_PIPE_RAW_IN, nbFrames);
        ITE_WaitEvent(ITE_EVT_GRABBML_EVENTS);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
        if (GrabBufferLR[0].address != 0)
        {
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
        }


#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "BML DATAPATH", "START_BML_PIPE", MAX_BML_START_TIME);
#endif
        return (1);
    }
    else
    {   //Streaming error
        // Grab task HR and/or LR should be aborted
        return (0);
    }
}
/************************************************************************/
/* ITE_BML_Start                            */

/* must Stop Grab and PipeLR                        */

/* launch Grab HR And LR Task; Enable PipeHR and PipeLR; START VPIP */

/************************************************************************/

int
ITE_BML_stripe_Start(
t_uint32    nbFrames,t_uint32 stripe_count)
{
    int         streaming_error;
    t_uint32  xwindowSize, xwindowoffset;
    t_uint16    tmp;
	t_uint32 Pipe_Scalar_Stripe__MinLenghth,Pipe_Scalar_Stripe__CropHSzie,U8_StripeCount;
    UNUSED(U8_StripeCount);
   #ifdef TEST_PERFORMANCE

   RECORD_SYSTEM_TIME("START", "BML DATAPATH","START_BML_PIPE",MAX_BML_START_TIME);

   #endif
 
    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_StopGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }

      if (ITE_IsGrabRunning(GRBPID_PIPE_RAW_IN))
     {
            ITE_StopGrab(GRBPID_PIPE_RAW_IN);
            ITE_WaitEvent(ITE_EVT_GRABBML_EVENTS);
     }

    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }
    usecase.stripeIndex = 0;
    //Configure for first stripe
    //ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
	ITE_ConfigureHRPipe_ForStripe(&(GrabBufferHR[0]), stripe_count,usecase.stripeIndex);
	
    switch (usecase.BML_input)
    {
        case (BAYERLOAD1):  ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_BayerLoad1); break;
        case (BAYERLOAD2):  ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_BayerLoad2); break;
        case (RGBLOAD):     ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_RGBLoad); break;
        default:            LOS_Log_Err("usecase.BML_input case didn't match, BAYERSTORE0 etc\n"); break;
    }
    ITE_SendBmlInfo(usecase.BMS_Mode);	
    ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Manual);
    //Set BML specific grab structure (shared today with BMS)
    grabparamsBMS.bml_params.nb_linetypes = 5;
    grabparamsBMS.bml_params.line_type[0] = BML_LINETYPE_STARTOFFRAME;
    grabparamsBMS.bml_params.line_type_nb[0] = 1;   // 1 lines embedded data (never more)
    grabparamsBMS.bml_params.line_type[1] = BML_LINETYPE_BLANK;
    grabparamsBMS.bml_params.line_type_nb[1] = BmlInfo.u16_NumberofNonActiveLinesAtTopEdge - 1; // + 1 ligne blank = 2 lines embedded dada
    grabparamsBMS.bml_params.line_type[2] = BML_LINETYPE_ACTIVE;

    //grabparamsBMS.bml_params.line_type_nb[2]=BmlInfo.u16_MaxOPYOutputSize-1; // WH-1 line of active data
    grabparamsBMS.bml_params.line_type_nb[2] = GrabBufferBMS[0].framesizeY - (BmlInfo.u16_NumberofNonActiveLinesAtTopEdge) - 1;                  // WH-1 line of active data
    grabparamsBMS.bml_params.line_type[3] = BML_LINETYPE_ACTIVELAST;
    grabparamsBMS.bml_params.line_type_nb[3] = 1;   // 1 line of active EOF data for go out blank ligne from BML
    grabparamsBMS.bml_params.line_type[4] = BML_LINETYPE_BLANK;
	grabparamsBMS.bml_params.line_type_nb[4] = 0x20; 
    grabparamsBMS.bml_params.interline_size = grabparamsBMS.x_frame_size / 2;

    //Read PE and Update Pixel order of buffer for BML to match with the pixel order expected by BMS. <Sandeep Sharma>
	tmp = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);
    switch(tmp)
	{
		case BML_GREENRED:
			grabparamsBMS.bml_params.pixel_order = BML_GREENRED;
		case BML_REDGREEN:
			grabparamsBMS.bml_params.pixel_order = BML_REDGREEN;
		case BML_BLUEGREEN:
			grabparamsBMS.bml_params.pixel_order = BML_BLUEGREEN;
		case BML_GREENBLUE:
			grabparamsBMS.bml_params.pixel_order = BML_GREENBLUE;
		default:
			LOS_Log("grabparamsBMS.bml_params.pixel_order from SystemConfig_Status_e_PixelOrder_Byte0 is WARNING = %d\n",tmp);
	}
    grabparamsBMS.bml_params.nb_embedded_lines = BmlInfo.u16_NumberofNonActiveLinesAtTopEdge;
    // Launch Grab out task: Warning: launch Grab HR before LR as grabctl launch HR when a previous LR is available
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
    ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);
    if (TraceBMLEvent)
    {
        BMLStartTime = LOS_getSystemTime();
        LOS_Log(
        "\n   -----------> BML start at %.3Lfs ( %9Ld us)\n",
        (BMLStartTime / ( double ) 1000000),
        BMLStartTime,
        NULL,
        NULL,
        NULL,
        NULL);
    }
   
switch (usecase.BML_GrbFormat)
{
    case GRBFMT_RAW8:
    case GRBFMT_FAST_RAW8:
		ITE_writePE(DMASetup_e_DMADataBitsInMemory_Byte0, DMADataBitsInMemory_8);
		break;
    case GRBFMT_RAW12:
    case GRBFMT_FAST_RAW12:
		ITE_writePE(DMASetup_e_DMADataBitsInMemory_Byte0, DMADataBitsInMemory_12);
		break;
    default:
		LOS_Log("Error:: Grab format for BML is not correct !!!\n");
}
    ITE_writePE(DMASetup_e_DMADataAccessAlignment_Byte0, DMADataAccessAlignment_e_8Byte);
    ITE_writePE(Pipe_Scalar_StripeInputs_0_u8_StripeCount_Byte0, 2);
// First stripe
    ITE_writePE(Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0, 0);	
    streaming_error = STARTVPIP_BML();
    xwindowSize= ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0); 
    xwindowoffset = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHStart_Byte0);
	ITE_setGrabParams(
    &grabparamsBMS,
    usecase.BML_GrbFormat,
    usecase.BMS_XSize,                              //GrabBufferBMS[0].framesizeX, //
    (GrabBufferBMS[0].framesizeY - (BmlInfo.u16_NumberofNonActiveLinesAtTopEdge)),
    xwindowSize,
    xwindowoffset,
    GrabBufferBMS[0].address,
    (t_bool) TRUE,
    0);
    usecase.mode = BML;
    usecase.state = STATE_PREPARED;	

    tmp = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
    if (tmp < GrabBufferBMS[0].framesizeX)
    {
        tmp = 0;
    }
    else
    {
        tmp = tmp - GrabBufferBMS[0].framesizeX;
    }
    if (TraceBMLEvent)
    {
        //DG: force interline to 0 for BML  performances evaluation
        grabparamsBMS.bml_params.interline_size = 0;
    }
    else
    {
//        grabparamsBMS.bml_params.interline_size = 6 * GrabBufferBMS[0].framesizeX;
//          grabparamsBMS.bml_params.interline_size = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
//          grabparamsBMS.bml_params.interline_size = ITE_readPE(RequestedFrameDimension_u16_VTLineLengthPck_Byte0);
    }
    Pipe_Scalar_Stripe__MinLenghth = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte0);
    Pipe_Scalar_Stripe__CropHSzie = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0);
	 LOS_Log("Pipe_Scalar_Stripe__MinLenghth = %d   Pipe_Scalar_Stripe__CropHSzie = %d \n",Pipe_Scalar_Stripe__MinLenghth,Pipe_Scalar_Stripe__CropHSzie);
    if(Pipe_Scalar_Stripe__MinLenghth > Pipe_Scalar_Stripe__CropHSzie)
    	{
    grabparamsBMS.bml_params.interline_size = Pipe_Scalar_Stripe__MinLenghth - Pipe_Scalar_Stripe__CropHSzie; //6000;
    	}
	else
		{
		grabparamsBMS.bml_params.interline_size =0 ;
		}
    LOS_Log("Requested Line Length_1 = %d\n",ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte0));
    LOS_Log("Programmed Line Length_1 = %d\n",grabparamsBMS.bml_params.interline_size);
  
    if (streaming_error == 0)
    {
        usecase.state = STATE_RUNNING;    	
        // Launch Grab input task:
        ITE_StartGrabNb(GRBPID_PIPE_RAW_IN, nbFrames);		
        ITE_WaitEvent(ITE_EVT_GRABBML_EVENTS);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);

// for 2nd stripe
  STOPVPIP_BML();  
  ITE_writePE(Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0, 1);
  streaming_error = STARTVPIP_BML();
   usecase.stripeIndex = 1;
  ITE_ConfigureHRPipe_ForStripe(&(GrabBufferHR[0]), stripe_count,usecase.stripeIndex);
   ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);   
   xwindowSize = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0);
   xwindowoffset = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHStart_Byte0);   
 	
		Pipe_Scalar_Stripe__MinLenghth = ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte0);
		Pipe_Scalar_Stripe__CropHSzie =  ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_StripeInCropHSize_Byte0);
		 LOS_Log("Pipe_Scalar_Stripe__MinLenghth = %d	Pipe_Scalar_Stripe__CropHSzie = %d \n",Pipe_Scalar_Stripe__MinLenghth,Pipe_Scalar_Stripe__CropHSzie);
		if(Pipe_Scalar_Stripe__MinLenghth > Pipe_Scalar_Stripe__CropHSzie)
			{
		grabparamsBMS.bml_params.interline_size = Pipe_Scalar_Stripe__MinLenghth - Pipe_Scalar_Stripe__CropHSzie ; //6000;
			}
		else
			{
			grabparamsBMS.bml_params.interline_size = 0; //64 ;
			}
		
	//grabparamsBMS.bml_params.interline_size= 8000;
    LOS_Log("Requested Line Length_2 = %d\n",ITE_readPE(Pipe_Scalar_StripeOutputs_0_s16_MinLineLength_Byte0));
    LOS_Log("Programmed Line Length_2 = %d\n",grabparamsBMS.bml_params.interline_size);
	 
   ITE_setGrabParams(
       &grabparamsBMS,
   	usecase.BML_GrbFormat,
   	usecase.BMS_XSize,                              
   	(GrabBufferBMS[0].framesizeY - BmlInfo.u16_NumberofNonActiveLinesAtTopEdge),
   	xwindowSize,
   	xwindowoffset,
   	GrabBufferBMS[0].address,
   	(t_bool) TRUE,    0);
   usecase.mode = BML;
   usecase.state = STATE_PREPARED;

     if (streaming_error == 0)    
     {
     
           usecase.state = STATE_RUNNING;
           ITE_StartGrabNb(GRBPID_PIPE_RAW_IN, nbFrames);
		   ITE_WaitEvent(ITE_EVT_GRABBML_EVENTS);
		   ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
     }
	 else
	 	{
	 	     LOS_Log("BML Stripe Error\n");
	 	     return (0);
	 	}
    usecase.stripeInUse = 0;
    usecase.stripeIndex = 0;

#ifdef TEST_PERFORMANCE

  RECORD_SYSTEM_TIME("STOP", "BML DATAPATH","START_BML_PIPE",MAX_BML_START_TIME);

  #endif
        return (1);
    }
    else
    {   //Streaming error
        // Grab task HR and/or LR should be aborted
        return (0);
    }
}


/****************************************/

/* ITE_BML_Stop             */

/*                  */

/****************************************/
void
ITE_BML_Stop(void)
{
   if(ITE_IsVPIPRunning())
    {
#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("START", "BML DATAPATH", "STOP_BML_PIPE", MAX_BML_STOP_TIME);
#endif
        STOPVPIP_BML();
        ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
        usecase.state = STATE_PREPARED;

        // <Hem> Set FrameDimensionProgMode back to Auto mode when BML is finished
        // solution to <ER : 362019>
        ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);
    ITE_writePE(Pipe_Scalar_StripeInputs_0_u8_StripeCount_Byte0, 0);
    ITE_writePE(Pipe_Scalar_StripeInputs_0_u8_StripeIndex_Byte0, 0);
#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "BML DATAPATH", "STOP_BML_PIPE", MAX_BML_STOP_TIME);
#endif
    }
    else
    {
        LOS_Log("BML stopped\n");
    }
}


/************************************************/

/* ITE_BML_Free                 */

/* DeAllocation of GrabBuffer_HR[] _BMS     */

/************************************************/
void
ITE_BML_Free(void)
{
    ITE_Still_Free();
    ITE_BMS_Free();
}


/********************************************************/

/* ITE_VideoPreview_Prepare             */

/* Initialized BufferLR Structure; Configure PipeLR */

/********************************************************/
void
ITE_VideoPreview_Prepare(
tps_sia_usecase p_sia_usecase)
{   // Warning je ne sais pas encore comment je gere le fait que le Buffer de sortie du pipeLR
    // soit le meme que le LCD buffer
    ITE_createSiaPictureBuffer(
    &(GrabBufferLR[0]),
    p_sia_usecase->LR_XSize,
    p_sia_usecase->LR_YSize,
    p_sia_usecase->LR_GrbFormat,
    BUF_LR);

    //GrabBufferLR[0] = LCDBuffer;
    ITE_ConfigureLRPipe(&(GrabBufferLR[0]));
    p_sia_usecase->mode = VIDEOPREVIEW;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************/

/* ITE_VideoPreview_Start           */

/* launch Grab Task; Enable PipeLR; START VPIP  */

/************************************************/
void
ITE_VideoPreview_Start(
t_uint32    nbFrames)
{
  UNUSED(nbFrames);
    /*
          //ITE_InitDatapath(InputImageSource_Sensor, OutputImageDestination_PixelPipes);
          //ITE_SendDatapath();
          // Launch Grab task
          ITE_StartGrabNb(GRBPID_PIPE_LR,nbFrames);
          ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
          ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
          STARTVPIP();
          usecase.state = STATE_RUNNING;
          */
}


/************************************************************************/

/* ITE_StillPreview_Stop                        */

/* Stop Grab Task; Wait for completion; STOP VPIP; Disable PipeLR   */

/************************************************************************/
void
ITE_VideoPreview_Stop(void)
{
    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    usecase.state = STATE_PREPARED;
}


/************************************************/

/* ITE_VideoPreview_Free            */

/* DeAllocation of GrabBuffer_LR[]      */

/************************************************/
void
ITE_VideoPreview_Free(void)
{
    // Warning GrabBuffer[0] is not allocated in memory but it is LCDBuffer
    //GrabBufferLR[0].address = NULL;
    //GrabBufferLR[1].address = NULL;
    if (GrabBufferLR[0].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[0]));
    }


    if (GrabBufferLR[1].address != 0)
    {
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[1]));
    }


    usecase.mode = MODE_UNDEFINED;
    usecase.state = STATE_FREE;
}


/********************************************************/

/* ITE_Video_Prepare                    */

/* Initialized BufferHR Structure; Configure PipeHR */

/********************************************************/
void
ITE_Video_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    UNUSED(nbFrames);
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);
    ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
    p_sia_usecase->mode = VIDEO;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************************************/

/* ITE_Video_Start                          */

/* must Stop Grab and PipeLR                        */

/* launch Grab HR And LR Task; Enable PipeHR and PipeLR; START VPIP */

/* WARNING for beginning we don't use LR during Still           */

/************************************************************************/
void
ITE_Video_Start(
t_uint32    nbFrames)
{
 UNUSED(nbFrames);
    /*
            if(ITE_IsGrabRunning(GRBPID_PIPE_LR)) {
            ITE_StopGrab(GRBPID_PIPE_LR);
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);}
            if(ITE_IsVPIPRunning())
            STOPVPIP();
            //ITE_InitDatapath(InputImageSource_Sensor, OutputImageDestination_PixelPipes);
            //ITE_SendDatapath();
            ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
            ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
            // Launch Grab task
            ITE_StartGrabNb(GRBPID_PIPE_LR,nbFrames);
            ITE_StartGrabNb(GRBPID_PIPE_HR,nbFrames);
            ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
            STARTVPIP();
            usecase.state = STATE_RUNNING;
            if (nbFrames != INFINITY) {
            ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
            }
          */
}


/****************************************/

/* ITE_Video_Stop           */

/*                  */

/****************************************/
void
ITE_Video_Stop(void)
{
    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_StopGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
    usecase.state = STATE_PREPARED;
}


/************************************************/

/* ITE_Video_Free               */

/* DeAllocation of GrabBuffer_HR[]      */

/************************************************/
void
ITE_Video_Free(void)
{
    if (GrabBufferHR[0].address != 0)
    {
        // if GrabBufferHR[0] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[0].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[0]));
    }


    if (GrabBufferHR[1].address != 0)
    {
        // if GrabBufferHR[1] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[1].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[1]));
    }


    usecase.mode = MODE_UNDEFINED;
    usecase.state = STATE_FREE;
}


/********************************************************/

/* ITE_DisplayBuffer(tps_siapicturebuffer p_Buffer) */

/* display a buffer on screen in "INFINITY mode"    */

/********************************************************/
void
ITE_DisplayBuffer(
tps_siapicturebuffer    p_Buffer)
{
    if ((p_Buffer->address) == 0)
    {
        LOS_Log("WARNING Buffer cannot be displayed becasue buffer empty\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }


#ifdef __ARM_LINUX
    else
    {
        struct mcde_source_buffer   fb_buffer;
        struct mcde_addrmap         fb_addr;
        short                       *frame_buffer_address;
        char                        fbname[1024];
        int                         fbfd = -1;
        int                         screensize;

        if (p_Buffer->pipe == BUF_LR)
        {
            g_dplLR_infinite = INFINITY;
            g_dplHR_infinite = 0;
        }


        if (p_Buffer->pipe == BUF_HR)
        {
            g_dplHR_infinite = INFINITY;
            g_dplLR_infinite = 0;
        }


        // Open FrameBuffer device
        strcpy(fbname, "/dev/fb0");
        fbfd = open(fbname, O_RDWR);
        if (fbfd == -1)
        {
            LOS_Log("Error: failed to open %s.\n", fbname);
            return;
        }


        LOS_Log("Opened %s.\n", fbname);

        // Format different than WVGA RGB565 ==> Need to use B2R2
        if
        (
            !((p_Buffer->framesizeX == 864)
            &&  (p_Buffer->framesizeY == 480)
            &&  (p_Buffer->Grb_colorformat == GRBFMT_R5G6B5)
            )
        )
        {
            // Allocate a buffer for a WVGA RGB565 picture
            screensize = 864 * 480 * 2;

            frame_buffer_address = ( short * ) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
            if (( int ) frame_buffer_address == -1)
            {
                printf("Error: failed to map framebuffer device to memory.\n");
                return; //__NO_WARNING__
            }


            // Configure B2R2 for filling this buffer
            ITE_Configure_B2R2(p_Buffer, ( unsigned int * ) &frame_buffer_address); //__NO_WARNING__

            // Give this buffer to FB
            //==> B2R2 output buffer is directly map to FrameBuffer
            // We pass 0 to buffid and NULL to buffaddr
            // and check these values in ITE_Configure_FrameBuffer().
            //fb_buffer.buffaddr = (struct mcde_addrmap)NULL;
            fb_buffer.buffid = 0;
        }


        // Format WVGA RGB565 directly support by MCDE
        else
        {
            // Give buffer to FB
            fb_addr.cpuaddr = ( unsigned long ) p_Buffer->logAddress;
            fb_addr.dmaaddr = ( unsigned long ) p_Buffer->address;
            fb_addr.bufflength = ( unsigned long ) p_Buffer->buffersizeByte;
            fb_buffer.buffaddr = fb_addr;
            fb_buffer.buffid = 1;
        }


        // Open and configure the FB device
        ITE_Configure_FrameBuffer(&fb_buffer, fbfd);

        return;
    }


#endif //__ARM_LINUX
}


/********************************************************/

/* ITE_DisplayBufferStop(void)                          */

/*                                                  */

/********************************************************/
void
ITE_DisplayBufferStop(void)
{
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
    {
        ITE_StopMcde();
        g_dplLR_infinite = 0;
        g_dplHR_infinite = 0;
    }


#endif
}


/****************************************************************/

/* ITE_DisplaySingleBuffer(tps_siapicturebuffer p_Buffer)   */

/* display a buffer on screen in "single step mode"     */

/****************************************************************/
void
ITE_DisplaySingleBuffer(
tps_siapicturebuffer    p_Buffer)
{
    if ((p_Buffer->address) == 0)
    {
        LOS_Log("WARNING Buffer cannot be displayed becasue buffer empty\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    else
    {
#if !(defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
        if (0xff != (p_Buffer->MCDE_colorformat))
        {
            ITE_StopMcde();
            ITE_configureMcde(p_Buffer);
            g_dplLR_infinite = 0;
            g_dplHR_infinite = 0;

            MMTE_MCDE_SwFrameSync();
        }
        else    // if b2r2 needed
        {
            if (0xff != (p_Buffer->B2R2_colorformat))
            {
                ITE_StopMcde();
                ITE_configureMcde(&LCDBuffer);
                MMTE_B2R2_Close();

                //ITE_ConvertBufferFormatwithB2R2(p_Buffer,&LCDBuffer);
                ITE_InitB2R2(&g_B2R2_node, p_Buffer, &LCDBuffer);
                g_B2R2HR_infinite = 0;
                if (p_Buffer->pipe == BUF_LR)
                {
                    g_dplLR_infinite = 255;
                    g_dplHR_infinite = 0;
                }


                if (p_Buffer->pipe == BUF_HR)
                {
                    g_dplHR_infinite = 255;
                    g_dplLR_infinite = 0;
                }


                LOS_Sleep(500);
                MMTE_B2R2LaunchNodeList(B2R2_AQ3, &g_B2R2_node.B2R2_set_aq_reg);
            }
            else
            {
                LOS_Log("WARNING Buffer cannot be displayed: unknown MCDE and B2R2 format\n");
            }
        }


        /*
                        char filename[20];

                    //add tempo so as buffer is OK: else buffer is generated but corrupted
                    LOS_Sleep(5000);

                    #ifdef DSI_DISPLAY
                    // display buffer
                    MMTE_MCDE_SwFrameSync();
                    #endif

                    snprintf(filename, 20, "frame");
                    ITE_StoreinBMPFile(filename,p_Buffer);
                      */
#endif
    }
}


/************************************************************************/

/* t_uint16 IsFormatCompatibleWithMCDE(enum e_grabFormat format)    */

/* return true if buffer can be displayed directly with MCDE        */

/*    false if not                          */

/************************************************************************/
t_uint16
IsFormatCompatibleWithMCDE(
enum e_grabFormat   format)
{
    t_uint16    status; // 0:true  1:false 0xff:under investigation
    switch (format)
    {
        case (GRBFMT_YUV422_RASTER_INTERLEAVED):        status = 0; break;
        case (GRBFMT_YUV420_RASTER_PLANAR_I420):        status = 0xff; break;                       // ??

        case (GRBFMT_YUV420_RASTER_PLANAR_YV12):        status = 0xff; break;                       // ??

        case (GRBFMT_YUV422_MB_SEMIPLANAR):             status = 0xff; break;
        case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):    status = 0xff; break;
        case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):   status = 0xff; break;
        case (GRBFMT_RGB30):                            status = 0xff; break;                       // ??

        case (GRBFMT_RAW8):                             status = 0xff; break;                       // ??

        case (GRBFMT_RAW12):                            status = 0xff; break;                       // ??

        case (GRBFMT_A4R4G4B4):                         status = 0; break;
        case (GRBFMT_A1R5G5B5):                         status = 0; break;
        case (GRBFMT_R5G6B5):                           status = 0; break;
        case (GRBFMT_R8G8B8):                           status = 0; break;
        case (GRBFMT_A8R8G8B8):                         status = 0; break;
        default:                                        status = 0xff; break;                       // could not happend
    }


    return (status);
}


#if !(defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
void
ITE_ConvertBufferFormatwithB2R2(
tps_siapicturebuffer    p_inputBuffer,
tps_siapicturebuffer    p_outputBuffer)
{
    //FIXME: not yet ported on 8500
#   if !(defined(__PEPS8500_SIA) || defined(_SVP_))
    ts_b2r2_config  B2R2_node;
    MMTE_B2R2_Close();
    ITE_InitB2R2(&B2R2_node, p_inputBuffer, p_outputBuffer);

    //process YUV MB with B2R2 to output RGB565 format with full resolution
    MMTE_B2R2LaunchNodeList(B2R2_AQ3, &B2R2_node.B2R2_set_aq_reg);
    MMTE_B2R2WaitEndOfNode();
    MMTE_B2R2_Close();
#   endif
}


#endif
void
ITE_WriteRGB16Buffer2GamFile(
ts_siapicturebuffer gamBuffer,
t_uint32            xSnapshot,
t_uint32            ySnapshot,
char                *fileName)
{
    FILE        *out_file;
    char        out_filename[64];
    t_uint32    gam_header[11];

    //dump grabbed frame in RGB16 format
    LOS_Log("\nwrite output file %s_%d_%d.gam\n", fileName, xSnapshot, ySnapshot, NULL, NULL, NULL, NULL);
    sprintf(out_filename, "../../%s_%d_%d.gam", fileName, ( int ) xSnapshot, ( int ) ySnapshot);    //__NO_WARNING__
    out_file = fopen(out_filename, "wb");
    if (out_file == NULL)
    {
        exit(1);
    }


    //write gamma header
    gam_header[0] = 0x444f0006;
    gam_header[1] = 0x00000080;
    gam_header[2] = xSnapshot;
    gam_header[3] = ySnapshot;
    gam_header[4] = xSnapshot * ySnapshot;
    gam_header[5] = 0x00000000;

    fwrite((t_uint8 *) (gam_header), 6 * 4, 1, out_file);
    fwrite((t_uint8 *) (gamBuffer.logAddress), (xSnapshot * ySnapshot * 2), 1, out_file);
    fclose(out_file);
    LOS_Log("output file %s_%d_%d.gam written", fileName, xSnapshot, ySnapshot, NULL, NULL, NULL, NULL);
}


//Tempo till a key is pressed on keyPad
void
ITE_WaitUntilKeyPressed(void)
{
    /*
            do {
            }
            while (g_keypad_still != 1);
            g_keypad_still = 0;
          */

    // key board driver no more supported (see in lib ite that ite_ske.c is no more compiled
}


//ported on 8500
//void ITE_Shutter(IrisCmd_te action)
//{
//t_uint16 coin;
//   ITE_writePE(IrisControl_e_ShutterAndIrisType_Type_Byte0,ShutterAndIrisType_e_Iris_shutter);
//   ITE_writePE(IrisControl_e_IrisCmd_Cmd_Byte0,IrisCmd_e_IrisOpen);
//   coin = ITE_readPE(IrisControl_e_TestCoin_bHostTestCoin_Byte0) + 1;
//   ITE_writePE(IrisControl_e_TestCoin_bHostTestCoin_Byte0,coin);
///   while (ITE_readPE(IrisStatus_e_TestCoin_bDeviceTestCoin_Byte0) != coin) {}
//   LOS_Log("iris status : %d\n",ITE_readPE(IrisStatus_e_IrisStatus_Status_Byte0));
//}

/********************************************************/

/* ITE_HR_Prepare                   */

/* Initialized BufferHR Structure; Configure PipeHR */

/********************************************************/

//new for 8500
void
ITE_HR_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
 UNUSED(nbFrames);
    //No reality to check use case in HR granularity
          ///ITE_CheckStillUseCase(p_sia_usecase); // check if format and size are correct according to Architecture doc
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);
    ITE_CleanBuffer(&(GrabBufferHR[0]));
    ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
    p_sia_usecase->mode = STILL;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************************************/

/* ITE_HR_Start                         */

/* launch Grab HR Task; Enable PipeHR ; START VPIP  */

/************************************************************************/

//new for 8500

//FIXME: add custom to support both sensor0 and 1
int
ITE_HR_Start(
t_uint32    nbFrames)
{
    int error = 0;

#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("START", "HR DATAPATH", "START_HR_PIPE", MAX_HR_START_TIME);
#endif
    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_StopGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    //ITE_InitDatapath(InputImageSource_e_Sensor0, OutputImageDestination_PixelPipes);
    //ITE_SendDatapath();
    if (GrabBufferHR[0].address != 0)
    {
        ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
        ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);
    }


    ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);

    //Perform Run command and wait for ISP_STREAMING event
    error = STARTVPIP();

    if ((error != 0) && (ITE_IsGrabRunning(GRBPID_PIPE_HR)))
    {
        LOS_Log("****************************************\n");
        LOS_Log("        WARNING Grab Task aborted !!!   \n");
        LOS_Log("****************************************\n");
        LOS_Log("TEST FAILED\n");
        ITE_AbortGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
        ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_STOP);
    }


#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("STOP", "HR DATAPATH", "START_HR_PIPE", MAX_HR_START_TIME);
#endif
    return (error);
}


/****************************************/

/* ITE_HR_Stop          */

/*                  */

/****************************************/

//new for 8500
void
ITE_HR_Stop(void)
{
     if(ITE_IsVPIPRunning())
    {

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("START", "HR DATAPATH", "STOP_HR_PIPE", MAX_HR_STOP_TIME);
#endif
        if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
        {
            ITE_StopGrab(GRBPID_PIPE_HR);
            ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
        }


        STOPVPIP();
        ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "HR DATAPATH", "STOP_HR_PIPE", MAX_HR_STOP_TIME);
#endif
    }
    else
    {
        LOS_Log("HR Pipe stopped\n");
        return;
    }  
}


void
ITE_HR_Abort(void)
{
    LOS_Log("\n>> ITE_HR_Abort");

    //RECORD_SYSTEM_TIME("START", "HR DATAPATH","STOP_HR_PIPE",MAX_HR_STOP_TIME);
    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_AbortGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    STOPVPIP();
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
    LOS_Log("<< ITE_HR_Abort");

    //RECORD_SYSTEM_TIME("STOP", "HR DATAPATH","STOP_HR_PIPE",MAX_HR_STOP_TIME);
}


/************************************************/

/* ITE_HR_Free              */

/* DeAllocation of GrabBuffer_HR[]      */

/************************************************/

//new for 8500
void
ITE_HR_Free(void)
{
    if (GrabBufferHR[0].address != 0)
    {
        // if GrabBufferHR[0] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[0].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[0]));
    }


    if (GrabBufferHR[1].address != 0)
    {
        // if GrabBufferHR[1] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferHR[1].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferHR[1]));
    }


    //Free grab cache buffer if needed
    if (usecase.esramGrabCacheMemHandle != 0)
    {
        t_cm_error  cmError;

        cmError = CM_FreeMpcMemory(usecase.esramGrabCacheMemHandle);
        if (cmError != CM_OK)
        {
            LOS_Log("\nERROR freeing grab cache buffer !!! \n", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }


        usecase.esramGrabCacheMemHandle = 0;
    }


    // Display the welcome screen
    //mle  DisplayStartScreen(0);
}


/********************************************************/

/* ITE_LR_Prepare                   */

/* Initialized BufferLR Structure; Configure PipeLR */

/********************************************************/

//new for 8500
void
ITE_LR_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    //No reality to check use case in HR granularity
          ///ITE_CheckStillUseCase(p_sia_usecase); // check if format and size are correct according to Architecture doc
    UNUSED(nbFrames);
    
    ITE_createSiaPictureBuffer(
    &(GrabBufferLR[0]),
    p_sia_usecase->LR_XSize,
    p_sia_usecase->LR_YSize,
    p_sia_usecase->LR_GrbFormat,
    BUF_LR);
    ITE_CleanBuffer(&(GrabBufferLR[0]));
    ITE_ConfigureLRPipe(&(GrabBufferLR[0]));
    p_sia_usecase->mode = STILL;
    p_sia_usecase->state = STATE_PREPARED;
}


/************************************************************************/

/* ITE_LR_Start                         */

/* launch Grab LR Task; Enable PipeLR ; START VPIP  */

/************************************************************************/

//new for 8500

//FIXME: add custom to support both sensor0 and 1
int
ITE_LR_Start(
t_uint32    nbFrames)
{
    int error = 0;
#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("START", "LR DATAPATH", "START_LR_PIPE", MAX_LR_START_TIME);
#endif
    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    if (GrabBufferLR[0].address != 0)
    {
        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
        ITE_StartGrabNb(GRBPID_PIPE_LR, nbFrames);
    }


    ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);

    //ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
    //Perform Run command and wait for ISP_STREAMING event
    error = STARTVPIP();

    if ((error != 0) && (ITE_IsGrabRunning(GRBPID_PIPE_LR)))
    {
        LOS_Log("****************************************\n");
        LOS_Log("      WARNING Grab Task aborted !!!     \n");
        LOS_Log("****************************************\n");
        ITE_AbortGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
        ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_STOP);
    }


    //ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
#ifdef TEST_PERFORMANCE
    RECORD_SYSTEM_TIME("STOP", "LR DATAPATH", "START_LR_PIPE", MAX_LR_STOP_TIME);
#endif
    return (error);
}


/****************************************/

/* ITE_LR_Stop              */

/*                  */

/****************************************/

//new for 8500
void
ITE_LR_Stop(void)
{
     if(ITE_IsVPIPRunning())
    {

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("START", "LR DATAPATH", "STOP_LR_PIPE", MAX_LR_STOP_TIME);
#endif
        if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
        {
            ITE_StopGrab(GRBPID_PIPE_LR);
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
        }


        STOPVPIP();
        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "LR DATAPATH", "STOP_LR_PIPE", MAX_LR_STOP_TIME);
#endif
    }
    else
    {
        LOS_Log("LR Pipe stopped\n");
        return;
    }
}


void
ITE_LR_Abort(void)
{
    LOS_Log("\n>> ITE_LR_Abort");

    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_AbortGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }

    STOPVPIP();
    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    
    LOS_Log("<< ITE_LR_Abort");

}


/************************************************/

/* ITE_LR_Free              */

/* DeAllocation of GrabBuffer_LR[]      */

/************************************************/

//new for 8500
void
ITE_LR_Free(void)
{
    if (GrabBufferLR[0].address != 0)
    {
        // if GrabBufferLR[0] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferLR[0].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[0]));
    }


    if (GrabBufferLR[1].address != 0)
    {
        // if GrabBufferLR[1] == LCDBuffer
        // if (LCDBuffer.handle == GrabBufferLR[1].handle) LCDBuffer.address = NULL;
        ITE_FreeSiaPictureBuffer(&(GrabBufferLR[1]));
    }


    // Display the welcome screen
    //mle DisplayStartScreen(0);
}


/********************************************************/

/* ITE_LRHR_Prepare                     */

/* Initialized BufferHR Structure; Configure PipeHR */

/* Initialized BufferLR Structure; Configure PipeLR */

/********************************************************/

//new for 8500
void
ITE_LRHR_Prepare(
tps_sia_usecase p_sia_usecase,
t_uint32        nbFrames)
{
    UNUSED(nbFrames);
    //No reality to check use case in HR granularity
          ///ITE_CheckStillUseCase(p_sia_usecase); // check if format and size are correct according to Architecture doc
    ITE_createSiaPictureBuffer(
    &(GrabBufferLR[0]),
    p_sia_usecase->LR_XSize,
    p_sia_usecase->LR_YSize,
    p_sia_usecase->LR_GrbFormat,
    BUF_LR);
    ITE_CleanBuffer(&(GrabBufferLR[0]));
    ITE_ConfigureLRPipe(&(GrabBufferLR[0]));
    ITE_createSiaPictureBuffer(
    &(GrabBufferHR[0]),
    p_sia_usecase->HR_XSize,
    p_sia_usecase->HR_YSize,
    p_sia_usecase->HR_GrbFormat,
    BUF_HR);
    ITE_CleanBuffer(&(GrabBufferHR[0]));
    ITE_ConfigureHRPipe(&(GrabBufferHR[0]));
}


/************************************************************************/

/* ITE_LRHR_Start                           */

/* launch Grab LR and HR Tasks; Enable both PipeLR and HR ; START VPIP  */

/************************************************************************/

//new for 8500

//FIXME: add custom to support both sensor0 and 1
int
ITE_LRHR_Start(
t_uint32    nbFrames)
{
    int error = 0;

    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_StopGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_StopGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    if (ITE_IsVPIPRunning())
    {
        STOPVPIP();
    }


    if (GrabBufferHR[0].address != 0)
    {
        ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Enable);
        ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);
    }


    if (GrabBufferLR[0].address != 0)
    {
        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Enable);
        ITE_StartGrabNb(GRBPID_PIPE_LR, nbFrames);
    }


    //ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
    //Perform Run command and wait for ISP_STREAMING event
    error = STARTVPIP();

    if (error != 0)
    {
        LOS_Log("****************************************\n");
        LOS_Log("       WARNING Grab Task aborted !!!    \n");
        LOS_Log("****************************************\n");
        LOS_Log("TEST FAILED\n");
        ITE_AbortGrab(GRBPID_PIPE_LR);
        ITE_AbortGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
        ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_STOP);
    }


    //ITE_writePE(ClockManagerControl_uwClkCheckerDetectWDW_MSByte, 0x10);
    return (error);
}


/****************************************/

/* ITE_LRHR_Stop            */

/*                  */

/****************************************/

//new for 8500
void
ITE_LRHR_Stop(void)
{
    if(ITE_IsVPIPRunning())
    {

        if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
        {
            ITE_StopGrab(GRBPID_PIPE_HR);
            ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
        }


        if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
        {
            ITE_StopGrab(GRBPID_PIPE_LR);
            ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
        }


        STOPVPIP();

        ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
        ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);

    }
    else
    {
        LOS_Log("LRHR stopped \n");
    }
	
}


void
ITE_LRHR_Abort(void)
{
    LOS_Log("\n>> ITE_LRHR_Abort");
    if (ITE_IsGrabRunning(GRBPID_PIPE_HR))
    {
        ITE_AbortGrab(GRBPID_PIPE_HR);
        ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);
    }


    if (ITE_IsGrabRunning(GRBPID_PIPE_LR))
    {
        ITE_AbortGrab(GRBPID_PIPE_LR);
        ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);
    }


    STOPVPIP();

    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
    LOS_Log("\n<< ITE_LRHR_Abort");
}


/************************************************/

/* ITE_LRHR_Free                */

/* DeAllocation of GrabBuffers LR and HR[]      */

/************************************************/

//new for 8500
void
ITE_LRHR_Free(void)
{
    ITE_LR_Free();
    ITE_HR_Free();
}

void ITE_stop_sia(void )
{
	if(ITE_IsVPIPRunning())
	{
 	    ITE_AbortGrab(GRBPID_PIPE_HR);
	    ITE_AbortGrab(GRBPID_PIPE_LR);
	    ITE_SendCommands(HostInterfaceCommand_e_STOP, ISP_STOP_STREAMING);
	    ITE_writePE(DataPathControl_e_Flag_BayerStore0Enable_Byte0, Flag_e_FALSE);
	    ITE_writePE(DataPathControl_e_Flag_BayerStore1Enable_Byte0, Flag_e_FALSE);
	    ITE_writePE(DataPathControl_e_Flag_BayerStore2Enable_Byte0, Flag_e_FALSE);
	    ITE_ActivePipe(GRBPID_PIPE_HR, PipeControl_Disable);
	    ITE_ActivePipe(GRBPID_PIPE_LR, PipeControl_Disable);
	    ITE_writePE(HostFrameConstraints_e_FrameDimensionProgMode_Byte0, FrameDimensionProgMode_e_Auto);
	}
	
}

//gamma curve values
//standard 0
//custom 1
//disable 2
void ITE_SetGammaCurve(char *curve)
{
    t_uint32 curve_no;
    t_uint32 status_coin;
    char        mess[256];

    int i = 0;
    t_uint32 custom_values[128] = { 0,113,155,186,212,235,255,274,291,307,322,336,350,363,375,387,399,410,421,431,442,451,461,471,480,489,497,506,515,523,531,539,547,554,562,569,577,584,591,598,605,612,619,625,632,638,645,651,657,664,670,676,682,688,694,699,705,711,716,722,728,733,738,744,749,754,760,765,770,775,780,785,790,795,800,805,810,815,820,824,829,834,839,843,848,852,857,861,866,870,875,879,884,888,892,897,901,905,909,913,918,922,926,930,934,938,942,946,950,954,958,962,966,970,974,978,982,986,989,993,997,1001,1004,1008,1012,1016,1019,1023 };

    t_uint32 *ptr_ce0_sharp_green = NULL;
    t_uint32 *ptr_ce0_sharp_red = NULL;
    t_uint32 *ptr_ce0_sharp_blue = NULL;
    t_uint32 *ptr_ce0_unsharp_green = NULL;
    t_uint32 *ptr_ce0_unsharp_red = NULL;
    t_uint32 *ptr_ce0_unsharp_blue = NULL;

    t_uint32 *ptr_ce1_sharp_green = NULL;
    t_uint32 *ptr_ce1_sharp_red = NULL;
    t_uint32 *ptr_ce1_sharp_blue = NULL;
    t_uint32 *ptr_ce1_unsharp_green = NULL;
    t_uint32 *ptr_ce1_unsharp_red = NULL;
    t_uint32 *ptr_ce1_unsharp_blue = NULL;

    union u_ITE_Event   event;

    MMTE_TEST_START("TestDynamicGammaUpdate", "/ite_nmf/test_results_nreg/gamma_tests", "Test Gamma_dynamic_update");

    snprintf(mess, sizeof(mess), "\nDynamic Gamma Test\n");
    MMTE_TEST_NEXT(mess);

    if(0 == strcmp(curve, "standard"))
        curve_no = 0;
    else if (0 == strcmp(curve, "custom"))
        curve_no = 1;
    else if (0 == strcmp(curve, "disable"))
        curve_no = 2;
    else
    {
        LOS_Log("\nInvalid value of gamma curve.\n");
        MMTE_TEST_SKIPPED();
        MMTE_TEST_FAILED();
        return;
    }


    if(curve_no == 1)  //custom
    {
        ptr_ce0_sharp_green = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*0);
        ptr_ce0_sharp_red = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*1);
        ptr_ce0_sharp_blue = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*2);
        ptr_ce0_unsharp_green = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*3);
        ptr_ce0_unsharp_red = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*4);
        ptr_ce0_unsharp_blue = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*5);

        ptr_ce1_sharp_green = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*6);
        ptr_ce1_sharp_red = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*7);
        ptr_ce1_sharp_blue = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*8);
        ptr_ce1_unsharp_green = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*9);
        ptr_ce1_unsharp_red = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*10);
        ptr_ce1_unsharp_blue = (t_uint32 *) pGammaLUTBuffer->logAddress + (GAMMA_LUT_TABLE_SIZE*11);


       //here shared memory addresses are filled with custom gaama values. (This is to be done by host)
       //But in case of testing with ite_nmf, we are hardcoding the values in shared memory addresses

        for(i = 0; i < NO_OF_LUTBINS; i++)
        {
            *(ptr_ce0_sharp_green + i) = (t_uint32)custom_values[i];
            *(ptr_ce0_sharp_red + i) = (t_uint32)custom_values[i];
            *(ptr_ce0_sharp_blue + i) = (t_uint32)custom_values[i];
            *(ptr_ce0_unsharp_green + i) = (t_uint32)custom_values[i];
            *(ptr_ce0_unsharp_red + i) = (t_uint32)custom_values[i];
            *(ptr_ce0_unsharp_blue + i) = (t_uint32)custom_values[i];

            *(ptr_ce1_sharp_green + i) = (t_uint32)custom_values[i];
            *(ptr_ce1_sharp_red+ i) = (t_uint32)custom_values[i];
            *(ptr_ce1_sharp_blue + i) = (t_uint32)custom_values[i];
            *(ptr_ce1_unsharp_green + i) = (t_uint32)custom_values[i];
            *(ptr_ce1_unsharp_red + i) = (t_uint32)custom_values[i];
            *(ptr_ce1_unsharp_blue + i) = (t_uint32)custom_values[i];
        }
    }

#if 1
    //first to check which pipe is active
    if(1 == ITE_readPE(DataPathControl_e_Flag_Pipe0Enable_Byte0))  //pipe 0 is active
    {
        snprintf(mess, sizeof(mess), "[Usecase] \nVALUE OF CURVE %ld\n",curve_no);
        MMTE_TEST_COMMENT(mess);

        ITE_writePE(CE_GammaControl_0_e_GammaCurve_Byte0, curve_no);
        ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_Sharp_Byte0, 0);
        ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_UnSharp_Byte0, 0);

        status_coin = ITE_readPE(CE_GammaStatus_0_e_Coin_Status_Byte0);

        if(status_coin == 0)   //code for toggling the coin
            ITE_writePE(CE_GammaControl_0_e_Coin_Ctrl_Byte0, 1);
        else
            ITE_writePE(CE_GammaControl_0_e_Coin_Ctrl_Byte0, 0);

        //waiting for events
        do
        {
            event = ITE_WaitEvent(ITE_EVT_ISPCTL_GAMMA_EVENTS);
            ITE_traceISPCTLevent(event);
            if (event.type == ITE_EVT_ISPCTL_ERROR)
            {
                LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
                MMTE_TEST_SKIPPED();
                return;
            }
        } while (!(event.ispctlInfo.info_id == ISP_HR_GAMMA_UPDATE_COMPLETE));

        //to check whether status coin is equal to control coin or not after gamma update
        if(ITE_readPE(CE_GammaStatus_0_e_Coin_Status_Byte0) == ITE_readPE(CE_GammaControl_0_e_Coin_Ctrl_Byte0))
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }

    if(1 == ITE_readPE(DataPathControl_e_Flag_Pipe1Enable_Byte0))  //pipe 1 is active
    {
        snprintf(mess, sizeof(mess), "[Usecase] \nVALUE OF CURVE %ld\n",curve_no);
        MMTE_TEST_COMMENT(mess);
        ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0, curve_no);
        ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_Sharp_Byte0, 0);
        ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_UnSharp_Byte0, 0);

        status_coin = ITE_readPE(CE_GammaStatus_1_e_Coin_Status_Byte0);

        if(status_coin == 0)    //toggling the coin
            ITE_writePE(CE_GammaControl_1_e_Coin_Ctrl_Byte0, 1);
        else
            ITE_writePE(CE_GammaControl_1_e_Coin_Ctrl_Byte0, 0);

        //waiting for events
        do
        {
            event = ITE_WaitEvent(ITE_EVT_ISPCTL_GAMMA_EVENTS);
            ITE_traceISPCTLevent(event);
            if (event.type == ITE_EVT_ISPCTL_ERROR)
            {
                LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
                MMTE_TEST_SKIPPED();
                return;
            }
        } while (!(event.ispctlInfo.info_id == ISP_LR_GAMMA_UPDATE_COMPLETE));

        //to check whether status coin is equal to control coin or not after gamma update
        if(ITE_readPE(CE_GammaStatus_1_e_Coin_Status_Byte0) == ITE_readPE(CE_GammaControl_1_e_Coin_Ctrl_Byte0))
        {
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_FAILED();
        }
    }


#else

//for combined testing of gamma and zoom to find latency
    if((1 == ITE_readPE(DataPathControl_e_Flag_Pipe0Enable_Byte0)) && (1 == ITE_readPE(DataPathControl_e_Flag_Pipe1Enable_Byte0)))  //pipe 0 is active
    {
        //For HR
        LOS_Log("\n>>>VALUE OF CURVE for HR %d\n",curve_no);
        ITE_writePE(CE_GammaControl_0_e_GammaCurve_Byte0, curve_no);
        ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_Sharp_Byte0, 0);
        ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_UnSharp_Byte0, 0);

        status_coin = ITE_readPE(CE_GammaStatus_0_e_Coin_Status_Byte0);

        if(status_coin == 0)   //code for toggling the coin
            ITE_writePE(CE_GammaControl_0_e_Coin_Ctrl_Byte0, 1);
        else
            ITE_writePE(CE_GammaControl_0_e_Coin_Ctrl_Byte0, 0);


        //for LR
        LOS_Log("\n>>>VALUE OF CURVE  for LR %d\n",curve_no);
        ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0, curve_no);
        ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_Sharp_Byte0, 0);
        ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_UnSharp_Byte0, 0);

        status_coin = ITE_readPE(CE_GammaStatus_1_e_Coin_Status_Byte0);

        if(status_coin == 0)    //toggling the coin
            ITE_writePE(CE_GammaControl_1_e_Coin_Ctrl_Byte0, 1);
        else
            ITE_writePE(CE_GammaControl_1_e_Coin_Ctrl_Byte0, 0);


        //zoom test
        ITE_NMF_DZ_Set(ITE_ConvToFloat("1.1"));


        //waiting for events HR
        do
        {
            event = ITE_WaitEvent(ITE_EVT_ISPCTL_GAMMA_EVENTS);
            ITE_traceISPCTLevent(event);
            if (event.type == ITE_EVT_ISPCTL_ERROR)
            {
                LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
                break;
            }
        } while (!(event.ispctlInfo.info_id == ISP_HR_GAMMA_UPDATE_COMPLETE));

        //waiting for events LR
        do
        {
            event = ITE_WaitEvent(ITE_EVT_ISPCTL_GAMMA_EVENTS);
            ITE_traceISPCTLevent(event);
            if (event.type == ITE_EVT_ISPCTL_ERROR)
            {
                LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
                break;
            }
        } while (!(event.ispctlInfo.info_id == ISP_LR_GAMMA_UPDATE_COMPLETE));

    }
#endif

}




#if defined(__PEPS8500_SIA)

/************************************************/

/* ITE_postBootPE_PEPS              */

/*                                  */

/************************************************/

//Rough Post Boot PE to start working on PEPS
void
ITE_postBootPE_PEPS(void)
{
    volatile float  data;
    float           G_maxpixrate;
    float           prescale;
    float           coef;
    float           lineLenght,
                    framesize;
    volatile float  framerate;

#   define S861_DEVICE_ID  0x20
    /*Hack for the 861 sensor, without this hack, the 861 stream at 80ms !!! */

    //FIXME: to be removed on PEPS
    SensorWrite(S861_DEVICE_ID, 0x3822, 1, 0x0003);

    /* set private_pll_div according to to System_clock_div set by XP70 */
    ITE_writePE(VideoTimingSensorConstraints_u16_MinimumVTSysClockDiv_Byte0, 2);
    ITE_writePE(VideoTimingSensorConstraints_u16_MaximumVTSysClockDiv_Byte0, 2);
    SensorWrite(S861_DEVICE_ID, 0x3803, 1, 0x11);

    ITE_writePE(
    Exposure_AlgorithmControls_e_Exposure_AlgorithmMode_Control_Byte0,
    Exposure_AlgorithmMode_e_COMPILED_MANUAL);
    data = 90000;
    ITE_writePE(Exposure_AlgorithmStatus_f_DesiredExposureTime_us_Byte0, *( volatile t_uint32 * ) &data);

    //No prescale: arbitrary choice
    data = 5;
    ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, ( char ) data);

    //FIXME: check framerate; here correspond to a static configuration
    data = DS_HOSTRXMAXDATARATE;
    G_maxpixrate = data / (0x0A0A & 0x00ff);

    prescale = 1;
    coef = 0.8;
    lineLenght = ITE_readPE(SensorFrameConstraints_u16_MinVTLineLengthPck_Byte0);   /*Downscale */

    /*Framesizemin = minLineLength * (sensor.y + minFrameBlanking) */
    framesize = lineLenght *
        (
            ITE_readPE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0) / prescale + ITE_readPE(
                SensorFrameConstraints_u16_MinVTFrameBlanking_Byte0)
        );
    framerate = coef * (G_maxpixrate * 1000000) / framesize;

    ITE_writePE(FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &framerate);
    ITE_writePE(FrameRateControl_f_UserMinimumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &framerate);
    data = ITE_readPE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0) - ((prescale + 3) * 8);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &data);

    //enable accurate manual framerate
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);
    ITE_writePE(VideoTimingHostInputs_e_DeratingRoundingMode_RoundingMode_Byte0, DeratingRoundingMode_e_ROUND_UP);  /*For 8 bits */

    //FIXME: can be removed on PEPS
    data = 100.0;
    ITE_writePE(Exposure_CompilerControls_f_ColdStartDesiredTime_us_Byte0, *( volatile t_uint32 * ) &data);         /*For reduce simulation time */
}


#endif

/************************************************/

/* ITE_postBootPE_TLM               */

/*                      */

/************************************************/

//Rough Post Boot PE to starITE_postBoott working on PEPS
void
ITE_postBoot(void)
{
    //volatile float          data = 1;   //,toto,toto1;
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = NULL;
    t_uint32                outpuModeCount = 0;
    t_uint32                sensorOutputModeDataSize = 0;
    t_uint32                sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);

    // <Hem>
    // this parameter should be initialized to zero by default by grab component itself. So that there is no API break in week 1137 grab component
    // but it is being initialized to 0. So set it to 1, so that BMS works fine.
    // [FIDO ER : 369188] BMS not working in ite_nmf in week 37 environment.
    grabparamsBMS.keepRatio = 1;

    //LOS_Log(">>>IN POST BOOT FUNCTION\n");
    //float lineLenght,framerate,framesize;
    //Sudeep: Allocate all required buffers in the beginning itself
    ITE_NMF_create_Streaming_Config_Data_Buffer(pOutpuModeBuffer);
    //LOS_Log("\n>>>Streaming_Config_Data_Buffer allocated\n");

    ITE_NMF_createFocusStatsBuffer(pFocusStatsBuffer, sizeof(AFStats_Statistics_ts));
    //LOS_Log("\n>>>createFocusStatsBuffer allocated\n");

    ITE_NMF_createFocusStatsBuffer(pAfFrameStatusBuffer, sizeof(FrameParamStatus_Af_ts));
    //LOS_Log("\n>>>createFocusStatsBuffer allocated\n");

    ITE_NMF_createStatsBuffer(pStatsBuffer);
    //LOS_Log("\n>>>createStatsBuffe allocated\n");

    ITE_NMF_createGammaBuffer(pGammaLUTBuffer);  //shikhar
    //LOS_Log("\n>>>createGammaBuffer allocated\n");

    ITE_NMF_createNVMBuffer(pNVMStatsBuffer);
    //LOS_Log("\n>>>createNVMBuffer allocated\n");


#if defined(_SVP_)
    (*(( volatile unsigned short * ) (smia_base_address + 0x54070))) = 1;
    (*(( volatile unsigned int * ) (smia_base_address + 0x56000))) = 0x02a; //
#endif

    //MAx prescale : arbitrary choice
    /*
    if (ite_sensorselect == 1)  //secondary camera
    {
        if (SensorsInfo[ite_sensorselect].revision.identity == 558)
        {
            data = 1;
        }


        if (SensorsInfo[ite_sensorselect].revision.identity == 851)
        {
            data = 4;
        }


        if (SensorsInfo[ite_sensorselect].revision.identity == 754)
        {
            data = 4;
        }
    }
    else
    {
        if (usecase.flag_backup_fw == 1)
        {
            data = 5;           // maxprescale set to 5 for backup firmare
        }
        else
        {
            data = 2;           //maxprescale set to 2 for LLD firmware
        }
    }
   */

    // Initializing PE's defined in CPrimarySensor and CSecondaySensor
    ITE_DoPostBootConfiguration();
    // Initialize sensor output modes related information in a buffer after Boot
    // so that it can be accessed anytime after boot.
    ITE_NMF_Get_SensorOutputMode_Data();

    p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) pOutpuModeBuffer->logAddress;
    sensorOutputModeDataSize = ITE_readPE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);
    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format == PreBoot.sensor.uwCsiRawFormat_MSByte)
        {
            LOS_Log("Default Sensor Mode Selected = %d \n", outpuModeCount);
            break;
        }
    }


    // Set BMS usecase as par selected sensor output mode
    usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[0].u32_woi_res_width;
    usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[0].u32_woi_res_height;

    usecase.BMS_YSize = p_Sensor_Output_Mode_Data[0].u32_output_res_height;
    usecase.BMS_XSize = p_Sensor_Output_Mode_Data[0].u32_output_res_width;

    usecase.framerate_x100 = p_Sensor_Output_Mode_Data[0].u32_max_frame_rate_x100;
    usecase.data_format = p_Sensor_Output_Mode_Data[0].u32_data_format;

    // ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, ( char ) data);

    /// Initialize sensor output modes related information in a buffer after Boot
    /// so that it can be accessed anytime after boot.
    //  ITE_NMF_Get_SensorOutputMode_Data();
    //FIXME: check framerate; here correspond to a static configuration
    //Request framerate
    ITE_SetFrameRateControl(DEFAULT_FPS, DEFAULT_FPS);

#if defined(_SVP_)
    ITE_writePE(VideoTimingHostInputs_e_DeratingRoundingMode_RoundingMode_Byte0, DeratingRoundingMode_e_ROUND_UP);// For 8 bits

    //FIXME: can be removed on PEPS
    data = 100.0;
    ITE_writePE(Exposure_CompilerControls_f_ColdStartDesiredTime_us_Byte0, *( volatile t_uint32 * ) &data);// For reduce simulation time
#endif

    // patch sensor if necessary (sequence from .ini)
    ITE_SendByteToSensorStr(SensorsInfo[ite_sensorselect].handling.initSequence);
}


/************************************************/

/* ITE_modecheck                                                         */

/*  checks the number of modes supported                       */

/************************************************/
void
ITE_modecheck(void)
{
    t_uint16    totalmodes;
    char        mess[256],
                pathname[256],
                filename[256];

    if (usecase.sensor == 0)
    {
        sprintf(filename, "Cam0_");
    }
    else
    {
        sprintf(filename, "Cam1_");
    }


    strcat(filename, "modecheck");
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/modecheck");
    strcat(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    mmte_testStart(filename, "modecheck", g_out_path);

    //reading total number of modes supported
    totalmodes = ITE_readPE(ReadLLAConfig_Status_u16_number_of_modes_Byte0);
    LOS_Log("TOTAL NUMBER OF MODES SUPPORTED BY THE SENSOR IS :%d\n", totalmodes);

    sprintf(mess, "Testing for total number of modes %d\n", totalmodes);
    mmte_testNext(mess);

    // AG TODO: Start using the buffer above
#if 0
    ts_bmsmodeinfo  bmsmodeinfo;
    t_uint16        mode_id;
    for (mode_id = 0; mode_id < totalmodes; mode_id++)
    {
        bmsmodeinfo = ITE_getmode(mode_id);

        sprintf(
        mess,
        "MODE:%d\t WOI: %d x %d Output: %d x %d Max fps_x100:%d \n",
        mode_id,
        bmsmodeinfo.woi_sizeX,
        bmsmodeinfo.woi_sizeY,
        bmsmodeinfo.out_sizeX,
        bmsmodeinfo.out_sizeY,
        bmsmodeinfo.maxfps_x100);
        MMTE_TEST_COMMENT(mess);
    }


#else
    ITE_NMF_Print_SensorOutputMode_Data(( unsigned int * ) (pOutpuModeBuffer->logAddress));
#endif
    mmte_testResult(TEST_PASSED);
    mmte_testEnd();
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


#if 0

/************************************************/

/* ITE_modecheck                                                         */

/*  Function to return a particular mode                       */

/************************************************/
ts_bmsmodeinfo
ITE_getmode(
t_uint16    mode_id)
{
    t_uint32        control_coin,
                    status_coin;
    ts_bmsmodeinfo  bmsmodeinfo;

    //volatile t_uint16 no_of_events_recv;
    ITE_writePE(ReadLLAConfig_Control_u16_SelectMode_Byte0, mode_id);

    // ITE_RefreshEventCount(Event0_Count_u16_EVENT0_30_Sensor_Output_Mode_Export_Notification_Byte0);
    control_coin = !(ITE_readPE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0));
    ITE_writePE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0, control_coin);

    do
    {
        status_coin = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);
        LOS_Sleep(10);
    } while (status_coin != control_coin);

    bmsmodeinfo.maxfps_x100 = ITE_readPE(ReadLLAConfig_Status_u16_max_frame_rate_x100_Byte0);
    bmsmodeinfo.woi_sizeX = ITE_readPE(ReadLLAConfig_Status_u16_woi_res_width_Byte0);
    bmsmodeinfo.woi_sizeY = ITE_readPE(ReadLLAConfig_Status_u16_woi_res_height_Byte0);
    bmsmodeinfo.out_sizeX = ITE_readPE(ReadLLAConfig_Status_u16_output_res_width_Byte0);
    bmsmodeinfo.out_sizeY = ITE_readPE(ReadLLAConfig_Status_u16_output_res_height_Byte0);

    //bmsmodeinfo.data_format = ITE_readPE(ReadLLAConfig_Status_e_DataFormat_SensorMode_Byte0);
    return (bmsmodeinfo);
}


#endif

/************************************************************************************************************************/

/* ITE_Load_FW                                                    */

/* Upto    */

/* WARNING:  */

/************************************************************************************************************************/
void
ITE_Load_FW(
tps_sia_usecase p_sia_usecase)
{
    if (TRUE == ITE_register_sia_components())
    {
        /* Log error messages if it is not possible */
        LOS_Log("Unable to instantiate and bind grab components\n", NULL, NULL, NULL, NULL, NULL, NULL);
        exit(1);
    }


    // include ITE_ISP_PowerOn : binary load and boot ewarp (set PC=0)
    // Send PreBoot page element sequence to ispctl then launch a "BOOT" ewarp command through page element
    if (ITE_ISP_Boot(p_sia_usecase->sensor, p_sia_usecase->flag_backup_fw, 0, 0))
    {
        exit(1);
    }
}


void ITE_TestValidFrameFlag()
{
    Coin_te  control_coin,
             status_coin;

    t_uint32 count1, count2,
             frame_start, frame_end;
    t_uint8  validFrameNotifyEnabled, grab_mode;

    // 0 - false     1 - true
    for(validFrameNotifyEnabled = 0; validFrameNotifyEnabled <= 1; validFrameNotifyEnabled++)
    {
        // 0 -NORMAL    1 - FORCE_OK    2 - FORCE_NOK
        for(grab_mode = 0; grab_mode <= 2; grab_mode++)
        {
            ITE_writePE(SystemSetup_e_Flag_Request_ValidFrameNotification_Ctrl_Byte0, validFrameNotifyEnabled);
            ITE_writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, grab_mode);

            // Toggle the System coin
            control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
            status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
            if (control_coin == status_coin)
            {
                LOS_Log("\nToggling Coin..\n");
                ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
            }
            else
            {
                LOS_Log("\nWarning:coins in unexpected state. Could be because ite_bms_start() got called.\n");
            }

            ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

            frame_start = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
            count1 = ITE_readPE(Event0_Count_u16_EVENT0_31_Valid_Frame_Notification_Byte0);

            LOS_Sleep(1000);
            frame_end = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
            count2 = ITE_readPE(Event0_Count_u16_EVENT0_31_Valid_Frame_Notification_Byte0);
            LOS_Log("\n>>>Value of count1 and count2 are %d   %d\n", count1, count2);

            if (Flag_e_TRUE == validFrameNotifyEnabled)
            {
                // Here we are expecting all frames coming with Valid frame notification
                if ((grab_mode == GrabMode_e_FORCE_OK) && (count2 - count1 == frame_end - frame_start))
                {
                    LOS_Log("\nTEST PASSED\n");
                }
                // In Force_NOK, no valid frame notification will come for any frame as f/w dont send it for NOK frames
                // In Normal mode, because we are not toggeling system coin, so no valid frame notification will come.
                else if ((grab_mode == GrabMode_e_NORMAL || grab_mode == GrabMode_e_FORCE_NOK) || (count2 - count1 == 0))
                {
                    LOS_Log("\nTEST PASSED\n");
                }
                else
                {
                    LOS_Log("\nTEST FAILED\n");
                }
            }
            // In this case flag is zero, so firmware won't send any valid frame notification in any of the cases.
            else if (Flag_e_FALSE == validFrameNotifyEnabled)
            {
                if (count2 - count1 == 0)
                {
                    LOS_Log("\nTEST PASSED\n");
                }
                else
                {
                   LOS_Log("\nTEST FAILED\n");
                }
            }
        }
    }

    // Stop always in FORCE_OK mode
    ITE_writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, 1);    // FORCE_0K mode

    // Toggle the System coin
    control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    if (control_coin == status_coin)
    {
        LOS_Log("\nToggling Coin..\n");
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
    }
    else
    {
        LOS_Log("\nWarning:coins in unexpected state. Could be because ite_bms_start() got called.\n");
    }
    ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

}


#if defined(_SVP_)

/************************************************/

/* ITE_postBootPE_TLM               */

/*                      */

/************************************************/

//Rough Post Boot PE to start working on PEPS
void
ITE_postBootPE_TLM(void)
{
    volatile float  data;
    (*(( volatile unsigned short * ) (smia_base_address + 0x54070))) = 1;
    (*(( volatile unsigned int * ) (smia_base_address + 0x56000))) = 0x02a; //

    //MAx prescale : arbitrary choice
    data = 5;
    ITE_writePE(HostFrameConstraints_u8_MaximumPreScale_Byte0, ( char ) data);

    //FIXME: check framerate; here correspond to a static configuration
    //Request framerate
    data = 5;   //to get a 2fps on TLM...
    ITE_writePE(FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &data);
    ITE_writePE(FrameRateControl_f_UserMinimumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &data);

    //Set Full FOV
    //data = 2048;
    data = ITE_readPE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &data);

    //enable accurate manual framerate
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);
    ITE_writePE(VideoTimingHostInputs_e_DeratingRoundingMode_RoundingMode_Byte0, DeratingRoundingMode_e_ROUND_UP);  /*For 8 bits */

    //FIXME: can be removed on PEPS
    data = 100.0;
    ITE_writePE(Exposure_CompilerControls_f_ColdStartDesiredTime_us_Byte0, *( volatile t_uint32 * ) &data);         /*For reduce simulation time */
}


#endif

