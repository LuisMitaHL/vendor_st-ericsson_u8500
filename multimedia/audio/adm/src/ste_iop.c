/*
 *  Copyright (C) ST-Ericsson SA 2010.
 *
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 2.1 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
/*! \file ste_iop.c
    \brief ALSA I/O plugin.

    The ALSA I/O plugin is the audio device interface to the application, and
    forwards all requests from the application to the ADM server process.

    Communication between the I/O plugin and the ADM server is via Unix sockets
    and PCM audio data is transferred using shared memory.

    A table of callback functions is defined in the I/O plugin. The callback
    functions are triggered from the application.

    The ALSA I/O plugin is implemented in a shared object file located at
    /usr/lib/alsa-lib

    The entry point of the plugin is defined via SND_PCM_PLUGIN_DEFINE_FUNC()
    macro. This macro defines the function with a proper name to be referred
    from alsa-lib.

*/

#include <sys/time.h> /* workaround */
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "ste_adm_client.h"
#include <alloca.h>

/********************* Types and definitions **********************************
*
*
*
******************************************************************************/

/**
* Get the number of items in an array
*/
#define ARRAY_SIZE(ary) (sizeof(ary)/sizeof(ary[0]))

/**
* Minimum number of channels
*/
#define STE_IOP_MIN_CHANNELS 1

/**
* Maximum number of channels
*/
#define STE_IOP_MAX_CHANNELS 6

/**
* Minimum sample rate
*/
#define STE_IOP_MIN_RATE 8000

/**
* Maximum sample rate
*/
#define STE_IOP_MAX_RATE 48000

/**
* Minimum period bytes
*/
#define STE_IOP_MIN_PERIOD_BYTES 128

/**
* Maximum period bytes
*/
#define STE_IOP_MAX_PERIOD_BYTES 65536

/**
* Typedef of boolean since it is not native to C.
*/
typedef short boolean;

/**
* Typedef of boolean TRUE since it is not native to C.
*/
#define TRUE (1)

/**
* Typedef of boolean FALSE since it is not native to C.
*/
#define FALSE (0)

static inline int MIN(int x, int y)
{
    return x < y ? x : y;
}

/**
* ALSA configuration.
*/
typedef struct {
    snd_pcm_format_t     pcm_format;
    snd_pcm_access_t     pcm_access;
    unsigned int         pcm_channels;
    unsigned int         pcm_rate;
    unsigned int         pcm_period_time;
} ste_adm_alsa_hwparams_t;

/**
* I/O plugin info
*/
typedef struct {
    snd_pcm_ioplug_t    io;
    /**< Handle of the I/O plugin */
    unsigned int        hw_ptr;
    /**< Current buffer position that is returned in the Pointer callback
        function */
    int                 adm_conn_id;
    /**< ADM server connection ID */
    ste_adm_alsa_hwparams_t hwparams;
    /**< PCM hardware parameters */
    char                device_name[64]; // TODO: Put length in ste_adm_client.h
    /**< Specifies the device ID. */
    char*               bufp;
    int                 bufsz;
    int                 cur_offs;
    int                 num_bufs;
    int                 cur_buf;
    int                 is_inited;
} ste_iop_t;

/********************* Declaration of static functions ************************
*
*
*
******************************************************************************/

/**
* Playback callback function corresponding to Start in the callback table of
* ioplug.
*
* This function is called when the PCM stream is started.
*
* The callback function is required for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_playback_start(snd_pcm_ioplug_t *io);

/**
* Capture callback function corresponding to Start in the callback table of
* ioplug.
*
* This function is called when the PCM stream is started.
*
* The callback function is required for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_capture_start(snd_pcm_ioplug_t *io);

/**
* Callback function corresponding to Stop in the callback table of ioplug.
*
* This function is called when the PCM stream is stopped.
*
* The callback function is required for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_stop(snd_pcm_ioplug_t *io);

/**
* Callback function corresponding to Pointer in the callback table of ioplug.
*
* This function is called when the current position in the buffer is requested.
* The position is returned in frames, ranging from 0 to buffer_size - 1.
*
* The callback function is required for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           Current buffer position.
*/
static snd_pcm_sframes_t ste_iop_pointer(snd_pcm_ioplug_t *io);

