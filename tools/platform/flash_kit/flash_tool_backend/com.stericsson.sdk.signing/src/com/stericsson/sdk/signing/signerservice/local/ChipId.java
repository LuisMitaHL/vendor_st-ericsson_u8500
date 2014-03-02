package com.stericsson.sdk.signing.signerservice.local;

/**
 * ChipId object contains the (secret) chipkey related to this chipid
 * 
 * @author qjankma
 */

public class ChipId {

    private int mChipId; // containing the id of this chipid

    private String mChipAlias; // containing user friendly alias name for chip ID

    private int[] mEncryptionKey; // containing the secret key

    /**
     * Constructor
     * 
     * @param chipid
     *            chip ID
     */

    public ChipId(int chipid) {
        mChipId = chipid;
    }

    /**
     * Adds the encryptionKey
     * 
     * @param encryptionKey
     *            encryption key
     */

    public void addEncryptionKey(int[] encryptionKey) {
        mEncryptionKey = encryptionKey;
    }

    /**
     * Set the chip alias name
     * 
     * @param alias
     *            chip alias name
     */
    public void setAlias(String alias) {
        mChipAlias = alias;
    }

    /**
     * Get the encryptionkey assosiated with this chipID
     * 
     * @return encryptionkey
     */

    public int[] getEncryptionKey() {
        return mEncryptionKey;
    }

    /**
     * Get the id of this chipid
     * 
     * @return the id
     */

    public int getId() {
        return mChipId;
    }

    /**
     * Get the alias for this chipid
     * 
     * @return the alias
     */
    public String getAlias() {
        return mChipAlias;
    }
}
