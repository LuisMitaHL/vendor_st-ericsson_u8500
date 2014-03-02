/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_nvm_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_testenv_utils.h"
#include "ite_nmf_standard_functions.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"
#include <test/api/test.h>
#include "ite_nmf_headers.h"
#include "nvm.h"

//#include "ite_nmf_tuning_functions.h"
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"


extern volatile tps_siaNVMbuffer pNVMStatsBuffer;
extern volatile NVMBuffersStorage_t* g_pNVMStatsBufferStorage;
extern ts_sia_usecase usecase;

#define CMD_ERROR       (CMD_COMPLETION) 1
#define ISP_REGION_IO   0xE0000000
#define ARM_REGION_IO   0x01000000

t_cmd_list  ite_cmd_list_nvm[] =
{
    { "NVMhelp", C_ite_dbg_NVMhelp_cmd,
        "NVMhelp: NVMhelp\n"
    },
    { "TestNVMDataSize", C_ite_dbg_TestNVM_Data_Supported_cmd,
        "TestNVMSize: TestNVMSize \n"
    },
    { "TestNVMData", C_ite_dbg_TestNVM_Data_cmd,
        "TestNVMData: TestNVMData \n"
    },
    { "Test_RAW_NVMData", C_ite_dbg_Test_RAW_NVM_Data_cmd,
      "Test_RAW_NVMData: Test_RAW_NVMData \n"
    }
};

