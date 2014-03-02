#ifndef _STE_FACE_DETECTOR_BASE_H_
#define _STE_FACE_DETECTOR_BASE_H_

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <OMX_IVCommon.h>

#include "STECamTrace.h"
#include "STECamera.h"

namespace android {

/* Maximum faces should be detected in input frame */
#define MAX_FACES   (10)

class STEFaceDetectorBase {
public:
    STEFaceDetectorBase() { }
    virtual ~STEFaceDetectorBase() { }

    // Init face detection thread
    virtual int init(
            int aMaxWidth,
            int aMaxHeight,
            int aMaxFaces,
            OMX_COLOR_FORMATTYPE aInputColorFormat) = 0;

    //Input frames will be post to
    //face detection thread when
    //application send START_FACE_DETECETION
    //command
    virtual int postInputFrame(
            char *buffer,
            int bufflen,
            int aWidth,
            int aHeight) = 0;

    virtual void getFaces(
            camera_frame_metadata_t *aFrameMetaData) = 0;

private:
    STEFaceDetectorBase(STEFaceDetectorBase &);
    STEFaceDetectorBase & operator=(const STEFaceDetectorBase &);
};    //class STEFaceDetectorBase


}  // namespace android

#endif  //_STE_FACE_DETECTOR_BASE_H_

