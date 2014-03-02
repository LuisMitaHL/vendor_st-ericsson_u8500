/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _LCDRIVER_H_
#define _LCDRIVER_H_

#if defined(_WIN32)
#ifdef LCDRIVER_EXPORTS
#define LCDRIVER_API __declspec(dllexport)
#elif defined LCDRIVER_LOCALDEFINES
#define LCDRIVER_API
#else
#define LCDRIVER_API __declspec(dllimport)
#endif //LCDRIVER_EXPORTS

#ifndef __int64
#define __int64 long long
#endif

typedef unsigned __int64 uint64;
typedef unsigned long uint32;
#elif (defined(__linux__) || defined(__APPLE__))
#ifdef LCDRIVER_EXPORTS
#define LCDRIVER_API __attribute__((visibility("default")))
#elif defined LCDRIVER_LOCALDEFINES
#define LCDRIVER_API
#else
#define LCDRIVER_API
#endif

typedef unsigned long long uint64;
typedef unsigned int uint32;

#define __cdecl
#else
#error "Unknown platform"
#endif

typedef unsigned char uint8;
typedef struct CLCDriverInterface *LCDContext;

/// <summary>
/// Command information.
/// </summary>
/// <param name="iGroup">Command group number.</param>
/// <param name="iCommand">Command number.</param>
/// <param name="iPermitted">Permition level.</param>
typedef struct {
    int iGroup;
    int iCommand;
    int iPermitted;
} TSupportedCmd;

/// <summary>
/// Description of ME device.
/// </summary>
/// <param name="pchPath">Absolute device path pointing at the described device.</param>
/// <param name="iPathSize">Size of pchPath.</param>
/// <param name="pchType">Type of device.</param>
/// <param name="iTypeSize">Size of pchType.</param>
/// <param name="uiBlockSize">Size of the smallest addressable unit in the device [Byte].</param>
/// <param name="uiStart">Offset [Byte] of the start of the device relative its parent's
/// offset 0 with granularity of its parent's block size.</param>
/// <param name="uiLength">Length of the device [Byte].</param>
typedef struct {
    const char *pchPath;
    int iPathSize;
    const char *pchType;
    int iTypeSize;
    uint64 uiBlockSize;
    uint64 uiStart;
    uint64 uiLength;
} TDevices;

/// <summary>
/// Description of filesystem file or directory in ME.
/// </summary>
/// <param name="pchName">Name of file or directory.</param>
/// <param name="iNameSize">Size of pchName.</param>
/// <param name="uiSize">Size of file or directory.</param>
/// <param name="iMode">Indicator of file or directory.</param>
/// <param name="iTime">Time of last modification.</param>
typedef struct {
    const char *pchName;
    int iNameSize;
    uint64 uiSize;
    int iMode;
    int iTime;
} TEntries;

/// <summary>
/// 12 SIM lock/unlock keys.
/// </summary>
/// <param name="pchNLCKLock">NLCKLock key.</param>
/// <param name="pchNSLCKLock">NSLCKLock key.</param>
/// <param name="pchSPLCKLock">SPLCKLock key.</param>
/// <param name="pchCLCKLock">CLCKLock key.</param>
/// <param name="pchPCKLock">PCKLock key.</param>
/// <param name="pchESLCKLock">ESLCKLock key.</param>
/// <param name="pchNLCKUnLock">NLCKUnLock key.</param>
/// <param name="pchNSLCKUnLock">NSLCKUnLock key.</param>
/// <param name="pchSPLCKUnLock">SPLCKUnLock key.</param>
/// <param name="pchCLCKUnLock">CLCKUnLock key.</param>
/// <param name="pchPCKUnLock">PCKUnLock key.</param>
/// <param name="pchESLCKUnLock">ESLCKUnLock key.</param>
typedef struct {
    char *pchNLCKLock;
    char *pchNSLCKLock;
    char *pchSPLCKLock;
    char *pchCLCKLock;
    char *pchPCKLock;
    char *pchESLCKLock;

    char *pchNLCKUnLock;
    char *pchNSLCKUnLock;
    char *pchSPLCKUnLock;
    char *pchCLCKUnLock;
    char *pchPCKUnLock;
    char *pchESLCKUnLock;
} TSIMLockKeys;

/// <summary>
/// LCDriver time out.
/// </summary>
/// <param name="uiSTO">Startup TimeOut.</param>
/// <param name="uiRTO">Response TimeOut.</param>
typedef struct {
    uint32 uiSTO;
    uint32 uiRTO;
} TLCDriverTimeouts;