/**
* Playback callback function corresponding to Transfer in the callback table of
* ioplug.
*
* This function is called when PCM audio data is available in the buffer and
* should be transferred to the ADM server. The buffer is referred to in the
* input parameter areas.
*
* The PCM audio data, that is referred to in areas, is copied to the shared
* memory. The server is notified that PCM audio data is available in the shared
* memory, and the shared memory id and audio data size is sent to the server.
*
* The callback function is optional for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
* @param    areas   The area array of audio data
* @param    offset  The offset (number of samples) to the audio data
* @param    size    The size (number of samples) to transfer
*
* @return           Number of transferred frames.
*/
static snd_pcm_sframes_t ste_iop_playback_transfer(
    snd_pcm_ioplug_t *io, const snd_pcm_channel_area_t *areas,
    snd_pcm_uframes_t offset, snd_pcm_uframes_t size);

/**
* Capture callback function corresponding to Transfer in the callback table of
* ioplug.
*
* This function is called when PCM audio data is requested and should be
* transferred from the ADM server to the application.
*
* The ADM server is notified that PCM audio data is requested, and the shared
* memory id and audio data size is sent to the server. The ADM server reads PCM
* audio data from the PCM source, and copies the data to the shared memory. The
* PCM audio data is then written to the buffer referred to in the output
* parameter areas in the transfer callback function.
*
* The callback function is optional for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
* @param    areas   The area array of audio data
* @param    offset  The offset (number of samples) to the audio data
* @param    size    The size (number of samples) to transfer
*
* @return           Number of transferred frames.
*/
static snd_pcm_sframes_t ste_iop_capture_transfer(
    snd_pcm_ioplug_t *io, const snd_pcm_channel_area_t *areas,
    snd_pcm_uframes_t offset, snd_pcm_uframes_t size);

/**
* Callback function corresponding to Close in the callback table of ioplug.
*
* This function is called when the PCM stream is closed.

* The PCM source/sink is closed, the connection to the ADM server is closed and
* the shared memory is detached and removed.
*
* The callback function is optional for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_close(snd_pcm_ioplug_t *io);

/**
* Callback function corresponding to Prepare in the callback table of ioplug.
*
* This function is called when the PCM stream is prepared.
*
* The shared memory that is used for transferring audio data to/from the ADM
* server is created, the connection to the ADM server is set up and the PCM
* source/sink is opened.
*
* The callback function is optional for an ALSA I/O plugin.
*
* @param    io      The ioplug handle.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_prepare(snd_pcm_ioplug_t *io);

/**
* Allows for clients to configure the HW specific parameters of the device
*
* The callback function is optional for an ALSA I/O plugin.
*
* @param    io The ioplug handle.
* @param    params Pointer to the configuration space.
*
* @return   0 if successful, or a negative error code.
*/
static int ste_iop_hwparams(snd_pcm_ioplug_t *io,
    snd_pcm_hw_params_t *params);

/**
* Set ioplug constraints, e.g. access mode, audio formats, number of channels,
* sample rate, period size, number of periods.
*
* @param    ste_iop The I/O plugin info struct
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_set_hw_constraint(ste_iop_t *ste_iop);

/**
* Setup and create the I/O plugin.
*
* @param    pcmp    Pointer to store the resultant PCM handle.
* @param    name    Name of PCM, used as input to snd_pcm_ioplug_create
* @param    stream  Stream direction, used as input to snd_pcm_ioplug_create
* @param    mode    PCM open mode, used as input to snd_pcm_ioplug_create
* @param    device_name Character string representation of the device name.
*
* @return           0 if successful, or a negative error code.
*/
static int ste_iop_open(snd_pcm_t **pcmp, const char *name,
                        snd_pcm_stream_t stream, int mode,
                        const char* device_name);

/**
* Playback callback functions for this I/O plugin
*/
static snd_pcm_ioplug_callback_t ste_iop_playback_callback = {
    .start      = ste_iop_playback_start,
    .stop       = ste_iop_stop,
    .pointer    = ste_iop_pointer,
    .transfer   = ste_iop_playback_transfer,
    .close      = ste_iop_close,
    .prepare    = ste_iop_prepare,
    .hw_params  = ste_iop_hwparams
};

