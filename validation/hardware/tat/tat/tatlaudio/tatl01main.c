/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlaudio.h"


int dth_init_service()
{
    hats_audio_err_t vl_audioerr = HATS_AUDIO_NO_ERROR;
    int result = 0;
    int status = 0;     /* use a different variable for additionnal initialisation status
                    in order to load the dth service even if an init routine fails*/

    //dthlaudio_syslog_set_level(LOG_DEBUG);

    //printf("register_dth_struct...\n");
    result = tatlaudio_register_dth_struct();
    //printf("done :-)\n");

    /* init volumes*/
    //printf("init volumes...\n");
    system("/usr/local/bin/audio/volumes.sh");
    //printf("done :-)\n");

    if (TAT_OK(result)) {

        /* asynch init */
        if (init_asynchLib() == -1) {
            result = -1;
            printf("init_asynchLib failed!\n");
        }

        if (TAT_OK(result)) {
            /*        |+ TODO: initialize service data here +|*/

            /* Select LineIn */
            vl_audioerr = SetSrc(AUDIO_DEVICE_0, CODEC_SRC_LINEIN);
            if (HATS_AUDIO_NO_ERROR != vl_audioerr) {
                status = hats_audio_err_to_dth(vl_audioerr);
                SYSLOG(LOG_ERR, "Failed to set LineIn");
            } else
                SYSLOG(LOG_DEBUG, "LineIn");


            /* Select Headset */
            vl_audioerr = SetSink(AUDIO_DEVICE_0, CODEC_DEST_HEADSET);
            if (HATS_AUDIO_NO_ERROR != vl_audioerr) {
                status = hats_audio_err_to_dth(vl_audioerr);
                SYSLOG(LOG_ERR, "Failed to set Headset");
            } else
                SYSLOG(LOG_DEBUG, "Headset");

#if defined HATS_AB_8520_HW
            /* Set AV Connector*/
            status = DthAudio_GpioConfig(TATAUDIO_GPIO_VIDEOCTRL_DIR, TATAUDIO_GPIO_VIDEOCTRL_OUT, TATAUDIO_GPIO_VIDEOCTRL_PUD,
                                         TATAUDIO_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER, 1, 0, 0);
            if (TAT_ERROR_OFF != status) {
                SYSLOG(LOG_ERR, "Failed to set gpios");
            } else
                SYSLOG(LOG_DEBUG, "GpioConfig");
#endif
        }
    }

    /* return status of register_dth_struct*/
    SYSLOG(LOG_DEBUG, "dth_init_service returns %d", result);

    return result;
}

void dth_uninit_service(void)
{
    /* TODO: release resources here */
}

int DthAudio_simple_get(struct dth_element *elem, void *value)
{
    return DthAudio_01SimpleGet(elem, value);
}

int DthAudio_simple_set(struct dth_element *elem, void *value)
{
    return DthAudio_02SimpleSet(elem, value);
}

int DthAudio_simple_exec(struct dth_element *elem)
{
    return DthAudio_03SimpleExec(elem);
}

int DthAudio_param_get(struct dth_element *elem, void *value)
{
    return DthAudio_01ParamGet(elem, value);
}

int DthAudio_param_set(struct dth_element *elem, void *value)
{
    return DthAudio_02ParamSet(elem, value);
}

int hats_audio_err_to_dth(hats_audio_err_t audioerr)
{
    DECLARE_ERR();

    switch (audioerr) {
    case HATS_AUDIO_NO_ERROR:

        TAT_SET_LAST_ERR(TAT_ERROR_OFF);
        break;

    case HATS_AUDIO_FAILED:

        SYSLOG(LOG_ERR, "Audio service: operation has failed (HATS_AUDIO_FAILED)");
        TAT_SET_LAST_ERR(TAT_ERROR_ON);
        break;

    case HATS_AUDIO_DEVICE_ALREADY_USED:

        SYSLOG(LOG_ERR, "Audio service: device busy (HATS_AUDIO_DEVICE_ALREADY_USED)");
        TAT_SET_LAST_ERR(EBUSY);
        break;

    case HATS_AUDIO_INVALID_PARAMETER:

        SYSLOG(LOG_ERR, "Audio service: invalid argument (HATS_AUDIO_INVALID_PARAMETER)");
        TAT_SET_LAST_ERR(EINVAL);
        break;

    case HATS_AUDIO_NOT_AVAILABLE:

        SYSLOG(LOG_ERR, "Audio service: not available (HATS_AUDIO_NOT_AVAILABLE)");
        TAT_SET_LAST_ERR(EAGAIN);
        break;

    case HATS_AUDIO_FORMAT_NOT_SUPPORTED:

        SYSLOG(LOG_ERR, "Audio service: audio format not supported (HATS_AUDIO_FORMAT_NOT_SUPPORTED)");
        TAT_SET_LAST_ERR(ENOTSUP);
        break;

    case HATS_AUDIO_NO_SPACE_LEFT:

        SYSLOG(LOG_ERR, "Audio service: disk full (HATS_AUDIO_NO_SPACE_LEFT)");
        TAT_SET_LAST_ERR(EDQUOT);
        break;

    default:

        SYSLOG(LOG_ERR, "Audio service: error %u", audioerr);
        TAT_SET_LAST_ERR(TAT_ERROR_ON);
    }

    RETURN_ERR();
}

int hats_video_err_to_dth(hats_video_err_t err)
{
    DECLARE_ERR();

    switch (err) {

    default:
        SYSLOG(LOG_ERR, "Video service: error %u", err);
        TAT_SET_LAST_ERR(TAT_ERROR_ON);
    }

    RETURN_ERR();
}
