/*
 * javadefs.h
 *
 *  Created on: 14.1.2010
 *      Author: rohacmic
 */

#ifndef LCDDEFS_H_
#define LCDDEFS_H_



#define LCD_INITIALIZE "Initialize"
#define LCD_CLOSE "Close"
#define LCD_SET_TIMEOUTS "SetTimeouts"
#define LCD_GET_TIMEOUTS "GetTimeouts"
#define LCD_CANCEL_CURRENT_LOADER_COMMAND "CancelCurrentLoaderCommand"
#define LCD_GET_LOADER_ERROR_DESCRIPTION "GetLoaderErrorDescription"

		// SYSTEM
#define LCD_SYSTEM_LOADER_STARTUP_STATUS "System_LoaderStartupStatus"
#define LCD_SYSTEM_CHANGE_BAUDRATE "System_ChangeBaudrate"
#define LCD_SYSTEM_REBOOT "System_Reboot"
#define LCD_SYSTEM_SHUTDOWN "System_Shutdown"
#define LCD_SYSTEM_SUPPORTED_COMMANDS "System_SupportedCommands"
#define LCD_SYSTEM_EXECUTE_SOFTWARE "System_ExecuteSoftware"
#define LCD_SYSTEM_AUTHENTICATE "System_Authenticate"
#define LCD_SYSTEM_DEAUTHENTICATE "System_Deauthenticate"
#define LCD_SYSTEM_GET_CONTROLKEYS "System_GetControlKeys" //maybe obsolete
#define LCD_SYSTEM_GET_CONTROLKEYS_DATA "System_GetControlKeysData"
#define LCD_SYSTEM_AUTHENTICATION_CHALLENGE "System_AuthenticationChallenge"
#define LCD_SYSTEM_COLLECT_DATA "System_CollectData"

		// FLASH
#define LCD_FLASH_PROCESS_FILE "Flash_ProcessFile"
#define LCD_FLASH_LIST_DEVICES "Flash_ListDevices"
#define LCD_FLASH_DUMP_AREA "Flash_DumpArea"
#define LCD_FLASH_ERASE_AREA "Flash_EraseArea"
#define LCD_FLASH_SET_ENHANCED_AREA "Flash_SetEnhancedArea"
#define LCD_FLASH_SELECT_LOADER_OPTIONS "Flash_SelectLoaderOptions"

		// FILE SYSTEM
// #define LCD_FILE_SYSTEM_SET_READ_ONLY "FileSystem_SetReadOnly"
#define LCD_FILE_SYSTEM_VOLUME_PROPERTIES "FileSystem_VolumeProperties"
#define LCD_FILE_SYSTEM_FORMAT_VOLUME "FileSystem_FormatVolume"
#define LCD_FILE_SYSTEM_LIST_DIRECTORY "FileSystem_ListDirectory"
#define LCD_FILE_SYSTEM_MOVE_FILE "FileSystem_MoveFile"
#define LCD_FILE_SYSTEM_DELETE_FILE "FileSystem_DeleteFile"
#define LCD_FILE_SYSTEM_COPY_FILE "FileSystem_CopyFile"
#define LCD_FILE_SYSTEM_CREATE_DIRECTORY "FileSystem_CreateDirectory"
#define LCD_FILE_SYSTEM_PROPERTIES "FileSystem_Properties"
#define LCD_FILE_SYSTEM_CHANGE_ACCESS "FileSystem_ChangeAccess"
#define LCD_FILE_SYSTEM_READ_LOADMODULE_MANIFESTS "FileSystem_ReadLoadModuleManifests"

		// OTP
#define LCD_OTP_READ_BITS "OTP_ReadBits"
#define LCD_OTP_SET_BITS "OTP_SetBits"
#define LCD_OTP_WRITE_AND_LOCK "OTP_WriteAndLock"

		// PARAMETER STORAGE
#define LCD_PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT "ParameterStorage_ReadGlobalDataUnit"
#define LCD_PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT "ParameterStorage_WriteGlobalDataUnit"
#define LCD_PARAMETER_STORAGE_READ_GLOBAL_DATA_SET "ParameterStorage_ReadGlobalDataSet"
#define LCD_PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET "ParameterStorage_WriteGlobalDataSet"
#define LCD_PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET "ParameterStorage_EraseGlobalDataSet"

		// SECURITY
#define LCD_SECURITY_SET_DOMAIN "Security_SetDomain"
#define LCD_SECURITY_GET_DOMAIN "Security_GetDomain"
#define LCD_SECURITY_GET_PROPERTIES "Security_GetProperties"
#define LCD_SECURITY_SET_PROPERTIES "Security_SetProperties"
#define LCD_SECURITY_BIND_PROPERTIES "Security_BindProperties"
#define LCD_SECURITY_WRITE_RPMB_KEY "Security_WriteRPMBKey"
#define LCD_SECURITY_STORE_SECURE_OBJECT "Security_StoreSecureObject"
#define LCD_SECURITY_INIT_ARB_TABLE "Security_InitARBTable"

		// RESET
// #define LCD_RESET_INIT_REQUEST "Reset_InitRequest"
// #define LCD_RESET_UPDATE_REQUEST "Reset_UpdateRequest"

#endif /* LCDDEFS_H_ */
