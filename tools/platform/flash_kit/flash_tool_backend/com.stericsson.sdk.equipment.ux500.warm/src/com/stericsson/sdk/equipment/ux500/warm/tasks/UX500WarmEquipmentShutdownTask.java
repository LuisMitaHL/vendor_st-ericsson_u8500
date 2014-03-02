package com.stericsson.sdk.equipment.ux500.warm.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * Task for shutting down a UX500 ME
 *
 * @author esrimpa
 */
public class UX500WarmEquipmentShutdownTask extends AbstractWarmEquipmentTask {

    /** Equipment instance */
    private AbstractUX500WarmEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /** String to identify this specific task **/
    public static final String ID = "SHUTDOWN_WARM";

    /**
     * Constructor.
     *
     * @param equipment
     *            Equipment interface
     */
    public UX500WarmEquipmentShutdownTask(AbstractUX500WarmEquipment equipment) {
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
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to shutdown equipment");
        int result = mobileEquipment.getFtpService().shutdown(mobileEquipment.getPort());
        if (result != 0) {
            resultCode = result;
            resultMessage = "failed to shutdown equipment";
        }
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
        return ID;
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
    public void setArguments(String[] arguments) {
    }
}
