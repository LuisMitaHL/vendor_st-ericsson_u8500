package com.stericsson.sdk.equipment.ux500.warm.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.io.AbstractFTPPort;

/**
 * Boot task for UX500 warm equipment.
 * 
 * @author esrimpa
 */
public class UX500WarmEquipmentBootTask extends AbstractWarmEquipmentTask {

    /** Equipment instance */
    private AbstractUX500WarmEquipment mobileEquipment;

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
    public UX500WarmEquipmentBootTask(AbstractUX500WarmEquipment equipment) {
        super(equipment);

        // Store UX500 warm equipment object
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

        try {
            getEquipment().setStatus(EquipmentState.BOOTING);
            notifyTaskStart();
            // get some information from the over FTP maybe and store it as a WarmEquipmentModel?
        } catch (Exception e) {
            resultCode = 1;
            resultMessage = e.getMessage();

        } finally {
            if ((resultCode != 0)) {
                getEquipment().setStatus(EquipmentState.ERROR, resultMessage);
            } else if (isDumping()) {
                getEquipment().setStatus(EquipmentState.DUMPING);
            } else {
                getEquipment().setStatus(EquipmentState.IDLE);
            }
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, (resultCode != 0));
    }

    private boolean isDumping() {
        return ((AbstractFTPPort) mobileEquipment.getPort()).isDumping();
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
        return getId() + "@" + mobileEquipment;
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
