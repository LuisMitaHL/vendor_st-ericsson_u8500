package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.Convert;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.PSReadGlobalDataUnitType;

/**
 * @author xdancho
 * 
 */
public class EquipmentReadGlobalDataUnitTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String storageId;

    /** Beware!!! This value store unsigned int */
    private int unitId;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentReadGlobalDataUnitTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";

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
        notifyTaskMessage("Read Global Data Unit: storage id: " + storageId + ", unit id: "
            + Convert.uintToLong(unitId));
        notifyTaskStart();

        PSReadGlobalDataUnitType result =
            (PSReadGlobalDataUnitType) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_UNIT,
                new Object[] {
                    storageId, unitId}, this);

        if (result.getStatus() != 0) {
            resultCode = result.getStatus();
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result.getStatus());
        } else {
            resultMessage = HexUtilities.toHexString(result.getData());
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name();
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
        storageId = arguments[2];

        if (storageId.trim().equalsIgnoreCase("GDFS")) {
            storageId = mobileEquipment.getProfile().getProperty(ConfigurationOption.PROPERTY_GDFS_AREA_PATH);
            if (storageId == null || storageId.trim().length() == 0) {
                notifyTaskMessage("GDFS area path is not defined in the current profile");
            }
        } else if (storageId.trim().equalsIgnoreCase("TA")) {
            storageId = mobileEquipment.getProfile().getProperty(ConfigurationOption.PROPERTY_TRIM_AREA_PATH);
            if (storageId == null || storageId.trim().length() == 0) {
                notifyTaskMessage("Trim area path is not defined in the current profile");
            }
        }

        // long parsing is there because we need parse 0xfffffffc ...
        if (HexUtilities.hasHexPrefix(arguments[3])) {
            unitId = (int) HexUtilities.parseLongHexDigit(arguments[3]);
        } else {
            unitId = (int) Long.parseLong(arguments[3]);
        }
    }

}
