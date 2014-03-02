package com.stericsson.sdk.brp;

import java.util.Locale;

/**
 * Enum of all available remote client commands
 * 
 * @author xolabju
 * 
 */
public enum CommandName {

    /** BACKEND_RELOAD_CONFIGURATION */
    BACKEND_RELOAD_CONFIGURATION,

    /** BACKEND_ENABLE_AUTO_SENSE */
    BACKEND_ENABLE_AUTO_SENSE,

    /** BACKEND_DISABLE_AUTO_SENSE */
    BACKEND_DISABLE_AUTO_SENSE,

    /** BACKEND_GET_AUTO_SENSE_STATUS */
    BACKEND_GET_AUTO_SENSE_STATUS,

    /** BACKEND_ENABLE_AUTO_INITIALIZE */
    BACKEND_ENABLE_AUTO_INITIALIZE,

    /** BACKEND_DISABLE_AUTO_INITIALIZE */
    BACKEND_DISABLE_AUTO_INITIALIZE,

    /** BACKEND_GET_AUTO_INITIALIZE_STATUS */
    BACKEND_GET_AUTO_INITIALIZE_STATUS,

    /** BACKEND_SET_ACTIVE_PROFILE;&lt;profile name&gt; */
    BACKEND_SET_ACTIVE_PROFILE,

    /** BACKEND_SET_EQUIPMENT_PROFILE;&lt;equipment-id&gt;;&lt;profile name&gt; */
    BACKEND_SET_EQUIPMENT_PROFILE,

    /** BACKEND_GET_EQUIPMENT_PROFILE;&lt;equipment-id&gt; */
    BACKEND_GET_EQUIPMENT_PROFILE,

    /** BACKEND_GET_ACTIVE_PROFILE */
    BACKEND_GET_ACTIVE_PROFILE,

    /** BACKEND_GET_AVAILABLE_PROFILES */
    BACKEND_GET_AVAILABLE_PROFILES,

    /** BACKEND_GET_CONNECTED_EQUIPMENTS */
    BACKEND_GET_CONNECTED_EQUIPMENTS,

    /** BACKEND_GET_NEXT_CONNECTED_EQUIPMENT */
    BACKEND_GET_NEXT_CONNECTED_EQUIPMENT,

    /** The BACKEND_GET_CORE_DUMP_LOCATION */
    BACKEND_GET_CORE_DUMP_LOCATION,

    /** BACKEND_SET_CORE_DUMP_LOCATION;&lt;path&gt; */
    BACKEND_SET_CORE_DUMP_LOCATION,

    /** BACKEND_SHUTDOWN_BACKEND;&lt;message&gt; */
    BACKEND_SHUTDOWN_BACKEND,

    /** BACKEND_SET_LOCAL_SIGNING;&lt;value&gt; */
    BACKEND_SET_LOCAL_SIGNING,

    /** BACKEND_TRIGGER_UART_PORT;&lt;port_name&gt; */
    BACKEND_TRIGGER_UART_PORT,

    /** BACKEND_GET_DEVICE_INFO;&lt;equipment-id&gt; */
    BACKEND_GET_DEVICE_INFO,

    /** COREDUMP_LIST_FILE;&lt;equipment-id&gt; */
    COREDUMP_LIST_FILE,

    /** COREDUMP_DOWNLOAD_DUMP;&lt;equipment-id&gt;;&lt;dump-file-name&gt;;&lt;path&gt; */
    COREDUMP_DOWNLOAD_DUMP,

    /** COREDUMP_SET_AUTO_DOWNLOAD;&lt;set_status&gt; */
    COREDUMP_SET_AUTO_DOWNLOAD,

    /** COREDUMP_GET_AUTO_DOWNLOAD */
    COREDUMP_GET_AUTO_DOWNLOAD,

    /** COREDUMP_SET_AUTO_DELETE;&lt;set_status&gt; */
    COREDUMP_SET_AUTO_DELETE,

    /** COREDUMP_GET_AUTO_DELETE */
    COREDUMP_GET_AUTO_DELETE_STATUS,

    /**
     * COREDUMP_SET_SUBSCRIPTION;&lt;subscriber-ip&gt;;&lt;subscriber-port&gt;;&lt;subscription-type
     * &gt;
     */
    COREDUMP_SET_SUBSCRIPTION,

    /** COREDUMP_GET_SUBSCRIPTION;&lt;subscriber-ip&gt;;&lt;subscriber-port&gt;;&lt; */
    COREDUMP_GET_SUBSCRIPTION,

    /** COREDUMP_DELETE_DUMP;&lt;equipment-id&gt;;&lt;dump-file-name&gt; */
    COREDUMP_DELETE_DUMP,

