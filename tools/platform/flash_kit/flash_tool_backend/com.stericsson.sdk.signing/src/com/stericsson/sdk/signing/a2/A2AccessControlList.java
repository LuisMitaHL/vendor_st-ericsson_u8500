package com.stericsson.sdk.signing.a2;

import com.stericsson.sdk.signing.cops.COPSControlFields;

/**
 * This class is ACL field in A2 header Access Control List (ACL) is a 4-byte field containing the
 * ACL_1 through ACL_4 Byte.
 * 
 * @author ezaptom
 * 
 */
public class A2AccessControlList {

    /** */
    public static final byte DEBUG_ON_ACCESS_APPLICATION_SIDE_DISABLE = 0x0;

    /** */
    public static final byte DEBUG_ON_ACCESS_APPLICATION_SIDE_ENABLE = 0x1;

    /** */
    public static final byte TARGET_CPU_ACCESS = 0x0;

    /** */
    public static final byte TARGET_CPU_APPLICATION = 0x1;

    /** */
    public static final byte TARGET_CPU_AUDIO = 0x2;

    /** */
    public static final byte SW_TYPE_BOOT_CODE_OR_SW_IMAGE = 0x0;

    /** */
    public static final byte SW_TYPE_LOADER = 0x1;

    /** */
    public static final byte ETX_LEVEL_DISABLE = 0x0;

    /** */
    public static final byte ETX_LEVEL_ENABLE = 0x1;

    /** */
    public static final byte ETX_LEVEL_IN_HEADER_DISABLE = 0x0;

    /** */
    public static final byte ETX_LEVEL_IN_HEADER_ENABLE = 0x1;

    /** */
    public static final byte INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC = 0x0;

    /** */
    public static final byte INTERACTIVE_LOAD_BIT_MODIFIED_INTERACTIVELY = 0x1;

    /** */
    public static final byte ASF_APPLICATION_SECURITY_OFF = 0x00;

    /** */
    public static final byte ASF_APPLICATION_SECURITY_ONLY_APPLIES_TO_DATA_LOADING = 0x01;

    /** */
    public static final byte ASF_APPLICATION_SECURITY_PAF_DEPENDENT = 0x02;

    /** */
    public static final byte ASF_APPLICATION_SECURITY_ALWAYS_ON = 0x03;

    /** */
    public static final byte PAYLOAD_TYPE_PHYSICAL_ADDRESS = 0x00;

    /** */
    public static final byte PAYLOAD_TYPE_NOR = 0x01;

    /** */
    public static final byte PAYLOAD_TYPE_NAND = 0x02;

    /** */
    public static final byte HEADER_ACTION_STORE = 0x01;

    /** */
    public static final byte HEADER_ACTION_STORE_REPLACE_SIGNATURE = 0x02;

    /** */
    public static final byte HEADER_ACTION_ONLY_VERIFY = 0x03;

    /***/
    public static final byte ADDRESS_FORMAT_PAGES_AND_4_BYTES = 0x0;

    /***/
    public static final byte ADDRESS_FORMAT_PAGES_ONLY = 0x1;

    /***/
    public static final byte SHORT_MAC_HEADER_ENABLE = 0x1;

    /***/
    public static final byte SHORT_MAC_HEADER_DISABLE = 0x0;

    /** */
    private byte payloadType = PAYLOAD_TYPE_PHYSICAL_ADDRESS;

    /** */
    private byte headerAction = HEADER_ACTION_STORE_REPLACE_SIGNATURE;

    /** */
    private byte nandAddressFormat = 0;

    /** */
    private byte shortMac = 0;

    /** */
    private byte debug = DEBUG_ON_ACCESS_APPLICATION_SIDE_DISABLE;

    /** */
    private byte targetCPU = TARGET_CPU_ACCESS;

    /** */
    private byte swType = SW_TYPE_BOOT_CODE_OR_SW_IMAGE;

    /** */
    private byte etxLevelRealTime = ETX_LEVEL_DISABLE;

    /** */
    private byte etxLevelHeaderUpdate = ETX_LEVEL_IN_HEADER_DISABLE;

    /** */
    private byte interactiveLoadBit = INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC;

    /** */
    private byte asfFlag = ASF_APPLICATION_SECURITY_ALWAYS_ON;

    /** */
    private int numberOfKeyblocks;

    /** */
    private int removeCertificateAndHashList;

    /** */
    private int trustedCode;

    /** */
    private int enableRadio;

    /** */
    private int serviceAbility;

    /** */
    private int payloadDecryption;

    /** */
    private int staticDataMACServiceMode;

    /** */
    private int domainDataMACServiceMode;

    /** */
    private int swMACServiceMode;

