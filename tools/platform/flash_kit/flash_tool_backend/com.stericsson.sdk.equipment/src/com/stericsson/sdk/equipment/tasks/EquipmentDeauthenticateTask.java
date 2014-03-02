package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Deauthenticate task
 * 
 * @author TSIKOR01
 * 
 */
public class EquipmentDeauthenticateTask extends AbstractEquipmentTask {
    private int resultCode;

    private String resultMessage;

    int permanentFlag;

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private static final String ERROR_PREFIX = "Failed to deauthenticate. Possible reason: ";

    /**
     * Constructor
     * 
     * @param equipment
     *            Equipment used for this command
     */
    public EquipmentDeauthenticateTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        mobileEquipment = equipment;
    }

    /**
     * Not available for this command
     */
    public void cancel() {
        // NA
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        if (!(resultCode == ERROR)) {
            int result =
                (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_DEAUTHENTICATE, new Object[] {
                    permanentFlag}, this);
            if (resultCode == ERROR_NONE && result != 0) {
                resultCode = result;
                resultMessage =
                    ERROR_PREFIX + mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_DEAUTHENTICATE.name();
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
        if (arguments[2].equals("null") || arguments[2].equalsIgnoreCase("false")) {
            permanentFlag = 0;
        } else if (arguments[2].equalsIgnoreCase("true")) {
            permanentFlag = 1;
        } else {
            resultMessage = "Parameter \"permanent authentication\"  flag should be set only to true or false";
            resultCode = ERROR;
        }
    }
}
