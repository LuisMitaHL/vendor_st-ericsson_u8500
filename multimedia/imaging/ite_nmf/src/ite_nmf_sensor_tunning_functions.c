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

#include "ite_nmf_sensor_tunning_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_testenv_utils.h"

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
#include "ite_nmf_sensor_tunning_functions.h"
#include "ite_nmf_headers.h"

#include <cm/inc/cm_macros.h>

#include <test/api/test.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"

/*****************************************/

//CHANGE THIS APPROPRIATELY FOR TESTING DIFFERENT SENSOR_TUNNING DATA
//#include "sensor_tunning_data_with_IQblock.h"
#include "sensor_tunning_data_with_IQblock.h"

/*****************************************/
#define READ_SENSOR_TUNNING_DATA_FROM_FILE    0
#define ISP_REGION_IO               0xE0000000

t_cmd_list  ite_cmd_list_sensor_tunning[] =
{
    { "SENSOR_TUNNING_help", C_ite_dbg_SENSOR_TUNNING_help_cmd, "SENSOR_TUNNING_help: SENSOR_TUNNING_help\n" },
    { "Test_SENSOR_TUNNING_Support", C_ite_dbg_Test_SENSOR_TUNNING_Support_cmd, "Test_SENSOR_TUNNING_Support: Test_SENSOR_TUNNING_Support \n" },
    { "Query_SENSOR_TUNNING_Idx", C_ite_dbg_Query_SENSOR_TUNNING_Idx_cmd,
            "Query_SENSOR_TUNNING_Idx: Query_SENSOR_TUNNING_Idx [<all>|<t_uint16 Index1> <t_uint16 Index2> ...]\n" },
    { "QuerySensorInfo", C_ite_dbg_QuerySensorInfo_cmd, "QuerySensorInfo: QuerySensorInfo\n" },
    { "Configure_SENSOR_TUNNING", C_ite_dbg_Configure_SENSOR_TUNNING_cmd, "Configure_SENSOR_TUNNING: Configure_SENSOR_TUNNING <t_uint8* filename_with_path>\n" }
};

static t_uint16    u16_TotalSubBlockIdsCount = 0;

static t_uint32
ITE_NMF_mapSensorTunningMemArea(
t_uint32    physicalAddr)
{
    t_uint32            sia_base_address;
    t_uint16            value;
    volatile t_uint16   *pReg;

    sia_base_address = LOS_Remap(
        (t_los_physical_address) SIA_MEM_BASE_ADDR,
        (SIA_MEM_END_ADDR - SIA_MEM_BASE_ADDR) + 1,
        LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
    value = (physicalAddr >> 24) << 8;                  // 16 MByte aligned page
    pReg = (t_uint16 *) (sia_base_address + SIA_ISP_MCU_IO_ADDR_0_OFFSET);
    *pReg = value;
    return (physicalAddr & 0x00ffffff) | ISP_REGION_IO; // return the address in the XP70 address space
}


static t_uint8 *
ITE_NMF_createSensorTunningBuffer(
tps_siaMetaDatabuffer   pBufferMetaData)
{
    t_uint8 *psensortunningBufferStorage;
    int     i;

    // NB: Beware that the following layout assumes Stride=1; Can be 1 (planar) or 4 (allow color interleaving).
    int     size = sizeof(PARSED_SENSOR_TUNNING_CONTENT);

    if (pBufferMetaData->handle == 0)
    {
        pBufferMetaData->handle = LOS_Alloc(size, 16, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);

        // pBufferMetaData->phyaddress = ILOS_GetPhysicalAddress(pBufferMetaData->handle);
        pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);
    }
    else
    {
        LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");
    }


    if (pBufferMetaData->handle != 0)
    {   //init buffer: mandatory for buffer fill test
        for (i = 0; i < size; i++)
        {
            *(( t_uint8 * ) pBufferMetaData->logAddress + i) = 0xff;
        }
    }
    else
    {
        LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");
    }


    pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapSensorTunningMemArea(pBufferMetaData->phyaddress);
    LOS_Log("Address of pBufferMetaData->ISPBufferBaseAddress = 0x%x\n", pBufferMetaData->ISPBufferBaseAddress);

    memcpy(( void * ) pBufferMetaData->logAddress, ( void * ) PARSED_SENSOR_TUNNING_CONTENT, size);

    psensortunningBufferStorage = (t_uint8 *) (pBufferMetaData->ISPBufferBaseAddress);

    return (psensortunningBufferStorage);
}


