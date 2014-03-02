package com.stericsson.sdk.signing.generic;

/**
 * @author xxvs0005
 * 
 */
public class SignatureTypeWithRevocationHashTypes {

    private SignatureType signatureType;

    private int revocationHashTypes = 0;

    /**
     * Constructor
     * 
     */
    public SignatureTypeWithRevocationHashTypes() {

    }

    /**
     * Constructor
     * @param pSignatureType type
     * @param pRevocationHashTypes revocation hash types
     * 
     */
    public SignatureTypeWithRevocationHashTypes(SignatureType pSignatureType,
            int pRevocationHashTypes) {
        super();
        this.signatureType = pSignatureType;
        this.revocationHashTypes = pRevocationHashTypes;
    }

    /**
     * Constructor
     * @param pSignatureType type
     * 
     */
    public SignatureTypeWithRevocationHashTypes(SignatureType pSignatureType) {
        super();
        this.signatureType = pSignatureType;
    }
    /**
     * @return signature type
     * 
     */
    public SignatureType getSignatureType() {
        return signatureType;
    }

    /**
     * @param pSignatureType type
     * 
     */
    public void setSignatureType(SignatureType pSignatureType) {
        this.signatureType = pSignatureType;
    }

    /**
     * @return revocation hash types
     * 
     */
    public int getRevocationHashTypes() {
        return revocationHashTypes;
    }

    /**
     * @param pRevocationHashTypes revocation hash types
     * 
     */
    public void setRevocationHashTypes(int pRevocationHashTypes) {
        this.revocationHashTypes = pRevocationHashTypes;
    }

}
