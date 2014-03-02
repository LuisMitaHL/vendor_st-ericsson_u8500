/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ext_hal.c
*   \brief Interface used by ANM policy manager part to address external HALs
*/
#define ANM_LOG_FILENAME "anm_ext_hal"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_ap.h"
#include <ste_adm_client.h>
#include <unistd.h>
#include <math.h>

#include "ste_anm_ext_hal.h"
#include "ste_hal_a2dp.h"
#include "ste_hal_usb.h"
//#include "ste_hal_widi.h"



static struct ext_hal_context_s ext_hal_content;

static ste_audio_output_descriptor_t * ext_hal_open_output_stub(hw_ext_module_t * hw_module,audio_devices_t   device ,audio_io_handle_t * handle){return 0;}
static int ext_hal_close_output_stub(hw_ext_module_t * hw_module,audio_devices_t   device){return 0;}
static int ext_hal_connect_output_stub(hw_ext_module_t * hw_module, audio_devices_t device,const char *device_address){return 0;}
static int ext_hal_disconnect_output_stub(hw_ext_module_t * hw_module, audio_devices_t device,const char *device_address){return 0;}
static int ext_hal_init_stub(hw_ext_module_t * hw_module) {return 0;}
static int ext_hal_reset_stub(hw_ext_module_t * hw_module) { return 0; }
static bool ext_hal_used_for_sonification_stub(hw_ext_module_t * hw_module,audio_devices_t  device) {return false;}
static audio_policy_dev_state_t ext_hal_get_device_connection_state_stub( hw_ext_module_t * hw_module,audio_devices_t device,const char* device_address) {return AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE;}
static ste_audio_output_descriptor_t* ext_hal_get_output_desc_stub(hw_ext_module_t * hw_module ,audio_io_handle_t handle ) {return NULL;}
static audio_io_handle_t ext_hal_get_io_handle_stub(hw_ext_module_t * hw_module,audio_devices_t device) {return -1;}
static int ext_hal_suspend_stub(hw_ext_module_t * hw_module) {return 0;}
static bool ext_hal_get_suspend_stub(hw_ext_module_t * hw_module) { return false; }
static bool ext_hal_is_external_io_stub(hw_ext_module_t * hw_module,audio_io_handle_t iohandle) { return false; }


static hw_module_ops_t s_StubOps = {
    ext_hal_init_stub,
    ext_hal_reset_stub,
    ext_hal_open_output_stub,
    ext_hal_close_output_stub,
    ext_hal_connect_output_stub,
    ext_hal_disconnect_output_stub,
    ext_hal_suspend_stub,
    ext_hal_get_suspend_stub,
    ext_hal_used_for_sonification_stub,
    ext_hal_get_output_desc_stub,
    ext_hal_get_device_connection_state_stub,
    ext_hal_get_io_handle_stub,
    ext_hal_is_external_io_stub
};

ext_hal_context_t * identify_ext_hal(audio_policy_anm * apm) {
    ALOG_INFO("identify_ext_hal()");
    unsigned int index=0;
    ext_hal_content.apm=apm;

    for(index=0;index<NUM_EXT_HAL;index++){
        ext_hal_content.hw_module[index].data=NULL;
        ext_hal_content.hw_module[index].is_available=false;
        ext_hal_content.hw_module[index].supported_input=0;
        ext_hal_content.hw_module[index].supported_output=0;
        ext_hal_content.hw_module[index].ops=&s_StubOps;
        ext_hal_content.hw_module[index].apm=apm;
    }

    index=EXT_HAL_A2DP;
#ifdef STD_A2DP_MNGT
    ext_hal_content.hw_module[index].is_available= true;
    a2dp_get_capabilities(&(ext_hal_content.hw_module[index]));
#else
    ext_hal_content.hw_module[index].is_available= false;
#endif

    index=EXT_HAL_USB;
#ifndef STE_USB_MNGT
    ext_hal_content.hw_module[index].is_available= true;
    usb_get_capabilities(&(ext_hal_content.hw_module[index]));
#else
    ext_hal_content.hw_module[index].is_available= false;
#endif

    return &ext_hal_content;
}


bool ext_hal_is_external_device(void * pExtHal,audio_devices_t device, void ** pModule){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    hw_ext_module_t * hw_module=NULL;

    if(pModule)
        *pModule=NULL;

    if(ext_hal==NULL)
        return false;

    for(index=0;index<NUM_EXT_HAL;index++){
        hw_module = &(ext_hal->hw_module[index]);
        if((hw_module->supported_output & device) != 0) {
            if(pModule)
                *pModule=hw_module;
            return true;
        }
        if((hw_module->supported_input & device) !=0 ) {
            if(pModule)
                *pModule=hw_module;
            return true;
        }
    }
    return false;
}


bool ext_hal_is_available(void * pExtHal,audio_devices_t device){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    if(ext_hal==NULL)
        return false;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return false;

    if(hw_module->is_available) {
        return true;
    }
    return false;
}

audio_io_handle_t ext_hal_get_io_handle(void * pExtHal,audio_devices_t device){

    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    if(ext_hal==NULL)
        return EINVAL;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return EINVAL;
    if(hw_module->ops->get_io_handle) {
        return(hw_module->ops->get_io_handle(hw_module,device));
    }
    return EINVAL;
}

