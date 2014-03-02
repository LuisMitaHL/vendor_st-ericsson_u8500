package com.stericsson.sdk.signing.cops;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.util.HashMap;

import com.stericsson.sdk.signing.a2.A2AccessControlList;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.a2.A2SoftwareVersion;
import com.stericsson.sdk.signing.a2.IA2MacModeConstants;

/**
 * Control fields container used in COPS
 * 
 * @author tomas
 */
public class COPSControlFields {

    /** Header identifier */
    public static final int HID_BABE0200 = 0x0002BEBA;

    /** Header identifier */
    public static final int HID_BABE0300 = 0x0003BEBA;

    /** RSA 1024 with SHA-1 */
    public static final int PROT_TYPE_RSA_1024_SIG_WITH_SHA_1 = 0x01;

    /** RSA 2048 with SHA-1 */
    public static final int PROT_TYPE_RSA_2048_SIG_WITH_SHA_1 = 0x02;

    /** RSA 1024 with SHA-256 */
    public static final int PROT_TYPE_RSA_1024_SIG_WITH_SHA_256 = 0x101;

    /** RSA 2048 with SHA-256 */
    public static final int PROT_TYPE_RSA_2048_SIG_WITH_SHA_256 = 0x102;

    /** Length in bytes for signature using RSA 1024 */
    public static final int RSA_1024_SIGNATURE_LENGTH = 128;

    /** Length in bytes for signature using RSA 2048 */
    public static final int RSA_2048_SIGNATURE_LENGTH = 256;

    /** Length in bits for RSA 1024 key */
    public static final int RSA_1024_BIT_LENGTH = 1024;

    /** Length in bits for RSA 2048 key */
    public static final int RSA_2048_BIT_LENGTH = 2048;

    private static final int FIELD_HEADER_ID = 0; /* BABE0200 BABE0300 */

    private static final int FIELD_ACCESS_CONTROL_LIST = 1; /* BABE0200 BABE0300 */

    private static final int FIELD_CHIP_CLASS = 2; /* BABE0200 BABE0300 */

    private static final int FIELD_PROTECTION_TYPE = 3; /* BABE0200 BABE0300 */

    private static final int FIELD_RESERVED = 4; /* BABE0300 */

    private static final int FIELD_INITIALIZATION = 5; /* BABE0200 */

    private static final int FIELD_HASH_LIST_TYPE = 6; /* BABE0200 BABE0300 */

    private static final int FIELD_NUMBER_OF_CERTIFICATES = 7; /* BABE0200 BABE0300 */

    private static final int FIELD_CERTIFICATE_LIST_LENGTH = 8; /* BABE0200 BABE0300 */

    private static final int FIELD_CUSTOMER_ID = 9; /* BABE0200 BABE0300 */

    private static final int FIELD_KEY_USAGE = 10; /* BABE0200 BABE0300 */

    private static final int FIELD_MODE = 11; /* BABE0200 BABE0300 */

    private static final int FIELD_PAYLOAD_DESTINATION_ADDRESS = 12; /* BABE0200 BABE0300 */

    private static final int FIELD_SOFTWARE_LENGTH = 13; /* BABE0300 */

    private static final int FIELD_PAYLOAD_LENGTH = 14; /* BABE0200 BABE0300 */

    private static final int FIELD_PAYLOAD_TYPE = 15; /* BABE0200 BABE0300 */

    private static final int FIELD_SOFTWARE_VERSION = 16; /* BABE0200 BABE0300 */

    private static final int FIELD_HEADER_DESTINATION_ADDRESS = 17; /* BABE0200 */

    private static final int FIELD_CUSTOMER_ID_IN_OTP = 18; /* BABE0300 */

    private static final int FIELD_HEADER_LENGTH = 19; /* BABE0200 BABE0300 */

    private static final int BAB0200_FIELD_OFFSET_ACCESS_CONTROL_LIST = 4;

    private static final int BAB0200_FIELD_OFFSET_CHIP_CLASS = 8;

    private static final int BAB0200_FIELD_OFFSET_PROTECTION_TYPE = 12;

