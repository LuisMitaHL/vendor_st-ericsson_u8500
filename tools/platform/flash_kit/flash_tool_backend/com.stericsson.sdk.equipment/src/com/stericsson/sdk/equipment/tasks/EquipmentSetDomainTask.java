/**
 * 
 */
package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for setting domain ME.
 * 
 * @author mbodan01
 */
public class EquipmentSetDomainTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /** Domain attribute for this task */
    private DomainTypes type;

    /**
     * @param equipment
     *            the equipment to operate on
     */
    public EquipmentSetDomainTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        type = null;
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
        if (type == null) {
            if (resultMessage.equalsIgnoreCase(mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(
                resultCode))) {
                resultMessage = "Internal call error: Task not initialized.";
            }
            resultCode = ERROR;
        } else {
            notifyTaskMessage("Setting equipment domain to " + type.getDomainName());
            notifyTaskStart();
            Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_SET_DOMAIN, new Object[] {
                type.getDomainId()}, this);
            resultCode = (Integer) obj;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.IEquipmentTask#getId() {@inheritDoc}
     */
    public String getId() {
        return CommandName.SECURITY_SET_DOMAIN.name();
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
    public void setArguments(String[] args) {
        int domainNumber = -1;
        if (args.length < 3) {
            resultMessage = "Missing arguments. Probably domain number is not defined.";
            return;
        }

        try {
            domainNumber = Integer.parseInt(args[2]);
        } catch (NumberFormatException e) {
            resultMessage = "Domain argument must be a number.";
            return;
        }
        if (domainNumber < 0 || domainNumber > 3) {
            resultMessage = "Domain argument must be a number between 0 and 3.";
            return;
        }

        type = DomainTypes.getDomain(domainNumber);
    }
}
