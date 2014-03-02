package com.stericsson.sdk.equipment.u5500.internal;


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
import com.stericsson.sdk.equipment.IRuntimeExecutor;
import com.stericsson.sdk.equipment.RuntimeExecutor;
import com.stericsson.sdk.equipment.RuntimeExecutorMock;
import com.stericsson.sdk.equipment.internal.EquipmentCommandModel;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.tasks.EquipmentSetEnhancedAreaTask;
import com.stericsson.sdk.equipment.u5500.Activator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomlju
 */
public class U5500Equipment extends AbstractLoaderCommunicationEquipment implements ILoaderCommunicationListener,
    ServiceTrackerCustomizer {

    /** */
    private static Logger logger = Logger.getLogger(U5500Equipment.class);

    private IRuntimeExecutor runtimeExecutor;

    /**
     * Constructor.
     * 
     * @param port
     *            Port
     * @param profile
     *            Profile
     */
    public U5500Equipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        setProperty(U5500Platform.PLATFORM_PROPERTY, U5500Platform.PLATFORM_PROPERTY, U5500Platform.PLATFORM, false);
        setProperty(U5500Platform.PORT_PROPERTY, U5500Platform.PORT_PROPERTY, port.getPortName(), false);

        taskMap.put(CommandName.FLASH_SET_ENHANCED_AREA.name(), EquipmentSetEnhancedAreaTask.class);

        if (loaderCommunicationService == null) {
            ServiceReference serviceReference = null;
            try {
                if (EnvironmentProperties.isRunningInTestMode()) {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=mock)")[0];
                    runtimeExecutor = new RuntimeExecutorMock();
                } else {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=normal)")[0];
                    runtimeExecutor = new RuntimeExecutor();
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
        return U5500Platform.PLATFORM + "@" + getPort().getPortName();
    }

    /**
     * @return runtimeExecutor
     */
    public IRuntimeExecutor getRuntimeExecutor() {
        return runtimeExecutor;
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

    private void updateFlashModel() {
        try {
            ListDevicesType deviceList =
                (ListDevicesType) sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, new Object[0], this);
            if (deviceList.getStatus() == 0) {
                setModel(EquipmentModel.FLASH, new U5500EquipmentFlashModel(deviceList));
            } else {
                setStatus(EquipmentState.ERROR);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void updateCommandModel() {
        SupportedCommandsType supportedCommands =
            (SupportedCommandsType) sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS, new Object[0],
                this);
        if (supportedCommands.getStatus() == 0) {
            new CommandModelHelper().updateModel(supportedCommands);
        } else {
            setStatus(EquipmentState.ERROR);
        }

    }

    private class CommandModelHelper {
        public void updateModel(SupportedCommandsType supportedCommands) {
            for (SupportedCommand command : supportedCommands.getSupportedCommands()) {
                setModel(EquipmentModel.COMMAND, new EquipmentCommandModel(supportedCommands));
                if (command.getCommand() < 100) {
                    logger.debug("The loader supports: "
                        + LCCommandMappings.getCommandString(command.getGroup(), command.getCommand()));
                }
            }
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
        return EquipmentType.U5500;
    }
}
