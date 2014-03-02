package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipmentFlashModel;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;

/**
 * @author zapletom
 * 
 */
public class DummyEquipmentListDevicesTask extends AbstractEquipmentTask {
    private DummyEquipment mobileEquipment;

    private int resultCode;

    private String resultMessage;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentListDevicesTask(DummyEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
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

        try {
            mobileEquipment.getMock().setResult(new ListDevicesType(0, 0));
        } catch (Exception e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }

        ListDevicesType result =
            (ListDevicesType) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES,
                new Object[] {}, this);
        if (result.getStatus() != 0) {
            resultCode = result.getStatus();
            try {
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(result.getStatus());
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {

            mobileEquipment.setModel(EquipmentModel.FLASH, new DummyEquipmentFlashModel(result));

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
