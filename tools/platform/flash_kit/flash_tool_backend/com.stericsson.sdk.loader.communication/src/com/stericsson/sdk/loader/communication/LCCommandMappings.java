package com.stericsson.sdk.loader.communication;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * Class responsible for loader command mappings
 * 
 * @author xdancho
 * 
 */
public final class LCCommandMappings {

    private static final String NOT_SUPPORTED = "Not supported";

    private LCCommandMappings() {
    }

    /** */
    public static final String[] GROUP_ID = new String[] {
        NOT_SUPPORTED, "system", "flash", "fileSystem", "otp", "parameterStorage", "security", "reset"};

    /** */
    public static final ArrayList<String> GROUP = new ArrayList<String>(Arrays.asList(GROUP_ID));

    /** */
    public static final String[] COMMAND_SYSTEM =
        new String[] {
            NOT_SUPPORTED, "LoaderStartupStatus", "ChangeBaudRate", "Reboot", "Shutdown", "SupportedCommands",
            "ExecuteSoftware", "Authenticate", "GetControlKeys ", "GetProgressStatus", "CollectData", "Deauthenticate"};

    /**
     * NOT_SUPPORTED[6] is because of command 'send_raw_data' which is implemented in LCD but not
     * supported by FlashKit
     */
    public static final String[] COMMAND_FLASH = new String[] {
        NOT_SUPPORTED, "ProcessFile", "ListDevices", "DumpArea", "EraseArea", NOT_SUPPORTED, "SetEnhancedArea",
        "SelectLoaderOptions"};

    /** */
    public static final String[] COMMAND_FILE_SYSTEM =
        new String[] {
            NOT_SUPPORTED, NOT_SUPPORTED, "VolumeProperties", "FormatVolume", "ListDirectory", "MoveFile",
            "DeleteFile", "CopyFile", "CreateDirectory", "Properties", "ChangeAccess", "ReadLoadModuleManifests"};

    /** */
    public static final String[] COMMAND_OTP = new String[] {
        NOT_SUPPORTED, "ReadBits", "SetBits", "WriteAndLock", "StoreSecurityObject"};

    /** */
    public static final String[] COMMAND_PARAMETER_STORAGE =
        new String[] {
            NOT_SUPPORTED, "ReadGlobalDataUnit", "WriteGlobalDataUnit", "ReadGlobalDataSet", "WriteGlobalDataSet",
            "EraseGlobalDataSet"};

    /**
     * 
     */
    public static final String[] COMMAND_SECURITY =
        new String[] {
            NOT_SUPPORTED, "SetDomain", "GetDomain", "GetProperties", "SetProperties", "BindProperties",
            "InitArbTable", "WriteRpmbKey"};

    /**
     * 
     */
    public static final String[] COMMAND_RESET = new String[] {
        NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED}; // "InitRequest", "UpdateRequest"

    private static final String[] BULK_COMMAND_NAMES =
        new String[] {
            "systemExecuteSoftware", "flashProcessFile", "flashDumpArea", "fileSystemCopyFile",
            "parameterStorageReadGlobalDataSet", "parameterStorageWriteGlobalDataSet", "securityStoreSecurityObject"};

    /** */

    public static final ArrayList<String> BULK_COMMANDS = new ArrayList<String>(Arrays.asList(BULK_COMMAND_NAMES));

    private static final String[] LENGTH_IN_BITS_COMMAND_ID = new String[] {
        "WriteBits"};

    /** */
    public static final ArrayList<String> LENGTH_IN_BITS_COMMANDS =
        new ArrayList<String>(Arrays.asList(LENGTH_IN_BITS_COMMAND_ID));

    /** */
    public static final int BULK_SOURCE_SOURCE = 1;

    /** */
    public static final int BULK_SOURCE_DESTINATION = 2;

    /** */
    public static final int BULK_SOURCE_BOTH = 3;

    /** */

    public static final int BULK_SOURCE_NONE = 4;

    private static final String UNKNOWN = "<Unknown Command Name>";

