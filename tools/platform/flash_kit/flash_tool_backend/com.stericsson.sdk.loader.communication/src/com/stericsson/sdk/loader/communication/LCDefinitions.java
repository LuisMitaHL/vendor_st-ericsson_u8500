package com.stericsson.sdk.loader.communication;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * @author xdancho
 */
public final class LCDefinitions {

    /** */
    public static final String BULK_DATA_RECEIVED = "bulk_data_received";

    /** */
    public static final String BULK_SESSION_END = "bulk_session_end";

    /** */
    public static final String COMMAND_ACK = "command_ack";

    /** */
    public static final String BULK_CMD_RECEIVED = "bulk_command_received";

    /** */
    public static final String COMMAND_GR = "command_general_response";

    /** */
    public static final String PROCESS_FILE_TYPE_FLASH_ARCHIVE = "x-empflash/flasharchive";

    /** */
    public static final int OTP_UNIT_ID_0 = 0;

    /** */
    public static final int OTP_UNIT_ID_2 = 2;

    /** */
    public static final int GLOBAL_DATA_SET_STORAGE_ID_ACCESS = 0;

    /** */
    public static final int GLOBAL_DATA_SET_STORAGE_ID_APPLICATION = 1;

    /** */
    public static final int GLOBAL_DATA_SET_STORAGE_ID_AUDIO = 2;

    /** */
    public static final int GLOBAL_DATA_SET_STORAGE_ID_TRIM_AREA = 3;

    /** */
    public static final int GLOBAL_DATA_SET_STORAGE_ID_GDFS = 4;

    /** */
    private static final String[] STORAGE_ID = new String[] {
        "ACCESS", "APPLICATION", "AUDIO", "TRIM_AREA", "GDFS"};

    /** */
    public static final ArrayList<String> GLOBAL_DATA_SET_STORAGE_IDS =
        new ArrayList<String>(Arrays.asList(STORAGE_ID));

    /** */
    public static final int SECURITY_UNIT_ID_STATIC = 0;

    /** */
    public static final int SECURITY_UNIT_ID_DYNAMIC = 1;

    /** constant used by batchTool */
    public static final boolean USE_BULK_TRUE = true;

    /** constant used by batchTool */
    public static final boolean USE_BULK_FALSE = false;

    // -----------------LC INTERFACE TEST COMMANDS
    // -------------------------------

    /** */
    public static final String TEST_COMMAND = "testCommand";

    /** */
    public static final String TEST_COMMAND2 = "testCommand2";

    // -----------------DEBUG COMMANDS-----------------------------------------

    /** */
    public static final String METHOD_NAME_DEBUG_GET_LOADER_STARTUP_STATUS = "dGetLoaderStartupStatus";

    // -----------------COMMAND GROUPS----------------------------------------

    /** */
    public static final int COMMAND_GROUP_SYSTEM = 1;

    /** */
    public static final int COMMAND_GROUP_FLASH = 2;

    /** */
    public static final int COMMAND_GROUP_FILE_SYSTEM = 3;

    /** */
    public static final int COMMAND_GROUP_OTP = 4;

    /** */
    public static final int COMMAND_GROUP_PARAMETER_STORAGE = 5;

    /** */
    public static final int COMMAND_GROUP_SECURITY = 6;

    /** */
    public static final int COMMAND_GROUP_RESET = 7;

    // -----------------------SYSTEM------------------------------------------

    /** */
    public static final int COMMAND_SYSTEM_INITIALIZE = 0;

    /** */
    public static final int COMMAND_SYSTEM_LOADER_STARTUP_STATUS = 1;

    /** */
    public static final int COMMAND_SYSTEM_CHANGE_BAUDRATE = 2;

    /** */
    public static final int COMMAND_SYSTEM_REBOOT = 3;

    /** */
    public static final int COMMAND_SYSTEM_SHUTDOWN = 4;

    /** */
    public static final int COMMAND_SYSTEM_SUPPORTED_COMMANDS = 5;

    /** */
    public static final int COMMAND_SYSTEM_EXECUTE_SOFTWARE = 6;

    /** */
    public static final int COMMAND_SYSTEM_AUTHENTICATE = 7;

