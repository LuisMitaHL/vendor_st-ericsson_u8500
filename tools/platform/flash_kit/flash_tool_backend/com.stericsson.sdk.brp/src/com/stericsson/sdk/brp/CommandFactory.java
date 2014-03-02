package com.stericsson.sdk.brp;

import java.util.HashMap;

import com.stericsson.sdk.brp.backend.DisableAutoInitialize;
import com.stericsson.sdk.brp.backend.DisableAutoSense;
import com.stericsson.sdk.brp.backend.EnableAutoInitialize;
import com.stericsson.sdk.brp.backend.EnableAutoSense;
import com.stericsson.sdk.brp.backend.GetActiveProfile;
import com.stericsson.sdk.brp.backend.GetAutoInitializeStatus;
import com.stericsson.sdk.brp.backend.GetAutoSenseStatus;
import com.stericsson.sdk.brp.backend.GetAvailableProfiles;
import com.stericsson.sdk.brp.backend.GetAvailableSecurityProperties;
import com.stericsson.sdk.brp.backend.GetConnectedEquipments;
import com.stericsson.sdk.brp.backend.GetEquipmentProfile;
import com.stericsson.sdk.brp.backend.GetNextConnectedEquipment;
import com.stericsson.sdk.brp.backend.ReloadConfiguration;
import com.stericsson.sdk.brp.backend.SetActiveProfile;
import com.stericsson.sdk.brp.backend.SetEquipmentProfile;
import com.stericsson.sdk.brp.backend.SetLocalSigning;
import com.stericsson.sdk.brp.backend.ShutdownBackend;
import com.stericsson.sdk.brp.backend.TriggerUARTPort;
import com.stericsson.sdk.brp.backend.GetDeviceInfo;
import com.stericsson.sdk.brp.coredump.DeleteDump;
import com.stericsson.sdk.brp.coredump.DownloadDump;
import com.stericsson.sdk.brp.coredump.GetAutoDeleteStatus;
import com.stericsson.sdk.brp.coredump.GetAutoDownload;
import com.stericsson.sdk.brp.coredump.GetCoreDumpLocation;
import com.stericsson.sdk.brp.coredump.GetSubscription;
import com.stericsson.sdk.brp.coredump.ListFile;
import com.stericsson.sdk.brp.coredump.SetAutoDelete;
import com.stericsson.sdk.brp.coredump.SetAutoDownload;
import com.stericsson.sdk.brp.coredump.SetCoreDumpLocation;
import com.stericsson.sdk.brp.coredump.SetSubscription;
import com.stericsson.sdk.brp.filesystem.ChangeAccess;
import com.stericsson.sdk.brp.filesystem.CheckCompatibility;
import com.stericsson.sdk.brp.filesystem.CopyFile;
import com.stericsson.sdk.brp.filesystem.CreateDirectory;
import com.stericsson.sdk.brp.filesystem.DeleteFile;
import com.stericsson.sdk.brp.filesystem.FormatVolume;
import com.stericsson.sdk.brp.filesystem.ListDirectory;
import com.stericsson.sdk.brp.filesystem.ListModules;
import com.stericsson.sdk.brp.filesystem.MoveFile;
import com.stericsson.sdk.brp.filesystem.Properties;
import com.stericsson.sdk.brp.filesystem.VolumeProperties;
import com.stericsson.sdk.brp.flash.DumpArea;
import com.stericsson.sdk.brp.flash.EraseArea;
import com.stericsson.sdk.brp.flash.ListDevices;
import com.stericsson.sdk.brp.flash.ProcessFile;
import com.stericsson.sdk.brp.flash.SelectLoaderOptions;
import com.stericsson.sdk.brp.flash.SetEnhancedArea;
import com.stericsson.sdk.brp.otp.BurnOTP;
import com.stericsson.sdk.brp.otp.BurnOTPData;
import com.stericsson.sdk.brp.otp.ReadOTP;
import com.stericsson.sdk.brp.otp.ReadOTPData;
import com.stericsson.sdk.brp.parameterstorage.EraseGlobalDataSet;
import com.stericsson.sdk.brp.parameterstorage.ReadGlobalDataSet;
import com.stericsson.sdk.brp.parameterstorage.ReadGlobalDataUnit;
import com.stericsson.sdk.brp.parameterstorage.WriteGlobalDataSet;
import com.stericsson.sdk.brp.parameterstorage.WriteGlobalDataUnit;
import com.stericsson.sdk.brp.security.BindProperties;
import com.stericsson.sdk.brp.security.GetDomain;
import com.stericsson.sdk.brp.security.GetEquipmentProperties;
import com.stericsson.sdk.brp.security.GetEquipmentProperty;
import com.stericsson.sdk.brp.security.SetDomain;
import com.stericsson.sdk.brp.security.SetEquipmentProperty;
import com.stericsson.sdk.brp.security.WriteRpmbKey;
import com.stericsson.sdk.brp.system.AuthenticateCertificate;
import com.stericsson.sdk.brp.system.AuthenticateControlKeys;
import com.stericsson.sdk.brp.system.CollectData;
import com.stericsson.sdk.brp.system.Deauthenticate;
import com.stericsson.sdk.brp.system.ExecuteSoftware;
import com.stericsson.sdk.brp.system.GetAuthenticationState;
import com.stericsson.sdk.brp.system.InitArbTable;
import com.stericsson.sdk.brp.system.InitializeEquipment;
import com.stericsson.sdk.brp.system.PermanentAuthentication;
import com.stericsson.sdk.brp.system.RebootEquipment;
import com.stericsson.sdk.brp.system.SendRawData;
import com.stericsson.sdk.brp.system.ShutdownEquipment;

