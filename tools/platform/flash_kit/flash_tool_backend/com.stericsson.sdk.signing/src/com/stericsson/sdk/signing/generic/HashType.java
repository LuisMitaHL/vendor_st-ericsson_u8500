package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

/**
 * @author xtomzap
 *
 */
public enum HashType {

    /** Supported hash types  */
    SHA1_HASH(GenericCertificate.SHA1_HASH_TYPE), SHA256_HASH(GenericCertificate.SHA256_HASH_TYPE),
    /** Supported hash types  */
    SHA384_HASH(GenericCertificate.SHA384_HASH_TYPE), SHA512_HASH(GenericCertificate.SHA512_HASH_TYPE),
    /** Supported hash types  */
    SHA1_HMAC(GenericCertificate.SHA1_HMAC_TYPE), SHA256_HMAC(GenericCertificate.SHA256_HMAC_TYPE),
    /** Supported hash types  */
    SHA384_HMAC(GenericCertificate.SHA384_HMAC_TYPE), SHA512_HMAC(GenericCertificate.SHA512_HMAC_TYPE);


    private int hashType;

    private HashType(int type) {
        hashType = type;
    }

    /** @return hash type */
    public int getHashType() {
        return hashType;
    }

    /**
     * 
     * @param hashType
     *            hash type
     * @return the size of the hash
     */
    public static int getHashSize(int hashType) {
        int size;
        switch (hashType) {
            case GenericCertificate.SHA1_HASH_TYPE:
            case GenericCertificate.SHA1_HMAC_TYPE:
                size = 20;
                break;
            case GenericCertificate.SHA256_HASH_TYPE:
            case GenericCertificate.SHA256_HMAC_TYPE:
                size = 32;
                break;
            case GenericCertificate.SHA384_HASH_TYPE:
            case GenericCertificate.SHA384_HMAC_TYPE:
                size = 48;
                break;
            case GenericCertificate.SHA512_HASH_TYPE:
            case GenericCertificate.SHA512_HMAC_TYPE:
                size = 64;
                break;
            default:
                size = 0;
                break;
        }
        return size;
    }

    /**
     * @param value of hash type
     * @return correct hash type or null
     */
    public static HashType getByValue(int value) {
        for (HashType type : HashType.values()) {
            if (type.getHashType() == value) {
                return type;
            }
        }
        return null;
    }
}
