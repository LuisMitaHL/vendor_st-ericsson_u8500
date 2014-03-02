package com.stericsson.sdk.equipment.tasks;

import java.nio.ByteBuffer;
import java.util.Arrays;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.OTPReadBitsType;

/**
 * Task for reading OTP data from specified ME and returning it as Base64 string.
 * 
 * @author pkutac01
 * @author Vit Sykala
 */
public class EquipmentReadOTPDataTask extends AbstractLCEquipmentTask {

    private static final int OTP_DATA_ARGUMENT_ID = 2;

    private static final int OTP_ROW_BYTE_SIZE = 8;

    private static final int OTP_ROW_BIT_SIZE = OTP_ROW_BYTE_SIZE * 8;

    private static final int OTP_ROW_COUNT = 64;

    private static Logger logger = Logger.getLogger(EquipmentReadOTPDataTask.class.getName());

    private final boolean[] readIndex = new boolean[OTP_ROW_COUNT];

    private int readIndexMin = Integer.MAX_VALUE;

    private int readIndexMax = 0;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment instance.
     */
    public EquipmentReadOTPDataTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment, LCDefinitions.METHOD_NAME_OTP_READ_BITS, CommandName.OTP_READ_OTP_DATA);

        for (int i = 0; i < readIndex.length; i++) {
            readIndex[i] = false;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setArguments(String[] pArguments) {
        byte[] data = Base64.decode(pArguments[OTP_DATA_ARGUMENT_ID]);
        logger.debug("Data received over BRP connection:\n" + Arrays.toString(data));

        int last = data.length - OTP_ROW_BYTE_SIZE;
        for (int i = 0; i <= last; i += OTP_ROW_BYTE_SIZE) {
            int index = data[i + 3] & 0xFF;
            index |= (data[i + 2] & 0xFF) << 8;

            readIndex[index] = true;
            if (index < readIndexMin) {
                readIndexMin = index;
            }
            if (index > readIndexMax) {
                readIndexMax = index;
            }
        }

        arguments = new Object[] {
            0, readIndexMin * OTP_ROW_BIT_SIZE, (readIndexMax - readIndexMin + 1) * OTP_ROW_BIT_SIZE};
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof OTPReadBitsType) {
            OTPReadBitsType result = (OTPReadBitsType) resultObj;
            resultCode = result.getStatus();
            if (resultCode == 0) {
                ByteBuffer buffer = ByteBuffer.allocate((readIndexMax - readIndexMin + 1) * OTP_ROW_BYTE_SIZE);

                byte[] data = result.getDataBits();
                logger.debug("Data read from ME:\n" + Arrays.toString(data));
                data = LittleEndianByteConverter.swapEndianess(data);
                logger.debug("Swaped Data read from ME:\n" + Arrays.toString(data));

                int last = data.length - OTP_ROW_BYTE_SIZE;
                for (int i = 0; i <= last; i += OTP_ROW_BYTE_SIZE) {
                    int index = data[i + 3] & 0xFF;
                    index |= (data[i + 2] & 0xFF) << 8;
                    try {
                        if (readIndex[index]) {
                            buffer.put(data, i, OTP_ROW_BYTE_SIZE);
                        } else {
                            logger.debug("Ignored OTP row index: " + index);
                        }
                    } catch (IndexOutOfBoundsException e) {
                        logger.error(e.getMessage(), e);

                        resultCode = ERROR;
                        resultMessage = "OTP row index out of range: " + index;

                        return;
                    }
                }

                int remaining = buffer.remaining();
                buffer.clear();
                data = new byte[buffer.capacity() - remaining];
                buffer.get(data, 0, data.length);
                logger.debug("Data to be sent over BRP connection:\n" + Arrays.toString(data));

                resultMessage = Base64.encode(data);
            } else {
                ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                    .getLoaderErrorDesc(resultCode);
            }
        } else {
            super.updateResult(resultObj);
        }
    }

}
