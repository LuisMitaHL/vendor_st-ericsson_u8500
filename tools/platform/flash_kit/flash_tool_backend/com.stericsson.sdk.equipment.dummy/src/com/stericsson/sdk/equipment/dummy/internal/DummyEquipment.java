package com.stericsson.sdk.equipment.dummy.internal;

import java.util.HashMap;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipment;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IRuntimeExecutor;
import com.stericsson.sdk.equipment.RuntimeExecutorMock;
import com.stericsson.sdk.equipment.dummy.Activator;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentAuthenticateCertificateTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentAuthenticateControlKeysTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentBindPropertiesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentBootTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentCollectDataTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentDeleteDumpTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentDownloadDumpTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentDumpAreaTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentEraseAreaTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentEraseGlobalDataSetTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentGetPropertiesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentGetPropertyTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentListDevicesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentListFileTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentProcessFileTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentReadGlobalDataSetTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentReadGlobalDataUnitTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentSendRawDataTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentSetEnhancedAreaTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentSetPropertyTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentShutdownTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentWriteGlobalDataSetTask;
import com.stericsson.sdk.equipment.dummy.internal.task.DummyEquipmentWriteGlobalDataUnitTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentChangeAccessTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentCopyFileTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentCreateDirectoryTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentDeleteFileTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentFormatVolumeTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentListDitrectoryTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentListLoadModulesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentMoveFileTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentPropertiesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.filesystem.DummyEquipmentVolumePropertiesTask;
import com.stericsson.sdk.equipment.dummy.internal.task.otp.DummyEquipmentBurnOTPDataTask;
import com.stericsson.sdk.equipment.dummy.internal.task.otp.DummyEquipmentBurnOTPTask;
import com.stericsson.sdk.equipment.dummy.internal.task.otp.DummyEquipmentReadOTPDataTask;
import com.stericsson.sdk.equipment.dummy.internal.task.otp.DummyEquipmentReadOTPTask;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock;

/**
 * 
 * @author xolabju
 * 
 */
public class DummyEquipment extends AbstractEquipment implements ILoaderCommunicationListener {

    private static Logger logger = Logger.getLogger(DummyEquipment.class);

    /** */
    private HashMap<String, Class<?>> taskMap;

    private ILoaderCommunicationService loaderCommunicationService;

    /***/
    public static final int PROPERTY_ID = 0x01;

    /***/
    public static final String PROPERTY_NAME = "name";

    private IRuntimeExecutor runtimeExecutor;

    private DummyEquipmentModelUpdater modelUpdater;

    private byte[][] otp = new byte[46][8];

    // TODO for new document it had to be LSB=3 MSB=2
    private static final int OTP_LSB = 3;

    private static final int OTP_MSB = 2;

    /**
     * Constructor.
     * 
     * @param port
     *            Port
     * @param profile
     *            Profile
     */
    public DummyEquipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        setProperty(DummyPlatform.PLATFORM_PROPERTY, DummyPlatform.PLATFORM_PROPERTY, DummyPlatform.PLATFORM, false);
        setProperty(DummyPlatform.PORT_PROPERTY, DummyPlatform.PORT_PROPERTY, port.getPortName(), false);
        setProperty(DummyPlatform.PROP_PUBLIC_ID, DummyPlatform.PROP_PUBLIC_ID, DummyPlatform.PUBLICID, false);

        modelUpdater = new DummyEquipmentModelUpdater(this);

        taskMap = new HashMap<String, Class<?>>();

        taskMap.put(CommandName.BOOT, DummyEquipmentBootTask.class);

