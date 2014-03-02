/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_system_app
 *  @{
 */
#include "c_system.h"
#ifdef CFG_ENABLE_APPLICATION_SYSTEM
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "commands.h"
#include "r_main.h"
#include "r_r15_family.h"
#include "r_io_subsystem.h"
#include "t_system_applications.h"
#include "r_communication_service.h"
#include "system_applications_functions.h"
#include "r_command_exec_service.h"
#include "r_r15_transport_layer.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "audit.h"
#include "r_serialization.h"
#include "r_main.h"

#ifdef MACH_TYPE_DB5700
#include "r_service_management.h"
#include "asic_rtc_macro.h"
#endif
#include "running_commands.h"

#ifdef CFG_ENABLE_PRINT_SERVER
#include "r_debug_subsystem.h"
#endif
//#ifdef MACH_TYPE_DB5700
#include "r_loader_sec_lib.h"
//#endif

#include "cops_data_manager.h"
#ifdef MACH_TYPE_DB5700
#include "cops_storage_protect.h"
#endif

#include "r_communication_service.h"
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_measurement_tool.h"
#endif

#include "cpu_support.h"
#ifdef MACH_TYPE_DB5700
#include "r_pd_mixsig_register_noos.h"
#include "c_mixsig_pd.h"
#endif
#include "r_communication_abstraction.h"
#ifdef MACH_TYPE_DB5700
#include "asic_wdog_macro.h"
#endif

#ifdef MACH_TYPE_DB5700
#include "asic_syscon_macro.h"
#include "r_pd_syscon.h"
#include "t_pd_cpu.h"
#include "r_pd_cpu.h"

#include "r_bootparam.h"
#endif

#include "r_basicdefinitions.h"
#include "memmap_physical.h"

#include "flash_process_file_info.h"
#include "r_service_management.h"
#include "stn8500_system_power_applications_functions.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*
 *  Initialization Loader Functions
 */

/*
 *  Contains the loader version string, this version string
 *  is compiled with the correct loader variant and revision
 *  along with 3 characters of signum and a time and date of build
 */
extern const unsigned char CurrentDate[];
extern const CommandPermissionList_t CommandPermissionList[];
static uint32 Initial_Protrom_Channel = 0; //to be defined properly and erased
extern Communication_t DebugCommunication;
ControlKeysData_t ControlKeyAuthData;
CA_AuthData_t CA_AuthData;

static boolean RTC_Initialized = FALSE;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ErrorCode_e Do_System_RebootImpl(uint16 Session, const uint32 Mode)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SYSTEM_REBOOT;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Mode);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_RebootImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return E_SUCCESS;
}

ErrorCode_e Do_System_ShutDownImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SYSTEM_SHUTDOWN;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_ShutDownImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_System_SupportedCommandsImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SYSTEM_SUPPORTEDCOMMANDS;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_SupportedCmdGroupsImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}




