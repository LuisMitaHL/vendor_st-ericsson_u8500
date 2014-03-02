package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.SystemCollectDataType;

/**
 * @author ezaptom
 * 
 */
public class EquipmentCollectDataTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private int type;

    /**
     * @param pEquipment
     *            mobile equipment
     */
    public EquipmentCollectDataTask(AbstractLoaderCommunicationEquipment pEquipment) {
        super(pEquipment);

        // Store U8500 equipment object
        mobileEquipment = pEquipment;

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
        notifyTaskMessage("Attempting to collect data from ME");
        SystemCollectDataType result =
            (SystemCollectDataType) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_COLLECT_DATA,
                new Object[] {
                    type}, this);
        if (result.getStatus() != 0) {
            resultCode = result.getStatus();
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        } else {
            resultMessage = result.getCollectedData();
            if (resultMessage == null || resultMessage.trim().length() == 0) {
                resultMessage = "No flash report received from ME.";
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_COLLECT_DATA.name();
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
    public void setArguments(String[] pArguments) {
        type = Integer.parseInt(pArguments[2]);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}
