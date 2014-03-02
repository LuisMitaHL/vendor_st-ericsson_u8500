/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef INCLUSION_GUARD_STE_ADM_DEV_H
#define INCLUSION_GUARD_STE_ADM_DEV_H

#include "OMX_Core.h"
#include "ste_adm_client.h"
#include "ste_adm_omx_tool.h"
#include "OMX_CoreExt.h"


#if defined(ADM_DBG_X86)
#include "ste_adm_hw_handler.h"
#else
#include "alsactrl_hwh.h"
#endif


struct dev_params
{
  const char* name_in;
  const char* name_out;
  int samplerate;
  ste_adm_format_t format;
  int alloc_buffers;
  int bufsz;
  int num_bufs;
  int shm_fd;
};

#ifdef ADM_MMPROBE
typedef enum {
    PCM_PROBE_IO                = 0, // offset compared to base_probe_id
    PCM_PROBE_COMMON_EFFECT     = 10,
    PCM_PROBE_COMMON_MIX_SPLIT  = 20,
    PCM_PROBE_VOICE_EFFECT      = 30,
    PCM_PROBE_APP_EFFECT        = 40,
    PCM_PROBE_APP_MIX_SPLIT     = 50
} pcm_probe_comp_t;

const char* probe_comp2str(pcm_probe_comp_t pcm_probe_comp);
#endif // ADM_MMPROBE

// Must be called at bootup
void dev_init();

// May not be called from a worker thread.
// The dev_name must have valid length (be no more than STE_ADM_MAX_DEVICE_NAME_LENGTH
// characters long.
//
// If vc != 0, open a voicecall device. In that case, name_in and
// name_out of params_p must be valid, and one of them must be equivalent
// to name.
//
// params_p->samplerate and params_p->num_channels must be valid.
// If params_p->num_channels is 0, the device is opened with the
// channel config of the source/sink.
//
// The returned handle (if valid) will always be >= 1
// If params_p->alloc_buffers != 0, dev_open() will return an enabled port with allocated buffers.
// If params_p->alloc_buffers == 0, dev_open() will return a disabled port
ste_adm_res_t dev_open(const char* name, int vc, struct dev_params* params_p, int* dev_handle);

// Close device.
// Returns 0 if ok, -ste_adm_res_t otherwise.
// Must be called from a thread that is not a worker thread.
ste_adm_res_t dev_close(int handle, int dealloc_buffers);

// Connect app/vc chain to common chain
ste_adm_res_t dev_connect_common(int handle);
// Disconnect app/vc chain from common chain
ste_adm_res_t dev_disconnect_common(int handle);

// Reconfigure the effect chains (app, vc, common) of device.
// The current effect chains are disconnected and freed, and the
// effects chains with new config are created and connected.
// If vc effect chain should be possible to reconfigure the
// corresponding in/out device and information about connection status
// must be provided.
ste_adm_res_t dev_reconfigure_effects(
    const char* name, const char* name2,
    int vc_in_disconnected, int vc_out_disconnected);

// Applys new configs from the database to all effects in the given chain,
// without breaking data flow.
ste_adm_res_t dev_rescan_effects(ste_adm_effect_chain_type_t chain, const char* name, const char* name2);

// Returns 1 if an app device is open for the given device name. Else 0 is returned.
int is_app_dev_open(const char* name);

// Returns 1 if device is in low power mode
int dev_get_lpa_mode(int handle);

// Returns a pointe to the name of the device.
// The name pointer is only valid while the device is open.
// Must be called from a thread that is not a worker thread.
const char* dev_get_name(int handle);


typedef void (*dev_il_event_fp_t)(void* param, int is_closed, il_comp_t comp, OMX_EVENTEXTTYPE eEvent, OMX_U32 Data1, OMX_U32 Data2);
typedef void (*dev_active_device_cb_fp_t)(void* param);
typedef void (*dev_lpa_mode_cb_fp_t)(void* param, int lpa_mode);


ste_adm_res_t dev_subscribe_active_devices(dev_active_device_cb_fp_t cb_fp,
                                     void* param,
                                     int* subscription_handle_p);