/**
* Capture callback functions for this I/O plugin
*/
static snd_pcm_ioplug_callback_t ste_iop_capture_callback = {
    .start      = ste_iop_capture_start,
    .stop       = ste_iop_stop,
    .pointer    = ste_iop_pointer,
    .transfer   = ste_iop_capture_transfer,
    .close      = ste_iop_close,
    .prepare    = ste_iop_prepare,
    .hw_params  = ste_iop_hwparams
};

/********************* Defintion of static functions **************************
*
*
*
******************************************************************************/

static int ste_iop_playback_start(snd_pcm_ioplug_t *io)
{
    // printf("ste_iop_playback_start\n");
    return 0;
}

static int ste_iop_capture_start(snd_pcm_ioplug_t *io)
{
    // printf("ste_iop_capture_start\n");
    ste_iop_t *ste_iop = io->private_data;
    ste_iop->hw_ptr    = io->period_size;
    return 0;
}

static int ste_iop_stop(snd_pcm_ioplug_t *io)
{
    // printf("ste_iop_stop\n");
    ste_iop_t *ste_iop = io->private_data;
    ste_iop->is_inited = 0;
    ste_iop->hw_ptr    = 0;
    return 0;
}

static snd_pcm_sframes_t ste_iop_pointer(snd_pcm_ioplug_t *io)
{
    ste_iop_t *ste_iop = io->private_data;
    // printf("ste_iop_pointer %d\n", ste_iop->hw_ptr);
    return ste_iop->hw_ptr;
}

static snd_pcm_sframes_t ste_iop_playback_transfer(
    snd_pcm_ioplug_t *io, const snd_pcm_channel_area_t *areas,
    snd_pcm_uframes_t offset, snd_pcm_uframes_t size)
{
    int err = 0;
    ste_iop_t *ste_iop = io->private_data;

    if (!ste_iop->is_inited) {
        ste_iop->hw_ptr    = 0;
        ste_iop->cur_offs  = 0;
        ste_iop->cur_buf   = 0;
        ste_iop->is_inited = 1;
    }

    /* Check that size is not bigger than the max value allowed */
    if (size > (STE_IOP_MAX_PERIOD_BYTES / (areas->step/8))) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: size is bigger than the max value allowed\n");
        return 0;
    }

    // TODO: Can client write directly into shared buffer? Support this?

    int bytes_to_copy = (areas->step / 8) * size;
    int bytes_copied  = 0;
    const char* src = ((char*)areas->addr + ((areas->first + (areas->step * offset)) / 8));
    while (bytes_copied < bytes_to_copy) {
        int cur_frame_bytes = MIN(bytes_to_copy - bytes_copied, ste_iop->bufsz - ste_iop->cur_offs);

        memcpy(ste_iop->bufp + ste_iop->cur_offs + ste_iop->cur_buf*ste_iop->bufsz,
            src + bytes_copied,
            cur_frame_bytes);
        bytes_copied      += cur_frame_bytes;
        ste_iop->cur_offs += cur_frame_bytes;

        if (ste_iop->cur_offs == ste_iop->bufsz) {
            int dummy_lpa_mode;
retry_send:
            err = ste_adm_client_send(ste_iop->adm_conn_id, ste_iop->cur_buf, ste_iop->bufsz, &dummy_lpa_mode);
            if (err == STE_ADM_RES_ERR_MSG_IO) {
                int tmp_err;
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Lost connection to ADM, reconnecting\n");
                ste_adm_client_disconnect(ste_iop->adm_conn_id);
                ste_iop->adm_conn_id = ste_adm_client_connect();
                if (ste_iop->adm_conn_id < 0) {
                    fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to reconnect to ADM\n");
                    return -1;
                }
                ste_iop->num_bufs = 3;
                tmp_err = ste_adm_client_open_device(ste_iop->adm_conn_id, ste_iop->device_name,
                    ste_iop->hwparams.pcm_rate, ste_iop->hwparams.pcm_channels, NULL,
                    &ste_iop->bufp, ste_iop->bufsz, ste_iop->num_bufs);
                ste_iop->cur_buf = 0;
                if (tmp_err != STE_ADM_RES_OK) {
                    fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to reopen to ADM, error=%d\n",
                            tmp_err);
                }
                goto retry_send;
            } else if (err < 0) {
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to send data to ADM server.\n");
                return err;
            }

            ste_iop->cur_buf  = (ste_iop->cur_buf + 1) % ste_iop->num_bufs;
            ste_iop->cur_offs = 0;
        }
    }

    ste_iop->hw_ptr += size;
    ste_iop->hw_ptr %= io->buffer_size;

    return size;
}