    /**
     * if general response timeout should be used with this command
     * 
     * @param cmd
     *            the command
     * @return true if timeout should be used, false otherwise
     */
    public static boolean isGeneralResponseTimeoutEnabled(String cmd) {

        if (cmd.equalsIgnoreCase(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES)) {
            return true;
        }
        return false;
    }

    // /**
    // * returns the source path for a command, this can be source, destination, both or none
    // *
    // * @param group
    // * the group number
    // * @param cmd
    // * the command number
    // * @return an bulkSource enum
    // */
    // public static int getBulkIDSource(int group, int cmd) {
    // if (BULK_COMMANDS.contains(getMethodString(group, cmd))) {
    // if (getMethodString(group, cmd).equals(LCDefinitions.METHOD_NAME_SYSTEM_EXECUTE_SOFTWARE)) {
    // return BULK_SOURCE_SOURCE;
    // } else if (getMethodString(group, cmd).equals(LCDefinitions.METHOD_NAME_FLASH_PROCESS_FILE))
    // {
    // return BULK_SOURCE_SOURCE;
    // } else if (getMethodString(group, cmd).equals(LCDefinitions.METHOD_NAME_FLASH_DUMP_AREA)) {
    // return BULK_SOURCE_DESTINATION;
    // } else if (getMethodString(group, cmd).equals(
    // LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_SET)) {
    // return BULK_SOURCE_DESTINATION;
    // } else if (getMethodString(group, cmd).equals(
    // LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_SET)) {
    // return BULK_SOURCE_SOURCE;
    // } else {
    // return BULK_SOURCE_NONE;
    // }
    // } else {
    // return BULK_SOURCE_NONE;
    // }
    //
    // }

    /**
     * checks if the command is a bulk command
     * 
     * @param cmd
     *            the command string
     * @return true if the command uses bulk false otherwise
     */
    public static boolean isBulkCommand(String cmd) {
        return BULK_COMMANDS.contains(cmd);
    }

    /**
     * get the method string from cmd and grp number
     * 
     * @param group
     *            group number
     * @param cmd
     *            command number
     * @return the command string
     */
    public static String getCommandString(int group, int cmd) {

        try {
            switch (group) {
                case LCDefinitions.COMMAND_GROUP_SYSTEM:
                    return COMMAND_SYSTEM[cmd];
                case LCDefinitions.COMMAND_GROUP_FLASH:
                    return COMMAND_FLASH[cmd];
                case LCDefinitions.COMMAND_GROUP_FILE_SYSTEM:
                    return COMMAND_FILE_SYSTEM[cmd];
                case LCDefinitions.COMMAND_GROUP_OTP:
                    return COMMAND_OTP[cmd];
                case LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE:
                    return COMMAND_PARAMETER_STORAGE[cmd];
                case LCDefinitions.COMMAND_GROUP_SECURITY:
                    return COMMAND_SECURITY[cmd];
                case LCDefinitions.COMMAND_GROUP_RESET:
                    return COMMAND_RESET[cmd];
                default:
                    return NOT_SUPPORTED;
            }
        } catch (ArrayIndexOutOfBoundsException e) {
            return UNKNOWN;
        }

    }

