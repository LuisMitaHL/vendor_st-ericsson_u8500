package com.stericsson.sdk.equipment.tasks;

import java.io.File;

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
public class EquipmentWriteGlobalDataSetTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private long dataLength;

    private String sourcePath;

    private String devicePath;

    private boolean useBulk;

    private boolean pathExist;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentWriteGlobalDataSetTask(AbstractLoaderCommunicationEquipment equipment) {
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
        if (devicePath == null || sourcePath == null) {
            // missing devicePath or targetPath
            resultMessage =
                "GDFS/Trim Area path is not defined in the current profile,"
                    + " e.g. Property GDFSAreaPath=\"<ME area path>\"" + " or Property TrimAreaPath=\"<ME area path>\"";
            resultCode = ERROR;
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }

        if (!pathExist) {
            resultMessage = "Given path : " + sourcePath + " does not exist or it is not a file!";
            resultCode = ERROR;
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }

        notifyTaskMessage("Write Global Data Set: devicePath: " + devicePath + ";dataLength: " + dataLength
            + ";sourcePath" + sourcePath);
        notifyTaskStart();

        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_SET, new Object[] {
                devicePath, dataLength, sourcePath, useBulk, null}, this);

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
        return CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name();
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#isCancelable() {@inheritDoc}
     */
    public boolean isCancelable() {
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

        sourcePath = arguments[3];
        dataLength = new File(sourcePath).length();
        if (isPCPath()) {
            removePCPrefix();
            validatePath();
            this.useBulk = true;
        } else {
            this.useBulk = false;
            pathExist = true;// we are not able to check if path on ME truly exists
            // this kind of check must be done by loader
        }

    }

    private boolean isPCPath() {
        return !hasMEPrefix() || hasPCPrefix();
    }

    private boolean hasMEPrefix() {
        return sourcePath.substring(0, 2).equalsIgnoreCase("ME");
    }

    private boolean hasPCPrefix() {
        return sourcePath.substring(0, 2).equalsIgnoreCase("PC");
    }

    private void removePCPrefix() {
        if (hasPCPrefix()) {
            sourcePath = sourcePath.substring(2);
        }
    }

    private void validatePath() {
        File gdfsFile = new File(sourcePath);
        if (gdfsFile.exists() && gdfsFile.isFile()) {
            pathExist = true;
        } else {
            pathExist = false;
            notifyTaskMessage("Given path : " + sourcePath + " does not exist or it is not a file!");
        }

    }
}
