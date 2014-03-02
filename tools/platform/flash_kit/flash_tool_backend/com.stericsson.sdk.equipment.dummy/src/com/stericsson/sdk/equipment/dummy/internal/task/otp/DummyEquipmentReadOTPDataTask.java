package com.stericsson.sdk.equipment.dummy.internal.task.otp;

import java.util.Arrays;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.AbstractDummyEquipmentTask;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for reading OTP data from specified ME and returning it as Base64 string.
 * 
 * @author Vit Sykala
 */
public class DummyEquipmentReadOTPDataTask extends AbstractDummyEquipmentTask {

    private static final int LINE_LENGTH = 8;

    private Logger logger = Logger.getLogger(DummyEquipmentReadOTPDataTask.class.getName());

    /**
     * @param equipment
     *            Equipment for which this task is.
     */
    public DummyEquipmentReadOTPDataTask(DummyEquipment equipment) {
        super(equipment, CommandName.OTP_READ_OTP_DATA);
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        byte[] input = Base64.decode((String) arguments[0]);
        logger.debug("input"+Arrays.toString(input));
        byte[] result = new byte[input.length];
        resultMessage = "";
        byte[] buffer = new byte[LINE_LENGTH];
        for (int i = 0; i <= input.length-LINE_LENGTH; i += LINE_LENGTH) {
            System.arraycopy(input, i, buffer, 0, LINE_LENGTH);
            logger.debug("line "+(i/LINE_LENGTH)+": "+Arrays.toString(buffer));
            buffer = ((DummyEquipment) getEquipment()).getOtp(buffer);
            System.arraycopy(buffer, 0, result, i, LINE_LENGTH);
        }
        resultMessage = Base64.encode(result);
        resultCode = 0;
        return new EquipmentTaskResult(resultCode, resultMessage, result, false);
    }
}
