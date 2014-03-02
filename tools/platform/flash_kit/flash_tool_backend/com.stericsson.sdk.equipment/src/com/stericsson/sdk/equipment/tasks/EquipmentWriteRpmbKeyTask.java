package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author mbocek01
 * 
 */
public class EquipmentWriteRpmbKeyTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private int deviceId;

    private boolean commercial;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentWriteRpmbKeyTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // nothing to implement,intentionally empty method
    }

    /**
     * @return result
     */
    public EquipmentTaskResult execute() {
        notifyTaskMessage("Writing RPMB key.");

        notifyTaskStart();

        Object obj =
            mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_WRITE_RPMB_KEY, 
                    new Object[] {deviceId, commercial}, this);

        resultCode = (Integer) obj;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * @return .
     */
    public String getId() {
        return CommandName.SECURITY_WRITE_RPMB_KEY.name();
    }

    /**
     * @return isCancelable
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * @param pArguments
     *            args
     */
    public void setArguments(String[] pArguments) {
        // nothing to implement,intentionally empty method
        BigInteger biDeviceId = new BigInteger(pArguments[2]);
        deviceId = biDeviceId.intValue();
        Boolean bCommercial = new Boolean(pArguments[3]);
        commercial = bCommercial.booleanValue();
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

}