CMD_COMPLETION ITE_NMF_Print_NVM_Data(CAM_DRV_NVM_T   *nvmBufferPtr);
CMD_COMPLETION ITE_NMF_Print_RAW_NVM_Data(t_uint8 * nvmBufferPtr);
/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_getNvmDataSize
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32 ITE_NMF_getNvmDataSize(void)
{
    return (ITE_readPE(Sensor_Tuning_Status_u16_NVM_Data_Size_Byte0));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_writeNvmBufferAddress
   PURPOSE  :
   ------------------------------------------------------------------------ */
void ITE_NMF_writeNvmBufferAddress(t_uint32 address)
{
    ITE_writePE(Sensor_Tuning_Control_u32_NVM_Data_Address_Byte0, address);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_toggleNvmControlCoin
   PURPOSE  :
   ------------------------------------------------------------------------ */
void ITE_NMF_toggleNvmControlCoin()
{
  Coin_te control_coin, status_coin;
  
  
  LOS_Log("\nToggling NVM Coin.\n");
  
  control_coin = (Coin_te)ITE_readPE(Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0);
  status_coin  = (Coin_te)ITE_readPE(Sensor_Tuning_Status_e_Coin_NVM_Status_Byte0);

  if (control_coin != status_coin)
  {
    LOS_Log("Coins not in proper state to execute the command.\n");
    LOS_Log("Status and Control Coins should be equal before executing the command.\n");
    return; //error
  }
  else
  {
      ITE_writePE(Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0, !control_coin);
  }
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_checkNvmStatusCoin
   PURPOSE  :
   ------------------------------------------------------------------------ */
void ITE_NMF_checkNvmStatusCoin()
{
    t_uint32    nvmSatus = ITE_readPE(Sensor_Tuning_Status_e_Coin_NVM_Status_Byte0);
    t_uint32    nvmControl = ITE_readPE(Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0);

    while (nvmSatus != nvmControl)
    {
           nvmSatus = ITE_readPE(Sensor_Tuning_Status_e_Coin_NVM_Status_Byte0);
    }
    return;
}


/* -----------------------------------------------------------------------
   FUNCTION : Init_NVM_ITECmdList
   PURPOSE  :
   ------------------------------------------------------------------------ */
void Init_NVM_ITECmdList(void)
{
    CLI_register_interface("NVM_ITE_CMB", sizeof(ite_cmd_list_nvm)/(sizeof(ite_cmd_list_nvm[0])), ite_cmd_list_nvm, 1);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_NVMhelp_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_NVMhelp_cmd(int a_nb_args, char ** ap_args)
{
  UNUSED(ap_args);
  
    LOS_Log("**********************************************************************\n");
    LOS_Log("******************************** NVM HELP *****************************\n");
    LOS_Log("**********************************************************************\n");

    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("TestNVMDataSize: Checks size of NVM data provided by FW \n");
        CLI_disp_error("TestNVMData: Checks NVM data provided by FW \n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    return CMD_ERROR;
    }
    LOS_Log("**********************************************************************\n");
    LOS_Log("******************************** NVM HELP END**************************\n");
    LOS_Log("**********************************************************************\n");

  return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestNVM_Data_Supported_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_TestNVM_Data_Supported_cmd(int a_nb_args, char ** ap_args)
{
    t_uint16    nvmDataSize = 0;
    char        mess[256];

    MMTE_TEST_START("Test_NVM_Data_SUPP","/ite_nmf/test_results_nreg/NVM_tests",
    "TestNVM_Data_SUPPORTED");

    snprintf(mess,sizeof(mess), "Testing for Test_NVM_Data_SUPPORTED");
    mmte_testNext(mess);

    LOS_Log("**********************************************************************\n");
    LOS_Log("********************* NVM DATA SUPPORT TEST **************************\n");
    LOS_Log("**********************************************************************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("TestNVMDataSize: Checks size of NVM data provided by FW \n");
            CLI_disp_error("syntax(TestSensorTunningSupport) : TestNVMDataSize \n\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            nvmDataSize = ITE_NMF_getNvmDataSize();
            if (0 != nvmDataSize)
            {
                LOS_Log("*** NVM data supported by sensor ***\n");
                LOS_Log("nvmDataSize = %d\n", nvmDataSize);
            MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
            }
        }
        else
        {
            LOS_Log("*** NVM data not supported by sensor ***\n");
            MMTE_TEST_SKIPPED();
        }
    }


    LOS_Log("**********************************************************************\n");
    LOS_Log("******************** NVM DATA SUPPORT TEST END************************\n");
    LOS_Log("**********************************************************************\n");
    
    return (CMD_ERROR);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestNVM_Data_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_TestNVM_Data_cmd(int a_nb_args, char ** ap_args)
{
    t_uint32                nvm_data_size = 0;
    t_uint32                 *nvmBufferPtr = NULL;
    char                    mess[256];

    MMTE_TEST_START(
    "TestNVMData","/ite_nmf/test_results_nreg/NVM_tests",
    "Testing for NVM_Data");

    snprintf(mess,sizeof(mess), "Testing for NVM_Data");
    mmte_testNext(mess);

    LOS_Log("**********************************************************************\n");
    LOS_Log("********************* NVM DATA VALIDITY TEST **************************\n");
    LOS_Log("**********************************************************************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("TestNVMData: Checks content of NVM data provided by FW \n");
            CLI_disp_error("syntax(TestNVMData) : TestNVMDataSize \n\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            nvm_data_size = ITE_NMF_getNvmDataSize();
            LOS_Log("NVM data size = %d\n", nvm_data_size);

            if (0 == nvm_data_size)
            {
                LOS_Log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                LOS_Log("!!!!!!!!!!!!!!!!!!! NVM  DATA  NOT  SUPPORTED  !!!!!!!!!!!!!!!!!!!!!!!!\n");
                LOS_Log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                MMTE_TEST_SKIPPED();
                return CMD_ERROR;
            }

            // allocate a buffer..
            //ITE_NMF_createNVMBuffer(pNVMStatsBuffer);
            nvmBufferPtr = (t_uint32*)(g_pNVMStatsBufferStorage->ispNVMAddr);
            LOS_Log("NVM buffer creation done\n");
            // assign data_address value
            ITE_NMF_writeNvmBufferAddress((t_uint32) nvmBufferPtr);
            LOS_Log("NVM buffer address passed to FW\n");
            // toggle nvm coin
            ITE_NMF_toggleNvmControlCoin();
            LOS_Log("NVM coin toggled\n");
            // wait for notification
            ITE_NMF_Wait_For_NVM_Notification();
            LOS_Log("NVM notification recievd\n");

            
         ITE_NMF_Print_NVM_Data((CAM_DRV_NVM_T*)(pNVMStatsBuffer->logAddress));

        }
        else
            {
                LOS_Log("*** Error ***\n");
                MMTE_TEST_FAILED();
                return CMD_ERROR;
             }
  }

    LOS_Log("**********************************************************************\n");
    LOS_Log("******************* NVM DATA VALIDITY TEST END ************************\n");
    LOS_Log("**********************************************************************\n");
    MMTE_TEST_PASSED();
    return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_RAW_NVM_Data_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Test_RAW_NVM_Data_cmd(int a_nb_args, char ** ap_args)
{
    t_uint32                nvm_data_size = 0;
    t_uint32                 *nvmBufferPtr = NULL;
    char                    mess[256];

    MMTE_TEST_START(
    "Test_RAW_NVMData","/ite_nmf/test_results_nreg/NVM_tests",
    "Testing for NVM_Data");

    snprintf(mess,sizeof(mess), "Testing for NVM_Data");
    mmte_testNext(mess);

    LOS_Log("**********************************************************************\n");
    LOS_Log("********************* NVM DATA VALIDITY TEST **************************\n");
    LOS_Log("**********************************************************************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("TestNVMData: Checks content of NVM data provided by FW \n");
            CLI_disp_error("syntax(TestNVMData) : TestNVMDataSize \n\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            nvm_data_size = ITE_NMF_getNvmDataSize();
            LOS_Log("NVM data size = %d\n", nvm_data_size);

            if (0 == nvm_data_size)
            {
                LOS_Log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                LOS_Log("!!!!!!!!!!!!!!!!!!! NVM  DATA  NOT  SUPPORTED  !!!!!!!!!!!!!!!!!!!!!!!!\n");
                LOS_Log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                MMTE_TEST_SKIPPED();
                return CMD_ERROR;
            }

            // allocate a buffer..
            //ITE_NMF_createNVMBuffer(pNVMStatsBuffer);
            nvmBufferPtr = (t_uint32*)(g_pNVMStatsBufferStorage->ispNVMAddr);
            LOS_Log("NVM buffer creation done\n");
            // assign data_address value
            ITE_NMF_writeNvmBufferAddress((t_uint32) nvmBufferPtr);
            LOS_Log("NVM buffer address passed to FW\n");
            // toggle nvm coin
            ITE_NMF_toggleNvmControlCoin();
            LOS_Log("NVM coin toggled\n");
            // wait for notification
            ITE_NMF_Wait_For_NVM_Notification();
            LOS_Log("NVM notification recievd\n");

            
            if  (0 == ITE_readPE(Sensor_Tuning_Control_e_TypeNVMExport_Byte0))
            
             {// print NVM data 
                LOS_Log("PLEASE Configure Raw NVM Data before sensor boot\n");
                //ITE_NMF_Print_NVM_Data((CAM_DRV_NVM_T*)(pNVMStatsBuffer->logAddress));
             }
            else
             {
                LOS_Log("Configure Raw NVM Data before sensor boot in PE :Sensor_Tuning_Control_e_TypeNVMExport_Byte0\n");
                ITE_NMF_Print_RAW_NVM_Data((t_uint8*)(pNVMStatsBuffer->logAddress));
             }        
          

        }
        else
            {
                LOS_Log("*** Error ***\n");
                MMTE_TEST_FAILED();
                return CMD_ERROR;
             }
  }

    LOS_Log("**********************************************************************\n");
    LOS_Log("******************* NVM DATA VALIDITY TEST END ************************\n");
    LOS_Log("**********************************************************************\n");
    MMTE_TEST_PASSED();
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_Test_NVM_Export_Done
   PURPOSE  :
   ------------------------------------------------------------------------ */
 void ITE_NMF_Wait_For_NVM_Notification()
    {
    union u_ITE_Event event;
    do
   {
       event = ITE_WaitEvent(ISP_NVM_EXPORT_DONE_EVENTS);
       ITE_traceISPCTLevent(event);
       if(event.type == ITE_EVT_ISPCTL_ERROR) break;
   }
   while( !(event.ispctlInfo.info_id == ISP_NVM_EXPORT_DONE));

}


CMD_COMPLETION ITE_NMF_Print_RAW_NVM_Data(t_uint8 * nvmBufferPtr)
{
    t_los_file      *nvm_dump;
    t_uint16 SizeOfData, i  = 0;
    char mess[256];
    char Nvm_filename[100]=IMAGING_PATH"/ite_nmf/test_results_nreg/NVM_tests/NVM_Raw_dump";
    static t_uint32 Nb = 0;
    char  extension[8] = "";

    snprintf(extension, sizeof(extension), "%d.txt", ( int ) Nb);
    Nb++;
    strcat(Nvm_filename, extension);

    nvm_dump = LOS_fopen(Nvm_filename, "wb+");

    SizeOfData = ITE_NMF_getNvmDataSize();

    LOS_Log("\n************ PRINTING RAW NVM DATA***************\n");
    LOS_Log("\n Total Size of Raw Data = %u \n", SizeOfData );
    
   snprintf(mess, sizeof(mess), "Index : Value\n");
   LOS_fwrite((t_uint8 *) mess, strlen(mess), 1, nvm_dump);

    while (i < SizeOfData)
    {
      LOS_Log("\n Index: [%u], Value: [%u]\n", i, *(nvmBufferPtr + i));
      snprintf(mess, sizeof(mess), "%u : %u\n", i, *(nvmBufferPtr + i));
      LOS_fwrite((t_uint8 *) mess, strlen(mess), 1, nvm_dump);
       i++;
    }

    LOS_fclose(nvm_dump);
    return CMD_COMPLETE;
}




CMD_COMPLETION ITE_NMF_Print_NVM_Data(CAM_DRV_NVM_T   *nvmBufferPtr)
    {
    CAM_DRV_NVM_LS_TABLE_T              *p_ls_comTable = (CAM_DRV_NVM_LS_TABLE_T  *)NULL;
    CAM_DRV_NVM_LSC_DATA_T                 *p_lsc_data = (CAM_DRV_NVM_LSC_DATA_T *)NULL;

    CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T  *mapped_couplet_type1_map_addr = (CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T  *)NULL;
 // CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T  *mapped_couplet_type2_map_addr = (CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T  *)NULL;
    CAM_DRV_NVM_SENSITIVITY_DATA_T      *mapped_p_sens = (CAM_DRV_NVM_SENSITIVITY_DATA_T  *)NULL;
    uint8*     p_ls_measured_lp_nums;
    t_uint32  u32_CalculatedOffset,k,count = 0;
    char mess[256];

    LOS_Log("\n************ NVM DATA***************\n");

    LOS_Log("\nnvmBufferPtr = %x\n", nvmBufferPtr);
    snprintf(mess,sizeof(mess),"\nnvmBufferPtr = %x\n", (unsigned int)nvmBufferPtr);
    mmte_testComment(mess);
    

    LOS_Log("nvmBufferPtr->sens_resolution.width = %u\n", nvmBufferPtr->sens_resolution.width);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->sens_resolution.width = %u\n", nvmBufferPtr->sens_resolution.width);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->sens_resolution.height = %u\n", nvmBufferPtr->sens_resolution.height);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->sens_resolution.height = %u\n", nvmBufferPtr->sens_resolution.height);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.C_F_object_distances_offset = %u\n", nvmBufferPtr->af_data.C_F_object_distances_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.C_F_object_distances_offset = %u\n", nvmBufferPtr->af_data.C_F_object_distances_offset);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.additional_af_data_offset = %u\n", nvmBufferPtr->af_data.additional_af_data_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.additional_af_data_offset = %u\n", nvmBufferPtr->af_data.additional_af_data_offset);
    mmte_testComment(mess);

    for(count = 0; count < CAM_DRV_NVM_MAX_AF_LENS_POS; count++)
    {
        LOS_Log("nvmBufferPtr->af_data.positions_horizontal[%d] = %u\n", count, nvmBufferPtr->af_data.positions_horizontal[count]);
        snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.additional_af_data_offset = %u\n", nvmBufferPtr->af_data.additional_af_data_offset);
        mmte_testComment(mess);
    
        LOS_Log("nvmBufferPtr->af_data.positions_upward[%lu] = %u\n", count, nvmBufferPtr->af_data.positions_upward[count]);
        snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.positions_upward[%lu] = %u\n", count, nvmBufferPtr->af_data.positions_upward[count]);
        mmte_testComment(mess);
    
        LOS_Log("nvmBufferPtr->af_data.positions_downward[%lu] = %u\n", count, nvmBufferPtr->af_data.positions_downward[count]);\
        snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.positions_downward[%lu] = %u\n", count, nvmBufferPtr->af_data.positions_downward[count]);
        mmte_testComment(mess); 
   }

    LOS_Log("nvmBufferPtr->af_data.obj_dis_availability_bitmask = %u\n", nvmBufferPtr->af_data.obj_dis_availability_bitmask);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.obj_dis_availability_bitmask = %u\n", nvmBufferPtr->af_data.obj_dis_availability_bitmask);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.size_of_additional_af_data = %u\n", nvmBufferPtr->af_data.size_of_additional_af_data);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.size_of_additional_af_data = %u\n", nvmBufferPtr->af_data.size_of_additional_af_data);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.fine_step_im = %u\n", nvmBufferPtr->af_data.fine_step_im);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.fine_step_im = %u\n", nvmBufferPtr->af_data.fine_step_im);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.fine_step_mi = %u\n", nvmBufferPtr->af_data.fine_step_mi);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.fine_step_mi = %u\n", nvmBufferPtr->af_data.fine_step_mi);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.availability_bitmask_h = %u\n", nvmBufferPtr->af_data.availability_bitmask_h);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.availability_bitmask_h = %u\n", nvmBufferPtr->af_data.availability_bitmask_h);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.availability_bitmask_u = %u\n", nvmBufferPtr->af_data.availability_bitmask_u);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.availability_bitmask_u = %u\n", nvmBufferPtr->af_data.availability_bitmask_u);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.availability_bitmask_d = %u\n", nvmBufferPtr->af_data.availability_bitmask_d);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.availability_bitmask_d = %u\n", nvmBufferPtr->af_data.availability_bitmask_d);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.pos_unit_relation_to_cw = %u\n", nvmBufferPtr->af_data.pos_unit_relation_to_cw);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.pos_unit_relation_to_cw = %u\n", nvmBufferPtr->af_data.pos_unit_relation_to_cw);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.total_positions_h = %u\n", nvmBufferPtr->af_data.total_positions_h);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.total_positions_h = %u\n", nvmBufferPtr->af_data.total_positions_h);
    mmte_testComment(mess);
    
    LOS_Log("nvmBufferPtr->af_data.total_positions_u = %u\n", nvmBufferPtr->af_data.total_positions_u);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.total_positions_u = %u\n", nvmBufferPtr->af_data.total_positions_u);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->af_data.total_positions_d = %u\n", nvmBufferPtr->af_data.total_positions_d);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.total_positions_d = %u\n", nvmBufferPtr->af_data.total_positions_d);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->sensitivity_data.sens_offset = %u\n", nvmBufferPtr->sensitivity_data.sens_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->sensitivity_data.sens_offset = %u\n", nvmBufferPtr->sensitivity_data.sens_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->sensitivity_data.sensitivity_colour_temps = %u\n", nvmBufferPtr->sensitivity_data.sensitivity_colour_temps);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->sensitivity_data.sensitivity_colour_temps = %u\n", nvmBufferPtr->sensitivity_data.sensitivity_colour_temps);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->sensitivity_data.padding1 = %u\n", nvmBufferPtr->sensitivity_data.padding1);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->sensitivity_data.padding1 = %u\n", nvmBufferPtr->sensitivity_data.padding1);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->nd_ms_data.shut_delay_us = %u\n", nvmBufferPtr->nd_ms_data.shut_delay_us);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->nd_ms_data.shut_delay_us = %u\n", nvmBufferPtr->nd_ms_data.shut_delay_us);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->nd_ms_data.ND_trans_x100 = %u\n", nvmBufferPtr->nd_ms_data.ND_trans_x100);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->nd_ms_data.ND_trans_x100 = %u\n", nvmBufferPtr->nd_ms_data.ND_trans_x100);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset = %u\n", nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset = %u\n", nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset = %u\n", nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset = %u\n", nvmBufferPtr->spatial_pedestal_data.spatial_pedestal_table_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->spatial_pedestal_data.number_of_tables = %u\n", nvmBufferPtr->spatial_pedestal_data.number_of_tables);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->spatial_pedestal_data.number_of_tables = %u\n", nvmBufferPtr->spatial_pedestal_data.number_of_tables);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->spatial_pedestal_data.padding1 = %u\n", nvmBufferPtr->spatial_pedestal_data.padding1);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->spatial_pedestal_data.padding1 = %u\n", nvmBufferPtr->spatial_pedestal_data.padding1);
    mmte_testComment(mess);


    LOS_Log("nvmBufferPtr->checksum = %lu\n", nvmBufferPtr->checksum);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->checksum = %lu\n", nvmBufferPtr->checksum);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->checksum_calculated = %lu\n", nvmBufferPtr->checksum_calculated);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->checksum_calculated = %lu\n", nvmBufferPtr->checksum_calculated);
    mmte_testComment(mess);


    LOS_Log("nvmBufferPtr->number_of_errors = %u\n", nvmBufferPtr->number_of_errors);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->number_of_errors = %lu\n", nvmBufferPtr->number_of_errors);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->checksum2 = %lu\n", nvmBufferPtr->checksum2);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->checksum2 = %lu\n", nvmBufferPtr->checksum2);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->decoder_status = %lu\n", nvmBufferPtr->decoder_status);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->decoder_status = %lu\n", nvmBufferPtr->decoder_status);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->total_data_size = %lu\n", nvmBufferPtr->total_data_size);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->total_data_size = %lu\n", nvmBufferPtr->total_data_size);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_af_data_object_distances = %u\n", nvmBufferPtr->offset_af_data_object_distances);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_af_data_object_distances = %u\n", nvmBufferPtr->offset_af_data_object_distances);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_af_additional_af_data = %u\n", nvmBufferPtr->offset_af_additional_af_data);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_af_additional_af_data = %u\n", nvmBufferPtr->offset_af_additional_af_data);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_ls_no_of_lens_positions = %u\n", nvmBufferPtr->offset_ls_no_of_lens_positions);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_ls_no_of_lens_positions = %u\n", nvmBufferPtr->offset_ls_no_of_lens_positions);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_com_tables = %u\n", nvmBufferPtr->offset_com_tables);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_com_tables = %u\n", nvmBufferPtr->offset_com_tables);
    mmte_testComment(mess);


    LOS_Log("nvmBufferPtr->offset_ls_lsc_data = %u\n", nvmBufferPtr->offset_ls_lsc_data);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_ls_lsc_data = %u\n", nvmBufferPtr->offset_ls_lsc_data);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_sensitivity_data = %u\n", nvmBufferPtr->offset_sensitivity_data);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_sensitivity_data = %u\n", nvmBufferPtr->offset_sensitivity_data);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_defect_couplet_type1 = %u\n", nvmBufferPtr->offset_defect_couplet_type1);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_defect_couplet_type1 = %u\n", nvmBufferPtr->offset_defect_couplet_type1);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_defect_couplet_type2 = %u\n", nvmBufferPtr->offset_defect_couplet_type2);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_defect_couplet_type2 = %u\n", nvmBufferPtr->offset_defect_couplet_type2);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_defect_line = %u\n", nvmBufferPtr->offset_defect_line);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_defect_line = %u\n", nvmBufferPtr->offset_defect_line);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_pedestal_table = %u\n", nvmBufferPtr->offset_pedestal_table);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->af_data.positions_downward[%lu] = %u\n", count, nvmBufferPtr->af_data.positions_downward[count]);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->offset_pedestal_data = %u\n", nvmBufferPtr->offset_pedestal_data);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->offset_pedestal_table = %u\n", nvmBufferPtr->offset_pedestal_table);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->padding1 = %u\n", nvmBufferPtr->padding1);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->padding1 = %u\n", nvmBufferPtr->padding1);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->padding2 = %u\n", nvmBufferPtr->padding2);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->padding2 = %u\n", nvmBufferPtr->padding2);
    mmte_testComment(mess);


    p_ls_measured_lp_nums = (uint8 *)((uint32)nvmBufferPtr + nvmBufferPtr->offset_ls_no_of_lens_positions);

    LOS_Log("nvmBufferPtr->ls_data.ls_measured_lp_nums_offset = %u\n", nvmBufferPtr->ls_data.ls_measured_lp_nums_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.ls_measured_lp_nums_offset = %u\n", nvmBufferPtr->ls_data.ls_measured_lp_nums_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->ls_data.ls_comTable_offset = %u\n", nvmBufferPtr->ls_data.ls_comTable_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.ls_comTable_offset = %u\n", nvmBufferPtr->ls_data.ls_comTable_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->ls_data.ls_measured_temp_num = %u\n", nvmBufferPtr->ls_data.ls_measured_temp_num);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.ls_measured_temp_num = %u\n", nvmBufferPtr->ls_data.ls_measured_temp_num);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->ls_data.ls_table_num = %u\n", nvmBufferPtr->ls_data.ls_table_num);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.ls_table_num = %u\n", nvmBufferPtr->ls_data.ls_table_num);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->ls_data.padding1 = %u\n", nvmBufferPtr->ls_data.padding1);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.padding1 = %u\n", nvmBufferPtr->ls_data.padding1);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->ls_data.padding2 = %u\n", nvmBufferPtr->ls_data.padding2);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->ls_data.padding2 = %u\n", nvmBufferPtr->ls_data.padding2);
    mmte_testComment(mess);

    //print the No of LP's at different temperatures
    for ( count = 0; count < nvmBufferPtr->ls_data.ls_measured_temp_num; count++)
    {
        LOS_Log("No of LP's at different temperatures\n");
        

        LOS_Log("p_ls_measured_lp_nums[%u] = %u \n", count, p_ls_measured_lp_nums[count]);
        snprintf(mess,sizeof(mess),"p_ls_measured_lp_nums[%lu] = %u \n", count, p_ls_measured_lp_nums[count]);
        mmte_testComment(mess);
    }

    //Print
     p_ls_comTable = (CAM_DRV_NVM_LS_TABLE_T  *) ((uint32)nvmBufferPtr +nvmBufferPtr->offset_com_tables);
     p_lsc_data  =      (CAM_DRV_NVM_LSC_DATA_T *) ((uint32)nvmBufferPtr + nvmBufferPtr->offset_ls_lsc_data);
     
    for ( count = 0; count < nvmBufferPtr->ls_data.ls_table_num; count++)
   {

     LOS_Log("******************LS Com table[%u]***********************\n", count);
     u32_CalculatedOffset =  (nvmBufferPtr->ls_data.ls_table_num - count) * sizeof(p_ls_comTable) + 
                                        (count * sizeof(p_lsc_data) * p_ls_comTable[count].x_division * p_ls_comTable[count].y_division);

      if(u32_CalculatedOffset == p_ls_comTable[count].lsc_data_offset)
      {
         LOS_Log("p_ls_comTable[%u].lsc_data_offset = %u\n", count, p_ls_comTable[count].lsc_data_offset);
         snprintf(mess, sizeof(mess), "p_ls_comTable[%lu].lsc_data_offset = %u\n", count, p_ls_comTable[count].lsc_data_offset);
         mmte_testComment(mess);
      }
      else
      {
     LOS_Log("p_ls_comTable[%u].lsc_data_offset = %u\n", count, p_ls_comTable[count].lsc_data_offset);
        LOS_Log("Offset Value not as Expected... u32_CalculatedOffset  = %u\n", u32_CalculatedOffset);
        snprintf(mess, sizeof(mess), "p_ls_comTable[%lu].lsc_data_offset = %u\n", count, p_ls_comTable[count].lsc_data_offset);
     mmte_testComment(mess);
      }
      

     LOS_Log("p_ls_comTable[%u].x_value_x1000 = %u\n", count, p_ls_comTable[count].x_value_x1000);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].x_value_x1000 = %u\n", count, p_ls_comTable[count].x_value_x1000);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].y_value_x1000 = %u\n", count, p_ls_comTable[count].y_value_x1000);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].y_value_x1000 = %u\n", count, p_ls_comTable[count].y_value_x1000);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].x_division = %u\n", count, p_ls_comTable[count].x_division);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].x_division = %u\n", count, p_ls_comTable[count].x_division);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].y_division = %u\n", count, p_ls_comTable[count].y_division);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].y_division = %u\n", count, p_ls_comTable[count].y_division);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].ch = %u\n", count, p_ls_comTable[count].ch);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].ch = %u\n", count, p_ls_comTable[count].ch);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].center_bit_depth = %u\n", count, p_ls_comTable[count].center_bit_depth);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].center_bit_depth = %u\n", count, p_ls_comTable[count].center_bit_depth);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].other_bit_depth = %u\n", count, p_ls_comTable[count].other_bit_depth);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].other_bit_depth = %u\n", count, p_ls_comTable[count].other_bit_depth);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].object_distance = %u\n", count, p_ls_comTable[count].object_distance);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].object_distance = %u\n", count, p_ls_comTable[count].object_distance);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].zoom_factor = %u\n", count, p_ls_comTable[count].zoom_factor);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].zoom_factor = %u\n", count, p_ls_comTable[count].zoom_factor);
     mmte_testComment(mess);

     LOS_Log("p_ls_comTable[%u].padding1 = %u\n", count, p_ls_comTable[count].padding1);
     snprintf(mess,sizeof(mess),"p_ls_comTable[%lu].padding1 = %u\n", count, p_ls_comTable[count].padding1);
     mmte_testComment(mess);

   
     LOS_Log("******************LSC Grid for table[%u]***********************\n", count);

     // We have to read 63 values in all as there are values for 63 Grids in the NVM map
     for( k=0; k<(p_ls_comTable[count].x_division * p_ls_comTable[count].y_division); k++)
      {

                LOS_Log("p_lsc_data[%u].R = %u\n", k, p_lsc_data[k].r);
                snprintf(mess,sizeof(mess),"p_lsc_data[%lu].R = %u\n", k, p_lsc_data[k].r);
                mmte_testComment(mess);

                LOS_Log("p_lsc_data[%u].B = %u\n", k, p_lsc_data[k].b);
                snprintf(mess,sizeof(mess),"p_lsc_data[%lu].B = %u\n", k, p_lsc_data[k].b);
                mmte_testComment(mess);


                if( LLA_RGB_CALIBRATED ==  p_ls_comTable[count].ch)
                {
                LOS_Log("p_lsc_data[%u].GB = %u\n", k, p_lsc_data[k].gb);
                snprintf(mess,sizeof(mess),"p_lsc_data[%lu].GB = %u\n", k, p_lsc_data[k].gb);
                mmte_testComment(mess);
                }
                else if ( LLA_RGBGrGb_CALIBRATED ==  p_ls_comTable[count].ch)
                {
                LOS_Log("p_lsc_data[%u].GR = %u\n", k, p_lsc_data[k].gr);
                snprintf(mess,sizeof(mess),"p_lsc_data[%lu].GR = %u\n", k, p_lsc_data[k].gr);
                mmte_testComment(mess);

                LOS_Log("p_lsc_data[%u].GB = %u\n", k, p_lsc_data[k].gb);
                snprintf(mess,sizeof(mess),"p_lsc_data[%lu].GB = %u\n", k, p_lsc_data[k].gb);
                mmte_testComment(mess);
                }
      }
        p_lsc_data += (p_ls_comTable[count].x_division * p_ls_comTable[count].y_division);

    }

    LOS_Log("nvmBufferPtr->defect_data.couplet_type1_map_offset = %u\n", nvmBufferPtr->defect_data.couplet_type1_map_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.couplet_type1_map_offset = %u\n", nvmBufferPtr->defect_data.couplet_type1_map_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->defect_data.couplet_type2_map_offset = %u\n", nvmBufferPtr->defect_data.couplet_type2_map_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.couplet_type2_map_offset = %u\n", nvmBufferPtr->defect_data.couplet_type2_map_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->defect_data.line_map_offset = %u\n", nvmBufferPtr->defect_data.line_map_offset);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.line_map_offset = %u\n", nvmBufferPtr->defect_data.line_map_offset);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->defect_data.num_of_couplet_type1_defects = %u\n", nvmBufferPtr->defect_data.num_of_couplet_type1_defects);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.num_of_couplet_type1_defects = %u\n", nvmBufferPtr->defect_data.num_of_couplet_type1_defects);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->defect_data.num_of_couplet_type2_defects = %u\n", nvmBufferPtr->defect_data.num_of_couplet_type2_defects);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.num_of_couplet_type2_defects = %u\n", nvmBufferPtr->defect_data.num_of_couplet_type2_defects);
    mmte_testComment(mess);

    LOS_Log("nvmBufferPtr->defect_data.num_of_line_defects = %u\n", nvmBufferPtr->defect_data.num_of_line_defects);
    snprintf(mess,sizeof(mess),"nvmBufferPtr->defect_data.num_of_line_defects = %u\n", nvmBufferPtr->defect_data.num_of_line_defects);
    mmte_testComment(mess);


    mapped_couplet_type1_map_addr = (CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T *) ((uint32)nvmBufferPtr + nvmBufferPtr->offset_defect_couplet_type1);
    for ( count=0; count< (uint8)(nvmBufferPtr->defect_data.num_of_couplet_type1_defects) ; count++)
    {
       LOS_Log("mapped_couplet_type1_map_addr[%u].x = %u\n", count, mapped_couplet_type1_map_addr[count].x);
       snprintf(mess,sizeof(mess),"mapped_couplet_type1_map_addr[%lu].x = %u\n", count, mapped_couplet_type1_map_addr[count].x);
       mmte_testComment(mess);

       LOS_Log("mapped_couplet_type1_map_addr[%u].y = %u\n", count, mapped_couplet_type1_map_addr[count].y);
       snprintf(mess,sizeof(mess),"mapped_couplet_type1_map_addr[%lu].y = %u\n", count, mapped_couplet_type1_map_addr[count].y);
       mmte_testComment(mess);

    }

    mapped_p_sens  = (CAM_DRV_NVM_SENSITIVITY_DATA_T *) ((uint32)nvmBufferPtr + nvmBufferPtr->offset_sensitivity_data);

    for ( count=0; count < nvmBufferPtr->sensitivity_data.sensitivity_colour_temps; count++)
    {
      LOS_Log("************************ Sensitivity Data **************************\n");
      LOS_Log("mapped_p_sens[%u].x_value_x1000 = %u\n", count, mapped_p_sens[count].x_value_x1000);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].x_value_x1000 = %u\n", count, mapped_p_sens[count].x_value_x1000);
      mmte_testComment(mess);

      LOS_Log("mapped_p_sens[%u].y_value_x1000 = %u\n", count, mapped_p_sens[count].y_value_x1000);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].y_value_x1000 = %u\n", count, mapped_p_sens[count].y_value_x1000);
      mmte_testComment(mess);

      LOS_Log("mapped_p_sens[%u].sens.r = %u\n", count, mapped_p_sens[count].sens.r);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].sens.r = %u\n", count, mapped_p_sens[count].sens.r);
      mmte_testComment(mess);

      LOS_Log("mapped_p_sens[%u].sens.b = %u\n", count, mapped_p_sens[count].sens.b);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].sens.b = %u\n", count, mapped_p_sens[count].sens.b);
      mmte_testComment(mess);

      LOS_Log("mapped_p_sens[%u].sens.gb = %u\n", count, mapped_p_sens[count].sens.gb);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].sens.gb = %u\n", count, mapped_p_sens[count].sens.gb);
      mmte_testComment(mess);

      LOS_Log("mapped_p_sens[%u].sens.gr = %u\n", count, mapped_p_sens[count].sens.gr);
      snprintf(mess,sizeof(mess),"mapped_p_sens[%lu].sens.gr = %u\n", count, mapped_p_sens[count].sens.gr);
      mmte_testComment(mess);
      }

  return CMD_COMPLETE;
}