    /** */
    private int swMACNormalMode;

    /** */
    private int domainDataMACNormalMode;

    /** */
    private int staticDataMACNormalMode;

    /**
     * COPS version.
     */
    protected int headerID;

    /**
     * This class represents ACL field in A2 header
     * 
     * @param hID ,
     *            header version
     * @param value ,
     *            value
     */
    public A2AccessControlList(int hID, int value) {
        headerID = hID;

        if (headerID == COPSControlFields.HID_BABE0200 || headerID == COPSControlFields.HID_BABE0300) {

            // Get ACL 1:st byte
            setDebug((byte) ((value & 0x1)));
            setTargetCPU((byte) ((value & 0x2) >> 1));
            setSWType((byte) ((value & 0x4) >> 2));
            setETXLevel((byte) ((value & 0x8) >> 3));
            setETXLevelHeader((byte) ((value & 0x10) >> 4));
            setInteractiveLoadBit((byte) ((value & 0x20) >> 5));
            setApplicationSecurity((byte) ((value & 0xC0) >> 6));

            // Get ACL 2:nd byte
            value = value >> 8;
            setShortMac((byte) ((value & 0x1)));
            setPayloadType((byte) ((value & 0x18) >> 3));
            setHeaderAction((byte) ((value & 0x60) >> 5));
            setNandAddressFormat((byte) ((value & 0x80) >> 7));

            // Get ACL 3 byte
            value = value >> 16;
            setNumberOfKeyblocks((byte) (value & 0xff));
        }
    }

    /**
     * Constructor
     * 
     * @param hID
     *            header id
     */
    public A2AccessControlList(int hID) {
        headerID = hID;
    }

    /**
     * The returned byte array can be either 4 or eight bytes depending on the version (HID).
     * 
     * @return ACL field with all attributes as int
     */
    public int getValue() {

        int acl = 0;

        if (headerID == COPSControlFields.HID_BABE0300) {

            // ACL 1
            acl += targetCPU;
            acl += (headerAction << 2);
            acl += (removeCertificateAndHashList << 4);
            acl += (trustedCode << 5);
            acl += (enableRadio << 6);
            acl += (serviceAbility << 7);

            // ACL 2
            // ACL 3
            acl += (numberOfKeyblocks << 16);

            // ACL 4
            acl += (staticDataMACNormalMode << 24);
            acl += (domainDataMACNormalMode << 25);
            acl += (swMACNormalMode << 26);
            acl += (staticDataMACServiceMode << 27);
            acl += (domainDataMACServiceMode << 28);
            acl += (swMACServiceMode << 29);
            acl += (payloadDecryption << 30);

            // ACL 5
            // ACL 6
            // ACL 7
        } else {

            // Set ACL 3 byte
            acl += (numberOfKeyblocks << 16);

            // Set ACL 2:nd byte
            acl += (shortMac << 8);
            acl += (payloadType << 11);
            acl += (headerAction << 13);
            acl += (nandAddressFormat << 15);

            // Set ACL 1:st byte
            acl += (asfFlag << 6);
            acl += (interactiveLoadBit << 5);
            acl += (etxLevelHeaderUpdate << 4);
            acl += (etxLevelRealTime << 3);
            acl += (swType << 2);
            acl += (targetCPU << 1);
            acl += (debug);
        }

        return acl;
    }

    /**
     * Set debag value
     * 
     * @param value
     *            0 = Debug on access/application side disabled, 1 = Debug on access/application
     *            side enabled
     */
    public void setDebug(byte value) {
        if (value != DEBUG_ON_ACCESS_APPLICATION_SIDE_DISABLE && value != DEBUG_ON_ACCESS_APPLICATION_SIDE_ENABLE) {
            throw new IllegalArgumentException("Invalid ACL Debug bit: " + value);
        }
        debug = value;
    }

    /**
     * Set debag value
     * 
     * @param iValue
     *            0 = Debug on access/application side disabled, 1 = Debug on access/application
     *            side enabled
     */
    public void setDebug(int iValue) {
        byte bValue = 0;
        if (iValue == 1) {
            bValue = 1;
            setDebug(bValue);
        } else {
            setDebug(bValue);
        }
    }

    /**
     * Sets the target CPU, 0=Access, 1=Application, 2=Audio
     * 
     * @param value
     *            numeric value representing cpu
     */
    public void setTargetCPU(byte value) {
        if ((value == TARGET_CPU_AUDIO) && (headerID != 0xBABE0300)) {
            throw new IllegalArgumentException("Invalid ACL target CPU bit");
        } else {
            if ((value == TARGET_CPU_ACCESS) || (value == TARGET_CPU_APPLICATION) || (value == TARGET_CPU_AUDIO)) {
                targetCPU = value;
            } else {
                throw new IllegalArgumentException("Invalid ACL target CPU bit: " + value);
            }
        }
    }

