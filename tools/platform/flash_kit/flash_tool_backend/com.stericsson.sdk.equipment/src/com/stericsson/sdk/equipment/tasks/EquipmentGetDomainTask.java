package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.DomainType;

/**
 * Task for getting domain ME.
 * 
 * @author mbodan01
 */
public class EquipmentGetDomainTask extends AbstractEquipmentTask {

    private static final String ERROR_RECEIVED_AN_UNEXPECTED_OBJECT = "Error: Received an unexpected object";

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentGetDomainTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = "No error";
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#cancel() {@inheritDoc}
     */
    public void cancel() {
        // nothing to implement
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#execute() {@inheritDoc}
     */
    public EquipmentTaskResult execute() {

        notifyTaskStart();

        notifyTaskMessage("Gaining equipment domain.");

        Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_DOMAIN, new Object[] {}, this);

        if (obj instanceof DomainType) {
            DomainType domainstruct = (DomainType) obj;
            resultCode = domainstruct.getStatus();

            if (resultCode != 0) {
                resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
                return new EquipmentTaskResult(resultCode, resultMessage, null, false);
            }

            DomainTypes type = DomainTypes.getDomain(domainstruct.getWrittenDomain());
            if (type != null) {
                resultMessage = type.getDomainName();
            } else {
                resultCode = ERROR;
                resultMessage =
                    String.format("Error: Received an unknown domain whitch number is %d.", domainstruct
                        .getWrittenDomain());
            }

        } else if (obj instanceof Integer) {
            resultCode = (Integer) obj;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);

        } else {
            resultCode = ERROR;
            resultMessage = ERROR_RECEIVED_AN_UNEXPECTED_OBJECT;
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.SECURITY_GET_DOMAIN.name();
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#isCancelable() {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#setArguments(java.lang.String[])
     *      {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
        // nothing to implement
    }

}
