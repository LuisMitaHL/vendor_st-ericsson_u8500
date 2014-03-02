package com.stericsson.sdk.equipment;

import java.util.HashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.tasks.EquipmentAuthenticateCertificateTask;
import com.stericsson.sdk.equipment.tasks.EquipmentAuthenticateControlKeysTask;
import com.stericsson.sdk.equipment.tasks.EquipmentBindPropertiesTask;
import com.stericsson.sdk.equipment.tasks.EquipmentBootTask;
import com.stericsson.sdk.equipment.tasks.EquipmentBootingDoneTask;
import com.stericsson.sdk.equipment.tasks.EquipmentBurnOTPDataTask;
import com.stericsson.sdk.equipment.tasks.EquipmentBurnOTPTask;
import com.stericsson.sdk.equipment.tasks.EquipmentChangeBaudRateTask;
import com.stericsson.sdk.equipment.tasks.EquipmentCollectDataTask;
import com.stericsson.sdk.equipment.tasks.EquipmentDeauthenticateTask;
import com.stericsson.sdk.equipment.tasks.EquipmentDumpAreaTask;
import com.stericsson.sdk.equipment.tasks.EquipmentEraseAreaTask;
import com.stericsson.sdk.equipment.tasks.EquipmentEraseGlobalDataSetTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetAuthenticationStateTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetDomainTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetPropertiesTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetPropertyTask;
import com.stericsson.sdk.equipment.tasks.EquipmentInitArbTableTask;
import com.stericsson.sdk.equipment.tasks.EquipmentListDevicesTask;
import com.stericsson.sdk.equipment.tasks.EquipmentLoaderCommunicatioInitTask;
import com.stericsson.sdk.equipment.tasks.EquipmentPermanentAuthenticationTask;
import com.stericsson.sdk.equipment.tasks.EquipmentProcessFileTask;
import com.stericsson.sdk.equipment.tasks.EquipmentReadGlobalDataSetTask;
import com.stericsson.sdk.equipment.tasks.EquipmentReadGlobalDataUnitTask;
import com.stericsson.sdk.equipment.tasks.EquipmentReadOTPDataTask;
import com.stericsson.sdk.equipment.tasks.EquipmentReadOTPTask;
import com.stericsson.sdk.equipment.tasks.EquipmentRebootTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSelectLoaderOptionsTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSendRawDataTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSetDomainTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSetPropertyTask;
import com.stericsson.sdk.equipment.tasks.EquipmentShutdownTask;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteGlobalDataSetTask;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteGlobalDataUnitTask;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteRpmbKeyTask;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;

/**
 * 
 * @author xmichroh
 * 
 */
public abstract class AbstractLoaderCommunicationEquipment extends AbstractEquipment {

    private static Logger logger = Logger.getLogger(AbstractLoaderCommunicationEquipment.class);

    /** */
    protected ILoaderCommunicationService loaderCommunicationService = null;

    /** */
    protected long instanceId;

    /** */
    protected HashMap<String, Class<?>> taskMap;

    private boolean changeBaudRateACK = false;

    /**
     * 
     * @param port
     *            the port object
     * @param profile
     *            the profile object
     */
    public AbstractLoaderCommunicationEquipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        taskMap = new HashMap<String, Class<?>>();

        taskMap.put(CommandName.BOOT, EquipmentBootTask.class);
        taskMap.put(CommandName.LOADER_COMM_INIT, EquipmentLoaderCommunicatioInitTask.class);
        taskMap.put(CommandName.CHANGE_BAUD_RATE, EquipmentChangeBaudRateTask.class);
        taskMap.put(CommandName.BOOTING_DONE, EquipmentBootingDoneTask.class);

