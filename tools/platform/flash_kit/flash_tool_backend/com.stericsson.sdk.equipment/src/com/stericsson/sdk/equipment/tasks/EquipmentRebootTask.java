package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author ezaptom
 * 
 */
public class EquipmentRebootTask extends AbstractEquipmentTask {

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private int resultCode;

    private String resultMessage;

    private int mode;

    /**
     * @param pEquipment
     *            mobile equipment
     */
    public EquipmentRebootTask(AbstractLoaderCommunicationEquipment pEquipment) {
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
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to reboot equipment");
        int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_REBOOT, new Object[] {
            mode}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_REBOOT_EQUIPMENT.name();
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
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        int radix = 10;
        if (pArguments[2].startsWith("0x")) {
            radix = 16;
            pArguments[2] = pArguments[2].substring(2);
        }

        BigInteger biMode = new BigInteger(pArguments[2], radix);
        mode = biMode.intValue();
    }
}
