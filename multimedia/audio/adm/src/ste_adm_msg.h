/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_protocol.h
*   \brief Communication protocol for STE ADM server.

    This header defines the protocol for client-server communication
    for the STE ADM server.
*/
#ifndef STE_ADM_PROT_INCLUSION_GUARD_H
#define STE_ADM_PROT_INCLUSION_GUARD_H

#include <stdint.h>
#include <sys/time.h>
#include "ste_adm_client.h"
#include "ste_adm_config.h"

/********************* Types and definitions **********************************
*
*
*
*******************************************************************************/

/**
* File system path where the listening socket will be created
*
* TODO: Move to another place (perhaps /tmp)
*/
#ifndef STE_ADM_SOCK_PATH
  #ifdef ANDROID
    #define STE_ADM_SOCK_PATH "/data/local/tmp/adm-socket"
  #else
    #define STE_ADM_SOCK_PATH "/home/ubuntu/Fairbanks/adm/socket_file"
  #endif
#endif

/**
* Magic number used in all commands to detect clobbering etc.
*/
#define STE_ADM_MAGIC (0x0112358D)

/**
* Defines all the available command ID:s, used within msg_base_t
* to specifiy the command type.
*/
typedef enum {
    MSG_ID_STRING,         /**< A simple string message. */
    MSG_ID_DATA_FEED,      /**< Notify that data is available. */
    MSG_ID_DATA_REQ,       /**< Request that data should be written
                                             to the client. */

    MSG_ID_OPEN_DEVICE,    /**< Opens a specific HW device. */
    MSG_ID_CLOSE_DEVICE,   /**< Close this clients
                                             connection to a specific device */

    MSG_ID_GET_LATENCY,    /**< Get the maximum output latency */
    MSG_ID_DRAIN,          /**< Drain and stop output devices */

    MSG_ID_VOICE_DEVICE_SUSPEND,
    MSG_ID_VOICE_DEVICE_RESUME,

    MSG_ID_SET_CSCALL_UPSTREAM_MUTE,
    MSG_ID_SET_CSCALL_DOWNSTREAM_MUTE,
    MSG_ID_GET_CSCALL_UPSTREAM_MUTE,
    MSG_ID_GET_CSCALL_DOWNSTREAM_MUTE,

    MSG_ID_SET_CSCALL_TTY,
    MSG_ID_GET_CSCALL_TTY,
    MSG_ID_SET_CSCALL_VOIP,

    MSG_ID_SET_CSCALL_UPSTREAM_VOLUME,
    MSG_ID_SET_CSCALL_DOWNSTREAM_VOLUME,
    MSG_ID_GET_CSCALL_UPSTREAM_VOLUME,
    MSG_ID_GET_CSCALL_DOWNSTREAM_VOLUME,

    MSG_ID_INIT_CSCALL_DOWNSTREAM_VOLUME,

    MSG_ID_SET_CSCALL_DICTAPHONE_MODE,
    MSG_ID_SET_CSCALL_DICTAPHONE_MUTE,
    MSG_ID_GET_CSCALL_DICTAPHONE_MUTE,

    MSG_ID_SET_APP_VOLUME,
    MSG_ID_GET_APP_VOLUME,

    MSG_ID_GET_TOPLEVEL_MAP,
    MSG_ID_SET_TOPLEVEL_MAP,
    MSG_ID_GET_TOPLEVEL_DEVICE,
    MSG_ID_D2D_CONNNECT,
    MSG_ID_D2D_DISCONNECT,

    MSG_ID_SET_EXTERNAL_DELAY,

    MSG_ID_GET_SINK_LATENCY,

    MSG_ID_START_COMFORT_TONE,
    MSG_ID_STOP_COMFORT_TONE,

    MSG_ID_CLOSE, // generic close command. Close dict / std dev / d2d. Same as close(fd), but offers return value.
    MSG_ID_SET_CSCALL_DEVICES,

    MSG_DBG_SHUTDOWN, // shuts down ADM. Used for debugging using Valgrind. Assumes ADM is idle.
    MSG_ID_SET_CSCALL_LOOPBACK,

    MSG_RESCAN_CONFIG,
    MSG_RESCAN_SPEECHPROC,
    MSG_RELOAD_DEVICE_SETTINGS,
    MSG_EXECUTE_SQL,
    MSG_GET_ACTIVE_DEVICES,
    MSG_ID_REOPEN_DB,
    MSG_DBG_DUMP_STATE,
    MSG_ID_MODEM_VC_STATE,
    MSG_ID_SET_PCM_PROBE,
    MSG_ID_GET_MODEM_TYPE,
    MSG_ID_SET_TUNING_MODE
} msg_id_t;

/**
* The command base structure to be included as the first member of all commands.
* Specifies a command ID and thtotal size (in characters) of the command.
*/
typedef struct {
    uint32_t magic;               /**< This must contain STE_ADM_MAGIC */
    msg_id_t cmd_id; /**< The command ID. */
    uint32_t size;                /**< The total size, in characters, of the
                                       command. */
    ste_adm_res_t result;         /**< Result of the command */

    // Used internally
    // TODO: Remove one of these!! No longer needed.
    int client_fd;                /**< INTERNAL USE ONLY: File descriptor request originated on */
    int reply_fd;                 /**< INTERNAL USE ONLY */
} msg_base_t;



