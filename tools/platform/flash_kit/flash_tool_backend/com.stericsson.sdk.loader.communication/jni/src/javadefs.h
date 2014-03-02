/*
 * javadefs.h
 *
 *  Created on: 14.1.2010
 *      Author: rohacmic
 */

#ifndef JAVADEFS_H_
#define JAVADEFS_H_

#define EX_EXCEPTION "java/lang/Exception"

#define CLASS_LOADER_COMMUNICATION_INTERFACE "com/stericsson/sdk/loader/communication/internal/LoaderCommunicationInterface"
#define CLS_LOADER_STARTUP_TYPE "com/stericsson/sdk/loader/communication/types/LoaderStartupType"
#define CLS_SUPPORTED_CMDS_TYPE "com/stericsson/sdk/loader/communication/types/SupportedCommandsType"
#define CLS_AUTHENTICATION_TYPE "com/stericsson/sdk/loader/communication/types/AuthenticationType"
#define CLS_LIST_DEVICES_TYPE "com/stericsson/sdk/loader/communication/types/ListDevicesType"
#define CLS_VOLUME_PROPS_TYPE "com/stericsson/sdk/loader/communication/types/VolumePropertiesType"
#define CLS_LIST_DIR_TYPE "com/stericsson/sdk/loader/communication/types/ListDirectoryType"
#define CLS_FS_PROPS_TYPE "com/stericsson/sdk/loader/communication/types/FSPropertiesType"
#define CLS_OTP_READ_BITS_TYPE "com/stericsson/sdk/loader/communication/types/OTPReadBitsType"
#define CLS_PSREAD_GLOBAL_DATA_UNIT_TYPE "com/stericsson/sdk/loader/communication/types/PSReadGlobalDataUnitType"
#define CLS_DOMAIN_TYPE "com/stericsson/sdk/loader/communication/types/DomainType"
#define CLS_SECURITY_PROPS_TYPE "com/stericsson/sdk/loader/communication/types/SecurityPropertiesType"
#define CLS_INIT_REQUEST_TYPE "com/stericsson/sdk/loader/communication/types/InitRequestType"
#define CLS_SYSTEM_COLLECT_DATA_TYPE "com/stericsson/sdk/loader/communication/types/SystemCollectDataType"
#define CLASS_TEST "com/stericsson/sdk/loader/communication/internal/xtclass"

#define METHOD_CONSTRUCTOR "<init>"

//Constructors' signatures
#define METHOD_CONST_LOADER_STARTUP_TYPE_SIG "(ILjava/lang/String;ILjava/lang/String;I)V"
#define METHOD_CONST_SUPPORTED_CMDS_TYPE_SIG "(II)V"
#define METHOD_CONST_AUTHENTICATION_TYPE_CONTROL_KEYS_SIG "(I)V"
#define METHOD_CONST_AUTHENTICATION_TYPE_CHALLENGE_SIG "(II[B)V"
#define METHOD_CONST_DEVICES_TYPE_SIG "(II)V"
#define METHOD_CONST_VOLUME_PROPS_TYPE_SIG "(ILjava/lang/String;IJJ)V"
#define METHOD_CONST_LIST_DIR_TYPE_SIG "(II)V"
#define METHOD_CONST_FS_PROPS_TYPE_SIG "(IIJIII)V"
#define METHOD_CONST_OTP_READ_BITS_TYPE_SIG "(I[B[B)V"
#define METHOD_CONST_PSREAD_GLOBAL_DATA_UNIT_TYPE_SIG "(I[B)V"
#define METHOD_CONST_DOMAIN_TYPE_SIG "(II)V"
#define METHOD_CONST_SECURITY_PROPS_TYPE_SIG "(I[B)V"
#define METHOD_CONST_INIT_REQUEST_TYPE_SIG "(I[B)V"
#define METHOD_CONST_SYSTEM_COLLECT_DATA_TYPE_SIG "(I[B)V"


//LoaderCommunicationInterface methods and signatures
#define SMETHOD_LCI_READ "read"
#define SMETHOD_LCI_READ_SIG "(IJJ)V"
#define SMETHOD_LCI_WRITE "write"
#define SMETHOD_LCI_WRITE_SIG "([BIJJ)V"
#define SMETHOD_LCI_CANCEL "cancel"
#define SMETHOD_LCI_CANCEL_SIG "(J)V"
#define SMETHOD_LCI_PROGRESS "progress"
#define SMETHOD_LCI_PROGRESS_SIG "(JJJ)V"
//#define SMETHOD_LCI_MESSAGE "message"
//#define SMETHOD_LCI_MESSAGE_SIG "(ILjava/lang/String;)V"

#define METHOD_SCMDST_ADD_SUPPORTED_CMD "addSupportedCommand"
#define METHOD_SCMDST_ADD_SUPPORTED_CMD_SIG "(III)V"

//ListDevicesType methods and signatures
#define METHOD_LDEVT_ADD_DEVICE "addDevice"
#define METHOD_LDEVT_ADD_DEVICE_SIG "(Ljava/lang/String;ILjava/lang/String;IJJJ)V"

//ListDirectoryType methods and signatures
#define METHOD_LDIRT_ADD_DIRECTORY "addDirectory"
#define METHOD_LDIRT_ADD_DIRECTORY_SIG "(Ljava/lang/String;IJII)V"

//Log methods and signatures
#define STATIC_METHOD_LCI_LOG "logMessage"
#define STATIC_METHOD_LCI_LOG_SIG "(ILjava/lang/String;)V"

#endif /* JAVADEFS_H_ */
