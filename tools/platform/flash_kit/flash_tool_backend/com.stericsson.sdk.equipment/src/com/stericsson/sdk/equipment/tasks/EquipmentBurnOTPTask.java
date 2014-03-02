package com.stericsson.sdk.equipment.tasks;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.util.Base64;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;

/**
 * Task for burning OTP data provided in specified file to specified ME.
 * 
 * @author pkutac01
 * 
 */
public class EquipmentBurnOTPTask extends EquipmentBurnOTPDataTask {

    private static final int PATH_ARGUMENT_ID = 2;

    private String filePath;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment instance.
     */
    public EquipmentBurnOTPTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.OTP_BURN_OTP.name();
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        filePath = pArguments[PATH_ARGUMENT_ID];

        try {
            byte[] data = readOTPData(filePath);

            super.setArguments(new String[] {
                pArguments[0], pArguments[1], Base64.encode(data)});
        } catch (Exception e) {
            resultCode = ERROR;
            resultMessage = e.getMessage();
        }
    }

    private byte[] readOTPData(String pFilePath) throws Exception {
        File file = new File(pFilePath);
        byte[] data = new byte[(int) file.length()];
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(file);
            int read = fis.read(data);
            if (read != data.length) {
                throw new IOException("Read " + read + " bytes instead of " + data.length + " bytes from file '"
                    + file.getAbsolutePath() + "'.");
            }
        } finally {
            if (fis != null) {
                fis.close();
            }
        }

        return data;
    }
}
