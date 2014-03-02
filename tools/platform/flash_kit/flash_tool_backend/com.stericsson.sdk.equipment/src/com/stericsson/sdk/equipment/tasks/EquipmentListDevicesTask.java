package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;

/**
 * @author zapletom
 * 
 */
public class EquipmentListDevicesTask extends AbstractEquipmentTask {

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentListDevicesTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to list devices from ME");
        ListDevicesType result =
            (ListDevicesType) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES,
                new Object[] {}, this);
        if (result.getStatus() != 0) {
            resultCode = result.getStatus();
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result.getStatus());
        } else {
            mobileEquipment.setModel(EquipmentModel.FLASH, new EquipmentFlashModel(result, null));

            resultMessage = result.getDevicesDescription();
            if (resultMessage == null || resultMessage.trim().length() == 0) {
                resultMessage = "No devices received from ME.";
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.FLASH_LIST_DEVICES.name();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        // TODO Auto-generated method stub
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}
