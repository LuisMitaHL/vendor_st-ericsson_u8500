package com.stericsson.sdk.equipment.tasks;

import java.io.File;
import java.io.FileOutputStream;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;

/**
 * Task for reading OTP data from specified ME and writing it to specified file.
 * 
 * @author pkutac01
 * 
 */
public class EquipmentReadOTPTask extends EquipmentReadOTPDataTask {

    private static final int PATH_ARGUMENT_ID = 2;

    private static final int OTP_ROW_BYTE_SIZE = 8;

    private static final int OTP_ROW_COUNT = 46;

    private String filePath;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment instance.
     */
    public EquipmentReadOTPTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.OTP_READ_OTP.name();
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        filePath = pArguments[PATH_ARGUMENT_ID];

        // Add indexes to OTP meta-data.
        byte[] data = new byte[OTP_ROW_COUNT * OTP_ROW_BYTE_SIZE];
        for (int i = 0; i < OTP_ROW_COUNT; i++) {
            data[(i * OTP_ROW_BYTE_SIZE) + 2] = (byte) (i >> 8);
            data[(i * OTP_ROW_BYTE_SIZE) + 3] = (byte) (i);
        }

        super.setArguments(new String[] {
            pArguments[0], pArguments[1], Base64.encode(data)});
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    protected void updateResult(Object resultObj) {
        super.updateResult(resultObj);

        if (resultCode == ERROR_NONE) {
            try {
                writeOTPData(filePath, Base64.decode(resultMessage));
                resultMessage =
                    ((AbstractLoaderCommunicationEquipment) getEquipment()).getLoaderCommunicationService()
                        .getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                resultCode = ERROR;
                resultMessage = e.getMessage();
            }
        }
    }

    private void writeOTPData(String pFilePath, byte[] pData) throws Exception {
        File file = new File(pFilePath);
        FileOutputStream fos = null;

        try {
            fos = new FileOutputStream(file);
            fos.write(pData);
            fos.flush();
        } finally {
            if (fos != null) {
                fos.close();
            }
        }
    }

}
