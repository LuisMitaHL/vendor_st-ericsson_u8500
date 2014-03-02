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
public class EquipmentEraseGlobalDataSetTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String devicePath;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentEraseGlobalDataSetTask(AbstractLoaderCommunicationEquipment equipment) {
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
        notifyTaskMessage("Erase Global Data Set: devicePath: " + devicePath);

        notifyTaskStart();

        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_ERASE_GLOBAL_DATA_SET, new Object[] {
                devicePath}, this);

        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name();
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
    }

}
