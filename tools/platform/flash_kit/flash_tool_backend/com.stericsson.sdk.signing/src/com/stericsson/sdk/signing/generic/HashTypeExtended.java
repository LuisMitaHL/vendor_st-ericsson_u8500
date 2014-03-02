package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

/**
 * @author xxvs0005
 *
 */
public enum HashTypeExtended {

    /** Supported hash types  */
    SHA1_HASH(1,"1", GenericCertificate.SHA1_HASH_TYPE), 
    /** Supported hash types  */
    SHA256_HASH(2, "256", GenericCertificate.SHA256_HASH_TYPE),
    /** Supported hash types  */
    SHA384_HASH(3, "384", GenericCertificate.SHA384_HASH_TYPE), 
    /** Supported hash types  */
    SHA512_HASH(4, "512", GenericCertificate.SHA512_HASH_TYPE),
    /** Supported hash types  */
    SHA1_HMAC(5, "1", GenericCertificate.SHA1_HMAC_TYPE), 
    /** Supported hash types  */
    SHA256_HMAC(6, "256", GenericCertificate.SHA256_HMAC_TYPE),
    /** Supported hash types  */
    SHA384_HMAC(7, "384", GenericCertificate.SHA384_HMAC_TYPE), 
    /** Supported hash types  */
    SHA512_HMAC(8, "512", GenericCertificate.SHA512_HMAC_TYPE);


    private int hashId;
    /*
     * hashDigestLength 
     */
    private String hashDigest;
    private int hashType;

    private HashTypeExtended(int id, String digest, int type) {
        hashId = id;
        hashDigest = digest;
        hashType = type;
    }

    /** @return hash id */
    public int getHashId() {
        return hashId;
    }

    /** @return hash digest */
    public String getHashDigest() {
        return hashDigest;
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
     * @param id of hash id
     * @return correct hash or null
     */
    public static HashTypeExtended getByHashId(int id) {
        for (HashTypeExtended type : HashTypeExtended.values()) {
            if (type.getHashId() == id) {
                return type;
            }
        }
        return null;
    }

    /**
     * @param value of hash type
     * @return correct hash or null
     */
    public static HashTypeExtended getByTypeValue(int value) {
        for (HashTypeExtended type : HashTypeExtended.values()) {
            if (type.getHashType() == value) {
                return type;
            }
        }
        return null;
    }
}