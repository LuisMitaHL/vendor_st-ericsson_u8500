//-------------------------------------------------------
// LCDInterface.h
// method and type declaration for LCD
//-------------------------------------------------------

#ifndef LCDINTERFACE_H_
#define LCDINTERFACE_H_

#ifdef __LINUX__
  #define UNIX
  #include <dlfcn.h>
  #define LCDRIVER "liblcdriver.so"
#endif

#ifdef __MACOSX__
  #define UNIX
  #include <dlfcn.h>
  #define LCDRIVER "liblcdriver.dylib"
#endif

#ifdef __WIN__
  #include <windows.h>
  #define LCDRIVER "LCDriver_CNH1606432.dll"
#endif

#ifdef _MSC_VER
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
#else
  #include <stdint.h>
#endif

#include <map>
#include <string>
#include <iostream>
#include "JNInative.h"
#include "lcddefs.h"
#include "util.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>

void* m_libd = NULL;

std::string LCDLibPath;
std::string LCMLibPath;

typedef struct {
	int TCACK; /*< Time for command packet acknowledge */
	int TBCR; /*< Time for bulk command packet to be recieved */
	int TBES; /*< Time for bulk session end  */
	int TBDR;
} R15_Timeouts_t;

typedef struct {
	int STO; // StartupTimeOut
	int RTO; // ResponseTimeOut
} LCDriver_Timeouts_t;

struct TSupportedCmd {
	int Group;
	int Command;
	int Permitted;
};

struct TDevices {
	char* Path;

	/**
	 * strlen of Path
	 */
	int PathSize;

	char* Type;

	/**
	 * strlen of Type
	 */
	int TypeSize;

	uint64_t BlockSize;

	uint64_t Start;

	uint64_t Length;
};

struct TEntries {
	char* Name;
	int NameSize;
	uint64_t Size;
	int Mode;
	int Time;
};

struct TCiphers {
	char* Ciper;
	int CiperSize;
};

struct TSIMLockKeys {
	char* szNLCKLock;
	char* szNSLCKLock;
	char* szSPLCKLock;
	char* szCLCKLock;
	char* szPCKLock;
	char* szESLCKLock;

	char* szNLCKUnLock;
	char* szNSLCKUnLock;
	char* szSPLCKUnLock;
	char* szCLCKUnLock;
	char* szPCKUnLock;
	char* szESLCKUnLock;
};

//LCMInstance map <> LCDObject
std::map<int64_t, void*> instanceMap;

// communication callback
typedef void (*CommunicationCallback_t)(const void *Data_p,
		const unsigned long Length, void *Param_p);
typedef void (*MessageCallback_t)(void *Communication_p, int MessageLength,
		char* MessageText);

//global communication callback function
CommunicationCallback_t communicationReadCallback_fn = NULL;
CommunicationCallback_t communicationWriteCallback_fn = NULL;
CommunicationCallback_t communicationCancelCallback_fn = NULL;

//message callback from LCD
MessageCallback_t messageCallbak_fn = NULL;

// pointers to READ, WRITE, CANCEL, PROGRESS
typedef bool (*DeviceRead_fn)(void *Data_p, unsigned long Length,
		CommunicationCallback_t Callback_fn, void *Param_p);
typedef int (*DeviceWrite_fn)(void *Data_p, unsigned long Length,
		CommunicationCallback_t Callback_fn, void *Param_p);
typedef int (*DeviceCancel_fn)(void *Param_p);
typedef void (*ProgressBar_fn)(void *Communication_p,
		uint64_t TotalLength, uint64_t TransferredLength);

void* deviceReadFunction_p;
void* deviceWriteFunction_p;
void* deviceCancelFunction_p;
void* progressFunction_p;
void* messageFunction_p;

//functions exported by LCD
typedef int (*LCD_Initialize)(void **Object, const char *InterfaceId,
		void* Read_fn, void* Write_fn, void* Cancel_fn, void** Instance,
		void* Message_fn, char* LCMLibPath, void* ProgressBar_fn);
typedef int (*LCD_RemoveInstance)(void *Object);
typedef int (*LCD_SetTimeouts)(void * Object, R15_Timeouts_t* R15_TOs,
		LCDriver_Timeouts_t* LCD_TOs);
