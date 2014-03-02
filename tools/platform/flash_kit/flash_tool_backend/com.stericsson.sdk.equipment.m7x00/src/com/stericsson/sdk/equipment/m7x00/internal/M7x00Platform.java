package com.stericsson.sdk.equipment.m7x00.internal;

import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractLCPlatform;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.port.PortUtilities;
import com.stericsson.sdk.equipment.m7x00.Activator;

/**
 * @author xtomlju
 */
public class M7x00Platform extends AbstractLCPlatform implements IEquipmentTaskListener {

    /** */
    public static final String PLATFORM = "m7x00";

    /** */
    private Logger logger = Logger.getLogger(M7x00Platform.class);

    /**
     * {@inheritDoc}
     */
    public void createEquipment(IPort port, IEquipmentProfile profile, boolean bootME) {

        // Always create and schedule a boot task before registering the equipment
        M7x00Equipment equipment = new M7x00Equipment(port, profile);
        // Register service and store service registration for later needs.

        ServiceRegistration registration =
            Activator.getBundleContext().registerService(IEquipment.class.getName(), equipment,
                equipment.getServiceProperties());
        try {
            registerEquipment(equipment, registration);

        } catch (Exception e) {
            registration.unregister();
            logger.error("Can not register equipment", e);
        }

        EquipmentState equipmentState = EquipmentState.IDLE;
        String equipmentStateMessage = "";

        IEquipmentTask bootTask = equipment.createTask(new String[] {
            CommandName.BOOT});
        bootTask.addTaskListener(this);
        IEquipmentTask loaderCommInit = equipment.createTask(new String[] {
            CommandName.LOADER_COMM_INIT});
        loaderCommInit.addTaskListener(this);
        IEquipmentTask changeBaudRate = equipment.createTask(new String[] {
            CommandName.CHANGE_BAUD_RATE});
        changeBaudRate.addTaskListener(this);

        try {
            equipment.setStatus(EquipmentState.BOOTING);


            if (bootME) {
                runTask(equipment, bootTask);
            }

            if (equipment.usesLoaderCommunication()) {
                runTask(equipment, loaderCommInit);
            } else {
                equipmentState = EquipmentState.LC_NOT_INITIALIZED;
                equipmentStateMessage = "RAM loading completed successfully. Loader communication is not initialized.";
            }

            if (bootME && !PortUtilities.isUSBPort(port) && !equipment.getChangeBaudRateROMAck()
                && equipment.usesLoaderCommunication()) {
                runTask(equipment, changeBaudRate);
            }

            equipment.setStatus(equipmentState, equipmentStateMessage);
            logger.info("Boot task finished without error.");

        } catch (Exception e) {
            String errorMessage = e.getMessage();
            try{
                errorMessage = checkIf64bitLcdLcmAreMissing(errorMessage);
            }catch(Exception ex){
                logger.error("ERROR while checking LCD/LCM drivers: " + ex.getMessage());
            }
            equipment.setStatus(EquipmentState.ERROR, errorMessage);
            logger.error("Boot task failed! Error message: " + e.getMessage());
        } finally {
            notifyNewEquipmentBooted(equipment);
        }
    }

    private void runTask(M7x00Equipment equipment, IEquipmentTask task) throws Exception {
        EquipmentTaskResult result = task.execute();
        if (result.getResultCode() != 0) {
            throw new Exception(result.getResultMessage());
        }
    }

    /**
     * Creates a dummy M7x00 equipment.
     */
    public void createDummyEquipment() {
        M7x00Equipment equipment = new M7x00Equipment(new AbstractPort("M7x00Port") {

            public int write(byte[] pBuffer, int pOffset, int pLength, int pTimeout) throws PortException {
                return 0;
            }

            public int read(byte[] pBuffer, int pOffset, int pLength, int pTimeout) throws PortException {
                return 0;
            }
        }, new IEquipmentProfile() {

            public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
                return new Hashtable<String, EquipmentProperty>();
            }

            public String getSofwarePath(String pType) {
                return null;
            }

            public String getProperty(String pKey) {
                return null;
            }

            public String getPlatformFilter() {
                return null;
            }

            public String getAlias() {
                return "M7x00_dummy_profile";
            }
        });

        ServiceRegistration registration =
            Activator.getBundleContext().registerService(IEquipment.class.getName(), equipment,
                equipment.getServiceProperties());
        try {
            registerEquipment(equipment, registration);
            equipment.setStatus(EquipmentState.IDLE);
        } catch (Exception e) {
            registration.unregister();
            logger.error("Can not register equipment", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {
        if (task.getEquipment() instanceof M7x00Equipment) {
            logger.info("Message from task '" + task + "': " + message);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task.getEquipment() instanceof M7x00Equipment) {
            logger.info("Task '" + task + "' started");
        }
    }
}
