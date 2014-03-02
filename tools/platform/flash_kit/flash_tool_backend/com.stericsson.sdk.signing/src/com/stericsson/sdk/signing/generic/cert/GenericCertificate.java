package com.stericsson.sdk.signing.generic.cert;

import com.stericsson.sdk.signing.AbstractByteSequence;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * @author xtomzap
 * 
 */
public abstract class GenericCertificate extends AbstractByteSequence {

    /** ISSW certificate magic number in generic format */
    public static final int MAGIC_ISSW_GENERIC = 0x4C554E44;

    /** ISSW certificate magic number in customer requested format */
    public static final int MAGIC_ISSW_CUSTOMER_REQUESTED = 0x49535357;

    /** Generic customer part magic number */
    public static final int MAGIC_GENERIC_CUSTOMER_PART = 0x54535543;

    /** Extended customer part magic number */
    public static final int MAGIC_EXTENDED_CUSTOMER_PART = 0x58535543;

    /** Signed Header magic number */
    public static final int MAGIC_SIGNED_HEADER = 0x53484452;

    /** Root key certificate magic number */
    public static final int MAGIC_ROOT_KEY_CERTIFICATE_HEADER = 0x6E6F794C;

    /** Customer root key certificate magic number */
    public static final int MAGIC_CUSTOMER_ROOT_KEY_CERTIFICATE_HEADER = 0x6E656741;

    /** ISSW certificate magic number to identify speedup structure */
    public static final int MAGIC_ISSW_SPEEDUP = 0x16793A22;

    /** Authentication certificate header */
    public static final int MAGIC_AUTHENTICATION_CERTIFICATE = 0x41435254;

    /** D&T certificate */
    public static final int MAGIC_DT_CERTIFICATE = 0x44435254;

    /** U8500 D&T certificate used for customer separation */
    public static final int MAGIC_U8500_DT_CERTIFICATE = 0x42435254;

    /* hash types */
    /** */
    public static final int SHA1_HASH_TYPE = 0x1;

    /** */
    public static final int SHA256_HASH_TYPE = 0x2;

    /** */
    public static final int SHA384_HASH_TYPE = 0x4;

    /** */
    public static final int SHA512_HASH_TYPE = 0x8;

    /** */
    public static final int SHA1_HMAC_TYPE = 0x10;

    /** */
    public static final int SHA256_HMAC_TYPE = 0x20;

    /** */
    public static final int SHA384_HMAC_TYPE = 0x40;

    /** */
    public static final int SHA512_HMAC_TYPE = 0x80;

    /** */
    public GenericCertificate() {
        super();
    }

    /**
     * 
     * @param data
     *            the payload hash
     */
    public abstract void setHash(byte[] data);

    /**
     * 
     * @return the signable part of the certificate
     */
    public abstract byte[] getSignableData();

    /**
     * 
     * @return the payload hash
     */
    public abstract byte[] getHash();

    /***
     * @param magic
     *            the magic number
     */
    public abstract void setMagic(int magic);

    /**
     * Get the certificate's magic value
     * 
     * @return the magic value
     */
    public abstract int getMagic();

    /**
     * 
     * @return the size of the signature
     */
    public abstract int getSignatureSize();

    /**
     * @param signerSettings signer settings
     * @return type of key
     * @throws SignerException thrown if type of key is not supported by sign tool
     */
    public SignatureType getTypeOfKey(ISignerSettings signerSettings) throws SignerException {
        return null;
    }

    /**
     * Updates fields in certificate structure
     * 
     * @param signerSettings sign settings
     * @param payload payload
     */
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {
        // default implementation
    }
}
