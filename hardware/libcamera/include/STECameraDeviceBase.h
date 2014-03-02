/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
*
* Code adapted for usage of OMX components.
*
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMERA_DEVICE_BASE_H
#define STECAMERA_DEVICE_BASE_H

#include <unistd.h>
#include <errno.h>
#include <hardware/camera.h>
#include <OMX_Core.h>

namespace android {

typedef struct
{
    int metadataBufferType;
    void* handle;
    int offset;
} video_metadata_t;

#define START_OF_STRUCT(ADDR,TYPE,MEMBER)   ((char*)(ADDR) - ((int)&((TYPE*)0)->MEMBER))
#define CAMHAL_GRALLOC_USAGE GRALLOC_USAGE_HW_TEXTURE | \
                             GRALLOC_USAGE_HW_RENDER | \
                             GRALLOC_USAGE_HW_2D | \
                             GRALLOC_USAGE_SW_READ_RARELY | \
                             GRALLOC_USAGE_SW_WRITE_NEVER

class STECameraDeviceBase {
public:
    STECameraDeviceBase();
    virtual ~STECameraDeviceBase();
	
	virtual OMX_ERRORTYPE init() = 0;

    /** Set the ANativeWindow to which preview frames are sent */
    virtual int setPreviewWindow(
        struct preview_stream_ops *window) = 0;

    /** Set the notification and data callbacks */
    virtual void setCallbacks(
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user) = 0;

    /**
     * The following three functions all take a msg_type, which is a bitmask of
     * the messages defined in include/ui/Camera.h
     */

    /**
     * Enable a message, or set of messages.
     */
    virtual void enableMsgType(int32_t msg_type) = 0;

    /**
     * Disable a message, or a set of messages.
     *
     * Once received a call to disableMsgType(CAMERA_MSG_VIDEO_FRAME), camera
     * HAL should not rely on its client to call releaseRecordingFrame() to
     * release video recording frames sent out by the cameral HAL before and
     * after the disableMsgType(CAMERA_MSG_VIDEO_FRAME) call. Camera HAL
     * clients must not modify/access any video recording frame after calling
     * disableMsgType(CAMERA_MSG_VIDEO_FRAME).
     */
    virtual void disableMsgType(int32_t msg_type) = 0;

    /**
     * Query whether a message, or a set of messages, is enabled.  Note that
     * this is operates as an AND, if any of the messages queried are off, this
     * will return false.
     */
    virtual int msgTypeEnabled(int32_t msg_type) = 0;

    /**
     * Start preview mode.
     */
    virtual int startPreview() = 0;

    /**
     * Stop a previously started preview.
     */
    virtual void stopPreview() = 0;

    /**
     * Returns true if preview is enabled.
     */
    virtual int previewEnabled() = 0;

    /**
     * Request the camera HAL to store meta data or real YUV data in the video
     * buffers sent out via CAMERA_MSG_VIDEO_FRAME for a recording session. If
     * it is not called, the default camera HAL behavior is to store real YUV
     * data in the video buffers.
     *
     * This method should be called before startRecording() in order to be
     * effective.
     *
     * If meta data is stored in the video buffers, it is up to the receiver of
     * the video buffers to interpret the contents and to find the actual frame
     * data with the help of the meta data in the buffer. How this is done is
     * outside of the scope of this method.
     *
     * Some camera HALs may not support storing meta data in the video buffers,
     * but all camera HALs should support storing real YUV data in the video
     * buffers. If the camera HAL does not support storing the meta data in the
     * video buffers when it is requested to do do, INVALID_OPERATION must be
     * returned. It is very useful for the camera HAL to pass meta data rather
     * than the actual frame data directly to the video encoder, since the
     * amount of the uncompressed frame data can be very large if video size is
     * large.
     *
     * @param enable if true to instruct the camera HAL to store
     *        meta data in the video buffers; false to instruct
     *        the camera HAL to store real YUV data in the video
     *        buffers.
     *
     * @return OK on success.
     */
    virtual int storeMetaDataInBuffers(int enable) = 0;

    /**
     * Start record mode. When a record image is available, a
     * CAMERA_MSG_VIDEO_FRAME message is sent with the corresponding
     * frame. Every record frame must be released by a camera HAL client via
     * releaseRecordingFrame() before the client calls
     * disableMsgType(CAMERA_MSG_VIDEO_FRAME). After the client calls
     * disableMsgType(CAMERA_MSG_VIDEO_FRAME), it is the camera HAL's
     * responsibility to manage the life-cycle of the video recording frames,
     * and the client must not modify/access any video recording frames.
     */
    virtual int startRecording() = 0;

    /**
     * Stop a previously started recording.
     */
    virtual void stopRecording() = 0;

    /**
     * Returns true if recording is enabled.
     */
    virtual int recordingEnabled() = 0;

    /**
     * Release a record frame previously returned by CAMERA_MSG_VIDEO_FRAME.
     *
     * It is camera HAL client's responsibility to release video recording
     * frames sent out by the camera HAL before the camera HAL receives a call
     * to disableMsgType(CAMERA_MSG_VIDEO_FRAME). After it receives the call to
     * disableMsgType(CAMERA_MSG_VIDEO_FRAME), it is the camera HAL's
     * responsibility to manage the life-cycle of the video recording frames.
     */
    virtual void releaseRecordingFrame(
        const void *opaque) = 0;

    /**
     * Start auto focus, the notification callback routine is called with
     * CAMERA_MSG_FOCUS once when focusing is complete. autoFocus() will be
     * called again if another auto focus is needed.
     */
    virtual int autoFocus() = 0;

    /**
     * Cancels auto-focus function. If the auto-focus is still in progress,
     * this function will cancel it. Whether the auto-focus is in progress or
     * not, this function will return the focus position to the default.  If
     * the camera does not support auto-focus, this is a no-op.
     */
    virtual int cancelAutoFocus() = 0;

    /**
     * Take a picture.
     */
    virtual int takePicture() = 0;

    /**
     * Cancel a picture that was started with takePicture. Calling this method
     * when no picture is being taken is a no-op.
     */
    virtual int cancelPicture() = 0;

    /**
     * Set the camera parameters. This returns BAD_VALUE if any parameter is
     * invalid or not supported.
     */
    virtual int setParameters( const char *parms) = 0;

    /** Retrieve the camera parameters.  The buffer returned by the camera HAL
        must be returned back to it with put_parameters, if put_parameters
        is not NULL.
     */
    virtual char* getParameters() = 0;

    /** The camera HAL uses its own memory to pass us the parameters when we
        call get_parameters.  Use this function to return the memory back to
        the camera HAL, if put_parameters is not NULL.  If put_parameters
        is NULL, then you have to use free() to release the memory.
    */
    virtual void putParameters(char *) = 0;

    /**
     * Send command to camera driver.
     */
    virtual int sendCommand(
        int32_t cmd, int32_t arg1, int32_t arg2) = 0;

    /**
     * Release the hardware resources owned by this object.  Note that this is
     * *not* done in the destructor.
    */
    virtual void release() = 0;

    /**
     * Dump state of the camera hardware
     */
    virtual int dump( int fd) const = 0;

    // Wraps a given STECameraDeviceBase instance into a camera_device
    static camera_device_t* CreateCameraDevice(STECameraDeviceBase* aCameraDevice);
private:
    STECameraDeviceBase(const STECameraDeviceBase &);
    STECameraDeviceBase &operator=(const STECameraDeviceBase &);

}; // class STECameraDeviceBase


}; // namespace android

#endif //STECAMERA_DEVICE_BASE_H
