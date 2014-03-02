package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.io.IOException;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.test.Activator;
import com.stericsson.sdk.signing.util.PEMParser;


/**
 * @author Xxvs0002
 *
 */
public class U5500EncrypterTest extends TestCase{

    private RSAPrivateKey key;

    private U5500Encrypter u5500Encrypter = new U5500Encrypter();

    private byte[] data = new byte[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    private static final String PRIVATE_PEM_KEY = "/u5500/1024_priv.pem";

    private RSAPrivateKey fromPriv = null;

    /**
     * 
     */
    @Before
    public void setUp(){
        key = new RSAPrivateKey() {
            /**
             * 
             */
            private static final long serialVersionUID = 1L;

            public BigInteger getModulus() {
                return new BigInteger("123456789");
            }

            public String getFormat() {
                return "PKCS#1";
            }

            public byte[] getEncoded() {
                return null;
            }

            public String getAlgorithm() {
                return "RSA";
            }

            public BigInteger getPrivateExponent() {
                return new BigInteger("987654321");
            }
        };

        String privKey = Activator.getResourcesPath() + PRIVATE_PEM_KEY;
        try {
            fromPriv = PEMParser.parsePrivateKey(privKey);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterSHA384HASH(){
        try {

            byte[] dataByte = u5500Encrypter.calculateU5500Signature(data, fromPriv, HashType.SHA384_HASH.getHashType());

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterSHA1HASH(){
        try {

            byte[] dataByte = u5500Encrypter.calculateU5500Signature(data, fromPriv, HashType.SHA1_HASH.getHashType());

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterSHA256HASH(){
        try {

            byte[] dataByte = u5500Encrypter.calculateU5500Signature(data, fromPriv, HashType.SHA256_HASH.getHashType());

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterSHA512HASH(){
        try {

            byte[] dataByte = u5500Encrypter.calculateU5500Signature(data, fromPriv, HashType.SHA512_HASH.getHashType());

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterThrowSignerServiceException(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500Signature(data, key, HashType.SHA384_HASH.getHashType());

            assertNotNull(dataByte);

            fail("Should not get here");
        } catch (SignerServiceException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA1HASH(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA1_HASH_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA1HMAC(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA1_HMAC_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA256HASH(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA256_HASH_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA256HMAC(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA256_HMAC_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA384HASH(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA384_HASH_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA384HMAC(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA384_HMAC_TYPE);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA512HASHThrowSignerServiceException(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA512_HASH_TYPE);

            assertNotNull(dataByte);
            fail("Should not get here");
        } catch (SignerServiceException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHA512HMACThrowSignerServiceException(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, fromPriv, GenericCertificate.SHA512_HMAC_TYPE);

            assertNotNull(dataByte);
            fail("Should not get here");
        } catch (SignerServiceException e) {
            e.printStackTrace();
        }
    }

    /**
     * 
     */
    @Test
    public void testU5500EncrypterPSSSHADefault(){
        try {
            byte[] dataByte = u5500Encrypter.calculateU5500PSSSignature(data, 
                    fromPriv, GenericCertificate.MAGIC_EXTENDED_CUSTOMER_PART);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }
}