typedef int (*LCD_GetTimeouts)(void *Object, R15_Timeouts_t** R15_TOs,
		LCDriver_Timeouts_t** LCD_TOs);
typedef int (*LCD_CancelCurrentLoaderCommand)(void *Object);
typedef int (*LCD_MessageCallback)(void *Communication_p, int MessageLength,
		char* MessageText);
typedef int (*LCD_GetLoaderErrorDescription)(int ErrorNr, char *ShortDescription, char *LongDescription, int ShorDescrBufLen, int LongDescrBufLen);

// SYSTEM
typedef int (*System_LoaderStartupStatus)(void *Object, char* Version,
		int* VersionSize, char* Protocol, int* ProtocolSize);
typedef int (*System_ChangeBaudrate)(void *Object, int Baudrate);
typedef int (*System_Reboot)(void *Object, int Mode);
typedef int (*System_Shutdown)(void *Object);
typedef int (*System_Deauthenticate)(void *Object, int permanentFlag);
typedef int (*System_SupportedCommands)(void *Object, TSupportedCmd* CmdList,
		int* CmdListSize);
typedef int (*System_ExecuteSoftware)(void *Object, const char* SourcePath,
		int useBulk);
typedef int (*System_Authenticate)(void *Object, int Type, int* size,
		unsigned char* data);

//this function might be a remove in future if is not used anymore
typedef int (*System_GetControlKeys)(void *Object, TSIMLockKeys* keys);

typedef int (*System_GetControlKeysData)(void *Object, int iDataSize,
		unsigned char *pSIMLockKeysData);
typedef int (*System_AuthenticationChallenge)(void *Object,
		unsigned char* ChallengeData, int DataSize);
typedef int (*System_CollectData)(void *pObject, int iType, int *piSize,
		char *pData);

// FLASH
typedef int (*Flash_ProcessFile)(void *Object, const char* SourcePath,
		const char* type, int useBulk, int DeleteBuffers);
typedef int (*Flash_ListDevices)(void *Object, TDevices* Devices,
		int* DeviceSize);
typedef int (*Flash_DumpArea)(void *Object, const char* PathToDump,
		uint64_t Start, uint64_t Length, const char* FilePath,
		int extraDumpParameters, int useBulk);
typedef int (*Flash_EraseArea)(void *Object, const char* Path,
		uint64_t Start, uint64_t Length);
typedef int (*Flash_SetEnhancedArea)(void *Object, const char* Path,
		uint64_t Start, uint64_t Length);
typedef int (*Flash_SelectLoaderOptions)(void *Object, uint32_t uiProperty, uint32_t uiValue);

// FILE SYSTEM
//typedef int (*FileSystem_SetReadOnly)(void *Object);
typedef int (*FileSystem_VolumeProperties)(void *Object,
		const char* DevicePath, char* FSType, int* FSTypeSize,
		uint64_t* Size, uint64_t* Free);
typedef int (*FileSystem_FormatVolume)(void *Object, const char* DevicePath);
typedef int (*FileSystem_ListDirectory)(void *Object, const char* Path,
		TEntries* Entries, int* DeviceSize);
typedef int (*FileSystem_MoveFile)(void *Object, const char* SourcePath,
		const char* DestinationPath);
typedef int (*FileSystem_DeleteFile)(void *Object, const char* TargetPath);
typedef int (*FileSystem_CopyFile)(void *Object, const char* SourcePath,
		int sourceUseBulk, const char* DestinationPath, int destinationUseBulk);
typedef int (*FileSystem_CreateDirectory)(void *Object, const char* TargetPath);
typedef int (*FileSystem_Properties)(void *Object, const char* TargetPath,
		int* Mode, uint64_t* Size, int* MTime, int* ATime, int* CTime);
typedef int (*FileSystem_ChangeAccess)(void *Object, const char* TargetPath,
		int Access);
typedef int (*FileSystem_ReadLoadModulesManifests)(void *Object,
		const char* TargetPath, const char* SourcePath);

// OTP
typedef int (*OTP_ReadBits)(void *Object, int OtpId, int BitStart,
		int BitLength, unsigned char* DataBuffer, int* DataBufferSize,
		unsigned char* StatusBuffer, int* StatusBufferSize);
typedef int (*OTP_SetBits)(void *Object, int OtpId, int BitStart,
		int BitLength, unsigned char* DataBuffer);
