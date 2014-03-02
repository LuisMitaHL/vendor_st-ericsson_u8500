/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_client.c
*   \brief Implementation of STE ADM client functionality.

    Implementation of all functionality needed for clients to
    communicate with the STE ADM server process. Upon successful connection,
    the clients get a client ID used in any subsequent calls. This ID is
    actually the socket file descriptor for the socket associated with the
    client.
*/
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/mman.h>

#include "ste_adm_msg.h"
#include "ste_adm_client.h"

#ifdef ADM_LOG_ANDROID
#define LOG_TAG "ADM_Client"
#include <utils/Log.h>
#define LOG_INFO_(format, arg...) ALOGI("%s: " format, __func__, ##arg)
#define LOG_WARN_(format, arg...) ALOGW("%s: " format, __func__, ##arg)
#define LOG_ERR_(format, arg...)  ALOGE("%s: " format, __func__, ##arg)
#else
#define LOG_INFO_(format, arg...) fprintf(stderr, "INFO\t%s: " format "\n", __func__, ##arg)
#define LOG_WARN_(format, arg...) fprintf(stderr, "WARN\t%s: " format "\n", __func__, ##arg)
#define LOG_ERR_(format, arg...)  fprintf(stderr, "ERR\t%s: " format  "\n", __func__, ##arg)
#endif

static ste_adm_res_t util_fd_recv_from_socket(int socket, int* fd);

#include "ste_adm_client_util.h"

//
// Copies the specified device name, after making sure it is
// not too long. If NULL is specified as src name, the
// dst string will be filled with an empty string.
// Returns != 0 if the string was correctly copied,
// and == 0 if the string was too long.
//
static int adm_copy_device_name(char* dst, const char* src);


/********************* Definition of exported functions ************************
*
*
*
*******************************************************************************/

int ste_adm_client_connect(void)
{
    int client_socket_fd = -1;              /* socket_fd for the client */
    struct sockaddr_un remote_sockaddr_un;  /* describes the remote socket
                                               address */

    /** @todo Is assert enough or should exit be used also?*/
    assert(strlen(STE_ADM_SOCK_PATH) <=
       sizeof(remote_sockaddr_un.sun_path));

    /* create the socket for this client */
    client_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_socket_fd == -1) {
        LOG_ERR_("socket() %s", strerror(errno));
        return -1;
    }

    LOG_INFO_("Local Socket %d created, Trying to connect to remote"
        "(server) socket %s",
        client_socket_fd, STE_ADM_SOCK_PATH);

    /* Setup remote address */
    remote_sockaddr_un.sun_family = AF_UNIX;
    strcpy(remote_sockaddr_un.sun_path, STE_ADM_SOCK_PATH);

    /* Connect to ADM, we need to keep retrying to avoid race conditions
       since ADM is now spawned as a thread in Android,
       TODO: remove this when NMF supports MPC */
    int retries = 0;
    while (retries++ < 20) {
        /* try to connect our socket to the servers socket */
        if (connect(client_socket_fd, (struct sockaddr *) &remote_sockaddr_un,
                sizeof(remote_sockaddr_un)) == -1) {
            LOG_ERR_("connect() %s", strerror(errno));
            LOG_INFO_("Sleep for 1 second and retry");
            sleep(1);
        } else {
            LOG_INFO_("Succesfully connected to ADM");
            break;
        }
    }

    if (retries >= 20) {
        LOG_ERR_("Too many attempts to connect failed!");
        return -ENOENT;
    }

    return client_socket_fd;
}

