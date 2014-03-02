package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for erasing a flash area on a U8500 ME
 * 
 * @author xolabju
 * 
 */
public class EquipmentEraseAreaTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String areaPath;

    private long offset;

    private long length;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentEraseAreaTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
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
        notifyTaskMessage("areaPath=" + areaPath + ", offset=" + offset + ", length=" + length);
        notifyTaskStart();
        mobileEquipment.setStatus(EquipmentState.ERASING);
        int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_ERASE_AREA, new Object[] {
            areaPath, offset, length}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
        }
        mobileEquipment.setStatus(EquipmentState.IDLE);
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
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
    public String getId() {
        return CommandName.FLASH_ERASE_AREA.name();
    }

    // /**
    // * {@inheritDoc}
    // */
    // public boolean schedule(String[] args) {
    // areaPath = args[0];
    // offset = Long.decode(args[1]);
    // length = Long.decode(args[2]);
    //
    // return schedule();
    // }

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
        areaPath = arguments[2];

        int radix = 10;
        if (arguments[3].startsWith("0x")) {
            radix = 16;
            arguments[3] = arguments[3].substring(2);
        }

        BigInteger biOffset = new BigInteger(arguments[3], radix);

        radix = 10;
        if (arguments[4].startsWith("0x")) {
            radix = 16;
            arguments[4] = arguments[4].substring(2);
        } else if (arguments[4].equalsIgnoreCase("ALL")) {
            arguments[4] = "-1";
        }

        BigInteger biLength = new BigInteger(arguments[4], radix);

        offset = biOffset.longValue();
        length = biLength.longValue();
    }

}
