package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericRootKeyCertificate;

/**
 * @author xtomzap
 *
 */
public class U5500GenericRootKeyCertificateTest extends TestCase {

    private GenericRootKeyCertificate cert;

    private Random random = new Random();

    /**
     * Test method for
     * {@link com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate#setData(byte[])}
     * .
     */
    public void testSetData() {
        cert = new GenericRootKeyCertificate();

        try {
            cert.setData(null);
            assertTrue(false);
        } catch (Exception e) {
            e.getMessage();
        }
        try {
            cert.setData(getRandomBytes(832));
            assertTrue(false);
        } catch (Exception e) {
            e.getMessage();
        }

        cert = new GenericRootKeyCertificate();
        byte[] data1 = cert.getData();
        data1[0] = 0;
        data1[2] = 0;
        try {
            cert.setData(data1);
            fail("should not get here");
        } catch (Exception e) {
            e.getMessage();
        }

        cert = new GenericRootKeyCertificate();
        data1 = cert.getData();
        try {
            cert.setData(data1);
        } catch (Exception e) {
            e.printStackTrace();
            fail(e.getMessage());
        }
        assertTrue(Arrays.equals(data1, cert.getData()));

        cert.setMagic(GenericCertificate.MAGIC_ROOT_KEY_CERTIFICATE_HEADER);
        assertEquals(GenericCertificate.MAGIC_ROOT_KEY_CERTIFICATE_HEADER, cert.getMagic());
        assertEquals(cert.getSignatureSize(), 256);
        cert.setHash(new byte[]{});
        assertNull(cert.getHash());
        assertNotNull(cert.getSKSignatureType());
        assertNotNull(cert.getSKSignatureHashType());

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

    private byte[] getRandomBytes(int size) {
        byte[] bytes = new byte[size];

        random.nextBytes(bytes);
        return bytes;
    }
}