static snd_pcm_sframes_t ste_iop_capture_transfer(
    snd_pcm_ioplug_t *io, const snd_pcm_channel_area_t *areas,
    snd_pcm_uframes_t offset, snd_pcm_uframes_t size)
{
    int err = 0;
    ste_iop_t *ste_iop = io->private_data;

    if (!ste_iop->is_inited) {
        ste_iop->cur_offs  = ste_iop->bufsz;
        ste_iop->cur_buf   = ste_iop->num_bufs-1;
        ste_iop->hw_ptr    = io->period_size;
        ste_iop->is_inited = 1;
    }

    /* Check that size is not bigger than the max value allowed */
    if (size > (STE_IOP_MAX_PERIOD_BYTES / (areas->step/8))) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Size is bigger than the max value allowed\n");
        return 0; // TODO: How to report error?
    }

    int bytes_to_copy = (areas->step / 8) * size;
    int bytes_copied  = 0;
    char* dst = (char*)areas->addr + ((areas->first + (areas->step * offset)) / 8);
    while (bytes_copied < bytes_to_copy) {
        // NOTE: ste_iop->cur_offs might be equal to ste_iop->bufsz here. This is OK.
        int cur_frame_bytes = MIN(bytes_to_copy - bytes_copied, ste_iop->bufsz - ste_iop->cur_offs);
        // printf("ste_iop->cur_offs=%d bytes_copied=%d\n",ste_iop->cur_offs, bytes_copied);

        memcpy(dst + bytes_copied,
               ste_iop->bufp + ste_iop->cur_offs + ste_iop->cur_buf*ste_iop->bufsz,
               cur_frame_bytes);
        bytes_copied      += cur_frame_bytes;
        ste_iop->cur_offs += cur_frame_bytes;

        if (ste_iop->cur_offs == ste_iop->bufsz) {
retry_read:
            err = ste_adm_client_receive(ste_iop->adm_conn_id, &ste_iop->cur_buf);
            if (err == STE_ADM_RES_ERR_MSG_IO) {
                int tmp_err;
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Lost connection to ADM, reconnecting\n");
                ste_adm_client_disconnect(ste_iop->adm_conn_id);
                ste_iop->adm_conn_id = ste_adm_client_connect();
                if (ste_iop->adm_conn_id < 0) {
                    fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to reconnect to ADM\n");
                    return -1;
                }
                ste_iop->num_bufs = 3;
                tmp_err = ste_adm_client_open_device(ste_iop->adm_conn_id, ste_iop->device_name,
                    ste_iop->hwparams.pcm_rate, ste_iop->hwparams.pcm_channels, NULL,
                    &ste_iop->bufp, ste_iop->bufsz, ste_iop->num_bufs);
                ste_iop->cur_buf = 0;
                if (tmp_err != STE_ADM_RES_OK) {
                    fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to reopen to ADM, error=%d\n",
                            tmp_err);
                }
                goto retry_read;
            } else if (err < 0) {
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to send data to ADM server.\n");
                return err;
            }

            ste_iop->cur_offs = 0;
        }
    }


    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to receive data from ADM server.\n");
        return err; // TODO: How to report error?
    }


    ste_iop->hw_ptr += size;
    ste_iop->hw_ptr %= io->buffer_size;

    return size;
}

static int ste_iop_close(snd_pcm_ioplug_t *io)
{
    int err = 0;
    ste_iop_t *ste_iop = io->private_data;

    /* Close the connection to the ADM server */
    err = ste_adm_client_disconnect(ste_iop->adm_conn_id);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to disconnect from server!\n");
        return err;
    }

    munmap(ste_iop->bufp, ste_iop->num_bufs * ste_iop->bufsz);
    return 0;
}

