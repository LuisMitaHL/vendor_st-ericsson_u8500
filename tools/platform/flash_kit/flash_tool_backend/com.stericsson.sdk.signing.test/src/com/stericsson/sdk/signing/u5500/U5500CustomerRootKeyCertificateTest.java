/**
 * 
 */
package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate;

/**
 * @author xolabju
 * 
 */
public class U5500CustomerRootKeyCertificateTest extends TestCase {

    private GenericCustomerRootKeyCertificate cert;

    private Random random = new Random();

    /**
     * Test method for
     * {@link com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate#setData(byte[])}
     * .
     */
    public void testSetData() {
        cert = new GenericCustomerRootKeyCertificate();

        try {
            cert.setData(null);
            assertTrue(false);
        } catch (Exception e) {
            e.getMessage();
        }
        try {
            cert.setData(getRandomBytes(23));
            assertTrue(false);
        } catch (Exception e) {
            e.getMessage();
        }

        cert = new GenericCustomerRootKeyCertificate();
        byte[] data1 = cert.getData();
        data1[0] = 0;
        data1[2] = 0;
        try {
            cert.setData(data1);
            fail("should not get here");
        } catch (Exception e) {
            e.getMessage();
        }

        cert = new GenericCustomerRootKeyCertificate();
        data1 = cert.getData();
        try {
            cert.setData(data1);
        } catch (Exception e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
        assertTrue(Arrays.equals(data1, cert.getData()));

        cert.setMagic(GenericCertificate.MAGIC_CUSTOMER_ROOT_KEY_CERTIFICATE_HEADER);
        cert.setCustomerRootKeySignatureType(SignatureType.RSASSA_PKCS_V1_5);
        cert.setCustomerRootKeySignatureHashType(HashType.SHA256_HASH);
        cert.setRootKeyCertificateData(getRandomBytes(832));

        data1 = cert.getData();

        try {
            cert.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        assertTrue(Arrays.equals(data1, cert.getData()));

        byte[] certAndSignature = new byte[data1.length + 256];
        System.arraycopy(data1, 0, certAndSignature, 0, data1.length);
        System.arraycopy(getRandomBytes(256), 0, certAndSignature, data1.length, 256);

        try {
            cert.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        assertTrue(Arrays.equals(data1, cert.getData()));
        assertFalse(Arrays.equals(certAndSignature, cert.getData()));
        assertEquals(data1.length, cert.getLength());

    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate#setHash(byte[])}
     * .
     */
    public void testSetHash() {
        cert = new GenericCustomerRootKeyCertificate();
        byte[] bytes = getRandomBytes(64);
        cert.setHash(bytes);
        assertTrue(Arrays.equals(bytes, cert.getHash()));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate#getSignableData()}
     * .
     */
    public void testGetSignableData() {
        cert = new GenericCustomerRootKeyCertificate();
        byte[] signableData = new byte[cert.getLength() - cert.getSignatureSize()];
        System.arraycopy(cert.getData(), 0, signableData, 0, signableData.length);
        assertTrue(Arrays.equals(signableData, cert.getSignableData()));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate#getSignatureSize()}
     * .
     */
    public void testGetSignatureSize() {
        cert = new GenericCustomerRootKeyCertificate();
        assertEquals(cert.getSignatureSize(), cert.getData().length - cert.getSignableData().length);
    }

//    private GenericKey getRandomKey() {
//        GenericKey rootKey = new GenericKey();
//        byte[] modulus = new byte[256];
//
//        random.nextBytes(modulus);
//
//        rootKey.setPublicExponent(6);
//        rootKey.setDummy((short) 4);
//        rootKey.setModulusSize((short) modulus.length);
//        rootKey.setModulus(modulus);
//
//        return rootKey;
//    }

    private byte[] getRandomBytes(int size) {
        byte[] bytes = new byte[size];

        random.nextBytes(bytes);
        return bytes;
    }

}
