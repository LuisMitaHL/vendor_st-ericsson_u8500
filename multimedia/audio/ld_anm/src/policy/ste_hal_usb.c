/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_hal_usb.c
*   \brief Interface used by ANM policy manager part to address USB external HAL
*/
#define ANM_LOG_FILENAME "anm_usb_hal"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_ap.h"
#include <ste_adm_client.h>
#include <unistd.h>
#include <math.h>
#include "ste_anm_ext_hal.h"
#include "ste_hal_usb.h"

char usb_state2str[IO_NUMBER][20]= {
    "IO_OPENED","IO_CONNECTED","IO_DISCONNECTED","IO_CLOSED"
};

static bool usb_used_for_sonification(hw_ext_module_t * hw_module,audio_devices_t  device){
    return true;
}

static int usb_init( hw_ext_module_t * hw_module){
    if(hw_module->data==NULL)
        hw_module->data=(void*)malloc(sizeof(struct usb_internal_data_s));

    ALOG_INFO("Initialization of ANM interface with USB external HAL");
    memset(hw_module->data,0,sizeof(struct usb_internal_data_s));
    ((struct usb_internal_data_s*)(hw_module->data))->state=IO_CLOSED;
    return 0;
}

static int usb_reset( hw_ext_module_t * hw_module){
    struct  usb_internal_data_s * usb_info = (struct  usb_internal_data_s *)hw_module->data;

    if(usb_info!=NULL)
        free(usb_info);
    hw_module->data=NULL;
    return 0;
}

static audio_io_handle_t usb_get_io_handle(hw_ext_module_t * hw_module,audio_devices_t device) {
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    if(usb_info==NULL) return -1;
    return(usb_info->output_handle);
}

static bool usb_is_known_io_handle(hw_ext_module_t * hw_module,audio_io_handle_t iohandle){
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    if(usb_info==NULL) return -1;
    return(usb_info->output_handle==iohandle);
}

static ste_audio_output_descriptor_t * usb_get_output_descriptor(hw_ext_module_t * hw_module,audio_io_handle_t handle) {
    struct  usb_internal_data_s * usb_info = (struct  usb_internal_data_s *)hw_module->data;

    if(usb_info==NULL) return NULL;
    if(usb_info->output_handle!=handle) return NULL;
    return(&(usb_info->output_desc));
}

static ste_audio_output_descriptor_t * usb_open_output(hw_ext_module_t * hw_module,audio_devices_t   device,audio_io_handle_t * pHandle){
    audio_policy_anm *apm=hw_module->apm;
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;

    if(usb_info==NULL)
        return NULL;

    *pHandle=-1;
    switch(usb_info->state) {
        case IO_CLOSED: break;
        case IO_DISCONNECTED:
        case IO_CONNECTED:
        case IO_OPENED:
            ALOG_INFO("usb_open_output : Output already opened ( current state : %s)",usb_state2str[usb_info->state]);
            return (&(usb_info->output_desc));
            break;
        default:
            ALOG_INFO("usb_open_output : unknown state");
    }

    ALOG_INFO("usb_open_output()" );
    ste_audio_output_descriptor_t *output_desc = &(usb_info->output_desc);
    init_audio_output_descriptor(output_desc);
    output_desc->mDevice = device;
    output_desc->mStrategyRefCount = 1;
    output_desc->mStrategy = STRATEGY_MEDIA;

    usb_info->output_handle = apm->mp_client_interface->open_output(apm->mService,
                                            &output_desc->mDevice,
                                            &output_desc->mSamplingRate,
                                            &output_desc->mFormat,
                                            &output_desc->mChannels,
                                            &output_desc->mLatency,
                                            output_desc->mFlags);

    if (usb_info->output_handle) {
        output_desc->output_id=usb_info->output_handle;
        ALOG_INFO("USB opened [IO Handle : %d]",usb_info->output_handle);
        usb_info->state=IO_OPENED;
        *pHandle=usb_info->output_handle;
        return output_desc;
    }
    return NULL;
}