    private static final int BAB0200_FIELD_OFFSET_INITIALIZATION = 16;

    private static final int BAB0200_FIELD_OFFSET_HASH_LIST_TYPE = 28;

    private static final int BAB0200_FIELD_OFFSET_NUMBER_OF_CERTIFICATES = 40;

    private static final int BAB0200_FIELD_OFFSET_CERTIFICATE_LIST_LENGTH = 44;

    private static final int BAB0200_FIELD_OFFSET_CUSTOMER_ID = 48;

    private static final int BAB0200_FIELD_OFFSET_KEY_USAGE = 52;

    private static final int BAB0200_FIELD_OFFSET_MODE = 56;

    private static final int BAB0200_FIELD_OFFSET_PAYLOAD_DESTINATION_ADDRESS = 60;

    private static final int BAB0200_FIELD_OFFSET_PAYLOAD_LENGTH = 64;

    private static final int BAB0200_FIELD_OFFSET_PAYLOAD_TYPE = 68;

    private static final int BAB0200_FIELD_OFFSET_SOFTWARE_VERSION = 72;

    private static final int BAB0200_FIELD_OFFSET_HEADER_DESTINATION_ADDRESS = 76;

    private static final int BAB0200_FIELD_OFFSET_HEADER_LENGTH = 80;

    private static final int BAB0300_FIELD_OFFSET_ACCESS_CONTROL_LIST = 4;

    private static final int BAB0300_FIELD_OFFSET_CHIP_CLASS = 12;

    private static final int BAB0300_FIELD_OFFSET_PROTECTION_TYPE = 16;

    private static final int BAB0300_FIELD_OFFSET_RESERVED = 20;

    private static final int BAB0300_FIELD_OFFSET_HASH_LIST_TYPE = 32;

    private static final int BAB0300_FIELD_OFFSET_NUMBER_OF_CERTIFICATES = 44;

    private static final int BAB0300_FIELD_OFFSET_CERTIFICATE_LIST_LENGTH = 48;

    private static final int BAB0300_FIELD_OFFSET_CUSTOMER_ID = 52;

    private static final int BAB0300_FIELD_OFFSET_KEY_USAGE = 56;

    private static final int BAB0300_FIELD_OFFSET_MODE = 64;

    private static final int BAB0300_FIELD_OFFSET_PAYLOAD_DESTINATION_ADDRESS = 68;

    private static final int BAB0300_FIELD_OFFSET_SOFTWARE_LENGTH = 72;

    private static final int BAB0300_FIELD_OFFSET_PAYLOAD_LENGTH = 80;

    private static final int BAB0300_FIELD_OFFSET_PAYLOAD_TYPE = 88;

    private static final int BAB0300_FIELD_OFFSET_SOFTWARE_VERSION = 92;

    private static final int BAB0300_FIELD_OFFSET_CUSTOMER_ID_IN_OTP = 96;

    private static final int BAB0300_FIELD_OFFSET_HEADER_LENGTH = 100;

    private static final int BABE0200_CONTROL_FIELDS_SIZE = 84;

    private static final int BABE0300_CONTROL_FIELDS_SIZE = 104;

    private static final int MAX_CONTROL_FIELDS_SIZE = BABE0300_CONTROL_FIELDS_SIZE;

    private static final int SHA_1_HASH_LENGTH = 20;

    private static final int SHA_256_HASH_LENGTH = 32;

    private static HashMap<Integer, Integer> babe0200Offsets;

    private static HashMap<Integer, Integer> babe0300Offsets;