/* -----------------------------------------------------------------------
   FUNCTION : Init_SENSOR_TUNNING_ITECmdList
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
Init_SENSOR_TUNNING_ITECmdList(void)
{
    CLI_register_interface(
    "SENSOR_TUNNING_ITE_CMB",
    sizeof(ite_cmd_list_sensor_tunning) / (sizeof(ite_cmd_list_sensor_tunning[0])),
    ite_cmd_list_sensor_tunning,
    1);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_SENSOR_TUNNING_help_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_SENSOR_TUNNING_help_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);
    
    MMTE_TEST_START(
    "SENSOR_TUNNING_help",
    "/ite_nmf/test_results_nreg/sensor_tunning_tests",
    "SENSOR_TUNNING_help_cmd");
    mmte_testNext(mess);

    if (a_nb_args == 1)
    {

         LOS_Log("TestSensorTunningSupport: Checks whether SENSOR_TUNNING is suppported by the firmware \n");
         LOS_Log("TestSensorTunningSupport[Syntax]: TestSensorTunningSupport \n\n");

         LOS_Log("Query_SENSOR_TUNNING_Idx: Query indexes of sub block Ids\n");
         LOS_Log("Query_SENSOR_TUNNING_Idx[Syntax]: Query_SENSOR_TUNNING_Idx [<all>|<t_uint16 Index1> <t_uint16 Index2> ...]\n\n");

         LOS_Log("QuerySensorInfo: Queries Sensor information \n");
         LOS_Log("QuerySensorInfo[Syntax]: QuerySensorInfo \n\n");

         LOS_Log("Configure_SENSOR_TUNNING: Pass SENSOR_TUNNING data buffer to the firmware\n");
#if READ_SENSOR_TUNNING_DATA_FROM_FILE
         LOS_Log("Configure_SENSOR_TUNNING[Syntax]: Configure_SENSOR_TUNNING <t_uint8* filename_with_path>\n");
         LOS_Log("Configure_SENSOR_TUNNING: filename_with_path is relative to ITE_ROOT_DIR ($(MMROOT)/imaging)\n");
         LOS_Log("Configure_SENSOR_TUNNING[Example]: Configure_SENSOR_TUNNING ite_nmf/testdata/Dummy_SENSOR_TUNNING_Data.bin \n\n");
#else
         LOS_Log("Configure_SENSOR_TUNNING[Syntax]: Configure_SENSOR_TUNNING\n");
#endif
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        return (CMD_ERR_ARGS);
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestSensorTunningSupport_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Test_SENSOR_TUNNING_Support_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    MMTE_TEST_START(
    "TestSensorTunningSupport",
    "/ite_nmf/test_results_nreg/sensor_tunning_tests",
    "Testing for TestSensorTunningSupport");
    mmte_testNext(mess);

    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            LOS_Log("TestSensorTunningSupport: Checks whether SENSOR_TUNNING is suppported by the firmware \n");
            LOS_Log("TestSensorTunningSupport[Syntax]: TestSensorTunningSupport \n\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            u16_TotalSubBlockIdsCount = (t_uint16) ITE_readPE(Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0);
            if (u16_TotalSubBlockIdsCount > 0)
            {
                LOS_Log("*** SENSOR_TUNNING Supported! ***\n");
                LOS_Log("%d Sub-block Ids present.\n", u16_TotalSubBlockIdsCount);
                sprintf(mess,"%d Sub-block Ids present.\n", u16_TotalSubBlockIdsCount);
                mmte_testComment(mess);
                MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
              
                
            }
            else    /*not supported*/
            {
                CLI_disp_error("*** SENSOR_TUNNING not Supported. ***\n");
                MMTE_TEST_FAILED();
                return (CMD_COMPLETE);
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            MMTE_TEST_FAILED();
            return (CMD_ERR_ARGS);
        }
    }
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Query_SENSOR_TUNNING_Idx_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Query_SENSOR_TUNNING_Idx_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint16    sensor_tunning_index,
                sensor_tunning_value,
                sensor_tunning_args;
    t_uint32    control_coin,
                status_coin;
    char        mess[256];

    MMTE_TEST_START("Query_SENSOR_TUNNING_Idx", "/ite_nmf/test_results_nreg/sensor_tunning_tests","Query_SENSOR_TUNNING_Idx");

    sprintf(mess, "Testing for Query_SENSOR_TUNNING_Idx_cmd");
    mmte_testNext(mess);
    if (a_nb_args >= 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("Query_SENSOR_TUNNING_Idx: Query indexes of sub block Ids\n");
            CLI_disp_error("Query_SENSOR_TUNNING_Idx[Syntax]: Query_SENSOR_TUNNING_Idx [<all>|<t_uint16 Index1> <t_uint16 Index2> ...]\n\n");
        }
        else if (0 == strcmp(ap_args[1], "all"))
        {
            if (u16_TotalSubBlockIdsCount > 0)
            {
                LOS_Log("Querying sub block Ids one by one..\n");
                for (sensor_tunning_index = 0; sensor_tunning_index < u16_TotalSubBlockIdsCount; sensor_tunning_index++)
                {
                    ITE_writePE(Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0, (t_uint32) sensor_tunning_index);

                    control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
                    status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);

                    if (control_coin != status_coin)
                    {
                        LOS_Log("Coins not in proper state to execute the command.\n");
                        LOS_Log("Status and Control Coins should be equal before executing the command.\n");

                        MMTE_TEST_FAILED();
                        return (CMD_ERR_GENERAL);
                    }
                    else    //toggle coin
                    {
                        ITE_writePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0, !control_coin);
                    }


                    //wait till the firmware toggles the coin
                    do
                    {
                        control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
                        status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);
                    } while (control_coin != status_coin);

                    sensor_tunning_value = (t_uint16) ITE_readPE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0);
                    LOS_Log("Sub Block Id at %dth index is: 0x%x\n", sensor_tunning_index, sensor_tunning_value);
                    sprintf(mess,"Sub Block Id at %dth index is: 0x%x\n", sensor_tunning_index, sensor_tunning_value);
                    mmte_testComment(mess);
                    
                }
            }
            else            /*not supported*/
            {
                LOS_Log("*** SENSOR_TUNNING not Supported. ***\n");
            }
        }
    }
    else if (a_nb_args > 1)
    {
        for (sensor_tunning_args = 1; sensor_tunning_args < a_nb_args; sensor_tunning_args++)
        {
            sensor_tunning_index = ITE_ConvToInt32(ap_args[sensor_tunning_args]);
            ITE_writePE(Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0, (t_uint32) sensor_tunning_index);

            control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
            status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);

            if (control_coin != status_coin)
            {
                LOS_Log("Coins not in proper state to execute the command.\n");
                LOS_Log("Status and Control Coins should be equal before executing the command.\n");

                MMTE_TEST_FAILED();
                return (CMD_ERR_GENERAL);
            }
            else            //toggle coin
            {
                ITE_writePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0, !control_coin);
            }


            //wait till the firmware toggles the coin
            do
            {
                control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
                status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);
            } while (control_coin != status_coin);

            sensor_tunning_value = (t_uint16) ITE_readPE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0);
            LOS_Log("Sub Block Id at %dth index is: 0x%x\n", sensor_tunning_index, sensor_tunning_value);
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");

        MMTE_TEST_FAILED();
        return (CMD_ERR_ARGS);
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_QuerySensorInfo_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_QuerySensorInfo_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    MMTE_TEST_START(
    "QuerySensorInfo",
    "/ite_nmf/test_results_nreg/sensor_tunning_tests",
    "QuerySensorInfo");

    sprintf(mess, "Testing for QuerySensorInfo");
    mmte_testNext(mess);

    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            LOS_Log("QuerySensorInfo: Queries Sensor information \n");
            LOS_Log("QuerySensorInfo[Syntax]: QuerySensorInfo \n\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            LOS_Log("Reading Sensor Information..\n");
            if (Flag_e_TRUE == ITE_readPE(SensorInformation_e_Flag_Available_Byte0))
            {
                LOS_Log("Model Id = 0x%x\n", (t_uint16) ITE_readPE(SensorInformation_u16_model_id_Byte0));
                LOS_Log("Revision Number = 0x%x\n", (t_uint16) ITE_readPE(SensorInformation_u8_revision_number_Byte0));
                LOS_Log("Manufacture Id = 0x%x\n", (t_uint16) ITE_readPE(SensorInformation_u8_manufacturer_id_Byte0));
                LOS_Log("SMIA Version = 0x%x\n", (t_uint16) ITE_readPE(SensorInformation_u8_smia_version_Byte0));
                sprintf(mess,"Model Id = 0x%x\nRevision Number = 0x%x\nManufacture Id = 0x%x\nSMIA Version = 0x%x\n",(t_uint16) ITE_readPE(SensorInformation_u16_model_id_Byte0),(t_uint16) ITE_readPE(SensorInformation_u8_revision_number_Byte0),(t_uint16) ITE_readPE(SensorInformation_u8_manufacturer_id_Byte0),(t_uint16) ITE_readPE(SensorInformation_u8_smia_version_Byte0));
                mmte_testComment(mess);
            }
            else
            {
                LOS_Log("Sensor Information not available..\n");
                MMTE_TEST_FAILED();
                return (CMD_ERR_GENERAL);
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            MMTE_TEST_FAILED();
            return (CMD_ERR_ARGS);
        }
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Configure_SENSOR_TUNNING_cmd
   PURPOSE  : To be called after sensor boot and before streaming
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Configure_SENSOR_TUNNING_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint8                 *psensortunningBufferForXP70 = NULL;
    char                    mess[256];

#if READ_SENSOR_TUNNING_DATA_FROM_FILE
    t_los_file              *fd = NULL;
    size_t                  read;
    size_t                  size;
    t_los_memory_handle     mem_handle;
    t_los_logical_address   logicalAddr = 0xFFFFFFFF;
#else
    ts_siaMetaDatabuffer    SensorTunningBuffer = { 0,
                            0,
                            0,
                            0,
                            0 };
    tps_siaMetaDatabuffer   psensortunningBuffer = &SensorTunningBuffer;
#endif
    MMTE_TEST_START(
    "Configure_SENSOR_TUNNING",
    "/ite_nmf/test_results_nreg/sensor_tunning_tests",
    "test_SENSOR_TUNNING_Support_cmd");

    sprintf(mess, "Testing for QuerySensorInfo_cmd");
    mmte_testNext(mess);

    if (a_nb_args > 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            LOS_Log("Configure_SENSOR_TUNNING: Pass SENSOR_TUNNING data buffer to the firmware\n");
#if READ_SENSOR_TUNNING_DATA_FROM_FILE
            LOS_Log("Configure_SENSOR_TUNNING[Syntax]: Configure_SENSOR_TUNNING <t_uint8* filename_with_path>\n");
            LOS_Log("Configure_SENSOR_TUNNING: filename_with_path is relative to ITE_ROOT_DIR ($(MMROOT)/imaging)\n");
            LOS_Log("Configure_SENSOR_TUNNING[Example]: Configure_SENSOR_TUNNING ite_nmf/testdata/Dummy_SENSOR_TUNNING_Data.bin \n\n");
#else
            LOS_Log("Configure_SENSOR_TUNNING[Syntax]: Configure_SENSOR_TUNNING\n");
#endif
            MMTE_TEST_PASSED();
            return (CMD_COMPLETE);
        }
    }


