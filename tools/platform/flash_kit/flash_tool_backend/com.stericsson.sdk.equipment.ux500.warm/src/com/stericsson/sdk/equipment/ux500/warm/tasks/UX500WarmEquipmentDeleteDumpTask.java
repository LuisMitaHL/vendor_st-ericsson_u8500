package com.stericsson.sdk.equipment.ux500.warm.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * Delete dump task
 * 
 * @author esrimpa
 * 
 */
public class UX500WarmEquipmentDeleteDumpTask extends AbstractWarmEquipmentTask {

    /** Equipment instance */
    private AbstractUX500WarmEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String coreFileName = null;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public UX500WarmEquipmentDeleteDumpTask(AbstractUX500WarmEquipment equipment) {
        super(equipment);

        // Store UX500 equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        long start = System.currentTimeMillis();
        if (resultCode == -1) {
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }
        notifyTaskStart();
        notifyTaskMessage("Attempting to delete dump file from ME");
        int result = mobileEquipment.getFtpService().deleteDump(mobileEquipment.getPort(), coreFileName);
        if (result != ERROR_NONE) {
            resultCode = result;
            resultMessage = "Failed to delete file from equipment";
        } else {
            mobileEquipment.setStatus(EquipmentState.IDLE);
        }
        long stop = System.currentTimeMillis();
        return new EquipmentTaskResult(resultCode, resultMessage, null, false, stop - start);
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
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
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
    public void setArguments(String[] pArguments) {
        coreFileName = null;

        // pArguments contains: name of command, device_id, name of property=value to be set
        if (pArguments.length < 3) {
            resultCode = -1;
            resultMessage = "Missing arguments. Probably dump file name is not provided.";
            return;
        }
        coreFileName = pArguments[2];
    }
}
