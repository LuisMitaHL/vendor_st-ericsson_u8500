package com.stericsson.sdk.backend.remote.executor;

import java.util.HashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.backend.DisableAutoInitializeExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.DisableAutoSenseExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.EnableAutoInitializeExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.EnableAutoSenseExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetActiveProfileExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetAutoInitializeStatusExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetAutoSenseStatusExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetAvailableProfilesExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetAvailableSecurityPropertiesFromProfileExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetConnectedEquipmentsExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetEquipmentProfileExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetNextConnectedEquipmentExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.ReloadConfigurationExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.SetActiveProfileExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.SetEquipmentProfileExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.SetLocalSigningExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.ShutdownBackendExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.TriggerUARTPortExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.DeleteDumpExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.DownloadDumpExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.GetAutoDeleteStatusExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.GetAutoDownloadExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.GetCoreDumpLocationExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.GetSubscriptionExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.ListFileExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.SetAutoDeleteExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.SetAutoDownloadExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.SetCoreDumpLocationExecutor;
import com.stericsson.sdk.backend.remote.executor.coredump.SetSubscriptionExecutor;
import com.stericsson.sdk.backend.remote.executor.filesystem.FileSystemCommandExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.DumpAreaExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.EraseAreaExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.ListDevicesExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.ProcessFileExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.SelectLoaderOptionsExecutor;
import com.stericsson.sdk.backend.remote.executor.flash.SetEnhancedAreaExecutor;
import com.stericsson.sdk.backend.remote.executor.otp.OTPCommandExecutor;
import com.stericsson.sdk.backend.remote.executor.parameterstorage.EraseGlobalDataSetExecutor;
import com.stericsson.sdk.backend.remote.executor.parameterstorage.ReadGlobalDataSetExecutor;
import com.stericsson.sdk.backend.remote.executor.parameterstorage.ReadGlobalDataUnitExecutor;
import com.stericsson.sdk.backend.remote.executor.parameterstorage.WriteGlobalDataSetExecutor;
import com.stericsson.sdk.backend.remote.executor.parameterstorage.WriteGlobalDataUnitExecutor;
import com.stericsson.sdk.backend.remote.executor.security.BindPropertiesExecutor;
import com.stericsson.sdk.backend.remote.executor.security.GetDomainExecutor;
import com.stericsson.sdk.backend.remote.executor.security.GetEquipmentPropertiesExecutor;
import com.stericsson.sdk.backend.remote.executor.security.GetEquipmentPropertyExecutor;
import com.stericsson.sdk.backend.remote.executor.security.InitArbTableExecutor;
import com.stericsson.sdk.backend.remote.executor.security.SetDomainExecutor;
import com.stericsson.sdk.backend.remote.executor.security.SetEquipmentPropertyExecutor;
import com.stericsson.sdk.backend.remote.executor.security.WriteRpmbKeyExecutor;
import com.stericsson.sdk.backend.remote.executor.system.AuthenticateCertificateExecutor;
import com.stericsson.sdk.backend.remote.executor.system.AuthenticateControlKeysExecutor;
import com.stericsson.sdk.backend.remote.executor.system.CollectDataExecutor;
import com.stericsson.sdk.backend.remote.executor.system.DeauthenticateExecutor;
import com.stericsson.sdk.backend.remote.executor.system.ExecuteSoftwareExecutor;
import com.stericsson.sdk.backend.remote.executor.system.GetAuthenticationStatusExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.GetDeviceInfoExecutor;
import com.stericsson.sdk.backend.remote.executor.system.InitializeEquipmentExecutor;
import com.stericsson.sdk.backend.remote.executor.system.PermanentAuthenticationExecutor;
import com.stericsson.sdk.backend.remote.executor.system.RebootEquipmentExecutor;
import com.stericsson.sdk.backend.remote.executor.system.SendRawDataExecutor;
import com.stericsson.sdk.backend.remote.executor.system.ShutdownEquipmentExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ICommandExecutor;
import com.stericsson.sdk.brp.IExecutionHandler;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * Factory class for creating remote backend protocol server executors
 * 
 * @author xolabju
 * 
 */
public final class ServerCommandExecutorFactory {

    private static Logger logger = Logger.getLogger(ServerCommandExecutorFactory.class);

    private ServerCommandExecutorFactory() {
    }

