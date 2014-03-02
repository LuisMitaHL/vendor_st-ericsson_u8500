package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.io.IOException;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;

import junit.framework.TestCase;

import org.junit.Before;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.test.Activator;
import com.stericsson.sdk.signing.util.PEMParser;


/**
 * @author Xxvs0002
 *
 */
public class U8500EncrypterTest extends TestCase{
    RSAPrivateKey key;

    U8500Encrypter u8500Encrypter = new U8500Encrypter();

    byte[] data = new byte[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    static final String PRIVATE_PEM_KEY = "/u5500/1024_priv.pem";

    RSAPrivateKey fromPriv = null;

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
    public void testU8500Encrypter(){
        try {

            byte[] dataByte = u8500Encrypter.calculateU8500Signature(data, fromPriv);

            assertNotNull(dataByte);
        } catch (SignerServiceException e) {
            e.printStackTrace();
            fail("Exception");
        }
    }

    /**
     * 
     */
    public void testU8500EncrypterThrowSignerServiceException(){
        try {

            byte[] dataByte = u8500Encrypter.calculateU8500Signature(data, key);

            assertNotNull(dataByte);
            fail("Should not get here");
        } catch (SignerServiceException e) {
            e.printStackTrace();
        }
    }
}