    /** SYSTEM_INITIALIZE_EQUIPMENT;&lt;equipment-id&gt; */
    SYSTEM_INITIALIZE_EQUIPMENT,

    /** SYSTEM_SHUTDOWN_EQUIPMENT;&lt;equipment-id&gt; */
    SYSTEM_SHUTDOWN_EQUIPMENT,

    /** SYSTEM_REBOOT_EQUIPMENT;&lt;equipment-id&gt;;&lt;mode&gt; */
    SYSTEM_REBOOT_EQUIPMENT,

    /** SYSTEM_EXECUTE_SOFTWARE;&lt;equipment-id&gt;;&lt;path&gt; */
    SYSTEM_EXECUTE_SOFTWARE,

    /** SYSTEM_AUTHENTICATE_CERTIFICATE;&lt;equipment-id;&lt;sign package name&gt; */
    SYSTEM_AUTHENTICATE_CERTIFICATE,

    /** SYSTEM_AUTHENTICATE_CONTROL_KEYS;&lt;equipment-id&gt;;&lt;key set&gt; */
    SYSTEM_AUTHENTICATE_CONTROL_KEYS,

    /** PERMANENT_AUTHENTICATION;&lt;equipment-id&gt;;&gt; */
    SYSTEM_PERMANENT_AUTHENTICATION,

    /** DEAUTHENTIFICATE;&lt;equipment-id&gt;;&lt;permanent&gt;; */
    SYSTEM_DEAUTHENTICATE,

    /** GET_AUTHENTICATION_TYPE;&lt;equipment-id&gt;;&gt; */
    SYSTEM_GET_AUTHENTICATION_STATE,

    /** SYSTEM_SEND_RAW_DATA;&lt;equipment-id&gt;;&lt;raw data&gt; */
    SYSTEM_SEND_RAW_DATA,

    /** SYSTEM_COLLECT_DATA;&lt;type&gt; */
    SYSTEM_COLLECT_DATA,

    /** FLASH_PROCESS_FILE;&lt;equipment-id&gt;;&lt;path&gt;[;CANCEL] */
    FLASH_PROCESS_FILE,

    /**
     * FLASH_DUMP_AREA;&lt;equipment-id&gt;;&lt;area
     * path&gt;;&lt;offset&gt;;&lt;length&gt;;&lt;path&gt;[;CANCEL]
     */
    FLASH_DUMP_AREA,

    /**
     * FLASH_ERASE_AREA;&lt;equipment-id&gt;;&lt;area path&gt;;&lt;offset&gt;;&lt;length&gt;
     */
    FLASH_ERASE_AREA,

    /**
     * FLASH_SET_ENHANCED_AREA;&lt;equipment-id&gt;;&lt;area path&gt;;&lt;soffset&gt;;&lt;length&gt;
     */
    FLASH_SET_ENHANCED_AREA,

    /**
     * FLASH_LIST_DEVICES;&lt;equipment-id&gt;
     */
    FLASH_LIST_DEVICES,

    /**
     * FLASH_SELECT_LOADER_OPTIONS;&lt;equipment-id&gt;
     */
    FLASH_SELECT_LOADER_OPTIONS,

    /**
     * PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET;&lt;equipment-id&gt;;&lt;storage- id&gt;;&lt;path&gt;
     */
    PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET,

    /**
     * PARAMETER_STORAGE_READ_GLOBAL_DATA_SET;&lt;equipment-id&gt;;&lt;storage- id&gt;;&lt;path&gt;
     */
    PARAMETER_STORAGE_READ_GLOBAL_DATA_SET,

    /**
     * PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET;&lt;equipment-id&gt;;&lt;storage- id&gt;
     */
    PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET,

    /**
     * PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT;&lt;equipment-id&gt;;&lt;storage
     * -id&gt;;&lt;unit-id& gt;;&lt;unit-data&gt;
     */
    PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT,

    /**
     * PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT;&lt;equipment-id&gt;;&lt;storage- id&gt;;&lt;unit-id&
     * gt;
     */
    PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT,

    /** SECURITY_SET_DOMAIN;&lt;equipment-id&gt;;&lt;domain&gt; */
    SECURITY_SET_DOMAIN,

    /** SECURITY_GET_DOMAIN;&lt;equipment-id&gt; */
    SECURITY_GET_DOMAIN,

    /** SECURITY_GET_EQUIPMENT_PROPERTIES;&lt;equipment-id&gt; */
    SECURITY_GET_EQUIPMENT_PROPERTIES,

    /** SECURITY_GET_EQUIPMENT_PROPERTY;&lt;equipment-id&gt;;&lt;property name | property id&gt; */
    SECURITY_GET_EQUIPMENT_PROPERTY,

    /** SECURITY_GET_AVAILABLE_SECURITY_PROPERTIES */
    BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES,