    private static HashMap<CommandName, Class<? extends ServerCommandExecutor>> executors =
        new HashMap<CommandName, Class<? extends ServerCommandExecutor>>();
    static {
        // backend commands
        executors.put(CommandName.BACKEND_DISABLE_AUTO_INITIALIZE, DisableAutoInitializeExecutor.class);
        executors.put(CommandName.BACKEND_DISABLE_AUTO_SENSE, DisableAutoSenseExecutor.class);
        executors.put(CommandName.BACKEND_ENABLE_AUTO_INITIALIZE, EnableAutoInitializeExecutor.class);
        executors.put(CommandName.BACKEND_ENABLE_AUTO_SENSE, EnableAutoSenseExecutor.class);
        executors.put(CommandName.BACKEND_GET_ACTIVE_PROFILE, GetActiveProfileExecutor.class);
        executors.put(CommandName.BACKEND_GET_AUTO_INITIALIZE_STATUS, GetAutoInitializeStatusExecutor.class);
        executors.put(CommandName.BACKEND_GET_AUTO_SENSE_STATUS, GetAutoSenseStatusExecutor.class);
        executors.put(CommandName.BACKEND_GET_AVAILABLE_PROFILES, GetAvailableProfilesExecutor.class);
        executors.put(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS, GetConnectedEquipmentsExecutor.class);
        executors.put(CommandName.BACKEND_GET_CORE_DUMP_LOCATION, GetCoreDumpLocationExecutor.class);
        executors.put(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT, GetNextConnectedEquipmentExecutor.class);
        executors.put(CommandName.BACKEND_RELOAD_CONFIGURATION, ReloadConfigurationExecutor.class);
        executors.put(CommandName.BACKEND_SET_ACTIVE_PROFILE, SetActiveProfileExecutor.class);
        executors.put(CommandName.BACKEND_SET_CORE_DUMP_LOCATION, SetCoreDumpLocationExecutor.class);
        executors.put(CommandName.BACKEND_SET_EQUIPMENT_PROFILE, SetEquipmentProfileExecutor.class);
        executors.put(CommandName.BACKEND_GET_EQUIPMENT_PROFILE, GetEquipmentProfileExecutor.class);
        executors.put(CommandName.BACKEND_SHUTDOWN_BACKEND, ShutdownBackendExecutor.class);
        executors.put(CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES,
            GetAvailableSecurityPropertiesFromProfileExecutor.class);
        executors.put(CommandName.BACKEND_SET_LOCAL_SIGNING, SetLocalSigningExecutor.class);
        executors.put(CommandName.BACKEND_TRIGGER_UART_PORT, TriggerUARTPortExecutor.class);
        executors.put(CommandName.BACKEND_GET_DEVICE_INFO, GetDeviceInfoExecutor.class);

        // Core dump
        executors.put(CommandName.COREDUMP_LIST_FILE, ListFileExecutor.class);
        executors.put(CommandName.COREDUMP_DOWNLOAD_DUMP, DownloadDumpExecutor.class);
        executors.put(CommandName.COREDUMP_SET_AUTO_DOWNLOAD, SetAutoDownloadExecutor.class);
        executors.put(CommandName.COREDUMP_GET_AUTO_DOWNLOAD, GetAutoDownloadExecutor.class);
        executors.put(CommandName.COREDUMP_SET_AUTO_DELETE, SetAutoDeleteExecutor.class);
        executors.put(CommandName.COREDUMP_GET_AUTO_DELETE_STATUS, GetAutoDeleteStatusExecutor.class);
        executors.put(CommandName.COREDUMP_DELETE_DUMP, DeleteDumpExecutor.class);
        executors.put(CommandName.COREDUMP_SET_SUBSCRIPTION, SetSubscriptionExecutor.class);
        executors.put(CommandName.COREDUMP_GET_SUBSCRIPTION, GetSubscriptionExecutor.class);

        // equipment storage commands
        executors.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET, WriteGlobalDataSetExecutor.class);
        executors.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET, ReadGlobalDataSetExecutor.class);
        executors.put(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET, EraseGlobalDataSetExecutor.class);
        executors.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT, WriteGlobalDataUnitExecutor.class);
        executors.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT, ReadGlobalDataUnitExecutor.class);
        // executors.put(CommandName.FLASH_DUMP_AREA, ReadGlobalDataSetExecutor.class);
        // executors.put(CommandName.FLASH_DUMP_AREA, WriteGlobalDataSetExecutor.class);
        // executors.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET,
        // WriteGDFSExecutor.class);
        // executors.put(CommandName.FLASH_DUMP_AREA, WriteGDFSExecutor.class);

        // flash commands
        executors.put(CommandName.FLASH_PROCESS_FILE, ProcessFileExecutor.class);
        executors.put(CommandName.FLASH_DUMP_AREA, DumpAreaExecutor.class);
        executors.put(CommandName.FLASH_ERASE_AREA, EraseAreaExecutor.class);
        executors.put(CommandName.FLASH_LIST_DEVICES, ListDevicesExecutor.class);
        executors.put(CommandName.FLASH_SET_ENHANCED_AREA, SetEnhancedAreaExecutor.class);
        executors.put(CommandName.FLASH_SELECT_LOADER_OPTIONS, SelectLoaderOptionsExecutor.class);
        // security commands
        executors.put(CommandName.SECURITY_SET_DOMAIN, SetDomainExecutor.class);
        executors.put(CommandName.SECURITY_GET_DOMAIN, GetDomainExecutor.class);
        executors.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES, GetEquipmentPropertiesExecutor.class);
        executors.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY, GetEquipmentPropertyExecutor.class);
        executors.put(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY, SetEquipmentPropertyExecutor.class);
        executors.put(CommandName.SECURITY_BIND_PROPERTIES, BindPropertiesExecutor.class);
        executors.put(CommandName.SECURITY_WRITE_RPMB_KEY, WriteRpmbKeyExecutor.class);

        // system commands
        executors.put(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE, AuthenticateCertificateExecutor.class);
        executors.put(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS, AuthenticateControlKeysExecutor.class);
        executors.put(CommandName.SYSTEM_EXECUTE_SOFTWARE, ExecuteSoftwareExecutor.class);
        executors.put(CommandName.SYSTEM_PERMANENT_AUTHENTICATION, PermanentAuthenticationExecutor.class);
        executors.put(CommandName.SYSTEM_INITIALIZE_EQUIPMENT, InitializeEquipmentExecutor.class);
        executors.put(CommandName.SYSTEM_REBOOT_EQUIPMENT, RebootEquipmentExecutor.class);
        executors.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT, ShutdownEquipmentExecutor.class);
        executors.put(CommandName.SYSTEM_SEND_RAW_DATA, SendRawDataExecutor.class);
        executors.put(CommandName.SYSTEM_COLLECT_DATA, CollectDataExecutor.class);

        executors.put(CommandName.SECURITY_INIT_ARB_TABLE, InitArbTableExecutor.class);

        executors.put(CommandName.SYSTEM_DEAUTHENTICATE, DeauthenticateExecutor.class);
        executors.put(CommandName.SYSTEM_GET_AUTHENTICATION_STATE, GetAuthenticationStatusExecutor.class);

        // file system commands
        executors.put(CommandName.FILE_SYSTEM_CHANGE_ACCESS, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_COPY_FILE, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_CREATE_DIRECTORY, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_DELETE_FILE, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_FORMAT_VOLUME, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_LIST_DIRECTORY, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_MOVE_FILE, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_PROPERTIES, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_LIST_MODULES, FileSystemCommandExecutor.class);
        executors.put(CommandName.FILE_SYSTEM_CHECK_COMPATIBILITY, FileSystemCommandExecutor.class);

        // OTP commands
        executors.put(CommandName.OTP_BURN_OTP, OTPCommandExecutor.class);
        executors.put(CommandName.OTP_BURN_OTP_DATA, OTPCommandExecutor.class);
        executors.put(CommandName.OTP_READ_OTP, OTPCommandExecutor.class);
        executors.put(CommandName.OTP_READ_OTP_DATA, OTPCommandExecutor.class);
    }

    /**
     * Creates a command executor for a specific command
     * 
     * @param completeCommand
     *            the complete command
     * @param handler
     *            the client handler to associate with the executor
     * @return a command executor for a specific command, or null if no executor was found
     * @throws InvalidSyntaxException
     *             if the provided command syntax is invalid
     */
    public static ICommandExecutor createCommandExecutor(String completeCommand, IExecutionHandler handler)
        throws InvalidSyntaxException {
        ICommandExecutor executor = null;
        AbstractCommand command = CommandFactory.createCommand(completeCommand);
        if (command != null) {
            Class<? extends ServerCommandExecutor> executorClass = executors.get(command.getCommandName());
            if (executorClass != null) {
                try {
                    executor = executorClass.newInstance();
                    executor.setCommand(command);
                    executor.setExecutionHandler(handler);
                } catch (InstantiationException e) {
                    logger.error(e.getMessage());
                } catch (IllegalAccessException e) {
                    logger.error(e.getMessage());
                }
            }
        }
        return executor;
    }

}
