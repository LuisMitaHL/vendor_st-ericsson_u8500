/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_sia_bootcmd.h"
#include <stdio.h>


#include "ite_event.h"
#include "ispctl_types.idt.h"
#include "ite_sensorinfo.h"
#include "ite_pipeinfo.h"
#include "ite_vpip.h"
#include "ite_convf900.h"
#include "ite_sia_interface_data.h"
#include "ite_main.h"

#include <los/api/los_api.h>
#include <cm/inc/cm_macros.h>

//For Linux
#include "MMIO_Camera.h"

#ifdef __TUNING_FILES_AVAILABLE__
#include "tuningloader_sync.h"
#endif

#include <Isp8500_primary_fw_split.h>
#include <Isp8500_primary_fw_ext_ddr.h>
#include <Isp8500_primary_fw_ext.h>
#include <Isp8500_primary_fw_data.h>
#include <Isp8500_secondary_fw_split.h>
#include <Isp8500_secondary_fw_ext_ddr.h>
#include <Isp8500_secondary_fw_ext.h>
#include <Isp8500_secondary_fw_data.h>
#include <primary_sensor.h>
#include <secondary_sensor.h>


#define DDR_PRGM_MEM_256_KB 1

extern tps_siaMetaDatabuffer  pTraceBuffer;

/********************************************************/
/* 		Private functions declaration 		*/
/********************************************************/
int ITE_ISP_PowerOn(int timeout, int flag_backup_fw);

int ITE_Loadfirmware_Split(void);
int ITE_Loadfirmware_Data(void);
int ITE_Loadfirmware_Ext(void);
int ITE_Loadbackupfirmware(void);


/********************************************************/
/* 		"Public" functions 	 		*/
/********************************************************/
ts_preboot PreBoot;
extern ts_sensInfo SensorsInfo[2];
t_uint32 ite_sensorselect;
extern t_uint8 ispctl_clientId;

extern t_uint32 prcmu_base_address;
extern t_uint32 smia_base_address;
extern t_uint8 ITE_Event_Log_flag;

extern t_cm_domain_id domainId;

volatile ts_siasensortunningbuffer   	sensortunningBuffer;
volatile tps_siasensortunningbuffer  	psensortunningBuffer = (tps_siasensortunningbuffer) & sensortunningBuffer;

