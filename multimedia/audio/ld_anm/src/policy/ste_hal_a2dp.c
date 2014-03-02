/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_hal_a2dp.c
*   \brief Interface used by ANM policy manager part to address A2DP external HAL

*/
#define ANM_LOG_FILENAME "anm_a2dp_hal"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_ap.h"
#include <ste_adm_client.h>
#include <unistd.h>
#include <math.h>
#include "ste_anm_ext_hal.h"
#include "ste_hal_a2dp.h"


char a2dp_state2string[IO_NUMBER][20]= {
    "IO_OPENED","IO_CONNECTED","IO_DISCONNECTED","IO_CLOSED"
};

static bool a2dp_used_for_sonification(hw_ext_module_t * hw_module,audio_devices_t  device){
    return true;
}

static int a2dp_init( hw_ext_module_t * hw_module){
    if(hw_module->data==NULL)
        hw_module->data=(void*)malloc(sizeof(struct a2dp_internal_data_s));

    ALOG_INFO("Initialization of ANM interface with A2dp external HAL");
    memset(hw_module->data,0,sizeof(struct a2dp_internal_data_s));
    ((struct a2dp_internal_data_s*)(hw_module->data))->state=IO_CLOSED;
    return 0;
}

static audio_io_handle_t a2dp_get_io_handle(hw_ext_module_t * hw_module,audio_devices_t device) {
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    if(a2dp_info==NULL) return -1;
    return(a2dp_info->output_handle);
}


static bool a2dp_is_known_io_handle(hw_ext_module_t * hw_module,audio_io_handle_t iohandle){
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    if(a2dp_info==NULL) return -1;
    return(a2dp_info->output_handle==iohandle);
}

static int a2dp_reset( hw_ext_module_t * hw_module){
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;

    if(a2dp_info!=NULL)
        free(a2dp_info);
    hw_module->data=NULL;
    return 0;
}

static ste_audio_output_descriptor_t * a2dp_open_output(hw_ext_module_t * hw_module,audio_devices_t   device,audio_io_handle_t * pHandle){
    audio_policy_anm *apm=hw_module->apm;
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    if(a2dp_info==NULL || pHandle==NULL) return NULL;

    *pHandle=-1;
    switch(a2dp_info->state) {
        case IO_CLOSED: break;
        case IO_DISCONNECTED:
        case IO_CONNECTED:
        case IO_OPENED:
            ALOG_WARN("a2dp_open_output : Output already opened ( current state : %s)",a2dp_state2string[a2dp_info->state]);
            return (&(a2dp_info->output_desc));
            break;
        default:
            ALOG_WARN("a2dp_open_output : unknown state ");
    }

    // when an A2DP device is connected, open an A2DP and a duplicated output
    ALOG_INFO("a2dp_open_output()" );
    ste_audio_output_descriptor_t *output_desc = &(a2dp_info->output_desc);
    init_audio_output_descriptor(output_desc);
    output_desc->mDevice = device;
    output_desc->mStrategyRefCount = 1;

    /* To Be Removed : Force the strategy to use with a2dp. */
    output_desc->mStrategy = STRATEGY_MEDIA;

    a2dp_info->output_handle = apm->mp_client_interface->open_output(apm->mService,
                                            &output_desc->mDevice,
                                            &output_desc->mSamplingRate,
                                            &output_desc->mFormat,
                                            &output_desc->mChannels,
                                            &output_desc->mLatency,
                                            output_desc->mFlags);
    if (a2dp_info->output_handle) {
        ALOG_INFO("A2DP opened [IO Handle : %d]",a2dp_info->output_handle);
        output_desc->output_id=a2dp_info->output_handle;
        a2dp_info->state=IO_OPENED;
        *pHandle=a2dp_info->output_handle;
        return output_desc;
    }
    return NULL;
}

