/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//Internal includes
#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 3
#define DBGT_PREFIX "NevenFaceDetector"

#include "NevenFaceDetector.h"
#include "STECamTrace.h"

namespace android {

NevenFaceDetector::NevenFaceDetector() {
    DBGT_PROLOG("");

    mMaxWidth = 0;
    mMaxHeight = 0;
    mMaxFaces = 0;
    mCurFrameHeight = 0;
    mCurFrameWidth = 0;
    mNumberOfFacesDetected = 0;
    mNumOfValidFaces = 0;
    mInputOmxColorFormat = OMX_COLOR_FormatUnused;
    mSdk = NULL;
    mDcr = NULL;
    mHfd = NULL;
    mFaceDetectionThread = 0;
    mIsFrameInProcessing = false;
    mRunFaceDetection = true;
    pthread_mutex_init(&mMutexFaceDetection, NULL);
    sem_init(&mSemStartFaceDetection, 0, 0);
    pthread_create(&mFaceDetectionThread, NULL, faceDetectionThread, this);

    DBGT_EPILOG("");
}

NevenFaceDetector::~NevenFaceDetector() {
    DBGT_PROLOG("");

    exitFaceDetectionThread(false);
    pthread_join(mFaceDetectionThread, NULL);

    // Close sequence should be hfd->dcr->sdk.
    // Do not change this close sequence.
    btk_FaceFinder_close( mHfd );
    btk_DCR_close( mDcr );
    btk_SDK_close( mSdk );

    pthread_mutex_destroy(&mMutexFaceDetection);
    sem_destroy(&mSemStartFaceDetection);

    mMem.clear();
    mHeap.clear();

    DBGT_EPILOG("");
}

bool NevenFaceDetector::isColorFormatSupported(
        OMX_COLOR_FORMATTYPE aOmxColor) {

    DBGT_PROLOG("Color format %d", (int)aOmxColor);

    bool flag = false;
    if((aOmxColor == OMX_COLOR_Format16bitRGB565) ||
            (aOmxColor == OMX_COLOR_FormatYUV420SemiPlanar) ||
           (aOmxColor == OMX_COLOR_FormatCbYCrY)){
        flag = true;
    }

    DBGT_EPILOG("flag = %d", flag);
    return flag;
}

int NevenFaceDetector::init(int aMaxWidth,
        int aMaxHeight,
        int aMaxFaces,
        OMX_COLOR_FORMATTYPE aOmxInputColorFormat) {

    DBGT_PROLOG("");

    if((aMaxFaces == 0)
            || (aMaxHeight == 0)
            || (aMaxWidth == 0)
            || !isColorFormatSupported(aOmxInputColorFormat)){
        DBGT_CRITICAL("Invalid input parameters");
        DBGT_EPILOG("");
        return -1;
    }

    if(aMaxFaces > MAX_FACES) {
        DBGT_PTRACE("max faces greater than MAX_FACES (%d)", MAX_FACES);
        DBGT_PTRACE("Setting max faces to %d", MAX_FACES);
        aMaxFaces = MAX_FACES;
    }

    mMaxHeight = aMaxHeight;
    mMaxWidth = aMaxWidth;
    mMaxFaces = aMaxFaces;
    mInputOmxColorFormat = aOmxInputColorFormat;

    const int MAX_FILE_SIZE = 65536;
    void* initData = malloc( MAX_FILE_SIZE ); /*  enough to fit entire file */
    //const char path[64] = "/system/usr/share/bmd/RFFstd_501.bmd";
    const char path[64] = "/system/usr/share/bmd/RFFspeed_501.bmd";
    int filedesc = open(path, O_RDONLY);
    DBGT_PTRACE("filedesc %d", filedesc);
    int initDataSize = read(filedesc, initData, MAX_FILE_SIZE);
    DBGT_PTRACE("initDataSize %d", initDataSize);
    close(filedesc);

    btk_SDKCreateParam sdkParam = btk_SDK_defaultParam();
    sdkParam.fpMalloc = malloc;
    sdkParam.fpFree = free;
    sdkParam.maxImageWidth = mMaxWidth;
    sdkParam.maxImageHeight = mMaxHeight;

    btk_Status status = btk_SDK_create(&sdkParam, &mSdk);
    // make sure everything went well
    if (status != btk_STATUS_OK) {
        DBGT_CRITICAL("btk_SDK_create failed %d", status);
        DBGT_EPILOG("");
        return 0;
    }

    btk_DCRCreateParam dcrParam = btk_DCR_defaultParam();
    btk_DCR_create( mSdk, &dcrParam, &mDcr );

    btk_FaceFinderCreateParam fdParam = btk_FaceFinder_defaultParam();
    fdParam.pModuleParam = initData;
    fdParam.moduleParamSize = initDataSize;
    fdParam.maxDetectableFaces = mMaxFaces;
    status = btk_FaceFinder_create( mSdk, &fdParam, &mHfd );
    btk_FaceFinder_setRange(mHfd, 20, mMaxWidth/2); /*  set eye distance range */

    // make sure everything went well
    if (status != btk_STATUS_OK) {
        DBGT_CRITICAL("btk_FaceFinder_create failed %d", status);
        DBGT_EPILOG("");
        return 0;
    }

    DBGT_PTRACE("sdk %p dcr %p hfd %p", mSdk,mDcr,mHfd);
    // free the configuration file
    free(initData);

    // output buffer color format is yuv400
    int bufferSize = mMaxWidth * mMaxHeight;
    mHeap = new MemoryHeapBase(bufferSize);
    if(mHeap != NULL) {
        mMem = new MemoryBase(mHeap, 0, bufferSize);
        if(mMem == NULL){
            DBGT_CRITICAL("Failed to allocate MemoryBase");
            status = btk_STATUS_ERROR;
        }
    } else {
        DBGT_CRITICAL("Failed to allocate MemoryHeapBase");
        status = btk_STATUS_ERROR;
    }

    DBGT_EPILOG("status = %d", (int)status);
    return (int)status;
}

int NevenFaceDetector::postInputFrame(char *buffer, int bufflen, int aWidth, int aHeight) {

    DBGT_PROLOG("buffer %p bufflen %d width %d height %d"
            ,buffer, bufflen, aWidth, aHeight);

    if((aWidth > mMaxWidth) || (aHeight > mMaxHeight)) {
        DBGT_CRITICAL("Width %d Height %d greater than MAX", aWidth, aHeight);
        DBGT_EPILOG("");
        return -1;
    }

    int status = 0;
    //If face detection thread is NOT busy
    //then post new frame for processing
    if(!IsFaceDetectionThreadBusy()) {
        if(mInputOmxColorFormat == OMX_COLOR_Format16bitRGB565) {
            //Copy frame data to local buffer
            //face detection thread will use
            //local buffer for processing
            // convert the image to B/W
            unsigned char * dst = (unsigned char *)mHeap->base();
            unsigned short const* src = (unsigned short const*)buffer;
            int wpr = aWidth;
            for (int y=0 ; y<aHeight; y++) {
                for (int x=0 ; x<aWidth ; x++) {
                    uint16_t rgb = src[x];
                    int r  = rgb >> 11;
                    int g2 = (rgb >> 5) & 0x3F;
                    int b  = rgb & 0x1F;
                    // L coefficients 0.299 0.587 0.11
                    int L = (r<<1) + (g2<<1) + (g2>>1) + b;
                    *dst++ = L;
                }
                src += wpr;
            }
        } else if (mInputOmxColorFormat == OMX_COLOR_FormatCbYCrY) {
                unsigned char * dst = (unsigned char *)mHeap->base();
                unsigned char * src = (unsigned char *)buffer;
                    for (int y=0 ; y<aHeight; y++) {
                        for (int x=0 ; x<aWidth ; x++) {
                            dst[y*aWidth+x] = src[(y*aWidth+x)*2+1];
                        }
                    }
        } else {
            //else copy only luma
            unsigned int len = aWidth * aHeight;
            memcpy(mHeap->base(),buffer,len);
        }

        mCurFrameWidth = aWidth;
        mCurFrameHeight = aHeight;

        mScaleX = 2000 / aWidth;
        mScaleY = 2000 / aHeight;
        DBGT_PTRACE("scaleX %f scaleY %f", mScaleX, mScaleY);

        sem_post(&mSemStartFaceDetection);
        status = 0;
    } else {
        DBGT_PTRACE("Thread busy!!!! skip frame....");
        status = 1;
    }

    DBGT_EPILOG("status = %d", status);
    return status;
}



int NevenFaceDetector::processInputFrame() {
    DBGT_PROLOG("");

    btk_Status status = btk_STATUS_OK;
    btk_DCR_assignGrayByteImage(mDcr, mHeap->base(), mCurFrameWidth, mCurFrameHeight);

    status = btk_FaceFinder_putDCR(mHfd, mDcr);
    if (status == btk_STATUS_OK) {
        mNumberOfFacesDetected = btk_FaceFinder_faces(mHfd);
        DBGT_PTRACE("Number of faces detected %d", mNumberOfFacesDetected);
        if(mNumberOfFacesDetected > MAX_FACES) {
            DBGT_CRITICAL("!!!!!ERROR : FACES DETECTED (%d) > MAX_FACES!!!!",
                    mNumberOfFacesDetected);
            mNumberOfFacesDetected = MAX_FACES;
        }
        if (mNumberOfFacesDetected > 0) {
            getFaceData();
        }
    } else {
        DBGT_CRITICAL("ERROR: Return 0 faces because error exists in btk_FaceFinder_putDCR.\n");
    }

    DBGT_EPILOG("status = %d", status);
    return status;
}

void NevenFaceDetector::getFaceData() {
    DBGT_PROLOG("");
    int i = mNumberOfFacesDetected;
    int faceIndex = 0;
    while(i>0) {
        btk_FaceFinder_getDCR(mHfd, mDcr);

        float confidence = (float)btk_DCR_confidence(mDcr) / (1 << 24);
        DBGT_PTRACE("Confidence %f", confidence);
        if(confidence >= 0.3) {

            btk_Node leftEye, rightEye;

            btk_DCR_getNode(mDcr, 0, &leftEye);
            btk_DCR_getNode(mDcr, 1, &rightEye);

            mFaces[faceIndex].score = (float)confidence * 10.0;
            mFaces[faceIndex].id = faceIndex;

            mFaces[faceIndex].left_eye[0] = -2000; //(leftEye.x) / (1 << 16);
            mFaces[faceIndex].left_eye[1] = -2000; //(leftEye.y) / (1 << 16);

            mFaces[faceIndex].right_eye[0] = -2000; //(rightEye.x) / (1 << 16);
            mFaces[faceIndex].right_eye[1] = -2000; //(rightEye.y) / (1 << 16);

            mFaces[faceIndex].mouth[0] = -2000; //(rightEye.x + leftEye.x) / (1 << 17);
            mFaces[faceIndex].mouth[1] = -2000; //(rightEye.y + leftEye.y) / (1 << 17);

            float eyeDist = (float)(rightEye.x - leftEye.x) / (1 << 16);
            float midpointx = (float)(rightEye.x + leftEye.x) / (1 << 17);
            float midpointy = (float)(rightEye.y + leftEye.y) / (1 << 17);
            DBGT_PTRACE("midX %f mixY %f eyeDist %f", midpointx, midpointy, eyeDist);

            float x1 = ((midpointx - mCurFrameWidth) * mScaleX) + 1000;
            float y1 = ((midpointy  - mCurFrameHeight) * mScaleY) + 1000;
            eyeDist = eyeDist * mScaleX;
            DBGT_PTRACE("x1 %f y1 %f eyeDist %f", x1, y1, eyeDist);

            mFaces[faceIndex].rect[0] = x1 - eyeDist*2;
            mFaces[faceIndex].rect[1] = y1 - eyeDist*2;
            mFaces[faceIndex].rect[2] = x1 + eyeDist*2;
            mFaces[faceIndex].rect[3] = y1 + eyeDist*2;

            DBGT_PTRACE("x1 %d y1 %d x2 %d y2 %d",
                mFaces[faceIndex].rect[0],
                mFaces[faceIndex].rect[1],
                mFaces[faceIndex].rect[2],
                mFaces[faceIndex].rect[3]);

            faceIndex++;
        }

        i--;
    }

    mNumOfValidFaces = faceIndex;
    DBGT_EPILOG("");
}
void *faceDetectionThread(void *arg) {

    DBGT_PROLOG("");

    NevenFaceDetector *steFaceDetect = static_cast<NevenFaceDetector *>(arg);
    steFaceDetect->waitForNextFrame();
    while(steFaceDetect->runFaceDetection()) {
        steFaceDetect->processInputFrame();
        steFaceDetect->allowNextFrame();
        steFaceDetect->waitForNextFrame();
    }

    pthread_exit(NULL);

    DBGT_EPILOG("");
    return NULL;
}

void NevenFaceDetector::waitForNextFrame() {
    DBGT_PROLOG("Face detection thread waiting for next input frame");
    sem_wait(&mSemStartFaceDetection);
    DBGT_EPILOG("New input frame arrived...");
}

bool NevenFaceDetector::runFaceDetection() {
    DBGT_PROLOG("");

    bool status = false;
    pthread_mutex_lock(&mMutexFaceDetection);
    status = mRunFaceDetection;
    pthread_mutex_unlock(&mMutexFaceDetection);

    DBGT_EPILOG("status = %d", status);
    return status;
}

void NevenFaceDetector::exitFaceDetectionThread(bool aFlag) {

    DBGT_PROLOG("");

    pthread_mutex_lock(&mMutexFaceDetection);
    mRunFaceDetection = aFlag;
    pthread_mutex_unlock(&mMutexFaceDetection);

    //Don't wait for next input frame
    sem_post(&mSemStartFaceDetection);

    DBGT_EPILOG("");
}


void NevenFaceDetector::allowNextFrame() {
    DBGT_PROLOG("");

    pthread_mutex_lock(&mMutexFaceDetection);

    mIsFrameInProcessing = false;

    pthread_mutex_unlock(&mMutexFaceDetection);

    DBGT_EPILOG("");
}

bool NevenFaceDetector::IsFaceDetectionThreadBusy() {
    DBGT_PROLOG("");
    bool status = false;

    pthread_mutex_lock(&mMutexFaceDetection);

    if(mIsFrameInProcessing) {

        // face detection thread is busy in
        // processing frame
        status = true;
    } else {
        // face detection thread is free,
        // send next frame for processing
        status = false;
        mIsFrameInProcessing = true;
    }
    pthread_mutex_unlock(&mMutexFaceDetection);

    DBGT_EPILOG("status = %d", status);
    return status;
}

void NevenFaceDetector::getFaces(camera_frame_metadata_t *aFrameMetaData) {
    DBGT_PROLOG("");

    if(mNumOfValidFaces > 0) {
        aFrameMetaData->number_of_faces = mNumOfValidFaces;
        memcpy(aFrameMetaData->faces,
                mFaces,
                sizeof(camera_face_t) * mNumOfValidFaces);
    } else {
        aFrameMetaData->number_of_faces = 0;
    }
    DBGT_EPILOG("");

}

} //namespace android