static int ste_iop_prepare(snd_pcm_ioplug_t *io)
{
    int err = 0;
    ste_iop_t *ste_iop = io->private_data;

    /* Connect to the ADM server */
    ste_iop->adm_conn_id = ste_adm_client_connect();
    if (ste_iop->adm_conn_id < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to connect to server!\n");
        return -1;
    }

    /* Notify server that device is being opened */
    ste_iop->num_bufs = 3;
    err = ste_adm_client_open_device(ste_iop->adm_conn_id, ste_iop->device_name,
            ste_iop->hwparams.pcm_rate, ste_iop->hwparams.pcm_channels, NULL,
            &ste_iop->bufp, ste_iop->bufsz, ste_iop->num_bufs);

    ste_iop->is_inited = 0;

    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to open device.\n");
        return err;
    }

    return 0;
}

static int ste_iop_hwparams(snd_pcm_ioplug_t *io,
    snd_pcm_hw_params_t *params)
{
    int err = 0;
    int direction;
    ste_iop_t *ste_iop = io->private_data;

    /* Get and print HW params */

    /* PCM format */
    err = snd_pcm_hw_params_get_format(params, &ste_iop->hwparams.pcm_format);

    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to get HW param PCM format: %s\n", snd_strerror(err));
        return err;
    }

    /* PCM access mode */
    err = snd_pcm_hw_params_get_access(params, &ste_iop->hwparams.pcm_access);

    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to get HW param PCM Access mode: %s\n",
            snd_strerror(err));
        return err;
    }

    /* PCM number of channels */
    err = snd_pcm_hw_params_get_channels(params, &ste_iop->hwparams.pcm_channels);

    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to get HW param PCM number of channels: %s\n",
            snd_strerror(err));
        return err;
    }

    /* Sample Rate */
    err = snd_pcm_hw_params_get_rate(params, &ste_iop->hwparams.pcm_rate,
        &direction);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to get HW param PCM Sample Rate: %s\n",
            snd_strerror(err));
        return err;
    }

    ste_iop->bufsz = ste_iop->hwparams.pcm_rate * 32 / 1000 * 2 * ste_iop->hwparams.pcm_channels;

    /* Period time */
    err = snd_pcm_hw_params_get_period_time(params, &ste_iop->hwparams.pcm_period_time,
        &direction);

    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to get HW param PCM period time: %s\n",
            snd_strerror(err));
        return err;
    }

    return 0;
}

static int ste_iop_set_hw_constraint(ste_iop_t *ste_iop)
{
    unsigned int access_list[] = {
        SND_PCM_ACCESS_RW_INTERLEAVED
    };
    unsigned int format_list[] = {
        SND_PCM_FORMAT_S16_LE
    };

    int err;

    /* Set access constraint */
    err = snd_pcm_ioplug_set_param_list(&ste_iop->io, SND_PCM_IOPLUG_HW_ACCESS,
        ARRAY_SIZE(access_list), access_list);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set access constraint: %s\n", snd_strerror(err));
        return err;
    }

    /* Set format constraint */
    err = snd_pcm_ioplug_set_param_list(&ste_iop->io, SND_PCM_IOPLUG_HW_FORMAT,
        ARRAY_SIZE(format_list), format_list);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set access constraint: %s\n", snd_strerror(err));
        return err;
    }

    /* Set channels constraint */
    err = snd_pcm_ioplug_set_param_minmax(&ste_iop->io,
        SND_PCM_IOPLUG_HW_CHANNELS, STE_IOP_MIN_CHANNELS, STE_IOP_MAX_CHANNELS);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set channels constraint: %s\n", snd_strerror(err));
        return err;
    }

    /* Set sample rate constraint */
    err = snd_pcm_ioplug_set_param_minmax(&ste_iop->io,
        SND_PCM_IOPLUG_HW_RATE, STE_IOP_MIN_RATE, STE_IOP_MAX_RATE);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set rate constraint: %s\n", snd_strerror(err));
        return err;
    }

    /* Set period bytes constraint */
    err = snd_pcm_ioplug_set_param_minmax(&ste_iop->io,
        SND_PCM_IOPLUG_HW_PERIOD_BYTES,
        STE_IOP_MIN_PERIOD_BYTES, STE_IOP_MAX_PERIOD_BYTES);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set period bytes constraint: %s\n",
            snd_strerror(err));
        return err;
    }

    return 0;
}

/** @todo A device framework needs to be introduced to be able to handle
    the different audio devices (configuration etc). For the I/O plugin the
    different audio devices could be defined in .asoundrc. */
