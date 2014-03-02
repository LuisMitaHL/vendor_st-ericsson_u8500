package com.stericsson.sdk.equipment.dummy.internal;

import org.apache.log4j.Logger;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractPlatform;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.dummy.Activator;
import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * 
 * @author xolabju
 * 
 */
public class DummyPlatform extends AbstractPlatform implements IEquipmentTaskListener {

    /** */
    public static final String PLATFORM = "Dummy";

    /** */
    public static final String PROP_PUBLIC_ID = "Public ID";

    /** */
    public static final String PUBLICID = "DummyPublicID";

    /** */
    private Logger logger = Logger.getLogger(DummyPlatform.class);

    /**
     * {@inheritDoc}
     */
    public void createEquipment(IPort port, IEquipmentProfile profile, boolean bootME) {

        // Always create and schedule a boot task before registering the equipment
        DummyEquipment equipment = new DummyEquipment(port, profile);

        // Register service and store service registration for later needs.
        ServiceRegistration registration =
            Activator.getBundleContext().registerService(IEquipment.class.getName(), equipment,
                equipment.getServiceProperties());
        IEquipmentTask bootTask = equipment.createTask(new String[] {CommandName.BOOT});
        if (bootTask == null) {
            throw new NullPointerException("Created task is null.");
        }
        try {
            registerEquipment(equipment, registration);
            bootTask.addTaskListener(this);
            bootTask.execute();
            logger.info("Registered dummy platform " + equipment.toString());
        } catch (Exception e) {
            registration.unregister();
            logger.error("Can not register equipment", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {
        if (task.getEquipment() instanceof DummyEquipment) {
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
        if (task.getEquipment() instanceof DummyEquipment) {
            logger.info("Task '" + task + "' started");
        }
    }
}
