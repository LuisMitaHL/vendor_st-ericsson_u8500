package com.stericsson.sdk.equipment.u5500.warm.internal;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractPlatform;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.u5500.warm.Activator;

/**
 * Creates and register a U5500 warm equipment.
 * 
 * @author esrimpa
 */
public class U5500WarmPlatform extends AbstractPlatform implements IEquipmentTaskListener {

    /**
     * Type of Platform
     */
    public static final String PLATFORM = "u5500";

    /**
     * State of equipment
     */
    public static final String STATE = "warm";

    /**
     * Logger instance
     */
    private Logger logger = Logger.getLogger(U5500WarmPlatform.class);

    /**
     * {@inheritDoc}
     */
    public void createEquipment(IPort port, IEquipmentProfile profile, boolean bootME) {

        // Always create and schedule a boot task before registering the equipment
        U5500WarmEquipment equipment = new U5500WarmEquipment(port, profile);

        // Register service and store service registration for later needs.
        ServiceRegistration registration =
            Activator.getBundleContext().registerService(IEquipment.class.getName(), equipment,
                equipment.getServiceProperties());
        IEquipmentTask bootTask = equipment.createTask(new String[] {CommandName.BOOT});
        if (bootTask == null) {
            throw new NullPointerException("Created task is null.");
        }
        bootTask.addTaskListener(this);
        try {
            registerEquipment(equipment, registration);
            EquipmentTaskResult result = bootTask.execute();
            if (result.getResultCode() != 0) {
                logger.error("Boot task failed! Error message: " + result.getResultMessage());
            } else {
                logger.info("Boot task finished without error.");
            }
        } catch (Exception e) {
            registration.unregister();
            logger.error("Can not register equipment", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {
        if (task.getEquipment() instanceof U5500WarmEquipment) {
            logger.info("Message from task '" + task + "': " + message);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
     // empty (there is nothing TO DO here)
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task.getEquipment() instanceof U5500WarmEquipment) {
            logger.info("Task '" + task + "' started");
        }
    }
}