ste_adm_res_t ste_adm_client_disconnect(int fd)
{
    int err;

    err = shutdown(fd, SHUT_RDWR);
    if (err) {
        LOG_ERR_("Shutdown of socket %d failed, errno %d: %s", fd, errno, strerror(errno));
    }

    err = close(fd);
    if (err) {
        LOG_ERR_("Close of local socket %d failed, errno %d: %s", fd, errno, strerror(errno));
    } else {
        LOG_INFO_("Local socket %d closed", fd);
    }

    /* Always return succes, not much to be done if this fails anyhow */
    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_client_send(int client_id, int buf_idx, int data_size, int *lpa_mode)
{
    msg_data_t cmd;

    /* Set up the command */
    memset(&cmd, 0, sizeof(cmd));
    cmd.base.magic           = STE_ADM_MAGIC;
    cmd.base.cmd_id          = MSG_ID_DATA_FEED;
    cmd.base.size            = sizeof(cmd);
    cmd.buf_idx              = buf_idx;
    cmd.data_size            = (unsigned int) data_size;
    cmd.lpa_mode = 0;

    int result = adm_exchange_cmd(client_id, &cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_send error %d, client_id %d", result, client_id);
    }

    *lpa_mode = cmd.lpa_mode;

    return result;

}

ste_adm_res_t ste_adm_client_receive(int client_id, int* buf_idx)
{
    msg_data_t cmd;

    /* Set up the command */
    memset(&cmd, 0, sizeof(msg_data_t));
    cmd.base.magic      = STE_ADM_MAGIC;
    cmd.base.cmd_id     = MSG_ID_DATA_REQ;
    cmd.base.size       = sizeof(msg_data_t);

    int result = adm_exchange_cmd(client_id, &cmd);
    if(result == STE_ADM_RES_OK){
        if (buf_idx) *buf_idx = cmd.buf_idx;
    } else {
        LOG_ERR_("ste_adm_client_receive error %d, client_id %d", result, client_id);
    }

    return result;
}

ste_adm_res_t ste_adm_client_open_device(int client_id, const char* name,
                     int samplerate, ste_adm_format_t format, int* actual_samplerate, char** bufp,
                     int bufsz, int num_bufs)
{
    /* Set up the command */
    msg_device_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_OPEN_DEVICE;
    cmd.samplerate  = samplerate;
    cmd.format      = format;
    cmd.bufsz       = bufsz;
    cmd.num_bufs    = num_bufs;

    if (!adm_copy_device_name(cmd.name, name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_exchange_cmd(client_id, &cmd);
    if (result == STE_ADM_RES_OK) {
        if (actual_samplerate) *actual_samplerate = cmd.actual_samplerate;
    } else {
        LOG_ERR_("Open device failed with error %d, client_id %d, device name %s", result, client_id, name);
    }

    int shm_fd = -1;
    if (result == STE_ADM_RES_OK) {
        // Receive file descriptor
        util_fd_recv_from_socket(client_id, &shm_fd);
        *bufp = (char*) mmap(0, (size_t) (cmd.bufsz*cmd.num_bufs),
                             PROT_WRITE | PROT_READ, MAP_SHARED,
                             shm_fd, 0);
        close(shm_fd);

        if (*bufp == (void*) -1) {
            printf("mmap failed");
            return STE_ADM_RES_INTERNAL_ERROR;
        }
    }

    return result;
}

ste_adm_res_t ste_adm_close_device(int client_id, const char* device_name)
{
    msg_device_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_CLOSE_DEVICE;

    if (!adm_copy_device_name(cmd.name, device_name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    int result = adm_exchange_cmd(client_id, &cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_close_device error %d, client_id %d, device name %s", result, client_id, device_name);
    }

    return result;
}


int adm_connect_and_exchange_cmd(void* cmd_base)
{
    // Create a new connection to ADM using socket..
    int fd = ste_adm_client_connect();
    if (fd < 0) {
        LOG_ERR_("ste_adm_client_connect() failed");
        return -1;
    }

    // Exchange command
    int result = adm_exchange_cmd(fd, cmd_base);

    // Done
    (void) ste_adm_client_disconnect(fd);
    return result;
}


int adm_exchange_cmd(int fd, void* cmd_base)
{
    msg_base_t* base = (msg_base_t*) cmd_base;
    int size;
    int ret;

    assert(base->size >= sizeof(msg_base_t));
    assert(base->size <= 16 * 1024); // sanity check
    base->magic = STE_ADM_MAGIC;

    size = 0;
    do {
        ret = send(fd,
                   (char*)cmd_base + size,
                   base->size - (size_t)size,
                   MSG_NOSIGNAL);
        size += ret;
    } while (ret > 0 && size < (int)base->size);

    if (ret <= 0) {
        LOG_ERR_("send(%d) %s", fd, strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    // Wait for response from ADM server
    size = 0;
    do {
        ret = recv(fd,
                   (char*)cmd_base + size,
                   base->size - (size_t)size,
                   0);
        size += ret;
    } while (ret > 0 && size < (int)base->size);

    if (ret <= 0) {
        LOG_ERR_("recv(%d) %s", fd, strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    // Exchange OK, use result from server
    return base->result;
}

static int adm_copy_device_name(char* dst, const char* src)
{
    if (src == NULL) {
        dst[0] = 0;
        return 1;
    } else {
        if (strlen(src) > STE_ADM_MAX_DEVICE_NAME_LENGTH) {
            return 0;
        } else {
            strcpy(dst, src);
            return 1;
        }
    }
}



ste_adm_res_t ste_adm_set_cscall_devices(int client_fd, const char* indev, const char* outdev)
{
    msg_cscall_devices_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_DEVICES;

    if (!adm_copy_device_name(cmd.indev, indev)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    if (!adm_copy_device_name(cmd.outdev, outdev)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    int result = adm_exchange_cmd(client_fd, &cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_set_cscall_devices error %d, client_fd %d", result, client_fd);
    }

    return result;

}

ste_adm_res_t ste_adm_client_set_cscall_upstream_mute(int enable_mute)
{
    ste_adm_res_t result = STE_ADM_RES_OK;

    msg_cscall_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_UPSTREAM_MUTE;
    cmd.mute        = enable_mute;

    result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_upstream_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_downstream_mute(int enable_mute)
{
    ste_adm_res_t result = STE_ADM_RES_OK;

    msg_cscall_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_DOWNSTREAM_MUTE;
    cmd.mute        = enable_mute;

    result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_downstream_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_upstream_mute(int* mute_enabled)
{
    // Public interface, NULL checks
    if (mute_enabled == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_cscall_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_UPSTREAM_MUTE;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *mute_enabled = cmd.mute;
    }
    else{
        LOG_ERR_("ste_adm_client_get_cscall_upstream_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_downstream_mute(int* mute_enabled)
{
    // Public interface, NULL checks
    if (mute_enabled == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_cscall_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_DOWNSTREAM_MUTE;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *mute_enabled = cmd.mute;
    }
    else{
        LOG_ERR_("ste_adm_client_get_cscall_downstream_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_tty_mode(ste_adm_tty_mode_t tty_mode)
{
    msg_cscall_tty_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_TTY;
    cmd.tty         = tty_mode;

    if (tty_mode > 3) {
        LOG_ERR_("ste_adm_client_set_cscall_tty_mode invalide mode %u", tty_mode);
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_tty_mode error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_voip_mode(int enabled, ste_adm_voip_mode_t *mode)
{
    msg_cscall_voip_t cmd;
    cmd.base.size       = sizeof(cmd);
    cmd.base.cmd_id     = MSG_ID_SET_CSCALL_VOIP;
    cmd.voip_enabled    = enabled;
    cmd.in_samplerate   = mode->in_samplerate;
    cmd.in_channels     = mode->in_channels;
    cmd.out_samplerate  = mode->out_samplerate;
    cmd.out_channels    = mode->out_channels;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_voip_mode error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_set_cscall_loopback_mode(int enabled, int codec)
{
    msg_cscall_loopback_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_LOOPBACK;
    cmd.enabled     = enabled;
    cmd.codec_type  = codec;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_set_cscall_loopback_mode error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_tty_mode(ste_adm_tty_mode_t* tty_mode)
{
    // Public interface, NULL checks
    if (tty_mode == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_cscall_tty_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_TTY;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *tty_mode = cmd.tty;
    }
    else{
        LOG_ERR_("ste_adm_client_get_cscall_tty_mode error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_downstream_volume(int volume)
{
    msg_cscall_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_DOWNSTREAM_VOLUME;
    cmd.volume      = volume;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_downstream_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_init_cscall_downstream_volume(int min, int max)
{
    msg_cscall_init_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_INIT_CSCALL_DOWNSTREAM_VOLUME;
    cmd.min      = min;
    cmd.max      = max;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_init_cscall_downstream_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_upstream_volume(int volume)
{
    msg_cscall_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_UPSTREAM_VOLUME;
    cmd.volume      = volume;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_upstream_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_upstream_volume(int* volume)
{
    // Public interface, NULL checks
    if (volume == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_cscall_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_UPSTREAM_VOLUME;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *volume = cmd.volume;
    }
    else{
        LOG_ERR_("ste_adm_client_get_cscall_upstream_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_downstream_volume(int* volume)
{
    // Public interface, NULL checks
    if (volume == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_cscall_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_DOWNSTREAM_VOLUME;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *volume = cmd.volume;
    }
    else{
        LOG_ERR_("ste_adm_client_get_cscall_downstream_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_dictaphone_mode(ste_adm_dictaphone_mode_t mode)
{
    msg_cscall_dict_mode_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_DICTAPHONE_MODE;
    cmd.mode = mode;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_dictaphone_mode error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_cscall_dictaphone_mute(int enable_mute)
{
    msg_cscall_dict_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_CSCALL_DICTAPHONE_MUTE;
    cmd.mute = enable_mute;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_cscall_dictaphone_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_cscall_dictaphone_mute(int *mute_enabled)
{
    msg_cscall_dict_mute_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_CSCALL_DICTAPHONE_MUTE;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *mute_enabled = cmd.mute;
    }else{
        LOG_ERR_("ste_adm_client_get_cscall_dictaphone_mute error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_app_volume(const char* dev_name, int volume)
{
    msg_app_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_APP_VOLUME;
    cmd.volume      = volume;

    if (!adm_copy_device_name(cmd.dev_name, dev_name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_app_volume error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_app_volume(const char* dev_name, int* volume)
{
    // Public interface, NULL checks
    if (volume == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_app_volume_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_APP_VOLUME;

     if (!adm_copy_device_name(cmd.dev_name, dev_name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *volume = cmd.volume;
    }
    else{
        LOG_ERR_("ste_adm_client_get_app_volume error %d", result);
    }

    return result;
}



ste_adm_res_t ste_adm_start_comfort_tone(ste_adm_comfort_tone_standard_t tone_standard,
                               ste_adm_comfort_tone_t          tone_type)
{
    msg_start_comfort_tone_t cmd;
    cmd.base.size     = sizeof(cmd);
    cmd.base.cmd_id   = MSG_ID_START_COMFORT_TONE;
    cmd.tone_standard = tone_standard;
    cmd.tone_type     = tone_type;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_start_comfort_tone error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_stop_comfort_tone()
{
    msg_base_t cmd;
    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_ID_STOP_COMFORT_TONE;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_start_comfort_tone error %d", result);
    }

    return result;
}


ste_adm_res_t ste_adm_client_get_toplevel_map(const char *toplevel_device, char *actual_device)
{
    ste_adm_res_t result;

    if (!toplevel_device || !actual_device) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_get_toplevel_map_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_TOPLEVEL_MAP;


    (void) strncpy(cmd.toplevel, toplevel_device, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);
    cmd.toplevel[STE_ADM_MAX_DEVICE_NAME_LENGTH] = '\0';

    result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        (void) strncpy(actual_device, cmd.actual, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);
        actual_device[STE_ADM_MAX_DEVICE_NAME_LENGTH] = '\0';
    }
    else{
        LOG_ERR_("ste_adm_client_get_toplevel_map error %d", result);
    }

    return result;
}


ste_adm_res_t ste_adm_client_set_toplevel_map(const char *toplevel_device, const char *actual_device)
{
    if (!toplevel_device || !actual_device) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_set_toplevel_map_t cmd;
    cmd.base.size    = sizeof(cmd);
    cmd.base.cmd_id  = MSG_ID_SET_TOPLEVEL_MAP;
    cmd.rescan       = 0;
    cmd.force        = 1;
    cmd.toplevel2[0] = 0;
    cmd.actual2[0]   = 0;

    if (!adm_copy_device_name(cmd.toplevel, toplevel_device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    if (!adm_copy_device_name(cmd.actual, actual_device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_toplevel_map error %d, device name %s", result, actual_device);
    }

    return result;
}

ste_adm_res_t ste_adm_set_toplevel_map_live(const char *toplevel_device,  const char *new_actual_device,
                                            const char *toplevel_device2, const char *new_actual_device2)
{
   if (!toplevel_device || !new_actual_device) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_set_toplevel_map_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_TOPLEVEL_MAP;
    cmd.rescan      = 1;
    cmd.force       = 0;

    if (!adm_copy_device_name(cmd.toplevel, toplevel_device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    if (!adm_copy_device_name(cmd.actual, new_actual_device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    if (toplevel_device2 && new_actual_device2) {
        if (!adm_copy_device_name(cmd.toplevel2, toplevel_device2)) {
            return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
        }

        if (!adm_copy_device_name(cmd.actual2, new_actual_device2)) {
            return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
        }
    } else {
        cmd.toplevel2[0] = 0;
        cmd.actual2[0]   = 0;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_set_toplevel_map_live %d, device name %s", result, new_actual_device);
    }

    return result;
}



ste_adm_res_t ste_adm_client_get_toplevel_device(const char *actual_device, char *toplevel_device)
{
    if (!toplevel_device || !actual_device) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_get_toplevel_device_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_TOPLEVEL_DEVICE;

    (void) strncpy(cmd.actual, actual_device, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);
    cmd.actual[STE_ADM_MAX_DEVICE_NAME_LENGTH] = '\0';

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        (void) strncpy(toplevel_device, cmd.toplevel, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);
        toplevel_device[STE_ADM_MAX_DEVICE_NAME_LENGTH] = '\0';
    } else {
        LOG_ERR_("ste_adm_client_get_toplevel_device error %d", result);
    }

    return result;
}

int ste_adm_dev2dev_connect(const char* src_dev, const char* dst_dev,
                            ste_adm_dev2dev_flags_t *flags)
{
    msg_d2d_connect_t cmd;
    msg_base_t response;
    int err = 0;
    int client_id;
    struct d2d_connection *conn = NULL;

    LOG_INFO_("Enter");

    /* Set up the command */
    cmd.base.magic  = STE_ADM_MAGIC;
    cmd.base.cmd_id = MSG_ID_D2D_CONNNECT;
    cmd.base.size   = sizeof(msg_d2d_connect_t);

    if (!adm_copy_device_name(cmd.dev_name_src, src_dev)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }
    if (!adm_copy_device_name(cmd.dev_name_dst, dst_dev)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    cmd.flags = *flags;

    // Create a new connection to ADM using socket..
    client_id = ste_adm_client_connect();
    if (client_id < 0) {
        LOG_ERR_("ste_adm_client_connect() failed");
        err = -1;
        goto cleanup;
    }
    /* Now, send dev2dev connect command to the server */
    err = send(client_id, (void *) &cmd, cmd.base.size, MSG_NOSIGNAL);

    if (err == -1) {
        LOG_ERR_("send() %s", strerror(errno));
        err = -errno;
        goto cleanup;
    }

    /* Wait for response from ADM server*/
    (void) recv(client_id, &response, sizeof(response), 0);

    /* Check that response is valid */
    assert(cmd.base.magic == response.magic);
    assert(cmd.base.cmd_id == response.cmd_id);

    if (response.result > 0) {
        *flags = response.result;
    } else {
        err = response.result;
        goto cleanup;
    }

    if (err < 0) {
        LOG_ERR_("ste_adm_dev2dev_connect failed with error \"%s\", src_dev %s, dst_dev %s", strerror(-err), src_dev, dst_dev);
        goto cleanup;
    }

cleanup:
    /* If error close the connection and return error code */
    if (err < 0) {
        /* Check if a valid connection is established */
        if (client_id > 0) {
                (void) ste_adm_client_disconnect(client_id);
        } else {
            /* If it was never added to the list free it, otherwise
                it would have been freed during disconnect */
            free(conn);
        }
        return err;
    } else {
        /* In case of no errors return the 'client_id' for this connection */
        return client_id;
    }
}

int ste_adm_dev2dev_disconnect(int client_id)
{
    msg_d2d_disconnect_t cmd;
    msg_base_t response;
    int err = 0;
    struct d2d_connection *conn = NULL;

    LOG_INFO_("Enter");

    /* Set up the command */
    cmd.base.magic  = STE_ADM_MAGIC;
    cmd.base.cmd_id = MSG_ID_D2D_DISCONNECT;
    cmd.base.size   = sizeof(msg_d2d_disconnect_t);

  if (client_id < 0) {
        LOG_ERR_("client_id is invalid");
        err = -1;
        goto cleanup;
    }
    /* Now, send dev2dev disconnect command to the server */
    err = send(client_id, (void *) &cmd, cmd.base.size, MSG_NOSIGNAL);

    if (err == -1) {
        LOG_ERR_("send() %s", strerror(errno));
        err = -errno;
        goto cleanup;
    }

    /* Wait for response from ADM server*/
    (void) recv(client_id, &response, sizeof(response), 0);

    /* Check that response is valid */
    assert(cmd.base.magic == response.magic);
    assert(cmd.base.cmd_id == response.cmd_id);

    if (response.result > 0) {
    //*flags = response.result;
    LOG_ERR_("ste_adm_dev2dev_disconnect response seems ok");
    } else {
        err = response.result;
        goto cleanup;
    }

    if (err < 0) {
        LOG_ERR_("ste_adm_dev2dev_disconnect failed with error : %s", strerror(-err));
        goto cleanup;
    }

cleanup:
    /* If error close the connection and return error code */
    if (err < 0) {
        /* Check if a valid connection is established */
        if (client_id > 0) {
                (void) ste_adm_client_disconnect(client_id);
        } else {
            /* If it was never added to the list free it, otherwise
                it would have been freed during disconnect */
            free(conn);
        }
        return err;
    } else {
        /* In case of no errors return the 'client_id' for this connection */
        return client_id;
    }
}

ste_adm_res_t ste_adm_client_set_external_delay(const char *device, int external_delay)
{
    msg_set_external_delay_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_EXTERNAL_DELAY;
    cmd.external_delay = external_delay;

    if (!adm_copy_device_name(cmd.device, device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_external_delay error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_get_sink_latency(const char *dev_name, uint32_t *latencyMs)
{
    // Public interface, NULL checks
    if (latencyMs == NULL) {
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    msg_sink_latency_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_GET_SINK_LATENCY;

     if (!adm_copy_device_name(cmd.dev_name, dev_name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result == STE_ADM_RES_OK) {
        *latencyMs = cmd.latencyMs;
    }
    else{
        LOG_ERR_("ste_adm_client_get_sink_latency error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_set_pcm_probe(int probe_id, int enabled)
{
    msg_set_pcm_probe_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_PCM_PROBE;
    cmd.probe_id = probe_id;
    cmd.enabled = enabled;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if(result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_set_pcm_probe error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_client_execute_sql(const char *sql)
{
    msg_execute_sql_t *cmd_p;
    unsigned int cmd_size;

    int fd = ste_adm_client_connect();
    if (fd < 0) {
        LOG_ERR_("ste_adm_client_connect() failed");
        return STE_ADM_RES_UNRECOVERABLE_ERROR;
    }

    cmd_size = offsetof(msg_execute_sql_t, sql) +
        sizeof(cmd_p->sql[0]) * ((unsigned int)strlen(sql) + 1);

    cmd_p = (msg_execute_sql_t *) calloc(1, cmd_size);

    if (cmd_p == NULL) {
        LOG_ERR_("Failed to allocate command!");
        return STE_ADM_RES_ERR_MALLOC;
    }

    /* Set up the command */
    cmd_p->base.magic      = STE_ADM_MAGIC;
    cmd_p->base.cmd_id     = MSG_EXECUTE_SQL;
    cmd_p->base.size       = cmd_size;

    /* Copy sql statement */
    strcpy(cmd_p->sql, sql);

    /* Now, send the command to the server */
    if (send(fd, (void *) cmd_p, cmd_p->base.size, MSG_NOSIGNAL) == -1) {
        LOG_ERR_("send(%d) %s", fd, strerror(errno));
        goto err;
    }

    /* Wait for ack from ADM server*/
    if (recv(fd, cmd_p, cmd_p->base.size, 0) == -1) {
        free(cmd_p);
        return STE_ADM_RES_ERR_MSG_IO;
    }

    ste_adm_res_t res = cmd_p->base.result;
    free(cmd_p);
    (void) ste_adm_client_disconnect(fd);

    if (res != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_client_execute_sql error %d", res);
    }

    return res;

err:
    free(cmd_p);
    (void) ste_adm_client_disconnect(fd);
    return STE_ADM_RES_UNRECOVERABLE_ERROR;
}

ste_adm_res_t ste_adm_client_max_out_latency(const char *device, int *latency)
{
    msg_get_max_out_latency_t cmd;

    LOG_INFO_("Enter");

    /* Set up the command */
    cmd.base.magic = STE_ADM_MAGIC;
    cmd.base.cmd_id = MSG_ID_GET_LATENCY;
    cmd.base.size = sizeof(cmd);
    if (!adm_copy_device_name(cmd.device, device)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }


    /* Send the command */
    cmd.base.result = adm_connect_and_exchange_cmd(&cmd);

    if(cmd.base.result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_max_out_latency error %d", cmd.base.result);
    }

    *latency = cmd.latency;

    return cmd.base.result;
}

ste_adm_res_t ste_adm_client_drain(int client_id)
{
    msg_base_t cmd;
    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_ID_DRAIN;

    ste_adm_res_t result = adm_exchange_cmd(client_id, &cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_client_drain error %d, client_id %d", result, client_id);
    }

    return result;
}

ste_adm_res_t ste_adm_client_debug_shutdown()
{
    msg_base_t cmd;

    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_DBG_SHUTDOWN;

    return adm_connect_and_exchange_cmd(&cmd);
}

ste_adm_res_t ste_adm_client_debug_dump_state()
{
    msg_base_t cmd;

    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_DBG_DUMP_STATE;

    return adm_connect_and_exchange_cmd(&cmd);
}

ste_adm_res_t ste_adm_rescan_config(const char* dev_name,
    ste_adm_effect_chain_type_t chain_id, const char *component_name)
{
    msg_rescan_config_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_RESCAN_CONFIG;
    cmd.chain_type  = chain_id;

    if (!adm_copy_device_name(cmd.dev_name, dev_name)) {
        return STE_ADM_RES_DEVICE_NAME_TOO_LONG;
    }

    strncpy(cmd.component_name, component_name, STE_ADM_MAX_DEVICE_NAME_LENGTH+1); // TODO: Use name length from IL header?
    cmd.component_name[STE_ADM_MAX_DEVICE_NAME_LENGTH] = 0;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_rescan_config error %d", result);
    }

    return result;
}


ste_adm_res_t ste_adm_rescan_speechproc()
{
    msg_base_t cmd;
    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_RESCAN_SPEECHPROC;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_rescan_speechproc error %d", result);
    }

    return result;
}

ste_adm_res_t ste_adm_reload_device_settings()
{
    msg_base_t cmd;
    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_RELOAD_DEVICE_SETTINGS;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_reload_device_settings error %d", result);
    }

    return result;
}


ste_adm_res_t ste_adm_client_request_active_device_list(int client_id)
{
    msg_get_active_devices_t cmd;
    memset(&cmd, 0, sizeof(msg_get_active_devices_t));

    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_GET_ACTIVE_DEVICES;
    cmd.base.magic  = STE_ADM_MAGIC;

    /* Now, send the data feed command to the server */
    if (send(client_id, (void *) &cmd, cmd.base.size, MSG_NOSIGNAL) == -1) {
        LOG_ERR_("send() failed %s", strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_client_read_active_device_list(int client_id, active_device_t **device_list_pp, int *nbr_devices_p)
{
    *nbr_devices_p = 0;
    *device_list_pp = NULL;

    msg_get_active_devices_t reply_msg;
    if (recv(client_id, &reply_msg, sizeof(reply_msg), 0) != sizeof(reply_msg)) {
        LOG_ERR_("recv() failed %s", strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.size != sizeof(msg_get_active_devices_t)) {
        LOG_ERR_("bad size received %d", reply_msg.base.size);
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.magic != STE_ADM_MAGIC) {
        LOG_ERR_("bad magic received %d", reply_msg.base.magic);
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.result == STE_ADM_RES_OK) {
        *device_list_pp = malloc((size_t) reply_msg.nbr_devices * sizeof(active_device_t));
        if (*device_list_pp == NULL) {
            LOG_ERR_("malloc failed");
            return STE_ADM_RES_ERR_MALLOC;
        }
        *nbr_devices_p = reply_msg.nbr_devices;

        int i;
        for (i = 0; i < reply_msg.nbr_devices; i++) {
            strcpy((*device_list_pp)[i].dev_name, reply_msg.device_list[i].dev_name);
            (*device_list_pp)[i].app_active = reply_msg.device_list[i].app_active;
            (*device_list_pp)[i].voice_active = reply_msg.device_list[i].voice_active;
            (*device_list_pp)[i].voicecall_samplerate = reply_msg.device_list[i].voicecall_samplerate;
        }
    }
    else
    {
        LOG_ERR_("error received %d", reply_msg.base.result);
    }

    return reply_msg.base.result;
}

ste_adm_res_t ste_adm_reopen_db()
{
    msg_base_t cmd;
    cmd.size   = sizeof(cmd);
    cmd.cmd_id = MSG_ID_REOPEN_DB;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_reopen_db error %d", result);
    }

    return result;
}


#define RES_CASE_STMT(__res) case __res: return # __res;

const char* ste_adm_res_to_str(ste_adm_res_t res)
{
    switch (res) {
        RES_CASE_STMT(STE_ADM_RES_OK)
        RES_CASE_STMT(STE_ADM_RES_UNRECOVERABLE_ERROR)
        RES_CASE_STMT(STE_ADM_RES_IL_ERROR)
        RES_CASE_STMT(STE_ADM_RES_INTERNAL_ERROR)
        RES_CASE_STMT(STE_ADM_RES_ALL_IN_USE)
        RES_CASE_STMT(STE_ADM_RES_DEV_LIMIT_REACHED)
        RES_CASE_STMT(STE_ADM_RES_DEV_PER_SESSION_LIMIT_REACHED)
        RES_CASE_STMT(STE_ADM_RES_ERR_MALLOC)
        RES_CASE_STMT(STE_ADM_RES_ERR_MSG_IO)
        RES_CASE_STMT(STE_ADM_RES_ERR_TOO_MANY_CLIENTS)
        RES_CASE_STMT(STE_ADM_RES_CHAIN_TOO_LONG)
        RES_CASE_STMT(STE_ADM_RES_UNKNOWN_IL_INDEX_NAME)
        RES_CASE_STMT(STE_ADM_RES_IL_UNSUPPORTED_INDEX)
        RES_CASE_STMT(STE_ADM_RES_IL_UNKNOWN_ERROR)
        RES_CASE_STMT(STE_ADM_RES_IL_INCONSISTENT)
        RES_CASE_STMT(STE_ADM_RES_UNKNOWN_DATABASE_ERROR)
        RES_CASE_STMT(STE_ADM_RES_DEVICE_NAME_TOO_LONG)
        RES_CASE_STMT(STE_ADM_RES_INVALID_PARAMETER)
        RES_CASE_STMT(STE_ADM_RES_DB_FATAL)
        RES_CASE_STMT(STE_ADM_RES_DB_INCONSISTENT)
        RES_CASE_STMT(STE_ADM_RES_NO_SUCH_DEVICE)
        RES_CASE_STMT(STE_ADM_RES_INVALID_CHANNEL_CONFIG)
        RES_CASE_STMT(STE_ADM_RES_NO_SUCH_SPEECH_CONFIG)
        RES_CASE_STMT(STE_ADM_RES_INCORRECT_STATE)
        RES_CASE_STMT(STE_ADM_RES_INVALID_BUFFER_SIZE)
        RES_CASE_STMT(STE_ADM_RES_PARTIAL_CSCALL_NOT_SUPPORTED)
        RES_CASE_STMT(STE_ADM_RES_IL_INSUFFICIENT_RESOURCES)
        RES_CASE_STMT(STE_ADM_RES_CSCALL_DISABLED)
        RES_CASE_STMT(STE_ADM_RES_ALSA_ERROR)
        RES_CASE_STMT(STE_ADM_RES_NO_SUCH_COMPONENT)
        RES_CASE_STMT(STE_ADM_RES_DEVICE_CLOSED)
        RES_CASE_STMT(STE_ADM_RES_DEVICE_ALREADY_OPEN)
        RES_CASE_STMT(STE_ADM_RES_CSCALL_NOT_ACTIVE)
        RES_CASE_STMT(STE_ADM_RES_CSCALL_FINISHED)
        RES_CASE_STMT(STE_ADM_RES_SETTINGS_NOT_COMPATIBLE)
        default: return "<Unknown result code>";
    }
}



// Receive a file descriptor from a socket
static ste_adm_res_t util_fd_recv_from_socket(int socket_fd, int* fd)
{
    struct msghdr msg;
    char control_data[CMSG_SPACE(sizeof(int))];

    char iobuf[8];
    struct iovec iov[1];      /* I/O vector */
    iov[0].iov_base = iobuf;
    iov[0].iov_len = 8;

    msg.msg_control    = &control_data;
    msg.msg_controllen = sizeof(control_data);
    msg.msg_name       = NULL;
    msg.msg_namelen    = 0;
    msg.msg_iov        = iov;
    msg.msg_iovlen     = 1;
    msg.msg_flags      = 0;

    if (recvmsg(socket_fd, &msg, 0) <= 0) {
        LOG_ERR_("recvmsg(%d) failed, errno=%s", socket_fd, strerror(errno));
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    struct cmsghdr *cm = CMSG_FIRSTHDR(&msg);
    if (cm->cmsg_level != SOL_SOCKET || cm->cmsg_type != SCM_RIGHTS) {
        LOG_ERR_("level or type wrong");
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    *fd = *( (int*) CMSG_DATA(cm));

    return STE_ADM_RES_OK;
}



ste_adm_res_t ste_adm_client_request_modem_vc_state(int client_id)
{
    msg_modem_vc_state_t cmd;
    memset(&cmd, 0, sizeof(msg_modem_vc_state_t));

    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_MODEM_VC_STATE;
    cmd.base.magic  = STE_ADM_MAGIC;

    /* Now, send the data feed command to the server */
    if (send(client_id, (void *) &cmd, cmd.base.size, 0) == -1) {
        LOG_ERR_("send() failed %s", strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_client_read_modem_vc_state(int client_id, ste_adm_vc_modem_status_t *vc_status)
{
    msg_modem_vc_state_t reply_msg;
    if (recv(client_id, &reply_msg, sizeof(reply_msg), 0) != sizeof(reply_msg)) {
        LOG_ERR_("recv() failed %s", strerror(errno));
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.size != sizeof(msg_modem_vc_state_t))
    {
        LOG_ERR_("bad size received %d", reply_msg.base.size);
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.magic != STE_ADM_MAGIC)
    {
        LOG_ERR_("bad magic received %d", reply_msg.base.magic);
        return STE_ADM_RES_ERR_MSG_IO;
    }

    if (reply_msg.base.result == STE_ADM_RES_OK) {
        *vc_status = reply_msg.vc_state;
    }
    else
    {
        LOG_ERR_("error received %d", reply_msg.base.result);
    }

    return reply_msg.base.result;
}

ste_adm_res_t ste_adm_client_get_modem_type(ste_adm_modem_type_t *modem_type)
{
    msg_get_modem_type_t cmd;

    LOG_INFO_("Enter");

    /* Set up the command */
    cmd.base.magic = STE_ADM_MAGIC;
    cmd.base.cmd_id = MSG_ID_GET_MODEM_TYPE;
    cmd.base.size = sizeof(cmd);

    /* Send the command */
    cmd.base.result = adm_connect_and_exchange_cmd(&cmd);

    if(cmd.base.result != STE_ADM_RES_OK){
        LOG_ERR_("ste_adm_client_get_modem_type error %d", cmd.base.result);
    }

    *modem_type = cmd.modem_type;

    return cmd.base.result;
}

ste_adm_res_t ste_adm_client_set_tuning_mode(int enabled)
{
    msg_set_tuning_mode_t cmd;
    cmd.base.size   = sizeof(cmd);
    cmd.base.cmd_id = MSG_ID_SET_TUNING_MODE;
    cmd.enabled = enabled;

    ste_adm_res_t result = adm_connect_and_exchange_cmd(&cmd);
    if (result != STE_ADM_RES_OK) {
        LOG_ERR_("ste_adm_client_set_tuning_mode error %d", result);
    }

    return result;
}

/********************* Definition of static functions **************************
*
*
*
*******************************************************************************/

