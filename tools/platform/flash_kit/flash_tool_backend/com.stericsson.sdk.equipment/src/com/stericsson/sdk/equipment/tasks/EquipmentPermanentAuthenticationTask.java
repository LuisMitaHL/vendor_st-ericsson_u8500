package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Permanent authentication task
 * 
 * @author tsikor01
 * 
 */
public class EquipmentPermanentAuthenticationTask extends AbstractEquipmentTask implements IAuthenticate {

    private int resultCode;

    private String resultMessage;

    boolean permanentFlag;

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private static final String ERROR_PREFIX = "Failed to authenticate. Possible reason: ";

    /**
     * Constructor
     * 
     * @param equipment
     *            Equipment used for this command
     */
    public EquipmentPermanentAuthenticationTask(AbstractLoaderCommunicationEquipment equipment) {
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
        resultCode = 0;
        int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, new Object[] {
            LCDefinitions.AUTHENTICATION_PERMANENT, this}, this);
        if (resultCode == 0 && result != 0) {
            resultCode = result;
            resultMessage =
                ERROR_PREFIX + mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name();
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
        // NA
    }

    /**
     * 
     * {@inheritDoc}
     */
    public byte[] getChallengeResponse(byte[] challenge) {
        return null;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public Object[] getControlKeys() {
        return null;
    }

}
