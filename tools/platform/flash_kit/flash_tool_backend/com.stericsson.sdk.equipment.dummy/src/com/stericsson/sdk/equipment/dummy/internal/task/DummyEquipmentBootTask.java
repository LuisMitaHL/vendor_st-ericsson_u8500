package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * 
 * @author xolabju
 */
public class DummyEquipmentBootTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentBootTask(DummyEquipment equipment) {
        super(equipment);

        // Store U8500 equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {

        getEquipment().setStatus(EquipmentState.BOOTING);
        notifyTaskStart();
        notifyTaskMessage("Booting Dummy ME");

        getEquipment().setStatus(EquipmentState.IDLE);
        return new EquipmentTaskResult(resultCode, resultMessage, null, (resultCode != 0));
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return "BOOT@" + mobileEquipment;
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.BOOT;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
    }

}