extern ts_sia_usecase usecase;
t_cm_memory_handle esramXP70_fw;
t_cm_memory_handle sdramXP70_fw;
/************************************************/
/* Int ITE_ISP_Boot(ccpselect)			*/
/* Prepare PreBoot Table 		 +	*/
/* and send BOOT CMD to Host interface		*/
/************************************************/
int ITE_ISP_Boot(int sensorselect, int flag_backup_fw, int smiapp_power_sequence, int sensor_tunning_before_boot)
{
  UNUSED(sensorselect);
  int irp_poweron_timeout = 10000;// 10secs
  int Error = 0,status;
  char mess[256],filename[50],pathname[255],sensorname[20];
  union u_ITE_Event event;
  //struct s_CsiConfig Primary_interface_csi;
  long long int time1,time2;
  snprintf(mess,sizeof(mess),IMAGING_PATH"/ite_nmf/test_results_nreg/performance");
  status = mkdir(mess, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status == -1)
  {
   if (EEXIST != errno)
   	{
    LOS_Log("\nERROR FOLDER NOT CREATED, ERRORNO:%d\n",errno);
	}
   }
  else
  {
  LOS_Log("\nFOLDER %s CREATED !!\n",mess);
  }

  if (usecase.sensor==0)
  {
   sprintf(sensorname,"Cam0_");
  }
 else
  {
   sprintf(sensorname,"Cam1_");
  }
  snprintf(filename,sizeof(filename),"Test_Boot_Time_%s",sensorname);
  snprintf(pathname,sizeof(pathname),IMAGING_PATH"/ite_nmf/test_results_nreg/performance");
  strcat(g_out_path,pathname);
  mmte_testStart(filename,"Testing_Boot_Time",g_out_path);

  ITE_InitMMDSPTimer();

  //load ewarp FW file and launch a "initEwarp" command to ispctl component
  if (ITE_ISP_PowerOn(irp_poweron_timeout, flag_backup_fw))
  {
       exit(1);
  }


#ifdef FS2_DEBUGGER
  ILOS_enableAlternateGpio(GPIO_PIN_0, ILOS_GPIO_ALTERNATE_C);
  ILOS_enableAlternateGpio(GPIO_PIN_1, ILOS_GPIO_ALTERNATE_C);
  ILOS_enableAlternateGpio(GPIO_PIN_2, ILOS_GPIO_ALTERNATE_C);
  ILOS_enableAlternateGpio(GPIO_PIN_3, ILOS_GPIO_ALTERNATE_C);
  ILOS_enableAlternateGpio(GPIO_PIN_4, ILOS_GPIO_ALTERNATE_C);
  ILOS_enableAlternateGpio(GPIO_PIN_20, ILOS_GPIO_ALTERNATE_C);
#endif

ITE_NMF_createTraceBuffer(pTraceBuffer);

    LOS_Log("ISP retrieved version: HW %i, FW_%i.%i.%i_SENSOR_%i_LLA_%i.%i_LLCD_%i.%i\n",
    (void *)ITE_readPE(DeviceParameters_u32_DeviceId_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_FirmwareVersionMajor_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_FirmwareVersionMinor_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_FirmwareVersionMicro_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_LLA_Sensor_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_LLA_MajorVersion_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_LLA_MinorVersion_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_LLCD_MajorVersion_Byte0),
    (void *)ITE_readPE(DeviceParameters_u32_LLCD_MinorVersion_Byte0));

 //Temporary Patch to support primary camera in CCP mode on SVP
 #if defined(_SVP_)
 interface_Type = SensorType_CCP;
 Secondary_interface_Ccp.Ccp_Static_BPP = 0x0A;
 Secondary_interface_Ccp.Ccp_Static_Data_Strobe = 0x01; //0x01=data clock 0x00=data strobe
 NMFCALL (ispctlConfigure, Secondary_Interface_Configuration)(interface_Type, Secondary_interface_csi, Secondary_interface_Ccp);
#endif

 ITE_DoPreBootConfiguration();

 if ( g_nvm_raw_data == 1)
 {
     LOS_Log("NVM_RAW = %d enabled!!\n",(int)g_nvm_raw_data);
    ITE_writePE(Sensor_Tuning_Control_e_TypeNVMExport_Byte0,TypeNVMExport_e_Raw);
 }
 else
 {
   LOS_Log("NVM_RAW = %d disabled!!\n",(int)g_nvm_raw_data);
 }

   mmte_testNext("Checking for BOOT TIME\n");
  time1 = LOS_getSystemTime();

  smiapp_power_sequence = ITE_readPE(SensorPowerManagement_Control_e_Flag_EnableSMIAPP_PowerUpSequence_Byte0);
  sensor_tunning_before_boot = ITE_readPE(Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0);
  Error = ITE_DoBootSequence(smiapp_power_sequence, sensor_tunning_before_boot);

  time2 = LOS_getSystemTime();
  LOS_Log("time for BOOT Cmd = %llu - %llu = %llu us\n",time2, time1, time2-time1);
  snprintf(mess,sizeof(mess),"time for BOOT Cmd = %llu us\n",time2-time1);
  mmte_testComment(mess);

  if (FALSE == g_test_perf_status)
  	{
  	  mmte_testComment("Test Failed in Performance\n");
	  mmte_testResult(TEST_FAILED);
  	}
  else
  	{
  	   mmte_testComment("Test PASSED\n");
  	   mmte_testResult(TEST_PASSED);
  	}

   mmte_testEnd();
   memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) ); 

 // known issue when Tosh boot
 if (Error == ISP_MASTER_I2C_ACCESS_FAILURE) {
   do
   {
	   event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
	   if(ITE_Event_Log_flag == TRUE) ITE_traceISPCTLevent(event);
   }
   while( !((event.type == ITE_EVT_ISPCTL_INFO) && (event.ispctlInfo.info_id == ISP_BOOT_COMPLETE)) );
   Error = 0;
 }

 LOS_Log("BOOT COMAND sent to ewarp!\n", NULL, NULL, NULL, NULL, NULL, NULL);
 return(Error);
}

/******************************************************************************
* \brief   Convert T1 to arm address and set sia_mem_page register
* \author  Denis Hory
* @param   t1_add: T1 addesse space
* @return  arm address
* @note    used to acces in indirect sia isp mem space
*******************************************************************************/
t_uint32 t1_to_arm(t_uint32 t1_addr){

    #define SIA_ISP_MEM 0x56000

    static t_uint16  mem_page = 0;
    t_uint16 mem_page_update = 0;

    mem_page_update = (t1_addr >>13) & 0x3F;

    if (mem_page_update != mem_page)
    { /*Update sia_mem_page register */
        //MMSIA_R_W(ISP_MEM_PAGE, mem_page_update);
        (*((volatile unsigned short *)(smia_base_address+0x54070))) = mem_page_update;
        mem_page = mem_page_update;
    }

    return (SIA_ISP_MEM+(t1_addr&0x00001fff));
}

/********************************************************/
/* 		"Private" functions 	 		*/
/********************************************************/

/********************************************************/
/*  int ITE_ISP_PowerOn(int timeout, char *version)	*/
/*  load Ewarp fw and boot ewarp (set PC = 0)		*/
/********************************************************/

int ITE_ISP_PowerOn(int timeout, int flag_backup_fw)
{
  t_uint16 error=TRUE;
  UNUSED(flag_backup_fw);

  /* mle if(flag_backup_fw==1)
   {
   	if(ITE_Loadbackupfirmware()!=0) return 1;
   }
   else*/
   {
   	//************************************
	//high 64kB of XP70 code inside eSRAM
	//************************************
   	//*********************************************
	//low 64kB of XP70code inside MCU XP70 L2 PSRAM
	//*********************************************
   	//*********************************************
	//low 64kB of XP70code inside MCU XP70 L2 PSRAM
	//*********************************************
   	if(ITE_Loadfirmware_Data()!=0) return 1;
   }

   NMFCALL(ispctlCommand, initISP)(ispctl_clientId);
   error = ITE_WaitEventTimeout(ITE_EVT_ISPCTL_INFO, timeout);

   if (error==TRUE)
   {
          LOS_Log("timeout waiting for IRP to boot\n", NULL, NULL, NULL, NULL, NULL, NULL);
          return 1;
   }

   return 0;
}