/**
 * Factory for creating command objects
 * 
 * @author xolabju
 * 
 */
public final class CommandFactory {

    private CommandFactory() {
    }

    private static HashMap<String, Class<? extends AbstractCommand>> availableCommands =
        new HashMap<String, Class<? extends AbstractCommand>>();

    static {
        // backend commands
        availableCommands.put(CommandName.BACKEND_DISABLE_AUTO_INITIALIZE.name(), DisableAutoInitialize.class);
        availableCommands.put(CommandName.BACKEND_DISABLE_AUTO_SENSE.name(), DisableAutoSense.class);
        availableCommands.put(CommandName.BACKEND_ENABLE_AUTO_INITIALIZE.name(), EnableAutoInitialize.class);
        availableCommands.put(CommandName.BACKEND_ENABLE_AUTO_SENSE.name(), EnableAutoSense.class);
        availableCommands.put(CommandName.BACKEND_GET_ACTIVE_PROFILE.name(), GetActiveProfile.class);
        availableCommands.put(CommandName.BACKEND_GET_AUTO_INITIALIZE_STATUS.name(), GetAutoInitializeStatus.class);
        availableCommands.put(CommandName.BACKEND_GET_AUTO_SENSE_STATUS.name(), GetAutoSenseStatus.class);
        availableCommands.put(CommandName.BACKEND_GET_AVAILABLE_PROFILES.name(), GetAvailableProfiles.class);
        availableCommands.put(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name(), GetConnectedEquipments.class);
        availableCommands.put(CommandName.BACKEND_GET_CORE_DUMP_LOCATION.name(), GetCoreDumpLocation.class);
        availableCommands.put(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT.name(), GetNextConnectedEquipment.class);
        availableCommands.put(CommandName.BACKEND_RELOAD_CONFIGURATION.name(), ReloadConfiguration.class);
        availableCommands.put(CommandName.BACKEND_SET_ACTIVE_PROFILE.name(), SetActiveProfile.class);
        availableCommands.put(CommandName.BACKEND_SET_CORE_DUMP_LOCATION.name(), SetCoreDumpLocation.class);
        availableCommands.put(CommandName.BACKEND_GET_EQUIPMENT_PROFILE.name(), GetEquipmentProfile.class);
        availableCommands.put(CommandName.BACKEND_SET_EQUIPMENT_PROFILE.name(), SetEquipmentProfile.class);
        availableCommands.put(CommandName.BACKEND_SHUTDOWN_BACKEND.name(), ShutdownBackend.class);
        availableCommands.put(CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name(),
            GetAvailableSecurityProperties.class);
        availableCommands.put(CommandName.BACKEND_SET_LOCAL_SIGNING.name(), SetLocalSigning.class);
        availableCommands.put(CommandName.BACKEND_TRIGGER_UART_PORT.name(), TriggerUARTPort.class);
        availableCommands.put(CommandName.BACKEND_GET_DEVICE_INFO.name(), GetDeviceInfo.class);

        // core dump commands
        availableCommands.put(CommandName.COREDUMP_LIST_FILE.name(), ListFile.class);
        availableCommands.put(CommandName.COREDUMP_DOWNLOAD_DUMP.name(), DownloadDump.class);
        availableCommands.put(CommandName.COREDUMP_DELETE_DUMP.name(), DeleteDump.class);
        availableCommands.put(CommandName.COREDUMP_SET_SUBSCRIPTION.name(), SetSubscription.class);
        availableCommands.put(CommandName.COREDUMP_GET_SUBSCRIPTION.name(), GetSubscription.class);
        availableCommands.put(CommandName.COREDUMP_SET_AUTO_DOWNLOAD.name(), SetAutoDownload.class);
        availableCommands.put(CommandName.COREDUMP_GET_AUTO_DOWNLOAD.name(), GetAutoDownload.class);
        availableCommands.put(CommandName.COREDUMP_SET_AUTO_DELETE.name(), SetAutoDelete.class);
        availableCommands.put(CommandName.COREDUMP_GET_AUTO_DELETE_STATUS.name(), GetAutoDeleteStatus.class);

        // equipment storage commands
        availableCommands.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(), WriteGlobalDataSet.class);
        availableCommands.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name(), ReadGlobalDataSet.class);
        availableCommands.put(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name(), EraseGlobalDataSet.class);
        availableCommands.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), WriteGlobalDataUnit.class);
        availableCommands.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name(), ReadGlobalDataUnit.class);

        // flash commands
        availableCommands.put(CommandName.FLASH_PROCESS_FILE.name(), ProcessFile.class);
        availableCommands.put(CommandName.FLASH_DUMP_AREA.name(), DumpArea.class);
        availableCommands.put(CommandName.FLASH_ERASE_AREA.name(), EraseArea.class);
        availableCommands.put(CommandName.FLASH_LIST_DEVICES.name(), ListDevices.class);
        availableCommands.put(CommandName.FLASH_SET_ENHANCED_AREA.name(), SetEnhancedArea.class);
        availableCommands.put(CommandName.FLASH_SELECT_LOADER_OPTIONS.name(), SelectLoaderOptions.class);

        // security commands
        availableCommands.put(CommandName.SECURITY_SET_DOMAIN.name(), SetDomain.class);
        availableCommands.put(CommandName.SECURITY_GET_DOMAIN.name(), GetDomain.class);
        availableCommands.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), GetEquipmentProperty.class);
        availableCommands.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name(), GetEquipmentProperties.class);

        availableCommands.put(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), SetEquipmentProperty.class);
        availableCommands.put(CommandName.SECURITY_BIND_PROPERTIES.name(), BindProperties.class);
        availableCommands.put(CommandName.SECURITY_WRITE_RPMB_KEY.name(), WriteRpmbKey.class);

        // system commands
        availableCommands.put(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), AuthenticateCertificate.class);
        availableCommands.put(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name(), AuthenticateControlKeys.class);
        availableCommands.put(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name(), PermanentAuthentication.class);
        availableCommands.put(CommandName.SYSTEM_GET_AUTHENTICATION_STATE.name(), GetAuthenticationState.class);
        availableCommands.put(CommandName.SYSTEM_DEAUTHENTICATE.name(), Deauthenticate.class);
        availableCommands.put(CommandName.SYSTEM_EXECUTE_SOFTWARE.name(), ExecuteSoftware.class);
        availableCommands.put(CommandName.SYSTEM_INITIALIZE_EQUIPMENT.name(), InitializeEquipment.class);
        availableCommands.put(CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), RebootEquipment.class);
        availableCommands.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), ShutdownEquipment.class);
        availableCommands.put(CommandName.SYSTEM_SEND_RAW_DATA.name(), SendRawData.class);
        availableCommands.put(CommandName.SYSTEM_COLLECT_DATA.name(), CollectData.class);
        availableCommands.put(CommandName.SECURITY_INIT_ARB_TABLE.name(), InitArbTable.class);

        // file system commands
        availableCommands.put(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name(), VolumeProperties.class);
        availableCommands.put(CommandName.FILE_SYSTEM_FORMAT_VOLUME.name(), FormatVolume.class);
        availableCommands.put(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name(), ListDirectory.class);
        availableCommands.put(CommandName.FILE_SYSTEM_MOVE_FILE.name(), MoveFile.class);
        availableCommands.put(CommandName.FILE_SYSTEM_DELETE_FILE.name(), DeleteFile.class);
        availableCommands.put(CommandName.FILE_SYSTEM_COPY_FILE.name(), CopyFile.class);
        availableCommands.put(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name(), CreateDirectory.class);
        availableCommands.put(CommandName.FILE_SYSTEM_PROPERTIES.name(), Properties.class);
        availableCommands.put(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name(), ChangeAccess.class);
        availableCommands.put(CommandName.FILE_SYSTEM_LIST_MODULES.name(), ListModules.class);
        availableCommands.put(CommandName.FILE_SYSTEM_CHECK_COMPATIBILITY.name(), CheckCompatibility.class);

        // OTP commands
        availableCommands.put(CommandName.OTP_BURN_OTP.name(), BurnOTP.class);
        availableCommands.put(CommandName.OTP_BURN_OTP_DATA.name(), BurnOTPData.class);
        availableCommands.put(CommandName.OTP_READ_OTP.name(), ReadOTP.class);
        availableCommands.put(CommandName.OTP_READ_OTP_DATA.name(), ReadOTPData.class);
    }

    /**
     * Creates a new command object
     * 
     * @param completeCommand
     *            the complete command string including command name, delimiters and parameters
     * @see {@link CommandName}
     * @return a command object, or null if the command name does not have a defined command class
     * @throws InvalidSyntaxException
     *             if the provided command syntax is invalid
     */
    public static AbstractCommand createCommand(String completeCommand) throws InvalidSyntaxException {
        AbstractCommand cmd = null;
        String cmdName = "";
        String[] split = completeCommand.split(AbstractCommand.DELIMITER);
        if (split != null) {
            if (split.length > 0 && split[0] != null) {
                cmdName = split[0].trim();
            }
        }
        Class<? extends AbstractCommand> clazz = availableCommands.get(cmdName);
        if (clazz != null) {
            try {
                cmd = clazz.newInstance();
            } catch (Exception e) {
                // ignore. should never happen
                e.printStackTrace();
            }
            if (cmd != null) {
                cmd.setCommand(completeCommand);
            }
        }
        return cmd;
    }

    /**
     * Creates a new command object
     * 
     * @param commandName
     *            the command name
     * @see {@link CommandName}
     * @param arguments
     *            the command arguments without any delimiters
     * @return a command object, or null if the command name does not have a defined command class
     * @throws InvalidSyntaxException
     *             if the provided command syntax is invalid
     */
    public static AbstractCommand createCommand(CommandName commandName, String[] arguments)
        throws InvalidSyntaxException {
        StringBuffer buffer = new StringBuffer();
        buffer.append(commandName.name());
        if (arguments != null && arguments.length > 0) {
            for (String arg : arguments) {
                buffer.append(AbstractCommand.DELIMITER + arg);
            }
        }
        return createCommand(buffer.toString());
    }

    /**
     * Creates a new empty command object
     * 
     * @param commandName
     *            the command name
     * @see {@link CommandName}
     * @return an empty command object, or null if the command name does not have a defined command
     *         class
     */
    public static AbstractCommand createCommand(CommandName commandName) {
        AbstractCommand cmd = null;
        Class<? extends AbstractCommand> clazz = availableCommands.get(commandName.name());
        if (clazz != null) {
            try {
                cmd = clazz.newInstance();
            } catch (Exception e) {
                // ignore. should never happen
                e.printStackTrace();
            }
        }
        return cmd;
    }

    /**
     * Creates a new cancel command object
     * 
     * @param completeCommand
     *            the complete command string including command name, delimiters and parameters
     * @see {@link CommandName}
     * @return a command object, or null if the command name does not have a defined command class
     * @throws InvalidSyntaxException
     *             if the provided command syntax is invalid
     */
    public static AbstractCommand createCancelCommand(String completeCommand) throws InvalidSyntaxException {
        if (completeCommand == null) {
            throw new InvalidSyntaxException("Command is null");
        }
        String commandAndCancel = completeCommand;
        if (commandAndCancel.endsWith(AbstractCommand.DELIMITER)) {
            commandAndCancel = commandAndCancel.substring(0, commandAndCancel.length() - 1);
        }
        commandAndCancel += AbstractCommand.DELIMITER + AbstractCommand.CANCEL;
        return createCommand(commandAndCancel);
    }

    /**
     * Creates a new cancel command object
     * 
     * @param commandName
     *            the command name
     * @see {@link CommandName}
     * @param arguments
     *            the command arguments without any delimiters
     * @return a command object, or null if the command name does not have a defined command class
     * @throws InvalidSyntaxException
     *             if the provided command syntax is invalid or if the command does not support
     *             cancel of the operation
     */
    public static AbstractCommand createCancelCommand(CommandName commandName, String[] arguments)
        throws InvalidSyntaxException {
        if (commandName == null || arguments == null) {
            throw new InvalidSyntaxException("Input is null");
        }
        String[] argsAndCancel = null;
        argsAndCancel = new String[arguments.length + 1];
        System.arraycopy(arguments, 0, argsAndCancel, 0, arguments.length);
        argsAndCancel[argsAndCancel.length - 1] = AbstractCommand.CANCEL;
        return createCommand(commandName, argsAndCancel);
    }

}