    /** */
    public static final int COMMAND_SYSTEM_GET_CONTROL_KEYS = 8;

    /** */
    public static final int COMMAND_SYSTEM_AUTHENTICATION_CHALLENGE = 9;

    /** */
    public static final int COMMAND_COLLECT_DATA = 10;

    /** */
    private static final int COMMAND_SYSTEM_DEAUTHENTICATE = 11;

    // --------------------FLASH--------------------------------------

    /** */
    public static final int COMMAND_FLASH_PROCESS_FILE = 1;

    /** */
    public static final int COMMAND_FLASH_LIST_DEVICES = 2;

    /** */
    public static final int COMMAND_FLASH_DUMP_AREA = 3;

    /** */
    public static final int COMMAND_FLASH_ERASE_AREA = 4;

    /** */
    public static final int COMMAND_FLASH_SET_ENHANCED_AREA = 6;

    /** */
    public static final int COMMAND_FLASH_SELECT_LOADER_OPTIONS = 7;

    // -----------------------FILE SYSTEM--------------------------

    /** */
    public static final int COMMAND_FS_SET_READ_ONLY = 1;

    /** */
    public static final int COMMAND_FS_VOLUME_PROPERTIES = 2;

    /** */
    public static final int COMMAND_FS_FORMAT_VOLUME = 3;

    /** */
    public static final int COMMAND_FS_LIST_DIRECTORY = 4;

    /** */
    public static final int COMMAND_FS_MOVE_FILE = 5;

    /** */
    public static final int COMMAND_FS_DELETE_FILE = 6;

    /** */
    public static final int COMMAND_FS_COPY_FILE = 7;

    /** */
    public static final int COMMAND_FS_CREATE_DIRECTORY = 8;

    /** */
    public static final int COMMAND_FS_PROPERTIES = 9;

    /** */
    public static final int COMMAND_FS_CHANGE_ACCESS = 10;

    /** */
    public static final int COMMAND_FS_READ_LOAD_MODULE_MANIFESTS = 11;

    // --------------------OTP---------------------------------------------

    /** */
    public static final int COMMAND_OTP_READ_BITS = 1;

    /** */
    public static final int COMMAND_OTP_WRITE_BITS = 2;

    /** */
    public static final int COMMAND_OTP_SET_BITS = 2;

    /** */
    public static final int COMMAND_OTP_WRITE_AND_LOCK = 3;

    /** */
    public static final int COMMAND_OTP_STORE_SECURE_OBJECT = 4;

    // --------------PARAMETER STORAGE------------------------------------

    /** 3 */
    public static final int COMMAND_PS_READ_GLOBAL_DATA_UNIT = 1;

    /** 4 */
    public static final int COMMAND_PS_WRITE_GLOBAL_DATA_UNIT = 2;

    /** 5 */
    public static final int COMMAND_PS_READ_GLOBAL_DATA_SET = 3;

    /** 6 */
    public static final int COMMAND_PS_WRITE_GLOBAL_DATA_SET = 4;

    /** 7 */
    public static final int COMMAND_PS_ERASE_GLOBAL_DATA_SET = 5;

    // ---------------SECURITY---------------------------------------

    /** 1 */
    public static final int COMMAND_SECURITY_SET_DOMAIN = 1;

    /** 2 */
    public static final int COMMAND_SECURITY_GET_DOMAIN = 2;

    /** 3 */
    public static final int COMMAND_SECURITY_GET_PROPERTY = 3;

    /** 4 */
    public static final int COMMAND_SECURITY_SET_PROPERTY = 4;

    /** 5 */
    public static final int COMMAND_SECURITY_BIND_PROPERTIES = 5;

    /** 6 */
    public static final int COMMAND_INIT_ARB_TABLE = 6;

    /** 7 */
    public static final int COMMAND_SECURITY_WRITE_RPMB_KEY = 7;

    // ----------------RESET------------------------------------------

    /** 1 */
    public static final int COMMAND_RESET_INIT_REQUEST = 1;

    /** 2 */
    public static final int COMMAND_RESET_UPDATE_REQUEST = 2;