typedef int (*OTP_WriteAndLock)(void *Object, int OtpId);
typedef int (*OTP_StoreSecureObject)(void *Object, const char* srcPath,
		int destination, int useBulk);

// PARAMETER STORAGE
typedef int (*ParameterStorage_ReadGlobalDataUnit)(void *Object,
		const char* GdfsId, int Unit, unsigned char* DataBuffer, int* Size);
typedef int (*ParameterStorage_WriteGlobalDataUnit)(void *Object,
		const char* GdfsId, int Unit, unsigned char* DataBuffer, int Size);
typedef int (*ParameterStorage_ReadGlobalDataSet)(void *Object,
		const char* GdfsId, const char* Path, int useBulk);
typedef int (*ParameterStorage_WriteGlobalDataSet)(void *Object,
		const char* GdfsId, const char* Path, int useBulk);
typedef int (*ParameterStorage_EraseGlobalDataSet)(void *Object,
		const char* GdfsId);

// SECURITY
typedef int (*Security_SetDomain)(void *Object, int Domain);
typedef int (*Security_GetDomain)(void *Object, int* WrittenDomain);
typedef int (*Security_GetProperties)(void *Object, int UnitId, int* Size,
		unsigned char* DataBuffer);
typedef int (*Security_SetProperties)(void *Object, int UnitId, int Size,
		unsigned char* DataBuffer);
typedef int (*Security_BindProperties)(void *Object);
typedef int (*Security_WriteRPMBKey)(void *Object, int deviceId, int commercial);
typedef int (*Security_ListSupportedCiphers)(void *Object, TCiphers* Ciphers,
		int* CipherSize);
typedef int (*Security_KeySharingStage1)(void *Object, int SessionId,
		char* CipherName, unsigned char* CertBuffer, int Size);
typedef int (*Security_KeySharingStage2)(void *Object);
typedef int (*Security_InitARBTable)(void *Object, int iType, int iLength,
		unsigned char* puarbdata);

// RESET GROUP (U380 SPECIFIC)
// typedef int (*Reset_InitRequest)(void* Object, int* Size, unsigned char* DataBuffer);
// typedef int (*Reset_UpdateRequest)(void* Object, int DataLength, unsigned char* DataBuffer);

// LCD
LCD_Initialize LCD_Initialize_fn;
LCD_RemoveInstance LCD_RemoveInstance_fn;
LCD_SetTimeouts LCD_SetTimeouts_fn;
LCD_GetTimeouts LCD_GetTimeouts_fn;
LCD_CancelCurrentLoaderCommand LCD_CancelCurrentLoaderCommand_fn;
LCD_GetLoaderErrorDescription LCD_GetLoaderErrorDescription_fn;

// SYSTEM
System_LoaderStartupStatus System_LoaderStartupStatus_fn;
System_ChangeBaudrate System_ChangeBaudrate_fn;
System_Reboot System_Reboot_fn;
System_Shutdown System_Shutdown_fn;
System_SupportedCommands System_SupportedCommands_fn;
System_ExecuteSoftware System_ExecuteSoftware_fn;
System_Authenticate System_Authenticate_fn;
System_GetControlKeys System_GetControlKeys_fn;
System_GetControlKeysData System_GetControlKeysData_fn;
System_AuthenticationChallenge System_AuthenticationChallenge_fn;
System_CollectData System_CollectData_fn;
System_Deauthenticate System_Deauthenticate_fn;

// FLASH
Flash_ProcessFile Flash_ProcessFile_fn;
Flash_ListDevices Flash_ListDevices_fn;
Flash_DumpArea Flash_DumpArea_fn;
Flash_EraseArea Flash_EraseArea_fn;
Flash_SetEnhancedArea Flash_SetEnhancedArea_fn;
Flash_SelectLoaderOptions Flash_SelectLoaderOptions_fn;