ErrorCode_e Do_System_AuthenticateImpl(uint16 Session, const uint32 Type)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SYSTEM_AUTHENTICATE;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Type);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_AuthenticateImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_System_DeauthenticateImpl(uint16 Session, const uint32 Type)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SYSTEM_DEAUTHENTICATE;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Type);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_DeauthenticateImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_System_CollectDataImpl(uint16 Session, const uint32 Type)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SYSTEM_COLLECTDATA;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Type);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c :  Do_System_CollectDataImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_System_GetProgressStatusImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SYSTEM_GETPROGRESSSTATUS;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_GetProgressStatusImpl (%d) ......ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Done_System_LoaderStartUpStatusImpl(uint16 Session, ErrorCode_e Status)
{
    return E_SUCCESS;
}
ErrorCode_e Done_System_GetControlKeysImpl(uint16 Session, ErrorCode_e Status,
        const char *NLCKLock_p, const char *NSLCKLock_p,
        const char *SPLCKLock_p, const char *CLCKLock_p,
        const char *PCKLock_p, const char *ESLCKLock_p,
        const char *NLCKUnlock_p, const char *NSLCKUnlock_p,
        const char *SPLCKUnlock_p, const char *CLCKUnlock_p,
        const char *PCKUnlock_p, const char *ESLCKUnlock_p)
{
    return E_UNSUPPORTED_CMD;
}
ErrorCode_e Done_System_GetControlKeysDataImpl(uint16 Session, ErrorCode_e Status,
        const uint32 iDataSize,  void *SIMLockKeysData)
{

    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    VERIFY(iDataSize == (N_SIMLOCK_CONTROL_KEYS *(CONTROL_KEY_LENGTH + 1)), E_INVALID_INPUT_PARAMETERS);

    memcpy(ControlKeyAuthData.ControlKeys, SIMLockKeysData, iDataSize);
    ControlKeyAuthData.ValidControlKeys = TRUE;
    ControlKeyAuthData.Length = N_SIMLOCK_CONTROL_KEYS * (CONTROL_KEY_LENGTH + 1);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

ErrorCode_e Done_System_AuthenticationChallengeImpl(uint16 Session, ErrorCode_e Status,
        const uint32 UpdatedAuthChallengeBlockLength,
        void *UpdatedAuthChallengeBlock_p)
{
    CA_AuthData.Length = UpdatedAuthChallengeBlockLength;
    CA_AuthData.Data_p = (uint8 *)malloc(UpdatedAuthChallengeBlockLength);
    ASSERT(CA_AuthData.Data_p != NULL);
    memcpy(CA_AuthData.Data_p, UpdatedAuthChallengeBlock_p, UpdatedAuthChallengeBlockLength);
    CA_AuthData.ValidDataBlock = TRUE;
    return E_SUCCESS;
}

ErrorCode_e Do_System_SetSystemTimeImpl(uint16 Session, const uint32 EpochTime)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData;
    uint8 *Data_p = NULL;

    CmmData.CommandNr = COMMAND_SYSTEM_SETSYSTEMTIME;
    CmmData.ApplicationNr = GROUP_SYSTEM;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, EpochTime);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("system_applications_functions.c : Do_System_SetSystemTimeImpl (%d)  .....ReturnValue:(%d)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}
/*******************************************************************************
 *  ME -> PC commands
 ******************************************************************************/
ErrorCode_e Do_System_R15LoaderStarted(ErrorCode_e ErrorCode)
{
    ErrorCode_e ReturnValue = ErrorCode;
    ErrorCode_e Status = E_SUCCESS;
    uint32 LVSize = 0;
    char *LoaderVersion = NULL;
    uint32 PVSize = (uint32)PROTOCOLVERSION_SIZE;
    char *ProtocolVersion = NULL;

    B_(printf("system_applications_functions.c : Do_System_R15LoaderStartUpStatus (%d)\n\n", __LINE__);)

    if (E_SUCCESS != ReturnValue) {
        /* send signal loader not started */
        // ReturnValue = Do_R15_LoaderStoped();
        goto ErrorExit;
    }

    while ((CurrentDate[LVSize] != '\0') && (LVSize < LOADER_VERSION_MAX_SIZE)) {
        LVSize++;
    }

    if (LVSize && (LVSize < LOADER_VERSION_MAX_SIZE)) {
        ReturnValue = E_SUCCESS;
    } else {
        ReturnValue = E_INVALID_CURRDATE_STRING_LENGTH;
    }

    LoaderVersion = (char *)malloc(LVSize + sizeof(uint32));
    ASSERT(NULL != LoaderVersion);

    memcpy(LoaderVersion, &LVSize, sizeof(uint32));
    memcpy(LoaderVersion + sizeof(uint32), CurrentDate, LVSize);

    ProtocolVersion = (char *)malloc(PROTOCOLVERSION_SIZE + sizeof(uint32));
    ASSERT(NULL != ProtocolVersion);

    memcpy(ProtocolVersion, &PVSize, sizeof(uint32));
    ProtocolVersion[0 + sizeof(uint32)] = PROTOCOL_VERSION_MAJOR;
    ProtocolVersion[1 + sizeof(uint32)] = PROTOCOL_VERSION_MINOR;

    /* send signal loader started */
    ReturnValue = Do_System_LoaderStartUpStatus(Status, LoaderVersion, ProtocolVersion);

    BUFFER_FREE(LoaderVersion);

    BUFFER_FREE(ProtocolVersion);

ErrorExit:
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*******************************************************************************
 *  Long running commands
 ******************************************************************************/
ErrorCode_e System_SupportedCommandsRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    SupportedCommand_t *Commands_p = NULL;
    uint32 CounterList = 0;
    uint32 CommandsCount = 0;
    uint32 Counter = 0;

    while (CommandPermissionList[CounterList].Group != 0) {
        CounterList++;
    }

    Commands_p = (SupportedCommand_t *)malloc(CounterList * sizeof(SupportedCommand_t));
    ASSERT(NULL != Commands_p);

    for (Counter = 0; Counter < CounterList; Counter++) {
        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Audit_Permission_Levels((CommandPermissionList_t *)&CommandPermissionList[Counter]);

        if (E_SUCCESS == ReturnValue) {

            Commands_p[CommandsCount].Group = CommandPermissionList[Counter].Group;
            Commands_p[CommandsCount].Command = CommandPermissionList[Counter].Command;
            Commands_p[CommandsCount].Permitted = 0;
            CommandsCount++;
        }
    }

    ReturnValue = E_SUCCESS;
    ExeContext_p->Running = FALSE;

    B_(printf("system_applications_functions.c : System_SupportedCommandsRepeat (%d) .....ReturnValue = (%d)\n\n", __LINE__, ReturnValue);)
    ReturnValue = Done_System_SupportedCommands(ExeContext_p->Received.SessionNr, ReturnValue, CommandsCount,  Commands_p);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(Commands_p);

    return ReturnValue;
}


ErrorCode_e System_RebootRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommunicationDevice_t *CurrentDevice_p = NULL;
#ifdef ENABLE_DEBUG
    CommunicationDevice_t *DebugDevice_p = NULL;
#endif

    ReturnValue = Do_Communication_GetCommunicationDevice(GlobalCommunication_p, &CurrentDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_RebootRepeat (%d) **Failed to get communication device!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

    ReturnValue = Do_CommDeviceShutDown(CurrentDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c(%d): Communication channel shut down failed! (%d)\n", __LINE__, ReturnValue);)
    }

#ifdef ENABLE_DEBUG
    ReturnValue = Do_Communication_GetCommunicationDevice(&DebugCommunication, &DebugDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_RebootRepeat (%d) **Failed to get debug communication device!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

    ReturnValue = Do_CommDeviceShutDown(DebugDevice_p);

    if (ReturnValue != E_SUCCESS) {
        /* XVSZOAN: UART is shut down, no printouts allowed! */
    }

#endif

    Do_System_Reboot();

    StallExecution();

    /*
     * This function will never return a value since it is designed to
     * stop the execution and wait for WD bark to occur
     */
    return E_SUCCESS;
}

ErrorCode_e System_ShutDownRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommunicationDevice_t *CurrentDevice_p = NULL;
#ifdef ENABLE_DEBUG
    CommunicationDevice_t *DebugDevice_p = NULL;
#endif
    C_(printf("system_applications_functions.c : System_ShutDownRepeat (%d)\n", __LINE__);)

    ReturnValue = Do_Communication_GetCommunicationDevice(GlobalCommunication_p, &CurrentDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_ShutDownRepeat (%d) **Failed to get communication device!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

    ReturnValue = Do_Stop_Service(NUMBER_OF_SERVICES);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_ShutDownRepeat (%d) **Shutting down all services failed!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

    ReturnValue = Do_CommDeviceShutDown(CurrentDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_ShutDownRepeat (%d) **Communication channel shut down failed!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

#ifdef ENABLE_DEBUG
    ReturnValue = Do_Communication_GetCommunicationDevice(&DebugCommunication, &DebugDevice_p);

    if (ReturnValue != E_SUCCESS) {
        A_(printf("system_applications_functions.c : System_ShutDownRepeat (%d) **Failed to get debug communication device!** .....ReturnValue = (%d)\n", __LINE__, ReturnValue);)
    }

    ReturnValue = Do_CommDeviceShutDown(DebugDevice_p);

    if (ReturnValue != E_SUCCESS) {
        /* XVSZOAN: UART is shut down, no printouts allowed! */
    }

#endif

    Do_System_Shutdown();

    StallExecution();
    /*
     * This function will never return a value since it is designed to
     * stop the execution and wait for the ME to shutdown
     */
    return E_SUCCESS;
}

/**
 * Implements authentication procedure
 *
 * @param [in, out] ExecutionContext_p - is pointer to structure that holds
 *                                       information about a command that is
 *                                       placed (or will be placed) in the
 *                                       command execution queue.
 * @retval E_SUCCESS                   - Successful ending.
 *
 * @retval E_INVALID_INPUT_PARAMETERS  - If CmdData_p is NULL.
 */
ErrorCode_e System_AuthenticateRepeat(void *ExecutionContext_p)
{
    CommandData_t *CmdData_p = &((ExecutionContext_t *)ExecutionContext_p)->Received;
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_UNSUPPORTED_CMD;
#else
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    static boolean PersistentAuth = FALSE;
    static Authentication_t State = AUTHENTICATION_TYPE_SELECTION;
    cops_device_state_t DeviceState;
    AuthenticationType_t Type;
    void *Data_p = CmdData_p->Payload.Data_p;

#ifdef MACH_TYPE_STN8500
    size_t ChallengeBlockLength = 0;
    uint8 *ChallengeBlock       = NULL;
#else
    uint32 ChallengeBlockLength = 0;
    AuthenticationChallengeDataBlock_t ChallengeBlock;
#endif

    ReturnValue = E_SUCCESS;

    switch (State) {
    case AUTHENTICATION_TYPE_SELECTION:
        Type = get_uint32_le((void **)&Data_p);

        switch (Type) {
        case CONTROL_KEYS_AUTHENTICATION:
            PersistentAuth = FALSE;
            State = PROCESS_CONTROL_KEYS_AUTHENTICATION;
            break;

        case CONTROL_KEYS_AUTHENTICATION_PERSISTENT:
            PersistentAuth = TRUE;
            State = PROCESS_CONTROL_KEYS_AUTHENTICATION;
            break;

        case CERTIFICATE_AUTHENTICATION:
            PersistentAuth = FALSE;
            State = PROCESS_CERTIFICATE_AUTHENTICATION;
            break;

        case CERTIFICATE_AUTHENTICATION_PERSISTENT:
            PersistentAuth = TRUE;
            State = PROCESS_CERTIFICATE_AUTHENTICATION;
            break;

        case PERMANENT_AUTHENTICATION:
            PersistentAuth = TRUE;
            State = PROCESS_PERMANENT_AUTHENTICATION;
            break;

        case GET_AUTHENTICATION_STATE:
            ReturnValue = COPS_GetDeviceState(&DeviceState);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            if (COPS_DEVICE_STATE_FULL == DeviceState) {
                ReturnValue = E_COPS_DEVICE_STATE_FULL;
                goto ErrorExit;
            } else if (COPS_DEVICE_STATE_RELAXED == DeviceState) {
                ReturnValue = E_COPS_DEVICE_STATE_RELAXED;
                goto ErrorExit;
            } else {
                ReturnValue = E_GENERAL_FATAL_ERROR;
                goto ErrorExit;
            }

            break;

        default:
            ReturnValue = E_UNDEFINED_AUTHENTICATION_TYPE;
            goto ErrorExit;
        }

        break;

        // go in the next state
    case PROCESS_CONTROL_KEYS_AUTHENTICATION:
        // send command Get Control Keys
        // COPS_AUTH_TYPE_SIMLOCK_KEYS

        memset(ControlKeyAuthData.ControlKeys, 0x00, N_SIMLOCK_CONTROL_KEYS *(CONTROL_KEY_LENGTH + 1));
        ControlKeyAuthData.ValidControlKeys = FALSE;
        ReturnValue = Do_System_GetControlKeysData();

        if (ReturnValue == E_SUCCESS) {
            State = CONTROL_KEYS_SET_VERIFICATION;
        } else {
            B_(printf("system_applications_functions.c : System_AuthenticateRepeat (%d)  **Loader failed to send command ... Do_System_GetControlKeysData**\n", __LINE__);)
            goto ErrorExit;
        }

        break;

    case CONTROL_KEYS_SET_VERIFICATION:

        //...wait to get control keys
        if (TRUE == ControlKeyAuthData.ValidControlKeys) {
            // call the verification of control keys
            ReturnValue = (ErrorCode_e)COPS_Authenticate(COPS_AUTH_TYPE_SIMLOCK_KEYS, PersistentAuth, ControlKeyAuthData.ControlKeys, N_SIMLOCK_CONTROL_KEYS * (CONTROL_KEY_LENGTH + 1));

            ControlKeyAuthData.ValidControlKeys = FALSE;

            if (E_LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION == ReturnValue) {
                /* Send the error code and go to the shut down loader state. */
                ReturnValue = Done_System_Authenticate(CmdData_p->SessionNr, ReturnValue);

                ASSERT(E_SUCCESS == ReturnValue);
                State = NO_ATTEMPT_LEFT_SHUTDOWN;
            } else {
                goto ErrorExit;
            }
        }

        break;

    case PROCESS_CERTIFICATE_AUTHENTICATION:
        //...send command Authentication Challenge
        CA_AuthData.ValidDataBlock = FALSE;
#ifdef MACH_TYPE_STN8500
        ReturnValue = COPS_GenerateChallenge(COPS_AUTH_TYPE_RSA_CHALLENGE, &ChallengeBlock, &ChallengeBlockLength);
#else
    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_CreateAuthChallengeDataBlock(&ChallengeBlock, &ChallengeBlockLength);
#endif
        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
        ReturnValue = Do_System_AuthenticationChallenge(ChallengeBlockLength, ChallengeBlock);

        if (ReturnValue == E_SUCCESS) {
            State = CHALLENGE_DATA_BLOCK_VERIFICATION;
            B_(printf("system_applications_functions.c : System_AuthenticateRepeat (%d)  **Loader send command    Do_System_AuthenticationChallenge**\n", __LINE__);)
        } else {
            B_(printf("system_applications_functions.c : System_AuthenticateRepeat (%d)  **Loader failed to send command   Do_System_AuthenticationChallenge**\n", __LINE__);)
            goto ErrorExit;
        }

        break;

    case CHALLENGE_DATA_BLOCK_VERIFICATION:

        //...wait to get Updated Auth challenge Block
        if (TRUE == CA_AuthData.ValidDataBlock) {
            B_(printf("system_applications_functions.c : System_AuthenticateRepeat (%d)  **Received Auth Challenge Response**\n", __LINE__);)
#ifdef MACH_TYPE_STN8500
            ReturnValue = COPS_Authenticate(COPS_AUTH_TYPE_RSA_CHALLENGE, PersistentAuth, CA_AuthData.Data_p, CA_AuthData.Length);
#else
    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Authenticate(CERTIFICATE_AUTHENTICATION, CA_AuthData.Data_p, CA_AuthData.Length);
#endif
            BUFFER_FREE(CA_AuthData.Data_p);
            CA_AuthData.ValidDataBlock = FALSE;

            if (E_LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION == ReturnValue) {
                /* Send the error code and go to the shut down loader state. */
                ReturnValue = Done_System_Authenticate(CmdData_p->SessionNr, ReturnValue);
                ASSERT(E_SUCCESS == ReturnValue);
                State = NO_ATTEMPT_LEFT_SHUTDOWN;
            } else {
                goto ErrorExit;
            }
        }

        break;

    case PROCESS_PERMANENT_AUTHENTICATION:

        ReturnValue = COPS_Authenticate(COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, PersistentAuth, NULL, 0);
        B_(printf("system_applications_functions.c : System_AuthenticateRepeat (%d)  **Permanent Authentication**\n", __LINE__);)

        if (E_LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION == ReturnValue) {
            /* Send the error code and go to the shut down loader state. */
            ReturnValue = Done_System_Authenticate(CmdData_p->SessionNr, ReturnValue);
            ASSERT(E_SUCCESS == ReturnValue);
            State = NO_ATTEMPT_LEFT_SHUTDOWN;
        } else {
            goto ErrorExit;
        }

        break;

    case NO_ATTEMPT_LEFT_SHUTDOWN:
        /* shut down loader state. */
        ReturnValue = Do_System_ShutDownImpl(CmdData_p->SessionNr);
        ((ExecutionContext_t *)ExecutionContext_p)->Running = FALSE;
        State = AUTHENTICATION_TYPE_SELECTION;
        break;

    default:
        /* Fatal Error: Unknown authentication state */
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    return ReturnValue;

ErrorExit:

    State = AUTHENTICATION_TYPE_SELECTION;
    BUFFER_FREE(ChallengeBlock);

#endif

    ReturnValue = Done_System_Authenticate(CmdData_p->SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ((ExecutionContext_t *)ExecutionContext_p)->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e System_DeauthenticateRepeat(void *ExecutionContext_p)
{
    CommandData_t *CmdData_p = &((ExecutionContext_t *)ExecutionContext_p)->Received;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    DeAuthenticationType_t Type;
    void *Data_p = CmdData_p->Payload.Data_p;

    Type = get_uint32_le((void **)&Data_p);

    switch (Type) {
    case DEAUTHENTICATION:
        ReturnValue = COPS_Deauthenticate(FALSE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case DEAUTHENTICATION_PERSISTENT:
        ReturnValue = COPS_Deauthenticate(TRUE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    default:
        /* Fatal Error: Unknown Deauthenticate type */
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

ErrorExit:
    ReturnValue = Done_System_Deauthenticate(CmdData_p->SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ((ExecutionContext_t *)ExecutionContext_p)->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e System_CollectDataRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 DataLength = 0;
    uint8 *CollectedData_p = NULL;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *Data_p = NULL;
    uint32 Type = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    Type = get_uint32_le((void **)&Data_p);

    switch (Type) {
    case DEBUG_PRINTOUTS:
#if (defined(CFG_ENABLE_PRINT_SERVER))
        DataLength = Do_ReadDebugQueue((void **)&CollectedData_p);
#endif
        break;

    case MEASUREMENT_RESULTS:
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
        DataLength = Do_Measurement_GetMeasurementData(NULL, (uint8 **)&CollectedData_p);
#endif
        break;

    case FLASH_INFO:
        DataLength = Do_ReadFlashInfoData((void **)&CollectedData_p);
        break;

    default:
        break;
    }

    ReturnValue = E_SUCCESS;

    ExeContext_p->Running = FALSE;
    ReturnValue = Done_System_CollectData(ExeContext_p->Received.SessionNr, ReturnValue, DataLength,  CollectedData_p);
    ASSERT(E_SUCCESS == ReturnValue);

    //#if(defined(ENABLE_PRINT_SERVER) || defined(CFG_ENABLE_MEASUREMENT_TOOL))
    BUFFER_FREE(CollectedData_p);
    //#endif

    return E_SUCCESS;
}

ErrorCode_e System_GetProgressStatusRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 ProgressStatus = 0;

    ReturnValue = CES_GetProgressStatus(&ProgressStatus);
    VERIFY(ReturnValue == E_SUCCESS, ReturnValue);

ErrorExit:
    ExeContext_p->Running = FALSE;
    ReturnValue = Done_System_GetProgressStatus(ExeContext_p->Received.SessionNr, ReturnValue, ProgressStatus);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e System_SetSystemTimeRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    ReturnValue = E_SUCCESS;

#ifdef MACH_TYPE_DB5700

    PD_SYSCON_Result_t PD_SysconReturnValue = PD_SYSCON_RESULT_FAILED_ERROR;
    void *Data_p = NULL;
    uint32 EpochTime = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    EpochTime = get_uint32_le((void **)&Data_p);

    /* Put RTC in reset */
    PD_SysconReturnValue = Do_PD_SYSCON_SetInternalReset(PD_SYSCON_RESET_BRIDGE_RTC, TRUE);
    VERIFY((PD_SYSCON_RESULT_OK == PD_SysconReturnValue) || (PD_SYSCON_RESULT_FAILED_INCORRECT_USE == PD_SysconReturnValue), E_RTC_INTIALIZATION_FAILED);
    /* Release RTC fromreset */
    PD_SysconReturnValue = Do_PD_SYSCON_SetInternalReset(PD_SYSCON_RESET_BRIDGE_RTC, FALSE);
    VERIFY(PD_SYSCON_RESULT_OK == PD_SysconReturnValue, E_RTC_INTIALIZATION_FAILED);

    /* Set new time to RTC */
    RTC_WR_ADJUST_TIME_VALUE(EpochTime);
    RTC_Initialized = TRUE;

ErrorExit:
#endif //MACH_TYPE_DB5700

    ExeContext_p->Running = FALSE;
    ReturnValue = Done_System_SetSystemTime(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}



boolean Read_RTC_Status(void)
{
    return RTC_Initialized;
}

ErrorCode_e System_ErrorHandlerRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint8 *Data_p = NULL;
    uint32 CommandNr = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    ReturnValue = (ErrorCode_e)get_uint32_le((void **)&Data_p);
    CommandNr = get_uint16_le((void **)&Data_p);

    switch (CommandNr) {
    case COMMAND_SYSTEM_SUPPORTEDCOMMANDS:
        ReturnValue = Done_System_SupportedCommands(ExeContext_p->Received.SessionNr, ReturnValue, 0, NULL);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

#ifdef MACH_TYPE_DB5700
    case COMMAND_SYSTEM_EXECUTESOFTWARE:
        ReturnValue = Done_System_ExecuteSoftware(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
#endif

    case COMMAND_SYSTEM_AUTHENTICATE:
        ReturnValue = Done_System_Authenticate(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SYSTEM_SHUTDOWN:
        //ReturnValue = --(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SYSTEM_REBOOT:
        //ReturnValue = --(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SYSTEM_GETPROGRESSSTATUS:
        ReturnValue = Done_System_GetProgressStatus(ExeContext_p->Received.SessionNr, ReturnValue, 0);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SYSTEM_SETSYSTEMTIME:
        ReturnValue = Done_System_SetSystemTime(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    default:
        break;
    }

    if (0 != ExeContext_p->Received.Payload.Size) {
        BUFFER_FREE(ExeContext_p->Received.Payload.Data_p);
    }

    ExeContext_p->Running = FALSE;
    ReturnValue = E_SUCCESS;
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
void CopyAndJumpTo(uint8 const *const copyFrom, uint8 *copyTo, uint32 copyLength, uint32 jumpTo)
{
    uint32 i;

    if ((copyTo == NULL) || (jumpTo == 0)) {
        goto Exit;
    }

    for (i = 0; i < copyLength; i++) {
        copyTo[i] = copyFrom[i];
    }

    (*(void( *)(uint32))(jumpTo))(Initial_Protrom_Channel);

Exit:
    StallExecution();
}
#endif // CFG_ENABLE_APPLICATION_SYSTEM
/** @}*/