    // ----------------AUTHENTICATE-----------------------------------

    /** Control keys = 0 */
    public static final int AUTHENTICATE_CONTROL_KEYS = 0;

    /** certificate = 1 */
    public static final int AUTHENTICATE_CERTIFICATE = 1;

    /** Permanent = 2 */
    public static final int AUTHENTICATION_PERMANENT = 2;

    /** Permanent authentication with control keys = 3 */
    public static final int AUTHENTICATE_CONTROL_KEYS_PERSISTENT = 3;

    /** Permanent authentication with certificate = 4 */
    public static final int AUTHENTICATE_CERTIFICATE_PERSISTENT = 4;

    /** Get authentication state = 5 */
    public static final int AUTHENTICATE_GET_AUTHENTICATION_STATE = 5;

    // -----------------REBOOT ----------------------------------------
    /** */
    public static final int REBOOT_NORMAL = 0;

    /** */
    public static final int REBOOT_SERVICE_MODE = 1;

    /** */
    public static final int REBOOT_JTAG_ENABLED = 2;

    /** */
    public static final int REBOOT_SERVICE_MODE_JTAG_ENABLED = 3;

    // -----------------BAUDRATE--------------------------------------

    /** */
    public static final int BAUDRATE_9600 = 9600;

    /** */
    public static final int BAUDRATE_19200 = 19200;

    /** */
    public static final int BAUDRATE_57600 = 57600;

    /** */
    public static final int BAUDRATE_115200 = 115200;

    /** */
    public static final int BAUDRATE_230400 = 230400;

    /** */
    public static final int BAUDRATE_460800 = 460800;

    /** */
    public static final int BAUDRATE_921600 = 921600;

    /** */
    public static final int BAUDRATE_1834200 = 1834200;

    /** */
    public static final int BAUDRATE_FAIL = 3;

    // -----------------DOMAIN----------------------------------------

    /** */
    public static final int DOMAIN_SERVICE = 0;

    /** */
    public static final int DOMAIN_PRODUCT = 1;

    /** */
    public static final int DOMAIN_RND = 2;

    /** */
    public static final int DOMAIN_FACTORY_DOMAIN = 3;

    // ----------------STORE SECURE OBJECT----------------------------------------

    /** always for ROM code patches */
    public static final int SECURE_OBJECT_PATCH_HEADER_DEFINED = 0x000000FF;

    /** rootkey in otp */
    public static final int SECURE_OBJECT_RK_OTP = 0x00000000;

    /** root key in flash */
    public static final int SECURE_OBJECT_RK_FLASH = 0x00000001;

    // ----------------HASSIUM OTP AREA----------------------------------------

    /** */
    public static final int HASSIUM_OTP_FUSE0 = 0;

    /** */
    public static final int HASSIUM_OTP_FUSE3 = 3;

    /** */
    public static final int HASSIUM_OTP_FUSE0_CACHE = 9;

    /** */
    public static final int HASSIUM_OTP_FUSE3_CACHE = 10;

    // ------------------CORRUPT FLASH -----------------------------------------

    /** */
    public static final int GENERAL_RESPONSE_TIMEOUT = 0xDEADDEAD;

    /** */
    public static final String[] CORRUPT_FLASH_MESSAGE =
        new String[] {
            "The Loader did not respond to command Flash ListDevices. This indicates",
            "that the loader have stopped working because the flash memory is corrupt.",
            "To be able to erase the flash the ME must be started without mounting of flash",
            "and filesystem devices.", "Do you want Platform Assistant to start the ME without mounting the flash and",
            "filesystem device next time the ME is started?"};

    // TODO: change this when supported in loaders
    /** */
    public static final long CORRUPT_FLASH_LENGTH = 0xFFFFFFFFFFFFFFFFL;

    // -----------------LOADER COMMAND
    // METHODS----------------------------------------

