/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
*
* Code adapted for usage of OMX components.
*
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
 */

#define DBGT_CONFIG_DEBUG 1
#define DBGT_CONFIG_AUTOVAR 1

#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG

#define DBGT_DECLARE_AUTOVAR
#define DBGT_LAYER 0
#define DBGT_PREFIX "DevBas"

#include "STECamTrace.h"
#include <STECameraDeviceBase.h>

namespace android {

STECameraDeviceBase::STECameraDeviceBase () {
}

STECameraDeviceBase::~STECameraDeviceBase () {
}

static STECameraDeviceBase* getSTECameraDevice(camera_device_t* aCamDevice) {
    DBGT_PROLOG("");
    if(NULL == aCamDevice) {
        DBGT_EPILOG("aCamDevice = %d", (int)aCamDevice);
        return NULL;
    }
    DBGT_EPILOG("");
    return (STECameraDeviceBase *) aCamDevice->priv;
}

static int set_preview_window(camera_device_t* aCameraDevice,
                                struct preview_stream_ops* aWindow) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->setPreviewWindow(aWindow);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static void set_callbacks(camera_device_t* aCameraDevice,
                            camera_notify_callback notify_cb,
                            camera_data_callback data_cb,
                            camera_data_timestamp_callback data_cb_timestamp,
                            camera_request_memory get_memory,
                            void *user) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->setCallbacks(notify_cb,
                                     data_cb,
                                     data_cb_timestamp,
                                     get_memory,
                                     user);
    }
    DBGT_EPILOG("");
}

static void enable_msg_type(camera_device_t* aCameraDevice, int32_t msg_type) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->enableMsgType(msg_type);
    }
    DBGT_EPILOG("");
}

static void disable_msg_type(camera_device_t* aCameraDevice, int32_t msg_type) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->disableMsgType(msg_type);
    }
    DBGT_EPILOG("");
}

static int msg_type_enabled(camera_device_t* aCameraDevice, int32_t msg_type) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->msgTypeEnabled(msg_type);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int start_preview(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval =  steCam->startPreview();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static void stop_preview(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->stopPreview();
    }
    DBGT_EPILOG("");
}

static int preview_enabled(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->previewEnabled();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int store_meta_data_in_buffers(camera_device_t* aCameraDevice, int enable) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->storeMetaDataInBuffers(enable);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int start_recording(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->startRecording();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static void stop_recording(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->stopRecording();
    }
    DBGT_EPILOG("");
}

static int recording_enabled(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->recordingEnabled();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static void release_recording_frame(camera_device_t* aCameraDevice,
                                      const void *opaque) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->releaseRecordingFrame(opaque);
    }
    DBGT_EPILOG("");
}

static int auto_focus(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->autoFocus();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int cancel_auto_focus(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->cancelAutoFocus();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int take_picture(camera_device_t* aCameraDevice) {
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->takePicture();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int cancel_picture(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->cancelPicture();
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static int set_parameters(camera_device_t* aCameraDevice, const char *parms) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->setParameters(parms);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static char* get_parameters(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        char *retval = steCam->getParameters();
        DBGT_EPILOG("");
        return retval;
    }
    DBGT_EPILOG("");
    return NULL;
}

static void put_parameters(camera_device_t* aCameraDevice, char* aParam) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->putParameters(aParam);
    }
    DBGT_EPILOG("");
}

static int send_command(camera_device_t* aCameraDevice,
                          int32_t cmd, int32_t arg1, int32_t arg2) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->sendCommand(cmd, arg1, arg2);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

static void release_s(camera_device_t* aCameraDevice) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        steCam->release();
    }
    DBGT_EPILOG("");
}

static int dump_s(camera_device_t* aCameraDevice, int fd) {
    DBGT_PROLOG("");
    STECameraDeviceBase *steCam = getSTECameraDevice(aCameraDevice);
    if (NULL != steCam) {
        int retval = steCam->dump(fd);
        DBGT_EPILOG("retval = %d", retval);
        return retval;
    }
    DBGT_CRITICAL("");
    return -EINVAL;
}

//static
camera_device_t* STECameraDeviceBase::CreateCameraDevice(STECameraDeviceBase* aCameraDevice) {

    camera_device_t* camera_device = NULL;
    camera_device_ops_t* camera_ops = NULL;

    DBGT_TRACE_INIT(libcamera);
    DBGT_PROLOG("");
     camera_device = (camera_device_t*)malloc(sizeof(*camera_device));
    if(!camera_device) {
        //LOGE("camera_device allocation fail");
        DBGT_CRITICAL("");
        return NULL;
    }

    camera_ops = (camera_device_ops_t*)malloc(sizeof(*camera_ops));
    if(!camera_ops) {
        //LOGE("camera_ops allocation fail");
        free(camera_device);
        camera_device = NULL;
        DBGT_CRITICAL("");
        return NULL;
    }

    memset(camera_device, 0, sizeof(*camera_device));
    memset(camera_ops, 0, sizeof(*camera_ops));

    camera_device->common.tag = HARDWARE_DEVICE_TAG;
    camera_device->common.version = 0;
    camera_device->ops = camera_ops;

    camera_ops->set_preview_window = set_preview_window;
    camera_ops->set_callbacks = set_callbacks;
    camera_ops->enable_msg_type = enable_msg_type;
    camera_ops->disable_msg_type = disable_msg_type;
    camera_ops->msg_type_enabled = msg_type_enabled;
    camera_ops->start_preview = start_preview;
    camera_ops->stop_preview = stop_preview;
    camera_ops->preview_enabled = preview_enabled;
    camera_ops->store_meta_data_in_buffers = store_meta_data_in_buffers;
    camera_ops->start_recording = start_recording;
    camera_ops->stop_recording = stop_recording;
    camera_ops->recording_enabled = recording_enabled;
    camera_ops->release_recording_frame = release_recording_frame;
    camera_ops->auto_focus = auto_focus;
    camera_ops->cancel_auto_focus = cancel_auto_focus;
    camera_ops->take_picture = take_picture;
    camera_ops->cancel_picture = cancel_picture;
    camera_ops->set_parameters = set_parameters;
    camera_ops->get_parameters = get_parameters;
    camera_ops->put_parameters = put_parameters;
    camera_ops->send_command = send_command;
    camera_ops->release = release_s;
    camera_ops->dump = dump_s;

    camera_device->priv = (void*)aCameraDevice;
    DBGT_EPILOG("");
    return camera_device;
}


}; // android
