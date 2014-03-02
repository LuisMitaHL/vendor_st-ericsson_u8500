package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.PSReadGlobalDataUnitType;

/**
 * @author xdancho
 * 
 */
public class DummyEquipmentReadGlobalDataUnitTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String storageId;

    private int unitId;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public DummyEquipmentReadGlobalDataUnitTask(DummyEquipment equipment) {
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
        notifyTaskMessage("Read Global Data Unit: storage id: " + storageId + ", unit id: " + unitId);
        notifyTaskStart();

        try {
            mobileEquipment.getMock().setResult(new PSReadGlobalDataUnitType(0, new byte[] {
                (byte) 0xCA, (byte) 0xFE, (byte) 0xBA, (byte) 0xBE}));
        } catch (Exception e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }

        PSReadGlobalDataUnitType result =
            (PSReadGlobalDataUnitType) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_UNIT,
                new Object[] {
                    storageId, unitId}, this);

        if (result.getStatus() != 0) {
            resultCode = result.getStatus();
            try {
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                e.printStackTrace();
            }
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
        unitId = Integer.parseInt(arguments[3]);

    }

}
