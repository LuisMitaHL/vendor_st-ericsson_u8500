#ifndef ANDROID_LOG_H
#define ANDROID_LOG_H

#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#ifndef ALOGD
#define ALOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGI
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif


#ifndef ALOGV
#define ALOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGE
#define ALOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGW
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARNING, LOG_TAG, __VA_ARGS__))
#endif

#endif /* ANDROID_LOG_H */