        // Initialize task map with system tasks
        taskMap.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), DummyEquipmentShutdownTask.class);
        taskMap.put(CommandName.SYSTEM_SEND_RAW_DATA.name(), DummyEquipmentSendRawDataTask.class);
        taskMap.put(CommandName.SYSTEM_COLLECT_DATA.name(), DummyEquipmentCollectDataTask.class);
        taskMap
            .put(CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), DummyEquipmentAuthenticateCertificateTask.class);
        taskMap.put(CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name(),
            DummyEquipmentAuthenticateControlKeysTask.class);

        // Initialize task map with flash tasks
        taskMap.put(CommandName.FLASH_PROCESS_FILE.name(), DummyEquipmentProcessFileTask.class);
        taskMap.put(CommandName.FLASH_DUMP_AREA.name(), DummyEquipmentDumpAreaTask.class);
        taskMap.put(CommandName.FLASH_ERASE_AREA.name(), DummyEquipmentEraseAreaTask.class);
        taskMap.put(CommandName.FLASH_LIST_DEVICES.name(), DummyEquipmentListDevicesTask.class);
        taskMap.put(CommandName.FLASH_SET_ENHANCED_AREA.name(), DummyEquipmentSetEnhancedAreaTask.class);

        // initialize with parameter storage tasks
        taskMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name(),
            DummyEquipmentReadGlobalDataUnitTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(),
            DummyEquipmentWriteGlobalDataUnitTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(),
            DummyEquipmentWriteGlobalDataSetTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name(),
            DummyEquipmentReadGlobalDataSetTask.class);
        taskMap.put(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name(),
            DummyEquipmentEraseGlobalDataSetTask.class);

        // initialize with security tasks
        taskMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name(), DummyEquipmentGetPropertiesTask.class);
        taskMap.put(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), DummyEquipmentGetPropertyTask.class);
        taskMap.put(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), DummyEquipmentSetPropertyTask.class);
        taskMap.put(CommandName.SECURITY_BIND_PROPERTIES.name(), DummyEquipmentBindPropertiesTask.class);

        taskMap.put(CommandName.COREDUMP_DOWNLOAD_DUMP.name(), DummyEquipmentDownloadDumpTask.class);
        taskMap.put(CommandName.COREDUMP_LIST_FILE.name(), DummyEquipmentListFileTask.class);
        taskMap.put(CommandName.COREDUMP_DELETE_DUMP.name(), DummyEquipmentDeleteDumpTask.class);

        taskMap.put(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name(), DummyEquipmentChangeAccessTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_COPY_FILE.name(), DummyEquipmentCopyFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name(), DummyEquipmentCreateDirectoryTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_DELETE_FILE.name(), DummyEquipmentDeleteFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_FORMAT_VOLUME.name(), DummyEquipmentFormatVolumeTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name(), DummyEquipmentListDitrectoryTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_MOVE_FILE.name(), DummyEquipmentMoveFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_PROPERTIES.name(), DummyEquipmentPropertiesTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name(), DummyEquipmentVolumePropertiesTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_LIST_MODULES.name(), DummyEquipmentListLoadModulesTask.class);

        taskMap.put(CommandName.OTP_BURN_OTP.name(), DummyEquipmentBurnOTPTask.class);
        taskMap.put(CommandName.OTP_BURN_OTP_DATA.name(), DummyEquipmentBurnOTPDataTask.class);
        taskMap.put(CommandName.OTP_READ_OTP.name(), DummyEquipmentReadOTPTask.class);
        taskMap.put(CommandName.OTP_READ_OTP_DATA.name(), DummyEquipmentReadOTPDataTask.class);

        otpInit();
        if (loaderCommunicationService == null) {
            ServiceReference serviceReference = null;
            try {
                serviceReference =
                    Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                        "(type=mock)")[0];

                runtimeExecutor = new RuntimeExecutorMock();
            } catch (InvalidSyntaxException e) {
                e.printStackTrace();
            }

            loaderCommunicationService =
                (ILoaderCommunicationService) Activator.getBundleContext().getService(serviceReference);
        }
    }

    /**
     * Initializes OTP data with row index values.
     */
    private void otpInit() {
        for (int i = 0; i < otp.length; i++) {
            otp[i][OTP_MSB] = (byte) (i >> 8);
            otp[i][OTP_LSB] = (byte) i;
        }
    }

    /**
     * Sets OTP row data.
     * 
     * @param data
     *            OTP row data (8 bytes).
     * @return true if setting row data is possible.
     */
    public boolean setOtp(byte[] data) {
        int index = getOtpIndex(data);
        byte[] otpdata = otp[index];
        // check overwrite ability
        for (int i = 0; i < 8; i++) {
            byte xor = (byte) (data[i] ^ otpdata[i]);
            // TODO add lock test here.
            if ((xor & otpdata[i]) != 0) {// want to switch from 1 to zero somewhere
                return false;
            }
        }
        System.arraycopy(data, 0, otpdata, 0, 8);
        return true;
    }

    /**
     * Returns OTP row data.
     * 
     * @param data
     *            OTP row data (8 bytes) to get index from.
     * @return OTP row data (8 bytes).
     */
    public byte[] getOtp(byte[] data) {
        byte[] result = new byte[8];
        System.arraycopy(otp[getOtpIndex(data)], 0, result, 0, 8);
        return result;
    }

    /**
     * Returns OTP row index.
     * 
     * @param data
     *            OTP row data (8 bytes) to get index from.
     * @return OTP row index.
     */
    private int getOtpIndex(byte[] data) {
        return data[OTP_LSB] | (data[OTP_MSB]<<8);
    }

    /**
     * @return get loader communication mock
     * @throws Exception
     *             if no loader communication mock was found
     */
    public ILoaderCommunicationMock getMock() throws Exception {
        if (loaderCommunicationService != null && loaderCommunicationService instanceof ILoaderCommunicationMock) {
            return (ILoaderCommunicationMock) loaderCommunicationService;
        } else {
            throw new Exception("New Loader Communication Mock registered!");
        }
    }

    /**
     * @return runtimeExecutor
     */
    public IRuntimeExecutor getRuntimeExecutor() {
        return runtimeExecutor;
    }

    /**
     * send a command
     * 
     * @param methodName
     *            the method of the command
     * @see LCDefinitions for commands
     * @param args
     *            arguments of the command
     * @param listener
     *            Interface to loader communication listener
     * @return result of the command
     */
    public Object sendCommand(String methodName, Object[] args, ILoaderCommunicationListener listener) {
        return loaderCommunicationService.sendCommand(1234, methodName, args, listener);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return DummyPlatform.PLATFORM + "@" + getPort().getPortName();
    }

    /**
     * {@inheritDoc}
     */
    public IEquipmentTask createTask(String[] arguments) {
        IEquipmentTask task = null;
        Class<?> clazz = taskMap.get(arguments[0]);
        if (clazz != null) {
            try {
                task = (IEquipmentTask) clazz.getConstructor(DummyEquipment.class).newInstance(this);
                task.setArguments(arguments);
            } catch (Exception e) {
                logger.error(e.getMessage());
            }
        } else {
            logger.error(arguments[0] + " not supported");
        }

        return task;
    }

    /**
     * {@inheritDoc}
     */
    public void updateModel(EquipmentModel model) {
        modelUpdater.updateModel(model);
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationError(IPort port, String message) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationMessage(String title, String message, boolean logOnly) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationProgress(long totalBytes, long transferredBytes) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public boolean isInWarmMode() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentType getEquipmentType() {
        return EquipmentType.DUMMY;
    }
}