static int ste_iop_open(snd_pcm_t **pcmp, const char *name,
                        snd_pcm_stream_t stream, int mode,
                        const char* device_name)
{
    ste_iop_t *ste_iop;
    int err;

    /* Allocate memory for ste_iop. Make sure the struct is zero initialized */
    ste_iop = calloc(1, sizeof(*ste_iop));
    if (ste_iop == NULL) {
        fprintf(stderr,"ST-Ericsson ALSA ADM I/O Plugin: Failed to allocate ste_iop!\n");
        return -ENOMEM;
    }

    /* Initialize IO Plugin struct */
    ste_iop->io.version = SND_PCM_IOPLUG_VERSION;
    ste_iop->io.name = "ST-Ericsson ALSA ADM I/O Plugin";
    ste_iop->io.private_data = ste_iop;
    ste_iop->io.mmap_rw = 0;
    ste_iop->io.stream = stream;
    strncpy(ste_iop->device_name, device_name, 64);
    ste_iop->device_name[63] = 0;

    /* Check if the plugin is opened for playback or capture, and set
        corresponding callback table */
    if (stream == SND_PCM_STREAM_PLAYBACK) {
        ste_iop->io.callback = &ste_iop_playback_callback;
    } else if (stream == SND_PCM_STREAM_CAPTURE) {
        ste_iop->io.callback = &ste_iop_capture_callback;
    } else {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: ERROR! Unknown PCM stream \n");
    }

    /* Create the IO Plugin */
    err = snd_pcm_ioplug_create(&ste_iop->io, name, stream, mode);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to create the IO plugin\n");
        return err;
    }

    /* Set HW constraint */
    err = ste_iop_set_hw_constraint(ste_iop);
    if (err < 0) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Failed to set HW constraint\n");
        return err;
    }

    /* Update the PCM handle */
    *pcmp = ste_iop->io.pcm;

    return 0;
}

/**
* The entry point of the plugin is defined via SND_PCM_PLUGIN_DEFINE_FUNC()
* macro. This macro defines the function with a proper name to be referred from
* alsa-lib.
*
* The following arguments are defined in the macro itself, so don't use
* variables with the same names to shadow parameters.
*
* pcmp -    Pointer to store the resultant PCM handle.
*
* name -    Name of PCM, used as input to snd_pcm_ioplug_create
*
* stream -  Stream direction, used as input to snd_pcm_ioplug_create
*
* mode -    PCM open mode, used as input to snd_pcm_ioplug_create
*
* @return   0 if successful, or a negative error code.
*/
__attribute__((visibility("default"))) SND_PCM_PLUGIN_DEFINE_FUNC(steiop)
{
    snd_config_iterator_t i, next;
    const char * device_name = NULL;
    boolean found_dev_name = FALSE;
    int err;

    (void)root;

    snd_config_for_each(i, next, conf) {
        snd_config_t *n = snd_config_iterator_entry(i);
        const char *id;
        if (snd_config_get_id(n, &id) < 0)
            continue;
        if (strcmp(id, "type") == 0)
            continue;
        if (strcmp(id, "device_name") == 0) {
            if (snd_config_get_type(n) != SND_CONFIG_TYPE_STRING) {
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Invalid type for %s\n", id);
                err = -EINVAL;
                goto cleanup;
            }
            else if (found_dev_name == TRUE) {
                fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: Multiple device name definitions!\n");
                err = -EINVAL;
                goto cleanup;
            }

            (void) snd_config_get_string(n, &device_name);
            found_dev_name = TRUE;
            continue;
        }

        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: ERROR! Unknown field in .asoundrc : %s \n",id);
        return -EINVAL;
    }

    if (found_dev_name == FALSE) {
        fprintf(stderr, "ST-Ericsson ALSA ADM I/O Plugin: ERROR! No device name supplied, "
                "please check /etc/asound.conf / .asoundrc file!\n");
        return -EINVAL;
    }

    err = ste_iop_open(pcmp, name, stream, mode, device_name);
cleanup:
    /* free(device_name); ALSA doesn't allow us to free here */

    return err;
}

__attribute__((visibility("default"))) SND_PCM_PLUGIN_SYMBOL(steiop)