/// <summary>
/// R15 protocol time out.
/// </summary>
/// <param name="TCACK">Time for command packet acknowledge.</param>
/// <param name="TBCR">Time for bulk command packet to be recieved.</param>
/// <param name="TBES">Time for bulk session end. NOTE: Starting from version R1J this timeout is not
///used in LCM but it stays in LCDriver interface to avoid interface changes.</param>
/// <param name="TBDR">Time for bulk data packet to be received.</param>
typedef struct {
    uint32 TCACK;
    uint32 TBCR;
    uint32 TBES;
    uint32 TBDR;
} TR15Timeouts;

/// <summary>
/// Protocol family.
/// </summary>
typedef enum {
    /// <summary>R15 protocol family.</summary>
    R15_PROTOCOL_FAMILY = 0,
    /// <summary>A2 protocol family.</summary>
    A2_PROTOCOL_FAMILY = 1,
    /// <summary>PROTROM protocol family.</summary>
    PROTROM_PROTOCOL_FAMILY = 3,
    /// <summary>Z protocol family.</summary>
    Z_PROTOCOL_FAMILY = 4
} TFamily;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    /// <summary>
    /// Initializes new instance of the LCD context.
    ///
    /// Deprecated.
    /// Instead of this function please use: SetLCMLibPath, CreateContext, SwitchProtocolFamily, ConfigureCommunicationDevice,
    /// SetMessageCallback, SetProgressCallback and StartContext. These new functions give separation between creation,
    /// configuration and startup of the context.
    /// </summary>
    /// <param name="pContext">A pointer to a LCDContext where to store the newly created context or to put
    /// the previously created context if the context with the specified interface id already exists.</param>
    /// <param name="InterfaceId">A zero terminated string containing the interface unique ID.</param>
    /// <param name="Read_fn">A pointer to a communication device read function.</param>
    /// <param name="Write_fn">A pointer to a communication device write function.</param>
    /// <param name="Cancel_fn">A pointer to a communication device cancel function.</param>
    /// <param name="Instance">A pointer to a pointer where to store the adress of the loader communication module instance.</param>
    /// <param name="Message_fn">A pointer to a message logging function.</param>
    /// <param name="LCMLibPath">A zero terminated string containing the path to the LCM library.</param>
    /// <param name="ProgressBar_fn">A pointer to function which receives the progress updates.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl Initialize(LCDContext *pContext, const char *InterfaceId, void *Read_fn, void *Write_fn, void *Cancel_fn, void **Instance, void *Message_fn, const char *LCMLibPath, void *ProgressBar_fn);

    /// <summary>
    /// Closes the LCD context.
    ///
    /// Deprecated.
    /// Instead of this function please use DestroyContext which properly sets the LCD context to NULL to avoid further usage
    /// of the destroyed context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl Close(LCDContext Context);

    /// <summary>
    /// Sets the global LCM library path.
    /// </summary>
    /// <param name="LCMLibPath">A zero terminated string containing the path to the LCM library.</param>
    LCDRIVER_API int __cdecl SetLCMLibPath(const char *LCMLibPath);

    /// <summary>
    /// Creates new LCD context with the specified interface ID.
    /// </summary>
    /// <param name="pContext">A pointer to a LCDContext where to store the newly created context or to put
    /// the previously created context if the context with the specified interface id already exists.</param>
    /// <param name="InterfaceId">A zero terminated string containing the interface unique ID.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl CreateContext(LCDContext *pContext, const char *InterfaceId);

    /// <summary>
    /// Sets the initial family of not started context, or switches the protocol family of started context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="Family">New family to be set.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl SwitchProtocolFamily(LCDContext Context, TFamily Family);

    /// <summary>
    /// Configures the communication device to be used for the context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="Read_fn">A pointer to a communication device read function.</param>
    /// <param name="Write_fn">A pointer to a communication device write function.</param>
    /// <param name="Cancel_fn">A pointer to a communication device cancel function.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl ConfigureCommunicationDevice(LCDContext Context, void *Read_fn, void *Write_fn, void *Cancel_fn);

    /// <summary>
    /// Sets the callback for message logging.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="Callback_fn">A pointer to a message logging function.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl SetMessageCallback(LCDContext Context, void *Callback_fn);

    /// <summary>
    /// Sets the callback for progress updates.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="Callback_fn">A pointer to function which receives the progress updates.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl SetProgressCallback(LCDContext Context, void *Callback_fn);

    /// <summary>
    /// Starts the context. Should be called after the required parameters are previously configured.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="Instance">A pointer to a pointer where to store the address of the started communication instance.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl StartContext(LCDContext Context, void **Instance);

    /// <summary>
    /// Destroys the context. Should be called last after we are finished with the context.
    /// </summary>
    /// <param name="pContext">Pointer to LCDContext on which to execute the operation.
    /// LCDContext is set to NULL after this function is called.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl DestroyContext(LCDContext *pContext);

    /// <summary>
    /// Sets the LCD and R15 protocol timeouts.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="R15_TOs">Pointer to TR15Timeouts structure.</param>
    /// <param name="LCD_TOs">Pointer to TLCDriverTimeouts structure.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl SetTimeouts(LCDContext Context, TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs);

    /// <summary>
    /// Gets the LCD and R15 protocol timeouts.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="R15_TOs">Pointer to TR15Timeouts structure.</param>
    /// <param name="LCD_TOs">Pointer to TLCDriverTimeouts structure.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl GetTimeouts(LCDContext Context, TR15Timeouts *R15_TOs, TLCDriverTimeouts *LCD_TOs);

    /// <summary>
    /// Cancels the currently executing command.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <returns>0 if OK, otherwise non-zero.</returns>
    LCDRIVER_API int __cdecl CancelCurrentLoaderCommand(LCDContext Context);

    /// <summary>
    /// Provides short and long description for given error code number.
    /// </summary>
    /// <param name="ErrorNr">Number of error code.</param>
    /// <param name="ShortDescription">Buffer for the short description of the error.</param>
    /// <param name="LongDescription">Buffer for the long description of the error.</param>
    /// <param name="ShorDescrBufLen">Maximum size of ShortDescription buffer.</param>
    /// <param name="ShorDescrBufLen">Maximum size of LongDescription buffer.</param>
    /// <returns>Void.</returns>
    LCDRIVER_API void __cdecl GetLoaderErrorDescription(uint32 ErrorNr, uint8 *ShortDescription, uint8 *LongDescription, uint32 ShorDescrBufLen, uint32 LongDescrBufLen);

    /// <summary>
    /// The Loader Start-up Status command is sent by the ME to notify the host that it has started. The Status parameter indicates in what mode the loader started.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchVersion">Loader version identifier.</param>
    /// <param name="piVersionSize">Number of allocated bytes for version string.</param>
    /// <param name="pchProtocol">Protocol version identifier.</param>
    /// <param name="piProtocolSize">Number of allocated bytes for protocol string.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_LoaderStartupStatus(LCDContext Context, char *pchVersion, int *piVersionSize, char *pchProtocol, int *piProtocolSize);

    /// <summary>
    /// The ChangeBaudrate command is used to instruct the loader to change the UART baud rate.
    /// The ME does not return GeneralResponse to this command. It changes to new BaudRate
    /// and expects the further communication over UART to be realized using the new BaudRate.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iBaudRate">Indicates the new UART baud rate.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_ChangeBaudrate(LCDContext Context, int iBaudRate);

    /// <summary>
    /// The Reboot command is used to instruct the loader to reset the ME. Upon receiving this command,
    /// the loader shuts down in a controlled fashion and restarts the ME. The Mode parameter is used to
    /// select the mode of reset. The ME does not accept any further communication after a successful
    /// response from this command has been returned.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iMode">Indicates the mode of reset:
    /// 0: indicates normal restart.
    /// 1: indicates restart in service mode.
    /// 2: indicates restart with JTAG debugging enabled.
    /// 3: indicates restart in service mode and with JTAG debugging enabled.
    /// </param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_Reboot(LCDContext Context, int iMode);

    /// <summary>
    /// The loader shuts down in a controlled fashion and proceeds to shut down the ME itself.
    /// The ME does not accept any further communication after a successful response from this
    /// command has been returned.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_Shutdown(LCDContext Context);

    /// <summary>
    /// The loader returns a list of implemented commands and whether they are permitted to
    /// execute in the current loader state.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pCmdList">A list with supported commands.</param>
    /// <param name="piCmdListSize">Number of commands in CmdList.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_SupportedCommands(LCDContext Context, TSupportedCmd *pCmdList, int *piCmdListSize);

    /// <summary>
    /// This command is used by the loader to extract debug, measurement or flashing status data.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iType">Type of data to be extracted.</param>
    /// <param name="piSize">Size of the collected data buffer.</param>
    /// <param name="pData">Buffer to hold the collected data.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_CollectData(LCDContext Context, int iType, int *piSize, char *pData);

    /// <summary>
    /// Receive, verify and execute software, which can be a signed loader.
    /// After having sent this command, the ME attempts to read the software
    /// payload data from the host using the Bulk protocol or from the flash
    /// file system depending on the selected path. The current loader does not
    /// respond to communication after this command has been successfully executed.
    /// If the executed software is another loader, communication can be resumed once the
    /// Loader Started command has been received. A response of E_OPERATION_CANCELLED
    /// indicates the operation was canceled.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="ExecuteMode">Execute mode: 1 = execution from specified address, 2 = first load the software then execute. </param>
    /// <param name="pchDevicePath">Target path. If use bulk the path is on PC, else on ME.</param>
    /// <param name="iUseBulk">If value = 1 -> source on PC, else source on ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_ExecuteSoftware(LCDContext Context, const uint32 ExecuteMode, const char *pchDevicePath, int iUseBulk);

    /// <summary>
    /// This command is used to escalate the privileges of the operator. Two ways of
    /// authentication are available by default; control key authentication and certificate based
    /// authentication. The authentication command sets the loader in a specific
    /// authentication context when it takes control over the command flow. After
    /// receiving the authentication command, the loader sends the appropriate request for information to the PC.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iType">Authentication type:0 = control key authentication,1 = certificate authentication.</param>
    /// <param name="piSize">Size of puchData.</param>
    /// <param name="puchdata">Data challenge. </param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_Authenticate(LCDContext Context, int iType, int *piSize, unsigned char *puchdata);

    /// <summary>
    /// This command deauthenticates the ME from prior
    /// authentications to prevent unauthorized access.
    /// </summary>
    /// <param name="iType">Deauthentication type:0 = deauthenticate until next reboot of the ME,1 = permanent deauthentication.</param>
    /// <returns> Status of the command.</returns>
    LCDRIVER_API int __cdecl System_Deauthenticate(LCDContext Context, int iType);

    /// <summary>
    /// This command is used by the loader to retrieve the SimLock control keys from the host to authenticate a user.
    /// The command is used in the authentication context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pSIMLockKeys">A struct with all lock/unlock keys.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_GetControlKeys(LCDContext Context, TSIMLockKeys *pSIMLockKeys);

    /// <summary>
    /// This command is used by the loader to retrieve the SimLock control keys data from the host to authenticate a user.
    /// The command is used in the authentication context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="SIMLockKeysData">SIMLockKeyData buffer that contain all SIMLock keys.</param>
    /// <param name="iDataSize">SIMLockKeyData buffer size.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_GetControlKeysData(LCDContext Context, int iDataSize, unsigned char *pSIMLockKeysData);

    /// <summary>
    /// This command is used by the loader to perform a certificate authentication.
    /// The command is only used in the authentication context.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="puchChallengeData">Authentication challenge. This challenge must be signed
    /// using the correct certificate and returned to the loader.</param>
    /// <param name="iDataSize">Authentication challenge buffer length.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_AuthenticationChallenge(LCDContext Context, unsigned char *puchChallengeData, int iDataSize);

    /// <summary>
    /// The Set System Time command is used to set the current epoch time for loader
    /// to configure the real time clock and use it for file system operations.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="uiEpochTime">Number of seconds that have elapsed since January 1, 1970</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_SetSystemTime(LCDContext Context, uint32 uiEpochTime);

    /// <summary>
    /// This command is used to instruct the Loader to switch to a new communication device.
    /// </summary>
    /// <param name="uiDevice">Device ID of the communication device to switch to.</param>
    /// <param name="uiDeviceParam">Communication device parameters.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_SwitchCommunicationDevice(LCDContext Context, uint32 uiDevice, uint32 uiDeviceParam);

    /// <summary>
    /// The Loader shuts down the global communication and enters in a Relay working mode.
    /// </summary>
    /// <param name="HostDeviceId">Communication device number of the relay input (host device).</param>
    /// <param name="TargetDeviceId">Communication device number of the relay output (target device).</param>
    /// <param name="ControlDeviceId">Communication device number for the loader commands (control device).</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl System_StartCommRelay(LCDContext Context, uint32 HostDeviceId, uint32 TargetDeviceId, uint32 ControlDeviceId);

    /// <summary>
    /// This command is used to initiate a flashing session. The type argument is
    /// used to select the type of file to process and Length parameter
    /// defines the total size of the file.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPath">Target path. If iUseBulk = 1, path is on PC.</param>
    /// <param name="pchType">Type of the opened file.</param>
    /// <param name="iUseBulk">If to use bulk protocol. If target is on PC iUseBulk shall be 1.</param>
    /// <param name="iDeleteBuffers">Specify if to delete bulk buffers after finish. 1 -> delete buffers. 0 -> don't delete buffers.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_ProcessFile(LCDContext Context, const char *pchPath, const char *pchType, int iUseBulk, int iDeleteBuffers);

    /// <summary>
    /// This command is used by the client application program to obtain the device tree.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pDevices">A struct with information about one flash device.</param>
    /// <param name="piDeviceSize">Number of flash devices, size of pDevices array.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_ListDevices(LCDContext Context, TDevices *pDevices, int *piDeviceSize);

    /// <summary>
    /// This command is used to initiate a dump session.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPathToDump">Path to the device to dump.</param>
    /// <param name="uiStart">Start of the dump relative to the start of the device indicated by Path [Byte].</param>
    /// <param name="uiLength">Length of the dump [Byte]. Actual length is determined by the device block size.</param>
    /// <param name="pchFilePath">File path on PC to store dump data to.</param>
    /// <param name="uiRedundantArea">0-> dump with redundant data, 1-> dump without redundant data.</param>
    /// <param name="iUseBulk">1-> save dump data on PC, 0-> save dump data on ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_DumpArea(LCDContext Context, const char *pchPathToDump, uint64 uiStart, uint64 uiLength, const char *pchFilePath, uint32 uiRedundantArea, int iUseBulk);

    /// <summary>
    /// This command is used to erase a flash device or part of a flash device.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPath">Path to the device to erase.</param>
    /// <param name="uiStart">Start of the erase relative to the start of the device
    /// indicated by path [Byte]. This must be a multiple of the block size of the device.</param>
    /// <param name="uiLength">Length of the dump [Byte]. This must be a multiple of the block size of the device.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_EraseArea(LCDContext Context, const char *pchPath, uint64 uiStart, uint64 uiLength);

    /// <summary>
    /// This command is used to initiate a flashing of raw data.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPath">Target path. If iUseBulk = 1, path is on PC.</param>
    /// <param name="uiStart">Address where flashing should start.</param>
    /// <param name="uiLength">Length of data to be flashed.</param>
    /// <param name="uiDevice">Device ID number.</param>
    /// <param name="iUseBulk">If to use bulk protocol. If target is on PC iUseBulk shall be 1.</param>
    /// <param name="iDeleteBuffers">Specify if to delete bulk buffers after finish. 1 -> delete buffers. 0 -> don't delete buffers.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_FlashRaw(LCDContext Context, const char *pchPath, uint64 uiStart, uint64 uiLength, uint32 uiDevice, int iUseBulk, int iDeleteBuffers);

    /// <summary>
    /// This command is used to set enhanced area in eMMC.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPathToDump">Path to the device where area should be set.</param>
    /// <param name="uiStart">Start of enhanced area [Byte].</param>
    /// <param name="uiLength">Length of enhanced area [Byte].</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_SetEnhancedArea(LCDContext Context, const char *pchPathToDump, uint64 uiStart, uint64 uiLength);

    /// <summary>
    /// This command is used to select different options in the loader during its life cycle.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="uiProperty">Defines which property of the loader will be set.</param>
    /// <param name="uiValue">The value with which the property will be set.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Flash_SelectLoaderOptions(LCDContext Context, uint32 uiProperty, uint32 uiValue);

    /// <summary>
    /// This command retrieves the properties of the specified file system volume. It is issued by the PC application.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchDevicePath">Path of file system volume.</param>
    /// <param name="pchFSType">File system type.</param>
    /// <param name="piFSTypeSize">Size of pchFSType array.</param>
    /// <param name="puiSize">Total size of the file system [Byte].</param>
    /// <param name="puiFree">Available space [Byte].</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_VolumeProperties(LCDContext Context, const char *pchDevicePath, char *pchFSType, int *piFSTypeSize, uint64 *puiSize, uint64 *puiFree);

    /// <summary>
    /// Formats unused file system volume specified in device path. This operation fails if the volume is currently in use.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchDevicePath">Device path of the file system volume.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_FormatVolume(LCDContext Context, const char *pchDevicePath);

    /// <summary>
    /// List files and directories residing in specified path.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPath">File system path.</param>
    /// <param name="pEntries">Struct with file and directory information.</param>
    /// <param name="piDeviceSize">Number of files or directories, number of element in pEntries array.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_ListDirectory(LCDContext Context, const char *pchPath, TEntries *pEntries, int *piDeviceSize);

    /// <summary>
    /// This command moves a file from the source path to the destination path if the source and destination differ.
    /// It also renames a file if the source path and the destination path are the same.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchSourcePath">File system path in ME to source.</param>
    /// <param name="pchDestinationPath">File system path in ME to destination.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_MoveFile(LCDContext Context, const char *pchSourcePath,  const char *pchDestinationPath);

    /// <summary>
    /// Delete the specified file or directory. The loader only deletes empty directories.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchTargetPath">File system path in ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_DeleteFile(LCDContext Context, const char *pchTargetPath);

    /// <summary>
    /// This command is used to do the following: Copy a file from the PC to the ME. Copy a file between two directories or file systems on the ME.
    /// Copy a file from the ME to the PC.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchSourcePath">If iSourceUseBulk = 1 -> path on PC, else path on ME.</param>
    /// <param name="iSourceUseBulk">If = 1 -> source on PC, else source on ME.</param>
    /// <param name="pchDestinationPath">If iDestinationUseBulk = 1 -> path on PC, else path on ME.</param>
    /// <param name="iDestinationUseBulk">If = 1 -> destination on PC, else destination on ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_CopyFile(LCDContext Context, const char *pchSourcePath, int iSourceUseBulk, const char *pchDestinationPath, int iDestinationUseBulk);

    /// <summary>
    /// This command is used to create a directory. It is issued by the PC application.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchTargetPath">File system path to target.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_CreateDirectory(LCDContext Context, const char *pchTargetPath);

    /// <summary>
    /// This command is used to retrieve the properties of a file or directory.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchTargetPath">File system path to target.</param>
    /// <param name="puiMode">File type and access restrictions descriptor.</param>
    /// <param name="puiSize">File size [Byte].</param>
    /// <param name="piMTime">Last modification timestamp.</param>
    /// <param name="piATime">Last access timestamp.</param>
    /// <param name="piCTime">Creation timestamp.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_Properties(LCDContext Context, const char *pchTargetPath, uint32 *puiMode, uint64 *puiSize, int *piMTime, int *piATime, int *piCTime);

    /// <summary>
    /// This command is used to change the access permissions of a specified path.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchTargetPath">File system path to target.</param>
    /// <param name="iAccess">New access permissions.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_ChangeAccess(LCDContext Context, const char *pchTargetPath, int iAccess);

    /// <summary>
    /// This command is used to read manifests contained in load modules at the specified path and send the data back over bulk protocol.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchTargetPath">Path on PC to save manifest.</param>
    /// <param name="pchSourcePath">Load module(s) file system path.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl FileSystem_ReadLoadModuleManifests(LCDContext Context, const char *pchTargetPath, const char *pchSourcePath);


    /// <summary>
    /// This command is used to read the specified bits from the OTP.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iOtpId">OTP area ID.</param>
    /// <param name="iBitStart">Starting offset [bit].</param>
    /// <param name="iBitLength">Length of read [bit].</param>
    /// <param name="puchDataBuffer">Received OTP data.</param>
    /// <param name="piDataBufferSize">Size of puchDataBuffer.</param>
    /// <param name="puchStatusBuffer">Lock status for each read bit.</param>
    /// <param name="piStatusBufferSize">Size of puchStatusBuffer.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl OTP_ReadBits(LCDContext Context, int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer, int *piDataBufferSize, unsigned char *puchStatusBuffer, int *piStatusBufferSize);

    /// <summary>
    /// This command stores the specified bits in the loader internal OTP structures in RAM, it does not write to OTP.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iOtpId">OTP area ID.</param>
    /// <param name="iBitStart">Starting offset [bit].</param>
    /// <param name="iBitLength">Length of write [bit].</param>
    /// <param name="puchDataBuffer">OTP data to write.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl OTP_SetBits(LCDContext Context, int iOtpId, int iBitStart, int iBitLength, unsigned char *puchDataBuffer);

    /// <summary>
    /// This command writes (burns) all data from loader internal OTP structures in RAM into the OTP fuse box.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iOtpId">OTP area ID.</param>
    /// <param name="iForceWrite">If true: write and lock all lockable areas even if not all bits are received in cache.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl OTP_WriteAndLock(LCDContext Context, int iOtpId, int iForceWrite);

    /// <summary>
    /// Reads a specified unit from the global data storage area/partition specified by the DevicePath parameter.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchGdfsId">GDFS ID.</param>
    /// <param name="iUnit">Unit ID to read.</param>
    /// <param name="puchDataBuffer">Received global data.</param>
    /// <param name="piSize">Size of puchDataBuffer.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl ParameterStorage_ReadGlobalDataUnit(LCDContext Context, const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int *piSize);

    /// <summary>
    /// Writes a specified unit to the global data storage area/partition specified by the pchGdfsId parameter.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchGdfsId">GDFS ID.</param>
    /// <param name="iUnit">Unit ID to write.</param>
    /// <param name="puchDataBuffer">Global data to write.</param>
    /// <param name="iSize">Size of puchDataBuffer.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl ParameterStorage_WriteGlobalDataUnit(LCDContext Context, const char *pchGdfsId, int iUnit, unsigned char *puchDataBuffer, int iSize);

    /// <summary>
    /// Reads the complete global data storage area/partition specified by the pchGdfsId parameter (reads all units at once).
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchGdfsId">GDFS ID.</param>
    /// <param name="pchPath">If iUseBulk=1 -> path on PC to save data set to. Else path on ME.</param>
    /// <param name="iUseBulk">1-> save global data on PC, 0-> save global data on ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl ParameterStorage_ReadGlobalDataSet(LCDContext Context, const char *pchGdfsId, const char *pchPath, int iUseBulk);

    /// <summary>
    /// Writes the complete global data storage area/partition specified by the pchGdfsId parameter (writes all units at once).
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchGdfsId">GDFS ID.</param>
    /// <param name="pchPath">If iUseBulk=1 -> path on PC. Else path on ME</param>
    /// <param name="iUseBulk">1-> global data source on PC, 0-> global data source on ME.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl ParameterStorage_WriteGlobalDataSet(LCDContext Context, const char *pchGdfsId, const char *pchPath, int iUseBulk);

    /// <summary>
    /// Erases the complete global data storage area/partition specified by the DevicePath parameter.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchGdfsId">GDFS ID.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl ParameterStorage_EraseGlobalDataSet(LCDContext Context, const char *pchGdfsId);


    /// <summary>
    /// This command is used to set the ME domain.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iDomain">Target domain.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_SetDomain(LCDContext Context, int iDomain);

    /// <summary>
    /// This command is used to get the ME domain.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="piWrittenDomain">The ME current domain.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_GetDomain(LCDContext Context, int *piWrittenDomain);

    /// <summary>
    /// This command is used to read a security data unit (such as a secure static or dynamic data unit).
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iUnitId">Unit ID to read.</param>
    /// <param name="piSize">Size of puchDataBuffer.</param>
    /// <param name="puchDataBuffer">The unit data.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_GetProperties(LCDContext Context, int iUnitId, int *piSize, unsigned char *puchDataBuffer);

    /// <summary>
    /// This command is used to write a security data unit (such as a secure static or dynamic data unit).
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iUnitId">Unit ID to write.</param>
    /// <param name="iSize">Size of puchDataBuffer.</param>
    /// <param name="puchDataBuffer">The data to write.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_SetProperties(LCDContext Context, int iUnitId, int iSize, unsigned char *puchDataBuffer);

    /// <summary>
    /// This command associates all security data units with the current ME.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_BindProperties(LCDContext Context);

    /// <summary>
    /// This command installs a secure object to the Boot record or OTP area.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchSourcePath">If iUseBulk = 1 -> path on PC, else path on ME.</param>
    /// <param name="iDestination">Secure object destination address on ME.</param>
    /// <param name="iUseBulk">Source on PC -> iUseBulk= 1. Source on ME -> iUseBulk= 0.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_StoreSecureObject(LCDContext Context, const char *pchSourcePath, int iDestination, int iUseBulk);

    /// <summary>
    /// This command is used to initialize a SW version table, intended for checking the ARB functionality.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iType">ARB data type:0 = COPS_ARB_DATA_TYPE_MODELID.</param>
    /// <param name="iLength">Length of puarbData.</param>
    /// <param name="puarbdata">For arb_data_type = COPS_ARB_DATA_TYPE_MODELID the data is a 16 bit modelid.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_InitARBTable(LCDContext Context, int iType, int iLength, unsigned char *puarbdata);

    /// <summary>
    /// This command is used to invoke programming of RPMB authentication key.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="dev_id">The device ID of the RPMB device.</param>
    /// <param name="commercial">Indicates if initialization with a commercial key or a development key is used for RPMB.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Security_WriteRPMBKey(LCDContext Context, uint32 uiDev_id, uint32 uiCommercial);

    /// <summary>
    /// The A2 loader shuts down in a controlled fashion and proceeds to shut down the ME itself.
    /// The ME does not accept any further communication after a successful response from this
    /// command has been returned.
    /// </summary>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_System_Shutdown(LCDContext Context);

    /// <summary>
    /// The PC uses the Loader version command to request version information from the loader
    /// The response holds the loader version information coded as ASCII characters in the data field
    /// </summary>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_System_LoaderVersion(LCDContext Context, char *pchLoaderVersion, int *piSize, int iTargetCPU);

    /// <summary>
    /// The Loader on loader command is used to transfer a new loader to the ME
    /// When the header or payload has been sent, the loader responds with a GR using status codes.
    /// </summary>
    /// <returns> Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_System_LoaderOnLoader(LCDContext Context, const char *pchPath, int iPLOfset, int iHLOffset, int iTargetCPU);

    /// <summary>
    /// The Reset command will reset the phone using the watchdog reset functionality of the ARM processor.
    /// </summary>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_System_Reset(LCDContext Context, int iTimeout);

    /// <summary>
    /// The Program flash command is sent by the PC to write a block of data into the flash memory.
    /// The block data is sent in the data field.
    /// When a complete block has been transmitted the loader verifies the data and responds with a GR using status codes
    /// </summary>
    /// <returns> Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_Flash_ProgramFlash(LCDContext Context, const char *pchPath, int iUseSpeedFlash);

    /// <summary>
    /// The Erase flash command is used to erase the complete flash memory.
    /// It checks what type of memory is used in the ME and erases the complete memory including the first memory block.
    /// If more than one flash is attached, all of them will be erased
    /// The loader responds with a GR using status codes
    /// </summary>
    /// <returns> Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_Flash_EraseFlash(LCDContext Context);

    /// <summary>
    /// The loader started message is sent from the ME to the PC as soon as the loader has started executing
    /// and is ready to receive commands.
    /// </summary>
    /// <returns> Status of the command.</returns>
    LCDRIVER_API int __cdecl A2_Control_LoaderStarted(LCDContext Context);

    /// <summary>
    /// Set protocol to use.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchFamily">Protocol to use. Valid values: R15_FAMILY, PROTROM_FAMILY, NOPROT</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl SetProtocolFamily(LCDContext Context, const char *pchFamily);

    /// <summary>
    /// Set ME in service mode and receive chip id.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="puiChipId">Received chip id.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Z_SetInServiceMode(LCDContext Context, unsigned int *puiChipId);

    /// <summary>
    /// Set communication baudrate when communicating via UART.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="iBaudrate">Baudrate to use. Valid values: 9600, 19200, 38400, 57600, 115200,
    /// 230400, 460800, 921600, 1625000.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Z_SetBaudrate(LCDContext Context, int iBaudrate);

    /// <summary>
    /// Exit Z-protocol and start using PROTROM-protocol.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl Z_Exit_Z_Protocol(LCDContext Context);

    /// <summary>
    /// Download loader using PROTROM-protocol.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="pchPath">Path to loader on PC.</param>
    /// <param name="iPLOffset">Offset in header to payload length.</param>
    /// <param name="iHLOffset">Offset in header to header length.</param>
    /// <param name="iContinueProtRom">1 -> continue use PROTROM-protocol after download the loader. O -> not use PROTROM-protocol after download the loader.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl PROTROM_DownloadLoader(LCDContext Context, const char *pchPath, int iPLOffset, int iHLOffset, int iContinueProtRom);

    /// <summary>
    /// Send intrinsic request to read security data using PROTROM-protocol.
    /// </summary>
    /// <param name="Context">LCD context on which to execute the operation.</param>
    /// <param name="uiSecDataId">ID of the security data to be read with the intrinsic request.</param>
    /// <param name="puchDataBuffer">Buffer where the read data will be stored.</param>
    /// <param name="piDataLength">IN: Length of DataBuffer; OUT: Read data length.</param>
    /// <returns>Status of the command.</returns>
    LCDRIVER_API int __cdecl PROTROM_ReadSecurityData(LCDContext Context, unsigned char uiSecDataId, unsigned char *puchDataBuffer, int *piDataLength);

    // Random SIMLock keys generation functions not related to communication
    LCDRIVER_API int __cdecl Algorithms_SimGeneratorInit(void *Object, int SecurityLevel);
    LCDRIVER_API int __cdecl Algorithms_SimGeneratorExit(void *Object);
    LCDRIVER_API int __cdecl Algorithms_SimGeneratorCalculate(void *Object, unsigned char *ByteArray, uint32 ArraySize);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _LCDRIVER_H_
