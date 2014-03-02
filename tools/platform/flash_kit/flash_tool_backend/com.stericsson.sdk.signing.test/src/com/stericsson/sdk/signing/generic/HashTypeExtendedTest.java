package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

import junit.framework.TestCase;


/**
 * @author Xxvs0002
 *
 */
public class HashTypeExtendedTest extends TestCase{

    private static final int SHA1_HASH_TYPE = GenericCertificate.SHA1_HASH_TYPE;
    private static final int SHA256_HASH_TYPE = GenericCertificate.SHA256_HASH_TYPE;
    private static final int SHA384_HASH_TYPE = GenericCertificate.SHA384_HASH_TYPE;
    private static final int SHA512_HASH_TYPE = GenericCertificate.SHA512_HASH_TYPE;

    private static final int SHA1_HMAC_TYPE = GenericCertificate.SHA1_HMAC_TYPE;
    private static final int SHA256_HMAC_TYPE = GenericCertificate.SHA256_HMAC_TYPE;
    private static final int SHA384_HMAC_TYPE = GenericCertificate.SHA384_HMAC_TYPE;
    private static final int SHA512_HMAC_TYPE = GenericCertificate.SHA512_HMAC_TYPE;

    /**
     * 
     */
    public void testHashTypeExtended(){
        assertEquals(HashTypeExtended.SHA1_HASH, HashTypeExtended.getByHashId(1));
        assertNull(HashTypeExtended.getByHashId(-1));

        assertEquals(HashTypeExtended.SHA256_HASH, HashTypeExtended.getByTypeValue(SHA256_HASH_TYPE));
        assertNull(HashTypeExtended.getByTypeValue(-1));

        assertEquals(20, HashTypeExtended.getHashSize(SHA1_HASH_TYPE));
        assertEquals(20, HashTypeExtended.getHashSize(SHA1_HMAC_TYPE));

        assertEquals(32, HashTypeExtended.getHashSize(SHA256_HMAC_TYPE));

        assertEquals(48, HashTypeExtended.getHashSize(SHA384_HASH_TYPE));
        assertEquals(48, HashTypeExtended.getHashSize(SHA384_HMAC_TYPE));

        assertEquals(64, HashTypeExtended.getHashSize(SHA512_HASH_TYPE));
        assertEquals(64, HashTypeExtended.getHashSize(SHA512_HMAC_TYPE));
    }
}
