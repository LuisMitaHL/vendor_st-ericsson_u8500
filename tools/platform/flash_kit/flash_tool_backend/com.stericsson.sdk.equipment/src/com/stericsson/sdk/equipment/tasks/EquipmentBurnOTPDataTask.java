package com.stericsson.sdk.equipment.tasks;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Task for burning OTP data provided as Base64 string to specified ME.
 * 
 * @author pkutac01
 * 
 */
public class EquipmentBurnOTPDataTask extends AbstractLCEquipmentTask {

    private static final int OTP_DATA_ARGUMENT_ID = 2;

    private static final int OTP_ROW_BYTE_SIZE = 8;

    private static final int BURN_BIT = 10;

    private static final int BURN_INDEX = BURN_BIT / 8;

    private static final byte BURN_MASK = (byte) (1 << (7 - BURN_BIT % 8));

    private static Logger logger = Logger.getLogger(EquipmentReadOTPDataTask.class.getName());

    private boolean shouldExecute = true;

    /**
     * {@inheritDoc}
     */

    @Override
    public EquipmentTaskResult execute() {
        if (shouldExecute) {
            return super.execute();
        } else {
            return new EquipmentTaskResult(ERROR_NONE, ((AbstractLoaderCommunicationEquipment) getEquipment())
                .getLoaderCommunicationService().getLoaderErrorDesc(ERROR_NONE)
                + " The input file does not contain any data that should be burned", null, false);
        }
    }

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment instance.
     */
    public EquipmentBurnOTPDataTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_OTP_SET_BITS, CommandName.OTP_BURN_OTP_DATA);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setArguments(String[] pArguments) {
        byte[] data = Base64.decode(pArguments[OTP_DATA_ARGUMENT_ID]);
        logger.debug("Data received over BRP connection:\n" + Arrays.toString(data));
        shouldExecute = false;// row to burn hasn't found yet
        List<Integer> toBurn = new ArrayList<Integer>();
        int minIndex = Integer.MAX_VALUE;
        int last = data.length - OTP_ROW_BYTE_SIZE;
        for (int i = 0; i <= last; i += OTP_ROW_BYTE_SIZE) {
            if ((data[i + BURN_INDEX] & BURN_MASK) == BURN_MASK) {
                int index = data[i + 3] & 0xFF;
                index |= (data[i + 2] & 0xFF) << 8;
                if (index < minIndex) {
                    minIndex = index;
                }
                toBurn.add(i);
                shouldExecute = true;// row to burn was found
            }
        }

        byte[] buffer = new byte[toBurn.size() * OTP_ROW_BYTE_SIZE];
        int offset = 0;
        for (int i : toBurn) {
            System.arraycopy(data, i, buffer, offset, OTP_ROW_BYTE_SIZE);
            // Set burn bit to 0, because it is set to 1 when row is to be burned.
            buffer[offset + BURN_INDEX] &= ~BURN_MASK;
            offset += OTP_ROW_BYTE_SIZE;
        }

        logger.debug("Data to be burned to ME:\n" + Arrays.toString(buffer));
        buffer = LittleEndianByteConverter.swapEndianess(buffer);
        logger.debug("Swaped data to be burned to ME:\n" + Arrays.toString(buffer));
        arguments = new Object[] {
            0, minIndex * 64, buffer.length * 8, buffer};
    }
}
