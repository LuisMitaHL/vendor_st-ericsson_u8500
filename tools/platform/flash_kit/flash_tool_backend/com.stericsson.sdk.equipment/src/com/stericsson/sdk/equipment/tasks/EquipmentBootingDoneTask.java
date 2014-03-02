package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * @author xtomzap
 * 
 */
public class EquipmentBootingDoneTask extends AbstractEquipmentTask {

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
    public EquipmentBootingDoneTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
    }

    /**
    * {@inheritDoc}
    */
    public EquipmentTaskResult execute() {
        EquipmentState state = getEquipment().getStatus().getState(); 
        if (state == EquipmentState.LC_NOT_INITIALIZED || state == EquipmentState.IDLE) {
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }
        return new EquipmentTaskResult(1, "Booting failed", null, true);
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
    public String getId() {
        return CommandName.BOOTING_DONE;
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
    public void setArguments(String[] arguments) {

    }
}
