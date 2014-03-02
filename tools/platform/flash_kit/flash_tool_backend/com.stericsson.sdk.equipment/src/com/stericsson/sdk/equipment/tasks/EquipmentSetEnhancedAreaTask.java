package com.stericsson.sdk.equipment.tasks;

import java.math.BigInteger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * 
 * @author mbocek01
 * 
 */
public class EquipmentSetEnhancedAreaTask extends AbstractEquipmentTask {

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
     * constructor
     * 
     * @param equipment
     *            .
     */
    public EquipmentSetEnhancedAreaTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * task is not cancelable
     */
    public void cancel() {
    }

    /**
     * method execute task set_enhanced_area
     * 
     * @return .
     */
    public EquipmentTaskResult execute() {
        notifyTaskMessage("areaPath=" + areaPath + ", offset=" + offset + ", length=" + length);
        notifyTaskStart();
        mobileEquipment.setStatus(EquipmentState.BUSY);
        int result =
            (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_SET_ENHANCED_AREA, new Object[] {
                areaPath, offset, length}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result);
        }
        mobileEquipment.setStatus(EquipmentState.IDLE);
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * returns task ID
     * 
     * @return id
     */
    public String getId() {
        return CommandName.FLASH_SET_ENHANCED_AREA.name();
    }

    /**
     * @return String representation of task
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * returns true if is task cancelable
     * 
     * @return false
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * method set task arguments
     * 
     * @param arguments
     *            .
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
        }

        BigInteger biLength = new BigInteger(arguments[4], radix);

        offset = biOffset.longValue();
        length = biLength.longValue();

    }

}