// FILE SYSTEM
// FileSystem_SetReadOnly FileSystem_SetReadOnly_fn;
FileSystem_VolumeProperties FileSystem_VolumeProperties_fn;
FileSystem_FormatVolume FileSystem_FormatVolume_fn;
FileSystem_ListDirectory FileSystem_ListDirectory_fn;
FileSystem_MoveFile FileSystem_MoveFile_fn;
FileSystem_DeleteFile FileSystem_DeleteFile_fn;
FileSystem_CopyFile FileSystem_CopyFile_fn;
FileSystem_CreateDirectory FileSystem_CreateDirectory_fn;
FileSystem_Properties FileSystem_Properties_fn;
FileSystem_ChangeAccess FileSystem_ChangeAccess_fn;
FileSystem_ReadLoadModulesManifests FileSystem_ReadLoadModulesManifests_fn;

// OTP
OTP_ReadBits OTP_ReadBits_fn;
OTP_SetBits OTP_SetBits_fn;
OTP_WriteAndLock OTP_WriteAndLock_fn;
OTP_StoreSecureObject OTP_StoreSecureObject_fn;

// PARAMETER STORAGE
ParameterStorage_ReadGlobalDataUnit ParameterStorage_ReadGlobalDataUnit_fn;
ParameterStorage_WriteGlobalDataUnit ParameterStorage_WriteGlobalDataUnit_fn;
ParameterStorage_ReadGlobalDataSet ParameterStorage_ReadGlobalDataSet_fn;
ParameterStorage_WriteGlobalDataSet ParameterStorage_WriteGlobalDataSet_fn;
ParameterStorage_EraseGlobalDataSet ParameterStorage_EraseGlobalDataSet_fn;

// SECURITY
Security_SetDomain Security_SetDomain_fn;
Security_GetDomain Security_GetDomain_fn;
Security_GetProperties Security_GetProperties_fn;
Security_SetProperties Security_SetProperties_fn;
Security_BindProperties Security_BindProperties_fn;
Security_WriteRPMBKey Security_WriteRpmbKey_fn;
Security_InitARBTable Security_InitARBTable_fn;

// Security_ListSupportedCiphers
// Security_KeySharingStage1
// Security_KeySharingStage2


// RESET
// Reset_InitRequest Reset_InitRequest_fn;
// Reset_UpdateRequest Reset_UpdateRequest_fn;

void* loadSharedLibrary(const char *libname, int imode = 2) {
#ifdef UNIX
	return dlopen(libname, imode);
#else

	std::string lcd_path(libname);
	lcd_path=lcd_path.substr(0,lcd_path.find_last_of('\\'));

	std::cout<<"INFO SetDllDirectory: "<<lcd_path.c_str()<<std::endl;
	SetDllDirectory(lcd_path.c_str());//due to load mingw library inside LCD

	return (void*) LoadLibrary(libname);
#endif
}

void* getFunction(JNIEnv *env, void *lib, const char *fnname, void* nullfn =
		(void*) null_fn) {
	void* fn = NULL;
#ifdef UNIX
	fn = dlsym(lib,fnname);
#else
	fn = (void*) GetProcAddress((HINSTANCE) lib, fnname);
#endif
	if (fn == NULL) {
		Logger::getInstance()->error(LP, std::string("Couldn't load function .")+ fnname+".");
		return nullfn;
	}
	return fn;
}

void printErrorMessageFromLib(JNIEnv *env) {
#ifdef UNIX
	Logger::getInstance()->debug(LP, dlerror());
#else
	std::stringstream ss;
	ss<<"WinApi error code: 0x"<<std::setbase(16)<< GetLastError();
	Logger::getInstance()->error(LP,ss.str());
#endif
}

bool freeSharedLibrary(void *libd) {
#ifdef UNIX
	return dlclose(libd);
#else
	return FreeLibrary((HINSTANCE) libd);

#endif
}

