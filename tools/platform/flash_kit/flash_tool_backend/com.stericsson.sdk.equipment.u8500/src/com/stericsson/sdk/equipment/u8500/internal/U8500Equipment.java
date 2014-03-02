package com.stericsson.sdk.equipment.u8500.internal;

import java.util.List;

import org.apache.log4j.Logger;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.internal.EquipmentCommandModel;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.tasks.EquipmentFlashModel;
import com.stericsson.sdk.equipment.tasks.EquipmentSetEnhancedAreaTask;
import com.stericsson.sdk.equipment.u8500.Activator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.SystemCollectDataType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomlju
 */
public class U8500Equipment extends AbstractLoaderCommunicationEquipment implements ILoaderCommunicationListener,
    ServiceTrackerCustomizer {

    /** */
    private static Logger logger = Logger.getLogger(U8500Equipment.class);

    /**
     * Constructor.
     * 
     * @param port
     *            Port
     * @param profile
     *            Profile
     */
    public U8500Equipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        setProperty(U8500Platform.PLATFORM_PROPERTY, U8500Platform.PLATFORM_PROPERTY, U8500Platform.PLATFORM, false);
        setProperty(U8500Platform.PORT_PROPERTY, U8500Platform.PORT_PROPERTY, port.getPortName(), false);

        taskMap.put(CommandName.FLASH_SET_ENHANCED_AREA.name(), EquipmentSetEnhancedAreaTask.class);

        if (loaderCommunicationService == null) {
            ServiceReference serviceReference = null;
            try {
                if (EnvironmentProperties.isRunningInTestMode()) {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=mock)")[0];
                } else {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=normal)")[0];
                }
            } catch (InvalidSyntaxException e) {
                logger.error(e.getMessage(), e);
            }

            if (serviceReference != null) {
                ServiceTracker tracker = new ServiceTracker(Activator.getBundleContext(), serviceReference, this);
                tracker.open();
            } else {
                logger.error("Failed to load service.");
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return U8500Platform.PLATFORM + "@" + getPort().getPortName();
    }

    /**
     * remove a LoaderCommunication instance
     * 
     * @return status of the operation
     */
    public boolean deinitLoaderCommunication() {
        return loaderCommunicationService.cleanUp(instanceId);
    }

    /**
     * set a timeout value
     * 
     * @param type
     *            the type of the timeout
     * @param value
     *            the value to set
     */
    public void setTimeouts(String type, long value) {
        loaderCommunicationService.setTimeouts(type, value);
    }

    /**
     * get the current timeout
     * 
     * @param type
     *            the type of timeout
     * @return the timeout in milliseconds
     */
    public long getTimeouts(String type) {
        return loaderCommunicationService.getTimeouts(type);
    }

    /**
     * 
     * @return a list of supported commands
     */
    public List<SupportedCommand> getSupportedCommands() {
        return loaderCommunicationService.getSupportedCommands(instanceId);
    }

    /**
     * Update what task services should be supported by this equipment.
     */
    private void updateCommandModel() {

        try {
            SupportedCommandsType supportedCommands =
                (SupportedCommandsType) sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS, new Object[0],
                    this);
            if (supportedCommands.getStatus() == 0) {
                new CommandModelHelper().updateModel(supportedCommands);
            } else {
                setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private class CommandModelHelper {
        public void updateModel(SupportedCommandsType supportedCommands) {
            setModel(EquipmentModel.COMMAND, new EquipmentCommandModel(supportedCommands));
            for (SupportedCommand command : supportedCommands.getSupportedCommands()) {
                if (command.getCommand() < 100) {
                    logger.debug("The loader supports: "
                        + LCCommandMappings.getCommandString(command.getGroup(), command.getCommand()));
                }
            }
        }
    }

    /**
     * 
     */
    private void updateFlashModel() {
        new FlashModelHelper().updateModel();
    }

    private class FlashModelHelper {
        public void updateModel() {
            try {
                ListDevicesType deviceList =
                    (ListDevicesType) sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, new Object[0],
                        U8500Equipment.this);
                if (deviceList.getStatus() == 0) {
                    SystemCollectDataType result2 =
                        (SystemCollectDataType) sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_COLLECT_DATA,
                            new Object[] {
                                2}, U8500Equipment.this);
                    if (result2.getStatus() == 0) {
                        setModel(EquipmentModel.FLASH, new EquipmentFlashModel(deviceList, result2.getCollectedData()));
                    } else {
                        setModel(EquipmentModel.FLASH, new EquipmentFlashModel(deviceList, null));
                    }
                } else {
                    setStatus(EquipmentState.ERROR);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationError(IPort port, String message) {
        // Not used
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationMessage(String title, String message, boolean logOnly) {
        // Not used
    }

    /**
     * {@inheritDoc}
     */
    public void loaderCommunicationProgress(long totalBytes, long transferredBytes) {
        // Not used
    }

    /**
     * {@inheritDoc}
     */
    public void updateModel(EquipmentModel model) {
        if (model == EquipmentModel.COMMAND) {
            updateCommandModel();
        } else if (model == EquipmentModel.FLASH) {
            updateFlashModel();
        }
    }

    /**
     * {@inheritDoc}
     */
    public Object addingService(ServiceReference reference) {
        loaderCommunicationService = (ILoaderCommunicationService) Activator.getBundleContext().getService(reference);
        return loaderCommunicationService;
    }

    /**
     * {@inheritDoc}
     */
    public void modifiedService(ServiceReference reference, Object serviceObject) {
        loaderCommunicationService = (ILoaderCommunicationService) Activator.getBundleContext().getService(reference);
    }

    /**
     * {@inheritDoc}
     */
    public void removedService(ServiceReference reference, Object service) {

    }

    /**
     * {@inheritDoc}
     */
    public EquipmentType getEquipmentType() {
        return EquipmentType.U8500;
    }
}
