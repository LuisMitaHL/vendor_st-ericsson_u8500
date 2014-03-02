#include <android/log.h>

#define PCSC_LOGE(...) __android_log_print(ANDROID_LOG_ERROR , "STE_PCSC", __VA_ARGS__)

#ifdef DEBUG
#define PCSC_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "STE_PCSC", __VA_ARGS__)
#define PCSC_LOGI(...) __android_log_print(ANDROID_LOG_INFO , "STE_PCSC", __VA_ARGS__)
#else
#define PCSC_LOGD(...) do { } while (0)
#define PCSC_LOGI(...) do { } while (0)
#endif /* DEBUG */
