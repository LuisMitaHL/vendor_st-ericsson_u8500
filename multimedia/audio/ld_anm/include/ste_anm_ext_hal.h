/*******************************************************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Header file of external HALs interface used by Policy manager.
 */
/********************************************************************************************************************/

/*! \file ste_anm_ext_hal.h
*   \brief header file used to interface external HALs
*/

#ifndef ANDROID_AUDIO_POLICY_MANAGER_ANM_EXT_HAL_H
#define ANDROID_AUDIO_POLICY_MANAGER_ANM_EXT_HAL_H
#include "ste_hal_anm_ap.h"

#define MAX_DEVICE_ADDRESS_LEN 20

enum ext_hal_id{
    EXT_HAL_A2DP,
    EXT_HAL_USB,
    EXT_HAL_WIDI,
    NUM_EXT_HAL
};

enum io_state{
    IO_OPENED,
    IO_CONNECTED,
    IO_DISCONNECTED,
    IO_CLOSED,
    IO_NUMBER
};

typedef struct hw_ext_module_s hw_ext_module_t;

typedef struct hw_module_ops_s {
    int (*init)(hw_ext_module_t * hw_module);
    int (*reset)(hw_ext_module_t * hw_module);
    ste_audio_output_descriptor_t *  (*open_output)(hw_ext_module_t * hw_module,audio_devices_t   device,audio_io_handle_t * pHandle);
    int (*close_output)(hw_ext_module_t * hw_module,audio_devices_t   device);
    int (*connect_output)(hw_ext_module_t * hw_module, audio_devices_t device,const char *device_address);
    int (*disconnect_output)(hw_ext_module_t * hw_module, audio_devices_t device,const char *device_address);
    int (*set_suspend)(hw_ext_module_t * hw_module);
    bool (*get_suspend)(hw_ext_module_t * hw_module);
    bool (*used_for_sonification)(hw_ext_module_t * hw_module,audio_devices_t  device);
    ste_audio_output_descriptor_t* (*get_output_descriptor)(hw_ext_module_t * hw_module,audio_io_handle_t handle);
    audio_policy_dev_state_t (*get_device_connection_state)( hw_ext_module_t * hw_module,audio_devices_t device,const char* device_address);
    audio_io_handle_t (*get_io_handle)(hw_ext_module_t * hw_module,audio_devices_t mDevice);
    bool (*is_known_io_handle)(hw_ext_module_t * hw_module,audio_io_handle_t iohandle);
} hw_module_ops_t;

struct hw_ext_module_s {
    audio_policy_anm * apm;
    bool is_available;
    char  name[50]; // base name of the audio HW module (primary, a2dp ...)
    hw_module_ops_t  * ops;
    audio_devices_t supported_output; // output profiles exposed by this module
    audio_devices_t supported_input;  // input profiles exposed by this module
    void * data;  // pointer on data own by hardware module itself ( coockies)
};

typedef struct ext_hal_context_s {
    struct hw_ext_module_s  hw_module[NUM_EXT_HAL];
    audio_policy_anm * apm;
}ext_hal_context_t;

ext_hal_context_t * identify_ext_hal(audio_policy_anm * apm);
bool ext_hal_duplication_is_supported(void * pExtHal);
audio_io_handle_t ext_hal_get_io_handle(void * pExtHal,audio_devices_t device);
bool ext_hal_is_external_io_handle(void * pExtHal,audio_io_handle_t iohandle);
bool ext_hal_is_available(void * pExtHal,audio_devices_t device);
ste_audio_output_descriptor_t* ext_hal_get_output_desc (void * pExtHal,audio_io_handle_t handle);
bool ext_hal_used_for_sonification(void * pExtHal,audio_devices_t  device);
int ext_hal_reset(void * pExtHal) ;
int ext_hal_init(void * pExtHal);
ste_audio_output_descriptor_t*  ext_hal_open_output(   void * pExtHal,audio_devices_t  device,audio_io_handle_t * pHandle);
int ext_hal_close_output(   void * pExtHal,audio_devices_t  device,const char* device_address);
int ext_hal_connect_output(   void * pExtHal,audio_devices_t  device,const char* device_address);
int ext_hal_disconnect_output(   void * pExtHal,audio_devices_t  device,const char* device_address);
audio_policy_dev_state_t ext_hal_get_device_connection_state( void * pExtHal,audio_devices_t device,const char* device_address);
bool ext_hal_is_external_device(void * pExtHal,audio_devices_t device, void ** pModule);
int ext_hal_update_suspend(  void * pExtHal);
bool ext_hal_get_suspend(  void * pExtHal,audio_devices_t  device) ;
audio_devices_t ext_hal_get_external_devices(void * pExtHal);
#endif
