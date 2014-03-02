package com.stericsson.sdk.brp;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * Factory method for command descriptions
 * 
 * @author xdancho
 * 
 */
public final class CommandDescription {

    CommandDescription instance = new CommandDescription();

    static Map<String, String> cmdDescMap = new HashMap<String, String>();

    static Map<String, String> argDescMap = new HashMap<String, String>();

    static {
        cmdDescMap.put(CommandName.BACKEND_DISABLE_AUTO_INITIALIZE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_DISABLE_AUTO_INITIALIZE.name())
            + " command is used to signal the backend to not initialize any mobile "
            + "equipment that can be in a uninitialized state.");
        cmdDescMap.put(CommandName.BACKEND_DISABLE_AUTO_SENSE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_DISABLE_AUTO_SENSE.name())
            + " command is used to signal the server backend to stop looking for new "
            + "equipments connected to the host.");
        cmdDescMap.put(CommandName.BACKEND_ENABLE_AUTO_INITIALIZE.name(), CommandName.BACKEND_ENABLE_AUTO_INITIALIZE
            + "desc");
        cmdDescMap.put(CommandName.BACKEND_ENABLE_AUTO_INITIALIZE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_ENABLE_AUTO_INITIALIZE.name())
            + " command is used to signal the backend to auto-initialize any mobile"
            + " equipment that can be in a uninitialized state.");
        cmdDescMap.put(CommandName.BACKEND_ENABLE_AUTO_SENSE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_ENABLE_AUTO_SENSE.name())
            + " command is used to signal the server backend to start looking for"
            + " new equipments connected to the host.");
        cmdDescMap.put(CommandName.BACKEND_GET_ACTIVE_PROFILE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_ACTIVE_PROFILE.name())
            + " message used to retrieve the current active profile on the server backend.");
        cmdDescMap.put(CommandName.BACKEND_GET_AUTO_INITIALIZE_STATUS.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_AUTO_INITIALIZE_STATUS.name())
            + " command is used to retrieve the current status of the 'auto-initialize' feature.");
        cmdDescMap.put(CommandName.BACKEND_GET_AUTO_SENSE_STATUS.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_AUTO_SENSE_STATUS.name())
            + " command is used to retrieve the current status for the 'auto sense'"
            + " (if the backend should accept connections from mobile equipments or not).");
        cmdDescMap.put(CommandName.BACKEND_GET_AVAILABLE_PROFILES.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_AVAILABLE_PROFILES.name())
            + " command is used to retrieve a list of available profiles known to the server backend.");
        cmdDescMap.put(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name())
            + " command is used to retrieve a list of currently connected equipments.");
        cmdDescMap.put(CommandName.BACKEND_GET_CORE_DUMP_LOCATION.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_CORE_DUMP_LOCATION.name())
            + " command is used to retrieve the location where the backend server stores any core dump data.");
        cmdDescMap.put(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT.name())
            + " command is used to trigger the backend to notify the client when new equipment is connected.");
        cmdDescMap.put(CommandName.BACKEND_RELOAD_CONFIGURATION.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_RELOAD_CONFIGURATION.name())
            + " command is used to signal the server backend to reload its configuration. "
            + "This can be useful if configuration was changed manually.");
        cmdDescMap.put(CommandName.BACKEND_SET_ACTIVE_PROFILE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_SET_ACTIVE_PROFILE.name())
            + " command is used to change the current active profile on the server backend.");
        cmdDescMap.put(CommandName.BACKEND_SET_CORE_DUMP_LOCATION.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_SET_CORE_DUMP_LOCATION.name())
            + " command is used to configure the location where the server backend will put core dump data.");
        cmdDescMap.put(CommandName.BACKEND_GET_EQUIPMENT_PROFILE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_EQUIPMENT_PROFILE.name())
            + " command is used to get the active profile for a connected equipment.");
        cmdDescMap.put(CommandName.BACKEND_SET_EQUIPMENT_PROFILE.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_SET_EQUIPMENT_PROFILE.name())
            + " command is used to set the active profile for a connected equipment.");
        cmdDescMap.put(CommandName.BACKEND_SHUTDOWN_BACKEND.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_SHUTDOWN_BACKEND.name())
            + " command is used to shutdown the backend remotely.");
        cmdDescMap.put(CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name())
            + " command is used to retrieve a list of available security properties defined in active profile.");
        cmdDescMap.put(CommandName.BACKEND_TRIGGER_UART_PORT.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_TRIGGER_UART_PORT.name())
            + " command is used to trigger UART port specified by its name.");
        cmdDescMap.put(CommandName.BACKEND_GET_DEVICE_INFO.name(), CommandName
            .getCLISyntax(CommandName.BACKEND_GET_DEVICE_INFO.name())
            + " command is used to get basic device information.");

        // CORE DUMP
        cmdDescMap.put(CommandName.COREDUMP_LIST_FILE.name(), CommandName.getCLISyntax(CommandName.COREDUMP_LIST_FILE
            .name())
            + " command is used to retrieve a list of available file for a dumping ME.");

        cmdDescMap.put(CommandName.COREDUMP_DOWNLOAD_DUMP.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_DOWNLOAD_DUMP.name())
            + " command is used to download a dump file from a dumping ME.");

        cmdDescMap.put(CommandName.COREDUMP_SET_AUTO_DOWNLOAD.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_SET_AUTO_DOWNLOAD.name())
            + " command is used to set and unset auto download of dump file.");

        cmdDescMap.put(CommandName.COREDUMP_GET_AUTO_DOWNLOAD.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_GET_AUTO_DOWNLOAD.name())
            + " command is used to get status of auto download of dump file.");

        cmdDescMap.put(CommandName.COREDUMP_SET_AUTO_DELETE.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_SET_AUTO_DELETE.name())
            + " command is used to set and unset auto delete of newly downloaded dump file.");

        cmdDescMap.put(CommandName.COREDUMP_GET_AUTO_DELETE_STATUS.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_GET_AUTO_DELETE_STATUS.name())
            + " command is used to get status of auto delete for newly downloaded dump file.");

        cmdDescMap.put(CommandName.COREDUMP_DELETE_DUMP.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_DELETE_DUMP.name())
            + " command is used to delete a dump file from a dumping ME.");

        cmdDescMap.put(CommandName.COREDUMP_SET_SUBSCRIPTION.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_SET_SUBSCRIPTION.name())
            + " command is used to set up a subscription for dump events.");

        cmdDescMap.put(CommandName.COREDUMP_GET_SUBSCRIPTION.name(), CommandName
            .getCLISyntax(CommandName.COREDUMP_GET_SUBSCRIPTION.name())
            + " command is used to get a list of subscriptions.");

        // FLASH
        cmdDescMap.put(CommandName.FLASH_DUMP_AREA.name(), CommandName.getCLISyntax(CommandName.FLASH_DUMP_AREA.name())
            + " command is used to dump an area of the flash from the specified equipment to specified path.");
        cmdDescMap.put(CommandName.FLASH_ERASE_AREA.name(), CommandName.getCLISyntax(CommandName.FLASH_ERASE_AREA
            .name())
            + " command is used to erase an area (or part of area) on the flash on the specified equipment.");
        cmdDescMap.put(CommandName.FLASH_PROCESS_FILE.name(), CommandName.getCLISyntax(CommandName.FLASH_PROCESS_FILE
            .name())
            + " command is used to let the loader executing on the equipment process a specified file."
            + " In most cases perform a flash operation.");
        cmdDescMap.put(CommandName.FLASH_LIST_DEVICES.name(), CommandName.getCLISyntax(CommandName.FLASH_LIST_DEVICES
            .name())
            + " command is used to let the loader retrieve detected block devices in the ME.");
        cmdDescMap.put(CommandName.FLASH_SET_ENHANCED_AREA.name(), CommandName
            .getCLISyntax(CommandName.FLASH_SET_ENHANCED_AREA.name())
            + " command is used to set an area (or part of area) as enhanced.");
        cmdDescMap.put(CommandName.FLASH_SELECT_LOADER_OPTIONS.name(), CommandName
                .getCLISyntax(CommandName.FLASH_SELECT_LOADER_OPTIONS.name())
                + " command is used to set temporary loader options (e.g. write forcibly enhanced image on unenhaced area).");

        // PARAMETER STORAGE
        cmdDescMap.put(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name(), CommandName
            .getCLISyntax(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name())
            + " command is used to erase a set of global data storage on connected equipment.");
        cmdDescMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name(), CommandName
            .getCLISyntax(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name())
            + " command is used to retrieve a complete set of global data from specified"
            + " storage on connected equipment.");
        cmdDescMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name(), CommandName
            .getCLISyntax(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name())
            + " command is used to read a single unit of parameter storage data from specified "
            + "storage on connected equipment.");
        cmdDescMap.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(), CommandName
            .getCLISyntax(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name())
            + " command is used to write a complete set of global data to specified storage on connected equipment.");
        cmdDescMap.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), CommandName
            .getCLISyntax(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name())
            + " command is used to write a single unit of parameter storage data to specified storage on "
            + "connected equipment.");

        // SECURITY
        cmdDescMap.put(CommandName.SECURITY_BIND_PROPERTIES.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_BIND_PROPERTIES.name())
            + " command is used to bind the properties previously set with SECURITY_SET_PROPERTIES message.");
        cmdDescMap.put(CommandName.SECURITY_GET_DOMAIN.name(), CommandName.getCLISyntax(CommandName.SECURITY_GET_DOMAIN
            .name())
            + " command is used to retrieve the security domain for connected equipment.");
        cmdDescMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name())
            + " command is used to retrieve specified property of specified equipment.");
        cmdDescMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name())
            + " command is used to retrieve a list of properties of specified equipment.");

        cmdDescMap.put(CommandName.SECURITY_SET_DOMAIN.name(), CommandName.getCLISyntax(CommandName.SECURITY_SET_DOMAIN
            .name())
            + " command is used to update the security domain for connected equipment.");
        cmdDescMap.put(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name())
            + " command is used to set one property of specified equipment.");

        cmdDescMap.put(CommandName.SECURITY_WRITE_RPMB_KEY.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_WRITE_RPMB_KEY.name())
            + " command is used to write authentication key into RPMB.");

        // SYSTEM
        cmdDescMap.put(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name())
            + " command is used to authenticate the user to elevate the rights to perform certain operations "
            + "on specified equipment. ");
        cmdDescMap.put(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name())
            + " command is used to authenticate the user to elevate the rights to perform certain operations on"
            + " specified equipment. ");
        cmdDescMap.put(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name())
            + "command is used for permanent authentication.");
        cmdDescMap.put(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name())
            + "command is serving for getting authentication state.");
        cmdDescMap.put(CommandName.SYSTEM_EXECUTE_SOFTWARE.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_EXECUTE_SOFTWARE.name())
            + " command is used to download and execute specified software on specified equipment.");
        cmdDescMap
            .put(CommandName.SYSTEM_INITIALIZE_EQUIPMENT.name(), CommandName.SYSTEM_INITIALIZE_EQUIPMENT + "desc");
        cmdDescMap.put(CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_REBOOT_EQUIPMENT.name())
            + " command is used to send a reboot command to specified equipment in order for it to reboot in"
            + " specified mode.");
        cmdDescMap.put(CommandName.SYSTEM_SEND_RAW_DATA.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_SEND_RAW_DATA.name())
            + " command is used to send raw data to the connected equipment.");
        cmdDescMap.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name())
            + " command is used to send a shutdown command to specified equipment in order for it to power down.");
        cmdDescMap.put(CommandName.SYSTEM_COLLECT_DATA.name(), CommandName.getCLISyntax(CommandName.SYSTEM_COLLECT_DATA
            .name())
            + " command is used to get flashing status report from ME.");
        cmdDescMap.put(CommandName.SECURITY_INIT_ARB_TABLE.name(), CommandName
            .getCLISyntax(CommandName.SECURITY_INIT_ARB_TABLE.name())
            + " command is used to initialize ARB table in ME.");
        cmdDescMap.put(CommandName.SYSTEM_DEAUTHENTICATE.name(), CommandName
            .getCLISyntax(CommandName.SYSTEM_DEAUTHENTICATE.name())
            + " command is used to deauthentication");

        // FILE SYSTEM
        cmdDescMap.put(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name())
            + " command is used to retrieve properties of the specified file system volume.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_FORMAT_VOLUME.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_FORMAT_VOLUME.name())
            + " command is used to format specified file system volume.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name())
            + " command is used to list files and directories in specified path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_MOVE_FILE.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_MOVE_FILE.name())
            + " command is used to move or rename file from the source path to the destination path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_DELETE_FILE.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_DELETE_FILE.name())
            + " command is used to delete file or directory in specified path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_COPY_FILE.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_COPY_FILE.name())
            + " command is used to copy file from the source path to the destination path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name())
            + " command is used to create directory in specified path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_PROPERTIES.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_PROPERTIES.name())
            + " command is used to retrieve properties of file or directory in specified path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name())
            + " command is used to change access permissions of specified path.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_LIST_MODULES.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_LIST_MODULES.name())
            + " command is used to list load modules that are installed on ME.");
        cmdDescMap.put(CommandName.FILE_SYSTEM_CHECK_COMPATIBILITY.name(), CommandName
            .getCLISyntax(CommandName.FILE_SYSTEM_CHECK_COMPATIBILITY.name())
            + " command is used to check compatibility of given load module with "
            + "the load modules that are installed on ME.");

        // OTP
        cmdDescMap.put(CommandName.OTP_BURN_OTP.name(), CommandName.getCLISyntax(CommandName.OTP_BURN_OTP.name())
            + " command is used to burn OTP data provided in the specified file to the specified ME.");
        cmdDescMap.put(CommandName.OTP_BURN_OTP_DATA.name(), CommandName.getCLISyntax(CommandName.OTP_BURN_OTP_DATA
            .name())
            + " command is used to burn OTP data provided as Base64 string to the specified ME.");
        cmdDescMap.put(CommandName.OTP_READ_OTP.name(), CommandName.getCLISyntax(CommandName.OTP_READ_OTP.name())
            + " command is used to read OTP data from the specified ME and write it to the specified file.");
        cmdDescMap.put(CommandName.OTP_READ_OTP_DATA.name(), CommandName.getCLISyntax(CommandName.OTP_READ_OTP_DATA
            .name())
            + " command is used to read OTP data from the specified ME and return it as Base64 string.");

        // ARGS
        argDescMap.put(AbstractCommand.PARAMETER_EQUIPMENT_ID,
            "The identifier string for a connected equipment. Set to '" + AbstractCommand.EQUIPMENT_ID_NEXT
                + "' to use next connected equipment. Leave empty or set to '" + AbstractCommand.EQUIPMENT_ID_CURRENT
                + "' to use the currently connected equipment.");
        argDescMap.put(AbstractCommand.PARAMETER_PROFILE_NAME, "The name of the profile");
        argDescMap.put(AbstractCommand.PARAMETER_REBOOT_MODE, "Mode of reboot operation");
        argDescMap.put(AbstractCommand.PARAMETER_PATH, "Path on host or equipment ");
        argDescMap.put(AbstractCommand.PARAMETER_SIGN_PACKAGE, "Name of sign package ");
        argDescMap.put(AbstractCommand.PARAMETER_CONTROL_KEYS, "A set of control keys to use for authentication");
        argDescMap.put(AbstractCommand.PARAMETER_STORAGE_ID, "Storage identifier");
        argDescMap.put(AbstractCommand.PARAMETER_UNIT_ID, "Unit identifier");
        argDescMap.put(AbstractCommand.PARAMETER_UNIT_DATA, "Data (bytes) to write ");
        argDescMap.put(AbstractCommand.PARAMETER_DOMAIN, "Domain identifier");
        argDescMap.put(AbstractCommand.PARAMETER_EQUIPMENT_PROPERTIES,
            "A structure describing one or more properties of the equipment");
        argDescMap.put(AbstractCommand.PARAMETER_EQUIPMENT_PROPERTY, "Property name or property id.");
        argDescMap.put(AbstractCommand.PARAMETER_EQUIPMENT_PROPERTY_SET,
            "Property name or property id followed by equals sign and the value of property to be set.");
        argDescMap.put(AbstractCommand.PARAMETER_AREA_PATH, "Absolute path of flash area");
        argDescMap.put(AbstractCommand.PARAMETER_LENGTH, "Length in bytes ");
        argDescMap.put(AbstractCommand.PARAMETER_OFFSET, "Offset in bytes");
        argDescMap.put(AbstractCommand.PARAMETER_WAIT, "Wait if profiles are not loaded yet.");
        argDescMap.put(AbstractCommand.PARAMETER_RAW_DATA, "The raw data to send.");
        argDescMap.put(AbstractCommand.PARAMETER_TYPE, "Type of flashing report.");
        argDescMap.put(AbstractCommand.PARAMETER_MESSAGE, "The message.");
        argDescMap.put(AbstractCommand.PARAMETER_SKIP_REDUNDANT_AREA,
            "Determines if redundant area should be skipped (true/false)");
        argDescMap.put(AbstractCommand.PARAMETER_INCLUDE_BAD_BLOCKS,
            "Determines if bad blocks should be included or not(true/false)");
        argDescMap.put(AbstractCommand.PARAMETER_COREDUMP_FILE_NAME, "The core dump file name.");
        argDescMap.put(AbstractCommand.PARAMETER_COREDUMP_SET_STATUS, "Status identifier enable/disable");
        argDescMap.put(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_IP, "The IP address of the subscriber");
        argDescMap.put(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIBER_PORT,
            "The port on which the subscriber is listening for notification");
        argDescMap.put(AbstractCommand.PARAMETER_COREDUMP_SUBSCRIPTION_TYPE, "The type of subscription. "
            + "Possible types are: 0 (detect dumping ME) and 1 (detect automatically downloaded dump)");
        argDescMap.put(AbstractCommand.PARAMETER_DEVICE_PATH, "Device path of the file system volume");
        argDescMap.put(AbstractCommand.PARAMETER_TARGET_PATH, "Target file or directory path in the file system");
        argDescMap.put(AbstractCommand.PARAMETER_SOURCE_PATH, "Source file path in the file system");
        argDescMap.put(AbstractCommand.PARAMETER_DESTINATION_PATH,
            "Destination file or directory path in the file system");
        argDescMap.put(AbstractCommand.PARAMETER_ACCESS, "Access permissions");
        argDescMap.put(AbstractCommand.PARAMETER_PORT_NAME, "UART port name");
        argDescMap.put(AbstractCommand.PARAMETER_OTP_DATA, "OTP data encoded as Base64 string");
        argDescMap.put(AbstractCommand.PARAMETER_PERMANENT, "Flags permanent authentication");

    }

    private CommandDescription() {

    }

    /**
     * get the command description
     * 
     * @param cmd
     *            the command to look up
     * @param cliSyntax
     *            if the cmd is in CLI syntax form
     * @return description of the command
     */

    public static String getCommandDescription(String cmd, boolean cliSyntax) {
        String cmdName = "";
        // find enum
        if (cliSyntax) {
            for (CommandName enumName : CommandName.values()) {
                if (CommandName.getCLISyntax(enumName.name()).equals(cmd.toLowerCase(Locale.getDefault()))) {
                    cmdName = enumName.name();
                    break;
                }
            }
        }

        return cmdDescMap.get(cmdName);
    }

    /**
     * get the command description
     * 
     * @param cmd
     *            the command to look up
     * @return description of the command
     */
    public static String getCommandDescription(String cmd) {
        return getCommandDescription(cmd, false);
    }

    /**
     * get the argument description
     * 
     * @param arg
     *            the argument to look up
     * @return the description of the argument
     */
    public static String getArgumentDescription(String arg) {
        return argDescMap.get(arg);
    }

}