bool loadFunctions(JNIEnv *env) {

	if (LCDLibPath.size() == 0) {
		m_libd = loadSharedLibrary(LCDRIVER);
	} else {
		m_libd = loadSharedLibrary(LCDLibPath.c_str());
	}

	if (m_libd == NULL) {
		printErrorMessageFromLib(env);
		env->ThrowNew(env->FindClass(EX_EXCEPTION), "Could not load LCDriver");
		return false;
	}

	LCD_Initialize_fn = (LCD_Initialize) getFunction(env, m_libd,
			LCD_INITIALIZE);
	LCD_RemoveInstance_fn = (LCD_RemoveInstance) getFunction(env, m_libd,
			LCD_CLOSE);
	LCD_SetTimeouts_fn = (LCD_SetTimeouts) getFunction(env, m_libd,
			LCD_SET_TIMEOUTS);
	LCD_GetTimeouts_fn = (LCD_GetTimeouts) getFunction(env, m_libd,
			LCD_GET_TIMEOUTS);
	LCD_CancelCurrentLoaderCommand_fn
			= (LCD_CancelCurrentLoaderCommand) getFunction(env, m_libd,
					LCD_CANCEL_CURRENT_LOADER_COMMAND);
  LCD_GetLoaderErrorDescription_fn = (LCD_GetLoaderErrorDescription) getFunction(env, m_libd, LCD_GET_LOADER_ERROR_DESCRIPTION);

	// SYSTEM
	System_LoaderStartupStatus_fn = (System_LoaderStartupStatus) getFunction(
			env, m_libd, LCD_SYSTEM_LOADER_STARTUP_STATUS);
	//System_ChangeBaudRate_fn = (System_ChangeBaudRate)getFunction(m_hDLL, LCD_SYSTEM_CHANGE_BAUDRATE);
	System_ChangeBaudrate_fn = (System_ChangeBaudrate) getFunction(env, m_libd,
			LCD_SYSTEM_CHANGE_BAUDRATE);
	System_Reboot_fn = (System_Reboot) getFunction(env, m_libd,
			LCD_SYSTEM_REBOOT);
	System_Shutdown_fn = (System_Shutdown) getFunction(env, m_libd,
			LCD_SYSTEM_SHUTDOWN);
	System_SupportedCommands_fn = (System_SupportedCommands) getFunction(env,
			m_libd, LCD_SYSTEM_SUPPORTED_COMMANDS);
	System_ExecuteSoftware_fn = (System_ExecuteSoftware) getFunction(env,
			m_libd, LCD_SYSTEM_EXECUTE_SOFTWARE);
	System_Authenticate_fn = (System_Authenticate) getFunction(env, m_libd,
			LCD_SYSTEM_AUTHENTICATE);
	System_Deauthenticate_fn = (System_Deauthenticate) getFunction(env, m_libd,
			LCD_SYSTEM_DEAUTHENTICATE);
	System_GetControlKeys_fn = (System_GetControlKeys) getFunction(env, m_libd,
			LCD_SYSTEM_GET_CONTROLKEYS);
	System_GetControlKeysData_fn = (System_GetControlKeysData) getFunction(env,
			m_libd, LCD_SYSTEM_GET_CONTROLKEYS_DATA);
	System_AuthenticationChallenge_fn
			= (System_AuthenticationChallenge) getFunction(env, m_libd,
					LCD_SYSTEM_AUTHENTICATION_CHALLENGE);
	System_CollectData_fn = (System_CollectData) getFunction(env, m_libd,
			LCD_SYSTEM_COLLECT_DATA);

	// FLASH
	Flash_ProcessFile_fn = (Flash_ProcessFile) getFunction(env, m_libd,
			LCD_FLASH_PROCESS_FILE);
	Flash_ListDevices_fn = (Flash_ListDevices) getFunction(env, m_libd,
			LCD_FLASH_LIST_DEVICES);
	Flash_DumpArea_fn = (Flash_DumpArea) getFunction(env, m_libd,
			LCD_FLASH_DUMP_AREA);
	Flash_EraseArea_fn = (Flash_EraseArea) getFunction(env, m_libd,
			LCD_FLASH_ERASE_AREA);
	Flash_SetEnhancedArea_fn = (Flash_SetEnhancedArea) getFunction(env, m_libd,
			LCD_FLASH_SET_ENHANCED_AREA);
    Flash_SelectLoaderOptions_fn = (Flash_SelectLoaderOptions) getFunction(env,
            m_libd, LCD_FLASH_SELECT_LOADER_OPTIONS);

	// FILE SYSTEM
	// FileSystem_SetReadOnly_fn = (FileSystem_SetReadOnly)getFunction(m_hDLL, LCD_FILE_SYSTEM_SET_READ_ONLY);
	FileSystem_VolumeProperties_fn = (FileSystem_VolumeProperties) getFunction(
			env, m_libd, LCD_FILE_SYSTEM_VOLUME_PROPERTIES);
	FileSystem_FormatVolume_fn = (FileSystem_FormatVolume) getFunction(env,
			m_libd, LCD_FILE_SYSTEM_FORMAT_VOLUME);
	FileSystem_ListDirectory_fn = (FileSystem_ListDirectory) getFunction(env,
			m_libd, LCD_FILE_SYSTEM_LIST_DIRECTORY);
	FileSystem_MoveFile_fn = (FileSystem_MoveFile) getFunction(env, m_libd,
			LCD_FILE_SYSTEM_MOVE_FILE);
	FileSystem_DeleteFile_fn = (FileSystem_DeleteFile) getFunction(env, m_libd,
			LCD_FILE_SYSTEM_DELETE_FILE);
	FileSystem_CopyFile_fn = (FileSystem_CopyFile) getFunction(env, m_libd,
			LCD_FILE_SYSTEM_COPY_FILE);
	FileSystem_CreateDirectory_fn = (FileSystem_CreateDirectory) getFunction(
			env, m_libd, LCD_FILE_SYSTEM_CREATE_DIRECTORY);
	FileSystem_Properties_fn = (FileSystem_Properties) getFunction(env, m_libd,
			LCD_FILE_SYSTEM_PROPERTIES);
	FileSystem_ChangeAccess_fn = (FileSystem_ChangeAccess) getFunction(env,
			m_libd, LCD_FILE_SYSTEM_CHANGE_ACCESS);
	FileSystem_ReadLoadModulesManifests_fn
			= (FileSystem_ReadLoadModulesManifests) getFunction(env, m_libd,
					LCD_FILE_SYSTEM_READ_LOADMODULE_MANIFESTS);

	// OTP
	OTP_ReadBits_fn
			= (OTP_ReadBits) getFunction(env, m_libd, LCD_OTP_READ_BITS);
	OTP_SetBits_fn = (OTP_SetBits) getFunction(env, m_libd, LCD_OTP_SET_BITS);
	OTP_WriteAndLock_fn = (OTP_WriteAndLock) getFunction(env, m_libd,
			LCD_OTP_WRITE_AND_LOCK);

	// PARAMETER STORAGE
	ParameterStorage_ReadGlobalDataUnit_fn
			= (ParameterStorage_ReadGlobalDataUnit) getFunction(env, m_libd,
					LCD_PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT);
	ParameterStorage_WriteGlobalDataUnit_fn
			= (ParameterStorage_WriteGlobalDataUnit) getFunction(env, m_libd,
					LCD_PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT);
	ParameterStorage_ReadGlobalDataSet_fn
			= (ParameterStorage_ReadGlobalDataSet) getFunction(env, m_libd,
					LCD_PARAMETER_STORAGE_READ_GLOBAL_DATA_SET);
	ParameterStorage_WriteGlobalDataSet_fn
			= (ParameterStorage_WriteGlobalDataSet) getFunction(env, m_libd,
					LCD_PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET);
	ParameterStorage_EraseGlobalDataSet_fn
			= (ParameterStorage_EraseGlobalDataSet) getFunction(env, m_libd,
					LCD_PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET);

	// SECURITY
	Security_SetDomain_fn = (Security_SetDomain) getFunction(env, m_libd,
			LCD_SECURITY_SET_DOMAIN);
	Security_GetDomain_fn = (Security_GetDomain) getFunction(env, m_libd,
			LCD_SECURITY_GET_DOMAIN);
	Security_GetProperties_fn = (Security_GetProperties) getFunction(env,
			m_libd, LCD_SECURITY_GET_PROPERTIES);
	Security_SetProperties_fn = (Security_SetProperties) getFunction(env,
			m_libd, LCD_SECURITY_SET_PROPERTIES);
	Security_BindProperties_fn = (Security_BindProperties) getFunction(env,
			m_libd, LCD_SECURITY_BIND_PROPERTIES);
	Security_WriteRpmbKey_fn = (Security_WriteRPMBKey) getFunction(env,
			m_libd, LCD_SECURITY_WRITE_RPMB_KEY);
	Security_InitARBTable_fn = (Security_InitARBTable) getFunction(env, m_libd,
			LCD_SECURITY_INIT_ARB_TABLE);
	OTP_StoreSecureObject_fn = (OTP_StoreSecureObject) getFunction(env, m_libd,
			LCD_SECURITY_STORE_SECURE_OBJECT);

	return true;
}
#endif /* LCDINTERFACE_H_ */

