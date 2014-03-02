/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMPROP_H
#define STECAMPROP_H
#include <OMX_Symbian_CameraExt_Ste.h>

namespace android {

#define uint32 unsigned int

typedef struct evcompensationproperty {
    float stepindex;
    long int compensation_value;
}evcompensationproperty;

//uint32 ev_compensation[13] = {-131072,-109226,-87381,-65536,-43690,-21845,0,21845,43690,65536,87381,109226,131072};

typedef struct wbproperty {
    int iMarkendofdata;
    const char* wb;
    OMX_WHITEBALCONTROLTYPE eWBType;
}wbproperty;

typedef struct focusproperty {
    int iMarkendofdata;
    const char* focus;
    OMX_IMAGE_FOCUSCONTROLTYPE efocusType;
}focusproperty;

typedef struct flashproperty {
    int iMarkendofdata;
    const char* flash;
    OMX_IMAGE_FLASHCONTROLTYPE eflashType;
}flashproperty;

typedef struct flickerproperty {
    int iMarkendofdata;
    const char* flicker;
    OMX_SYMBIAN_FLICKERREMOVALTYPE eflickerType;
}flickerproperty;

typedef struct scenemodeproperty {
    int iMarkendofdata;
    const char* scenemode;
    OMX_SYMBIAN_SCENEMODETYPE eScenemodeType;
}scenemodeproperty;

typedef struct detailscenemodeproperty {
    int iMarkendofdata;
    const char* scenemode;
    int iSceneMode;
}detailscenemodeproperty;

typedef struct imagefilterproperty {
    int iMarkendofdata;
    const char* imagefilter;
    OMX_IMAGEFILTERTYPE  eImageFilter;
}imagefilterproperty;

typedef struct meteringProperty {
    int iMarkendofdata;
    const char* metering;
    OMX_METERINGTYPE  eMetering;
}meteringProperty;

typedef struct RegionControlProperty {
    int iMarkendofdata;
    const char* regioncontrol;
    OMX_SYMBIAN_FOCUSREGIONCONTROL  eRegion;
}RegionControlProperty;

typedef struct RegionExtFocus {
    OMX_SYMBIAN_CONFIG_EXTFOCUSSTATUSTYPE mFocusStatus;
    OMX_SYMBIAN_AFROITYPE mFiller[OMX_SYMBIAN_MAX_NUMBER_OF_ROIS-1];
}RegionExtFocus;

typedef struct FocusMode {
    int iMarkendofdata;
    const char* focus;
    OMX_IMAGE_FOCUSCONTROLTYPE eFocusType;
    OMX_SYMBIAN_FOCUSRANGETYPE eRangeType;
}FocusMode;

};
#endif // STECAMPROP_H
