/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/
#include "callbacks.h"
#include "CRM.h"
#include "IPPhwReg.h"
#include "GPIOManager_OpInterface.h"
#include "FLADriver.h"
#include "Flash.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_LLA_Abstraction_callbacksTraces.h"
#endif
CAM_DRV_CONFIG_INIT_T   g_callback_fn =
{
    cam_drv_block_alloc_no_wait_fn,
    cam_drv_block_dealloc_fn,
    cam_drv_event_fn,
    cam_drv_msg_send_fn,
    cam_drv_timer_start_fn,
    cam_drv_timer_stop_fn,
    ( void * ) cam_drv_control_physical_pins_fn,
    cam_drv_common_i2c_read_fn,
    cam_drv_common_i2c_write_fn,
    cam_drv_sensor_about_to_stop_fn,
    cam_drv_sensor_started_fn,
    LLA_AbstractionCamDrvIntTimer,  //Callback API for interrupt timer implementation
    cam_drv_blocking_timer_fn,                  // Blocking timer for LLCD
    cam_drv_control_additional_physical_pins_fn // Call back for flash
};

void *
cam_drv_block_alloc_no_wait_fn(
uint32  block_size)
{
    void    *ptr_Memory = NULL;

#if USE_MALLOC_FREE_IMPLEMETATION
#else
    ptr_Memory = malloc(block_size);
    if (NULL == ptr_Memory)
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_block_alloc_no_wait_fn() returned error!!");
#endif
        ASSERT_XP70();
    }


#endif
    return (ptr_Memory);
}


void
cam_drv_block_dealloc_fn(void *p_block)
{
#if USE_MALLOC_FREE_IMPLEMETATION
    // Malloc free implementation should not call free
#else
    free(p_block);
#endif
}


void
cam_drv_event_fn(
CAM_DRV_EVENT_E         event_code,
CAM_DRV_EVENT_INFO_U    *p_event_info,
bool8                   isr_event)
{
#if INCLUDE_FOCUS_MODULES
    /*Handling the FLADriver related Event in the callback funtion */
    if ( (CAM_DRV_AF_LENS_MOVED == event_code)
         ||  (CAM_DRV_AF_LENS_POSITION_MEASURED == event_code)
         ||  (CAM_DRV_LENS_STOPPED == event_code))
    {
        g_FLADriver_LLLCtrlStatusParam.u8_Error = Flag_e_FALSE;

        // [PM]Timer has already stopped. This call is just to disable
        // the timer and update g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type
        if (CAM_DRV_AF_LENS_MOVED == event_code)
        {
            cam_drv_timer_stop_fn(CAM_DRV_INT_TIMER_2);
        }

        FLADriver_LLAEventHandler(event_code, p_event_info, isr_event);

    }
    else if(CAM_DRV_EVENT_ERROR == event_code)
    {

       OstTraceInt0(TRACE_ERROR, "<ERROR>!!Lens movement returned error!!");

       g_FLADriver_LLLCtrlStatusParam.u8_Error = Flag_e_TRUE;

       if ((NULL != p_event_info) && (CAM_DRV_EVENT_ERROR_LENS == p_event_info->error_id))
       {
          OstTraceInt0(TRACE_ERROR, "<ERROR>!!CAM_DRV_EVENT_ERROR_LENS!!");

       }
       else
       {
          OstTraceInt0(TRACE_ERROR, "<ERROR>!!CAM_DRV_EVENT_ERROR but no CAM_DRV_EVENT_ERROR_LENS!!");

       }
    }


#endif //INCLUDE_FOCUS_MODULES
    if
    (
        CAM_DRV_REGISTER_FOR_NEXT_FSC == event_code
    ||  CAM_DRV_REGISTER_FOR_NEXT_FEC == event_code
    ||  CAM_DRV_REGISTER_FOR_FSC == event_code
    ||  CAM_DRV_REGISTER_FOR_FEC == event_code
    ||  CAM_DRV_UNREGISTER_FOR_FSC == event_code
    ||  CAM_DRV_UNREGISTER_FOR_FEC == event_code
    )
    {
        /*
          This callback function is used by LLD for registering with FW for getting notification for various
           events like FEC, FSC, etc
                */
        LLA_AbstractionRegisterReq(event_code);
    }


    //Error Handling for Flash Errors
    if ((NULL != p_event_info) && (CAM_DRV_EVENT_ERROR_FLASH_STROBE == p_event_info->error_id))
    {
        g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_OUT_OF_FLASH_API_CONTEXT;
        Flash_HandleFlashErrors();
    }


    return;
}


