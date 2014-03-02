package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for deletinging files on ftp on a Dummy ME
 * 
 * @author esrimpa
 * 
 */
public class DummyEquipmentDeleteDumpTask extends AbstractWarmEquipmentTask {

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
    public DummyEquipmentDeleteDumpTask(DummyEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
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
        notifyTaskMessage("Attempting to delete dump file from ME");
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
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
    public String getId() {
        return CommandName.COREDUMP_DELETE_DUMP.name();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
    }

}
