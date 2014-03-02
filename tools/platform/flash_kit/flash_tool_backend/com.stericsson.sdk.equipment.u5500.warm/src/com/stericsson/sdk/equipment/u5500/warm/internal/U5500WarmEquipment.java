package com.stericsson.sdk.equipment.u5500.warm.internal;

import java.util.HashMap;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.u5500.warm.Activator;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentBootTask;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentDeleteDumpTask;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentDownloadDumpTask;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentListFileTask;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentShutdownTask;
import com.stericsson.sdk.ftp.communication.IFTPCommunicationService;

/**
 * Creates and schedule tasks for U500 type of equipment.
 *
 * @author esrimpa
 * 
 */
public class U5500WarmEquipment extends AbstractUX500WarmEquipment {

    /**
     * Logger instance
     */
    private static Logger logger = Logger.getLogger(U5500WarmEquipment.class);

    private HashMap<String, Class<?>> taskMap;

    /**
     * Constructor.
     *
     * @param port
     *            Equipment port
     * @param profile
     *            Profile
     */
    public U5500WarmEquipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        setProperty(U5500WarmPlatform.PLATFORM_PROPERTY, U5500WarmPlatform.PLATFORM_PROPERTY,
            U5500WarmPlatform.PLATFORM, false);
        setProperty(U5500WarmPlatform.PORT_PROPERTY, U5500WarmPlatform.PORT_PROPERTY, port.getPortName(), false);

        taskMap = new HashMap<String, Class<?>>();
        taskMap.put(CommandName.BOOT, UX500WarmEquipmentBootTask.class);

        // Initialize task map with system tasks
        taskMap.put(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), UX500WarmEquipmentShutdownTask.class);
        taskMap.put(CommandName.COREDUMP_LIST_FILE.name(), UX500WarmEquipmentListFileTask.class);
        taskMap.put(CommandName.COREDUMP_DOWNLOAD_DUMP.name(), UX500WarmEquipmentDownloadDumpTask.class);
        taskMap.put(CommandName.COREDUMP_DELETE_DUMP.name(), UX500WarmEquipmentDeleteDumpTask.class);

        ServiceReference serviceReference = null;

        // create FTP communication service
        if (ftpService == null) {
            try {
                serviceReference =
                    Activator.getBundleContext().getServiceReferences(IFTPCommunicationService.class.getName(),
                        "(type=warm)")[0];

            } catch (InvalidSyntaxException e) {
                logger.error("Creating FTP communication service failed! " + e.getMessage());
            }
            ftpService = (IFTPCommunicationService) Activator.getBundleContext().getService(serviceReference);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return U5500WarmPlatform.PLATFORM + "@" + getPort().getPortName();
    }

    /**
     * {@inheritDoc}
     */
    public IEquipmentTask createTask(String[] arguments) {
        IEquipmentTask task = null;
        Class<?> clazz = taskMap.get(arguments[0]);
        if (clazz != null) {
            try {
                task = (IEquipmentTask) clazz.getConstructor(AbstractUX500WarmEquipment.class).newInstance(this);
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
    public EquipmentType getEquipmentType() {
        return EquipmentType.U5500;
    }

}
