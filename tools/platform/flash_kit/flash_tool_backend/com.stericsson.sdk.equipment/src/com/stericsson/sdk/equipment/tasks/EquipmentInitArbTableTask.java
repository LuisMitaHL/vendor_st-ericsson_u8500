package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author xtomzap
 * 
 */
public class EquipmentInitArbTableTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private int resultCode;

    private String resultMessage;

    private byte[] data;

    /**
     * @param equipment
     *            instance
     */
    public EquipmentInitArbTableTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        mobileEquipment = equipment;
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskMessage("iType=" + 0 + ", iLength=" + data.length + ", data="
            + HexUtilities.removeHexPrefix(HexUtilities.toHexString(data)));
        notifyTaskStart();
        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_INIT_ARB_TABLE, new Object[] {
                0, data.length, data}, this);
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
        return CommandName.SECURITY_INIT_ARB_TABLE.name();
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
        int radix = 10;
        if (arguments[2].startsWith("0x")) {
            radix = 16;
            arguments[2] = arguments[2].substring(2);
        }

        try {
            BigInteger biData = new BigInteger(arguments[2], radix);
            data = HexUtilities.toByteArray(HexUtilities.removeHexPrefix(HexUtilities.toHexString(biData.intValue())));
        } catch (NumberFormatException e) {
            throw new RuntimeException("Option '-arb_data' does not support value \'" + arguments[2] + "\'", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}