// TODO: Rename cmd --> msg
// TODO: Rename ste_adm_-prefix, this is not a public interface

/**
* This command is used to send a simple string message. The server will just
* output the string to a suitable output. Used for debugging purposes only
* and will likely be removed.
* TODO: REMOVE
*/
typedef struct {
    msg_base_t base; /**< Base command information */
    char msg[1];                  /**< "Piggybacked" message string */
} msg_str_t;

/**
* This command is used to transfer PCM audio data to/from the ADM server.
*/
typedef struct {
    msg_base_t base;
    uint32_t                data_size;
    uint64_t                ts;
    uint32_t                lpa_mode;
    int                     buf_idx;

    // The data follows here, data_size bytes.
    char data[];
} msg_data_t;


/**
* This command is used to open/close a PCM source/sink.
*/
typedef struct {
    msg_base_t base;       /**< Base command information */

    int samplerate;                     /**< IN: Requested samplerate */
    ste_adm_format_t format;            /**< IN: Sample format, number of channels etc */
    char name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];

    int actual_samplerate;              /**< OUT: Accepted samplerate */

    //
    int bufsz;
    int num_bufs;

    // NOTE: The reply message will be followed by a sendmsg() transfer of the file handle!!

} msg_device_t;


typedef struct
{
    msg_base_t base;
    char indev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char outdev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
} msg_cscall_devices_t;

typedef struct
{
    msg_base_t base;
    int mute;
} msg_cscall_mute_t;

typedef struct
{
    msg_base_t base;
    ste_adm_tty_mode_t tty;
} msg_cscall_tty_t;

typedef struct
{
    msg_base_t base;
    int voip_enabled;
    int in_samplerate;
    int in_channels;
    int out_samplerate;
    int out_channels;    
} msg_cscall_voip_t;

typedef struct
{
    msg_base_t base;
    int volume;
} msg_cscall_volume_t;

typedef struct
{
    msg_base_t base;
    int min;
    int max;
} msg_cscall_init_volume_t;

typedef struct
{
    msg_base_t base;
    int mode;
} msg_cscall_dict_mode_t;

typedef struct
{
    msg_base_t base;
    int mute;
} msg_cscall_dict_mute_t;

typedef struct
{
    msg_base_t base;
    char                    dev_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    int                     volume;
} msg_app_volume_t;

typedef struct
{
    msg_base_t base;
    char toplevel[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char actual[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
} msg_get_toplevel_map_t;

typedef struct
{
    msg_base_t base;
    char toplevel[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char actual[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char toplevel2[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char actual2[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    int  rescan;
    int  force;
} msg_set_toplevel_map_t;

typedef struct
{
    msg_base_t base;
    char actual[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char toplevel[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
} msg_get_toplevel_device_t;

typedef struct
{
    msg_base_t base;
    char dev_name_src[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char dev_name_dst[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    ste_adm_dev2dev_flags_t flags;
} msg_d2d_connect_t;

typedef struct
{
    msg_base_t base;
    ste_adm_dev2dev_flags_t flags;
} msg_d2d_disconnect_t;

typedef struct
{
    msg_base_t base;
    char                    device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    int                     external_delay;
} msg_set_external_delay_t;

typedef struct
{
    msg_base_t base;
    int                     probe_id;
    int                     enabled;
} msg_set_pcm_probe_t;

typedef struct
{
    msg_base_t base;
    char                    dev_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    uint32_t                latencyMs;
} msg_sink_latency_t;

typedef struct
{
    msg_base_t base;
    char                    sql[];
} msg_execute_sql_t;

typedef struct
{
    msg_base_t base;
    char                    device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    int                     latency;
} msg_get_max_out_latency_t;

typedef struct {
    msg_base_t base;
    char                    dev_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    ste_adm_effect_chain_type_t chain_type;
    char                    component_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1]; // TODO: Use IL comp name length instead
    int                     nIndex;
    int                     wait_until_changed; // When true the command will block the socket until a settings changed event is received.
    struct
    {
        uint32_t                size;
        char                    data[4000]; // TODO: Symbollic?
    } config_struct;

} msg_get_set_config;

typedef struct {
    msg_base_t base;
    char                    dev_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    ste_adm_effect_chain_type_t chain_type;
    char                    component_name[STE_ADM_MAX_DEVICE_NAME_LENGTH+1]; // TODO: Use IL comp name length instead
    int                     nIndex;
} msg_rescan_config_t;


typedef struct
{
    msg_base_t         base;
    ste_adm_comfort_tone_standard_t tone_standard;
    ste_adm_comfort_tone_t          tone_type;
} msg_start_comfort_tone_t;


typedef struct
{
    msg_base_t base;
    int enabled;
    int codec_type;
} msg_cscall_loopback_t;

typedef struct {
    msg_base_t base;
    int nbr_devices;
    active_device_t device_list[ADM_NUM_PHY_DEV];
} msg_get_active_devices_t;

typedef struct
{
    msg_base_t base;
    int enabled;
} msg_set_tuning_mode_t;

typedef struct
{
    msg_base_t base;
    ste_adm_vc_modem_status_t vc_state;
} msg_modem_vc_state_t;

typedef struct
{
    msg_base_t base;
    ste_adm_modem_type_t modem_type;
} msg_get_modem_type_t;

#endif /* STE_ADM_PROT_INCLUSION_GUARD_H */







