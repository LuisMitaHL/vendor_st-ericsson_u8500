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
#ifndef _CALL_BACKS_H_
#   define _CALL_BACKS_H_

#   include "Platform.h"
#   include "cam_drv.h"

#   include "MasterI2C_op_interface.h"
#   include "SystemConfig.h"
#   include "MissingModules.h"

#   define LLA_MAXIMUM_NO_OF_BYTES_IN_I2C  (BYTES_IN_HW_FIFO_DEPTH)
#   define STE_FUNCTION_PTR_NULL           (( void * ) 0)

extern CAM_DRV_CONFIG_INIT_T    g_callback_fn;

extern void                     *cam_drv_block_alloc_no_wait_fn (uint32 block_size)TO_EXT_RAM2;
extern void                     cam_drv_block_dealloc_fn (void *p_block)TO_EXT_RAM2;

extern void                     cam_drv_control_physical_pins_fn (
                                CAM_DRV_SELECT_CAMERA_E cam_id,
                                CAM_DRV_PHYSICAL_PINS_E pin_name,
                                CAM_DRV_PIN_ACTION_E    action)TO_EXT_RAM2;

extern void                     cam_drv_event_fn (
                                CAM_DRV_EVENT_E         event_code,
                                CAM_DRV_EVENT_INFO_U    *p_event_info,
                                bool8                   isr_event);

extern CAM_DRV_FN_RETVAL_E      cam_drv_msg_send_fn (CAM_DRV_MSG_E msg_id, void *p_msg_info, uint16 msg_size);

extern void                     cam_drv_timer_start_fn (CAM_DRV_OS_TIMER_E timer_id, uint32 timer_ticks);

extern void                     cam_drv_timer_stop_fn (CAM_DRV_OS_TIMER_E timer_id);

//Callback API implementing interrupt timer
extern void                     LLA_AbstractionCamDrvIntTimer (CAM_DRV_INT_TIMER_E timer_id, uint32 time_in_10us);

extern CAM_DRV_FN_RETVAL_E      cam_drv_common_i2c_read_fn (
                                uint8                       i2c_addr,
                                uint16                      reg_addr,
                                CAM_DRV_I2C_REG_SIZE_E      reg_addr_size,
                                uint16                      nr_of_bytes_to_read,
                                CAM_DRV_I2C_BYTE_ORDER_E    byte_order,
                                uint8                       *p_data);

extern CAM_DRV_FN_RETVAL_E      cam_drv_common_i2c_write_fn (
                                uint8                       i2c_addr,
                                uint16                      reg_addr,
                                CAM_DRV_I2C_REG_SIZE_E      reg_addr_size,
                                uint16                      nr_of_bytes_to_write,
                                CAM_DRV_I2C_BYTE_ORDER_E    byte_order,
                                uint8                       *p_data);

extern void cam_drv_sensor_about_to_stop_fn () TO_EXT_RAM2;
extern void cam_drv_sensor_started_fn () TO_EXT_RAM2;
void cam_drv_blocking_timer_fn (uint32 time_in_10us) TO_EXT_RAM2;

extern CAM_DRV_FN_RETVAL_E cam_drv_control_additional_physical_pins_fn (CAM_DRV_SELECT_CAMERA_E cam_id, CAM_DRV_OPTIONAL_PHYSICAL_PINS_E pin_name, CAM_DRV_PIN_ACTION_E action)TO_EXT_RAM2;

#endif // _CALL_BACKS_H_