static int a2dp_close_output(hw_ext_module_t * hw_module,audio_devices_t   device){
    audio_policy_anm *apm=hw_module->apm;
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    struct str_parms *param;
    if(a2dp_info==NULL) return -1;

    switch(a2dp_info->state) {
        case IO_OPENED:
        case IO_DISCONNECTED:
            //continue treatment
            break;
        case IO_CONNECTED:
            ALOG_WARN("A2dp output is not in disconnected state ; Impossible to close it");
            return -1;
            break;
        case IO_CLOSED:
            ALOG_WARN("a2dp_close_output : already closed ");
            return 0;
            break;
        default:
            ALOG_WARN("a2dp_close_output : unknown state ");
    }

    if (a2dp_info->output_handle <=0 ) {
        ALOG_WARN("A2dp output is invalid ; Impossible to connect it");
        return -1;
    }
    ALOG_INFO("A2DP request closing ");
    param = str_parms_create_str("closing");
    str_parms_add_str(param, "closing", "true");
    apm->mp_client_interface->set_parameters(apm->mService, a2dp_info->output_handle, str_parms_to_str(param), 0);
    str_parms_destroy(param);

    apm->mp_client_interface->close_output(apm->mService, a2dp_info->output_handle);
    ALOG_INFO("A2DP closed ");
    return(a2dp_init(hw_module));
}


static int a2dp_connect_output(hw_ext_module_t * hw_module,
                                                                    audio_devices_t              device,
                                                                    const char*                  device_address)
{
    audio_policy_anm *apm=hw_module->apm;
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    char *str = NULL;
    struct str_parms *param;

    if(a2dp_info==NULL) return -1;

    switch(a2dp_info->state) {
        case IO_OPENED:
        case IO_DISCONNECTED:
            //continue treatment
            break;
        case IO_CLOSED:
            ALOG_WARN("a2dp_connect_output : A2dp output is in closed state ; Impossible to connect it");
            return -1;
            break;
        case IO_CONNECTED:
            ALOG_WARN("a2dp_connect_output : already connected ");
            return 0;
            break;
        default:
            ALOG_WARN("a2dp_connect_output : unknown state ");
    }

    if (a2dp_info->output_handle <=0 ) {
        ALOG_WARN("A2dp output is invalid ; Impossible to connect it");
        return -1;
    }

    ALOG_INFO("a2dp_connect_output()  A2DP output %d for device %s, mStrategyRefCount = %d",
            a2dp_info->output_handle,
            device_address,
            a2dp_info->output_desc.mStrategyRefCount);

    //TODO: configure audio effect output stage here
    // If both A2DP and duplicated outputs are open, send device address to A2DP hardware
    // interface
    param = str_parms_create_str("a2dp_sink_address");
    str_parms_add_str(param, "a2dp_sink_address", device_address);
    str = str_parms_to_str(param);
    apm->mp_client_interface->set_parameters(apm->mService, a2dp_info->output_handle, str, 0);
    strncpy(a2dp_info->device_address, device_address, MAX_DEVICE_ADDRESS_LEN);
    str_parms_destroy(param);
    a2dp_info->suspended = false;
    a2dp_info->state=IO_CONNECTED;
    return 0;
}

static int a2dp_disconnect_output(hw_ext_module_t * hw_module, audio_devices_t device,
                                                    const char *device_address)
{
    audio_policy_anm *apm=hw_module->apm;
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    if(a2dp_info==NULL) return -1;

    switch(a2dp_info->state) {
        case IO_CONNECTED:
            //continue treatment
            break;
        case IO_CLOSED:
        case IO_OPENED:
        case IO_DISCONNECTED:
            ALOG_WARN("a2dp_disconnect_output() : already disconnected ");
            return 0;
            break;
        default:
            ALOG_WARN("a2dp_disconnect_output() : unknown state ");
    }

    ALOG_INFO("a2dp_disconnect_output() disconnecting  output");

    if (a2dp_info->output_handle == 0) {
        ALOG_WARN("a2dp_disconnect_output() disconnecting A2DP and no A2DP output!");
        return ENOSYS;
    }

    if (strcmp(a2dp_info->device_address, device_address) != 0) {
        ALOG_WARN("a2dp_disconnect_output() disconnecting unknow A2DP sink address %s",
                device_address);
        return ENOSYS;
    }

    memset(a2dp_info->device_address, 0, MAX_DEVICE_ADDRESS_LEN);
    a2dp_info->suspended = false;
    a2dp_info->state=IO_DISCONNECTED;
    return 0;
}

