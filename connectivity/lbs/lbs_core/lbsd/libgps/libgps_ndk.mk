#libgps_ndk make file

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(LBS_SET_GNSSCHIP_VER),CG2910)
LOCAL_ENABLE_CG2910 := true
endif

LOCAL_MODULE := libgps
ifeq ($(LOCAL_ENABLE_CG2910),true)
LOCAL_SRC_FILES    :=  CG2910/lib/Android_gcc432_ARMv5te/libgps.a
else
LOCAL_SRC_FILES    :=  CG2900/lib/Android_gcc432_ARMv5te/libgps.a
endif

include $(PREBUILT_STATIC_LIBRARY)








