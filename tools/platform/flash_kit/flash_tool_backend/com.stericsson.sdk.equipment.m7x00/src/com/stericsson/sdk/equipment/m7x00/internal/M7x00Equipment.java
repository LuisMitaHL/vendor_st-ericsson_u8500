package com.stericsson.sdk.equipment.m7x00.internal;

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
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IRuntimeExecutor;
import com.stericsson.sdk.equipment.RuntimeExecutor;
import com.stericsson.sdk.equipment.RuntimeExecutorMock;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.m7x00.Activator;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentChangeAccessTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCheckCompatibilityTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCopyFileTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentCreateDirectoryTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentDeleteFileTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentFormatVolumeTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentListDirectoryTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentListLoadModulesTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentMoveFileTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentPropertiesTask;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentVolumePropertiesTask;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xtomlju
 * @author Vit Sykala
 */
public class M7x00Equipment extends AbstractLoaderCommunicationEquipment implements ILoaderCommunicationListener,
    ServiceTrackerCustomizer {

    /** */
    private static Logger logger = Logger.getLogger(M7x00Equipment.class);

    private IRuntimeExecutor runtimeExecutor;

    private M7x00EquipmentModelUpdater modelUpdater;

    /**
     * Constructor.
     * 
     * @param port
     *            Port
     * @param profile
     *            Profile
     */
    public M7x00Equipment(IPort port, IEquipmentProfile profile) {
        super(port, profile);
        setProperty(M7x00Platform.PLATFORM_PROPERTY, M7x00Platform.PLATFORM_PROPERTY, M7x00Platform.PLATFORM, false);
        setProperty(M7x00Platform.PORT_PROPERTY, M7x00Platform.PORT_PROPERTY, port.getPortName(), false);

        modelUpdater = new M7x00EquipmentModelUpdater(this);

        // Initialize task map with File System tasks
        taskMap.put(CommandName.FILE_SYSTEM_CHANGE_ACCESS.name(), M7X00EquipmentChangeAccessTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_COPY_FILE.name(), M7X00EquipmentCopyFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_CREATE_DIRECTORY.name(), M7X00EquipmentCreateDirectoryTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_DELETE_FILE.name(), M7X00EquipmentDeleteFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_FORMAT_VOLUME.name(), M7X00EquipmentFormatVolumeTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name(), M7X00EquipmentListDirectoryTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_MOVE_FILE.name(), M7X00EquipmentMoveFileTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_PROPERTIES.name(), M7X00EquipmentPropertiesTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_VOLUME_PROPERTIES.name(), M7X00EquipmentVolumePropertiesTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_LIST_MODULES.name(), M7X00EquipmentListLoadModulesTask.class);
        taskMap.put(CommandName.FILE_SYSTEM_CHECK_COMPATIBILITY.name(), M7X00EquipmentCheckCompatibilityTask.class);

        if (loaderCommunicationService == null) {
            ServiceReference serviceReference = null;
            try {
                if (!EnvironmentProperties.isRunningInTestMode()) {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=normal)")[0];

                    runtimeExecutor = new RuntimeExecutor();
                } else {
                    serviceReference =
                        Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                            "(type=mock)")[0];
                    runtimeExecutor = new RuntimeExecutorMock();
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
        return M7x00Platform.PLATFORM + "@" + getPort().getPortName();
    }

    /**
     * @return runtimeExecutor
     */
    public IRuntimeExecutor getRuntimeExecutor() {
        return runtimeExecutor;
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
        modelUpdater.updateModel(model);
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
        return EquipmentType.M7X00;
    }
}
