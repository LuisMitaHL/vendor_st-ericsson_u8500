package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author xmilhan
 * 
 */
public class EquipmentReadGlobalDataSetTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String targetPath;

    private String devicePath;

    private boolean useBulk;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentReadGlobalDataSetTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;

    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#cancel() {@inheritDoc}
     */
    public void cancel() {
        // TODO Auto-generated method stub

    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#execute() {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        if (devicePath == null || targetPath == null) {
            // missing devicePath or targetPath
            resultMessage =
                "GDFS/Trim Area path is not defined in the current profile,"
                    + " e.g. Property GDFSAreaPath=\"<ME area path>\"" + " or Property TrimAreaPath=\"<ME area path>\"";
            resultCode = ERROR;
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }

        notifyTaskMessage("Read Global Data Set: devicePath: " + devicePath + ";targetPath " + targetPath);
        notifyTaskStart();

        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_SET, new Object[] {
                devicePath, targetPath, useBulk, null}, this);

        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name();
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#isCancelable() {@inheritDoc}
     */
    public boolean isCancelable() {
        // TODO Auto-generated method stub
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#setArguments(java.lang.String[])
     *      {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
        // PARAMETER_EQUIPMENT_ID, PARAMETER_STORAGE_ID, PARAMETER_PATH
        // DevicePath, DataLength, SourcePath
        devicePath = arguments[2];

        if (devicePath.trim().equalsIgnoreCase("GDFS")) {
            devicePath = mobileEquipment.getProfile().getProperty(ConfigurationOption.PROPERTY_GDFS_AREA_PATH);
            if (devicePath == null || devicePath.trim().length() == 0) {
                notifyTaskMessage("GDFS area path is not defined in the current profile");
            }
        } else if (devicePath.trim().equalsIgnoreCase("TA")) {
            devicePath = mobileEquipment.getProfile().getProperty(ConfigurationOption.PROPERTY_TRIM_AREA_PATH);
            if (devicePath == null || devicePath.trim().length() == 0) {
                notifyTaskMessage("Trim area path is not defined in the current profile");
            }
        }

        // targetPath = arguments[3].substring(3, arguments[3].length());
        targetPath = arguments[3];
        if (arguments[3].substring(0, 2).equalsIgnoreCase("PC")
            || !(arguments[2].substring(0, 2).equalsIgnoreCase("ME"))) {
            this.useBulk = true;
        } else {
            this.useBulk = false;
        }

    }

}
