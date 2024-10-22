/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface */

#ifndef _Included_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
#define _Included_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
#ifdef __cplusplus
extern "C" {
#endif
#undef com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_ERROR_SHORT_DESCRIPTION
#define com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_ERROR_SHORT_DESCRIPTION 1L
#undef com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_ERROR_LONG_DESCRIPTION
#define com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_ERROR_LONG_DESCRIPTION 2L
#undef com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_QUEUE_SIZE
#define com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_QUEUE_SIZE 10L
/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    getLoaderErrorDescription
 * Signature: (I)I
 */
JNIEXPORT jstring JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_getLoaderErrorDescription
  (JNIEnv *, jobject, jint);
  
/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    initialize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_initialize
  (JNIEnv *, jobject);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    deInitialize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_deInitialize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSetTimeouts
 * Signature: (JIIII)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSetTimeouts
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    setLCDPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_setLCDPath
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    setLCMPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_setLCMPath
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    readReply
 * Signature: ([BJJJ)V
 */
JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_readReply
  (JNIEnv *, jobject, jbyteArray, jlong, jlong, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    writeReply
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_writeReply
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemLoaderStartupStatus
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemLoaderStartupStatus
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemChangeBaudRate
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemChangeBaudRate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemReboot
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemReboot
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemShutdown
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemShutdown
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemSupportedCommands
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemSupportedCommands
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemExecuteSoftware
 * Signature: (JLjava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemExecuteSoftware
  (JNIEnv *, jobject, jlong, jstring, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemAuthenticate
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemAuthenticate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemDeauthenticate
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemDeauthenticate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemGetControlKeys
 * Signature: (J[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemGetControlKeys
  (JNIEnv *, jobject, jlong, jobjectArray);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemAuthenticationChallenge
 * Signature: (JI[B)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemAuthenticationChallenge
  (JNIEnv *, jobject, jlong, jint, jbyteArray);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSystemCollectData
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemCollectData
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashProcessFile
 * Signature: (JLjava/lang/String;Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashProcessFile
  (JNIEnv *, jobject, jlong, jstring, jstring, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashListDevices
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashListDevices
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashDumpArea
 * Signature: (JLjava/lang/String;JJLjava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashDumpArea
  (JNIEnv *, jobject, jlong, jstring, jlong, jlong, jstring, jint, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashEraseArea
 * Signature: (JLjava/lang/String;JJ)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashEraseArea
  (JNIEnv *, jobject, jlong, jstring, jlong, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashSetEnhancedArea
 * Signature: (JLjava/lang/String;JJ)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashSetEnhancedArea
  (JNIEnv *, jobject, jlong, jstring, jlong, jlong);

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashSelectLoaderOptions
  (JNIEnv *, jobject, jlong, jlong, jlong);


/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFlashSelectLoaderOptions
 * Signature: (JJJ)I
 *
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashSelectLoaderOptions
  (JNIEnv *, jobject, jlong, jlong);
*/


/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface 
 * Method:    nFSVolumeProperties
 * Signature: (JLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSVolumeProperties
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSFormatVolume
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSFormatVolume
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSListDirectory
 * Signature: (JLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSListDirectory
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSMoveFile
 * Signature: (JLjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSMoveFile
  (JNIEnv *, jobject, jlong, jstring, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSDeleteFile
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSDeleteFile
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSCopyFile
 * Signature: (JLjava/lang/String;Ljava/lang/String;ZZ)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSCopyFile
  (JNIEnv *, jobject, jlong, jstring, jstring, jboolean, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSCreateDirectory
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSCreateDirectory
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSProperties
 * Signature: (JLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSProperties
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSChangeAccess
 * Signature: (JLjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSChangeAccess
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nFSReadLoadModuleManifests
 * Signature: (JLjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSReadLoadModuleManifests
  (JNIEnv *, jobject, jlong, jstring, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nOTPReadBits
 * Signature: (JIII)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPReadBits
  (JNIEnv *, jobject, jlong, jint, jint, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nOTPSetBits
 * Signature: (JIII[B)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPSetBits
  (JNIEnv *, jobject, jlong, jint, jint, jint, jbyteArray);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nOTPWriteAndLockBits
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPWriteAndLockBits
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nOTPStoreSecurityObject
 * Signature: (JLjava/lang/String;IZ)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPStoreSecurityObject
  (JNIEnv *, jobject, jlong, jstring, jint, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nPSReadGlobalDataUnit
 * Signature: (JLjava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSReadGlobalDataUnit
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nPSWriteGlobalDataUnit
 * Signature: (JLjava/lang/String;I[BI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSWriteGlobalDataUnit
  (JNIEnv *, jobject, jlong, jstring, jint, jbyteArray, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nPSReadGlobalDataSet
 * Signature: (JLjava/lang/String;Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSReadGlobalDataSet
  (JNIEnv *, jobject, jlong, jstring, jstring, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nPSWriteGlobalDataSet
 * Signature: (JLjava/lang/String;JLjava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSWriteGlobalDataSet
  (JNIEnv *, jobject, jlong, jstring, jlong, jstring, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nPSEraseGlobalDataSet
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSEraseGlobalDataSet
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecuritySetDomain
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecuritySetDomain
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecurityGetDomain
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityGetDomain
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecurityGetProperties
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityGetProperties
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecuritySetProperties
 * Signature: (JII[B)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecuritySetProperties
  (JNIEnv *, jobject, jlong, jint, jint, jbyteArray);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecurityBindProperties
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityBindProperties
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecurityWriteRpmbKey
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityWriteRpmbKey
  (JNIEnv *, jobject, jlong, jint, jboolean);

/*
 * Class:     com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface
 * Method:    nSecurityInitArbTable
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityInitArbTable
  (JNIEnv *, jobject, jlong, jint, jint, jbyteArray);

#ifdef __cplusplus
}
#endif
#endif
