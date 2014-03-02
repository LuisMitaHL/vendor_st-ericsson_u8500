package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for shutting down a U8500 ME
 * 
 * @author xolabju
 * 
 */
public class EquipmentProcessFileTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String filePath;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentProcessFileTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store U8500 equipment object
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
        long start = System.currentTimeMillis();
        mobileEquipment.setStatus(EquipmentState.FLASHING);
        notifyTaskMessage("Processing file: " + filePath);

        int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_PROCESS_FILE, new Object[] {
            100L, "x-empflash/flasharchive", filePath, true, null}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
        }
        mobileEquipment.setStatus(EquipmentState.IDLE);
        long stop = System.currentTimeMillis();
        return new EquipmentTaskResult(resultCode, resultMessage, null, false, stop - start);
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.FLASH_PROCESS_FILE.name();
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
        filePath = arguments[2];

    }
}