    /**
     * Sets SW type
     * 
     * @param value
     *            0 = Boot code or Software, 1 = Loader
     */
    public void setSWType(byte value) {
        if (value != SW_TYPE_BOOT_CODE_OR_SW_IMAGE && value != SW_TYPE_LOADER) {
            throw new IllegalArgumentException("Invalid ACL SW type bit: " + value);
        }
        swType = value;
    }

    /**
     * Sets ETX level
     * 
     * @param value
     *            0 = ETX level in header disabled, 1 = ETX level in header enabled
     */
    public void setETXLevel(byte value) {
        if (value != ETX_LEVEL_ENABLE && value != ETX_LEVEL_DISABLE) {
            throw new IllegalArgumentException("Invalid ACL ETX level bit: " + value);
        }
        etxLevelRealTime = value;
    }

    /**
     * Sets ETX level header
     * 
     * @param value
     *            0 if disabled, 1 if enabled
     */
    public void setETXLevelHeader(byte value) {
        if (value != ETX_LEVEL_IN_HEADER_ENABLE && value != ETX_LEVEL_IN_HEADER_DISABLE) {
            throw new IllegalArgumentException("Invalid ACL ETX level header bit: " + value);
        }
        etxLevelHeaderUpdate = value;
    }

    /**
     * Sets interactive load bit
     * 
     * @param value
     *            0 = Load image is static. 1 = The load image is modified interactively and with
     *            secure identification of loading tool.
     */
    public void setInteractiveLoadBit(byte value) {
        if (value != INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC && value != INTERACTIVE_LOAD_BIT_MODIFIED_INTERACTIVELY) {
            throw new IllegalArgumentException("Invalid ACL interactive load bit: " + value);
        }
        interactiveLoadBit = value;
    }

    /**
     * Sets application security bits
     * 
     * @param value
     *            00 = Application security off. 01 = Application security only applies to data
     *            loading. 10 = Application security PAF dependent. 11 = Application security Always
     *            on
     */
    public void setApplicationSecurity(byte value) {
        if (value != ASF_APPLICATION_SECURITY_ALWAYS_ON && value != ASF_APPLICATION_SECURITY_OFF
            && value != ASF_APPLICATION_SECURITY_ONLY_APPLIES_TO_DATA_LOADING
            && value != ASF_APPLICATION_SECURITY_PAF_DEPENDENT) {
            throw new IllegalArgumentException("Invalid ACL application security bit: " + value);
        }
        asfFlag = value;
    }

    /**
     * @param sm
     *            TBD
     */
    public void setShortMac(byte sm) {
        shortMac = sm;
    }

    /**
     * Sets payload type bits
     * 
     * @param value
     *            00 = The header and payload will be linked to the targets physical address space.
     *            01 = Software header and payload built for NOR flash. Start address of the NOR
     *            flash media device will be zero. 10 = Software header and payload built for NAND
     *            flash. Start address of the NAND flash media device will be zero.
     */
    public void setPayloadType(byte value) {
        if (value != PAYLOAD_TYPE_PHYSICAL_ADDRESS && value != PAYLOAD_TYPE_NAND && value != PAYLOAD_TYPE_NOR) {
            throw new IllegalArgumentException("Invalid payload type value: " + value);
        }
        payloadType = value;
    }

    /**
     * Sets header actions (header security).
     * 
     * @param value
     *            01 = The header is stored in flash as it is. No replacement of the signature. 10 =
     *            The header signature is replaced before it is stored in flash. 11 = The header is
     *            verified during download and not stored in the flash device.
     */
    public void setHeaderAction(byte value) {
        if (value != HEADER_ACTION_ONLY_VERIFY && value != HEADER_ACTION_STORE
            && value != HEADER_ACTION_STORE_REPLACE_SIGNATURE) {
            throw new IllegalArgumentException("Invalid security header action value: " + value);
        }
        headerAction = value;
    }

    /**
     * Sets NAND address format.
     * 
     * @param pagesOnly
     *            0 = The data has pages + 4 bytes of redundant area. 1 = The data only has pages.
     *            No redundant data in the payload.
     */
    public void setNandAddressFormat(byte pagesOnly) {
        nandAddressFormat = pagesOnly;
    }

    /**
     * Sets number of key blocks
     * 
     * @param amountOfKeyblocks
     *            0000 = Payload NOT encrypted (No Keys). 0001 - 1111 = Nr Of Keys
     */
    public void setNumberOfKeyblocks(byte amountOfKeyblocks) {
        numberOfKeyblocks = amountOfKeyblocks;
    }
}