        // Initialize task map with system tasks
        taskMap.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), EquipmentShutdownTask.class);
        taskMap.put(CommandName.SYSTEM_SEND_RAW_DATA.name(), EquipmentSendRawDataTask.class);
        taskMap.put(CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), EquipmentRebootTask.class);
        taskMap.put(CommandName.SYSTEM_COLLECT_DATA.name(), EquipmentCollectDataTask.class);
        taskMap.put(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name(), EquipmentPermanentAuthenticationTask.class);
        taskMap.put(CommandName.SECURITY_INIT_ARB_TABLE.name(), EquipmentInitArbTableTask.class);
        taskMap.put(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), EquipmentAuthenticateCertificateTask.class);
        taskMap.put(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name(), EquipmentAuthenticateControlKeysTask.class);
        taskMap.put(CommandName.SYSTEM_GET_AUTHENTICATION_STATE.name(), EquipmentGetAuthenticationStateTask.class);
        taskMap.put(CommandName.SYSTEM_DEAUTHENTICATE.name(), EquipmentDeauthenticateTask.class);

        // Initialize task map with flash tasks
        taskMap.put(CommandName.FLASH_PROCESS_FILE.name(), EquipmentProcessFileTask.class);
        taskMap.put(CommandName.FLASH_DUMP_AREA.name(), EquipmentDumpAreaTask.class);
        taskMap.put(CommandName.FLASH_ERASE_AREA.name(), EquipmentEraseAreaTask.class);
        taskMap.put(CommandName.FLASH_LIST_DEVICES.name(), EquipmentListDevicesTask.class);
        taskMap.put(CommandName.FLASH_SELECT_LOADER_OPTIONS.name(), EquipmentSelectLoaderOptionsTask.class);

        // Initialize task map with parameter storage tasks
        taskMap.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(), EquipmentWriteGlobalDataSetTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name(), EquipmentReadGlobalDataSetTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name(), EquipmentEraseGlobalDataSetTask.class);
        taskMap
            .put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), EquipmentWriteGlobalDataUnitTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name(), EquipmentReadGlobalDataUnitTask.class);

        // Initialize task map with security tasks
        taskMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name(), EquipmentGetPropertiesTask.class);
        taskMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), EquipmentGetPropertyTask.class);
        taskMap.put(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), EquipmentSetPropertyTask.class);
        taskMap.put(CommandName.SECURITY_BIND_PROPERTIES.name(), EquipmentBindPropertiesTask.class);
        taskMap.put(CommandName.SECURITY_GET_DOMAIN.name(), EquipmentGetDomainTask.class);
        taskMap.put(CommandName.SECURITY_SET_DOMAIN.name(), EquipmentSetDomainTask.class);
        taskMap.put(CommandName.SECURITY_WRITE_RPMB_KEY.name(), EquipmentWriteRpmbKeyTask.class);

        // Initialize task map with OTP tasks
        taskMap.put(CommandName.OTP_BURN_OTP.name(), EquipmentBurnOTPTask.class);
        taskMap.put(CommandName.OTP_BURN_OTP_DATA.name(), EquipmentBurnOTPDataTask.class);
        taskMap.put(CommandName.OTP_READ_OTP.name(), EquipmentReadOTPTask.class);
        taskMap.put(CommandName.OTP_READ_OTP_DATA.name(), EquipmentReadOTPDataTask.class);
    }

    /**
     * Create a LoaderCommunication instance.
     * 
     * @throws Exception .
     */
    public final void initLoaderCommunication() throws Exception {
        instanceId = loaderCommunicationService.initialize(getPort());
    }

    /**
     * Send a command.
     * 
     * @param methodName
     *            the method of the command {see LCDefinitions for commands}
     * @param args
     *            arguments of the command
     * 
     * @param listener
     *            Interface to loader communication listener
     * @return result of the command
     */
    public final Object sendCommand(String methodName, Object[] args, ILoaderCommunicationListener listener) {
        return loaderCommunicationService.sendCommand(instanceId, methodName, args, listener);
    }

    /**
     * Send raw data on the port.
     * 
     * @param rawData
     *            the byte array to send
     */
    public final void sendRawData(byte[] rawData) {
        loaderCommunicationService.sendRawData(instanceId, rawData);
    }

    /**
     * {@inheritDoc}
     */
    public IEquipmentTask createTask(String[] arguments) {
        IEquipmentTask task = null;
        Class<?> clazz = taskMap.get(arguments[0]);
        if (clazz != null) {
            try {
                task =
                    (IEquipmentTask) clazz.getConstructor(AbstractLoaderCommunicationEquipment.class).newInstance(this);
                task.setArguments(arguments);
            } catch (Exception e) {
                logger.error(e.getMessage());
                throw new RuntimeException(e);
            }
        } else {
            logger.error(arguments[0] + " not supported");
        }

        return task;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isInWarmMode() {
        return false;
    }

    /**
     * @return true if baud rate has been already changed in ROM code
     */
    public boolean getChangeBaudRateROMAck() {
        return changeBaudRateACK;
    }

    /**
     * @param changeBaudRate
     *            true if baud rate has been changed in ROM code
     */
    public void setChangeBaudRateROMAck(boolean changeBaudRate) {
        changeBaudRateACK = changeBaudRate;
    }

    /**
     * @return loaderCommunicationService
     */
    public ILoaderCommunicationService getLoaderCommunicationService() {
        return loaderCommunicationService;
    }
}