static audio_policy_dev_state_t a2dp_get_device_connection_state( hw_ext_module_t * hw_module,
                                                                    audio_devices_t              device,
                                                                    const char*                  device_address) {
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    audio_policy_dev_state_t status=AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE;

    if(a2dp_info==NULL)
        return status;

    if(strcmp(device_address,a2dp_info->device_address)!=0)
        return status;

    if(a2dp_info->state == IO_OPENED || a2dp_info->state == IO_CONNECTED){
        status = AUDIO_POLICY_DEVICE_STATE_AVAILABLE;
    }
    ALOG_INFO("a2dp_get_device_connection_state()  A2DP is %s",(status==AUDIO_POLICY_DEVICE_STATE_AVAILABLE ? "available" : "unavailable"));
    return status;
}

static bool a2dp_get_suspend_state(hw_ext_module_t * hw_module)
{
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    if(a2dp_info==NULL) return false;
    ALOG_INFO("a2dp_get_suspend_state()  return %s ",(a2dp_info->suspended==true ? "true" : "false"));
    return(a2dp_info->suspended);
}

static int a2dp_set_suspend_state(hw_ext_module_t * hw_module)
{
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    audio_policy_anm *apm=hw_module->apm;
    int status=-1;

    if(a2dp_info==NULL) return -1;

    // suspend A2DP output if:
    //      (NOT already suspended) &&
    //      ((SCO device is connected &&
    //       (forced usage for communication || for record is SCO))) ||
    //      (phone state is ringing || in call)
    //
    // restore A2DP output if:
    //      (Already suspended) &&
    //      ((SCO device is NOT connected ||
    //       (forced usage NOT for communication && NOT for record is SCO))) &&
    //      (phone state is NOT ringing && NOT in call)
    //
    if (a2dp_info->output_handle == 0) {
        return 0;
    }

    if (a2dp_info->suspended) {
        if ((/*(mScoDeviceAddress == "") ||*/
             ((apm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION] != AUDIO_POLICY_FORCE_BT_SCO) &&
              (apm->mForceUse[AUDIO_POLICY_FORCE_FOR_RECORD] != AUDIO_POLICY_FORCE_BT_SCO))) &&
             ((apm->mPhoneState != AUDIO_MODE_IN_CALL) &&
              (apm->mPhoneState != AUDIO_MODE_RINGTONE))) {

            ALOG_INFO("a2dp_set_suspend_state() restoreOutput %x", a2dp_info->output_handle);
            apm->mp_client_interface->restore_output(apm->mService, a2dp_info->output_handle);
            a2dp_info->suspended = false;
        }
    } else {
        if ((/*(mScoDeviceAddress != "") &&*/
             ((apm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION] == AUDIO_POLICY_FORCE_BT_SCO) ||
              (apm->mForceUse[AUDIO_POLICY_FORCE_FOR_RECORD] == AUDIO_POLICY_FORCE_BT_SCO))) ||
             ((apm->mPhoneState == AUDIO_MODE_IN_CALL) ||
              (apm->mPhoneState == AUDIO_MODE_RINGTONE))) {

            ALOG_INFO("a2dp_set_suspend_state() suspendOutput", a2dp_info->output_handle);
            apm->mp_client_interface->suspend_output(apm->mService, a2dp_info->output_handle);
            a2dp_info->suspended = true;
        }
    }
    return 0;
}


static ste_audio_output_descriptor_t * a2dp_get_output_descriptor(hw_ext_module_t * hw_module,audio_io_handle_t handle) {
    struct a2dp_internal_data_s * a2dp_info = (struct a2dp_internal_data_s *)hw_module->data;
    int status=0;

    if(a2dp_info==NULL) return NULL;
    if(a2dp_info->output_handle!=handle) return NULL;
    return(&(a2dp_info->output_desc));
}

static hw_module_ops_t s_a2dp_ops = {
    a2dp_init,
    a2dp_reset,
    a2dp_open_output,
    a2dp_close_output,
    a2dp_connect_output,
    a2dp_disconnect_output,
    a2dp_set_suspend_state,
    a2dp_get_suspend_state,
    a2dp_used_for_sonification,
    a2dp_get_output_descriptor,
    a2dp_get_device_connection_state,
    a2dp_get_io_handle,
    a2dp_is_known_io_handle
};

int a2dp_get_capabilities(hw_ext_module_t * hw_module){
    ALOG_INFO("a2dp_get_capabilities()");
    strcpy(hw_module->name,"A2DP");
    hw_module->ops=&s_a2dp_ops;
    hw_module->supported_input=0;
    hw_module->supported_output=AUDIO_DEVICE_OUT_ALL_A2DP;
    hw_module->data=NULL;
    return 0;
}