    static {
        babe0200Offsets = new HashMap<Integer, Integer>();

        babe0200Offsets.put(FIELD_HEADER_ID, 0);
        babe0200Offsets.put(FIELD_ACCESS_CONTROL_LIST, BAB0200_FIELD_OFFSET_ACCESS_CONTROL_LIST);
        babe0200Offsets.put(FIELD_CHIP_CLASS, BAB0200_FIELD_OFFSET_CHIP_CLASS);
        babe0200Offsets.put(FIELD_PROTECTION_TYPE, BAB0200_FIELD_OFFSET_PROTECTION_TYPE);
        babe0200Offsets.put(FIELD_INITIALIZATION, BAB0200_FIELD_OFFSET_INITIALIZATION);
        babe0200Offsets.put(FIELD_HASH_LIST_TYPE, BAB0200_FIELD_OFFSET_HASH_LIST_TYPE);
        babe0200Offsets.put(FIELD_NUMBER_OF_CERTIFICATES, BAB0200_FIELD_OFFSET_NUMBER_OF_CERTIFICATES);
        babe0200Offsets.put(FIELD_CERTIFICATE_LIST_LENGTH, BAB0200_FIELD_OFFSET_CERTIFICATE_LIST_LENGTH);
        babe0200Offsets.put(FIELD_CUSTOMER_ID, BAB0200_FIELD_OFFSET_CUSTOMER_ID);
        babe0200Offsets.put(FIELD_KEY_USAGE, BAB0200_FIELD_OFFSET_KEY_USAGE);
        babe0200Offsets.put(FIELD_MODE, BAB0200_FIELD_OFFSET_MODE);
        babe0200Offsets.put(FIELD_PAYLOAD_DESTINATION_ADDRESS, BAB0200_FIELD_OFFSET_PAYLOAD_DESTINATION_ADDRESS);
        babe0200Offsets.put(FIELD_PAYLOAD_LENGTH, BAB0200_FIELD_OFFSET_PAYLOAD_LENGTH);
        babe0200Offsets.put(FIELD_PAYLOAD_TYPE, BAB0200_FIELD_OFFSET_PAYLOAD_TYPE);
        babe0200Offsets.put(FIELD_SOFTWARE_VERSION, BAB0200_FIELD_OFFSET_SOFTWARE_VERSION);
        babe0200Offsets.put(FIELD_HEADER_DESTINATION_ADDRESS, BAB0200_FIELD_OFFSET_HEADER_DESTINATION_ADDRESS);
        babe0200Offsets.put(FIELD_HEADER_LENGTH, BAB0200_FIELD_OFFSET_HEADER_LENGTH);

        babe0300Offsets = new HashMap<Integer, Integer>();

        babe0300Offsets.put(FIELD_HEADER_ID, 0);
        babe0300Offsets.put(FIELD_ACCESS_CONTROL_LIST, BAB0300_FIELD_OFFSET_ACCESS_CONTROL_LIST);
        babe0300Offsets.put(FIELD_CHIP_CLASS, BAB0300_FIELD_OFFSET_CHIP_CLASS);
        babe0300Offsets.put(FIELD_PROTECTION_TYPE, BAB0300_FIELD_OFFSET_PROTECTION_TYPE);
        babe0300Offsets.put(FIELD_RESERVED, BAB0300_FIELD_OFFSET_RESERVED);
        babe0300Offsets.put(FIELD_HASH_LIST_TYPE, BAB0300_FIELD_OFFSET_HASH_LIST_TYPE);
        babe0300Offsets.put(FIELD_NUMBER_OF_CERTIFICATES, BAB0300_FIELD_OFFSET_NUMBER_OF_CERTIFICATES);
        babe0300Offsets.put(FIELD_CERTIFICATE_LIST_LENGTH, BAB0300_FIELD_OFFSET_CERTIFICATE_LIST_LENGTH);
        babe0300Offsets.put(FIELD_CUSTOMER_ID, BAB0300_FIELD_OFFSET_CUSTOMER_ID);
        babe0300Offsets.put(FIELD_KEY_USAGE, BAB0300_FIELD_OFFSET_KEY_USAGE);
        babe0300Offsets.put(FIELD_MODE, BAB0300_FIELD_OFFSET_MODE);
        babe0300Offsets.put(FIELD_PAYLOAD_DESTINATION_ADDRESS, BAB0300_FIELD_OFFSET_PAYLOAD_DESTINATION_ADDRESS);
        babe0300Offsets.put(FIELD_SOFTWARE_LENGTH, BAB0300_FIELD_OFFSET_SOFTWARE_LENGTH);
        babe0300Offsets.put(FIELD_PAYLOAD_LENGTH, BAB0300_FIELD_OFFSET_PAYLOAD_LENGTH);
        babe0300Offsets.put(FIELD_PAYLOAD_TYPE, BAB0300_FIELD_OFFSET_PAYLOAD_TYPE);
        babe0300Offsets.put(FIELD_SOFTWARE_VERSION, BAB0300_FIELD_OFFSET_SOFTWARE_VERSION);
        babe0300Offsets.put(FIELD_CUSTOMER_ID_IN_OTP, BAB0300_FIELD_OFFSET_CUSTOMER_ID_IN_OTP);
        babe0300Offsets.put(FIELD_HEADER_LENGTH, BAB0300_FIELD_OFFSET_HEADER_LENGTH);
    }