CAM_DRV_FN_RETVAL_E
cam_drv_msg_send_fn(
CAM_DRV_MSG_E   msg_id,
void            *p_msg_info,
uint16          msg_size)
{
    CAM_DRV_FN_RETVAL_E retVal = CAM_DRV_OK;

    //store the requested message information in LLA Abstraction layer
    retVal = LLA_Abstracetion_SendDrvMsg(msg_id, p_msg_info, msg_size);

    return (retVal);
}


void
cam_drv_timer_start_fn(
CAM_DRV_OS_TIMER_E  timer_id,
uint32              timer_ticks)
{
}


void
cam_drv_timer_stop_fn(
CAM_DRV_OS_TIMER_E  timer_id)
{
#if INCLUDE_FOCUS_MODULES
    if (timer_id == CAM_DRV_INT_TIMER_2)
    {
        LLA_AbstractionCamDrvIntTimer(CAM_DRV_INT_TIMER_2, 0);
    }


#endif
}


// Blocking timer from LLCD
void
cam_drv_blocking_timer_fn(
uint32  time_in_10us)
{
    OstTraceInt1(TRACE_DEBUG, "cam_drv_blocking_timer_fn = %d", time_in_10us * 10);
    GPIOManager_Delay_us((float_t) time_in_10us * 10);
}


