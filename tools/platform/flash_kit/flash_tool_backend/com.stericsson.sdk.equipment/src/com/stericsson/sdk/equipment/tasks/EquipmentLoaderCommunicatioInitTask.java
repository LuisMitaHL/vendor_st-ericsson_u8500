package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task to initialize loader communication
 * 
 * @author xtomzap
 * 
 */
public class EquipmentLoaderCommunicatioInitTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private final AbstractLoaderCommunicationEquipment mobileEquipment;

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
    public EquipmentLoaderCommunicatioInitTask(AbstractLoaderCommunicationEquipment equipment) {
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

        try {
            if (mobileEquipment.usesLoaderCommunication()) {
                notifyTaskMessage("Switching to loader communication");
                try {
                    mobileEquipment.initLoaderCommunication();
                } catch (Exception e) {
                    String result = "Cannot initialize loader communication: " + e.getMessage();
                    notifyTaskMessage(result);
                    throw new EquipmentBootException(result);
                }

                int result =
                    (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_LOADER_STARTUP_STATUS,
                        new Object[0], this);

                if (result != 0) {
                    resultCode = result;
                    resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
                }
            }

        } catch (EquipmentBootException e) {
            resultCode = 1;
            resultMessage = e.getMessage();
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, (resultCode != 0));
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.LOADER_COMM_INIT;
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
