package com.stericsson.sdk.signing.generic;

/**
 * @author xtomzap
 * 
 */
public enum SignatureType {

    /** Supported signature types */
    RSASSA_PKCS_V1_5(0x01), RSASSA_PPS(0x02), ELGAMAL(0x04), ECDSA(0x08), DSA(0x10);

    private int signatureType;

    private SignatureType(int type) {
        signatureType = type;
    }

    /** @return signature type */
    public int getSignatureType() {
        return signatureType;
    }

    /**
     * @param value signature value
     * @return signature type
     */
    public static SignatureType getByValue(int value) {
        for (SignatureType type : SignatureType.values()) {
            if (type.getSignatureType() == value) {
                return type;
            }
        }
        return null;
    }
}