    /** SECURITY_SET_EQUIPMENT_PROPERTY;&lt;equipment-id&gt;;&lt;property name = property value&gt; */
    SECURITY_SET_EQUIPMENT_PROPERTY,

    /** SECURITY_BIND_PROPERTIES;&lt;equipment-id&gt; */
    SECURITY_BIND_PROPERTIES,

    /** SECURITY_INIT_ARB_TABLE;&lt;puarbdata&gt; */
    SECURITY_INIT_ARB_TABLE,

    /** SECURITY_WRITE_RPMB_KEY;&lt;equipment-id&gt; */
    SECURITY_WRITE_RPMB_KEY,

    /** FILE_SYSTEM_VOLUME_PROPERTIES;&lt;equipment-id&gt;;&lt;device-path&gt; */
    FILE_SYSTEM_VOLUME_PROPERTIES,

    /** FILE_SYSTEM_FORMAT_VOLUME;&lt;equipment-id&gt;;&lt;device-path&gt; */
    FILE_SYSTEM_FORMAT_VOLUME,

    /** FILE_SYSTEM_LIST_DIRECTORY;&lt;equipment-id&gt;;&lt;target-path&gt; */
    FILE_SYSTEM_LIST_DIRECTORY,

    /** FILE_SYSTEM_MOVE_FILE;&lt;equipment-id&gt;;&lt;source-path&gt;;&lt;destination-path&gt; */
    FILE_SYSTEM_MOVE_FILE,

    /** FILE_SYSTEM_DELETE_FILE;&lt;equipment-id&gt;;&lt;target-path&gt; */
    FILE_SYSTEM_DELETE_FILE,

    /** FILE_SYSTEM_COPY_FILE;&lt;equipment-id&gt;;&lt;source-path&gt;;&lt;destination-path&gt; */
    FILE_SYSTEM_COPY_FILE,

    /** FILE_SYSTEM_CREATE_DIRECTORY;&lt;equipment-id&gt;;&lt;target-path&gt; */
    FILE_SYSTEM_CREATE_DIRECTORY,

    /** FILE_SYSTEM_PROPERTIES;&lt;equipment-id&gt;;&lt;target-path&gt; */
    FILE_SYSTEM_PROPERTIES,

    /** FILE_SYSTEM_CHANGE_ACCESS;&lt;equipment-id&gt;;&lt;target-path&gt;;&lt;access&gt; */
    FILE_SYSTEM_CHANGE_ACCESS,

    /** FILE_SYSTEM_LIST_MODULES;&lt;equipment-id&gt; */
    FILE_SYSTEM_LIST_MODULES,

    /** FILE_SYSTEM_CHECK_COMPATIBILITY;&lt;equipment-id&gt;;&lt;path&gt; */
    FILE_SYSTEM_CHECK_COMPATIBILITY,

    /** OTP_BURN_OTP;&lt;equipment-id&gt;;&lt;path&gt; */
    OTP_BURN_OTP,

    /** OTP_BURN_OTP_DATA;&lt;equipment-id&gt;;&lt;otp-data&gt; */
    OTP_BURN_OTP_DATA,

    /** OTP_READ_OTP;&lt;equipment-id&gt;;&lt;path&gt; */
    OTP_READ_OTP,

    /** OTP_READ_OTP_DATA;&lt;equipment-id&gt;;&lt;otp-data&gt; */
    OTP_READ_OTP_DATA;

    static final String PARAMETER_STORAGE_PREFIX = "PARAMETER_STORAGE_";

    static final String FILE_SYSTEM_PREFIX = "FILE_SYSTEM_";

    /** BOOT task ID */
    public static final String BOOT = "BOOT";

    /** CHANGE_BAUD_RATE task ID */
    public static final String CHANGE_BAUD_RATE = "CHANGE_BAUD_RATE";

    /** LOADER_COMM_INIT Task ID */
    public static final String LOADER_COMM_INIT = "LOADER_COMM_INIT";

    /** BOOTING_DONE Task ID */
    public static final String BOOTING_DONE = "BOOTING_DONE";

    /**
     * returns the CLI syntax of the BRP commands
     * 
     * @param cmd
     *            in BRP syntax
     * @return the CLI syntax of the command
     */
    public static String getCLISyntax(String cmd) {

        String returnString = "";
        if (cmd.startsWith(PARAMETER_STORAGE_PREFIX)) {
            returnString = cmd.split(PARAMETER_STORAGE_PREFIX)[1];
        } else if (cmd.startsWith(FILE_SYSTEM_PREFIX)) {
            returnString = cmd.split(FILE_SYSTEM_PREFIX)[1];
        } else {
            returnString = cmd.substring(cmd.indexOf("_") + 1, cmd.length());
        }

        return returnString.toLowerCase(Locale.getDefault());
    }

}