    private ByteBuffer fieldBuffer;

    /**
     * @param channel
     *            TBD
     * @throws IOException
     *             TBD
     */
    public COPSControlFields(ReadableByteChannel channel) throws IOException {

        fieldBuffer = ByteBuffer.allocate(MAX_CONTROL_FIELDS_SIZE);
        fieldBuffer.order(ByteOrder.LITTLE_ENDIAN);
        fieldBuffer.limit(4); // Limit first read to header id field

        channel.read(fieldBuffer);

        int headerID = fieldBuffer.getInt(0);

        if (headerID == HID_BABE0200) {
            fieldBuffer.limit(BABE0200_CONTROL_FIELDS_SIZE);
        } else if (headerID == HID_BABE0300) {
            fieldBuffer.limit(BABE0300_CONTROL_FIELDS_SIZE);
        } else {
            throw new IOException("Unknown COPS header version " + Integer.toHexString(headerID));
        }

        channel.read(fieldBuffer); // Read the rest of the control fields
    }

    /**
     * @param headerID
     *            TBD
     */
    public COPSControlFields(int headerID) {

        fieldBuffer = ByteBuffer.allocate(MAX_CONTROL_FIELDS_SIZE);
        fieldBuffer.order(ByteOrder.LITTLE_ENDIAN);

        if (headerID == HID_BABE0200) {
            fieldBuffer.limit(BABE0200_CONTROL_FIELDS_SIZE);
        } else if (headerID == HID_BABE0300) {
            fieldBuffer.limit(BABE0300_CONTROL_FIELDS_SIZE);
        }

        fieldBuffer.putInt(0, headerID);
    }

    /**
     * @param output
     *            TBD
     * @throws IOException
     *             TBD
     */
    public void write(WritableByteChannel output) throws IOException {
        fieldBuffer.rewind();
        output.write(fieldBuffer);
    }

    /**
     * Set header ID
     * 
     * @param headerID
     *            headerID
     */
    public void setHeaderID(int headerID) {
        fieldBuffer.putInt(getFieldOffset(headerID, FIELD_HEADER_ID), headerID);
    }

    /**
     * @return TBD
     */
    public int getLength() {

        int result = 0;

        if (getHeaderID() == HID_BABE0200) {
            result = BABE0200_CONTROL_FIELDS_SIZE;
        } else if (getHeaderID() == HID_BABE0300) {
            result = BABE0300_CONTROL_FIELDS_SIZE;
        }

        return result;
    }

    /**
     * @return TBD
     */
    public int getHeaderID() {
        return fieldBuffer.getInt(0);
    }