void
cam_drv_control_physical_pins_fn(
CAM_DRV_SELECT_CAMERA_E cam_id,
CAM_DRV_PHYSICAL_PINS_E pin_name,
CAM_DRV_PIN_ACTION_E    action)
{
    OstTraceInt3(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : cam_id = %x, pin_name = %x, action = %x", cam_id, pin_name, action);


    switch (pin_name)
    {
        case CAM_DRV_PHYSICAL_PIN_VOLTAGES:
           if(
               (CAM_DRV_PIN_ACTION_INIT != action) &&
               (Flag_e_TRUE == g_SensorPowerManagement_Control.e_Flag_EnableSMIAPP_PowerUpSequence) &&
               (PowerAction_e_idle == g_SensorPowerManagement_Control.e_PowerAction)
              )
           {
                OstTraceInt1(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_VOLTAGES, action = %x", action);
                g_SensorPowerManagement_Status.u16_VoltageAnalog_x100 = 0;
                g_SensorPowerManagement_Status.u16_VoltageDigital_x100 = 0;
                if(CAM_DRV_PIN_ACTION_ACTIVATE == action)
                    g_SensorPowerManagement_Status.e_PowerCommand = PowerCommand_e_voltage_on;
                else
                     g_SensorPowerManagement_Status.e_PowerCommand = PowerCommand_e_voltage_off;
                g_SensorPowerManagement_Control.e_PowerAction = PowerAction_e_requested;

                EventManager_Power_Notification_Notify();
                while(PowerAction_e_complete != g_SensorPowerManagement_Control.e_PowerAction);
                OstTraceInt0(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_VOLTAGES : while() done");

                if(Flag_e_FALSE == g_SensorPowerManagement_Control.e_Flag_Result)
                {
#if USE_TRACE_ERROR
                     OstTraceInt0(TRACE_ERROR, "<ERROR>!!<CAM_DRV_PHYSICAL_PIN_VOLTAGES>Power-up sequence specified by host is failure in g_SensorPowerManagement_Control.e_Flag_Result!!");
#endif
                     ASSERT_XP70();
                }
                g_SensorPowerManagement_Control.e_PowerAction = PowerAction_e_idle;

            }
           else
            {
                OstTraceInt0(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_VOLTAGES : else");
            }
            break;

        case CAM_DRV_PHYSICAL_PIN_CLOCK:
           if(
               (CAM_DRV_PIN_ACTION_INIT != action) &&
               (Flag_e_TRUE == g_SensorPowerManagement_Control.e_Flag_EnableSMIAPP_PowerUpSequence) &&
               (PowerAction_e_idle == g_SensorPowerManagement_Control.e_PowerAction)
              )
           {
            OstTraceInt1(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_CLOCK, action = %x", action);
                g_SensorPowerManagement_Status.u32_ExternalClockFreq_MHz_x100 = g_SystemSetup.f_SensorInputClockFreq_Mhz;
                if(CAM_DRV_PIN_ACTION_ACTIVATE == action)
                    g_SensorPowerManagement_Status.e_PowerCommand = PowerCommand_e_ext_clk_on;
                else
                     g_SensorPowerManagement_Status.e_PowerCommand = PowerCommand_e_ext_clk_off;
                g_SensorPowerManagement_Control.e_PowerAction = PowerAction_e_requested;

                EventManager_Power_Notification_Notify();
                while(PowerAction_e_complete != g_SensorPowerManagement_Control.e_PowerAction)
                           ;
                OstTraceInt0(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_CLOCK : while() done");
                if(Flag_e_FALSE == g_SensorPowerManagement_Control.e_Flag_Result)
                {
#if USE_TRACE_ERROR
                     OstTraceInt0(TRACE_ERROR, "<ERROR>!!<CAM_DRV_PHYSICAL_PIN_CLOCK>Power-up sequence specified by host is failure in g_SensorPowerManagement_Control.e_Flag_Result!!");
#endif
                     ASSERT_XP70();
                }

                if(CAM_DRV_PIN_ACTION_ACTIVATE == action)
                {
                    MasterI2C_ModeStaticInitialise();
                    OstTraceInt0(TRACE_DEBUG, "done MasterI2C_ModeStaticInitialise()");
                }

                g_SensorPowerManagement_Control.e_PowerAction = PowerAction_e_idle;
            }
           else
           {
                OstTraceInt0(TRACE_DEBUG, "cam_drv_control_physical_pins_fn : CAM_DRV_PHYSICAL_PIN_CLOCK : else");
           }
            break;

        case CAM_DRV_PHYSICAL_PIN_XSHUTDOWN:        // based on the choice of sensor, set the appropriate GPIO pin high
            /* TODO: <LLA> <AG>: Read if there is any init state for XshutDown state and put, currently init is not supported */
            if (CAM_DRV_PIN_ACTION_INIT != action)
            {
                if ((CAM_DRV_CAMERA_PRIMARY == cam_id) && GPIOManager_IsSensor0XShutDownSupported())
                {
                    if (CAM_DRV_PIN_ACTION_ACTIVATE == action)
                    {
                        GPIOManager_AssertSensor0Gpio();
                    }
                    else
                    {
                        GPIOManager_DeAssertSensor0Gpio();
                    }
                }
                else if ((CAM_DRV_CAMERA_SECONDARY == cam_id) && GPIOManager_IsSensor1XShutDownSupported())
                {
                    if (CAM_DRV_PIN_ACTION_ACTIVATE == action)
                    {
                        GPIOManager_AssertSensor1Gpio();
                    }
                    else
                    {
                        GPIOManager_DeAssertSensor1Gpio();
                    }
                }
            }


            break;
        default:

#if USE_TRACE_ERROR
             OstTraceInt0(TRACE_ERROR, "<ERROR>!! cam_drv_control_physical_pins_fn() -> Default case hit, should never come here!!");
#endif
             ASSERT_XP70();
            break;
    }
}



CAM_DRV_FN_RETVAL_E cam_drv_control_additional_physical_pins_fn (CAM_DRV_SELECT_CAMERA_E cam_id, CAM_DRV_OPTIONAL_PHYSICAL_PINS_E pin_name, CAM_DRV_PIN_ACTION_E action)
{
    OstTraceInt3(TRACE_DEBUG, "cam_drv_control_additional_physical_pins_fn : cam_id = %x, pin_name = %x, action = %x", cam_id, pin_name, action);

    if (CAM_DRV_PHYSICAL_PIN_FLASH == pin_name)
    {
        if (CAM_DRV_PIN_ACTION_ACTIVATE == action)
        {
            GPIOManager_TriggerUpFlashOnStreaming();
        }
        else
        {
            GPIOManager_TriggerDownFlashOnStreaming();
        }
    }
    else
    {
#if USE_TRACE_ERROR
         OstTraceInt0(TRACE_ERROR, "<ERROR>!! cam_drv_control_additional_physical_pins_fn() -> else case hit, No Pin except Flash is supported!!");
#endif
         ASSERT_XP70();

    }
    return (CAM_DRV_FN_OK);
}


uint8_t
get_register_address_type(
CAM_DRV_I2C_REG_SIZE_E  reg_addr_size)
{
    if (CAM_DRV_I2C_REG_SIZE_16BIT == reg_addr_size)
    {
        return (DeviceIndex_e_16BitDataIndex);
    }
    else if (CAM_DRV_I2C_REG_SIZE_8BIT == reg_addr_size)
    {
        return (DeviceIndex_e_8BitDataIndex);
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!get_register_address_type(): wrong reg address size, the value should be 8/16!!");
#endif
        ASSERT_XP70();
    }
}


CAM_DRV_FN_RETVAL_E
cam_drv_common_i2c_read_fn(
uint8                       i2c_addr,
uint16                      reg_addr,
CAM_DRV_I2C_REG_SIZE_E      reg_addr_size,
uint16                      nr_of_bytes_to_read,
CAM_DRV_I2C_BYTE_ORDER_E    byte_order,
uint8                       *p_data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint16_t    u16_NoOfBytes;
    uint8_t     e_DeviceAddress_Type;
    uint8_t     e_DeviceIndex_Type;
    uint8_t     u8_NumberOfBytes;
    uint8_t     u8_NumberOfIterations;
    uint8_t     e_Flag_Status = Flag_e_TRUE;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_DeviceAddress_Type = DeviceAddress_e_7BitDeviceAddress;
    e_DeviceIndex_Type = get_register_address_type(reg_addr_size);

    if (nr_of_bytes_to_read)
    {
        // Passed size should not be greater than the hardware fifo size
        u8_NumberOfIterations = (nr_of_bytes_to_read / LLA_MAXIMUM_NO_OF_BYTES_IN_I2C);

        while (u8_NumberOfIterations)
        {
            e_Flag_Status = MasterI2C_ReadNBytes(
                (uint16) i2c_addr,
                reg_addr,
                ( uint8_t * ) p_data,
                LLA_MAXIMUM_NO_OF_BYTES_IN_I2C,
                e_DeviceAddress_Type,
                e_DeviceIndex_Type);

            if (Flag_e_TRUE == e_Flag_Status)
            {
                p_data = p_data + LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
                reg_addr = reg_addr + LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
                u8_NumberOfIterations--;
            }
            else
            {
                // ideally code should not come here
#if USE_TRACE_ERROR
                OstTraceInt0(TRACE_ERROR, "<ERROR>!!1. cam_drv_common_i2c_read_fn() - I2C read Failure!!");
#endif
                ASSERT_XP70();
                break;
            }
        }


        if (Flag_e_TRUE == e_Flag_Status)
        {
            u8_NumberOfBytes = (uint8) nr_of_bytes_to_read % LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
            if (0 != u8_NumberOfBytes)
            {
                e_Flag_Status = MasterI2C_ReadNBytes(
                    (uint16) i2c_addr,
                    reg_addr,
                    ( uint8_t * ) p_data,
                    u8_NumberOfBytes,
                    e_DeviceAddress_Type,
                    e_DeviceIndex_Type);
            }
        }
        else
        {
#if USE_TRACE_ERROR
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!2. cam_drv_common_i2c_read_fn() - I2C read Failure!!");
#endif

            // ideally code should not come here
            ASSERT_XP70();
        }
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_common_i2c_read_fn() - No of bytes to read are 0!!");
#endif

        // ideally code should not come here
        ASSERT_XP70();
        e_Flag_Status = Flag_e_FALSE;
    }


    if (CAM_DRV_I2C_BYTE_ORDER_REVERSED == byte_order)
    {
        if (2 == nr_of_bytes_to_read)
        {
            // Extract Byte 1
            u8_NumberOfIterations = p_data[1];

            // Replace byte 1 with byte 0
            p_data[1] = p_data[0];

            // Replace byte 0 with byte 1
            p_data[0] = u8_NumberOfIterations;
        }
        else if (4 == nr_of_bytes_to_read)
        {
            u8_NumberOfIterations = p_data[3];
            p_data[3] = p_data[0];
            p_data[0] = u8_NumberOfIterations;

            u8_NumberOfIterations = p_data[2];
            p_data[2] = p_data[1];
            p_data[1] = u8_NumberOfIterations;
        }
    }
    else if ((CAM_DRV_I2C_BYTE_ORDER_REVERSED_16BIT == byte_order) && (0 == nr_of_bytes_to_read % 2))
    {
        // 1. code should only execute for 16 bit reversed
        // 2. Only when driver has requested even no of bytes, 16 bit reversed don't make sense for odd reades
        u16_NoOfBytes = nr_of_bytes_to_read / 2;
        while (u16_NoOfBytes)
        {
            // Extract Byte 0
            u8_NumberOfIterations = p_data[2 * (u16_NoOfBytes - 1)];

            // Replace byte 0 with byte 1
            p_data[2 * (u16_NoOfBytes - 1)] = p_data[2 * u16_NoOfBytes - 1];

            // Replace byte 1 with byte 0
            p_data[2 * u16_NoOfBytes - 1] = u8_NumberOfIterations;

            u16_NoOfBytes--;
        }
    }


    return (Flag_e_TRUE == e_Flag_Status ? CAM_DRV_FN_OK : CAM_DRV_FN_FAIL);
}


CAM_DRV_FN_RETVAL_E
cam_drv_common_i2c_write_fn(
uint8                       i2c_addr,
uint16                      reg_addr,
CAM_DRV_I2C_REG_SIZE_E      reg_addr_size,
uint16                      nr_of_bytes_to_write,
CAM_DRV_I2C_BYTE_ORDER_E    byte_order,
uint8                       *p_data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint16_t    u16_NoOfBytes;
    uint8_t     e_DeviceAddress_Type;
    uint8_t     e_DeviceIndex_Type;
    uint8_t     u8_NumberOfBytes;
    uint8_t     u8_NumberOfIterations;
    uint8_t     e_Flag_Status = Flag_e_TRUE;
    uint8_t     e_SendType_Type = SendType_e_Synchronous;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_DeviceAddress_Type = DeviceAddress_e_7BitDeviceAddress;
    e_DeviceIndex_Type = get_register_address_type(reg_addr_size);

    if (CAM_DRV_I2C_BYTE_ORDER_REVERSED == byte_order)
    {
        if (2 == nr_of_bytes_to_write)
        {
            // Extract Byte 1
            u8_NumberOfIterations = p_data[1];

            // Replace byte 1 with byte 0
            p_data[1] = p_data[0];

            // Replace byte 0 with byte 1
            p_data[0] = u8_NumberOfIterations;
        }
        else if (4 == nr_of_bytes_to_write)
        {
            u8_NumberOfIterations = p_data[3];
            p_data[3] = p_data[0];
            p_data[0] = u8_NumberOfIterations;

            u8_NumberOfIterations = p_data[2];
            p_data[2] = p_data[1];
            p_data[1] = u8_NumberOfIterations;
        }
    }
    else if ((CAM_DRV_I2C_BYTE_ORDER_REVERSED_16BIT == byte_order) && (0 == nr_of_bytes_to_write % 2))
    {
        // 1. code should only execute for 16 bit reversed
        // 2. Only when driver has requested even no of bytes, 16 bit reversed don't make sense for odd reades
        u16_NoOfBytes = nr_of_bytes_to_write / 2;
        while (u16_NoOfBytes)
        {
            // Extract Byte 0
            u8_NumberOfIterations = p_data[2 * (u16_NoOfBytes - 1)];

            // Replace byte 0 with byte 1
            p_data[2 * (u16_NoOfBytes - 1)] = p_data[2 * u16_NoOfBytes - 1];

            // Replace byte 1 with byte 0
            p_data[2 * u16_NoOfBytes - 1] = u8_NumberOfIterations;

            u16_NoOfBytes--;
        }
    }


    if (nr_of_bytes_to_write)
    {
        // Passed size should not be greater than the hardware fifo size
        u8_NumberOfIterations = (nr_of_bytes_to_write / LLA_MAXIMUM_NO_OF_BYTES_IN_I2C);

        while (u8_NumberOfIterations)
        {
            e_Flag_Status = MasterI2C_WriteNByte(
                (uint16) i2c_addr,
                reg_addr,
                p_data,
                LLA_MAXIMUM_NO_OF_BYTES_IN_I2C,
                e_DeviceAddress_Type,
                e_DeviceIndex_Type,
                e_SendType_Type);

            if (Flag_e_TRUE == e_Flag_Status)
            {
                p_data = p_data + LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
                reg_addr = reg_addr + LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
                u8_NumberOfIterations--;
            }
            else
            {
                // ideally code should not come here
#if USE_TRACE_ERROR
                OstTraceInt0(TRACE_ERROR, "<ERROR>!!1. cam_drv_common_i2c_write_fn() - i2c write failure!!");
#endif
                ASSERT_XP70();
                break;
            }
        }


        if (Flag_e_TRUE == e_Flag_Status)
        {
            u8_NumberOfBytes = (uint8_t) nr_of_bytes_to_write % LLA_MAXIMUM_NO_OF_BYTES_IN_I2C;
            if (0 != u8_NumberOfBytes)
            {
                e_Flag_Status = MasterI2C_WriteNByte(
                    (uint16) i2c_addr,
                    reg_addr,
                    p_data,
                    u8_NumberOfBytes,
                    e_DeviceAddress_Type,
                    e_DeviceIndex_Type,
                    e_SendType_Type);
            }
        }
        else
        {
#if USE_TRACE_ERROR
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!2. cam_drv_common_i2c_write_fn() - i2c write failure!!");
#endif

            // ideally code should not come here
            ASSERT_XP70();
        }
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_common_i2c_write_fn() - No of bytes to write are 0!!");
#endif

        // ideally code should not come here
        ASSERT_XP70();
        e_Flag_Status = Flag_e_FALSE;
    }


    return (Flag_e_TRUE == e_Flag_Status ? CAM_DRV_FN_OK : CAM_DRV_FN_FAIL);
}


void
cam_drv_sensor_about_to_stop_fn(void)
{
#if 0
    if (Stream_IsISPInputStreaming())
    {
        Stream_StopRx();

        ///<Todo: BG> - Statemachine is currently static; cannot call from here
       /// Need to see if this can be changed to a global fn
#   if 0
        StateMachine(
        StreamResource_e_Rx,
        g_Stream_InputControl.e_StreamCommand_Rx,
        &g_Stream_InputStatus.e_StreamResourceStatus_Rx);
#   endif
        Set_ISP_SMIARX_ISP_SMIARX_CTRL(0, 1, 0);    // rx_start,rx_stop,rx_abort
        WaitForResourceToStop(StreamResource_e_Rx);

        CRM_SwitchToEmulatedSensorClocks();

        if (SystemConfig_IPPSetupRequired())
        {
            if (SystemConfig_IsInputInterfaceCCP())
            {
                Set_IPP_IPP_INTERNAL_EN_CLK_CCP(0);

                // wait for 15us
                GPIOManager_Delay_us(15);
            }


            // disable the IPP_DPHY_TOP_IF
            Set_IPP_IPP_DPHY_TOP_IF_EN(0);
        }


        CRM_SetSensorClockUnavailable();

        // <Todo: BG> - Check if this is required at all..
        //  This may change the statemachine state externally which may cause undesirable effect..
        Stream_SetISPInputStatusToStop();
    }


#endif
}


void
cam_drv_sensor_started_fn(void)
{
#if 0
    GPIOManager_Delay_us(g_SystemSetup.f_SensorStartDelay_us);
    if (SystemConfig_IsInputInterfaceCCP())
    {
        // For CCP sensor, after starting sensor, wait for some time so that the clocks
        // available from the sensor are good.
        //GPIOManager_Toggle_GPIO_Pin();
        GPIOManager_Delay_us(g_SystemSetup.f_SensorStartDelay_us);

        if (SystemConfig_IPPSetupRequired())
        {
            // enable IPP_SD_STATIC_CCP_EN
            Set_IPP_IPP_SD_STATIC_CCP_EN(STATIC_SD_CCP_EN_B_0x1);

            // enable the IPP_DPHY_TOP_IF
            Set_IPP_IPP_DPHY_TOP_IF_EN(1);

            // enable IPP internal CCP clock
            Set_IPP_IPP_INTERNAL_EN_CLK_CCP(INTERNAL_EN_CLK_CCP_B_0x1);
        }
    }
    else
    {
        // For CSI sensor, after starting sensor, no need to wait for
        // before enabling the IPP_DPHY
        if (SystemConfig_IPPSetupRequired())
        {
            // enable the IPP_DPHY_TOP_IF
            Set_IPP_IPP_DPHY_TOP_IF_EN(1);
        }


        //GPIOManager_Toggle_GPIO_Pin();
        GPIOManager_Delay_us(g_SystemSetup.f_SensorStartDelay_us);
    }


    // switch to real sensor clocks as sensor has started & clocks are good.
    CRM_SwitchToRealSensorClocks();

    // set sensor clock available in the Clock Manager.
    CRM_SetSensorClockAvailable();

    WaitForResourceToStart(StreamResource_e_Sensor);

    //  <Todo: BG> - Should event notification be sent to host?
    // For initial streaming these steps are already done in algorithm update
    // Only valid for changeover
    if (Zoom_IsFDMRequestPending())
    {
        ZoomTop_UpdateSensorConfig();

        ZoomTop_FDMRequestAccepted();

        ISPSetup();
    }


    // workaround for stream state machine
    Stream_StartRx();

    StartRx();
    WaitForResourceToStart(StreamResource_e_Rx);

    // <Todo: BG> - Check if this is required at all..
    //  This may change the statemachine state externally which may cause undesirable effect..
    Stream_SetISPInputStatusToStreaming();

    EventManager_ISPStreaming_Notify();
#endif
}


void
LLA_AbstractionCamDrvIntTimer(
CAM_DRV_INT_TIMER_E timer_id,
uint32              time_in_10us)
{
    //Currently only sigle interrupt timer is suppported and that also with ID = CAM_DRV_INT_TIMER_1
    if ((CAM_DRV_INT_TIMER_1 != timer_id) && (CAM_DRV_INT_TIMER_2 != timer_id))
    {
        //This case should not be happned
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!WRONG timer initiated!!");
#endif
        ASSERT_XP70();

        return;
    }

    if (CAM_DRV_INT_TIMER_1 == timer_id)
    {
        GPIOManager_StartIntTimer_us(time_in_10us * 10);
    }

#if INCLUDE_FOCUS_MODULES
    if (CAM_DRV_INT_TIMER_2 == timer_id)
    {
        FLADriver_StartTimer(time_in_10us);
    }
#endif //INCLUDE_FOCUS_MODULES

    return;
}

