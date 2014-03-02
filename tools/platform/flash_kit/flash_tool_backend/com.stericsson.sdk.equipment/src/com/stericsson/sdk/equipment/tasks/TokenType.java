package com.stericsson.sdk.equipment.tasks;

import java.io.IOException;
import java.util.Arrays;
import java.util.Locale;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.bootrom.BootRomTokens;
import com.stericsson.sdk.equipment.image.BinaryImage;
import com.stericsson.sdk.equipment.image.TableOfContentsItem;
import com.stericsson.sdk.equipment.io.AbstractPort;

/**
 * @author xtomzap
 * 
 */
public enum TokenType {

    /** Token for X-Loader */
    XLOADER(new byte[] {
        (byte) 0x00, (byte) 0x00, (byte) 0x40, (byte) 0xA0}, TableOfContentsItem.FILENAME_XLOADER),

    /** Token for Tee */
    TEE(new byte[] {
        (byte) 0x0B, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_TEE),

    /** Token for Power Management */
    PWR_MGT(new byte[] {
        (byte) 0x02, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_PWR_MGT),

    /** Token for Memory Init */
    MEM_INIT(new byte[] {
        (byte) 0x00, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_MEM_INIT),

    /** Token for Memory Init Passed Token */
    MEM_INIT_PASSED_TOKEN(new byte[] {
        (byte) 0x00, (byte) 0x00, (byte) 0x70, (byte) 0xD0}, "") {
        /**
         * {@inheritDoc}
         */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException {
            equipmentBootTask.notifyTaskMessage("Mem Init Passed Token received: "
                + HexUtilities.toHexString(getTokenValue(), 0, 4, 4, true));
        }
    },

    /** Token for Memory Init Failed Token */
    MEM_INIT_FAILED_TOKEN(new byte[] {
        (byte) 0x01, (byte) 0x00, (byte) 0x70, (byte) 0xD0}, "") {
        /**
         * {@inheritDoc}
         */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException {
            equipmentBootTask.notifyTaskMessage(TableOfContentsItem.FILENAME_MEM_INIT + " Failed Token received: "
                + HexUtilities.toHexString(getTokenValue(), 0, 4, 4, true));
            throw new EquipmentBootException(TableOfContentsItem.FILENAME_MEM_INIT + " execution failed.");
        }
    },

    /** Token for IPL */
    IPL(new byte[] {
        (byte) 0x03, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_IPL),

    /** Token for Modem Images */
    MODEM_IMAGES(new byte[] {
        (byte) 0x04, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_MODEM),

    /** Token for Normal */
    NORMAL(new byte[] {
        (byte) 0x01, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_NORMAL),

    /** Token for Debug and Trace */
    DNT(new byte[] {
        (byte) 0x05, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_DNT),

    /** Token for STE Debug and Trace */
    STE_DNT(new byte[] {
        (byte) 0x09, (byte) 0x00, (byte) 0x30, (byte) 0xA0}, TableOfContentsItem.FILENAME_STE_DNT),

    /** Token for Change Baud Rate Acknowledgment */
    CHANGE_BAUD_RATE_ACK_2_0(new byte[] {
        (byte) 0xBD, (byte) 0x00, (byte) 0x00, (byte) 0xAC}, "") {
        /** {@inheritDoc} */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException, IOException {
            // nothing to do here
        }
    },
    /** Token for Change Baud Rate Acknowledgment since selena(Jenny)(thorium) V2.1     */
    CHANGE_BAUD_RATE_ACK_2_1(new byte[] {
        (byte) 0xAC, (byte) 0x00, (byte) 0x00, (byte) 0xBD}, "") {
        /** {@inheritDoc} */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException, IOException {
            // nothing to do here
        }
    },
    /**
     * Token for Change Baud Rate Acknowledgment (incorrect, but received instead of the correct
     * one)
     */
    CHANGE_BAUD_RATE_ACK_2_0_INCORRECT(new byte[] {
        (byte) 0xBD, (byte) 0x00, (byte) 0x00, (byte) 0xFC}, "") {
        /** {@inheritDoc} */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException, IOException {
            // nothing to do here
        }
    },

    /**
     * Token for File Allocation String Extension
     */
    FILE_ALLOCATION_STRING_EXTENSION(new byte[] {
        (byte) 0x01, (byte) 0x00, (byte) 0x50, (byte) 0xA0}, "") {
        /**
         * {@inheritDoc}
         */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException, IOException {

            int expectedLength = 12;
            byte[] buffer = new byte[expectedLength];
            equipmentBootTask.readFully(port, buffer, 0, expectedLength);
            String fName = new String(buffer, "UTF-8").toUpperCase(Locale.getDefault()).trim();

            // handle debug message PRINTF:ON 
            // left separate for future introduction of functionality (check below)
            if (fName.equals("PRINTF:ON")) {
                return;
            }

            // handle debug message PRINTF:OFF
            // left separate for future introduction of functionality (check up)
            if (fName.equals("PRINTF:OFF")) {
                return;
            }

            // handle debug message from xloader
            if (fName.equals("PRINTF")) {
                // message length
                buffer = new byte[4];
                equipmentBootTask.readFully(port, buffer, 0, 4);
                int length = 0;
                for (int i = 0; i < buffer.length; i++) {
                    length += (int) (buffer[i] & 0xff) << (8 * i);
                }
                buffer = new byte[length];
                equipmentBootTask.readFully(port, buffer, 0, length);
                String message = new String(buffer, "UTF-8").toUpperCase(Locale.getDefault()).trim();
                equipmentBootTask.notifyTaskMessage("X-Loader DEBUG : " + message);
                return;
            }

            if (bootImage.getTOC().getItem(fName) == null) {
                throw new EquipmentBootException("File " + fName + " doesn't exist in TOC.");
            }

            writeBinary(equipmentBootTask, port, bootImage, fName);
        }
    },

    /** Unknown Token */
    UNKNOWN(new byte[] {}, "") {
        /** {@inheritDoc} */
        @Override
        public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
            throws EquipmentBootException, IOException {
            equipmentBootTask.notifyTaskMessage(BootRomTokens.getTokenDescription(getTokenValue()));
        }
    };

    private byte[] value;

    private String tocFileName;

    private boolean isLastToken = false;

    TokenType(byte[] tValue, String tTocFileName) {
        value = tValue;
        tocFileName = tTocFileName;
    }

    /**
     * @return byte representation of token
     */
    public byte[] getTokenValue() {
        return value;
    }

    /**
     * @return name of file in the TOC
     */
    public String getTocFileName() {
        return tocFileName;
    }

    /**
     * @param array
     *            byte representation of token
     */
    public void setTokenValue(byte[] array) {
        value = array;
    }

    /**
     * @param tLastToken
     *            true if next token should be read
     */
    public void setLastToken(boolean tLastToken) {
        isLastToken = tLastToken;
    }

    /**
     * @return true if next token should be read
     */
    public boolean isLastToken() {
        return isLastToken;
    }

    /**
     * Finds correct token instance by its byte representation
     * 
     * @param pValue
     *            given bytes
     * @return token instance
     */
    public static TokenType getTokenByValue(byte[] pValue) {
        for (TokenType token : TokenType.values()) {
            if (Arrays.equals(pValue, token.getTokenValue())) {
                return token;
            }
        }
        TokenType unknown = UNKNOWN;
        unknown.setTokenValue(pValue);
        return unknown;
    }

    /**
     * @param equipmentBootTask
     *            instance of boot task
     * @param port
     *            communication port
     * @param bootImage
     *            boot image
     * @param fileName
     *            name of which will be written to port
     * @throws IOException
     *             problem during reading/writing to port
     */
    protected void writeBinary(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage,
        String fileName) throws IOException {
        equipmentBootTask.notifyTaskMessage("Sending " + fileName);
        equipmentBootTask.writeBinary(fileName, port, bootImage);
        if (fileName.equalsIgnoreCase(TableOfContentsItem.FILENAME_NORMAL)) {
            setLastToken(true);
        }
    }

    /**
     * @param equipmentBootTask
     *            equipment boot task instance
     * @param port
     *            communication port
     * @param bootImage
     *            boot image
     * @throws EquipmentBootException
     *             problem during boot procedure
     * @throws IOException
     *             problem during reading/writing to port
     */
    public void handleToken(EquipmentBootTask equipmentBootTask, AbstractPort port, BinaryImage bootImage)
        throws EquipmentBootException, IOException {
        writeBinary(equipmentBootTask, port, bootImage, tocFileName);
    }
}