    /** "systemLoaderStartupStatus" */
    public static final String METHOD_NAME_SYSTEM_LOADER_STARTUP_STATUS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_LOADER_STARTUP_STATUS);

    /** "systemChangeBaudRate" */
    public static final String METHOD_NAME_SYSTEM_CHANGE_BAUDRATE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_CHANGE_BAUDRATE);

    /** "systemReboot" */
    public static final String METHOD_NAME_SYSTEM_REBOOT =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_REBOOT);

    /** "systemShutdown" */
    public static final String METHOD_NAME_SYSTEM_SHUTDOWN =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_SHUTDOWN);

    /** "systemSupportedCommands" */
    public static final String METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_SUPPORTED_COMMANDS);

    /** "systemExecuteSoftware" */
    public static final String METHOD_NAME_SYSTEM_EXECUTE_SOFTWARE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_EXECUTE_SOFTWARE);

    /** "systemAuthenticate" */
    public static final String METHOD_NAME_SYSTEM_AUTHENTICATE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_AUTHENTICATE);

    /** "systemDeauthenticate" */
    public static final String METHOD_NAME_SYSTEM_DEAUTHENTICATE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_SYSTEM_DEAUTHENTICATE);

    /** "getFlashReport" */
    public static final String METHOD_NAME_SYSTEM_COLLECT_DATA =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SYSTEM, COMMAND_COLLECT_DATA);

    /** "flashProcessFile" */
    public static final String METHOD_NAME_FLASH_PROCESS_FILE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_PROCESS_FILE);

    /** "flashListDevices" */
    public static final String METHOD_NAME_FLASH_LIST_DEVICES =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_LIST_DEVICES);

    /** "flashDumpArea" */
    public static final String METHOD_NAME_FLASH_DUMP_AREA =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_DUMP_AREA);

    /** "flashEraseArea" */
    public static final String METHOD_NAME_FLASH_ERASE_AREA =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_ERASE_AREA);

    /** "flashSetEnhancedArea" */
    public static final String METHOD_NAME_FLASH_SET_ENHANCED_AREA =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_SET_ENHANCED_AREA);

    /** "flashSelectLoaderOptions" */
    public static final String METHOD_NAME_FLASH_SELECT_LOADER_OPTIONS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FLASH, COMMAND_FLASH_SELECT_LOADER_OPTIONS);

    /** "fileSystemVolumeProperties" */
    public static final String METHOD_NAME_FS_VOLUME_PROPERTIES =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_VOLUME_PROPERTIES);

    /** "fileSystemFormatVolume" */
    public static final String METHOD_NAME_FS_FORMAT_VOLUME =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_FORMAT_VOLUME);

    /** "fileSystemListDirectory" */
    public static final String METHOD_NAME_FS_LIST_DIRECTORY =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_LIST_DIRECTORY);

    /** "fileSystemMoveFile" */
    public static final String METHOD_NAME_FS_MOVE_FILE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_MOVE_FILE);

    /** "fileSystemDeleteFile" */
    public static final String METHOD_NAME_FS_DELETE_FILE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_DELETE_FILE);

    /** "fileSystemCopyFile" */
    public static final String METHOD_NAME_FS_COPY_FILE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_COPY_FILE);

    /** "fileSystemCreateDirectory" */
    public static final String METHOD_NAME_FS_CREATE_DIRECTORY =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_CREATE_DIRECTORY);

    /** "fileSystemProperties" */
    public static final String METHOD_NAME_FS_PROPERTIES =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_PROPERTIES);

    /** "fileSystemChangeAccess" */
    public static final String METHOD_NAME_FS_CHANGE_ACCESS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_CHANGE_ACCESS);

    /** "fileSystemReadLoadModuleManifests" */
    public static final String METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_FILE_SYSTEM, COMMAND_FS_READ_LOAD_MODULE_MANIFESTS);

    /** "otpReadBits" */
    public static final String METHOD_NAME_OTP_READ_BITS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_OTP, COMMAND_OTP_READ_BITS);

    /** "otpWriteBits" */
    public static final String METHOD_NAME_OTP_WRITE_BITS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_OTP, COMMAND_OTP_WRITE_BITS);

    /** "otpWriteBits" */
    public static final String METHOD_NAME_OTP_SET_BITS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_OTP, COMMAND_OTP_SET_BITS);

    /** "otpWriteAndLock" */
    public static final String METHOD_NAME_OTP_WRITE_AND_LOCK_BITS =
        LCCommandMappings.getMethodString(COMMAND_GROUP_OTP, COMMAND_OTP_WRITE_AND_LOCK);

    /** "parameterStorageReadGlobalDataUnit" */
    public static final String METHOD_NAME_PS_READ_GLOBAL_DATA_UNIT =
        LCCommandMappings.getMethodString(COMMAND_GROUP_PARAMETER_STORAGE, COMMAND_PS_READ_GLOBAL_DATA_UNIT);

    /** "parameterStorageWriteGlobalDataUnit" */
    public static final String METHOD_NAME_PS_WRITE_GLOBAL_DATA_UNIT =
        LCCommandMappings.getMethodString(COMMAND_GROUP_PARAMETER_STORAGE, COMMAND_PS_WRITE_GLOBAL_DATA_UNIT);

    /** "parameterStorageReadGlobalDataSet" */
    public static final String METHOD_NAME_PS_READ_GLOBAL_DATA_SET =
        LCCommandMappings.getMethodString(COMMAND_GROUP_PARAMETER_STORAGE, COMMAND_PS_READ_GLOBAL_DATA_SET);

    /** "parameterStorageWriteGlobalDataSet" */
    public static final String METHOD_NAME_PS_WRITE_GLOBAL_DATA_SET =
        LCCommandMappings.getMethodString(COMMAND_GROUP_PARAMETER_STORAGE, COMMAND_PS_WRITE_GLOBAL_DATA_SET);

    /** "parameterStorageEraseGlobalDataSet" */
    public static final String METHOD_NAME_PS_ERASE_GLOBAL_DATA_SET =
        LCCommandMappings.getMethodString(COMMAND_GROUP_PARAMETER_STORAGE, COMMAND_PS_ERASE_GLOBAL_DATA_SET);

    /** "securitySetDomain" */
    public static final String METHOD_NAME_SECURITY_SET_DOMAIN =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_SET_DOMAIN);

    /** "securityGetDomain" */
    public static final String METHOD_NAME_SECURITY_GET_DOMAIN =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_GET_DOMAIN);

    /** "securityGetProperty" */
    public static final String METHOD_NAME_SECURITY_GET_PROPERTY =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_GET_PROPERTY);

    /** "securitySetProperty" */
    public static final String METHOD_NAME_SECURITY_SET_PROPERTY =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_SET_PROPERTY);

    /** "securityBindProperties" */
    public static final String METHOD_NAME_SECURITY_BIND_PROPERTIES =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_BIND_PROPERTIES);

    /** "securityWriteRpmbKey" */
    public static final String METHOD_NAME_SECURITY_WRITE_RPMB_KEY =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_SECURITY_WRITE_RPMB_KEY);

    /** "securityInitArbTable" */
    public static final String METHOD_NAME_SECURITY_INIT_ARB_TABLE =
        LCCommandMappings.getMethodString(COMMAND_GROUP_SECURITY, COMMAND_INIT_ARB_TABLE);

    /** "securityStoreSecurityObject" */
    public static final String METHOD_NAME_OTP_STORE_SECURE_OBJECT =
        LCCommandMappings.getMethodString(COMMAND_GROUP_OTP, COMMAND_OTP_STORE_SECURE_OBJECT);

    /** "resetInitRequest" */
    public static final String METHOD_NAME_RESET_INIT_REQUEST =
        LCCommandMappings.getMethodString(COMMAND_GROUP_RESET, COMMAND_RESET_INIT_REQUEST);

    /** "resetUpdateRequest" */
    public static final String METHOD_NAME_RESET_UPDATE_REQUEST =
        LCCommandMappings.getMethodString(COMMAND_GROUP_RESET, COMMAND_RESET_UPDATE_REQUEST);

    /** */
    public static final String DEBUG_SUPPORTED_COMMANDS = "dSystemSupportedCommands";

    /** */
    public static final String DEBUG_LOADER_STARTUP = "dGetLoaderStartupStatus";

    /** */
    public static final String DEBUG_FLASH_LIST_DEVICES = "dflashListDevices";

    private LCDefinitions() {

    }

}