#if READ_SENSOR_TUNNING_DATA_FROM_FILE
    if (a_nb_args == 2)
    {
        snprintf(filename, sizeof(filename), ITE_ROOT_DIR "/%s", ap_args[1]);
        fd = LOS_fopen(filename, "rb");
        if (fd == NULL)
        {
            LOS_Log("Unable to open SENSOR_TUNNING file %s\n", ( void * ) filename);

            MMTE_TEST_FAILED();
            return (CMD_ERR_GENERAL);
        }


        LOS_fseek(fd, 0, LOS_SEEK_END);
        size = (size_t) LOS_ftell(fd);
        LOS_fseek(fd, 0, LOS_SEEK_SET);
        LOS_Log("SENSOR_TUNNING file size is %i bytes\n", ( void * ) size);

        mem_handle = LOS_Alloc(size, 4 /*to be verified*/, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
        logicalAddr = LOS_GetLogicalAddress(mem_handle);

        read = LOS_fread(( void * ) logicalAddr, 1, size, fd);
        if (read != size)
        {
            LOS_Log("Read %i bytes instead of %i\n", ( void * ) read, ( void * ) size);
            LOS_Free(mem_handle);
            LOS_fclose(fd);

            MMTE_TEST_FAILED();
            return (CMD_ERR_GENERAL);
        }


        LOS_fclose(fd);

        psensortunningBufferForXP70 = ( t_uint8 * ) logicalAddr;

        //SENSOR_TUNNING buffer ready; so now send its address to firmware
        ITE_writePE(SENSOR_TUNNING_Control_u32_SENSOR_TUNNING_DataBuffer_Byte0, (t_uint32) psensortunningBufferForXP70);
    }


#else
    if (a_nb_args == 1)
    {
        psensortunningBufferForXP70 = ( t_uint8 * ) ITE_NMF_createSensorTunningBuffer(psensortunningBuffer);

        //the below prints are only for testing
        LOS_Log("PARSED_SENSOR_TUNNING_CONTENT[3] = 0x%x\n", PARSED_SENSOR_TUNNING_CONTENT[3]);
        LOS_Log("Address of psensortunningBuffer = 0x%x\n", (t_uint32) psensortunningBufferForXP70);

        //SENSOR_TUNNING buffer ready; so now send its address to firmware
        ITE_writePE(Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte0, (t_uint32) psensortunningBufferForXP70);
    }


#endif
    else
    {
        CLI_disp_error("Not correct command arguments\n");

        MMTE_TEST_FAILED();
        return (CMD_ERR_ARGS);
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_Query_SENSOR_TUNNING_Indices
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
ITE_Query_SENSOR_TUNNING_Indices()
{
    t_uint16    sensor_tunning_index,
                sensor_tunning_value;
    t_uint32    control_coin,
                status_coin;
    t_uint16 totalSubBlockIdsCount;
    
    // MMTE_TEST_START("Query_SENSOR_TUNNING_Idx", "/usr/share/mm-valid/imaging/ite_nmf/test_results_nreg/sensor_tunning_tests","Query_SENSOR_TUNNING_Idx");   

     totalSubBlockIdsCount = (t_uint16) ITE_readPE(Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0);

    if (totalSubBlockIdsCount > 0)
    {
        LOS_Log("Querying sub block Ids one by one..\n");
        for (sensor_tunning_index = 0; sensor_tunning_index < totalSubBlockIdsCount; sensor_tunning_index++)
        {
            ITE_writePE(Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0, (t_uint32) sensor_tunning_index);

            control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
            status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);

            if (control_coin != status_coin)
            {
                LOS_Log("Coins not in proper state to execute the command.\n");
                LOS_Log("Status and Control Coins should be equal before executing the command.\n");
                //  MMTE_TEST_FAILED();
                return (CMD_ERR_GENERAL);
            }
            else    //toggle coin
            {
                ITE_writePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0, !control_coin);
            }


            //wait till the firmware toggles the coin
            control_coin = ITE_readPE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0);
            do
            {
                status_coin = ITE_readPE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0);
            } while (control_coin != status_coin);

            sensor_tunning_value = (t_uint16) ITE_readPE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0);
            LOS_Log("Sub Block Id at %dth index is: 0x%x\n", sensor_tunning_index, sensor_tunning_value);            
            //sprintf(mess,"Sub Block Id at %dth index is: 0x%x\n", sensor_tunning_index, sensor_tunning_value);
            //mmte_testComment(mess);
            }
        }
    else            /*not supported*/
    {
         LOS_Log("*** SENSOR_TUNNING not Supported. ***\n");
    }

    // MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_Configure_SENSOR_TUNNING
   PURPOSE  : To be called after sensor boot and before streaming
   ------------------------------------------------------------------------ */
CMD_COMPLETION ITE_Configure_SENSOR_TUNNING()
{
    t_uint8                 *psensortunningBufferForXP70 = NULL;
    ts_siaMetaDatabuffer    SensorTunningBuffer = { 0,
                        0,
                        0,
                        0,
                        0 };
    tps_siaMetaDatabuffer   psensortunningBuffer = &SensorTunningBuffer;

    //sprintf(mess, "Testing for QuerySensorInfo_cmd");
    //mmte_testNext(mess);

	psensortunningBufferForXP70 = ( t_uint8 * ) ITE_NMF_createSensorTunningBuffer(psensortunningBuffer);

    //the below prints are only for testing
    LOS_Log("PARSED_SENSOR_TUNNING_CONTENT[3] = 0x%x\n", PARSED_SENSOR_TUNNING_CONTENT[3]);
    LOS_Log("Address of psensortunningBuffer = 0x%x\n", (t_uint32) psensortunningBufferForXP70);

    //SENSOR_TUNNING buffer ready; so now send its address to firmware
    ITE_writePE(Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte0, (t_uint32) psensortunningBufferForXP70);

    return (CMD_COMPLETE);
}

