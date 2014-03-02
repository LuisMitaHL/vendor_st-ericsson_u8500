package com.stericsson.sdk.signing.data;

import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;

/**
 * Represents data of a one key contained in an ISSW certificate. Used for ISSW certificate
 * creation.
 * 
 * @author TSIKOR01
 * 
 */
public class KeyData {

    /**
     * Key file path.
     */
    private String path;

    /**
     * Key type.
     */
    private SignatureType type;

    /**
     * Revocation hashes. (bit 8-15 of keyType)
     */
    private int hashes = 0;

    /**
     * Class constructor.
     * 
     * @param pPath
     *            Key file path.
     * @param pType
     *            Key type.
     */
    public KeyData(String pPath, SignatureType pType) {
        this.path = pPath;
        this.type = pType;
        this.hashes = 0;
    }

     /**
     * Class constructor.
     * 
     * @param pPath
     *            Key file path.
     * @param pType
     *            Key type.
     * @param pHashes
     *            Revocation hashes.
     */
    public KeyData(String pPath, SignatureType pType, int pHashes) {
        this.path = pPath;
        this.type = pType;
        this.hashes = pHashes;
    }

    /**
     * @return Key file path.
     */
    public String getPath() {
        return path;
    }

    /**
     * @param value
     *            Key file path.
     */
    public void setPath(String value) {
        this.path = value;
    }

    /**
     * @return Key type.
     */
    public SignatureType getType() {
        return type;
    }

    /**
     * @param value
     *            Key type.
     */
    public void setType(SignatureType value) {
        this.type = value;
    }

    /**
     * @return Revocation hashes.
     */
    public int getHashes() {
        return hashes;
    }

    /**
     * @param value
     *            Revocation hashes.
     */
    public void setHashes(int value) {
        this.hashes = value;
    }

    /**
     * @return signature type and revocation hash types.
     */
    public SignatureTypeWithRevocationHashTypes getTypeWithRevocationHashTypes() {
        SignatureTypeWithRevocationHashTypes signatureTypeWithRevocationHashTypes = new SignatureTypeWithRevocationHashTypes();
        signatureTypeWithRevocationHashTypes.setSignatureType(type);
        signatureTypeWithRevocationHashTypes.setRevocationHashTypes(hashes);
        return signatureTypeWithRevocationHashTypes;
    }

}