ste_audio_output_descriptor_t* ext_hal_get_output_desc (void * pExtHal,audio_io_handle_t handle){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    ste_audio_output_descriptor_t* descr=NULL;
    unsigned int index=0;
    hw_ext_module_t * hw_module=NULL;

    if(ext_hal==NULL || handle<=0)
        return NULL;

    for(index=0;index<NUM_EXT_HAL;index++){
        hw_module = &(ext_hal->hw_module[index]);
        if(hw_module->ops->get_output_descriptor){
            descr = hw_module->ops->get_output_descriptor(hw_module,handle);
            if(descr!=NULL) return descr;
        }
    }
    return NULL;
}

ste_audio_output_descriptor_t* ext_hal_open_output(   void * pExtHal,audio_devices_t  device,audio_io_handle_t * pHandle){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    if(ext_hal==NULL || pHandle==NULL)
        return NULL;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return NULL;

    if(hw_module->ops->open_output){
        return(hw_module->ops->open_output(hw_module,device,pHandle));
    }
    return NULL;
}

int ext_hal_close_output(   void * pExtHal,audio_devices_t  device,const char* device_address){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    int status=EINVAL;
    if(ext_hal==NULL)
        return status;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return 0;

    if(hw_module->ops->close_output){
        status=hw_module->ops->close_output(hw_module,device);
    }
    return(status);
}

int ext_hal_connect_output(   void * pExtHal,audio_devices_t  device,const char* device_address){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    int status=EINVAL;
    if(ext_hal==NULL)
        return status;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return 0;

    if(hw_module->ops->connect_output){
        status=hw_module->ops->connect_output(hw_module,device,device_address);
    }
    return(status);
}

int ext_hal_disconnect_output(   void * pExtHal,audio_devices_t  device,const char* device_address){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    int status=EINVAL;
    if(ext_hal==NULL)
        return status;

    hw_ext_module_t * hw_module=NULL;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return 0;

    if(hw_module->ops->disconnect_output){
        status=hw_module->ops->disconnect_output(hw_module,device,device_address);
    }
    return(status);
}

int ext_hal_update_suspend(  void * pExtHal)
{
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    if(ext_hal==NULL)
        return EINVAL;

    for(index=0;index<NUM_EXT_HAL;index++) {
        if(ext_hal->hw_module[index].ops->set_suspend) {
            ext_hal->hw_module[index].ops->set_suspend(&(ext_hal->hw_module[index]));
        }
    }
     return 0;
}

bool ext_hal_get_suspend(  void * pExtHal,audio_devices_t  device)
{
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    bool status=false;
    hw_ext_module_t * hw_module=NULL;

    if(ext_hal==NULL)
        return false;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return false;

    if(hw_module->ops->get_suspend){
        status=hw_module->ops->get_suspend(hw_module);
    }
    return(status);
}

audio_policy_dev_state_t ext_hal_get_device_connection_state( void * pExtHal,audio_devices_t device,const char* device_address) {

    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    hw_ext_module_t * hw_module=NULL;
    audio_policy_dev_state_t status = AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE;
    if(ext_hal==NULL)
        return status;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return status;

    if(hw_module->ops->get_device_connection_state){
        status=hw_module->ops->get_device_connection_state(hw_module,device,device_address);
    }
    return(status);
}

bool ext_hal_used_for_sonification(void * pExtHal,audio_devices_t  device){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    hw_ext_module_t * hw_module=NULL;

    if(ext_hal==NULL)
        return false;

    if(ext_hal_is_external_device(pExtHal,device,(void**)(&hw_module))==false)
        return false;

    if(hw_module->ops->used_for_sonification) {
        return(hw_module->ops->used_for_sonification(hw_module,device));
    }
    return false;
}

bool ext_hal_duplication_is_supported(void * pExtHal){
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    if(ext_hal==NULL)
        return false;

    for(index=0;index<NUM_EXT_HAL;index++) {
        if(ext_hal->hw_module[index].ops->used_for_sonification) {
            if(ext_hal->hw_module[index].ops->used_for_sonification(&(ext_hal->hw_module[index]),ext_hal->hw_module[index].supported_output)==true) {
                return true;
            }
        }
    }
    return false;
}

bool ext_hal_is_external_io_handle(void * pExtHal,audio_io_handle_t iohandle)
{
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    if(ext_hal==NULL)
        return false;

    for(index=0;index<NUM_EXT_HAL;index++) {
        if(ext_hal->hw_module[index].ops->is_known_io_handle) {
            if(ext_hal->hw_module[index].ops->is_known_io_handle(&(ext_hal->hw_module[index]),iohandle))
                return true;
        }
    }
    return false;
}

audio_devices_t ext_hal_get_external_devices(void * pExtHal)
{
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    audio_devices_t external_devices=0;

    if(    ext_hal==NULL)
        return 0;

    for(index=0;index<NUM_EXT_HAL;index++) {
        external_devices |= ext_hal->hw_module[index].supported_output;
        external_devices |= ext_hal->hw_module[index].supported_input;
    }
    return external_devices;
}

int ext_hal_init(void * pExtHal) {
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    if(ext_hal==NULL)
        return EINVAL;

    for(index=0;index<NUM_EXT_HAL;index++) {
        if(ext_hal->hw_module[index].ops->init) {
            ext_hal->hw_module[index].ops->init(&(ext_hal->hw_module[index]));
        }
    }
    return 0;
}


int ext_hal_reset(void * pExtHal) {
    ext_hal_context_t * ext_hal = (ext_hal_context_t *)pExtHal;
    unsigned int index=0;
    if(ext_hal==NULL)
        return EINVAL;

    for(index=0;index<NUM_EXT_HAL;index++) {
        if(ext_hal->hw_module[index].ops->reset) {
            ext_hal->hw_module[index].ops->reset(&(ext_hal->hw_module[index]));
        }
    }
    return 0;
}

