package com.stericsson.sdk.equipment.dummy.internal.task.otp;

import java.util.Arrays;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.AbstractDummyEquipmentTask;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for burning OTP data provided as Base64 string to specified ME.
 * 
 * @author vsykal01
 * 
 */
public class DummyEquipmentBurnOTPDataTask extends AbstractDummyEquipmentTask {

    private static final int LINE_LENGTH = 8;

    private static final int BURN_BIT = 10;

    private static final int BURN_INDEX = BURN_BIT / 8;

    private static final byte BURN_MASK = (byte) (1 << (7-BURN_BIT % 8));

    private Logger logger = Logger.getLogger(DummyEquipmentBurnOTPDataTask.class.getName());

    /**
     * @param equipment
     *            Equipment for which this task is.
     */
    public DummyEquipmentBurnOTPDataTask(DummyEquipment equipment) {
        super(equipment, CommandName.OTP_BURN_OTP_DATA);
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        byte[] input = Base64.decode((String) arguments[0]);
        logger.debug("input: "+Arrays.toString(input));
        resultMessage = "";
        byte[] buffer = new byte[LINE_LENGTH];
        resultCode = 0;
        for (int i = 0; i <= input.length - LINE_LENGTH; i += LINE_LENGTH) {
            System.arraycopy(input, i, buffer, 0, LINE_LENGTH);
            logger.debug("line "+(i/LINE_LENGTH)+": "+Arrays.toString(buffer));
            if ((buffer[BURN_INDEX] & BURN_MASK) == BURN_MASK) {
                buffer[BURN_INDEX] ^= BURN_MASK;// remove burn bit
                boolean result = ((DummyEquipment) getEquipment()).setOtp(buffer);
                resultMessage += Arrays.toString(buffer) + " - " + result + "\n";
                if (!result) {
                    resultCode = 1;
                    resultMessage="FAILED TO WRITE OTP\n"+resultMessage;
                    break;
                }
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, resultMessage, false);
    }
}