static int usb_connect_output(hw_ext_module_t * hw_module,
                                                                    audio_devices_t              device,
                                                                    const char*                  CardAndDevice)
{
    audio_policy_anm *apm=hw_module->apm;
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    char *str = NULL;
    struct str_parms *param;

    if(usb_info==NULL) return -1;

    switch(usb_info->state) {
        case IO_OPENED:
        case IO_DISCONNECTED:
            //continue treatment
            break;
        case IO_CLOSED:
            ALOG_WARN("USB output is in closed state ; Impossible to connect it");
            return -1;
            break;
        case IO_CONNECTED:
            ALOG_WARN("usb_connect_output : already connected");
            return 0;
            break;
        default:
            ALOG_WARN("usb_connect_output : unknown state");
    }

    if (usb_info->output_handle <=0 ) {
        ALOG_WARN("USB output is invalid ; Impossible to connect it");
        return -1;
    }

    ALOG_INFO("USB Output = %d, mStrategyRefCount = %d, CardAndDevice =>  %s",
                usb_info->output_handle,usb_info->output_desc.mStrategyRefCount,CardAndDevice);

    // handle USB device connection
    strncpy(usb_info->usb_card_and_device,CardAndDevice, sizeof(char)*MAX_DEVICE_ADDRESS_LEN);
    param = str_parms_create_str(usb_info->usb_card_and_device);
    str = str_parms_to_str(param);
    apm->mp_client_interface->set_parameters(apm->mService, usb_info->output_handle, str, 0);
    str_parms_destroy(param);

    usb_info->state=IO_CONNECTED;
    return 0;
}


static int usb_disconnect_output(hw_ext_module_t * hw_module, audio_devices_t device,
                                                    const char *CardAndDevice)
{
    audio_policy_anm *apm=hw_module->apm;
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    if(usb_info==NULL) return -1;

    switch(usb_info->state) {
        case IO_CONNECTED:
            //continue treatment
            break;
        case IO_CLOSED:
        case IO_OPENED:
        case IO_DISCONNECTED:
            ALOG_WARN("usb_disconnect_output : already disconnected");
            return 0;
            break;
        default:
            ALOG_WARN("usb_disconnect_output : unknown state");
    }

    ALOG_INFO("USBHandleOutputDisconnection() disconnecting  output");

    if (usb_info->output_handle == 0) {
        ALOG_WARN("USBHandleOutputDisconnection() disconnecting USB and no USB output!");
        return ENOSYS;
    }

    if (strcmp(usb_info->usb_card_and_device, CardAndDevice) != 0) {
        ALOG_WARN("USBHandleOutputDisconnection() disconnecting unknow USB sink address %s",
                CardAndDevice);
        return ENOSYS;
    }

    memset(usb_info->usb_card_and_device, 0, MAX_DEVICE_ADDRESS_LEN);
    usb_info->state=IO_DISCONNECTED;
    return 0;
}

static int usb_close_output(hw_ext_module_t * hw_module,audio_devices_t   device){
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    audio_policy_anm *apm=hw_module->apm;
    struct str_parms *param;

    if(usb_info==NULL)
        return -1;

    switch(usb_info->state) {
        case IO_OPENED:
        case IO_DISCONNECTED:
            //continue treatment
            break;
        case IO_CONNECTED:
            ALOG_WARN("usb_close_output : USB output is not in disconnected state ; Impossible to close it");
            return -1;
            break;
        case IO_CLOSED:
            ALOG_WARN("usb_close_output : already closed");
            return 0;
            break;
        default:
            ALOG_WARN("usb_close_output : unknown state");
    }

    if (usb_info->output_handle <=0 ) {
        ALOG_WARN("USB output is invalid ; Impossible to connect it");
        return -1;
    }
    ALOG_INFO("USB request closing ");
    param = str_parms_create_str("closing");
    str_parms_add_str(param, "closing", "true");
    apm->mp_client_interface->set_parameters(apm->mService, usb_info->output_handle, str_parms_to_str(param), 0);
    str_parms_destroy(param);

    apm->mp_client_interface->close_output(apm->mService, usb_info->output_handle);
    ALOG_INFO("Usb closed ");
    return(usb_init(hw_module));
}

static audio_policy_dev_state_t usb_get_device_connection_state( hw_ext_module_t * hw_module,
                                                                    audio_devices_t              device,
                                                                    const char*                  CardAndDevice) {
    struct usb_internal_data_s * usb_info = (struct usb_internal_data_s *)hw_module->data;
    audio_policy_dev_state_t status=AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE;

    if(usb_info==NULL)
        return status;

    if(usb_info->state == IO_OPENED || usb_info->state == IO_CONNECTED){
        status = AUDIO_POLICY_DEVICE_STATE_AVAILABLE;
    }
    return status;
}

static hw_module_ops_t s_usb_ops = {
    usb_init,
    usb_reset,   // reset
    usb_open_output,
    usb_close_output,
    usb_connect_output,
    usb_disconnect_output,
    NULL,
    NULL,
    usb_used_for_sonification,
    usb_get_output_descriptor,
    usb_get_device_connection_state,
    usb_get_io_handle,
    usb_is_known_io_handle
};

int usb_get_capabilities(hw_ext_module_t * hw_module){
    ALOG_INFO("GetUSBCapabilities()");
    strcpy(hw_module->name,"USB");
    hw_module->ops=&s_usb_ops;
    hw_module->supported_input=0;
    hw_module->supported_output=AUDIO_DEVICE_OUT_ALL_USB;
    hw_module->data=NULL;
    return 0;
}