void dev_unsubscribe_active_devices(int handle);

/**
*   Returns a list with al open devices.
*
* device_list            - Out parameter with the list.
* size                   - In/out parameter. Takes size of vector as input and returns size of content.
*
*/
int dev_get_active_devices(active_device_t* device_list, int* size);

// Notify subscribers of active device changes
void dev_notify_active_device_changes();



void dev_unsubscribe_lpa_mode(int handle);
ste_adm_res_t dev_subscribe_lpa_mode(dev_lpa_mode_cb_fp_t cb_fp,
                                     int dev_handle,
                                     void* param,
                                     int* subscription_handle_p);


// Return the IL component and port that corresponds to
// the device - i.e. where to feed/get data to/from the
// device.
// May be called from either high-prio or low-prio thread.
int dev_get_endpoint(int handle, il_comp_t* comp_p, OMX_U32* port_p);


// Return the IL component and port that corresponds to
// the echo reference of the specified device, if any.
// May be called from either high-prio or low-prio thread.
// If there is no echo reference port for the given device,
// comp_p is set to NULL, and STE_ADM_RES_OK is returned.
ste_adm_res_t dev_get_endpoint_echoref(int handle, il_comp_t* comp_p, OMX_U32* port_p);

// May not be called from a worker thread.
// The dev_name must have valid length (be no more than STE_ADM_MAX_DEVICE_NAME_LENGTH
// characters long.
// Sets the volume of the device, requires that the device has already been opened.
// If vc != 0, the volume is set for voicecall.
// If cap == 1 the volume is capped according to the settings in the graph.
ste_adm_res_t dev_set_volume(const char* name, int vc, int volume, int cap_volume);

// May not be called from a worker thread.
// The dev_name must have valid length (be no more than STE_ADM_MAX_DEVICE_NAME_LENGTH
// characters long.
// Gets the volume of the device, requires that the device has already been opened.
// If vc != 0, the volume is set for voicecall.
ste_adm_res_t dev_get_volume(const char* name, int vc, int *volume);

// May not be called from a worker thread.
// The dev_name must have valid length (be no more than STE_ADM_MAX_DEVICE_NAME_LENGTH
// characters long.
// Gets the latency of the sink, requires that the device has already been opened.
ste_adm_res_t dev_get_sink_latency(const char* name, uint32_t *latencyMs);

#ifdef ADM_MMPROBE
// May not be called from a worker thread.
// Sets the state (enabled/disabled) of the specified pcm probe
ste_adm_res_t dev_set_pcm_probe(int probe_id, int enabled);

// Get the pcm probe id and state (enabled/disabled)
// device_name     - top level device name
// pcm_probe_comp  - probe component in the omx graph
// index           - specifying the effect position (starting at 0) when 
//                   pcm_probe_comp is an effect, otherwise specifying 
//                   the port index of pcm_probe_comp.
// probe_id        - out parameter specifying probe id.
// probe_enabled   - out parameter specifying probe status (enabled/disabled)
ste_adm_res_t dev_get_pcm_probe(const char* device_name, 
    pcm_probe_comp_t pcm_probe_comp, int index,
    int* probe_id, int* probe_enabled);
#endif // ADM_MMPROBE

// May not be called from a worker thread.
// The handle must be a valid handle to a device
ste_adm_res_t dev_set_sidetone(int handle, int enable);

// Changes TTY mode on and off.
// May only be called when no voice devices are open.
ste_adm_res_t dev_set_tty_mode(ste_adm_tty_mode_t tty_mode);

// Returns 1 if TTY is enabled;
ste_adm_tty_mode_t  dev_get_tty_mode();

// Rescan HW handler
ste_adm_res_t dev_rescan_hw_handler(const char* new_dev, fadeSpeed_t fadeSpeed);

// Open device to device hw connection
ste_adm_res_t dev_open_d2d(const char* src_dev, const char* dst_dev, int* dev_handle);

// Close device to device hw connection
ste_adm_res_t dev_close_d2d(int* dev_handle);

#endif // INCLUSION_GUARD_STE_ADM_DEV_H
