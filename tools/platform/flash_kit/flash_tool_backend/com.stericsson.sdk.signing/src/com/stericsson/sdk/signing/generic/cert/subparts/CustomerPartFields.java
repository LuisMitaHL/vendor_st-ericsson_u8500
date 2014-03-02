package com.stericsson.sdk.signing.generic.cert.subparts;

/**
 * 
 * @author TSIKOR01
 * 
 */
public enum CustomerPartFields {

    /**
     * 
     */
    MAGIC(1, 4),

    /**
     * 
     */
    KEY_LIST(2, 4),

    /**
     * 
     */
    NUMBER_OF_KEYS_IN_LIST(3, 4),

    /**
     * 
     */
    MINOR_BUILD_VERSION(4, 2),

    /**
     * 
     */
    MAJOR_BUILD_VERSION(5, 2),

    /**
     * 
     */
    UTC_TIME(6, 4),

    /**
     * 
     */
    FLAGS(7, 4),

    /**
     * 
     */
    GENERIC_SIGNING_CONSTRAINTS(8, 16),

    /**
     * 
     */
    EXTENDED_SIGNING_CONSTRAINTS(8, 64),

    /**
     * 
     */
    RESERVED_BYTES(9, 16);

    private int size;

    private int position;

    CustomerPartFields(int pPosition, int pSize) {
        this.position = pPosition;
        this.size = pSize;
    }

    /**
     * Position in the sequence
     * 
     * @return position
     */
    public int getPosition() {
        return position;
    }

    /**
     * Gets size of the field
     * 
     * @return size of the field
     */
    public int getSize() {
        return size;
    }
}