    /**
     * Set ACL field. ACL field in A2 header Access Control List (ACL) is a 4-byte field containing
     * the ACL_1 through ACL_4 Byte.
     * 
     * @param signerSettings
     *            signerSettings
     */
    public void setA2Acl(A2SignerSettings signerSettings) {

        A2AccessControlList aclField = new A2AccessControlList(getHeaderID());
        aclField.setDebug((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_DEBUG));
        aclField.setETXLevel((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_EXT_LEVEL));
        aclField.setETXLevelHeader((Byte) signerSettings
            .getSignerSetting(ICOPSSignerSettings.KEY_ACL_ETX_LEVEL_IN_HEADER));
        aclField.setTargetCPU((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU));
        aclField.setSWType((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE));
        aclField.setInteractiveLoadBit((Byte) signerSettings
            .getSignerSetting(ICOPSSignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT));
        aclField.setPayloadType((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE));
        aclField.setApplicationSecurity((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC));
        aclField.setHeaderAction((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_HDR_SECURITY));
        aclField.setNandAddressFormat((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_FORMAT));
        aclField.setShortMac((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_SHORT_MAC));

        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_ACCESS_CONTROL_LIST), aclField.getValue());

    }

    /**
     * Set SW_VERSION field. Software Version field in A2 Header is a 4-byte field that contains the
     * software version.
     * 
     * @param signerSettings
     *            signerSettings
     */
    public void setA2SoftwareVersion(A2SignerSettings signerSettings) {

        A2SoftwareVersion softwareVersion = new A2SoftwareVersion();

        softwareVersion.setCPU((Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU));
        softwareVersion.setMode((Integer) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE));
        softwareVersion.setAntiRollbackRequired((Integer) signerSettings
            .getSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_ANTI_ROLL_REQUIRED));
        softwareVersion.setSoftwareVersion((Integer) signerSettings
            .getSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION));

        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_SOFTWARE_VERSION), softwareVersion
            .getSoftwareVersion(getHeaderID()));
    }

    /**
     * Set "MODE" field in A2 header. The mode field contains the MAC calculation mode in A2. The
     * MAC mode is the sum of ACCESS or APPLICATION plus the suitable MAC type.
     * 
     * @param signerSettings
     *            signerSettings
     */
    public void setA2Mode(A2SignerSettings signerSettings) {

        byte cpu;
        byte mode;

        byte targetCPU = (Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU);

        if (targetCPU == A2AccessControlList.TARGET_CPU_ACCESS) {
            cpu = IA2MacModeConstants.ACCESS;
        } else {
            cpu = IA2MacModeConstants.APPLICATION;
        }

        mode = (Byte) signerSettings.getSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE);
        int res = 0;
        // for BABE0300 the MAC calculation mode is the only value that should be set
        if (getHeaderID() == COPSControlFields.HID_BABE0300) {
            res = mode;
        } else {
            res = cpu + mode;
        }

        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_MODE), res);
    }

    /**
     * Set "PL_TYPE" field in A2 header. PL_TYPE is a 4-byte field that contains the type of
     * payload.
     * 
     * @param signerSettings
     *            signerSettings
     */
    public void setA2PLType(A2SignerSettings signerSettings) {

        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_PAYLOAD_TYPE), (Integer) signerSettings
            .getSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE));
    }

    /**
     * Set HD_DEST_ADDRESS in A2 header. It's value depends on the ACL header security value.
     * HD_DEST_ADDRESS is a 4-byte field that contains the destination address where the header is
     * to be stored in flash. If the second byte of ACL has bits 5-6 set to 11 (verifyOnly) the
     * address is 0xFFFFEEEE.
     * 
     * @param signerSettings
     *            signerSettings
     */
    public void setA2HeaderDestinationAddress(A2SignerSettings signerSettings) {
        int headerID = getHeaderID();
        if (headerID == COPSControlFields.HID_BABE0200) {
            fieldBuffer.order(ByteOrder.BIG_ENDIAN);
            fieldBuffer.putInt(getFieldOffset(headerID, FIELD_HEADER_DESTINATION_ADDRESS), (Integer) signerSettings
                .getSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS));
            fieldBuffer.order(ByteOrder.LITTLE_ENDIAN);
        }
    }

    /**
     * @return TBD
     */
    public long getPayloadLength() {

        int headerID = fieldBuffer.getInt(0);
        long length = 0;

        if (headerID == HID_BABE0200) {
            length = fieldBuffer.getInt(getFieldOffset(HID_BABE0200, FIELD_PAYLOAD_LENGTH));
        } else if (headerID == HID_BABE0300) {
            length = fieldBuffer.getLong(getFieldOffset(HID_BABE0300, FIELD_PAYLOAD_LENGTH));
        }

        return length;
    }

    /**
     * @return The length (in bytes) of the certificate list
     */
    public int getCertificateListLength() {
        return fieldBuffer.getInt(getFieldOffset(fieldBuffer.getInt(0), FIELD_CERTIFICATE_LIST_LENGTH));
    }

    /**
     * @return TBD
     */
    public int getPaddingLength() {
        int headerLength = fieldBuffer.getInt(getFieldOffset(fieldBuffer.getInt(0), FIELD_HEADER_LENGTH));
        int paddingLength = headerLength % 8;
        if (paddingLength > 0) {
            paddingLength = 8 - paddingLength;
        }
        return paddingLength;
    }

    /**
     * @return TBD
     */
    public int getProtectionType() {
        return fieldBuffer.getInt(getFieldOffset(getHeaderID(), FIELD_PROTECTION_TYPE));
    }

    /**
     * @return The length (in bytes) of the signature
     */
    public int getSignatureLength() {

        int length = 0;
        int protectionType = getProtectionType();

        if ((protectionType == PROT_TYPE_RSA_1024_SIG_WITH_SHA_1)
            || (protectionType == PROT_TYPE_RSA_1024_SIG_WITH_SHA_256)) {
            length = RSA_1024_SIGNATURE_LENGTH;
        } else if ((protectionType == PROT_TYPE_RSA_2048_SIG_WITH_SHA_1)
            || (protectionType == PROT_TYPE_RSA_2048_SIG_WITH_SHA_256)) {
            length = RSA_2048_SIGNATURE_LENGTH;
        }

        return length;
    }

    /**
     * @return The length (in bytes) of the hash list
     */
    public int getHashListLength() {

        int hashLength = 0;
        int protectionType = getProtectionType();

        if ((protectionType == PROT_TYPE_RSA_1024_SIG_WITH_SHA_1)
            || (protectionType == PROT_TYPE_RSA_2048_SIG_WITH_SHA_1)) {
            hashLength = SHA_1_HASH_LENGTH;
        } else if ((protectionType == PROT_TYPE_RSA_1024_SIG_WITH_SHA_256)
            || (protectionType == PROT_TYPE_RSA_2048_SIG_WITH_SHA_256)) {
            hashLength = SHA_256_HASH_LENGTH;
        }

        return getHashListNumberOfBlocks() * hashLength;
    }

    /**
     * @return TBD
     */
    public int getHashListNumberOfBlocks() {
        return 0;
    }

    /**
     * @return The size (in bytes) of the block of data each hash list value is calculated on
     */
    public int getHashListBlockSize() {
        return 0;
    }

    /**
     * @return The type of hash list
     */
    public int getHashListType() {
        return 0;
    }

    /**
     * @param count
     *            TBD
     */
    public void setHashListNumberOfBlocks(int count) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_HASH_LIST_TYPE) + 4, count);
    }

    /**
     * @param size
     *            TBD
     */
    public void setHashListBlockSize(int size) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_HASH_LIST_TYPE), size);
    }

    /**
     * @param type
     *            TBD
     */
    public void setHashListType(int type) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_HASH_LIST_TYPE) + 8, type);
    }

    /**
     * @return The header length stored in the control fields
     */
    public int getHeaderLength() {
        return fieldBuffer.getInt(getFieldOffset(getHeaderID(), FIELD_HEADER_LENGTH));
    }

    /**
     * @return TBD
     */
    public byte[] getBytes() {
        byte[] fieldData = new byte[fieldBuffer.limit()];
        System.arraycopy(fieldBuffer.array(), 0, fieldData, 0, fieldData.length);
        return fieldData;
    }

    /**
     * @param headerID
     *            TBD
     * @param field
     *            TBD
     * @return TBD
     */
    private int getFieldOffset(int headerID, int field) {

        int offset = -1;

        if (headerID == HID_BABE0200) {
            offset = (Integer) babe0200Offsets.get(field);
        } else if (headerID == HID_BABE0300) {
            offset = (Integer) babe0300Offsets.get(field);
        }

        return offset;
    }

    /**
     * @param protectionType
     *            TBD
     */
    public void setProtectionType(int protectionType) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_PROTECTION_TYPE), protectionType);
    }

    /**
     * @param keyUsage
     *            TBD
     */
    public void setKeyUsage(byte[] keyUsage) {

        fieldBuffer.position(getFieldOffset(getHeaderID(), FIELD_KEY_USAGE));

        for (int i = keyUsage.length - 1; i >= 0; i--) {
            fieldBuffer.put(keyUsage[i]);
        }
    }

    /**
     * @param length
     *            TBD
     */
    public void setCertificateListLength(int length) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_CERTIFICATE_LIST_LENGTH), length);
    }

    /**
     * @param numberOfCertificates
     *            TBD
     */
    public void setNumberOfCertificates(int numberOfCertificates) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_NUMBER_OF_CERTIFICATES), numberOfCertificates);
    }

    /**
     * @param chipClass
     *            TBD
     */
    public void setChipClass(int chipClass) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_CHIP_CLASS), chipClass);
    }

    /**
     * @param cid
     *            TBD
     */
    public void setCustomerID(int cid) {
        fieldBuffer.putShort(getFieldOffset(getHeaderID(), FIELD_CUSTOMER_ID), (short) cid);
    }

    /**
     * @return The Customer id stored in the control fields
     */
    public int getCustomerID() {
        return fieldBuffer.getInt(getFieldOffset(getHeaderID(), FIELD_CUSTOMER_ID));
    }

    /**
     * @param length
     *            TBD
     */
    public void setHeaderLength(int length) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_HEADER_LENGTH), length);
    }

    /**
     * @param length
     *            TBD
     */
    public void setPayloadLength(long length) {
        int headerID = fieldBuffer.getInt(0);

        if (headerID == HID_BABE0200) {
            fieldBuffer.putInt(getFieldOffset(HID_BABE0200, FIELD_PAYLOAD_LENGTH), (int) length);
        } else if (headerID == HID_BABE0300) {
            fieldBuffer.putLong(getFieldOffset(HID_BABE0300, FIELD_PAYLOAD_LENGTH), length);
        }
    }

    /**
     * @param destinationAddress
     *            TBD
     */
    public void setPayloadDestinationAddress(long destinationAddress) {
        int headerID = fieldBuffer.getInt(0);

        fieldBuffer.order(ByteOrder.BIG_ENDIAN);

        if (headerID == HID_BABE0200) {
            fieldBuffer.putInt(getFieldOffset(headerID, FIELD_PAYLOAD_DESTINATION_ADDRESS), (int) destinationAddress);
        } else if (headerID == HID_BABE0300) {
            fieldBuffer.putLong(getFieldOffset(headerID, FIELD_PAYLOAD_DESTINATION_ADDRESS), destinationAddress);
        }

        fieldBuffer.order(ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * 
     * @param numberOfKeyBlocks
     *            TBD
     */
    public void setNumberOfKeyblocks(byte numberOfKeyBlocks) {
        int a2AclValue = fieldBuffer.getInt(getFieldOffset(getHeaderID(), FIELD_ACCESS_CONTROL_LIST));
        A2AccessControlList aclField = new A2AccessControlList(getHeaderID(), a2AclValue);
        aclField.setNumberOfKeyblocks(numberOfKeyBlocks);

        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_ACCESS_CONTROL_LIST), aclField.getValue());
    }

    /**
     * @param cid
     *            Customer ID to be found in OTP
     */
    public void setCustomerIDInOTP(int cid) {
        fieldBuffer.putInt(getFieldOffset(getHeaderID(), FIELD_CUSTOMER_ID_IN_OTP), cid);
    }

    /**
     * @param length
     *            Software length
     */
    public void setSoftwareLength(long length) {
        fieldBuffer.putLong(getFieldOffset(getHeaderID(), FIELD_SOFTWARE_LENGTH), length);
    }
}