    /**
     * Returns the method string from cmd and grp number
     * 
     * @param group
     *            group number
     * @param cmd
     *            command number
     * @return the method string
     */
    public static String getMethodString(int group, int cmd) {

        String command = null;

        switch (group) {

            case LCDefinitions.COMMAND_GROUP_SYSTEM:
                command = COMMAND_SYSTEM[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_FLASH:
                command = COMMAND_FLASH[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_FILE_SYSTEM:
                command = COMMAND_FILE_SYSTEM[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_OTP:
                command = COMMAND_OTP[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE:
                command = COMMAND_PARAMETER_STORAGE[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_SECURITY:
                command = COMMAND_SECURITY[cmd];
                break;
            case LCDefinitions.COMMAND_GROUP_RESET:
                command = COMMAND_RESET[cmd];
                break;
            default:
                return NOT_SUPPORTED;
        }

        return GROUP_ID[group] + command;
    }

    /**
     * returns a html encoded string of the command desription.
     * 
     * @param group
     *            group number
     * @param cmd
     *            command number
     * @return description
     */
    public static String getDescription(int group, int cmd) {

        switch (group) {

            case LCDefinitions.COMMAND_GROUP_SYSTEM:
                return getSystemDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_FLASH:
                return getFlashDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_FILE_SYSTEM:
                return getFileSystemDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_OTP:
                return getOTPDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE:
                return getParamterStorageDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_SECURITY:
                return getSecurityDesc(cmd);

            case LCDefinitions.COMMAND_GROUP_RESET:
                return getResetDesc(cmd);

            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getOTPDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_OTP_READ_BITS:
                return "Reads the specified bits from the OTP";
            case LCDefinitions.COMMAND_OTP_SET_BITS:
                return "Set the OTP bit array";
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getFileSystemDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_FS_VOLUME_PROPERTIES:
                return "Retrieve properties of the specified file system volume";
            case LCDefinitions.COMMAND_FS_FORMAT_VOLUME:
                return "Formats an unmounted file system volume. This operation fails if the volume is currently in use.";
            case LCDefinitions.COMMAND_FS_LIST_DIRECTORY:
                return "List files and directories residing in a specified path";
            case LCDefinitions.COMMAND_FS_MOVE_FILE:
                return "Moves or renames a file.";
            case LCDefinitions.COMMAND_FS_DELETE_FILE:
                return "Deletes the specified file or directory. The Loader will only delete empty directories.";
            case LCDefinitions.COMMAND_FS_COPY_FILE:
                return "<html>Copies a file from the PC to the ME, between two directories or filesystems on <br>"
                    + "the ME or from the ME to the PC.";
            case LCDefinitions.COMMAND_FS_CREATE_DIRECTORY:
                return "Creates a directory";
            case LCDefinitions.COMMAND_FS_PROPERTIES:
                return "Retrieves the properties of a file or directory";
            case LCDefinitions.COMMAND_FS_CHANGE_ACCESS:
                return "Changes the access permissions of a path";
            case LCDefinitions.COMMAND_FS_READ_LOAD_MODULE_MANIFESTS:
                return "Read load module manifests";
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getSystemDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_SYSTEM_LOADER_STARTUP_STATUS:
                return "<html>The Loader Start-up Status command is sent by the ME to notify the host that it has started. <br>"
                    + "The Status parameter indicates in what mode the Loader started.";
            case LCDefinitions.COMMAND_SYSTEM_CHANGE_BAUDRATE:
                return "Change the baud rate (only applicable to UART connections)";
            case LCDefinitions.COMMAND_SYSTEM_REBOOT:
                return "<html>The Reboot command is used to instruct the Loader to reset the ME. Upon receiving this command,<br>"
                    + " the Loader shuts down in a controlled fashion and restarts the ME. The Mode parameter is used to <br>"
                    + "select the mode of reset.";
            case LCDefinitions.COMMAND_SYSTEM_SHUTDOWN:
                return "The Loader shuts down in a controlled fashion and proceeds to shut down the ME itself.";
            case LCDefinitions.COMMAND_SYSTEM_SUPPORTED_COMMANDS:
                return "<html>The Loader returns a list of implemented commands and whether they are permitted to <br>"
                    + "execute in the current Loader state. Note that further fine-grained permission controls might <br>"
                    + "deny execution of a specific command anyway.";
            case LCDefinitions.COMMAND_SYSTEM_EXECUTE_SOFTWARE:
                return "<html>Receive, verify and execute software, which can be a signed Loader. After having sent this <br>"
                    + "command, the ME will attempt to read the software payload data from the host using the Bulk <br>"
                    + "protocol or from the flash file system depending on the selected path.";
            case LCDefinitions.COMMAND_SYSTEM_AUTHENTICATE:
                return "<html>This command is used to escalate the privileges of the operator. Two modes of authentication<br>"
                    + " are available by default; Control Key authentication and Certificate based authentication.<br>"
                    + " The authentication command sets the loader in a specific authentication context when it takes<br>"
                    + " control over the command flow. After receiving the authentication command, the Loader will send<br>"
                    + " the appropriate request for information to the PC.";
            case LCDefinitions.COMMAND_SYSTEM_GET_CONTROL_KEYS:
                return "<html>This command is used by the Loader to retrieve the SimLock Control Keys from the host in <br>"
                    + "order to authenticate a user. This command is used in authentication context.";
            case LCDefinitions.COMMAND_SYSTEM_AUTHENTICATION_CHALLENGE:
                return "<html>This command is used by the Loader to perform a certificate authentication. This command<br>"
                    + " is only used in authentication context.";
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getFlashDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_FLASH_PROCESS_FILE:
                return "<html>This command is used to initiate a flashing session. The Type argument is used to select<br>"
                    + " the type of file to process.";
            case LCDefinitions.COMMAND_FLASH_LIST_DEVICES:
                return "<html>The Loader returns a list of detected block devices. A block device can be a physical device<br>"
                    + " ('flash0', 'mmc0', 'usb0'), a logical device ('cabs0', 'mbbs0') or a file system volume <br>"
                    + "('boot', 'sys'). Together they form paths on the form  '/flash0/mbbs0' or '/flash1/cabs1/vfat'.";
            case LCDefinitions.COMMAND_FLASH_DUMP_AREA:
                return "This command is used to initiate a dumping session";
            case LCDefinitions.COMMAND_FLASH_ERASE_AREA:
                return "This command erases the specified area of a device";
            case LCDefinitions.COMMAND_FLASH_SET_ENHANCED_AREA:
                return "This command sets specified area as Enhanced";
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getSecurityDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_SECURITY_SET_DOMAIN:
                return "Set the ME domain";
            case LCDefinitions.COMMAND_SECURITY_GET_DOMAIN:
                return "Get the ME domain";
            case LCDefinitions.COMMAND_SECURITY_GET_PROPERTY:
                return "Reads a security data unit (Such as a secure static or dynamic data unit)";
            case LCDefinitions.COMMAND_SECURITY_SET_PROPERTY:
                return "Writes a security data unit (Such as a secure static or dynamic data unit)";
            case LCDefinitions.COMMAND_SECURITY_BIND_PROPERTIES:
                return "Associates all security data units with this ME.";
            case LCDefinitions.COMMAND_SECURITY_WRITE_RPMB_KEY:
                return "Store authentication key in RPMB.";
                // case LCDefinitions.COMMAND_SECURITY_STORE_SECURE_OBJECT:
                // return NOT_SUPPORTED;
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getParamterStorageDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_PS_READ_GLOBAL_DATA_UNIT:
                return "Reads the specified unit from GDFS";
            case LCDefinitions.COMMAND_PS_WRITE_GLOBAL_DATA_UNIT:
                return "Writes the specified unit to GDFS";
            case LCDefinitions.COMMAND_PS_READ_GLOBAL_DATA_SET:
                return "Reads a complete Global Data area";
            case LCDefinitions.COMMAND_PS_WRITE_GLOBAL_DATA_SET:
                return "Writes a complete Global Data area";
            case LCDefinitions.COMMAND_PS_ERASE_GLOBAL_DATA_SET:
                return "Erases a complete Global Data area";
            default:
                return NOT_SUPPORTED;
        }
    }

    private static String getResetDesc(int cmd) {
        switch (cmd) {
            case LCDefinitions.COMMAND_RESET_INIT_REQUEST:
                return "Initiate the update procedure of changing IMEI, domain or to recover static data";
            case LCDefinitions.COMMAND_RESET_UPDATE_REQUEST:
                return "Update IMEI, domain or recover static data";
            default:
                return NOT_SUPPORTED;
        }
    }
}