//************************************
//free eSRAM
//************************************
void ITE_Freefirmware_Ext(void)
{
   t_cm_error cmError = CM_OK;
   if(esramXP70_fw!=0) cmError=CM_FreeMpcMemory( esramXP70_fw );
   if(sdramXP70_fw!=0) cmError=CM_FreeMpcMemory( sdramXP70_fw );
   if (cmError != CM_OK) LOS_Log("\nERROR freeing Freefirmware_Ex buffer !!! \n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

#if DDR_PRGM_MEM_256_KB
//*******************************
//low data in TCDM memory of XP70
//*******************************
int ITE_Loadfirmware_Data(void)
{
   t_los_memory_handle mem_handle1, mem_handle2;
   t_los_logical_address logicalAddr_split, logicalAddr_data;


   t_uint8* p_logicalAddr_data = NULL;
   t_uint8* p_logicalAddr_split = NULL;
   t_uint8* p_logicalAddr_ext = NULL;
   t_uint8* p_logicalAddr_ddr = NULL;
   t_uint32 u32_size_data = 0;
   t_uint32 u32_size_split = 0;
   t_uint32 u32_size_ext  = 0;
   t_uint32 u32_size_ddr = 0;

//   t_uint32 XP70_fw_buffer_Size=128*1024;
   t_uint32 XP70_fw_buffer_Size = 64*1024; //fix for grab cache error
   t_uint32 XP70_fw_buffer_Size_in_ddr= 2048*1024; //128*1024;
   t_cm_system_address XP70_fw_start_Addr, XP70_fw_start_Addr_ddr;
   t_cm_error cmError;

   OMX_ERRORTYPE error;

   #define pictor_in_XP70_L2_MEM_BASE_ADDR    (0x40000)
   #define pictor_in_XP70_TCDM_MEM_BASE_ADDR  (0x60000)

    if(0==usecase.sensor)
    {
        p_logicalAddr_data = (t_uint8 *)Isp8500_primary_fw_data;
        u32_size_data = ISP8500_PRIMARY_FW_DATA_SIZE;

        p_logicalAddr_split = (t_uint8 *)Isp8500_primary_fw_split;
        u32_size_split = ISP8500_PRIMARY_FW_SPLIT_SIZE;

        p_logicalAddr_ext = (t_uint8 *)Isp8500_primary_fw_ext;
        u32_size_ext  = ISP8500_PRIMARY_FW_EXT_SIZE;

        p_logicalAddr_ddr = (t_uint8 *)Isp8500_primary_fw_ext_ddr;
        u32_size_ddr = ISP8500_PRIMARY_FW_EXT_DDR_SIZE;
    }
    else
    {
        p_logicalAddr_data = (t_uint8 *)Isp8500_secondary_fw_data;
        u32_size_data = ISP8500_SECONDARY_FW_DATA_SIZE;

        p_logicalAddr_split = (t_uint8 *)Isp8500_secondary_fw_split;
        u32_size_split = ISP8500_SECONDARY_FW_SPLIT_SIZE;

        p_logicalAddr_ext = (t_uint8 *)Isp8500_secondary_fw_ext;
        u32_size_ext  = ISP8500_SECONDARY_FW_EXT_SIZE;

        p_logicalAddr_ddr = (t_uint8 *)Isp8500_secondary_fw_ext_ddr;
        u32_size_ddr = ISP8500_SECONDARY_FW_EXT_DDR_SIZE;

    }


    //Memory allocation
    mem_handle1=LOS_Alloc(u32_size_split, 32, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    logicalAddr_split = LOS_GetLogicalAddress(mem_handle1) ;

    mem_handle2=LOS_Alloc(u32_size_data, 8192, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    logicalAddr_data = LOS_GetLogicalAddress(mem_handle2) ;

    //Allocation for ESRAM
    cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_ESRAM16, XP70_fw_buffer_Size, CM_MM_ALIGN_NONE, &esramXP70_fw);
    if (cmError != CM_OK) LOS_Log("\nERROR allocating grab cache buffer\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    cmError=CM_GetMpcMemorySystemAddress (esramXP70_fw, &XP70_fw_start_Addr);
    if (cmError != CM_OK) LOS_Log("\nERROR retrieving grab cache buffer address\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);

    //Allocation for SDRAM
    cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_SDRAM16, XP70_fw_buffer_Size_in_ddr, CM_MM_ALIGN_NONE, &sdramXP70_fw);
    if (cmError != CM_OK) LOS_Log("\nERROR allocating grab cache buffer\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    cmError=CM_GetMpcMemorySystemAddress (sdramXP70_fw, &XP70_fw_start_Addr_ddr);
    if (cmError != CM_OK) LOS_Log("\nERROR retrieving grab cache buffer address\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);

    memcpy((void *)logicalAddr_data, (void *)p_logicalAddr_data, u32_size_data);
    memcpy((void *)logicalAddr_split, (void *)p_logicalAddr_split, u32_size_split);
    memcpy((void *) (XP70_fw_start_Addr.logical + (((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000 + 0x00001000) - XP70_fw_start_Addr.physical)), (void *)p_logicalAddr_ext, u32_size_ext);
    memcpy((void *) (XP70_fw_start_Addr_ddr.logical + (((XP70_fw_start_Addr_ddr.physical & 0xFFF00000) + 0x00100000) - XP70_fw_start_Addr_ddr.physical)), (void *)p_logicalAddr_ddr, u32_size_ddr);


    MMIO_Camera::initMMDSPTimer();
    LOS_Log("Transferring XP70 firmware data\n");

    error = MMIO_Camera::loadXP70FW(
    (void*)((XP70_fw_start_Addr_ddr.physical & 0xFFF00000) + 0x00100000), (OMX_U32)u32_size_ddr,
    (void*)((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000 + 0x00001000), (OMX_U32)u32_size_ext,
    (void*)logicalAddr_split , (OMX_U32) u32_size_split,
    (void*)logicalAddr_data, (OMX_U32)u32_size_data);

    if (error)
     {
        return 1;
     }

  LOS_Log("Done uploading XP70 firmware data\n");
  //Free the memory allocations
  LOS_Free(mem_handle1);
  LOS_Free(mem_handle2);

  return(0);
}


#else

//*******************************
//low data in TCDM memory of XP70
//*******************************
int ITE_Loadfirmware_Data(void)
{

   t_physical_address irp_fw_addr;

   t_los_memory_handle mem_handle1, mem_handle2, mem_handle3, mem_handle4;
   t_los_logical_address logicalAddr_split, logicalAddr_data, logicalAddr_ext, logicalAddr_ddr;
   t_cm_physical_address physicalAddr, physicalAddr2;
   UNUSED(logicalAddr_ddr);

   t_uint32 XP70_fw_buffer_Size=128*1024;
   t_uint32 XP70_fw_buffer_Size_in_ddr=128*1024;
   t_cm_system_address XP70_fw_start_Addr, XP70_fw_start_Addr_ddr;
   t_cm_error cmError;
 //  t_cm_domain_id domainId, domainId2;
 //  t_cm_domain_memory mydomain, mydomain2;

  UNUSED(irp_fw_addr);
  UNUSED(physicalAddr);
  UNUSED(physicalAddr2);


   OMX_ERRORTYPE error;

   t_uint32 i, offset;
   t_uint32 itval;
   #define pictor_in_XP70_L2_MEM_BASE_ADDR    (0x40000)
   #define pictor_in_XP70_TCDM_MEM_BASE_ADDR  (0x60000)

   UNUSED(i);
   UNUSED(itval);
   UNUSED(offset);

   size_t irp_fw_split_size, irp_fw_data_size, irp_fw_ext_size, irp_fw_ddr_size;
   t_los_file *fd_split, *fd_data, *fd_ext, *fd_ddr;
   size_t read;
   char filename_split[256], filename_data[256], filename_ext[256], filename_ddr[256];
   //snprintf(filename, 256, "too""/vpip8820_fw.bin");

   #ifdef ANDROID
   if(ite_sensorselect==0) //Tosh sensor, ite_ccpselect initialized through ITE_InitUseCase() call
    {

			snprintf(filename_split, sizeof(filename_split),"/system/bin/Isp8500_primary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), "/system/bin/Isp8500_primary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), "/system/bin/Isp8500_primary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), "/system/bin/Isp8500_primary_fw_ext_ddr.bin");
 	 }
    else
    {

            LOS_Log("!!!!!!load xp70 LLD Isp8500_smia_fw(data + split + ext).bin from the file system !!!!!!!!\n",NULL,NULL,NULL,NULL,NULL,NULL);
			snprintf(filename_split, sizeof(filename_split), "/system/bin/Isp8500_secondary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), "/system/bin/Isp8500_secondary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), "/system/bin/Isp8500_secondary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), "/system/bin/Isp8500_secondary_fw_ext_ddr.bin");
    }

	#else

     if(ite_sensorselect==0) //Tosh sensor, ite_ccpselect initialized through ITE_InitUseCase() call
    {

#ifndef __ARM_SYMBIAN
			snprintf(filename_split, sizeof(filename_split), FW_LLA_ISP_ROOT_DIR"/Isp8500_primary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), FW_LLA_ISP_ROOT_DIR"/Isp8500_primary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), FW_LLA_ISP_ROOT_DIR"/Isp8500_primary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), FW_LLA_ISP_ROOT_DIR"/Isp8500_primary_fw_ext_ddr.bin");
#else
			snprintf(filename_split, sizeof(filename_split), "f:/imaging/Isp8500_primary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), "f:/imaging/Isp8500_primary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), "f:/imaging/Isp8500_primary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), "f:/imaging/Isp8500_primary_fw_ext_ddr.bin");
#endif //__ARM_SYMBIAN
 	 }
    else
    {
            LOS_Log("!!!!!!load xp70 LLD Secondary Isp8500_smia_fw(data + split + ext).bin from the file system !!!!!!!!\n",NULL,NULL,NULL,NULL,NULL,NULL);
#ifndef __ARM_SYMBIAN
			snprintf(filename_split, sizeof(filename_split), FW_LLA_ISP_ROOT_DIR"/Isp8500_secondary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), FW_LLA_ISP_ROOT_DIR"/Isp8500_secondary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), FW_LLA_ISP_ROOT_DIR"/Isp8500_secondary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), FW_LLA_ISP_ROOT_DIR"/Isp8500_secondary_fw_ext_ddr.bin");
#else
			snprintf(filename_split, sizeof(filename_split), "f:/imaging/Isp8500_secondary_fw_split.bin");
			snprintf(filename_data, sizeof(filename_data), "f:/imaging/Isp8500_secondary_fw_data.bin");
			snprintf(filename_ext, sizeof(filename_ext), "f:/imaging/Isp8500_secondary_fw_ext.bin");
			snprintf(filename_ddr, sizeof(filename_ddr), "f:/imaging/Isp8500_secondary_fw_ext_ddr.bin");
#endif //__ARM_SYMBIAN
    }
#endif //ANDROID


		//Open the FW binary file
	fd_split = LOS_fopen(filename_split, "rb");
	if (fd_split == NULL)
	{
		LOS_Log("unable to open ISP firmware file %s\n", (void*)filename_split, NULL, NULL, NULL, NULL, NULL);
		return 1;
	}
	fd_data = LOS_fopen(filename_data, "rb");
	if (fd_data == NULL)
	{
		LOS_Log("unable to open ISP firmware file %s\n", (void*)filename_data, NULL, NULL, NULL, NULL, NULL);
		return 1;
	}

	fd_ext = LOS_fopen(filename_ext, "rb");
	if (fd_ext == NULL)
	{
		LOS_Log("unable to open ISP firmware file %s\n", (void*)filename_ext, NULL, NULL, NULL, NULL, NULL);
		return 1;
	}

	fd_ddr = LOS_fopen(filename_ddr, "rb");
	if (fd_ddr == NULL)
	{
		LOS_Log("unable to open ISP firmware file %s\n", (void*)filename_ddr, NULL, NULL, NULL, NULL, NULL);
		return 1;
	}

	   //Get the sizes of the binary files
	LOS_fseek(fd_split, 0, LOS_SEEK_END);
	irp_fw_split_size = (size_t)LOS_ftell(fd_split);
	LOS_fseek(fd_split, 0, LOS_SEEK_SET);
	LOS_Log("firmware split size is %i bytes\n",  (void*)irp_fw_split_size, NULL, NULL, NULL, NULL, NULL);

	LOS_fseek(fd_data, 0, LOS_SEEK_END);
	irp_fw_data_size = (size_t)LOS_ftell(fd_data);
	LOS_fseek(fd_data, 0, LOS_SEEK_SET);
	LOS_Log("firmware data size is %i bytes\n",  (void*)irp_fw_data_size, NULL, NULL, NULL, NULL, NULL);

	LOS_fseek(fd_ext, 0, LOS_SEEK_END);
	irp_fw_ext_size = (size_t)LOS_ftell(fd_ext);
	LOS_fseek(fd_ext, 0, LOS_SEEK_SET);
	LOS_Log("firmware ext size is %i bytes\n",  (void*)irp_fw_ext_size, NULL, NULL, NULL, NULL, NULL);

	LOS_fseek(fd_ddr, 0, LOS_SEEK_END);
	irp_fw_ddr_size = (size_t)LOS_ftell(fd_ddr);
	LOS_fseek(fd_ddr, 0, LOS_SEEK_SET);
	LOS_Log("firmware ddr size is %i bytes\n",  (void*)irp_fw_ddr_size, NULL, NULL, NULL, NULL, NULL);

	//Memory allocation
	mem_handle1=LOS_Alloc(irp_fw_split_size, 32, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	logicalAddr_split = LOS_GetLogicalAddress(mem_handle1) ;

	mem_handle2=LOS_Alloc(irp_fw_data_size, 8192, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	logicalAddr_data = LOS_GetLogicalAddress(mem_handle2) ;

	mem_handle3=LOS_Alloc(irp_fw_ext_size, 256, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	logicalAddr_ext = LOS_GetLogicalAddress(mem_handle3) ;

	mem_handle4=LOS_Alloc(irp_fw_ddr_size, 256, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	// Not used so not allocating
	//logicalAddr_ddr = LOS_GetLogicalAddress(mem_handle4) ;

	//Allocation for ESRAM
	cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_ESRAM16, XP70_fw_buffer_Size, CM_MM_ALIGN_NONE, &esramXP70_fw);
	if (cmError != CM_OK) LOS_Log("\nERROR allocating grab cache buffer\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	cmError=CM_GetMpcMemorySystemAddress (esramXP70_fw, &XP70_fw_start_Addr);
	if (cmError != CM_OK) LOS_Log("\nERROR retrieving grab cache buffer address\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);

	//if FW in 4 parts, the ext file size is different than 64k
	//mle if (irp_fw_ext_size != 64*1024){
		memcpy((void *) (XP70_fw_start_Addr.logical + (((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000 + 0x00001000) - XP70_fw_start_Addr.physical)),\
			(void *) logicalAddr_ext,\
			irp_fw_ext_size);
	/*}
	else{
		memcpy((void *) (XP70_fw_start_Addr.logical + (((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000) - XP70_fw_start_Addr.physical)),\
			(void *) logicalAddr_ext,\
			irp_fw_ext_size);
	}*/


	//Allocation for SDRAM
	cmError=CM_AllocMpcMemory(domainId, CM_MM_MPC_SDRAM16, XP70_fw_buffer_Size_in_ddr, CM_MM_ALIGN_NONE, &sdramXP70_fw);
	if (cmError != CM_OK) LOS_Log("\nERROR allocating grab cache buffer\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	cmError=CM_GetMpcMemorySystemAddress (sdramXP70_fw, &XP70_fw_start_Addr_ddr);
	if (cmError != CM_OK) LOS_Log("\nERROR retrieving grab cache buffer address\n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);

	memcpy((void *) (XP70_fw_start_Addr_ddr.logical + (((XP70_fw_start_Addr_ddr.physical & 0xFFFF0000) + 0x00010000) - XP70_fw_start_Addr_ddr.physical)),\
			(void *) logicalAddr_ext,\
			irp_fw_ext_size);



	//Check the read bytes
	read = LOS_fread((void *)logicalAddr_data, 1, irp_fw_data_size, fd_data);
	if (read != irp_fw_data_size)
	{
		LOS_Log("Read %i bytes instead of %i for data FW\n", (void*)read, (void*)irp_fw_data_size, NULL, NULL, NULL, NULL);
		return 1;
	}

	read = LOS_fread((void *)logicalAddr_split, 1, irp_fw_split_size, fd_split);
	if (read != irp_fw_split_size)
	{
		LOS_Log("Read %i bytes instead of %i for split FW\n", (void*)read, (void*)irp_fw_split_size, NULL, NULL, NULL, NULL);
		return 1;
	}

	//if FW in 4 parts, the ext file size is different than 64k
	if (irp_fw_ext_size != 64*1024){
		read = LOS_fread((void *) (XP70_fw_start_Addr.logical + (((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000 + 0x00001000) - XP70_fw_start_Addr.physical)), 1, irp_fw_ext_size, fd_ext);
	}
	else{
		read = LOS_fread((void *) (XP70_fw_start_Addr.logical + (((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000) - XP70_fw_start_Addr.physical)), 1, irp_fw_ext_size, fd_ext);
	}

	if (read != irp_fw_ext_size)
	{
		LOS_Log("Read %i bytes instead of %i for ext FW\n", (void*)read, (void*)irp_fw_ext_size, NULL, NULL, NULL, NULL);
		return 1;
	}

	read = LOS_fread((void *) (XP70_fw_start_Addr_ddr.logical + (((XP70_fw_start_Addr_ddr.physical & 0xFFFF0000) + 0x00010000) - XP70_fw_start_Addr_ddr.physical)), 1, irp_fw_ddr_size, fd_ddr);
	if (read != irp_fw_ddr_size)
	{
		LOS_Log("Read %i bytes instead of %i for ddr FW\n", (void*)read, (void*)irp_fw_ddr_size, NULL, NULL, NULL, NULL);
		return 1;
	}


   //End of file access
   LOS_fclose(fd_split);
   LOS_fclose(fd_data);
   LOS_fclose(fd_ext);
   LOS_fclose(fd_ddr);

   LOS_Log("Transferring XP70 firmware data\n");
   //offset = 0;

   MMIO_Camera::initMMDSPTimer();

   //if FW in 4 parts, the ext file size is different than 64k
   if (irp_fw_ext_size != 64*1024){
      error = MMIO_Camera::loadXP70FW(
	(void*)((XP70_fw_start_Addr_ddr.physical & 0xFFFF0000) + 0x00010000), (OMX_U32)irp_fw_ddr_size,
	(void*)((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000 + 0x00001000), (OMX_U32)irp_fw_ext_size,
	(void*)logicalAddr_split , (OMX_U32) irp_fw_split_size,
	(void*)logicalAddr_data, (OMX_U32)irp_fw_data_size);

   }
   else{
      error = MMIO_Camera::loadXP70FW(
	(void*)((XP70_fw_start_Addr_ddr.physical & 0xFFFF0000) + 0x00010000), (OMX_U32)irp_fw_ddr_size,
	(void*)((XP70_fw_start_Addr.physical & 0xFFFF0000) + 0x00010000), (OMX_U32)irp_fw_ext_size,
	(void*)logicalAddr_split , (OMX_U32) irp_fw_split_size,
	(void*)logicalAddr_data, (OMX_U32)irp_fw_data_size);
   }

if (error)
	   return 1;



  LOS_Log("Done uploading XP70 firmware data\n");
	  //Free the memory allocations
  LOS_Free(mem_handle1);
  LOS_Free(mem_handle2);
  LOS_Free(mem_handle3);
  LOS_Free(mem_handle4);

  return(0);
}

#endif /* DDR_PRGM_MEM_256_KB */

#ifdef __TUNING_FILES_AVAILABLE__

t_los_memory_handle ITE_FirmwareTuning(void) {

t_camera_info caminfo;
t_fw_blocks_info fwBlocks;

//unsigned int aDestBufferSize= 4096;

t_uint16 totalsubblockID;
t_uint8 i;
t_uint8 Maxtabidx=64;
t_uint16 subblockId_tab[Maxtabidx];  // Max size = ???
t_uint16 iCurrentFwSubBlockIndex;
t_tuningloader_sync_error_code tlmErr;

//t_los_memory_handle mem_handle=0;
//t_los_logical_address logicalAddr;

for(i=0;i<Maxtabidx;i++)
 fwBlocks.iSubBlockId[i]   = 0;

totalsubblockID = ITE_readPE(Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0);
//statuscoinsubblock = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);

if (totalsubblockID > 0) {
	LOS_Log(" Tuning supported by the firmware: %d FwSubBlocks supported\n",totalsubblockID);
	// construct SubBlock table ID
	for (iCurrentFwSubBlockIndex=0;iCurrentFwSubBlockIndex<totalsubblockID;iCurrentFwSubBlockIndex++) {
	    ITE_writePE(Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0,iCurrentFwSubBlockIndex);
    	    if (Coin_e_Heads == ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0))
    		{ITE_writePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0,Coin_e_Tails);
	         // wait for coin status toggled
		 do { LOS_Sleep(20); }
		 while (Coin_e_Tails != ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0));
		}
    	    else {ITE_writePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0,Coin_e_Heads);
	         // wait for coin status toggled
		 do { LOS_Sleep(20); }
		 while (Coin_e_Heads != ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0));
	        }
	    subblockId_tab[iCurrentFwSubBlockIndex] = ITE_readPE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0);
	}


	caminfo.manufId = ITE_readPE(SensorInformation_u8_manufacturer_id_Byte0);
	caminfo.modelId = ITE_readPE(SensorInformation_u16_model_id_Byte0);
	caminfo.revNum  = ITE_readPE(SensorInformation_u8_revision_number_Byte0);

	fwBlocks.subBlockCount = totalsubblockID;
	for(i=0;i<Maxtabidx;i++)
		fwBlocks.iSubBlockId[i]   = subblockId_tab[i];

	ITE_NMF_create_SENSOR_TUNNING_Buffer(psensortunningBuffer);

	//Memory allocation
	/*
	mem_handle=LOS_Alloc(aDestBufferSize, 32, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	if(mem_handle == 0) {
		LOS_Log("FirmwareTuning_LoadData: firmware tuning data loading failed: unable to allocate shared memory\n");
		return(0);
		}
	logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
	*/
	tlmErr = LoadFirmwareTuningData( &caminfo,
                         	&fwBlocks,
                         	(void *)(psensortunningBuffer->logAddress),
                         	psensortunningBuffer->size);

	if(tlmErr != TUNINGLOADER_SYNC_OK) {
		LOS_Log("FirmwareTuning_LoadData: firmware tuning data loading failed: %d\n",tlmErr);
		return(0);
		}

	LOS_Log("logical adress sensor tunning : %x\n",psensortunningBuffer->logAddress);
	ITE_writePE(Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte0,psensortunningBuffer->ISPBufferBaseAddress);

	}
else {
	LOS_Log(" Tuning not supported by the firmware\n");
	}
return(psensortunningBuffer->handle);
}
#endif  //__TUNING_FILES_AVAILABLE__


void ITE_DoPreBootConfiguration()
{
    CSensor *iPtr_Sensor = NULL;
    t_uint32 NbOfElementInSensorPreBootTable = 0;
    t_uint32 u32_gpio = 0;
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    ts_PageElement * iPtr_PreBootPeList = NULL;
    LOS_Log(">> ITE_DoPreBootConfiguration\n");
    if(usecase.sensor == 0)
    {
        iPtr_Sensor = new CSensorPrimary();
    }
    else
    {
        iPtr_Sensor = new CSensorSecondary();
    }

    iPtr_PreBootPeList = iPtr_Sensor->GetSensorPreBootTable(&NbOfElementInSensorPreBootTable);
    ITE_writeListPE(iPtr_PreBootPeList, NbOfElementInSensorPreBootTable);

    ret = MMIO_Camera::getCameraFunction(MMIO_Camera::ePrimaryXSD, (unsigned int*)&u32_gpio);
    if (ret != OMX_ErrorNone)
    {
        LOS_Log("ERROR: could not get primary GPIO\n");
        return;
    }

    ITE_writePE(GPIOControl_u8_Sensor0XShutdownGPO_Byte0, u32_gpio);
    ret = MMIO_Camera::getCameraFunction(MMIO_Camera::eSecondaryXSD, (unsigned int*)&u32_gpio);
    if (ret  != OMX_ErrorNone)
    {
        LOS_Log("ERROR: could not get primary GPIO\n");
        return;
    }

    ITE_writePE(GPIOControl_u8_Sensor1XShutdownGPO_Byte0, u32_gpio);

    PreBoot.sensor.uwCsiRawFormat_MSByte = ITE_readPE(VideoTimingHostInputs_u16_CsiRawFormat_Byte0);

    LOS_Log("<< ITE_DoPreBootConfiguration\n");
}



void ITE_DoPostBootConfiguration()
{
    CSensor *iPtr_Sensor = NULL;
    t_uint32 NbOfElementInSensorPostBootTable = 0;
    ts_PageElement * iPtr_PostBootPeList = NULL;

    LOS_Log(">> ITE_DoPostSensorConfiguration\n");
    if(usecase.sensor == 0)
    {
        iPtr_Sensor = new CSensorPrimary();
    }
    else
    {
        iPtr_Sensor = new CSensorSecondary();
    }

    iPtr_PostBootPeList = iPtr_Sensor->GetSensorPostBootTable(&NbOfElementInSensorPostBootTable);
    ITE_writeListPE(iPtr_PostBootPeList, NbOfElementInSensorPostBootTable);
    LOS_Log("<< ITE_DoPostSensorConfiguration\n");
}


#if FLASH_DRIVER_INCLUDE
// ER:408207: Flash driver integration in ITE NMF.

#include "flash_api.h"

//Camera flash handle
CFlashDriver * pFlashDriver = NULL;


//Open the camera flash handle
int ITE_FlashDriverInit(void)
{
	int retVal = 0; /*OK*/;
	TFlashMode modes;
	unsigned long intensity,duration;
	TFlashDetails details;
	TFlashStatus status;
	UNUSED(intensity);
	UNUSED(duration);
	UNUSED(status);
	
	pFlashDriver = CFlashDriver::Open();
	if (NULL == pFlashDriver)
	{
       	LOS_Log("Error: could not open flash driver.\n");
       	retVal = -1;

		goto OUT;
    	}

	//Get the supported flash modes
	pFlashDriver->GetSupportedFlashModes(modes,EPrimary);
	LOS_Log("Supported Modes %x\n",modes);

	pFlashDriver->GetFlashModeDetails(FLASH_MODE_STILL_LED_EXTERNAL_STROBE, details, EPrimary);
	LOS_Log("Mode %x, Intensity(uA):max %d, Min %d, MAX strobe(uSec): %d\n",
		FLASH_MODE_STILL_LED_EXTERNAL_STROBE, details.MaxIntensity,details.MinIntensity,details.MaxStrobeDuration);
	LOS_Log("IP %d, DP %d, TP %d\n",details.IsIntensityProgrammable,
		details.IsDurationProgrammable,details.IsTimeoutProgrammable);

	//Configure flash driver
	ITE_FlashDriverConfigure();
	
OUT:
	return retVal;
}

//Configure flash driver for still LED external strobe mode.
//Trigger by ISP FW
int ITE_FlashDriverConfigure()
{
	int retVal = 0; /*OK*/

	TFlashMode modes;
	unsigned long intensity,duration;
	TFlashDetails details;
	TFlashStatus status;

	UNUSED(modes);
	UNUSED(details);
    
	//Check if flash driver has been initialized or nor
	if (NULL == pFlashDriver)
	{
       	LOS_Log("Error: Flash driver is not initialized.\n");
       	retVal = -1;

		goto OUT;
    	}

	//flash strobing is under fw control, so duration is controlled by fw.
	duration = 0;
	intensity = 0x60; //Hardcoded value taken from OMX traces

	//Enable/Configure flash driver for STILL LED EXTERNAL STROBE mode
	retVal = pFlashDriver->EnableFlashMode(FLASH_MODE_STILL_LED_EXTERNAL_STROBE ,NULL ,NULL, EPrimary);
	retVal = pFlashDriver->ConfigureFlashMode(FLASH_MODE_STILL_LED_EXTERNAL_STROBE,duration,intensity,0,EPrimary);

	retVal = pFlashDriver->GetStatus(FLASH_MODE_STILL_LED_EXTERNAL_STROBE,status,EPrimary);
	LOS_Log("Mode:%x, status %x\n", FLASH_MODE_STILL_LED_EXTERNAL_STROBE, status);
	
OUT:

	return retVal;
}

//Close the camera flash handle
int ITE_FlashDriverDeInit(void)
{
	int retVal = 0; /*OK*/;

	if(pFlashDriver)
		pFlashDriver->Close();

	pFlashDriver = NULL;
	LOS_Log("Flash driver de-init completed. \n");
		
	return retVal;
}

#endif //FLASH_DRIVER_INCLUDE

