#ifndef _STE_FACE_DETECTOR_H_
#define _STE_FACE_DETECTOR_H_

#include "STEFaceDetectorBase.h"

extern "C" {
#include <fd_emb_sdk.h>
}

namespace android {

void *faceDetectionThread(void *arg) ;

class NevenFaceDetector : public STEFaceDetectorBase {
public:
    NevenFaceDetector();
    ~NevenFaceDetector();
    int init(int aMaxWidth,
            int aMaxHeight,
            int aMaxFaces,
            OMX_COLOR_FORMATTYPE aInputColorFormat);

    int postInputFrame(char *buffer,
            int bufflen,
            int aWidth,
            int aHeight);

    void getFaces(
            camera_frame_metadata_t *aFrameMetaData);

    int processInputFrame();
    void waitForNextFrame();
    bool runFaceDetection();
    void exitFaceDetectionThread(bool aFlag);
    void allowNextFrame();
    bool IsFaceDetectionThreadBusy();
    void getFaceData();
    bool isColorFormatSupported(OMX_COLOR_FORMATTYPE aOmxColor);

private:
    //Input frame information
    OMX_COLOR_FORMATTYPE mInputOmxColorFormat;
    int mMaxHeight;
    int mMaxWidth;
    int mMaxFaces;
    int mCurFrameHeight;
    int mCurFrameWidth;
    float mScaleX, mScaleY;

    //Neven library interface
    btk_HSDK mSdk;
    btk_HDCR mDcr;
    btk_HFaceFinder mHfd;

    //Thread variables
    pthread_t mFaceDetectionThread;
    sem_t mSemStartFaceDetection;
    bool mIsFrameInProcessing;
    bool mRunFaceDetection;
    pthread_mutex_t mMutexFaceDetection;

    sp <MemoryHeapBase> mHeap;
    sp <MemoryBase> mMem;

    int mNumberOfFacesDetected;
    int mNumOfValidFaces;   //faces with confidence > 0.2
    camera_face_t mFaces[MAX_FACES];
}; //class NevenFaceDetector

}  // namespace android

#endif  //_STE_FACE_DETECTOR_H_
