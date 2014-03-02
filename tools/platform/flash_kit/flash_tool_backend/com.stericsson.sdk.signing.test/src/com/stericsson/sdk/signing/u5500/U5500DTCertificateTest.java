package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericDTCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;

/**
 * @author xtomzap
 * 
 */
public class U5500DTCertificateTest extends TestCase {

    private GenericDTCertificate cert;

    /** */
    public void testSetData() {
        Random random = new Random();
        byte[] pSignature = new byte[32];
        random.nextBytes(pSignature);

        cert = new GenericDTCertificate();

        cert.setDTCertificateSize(99);
        cert.setSignatureSize(32);
        cert.setSignatureHashType(HashType.SHA256_HASH);
        cert.setSerialNumber(99);
        cert.setType(99);

        cert.setLevel((byte) 99);
        cert.setSecureDebug((byte) 99);
        cert.setApeDebug((byte) 99);
        cert.setModemDebug((byte) 99);
        cert.setPRCMUDebug((byte) 99);
        cert.setSTM((byte) 0x01);
        cert.setSpare(new byte[] {
            1, 2});

        cert.setReferenceValue(new byte[] {
            1, 2, 3});
        cert.addKeyToReplace(new GenericISSWCustomerPartKey(), 9);
        cert.setSignature(pSignature);

        byte[] data1 = cert.getData();
        try {
            cert.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        byte[] data2 = cert.getData();

        assertTrue(Arrays.equals(data1, data2));

        byte[] signableData = cert.getSignableData();
        byte[] signature = cert.getSignature();
        byte[] data3 = new byte[signableData.length + signature.length];
        System.arraycopy(signableData, 0, data3, 0, signableData.length);
        System.arraycopy(signature, 0, data3, signableData.length, signature.length);

        assertTrue(Arrays.equals(data3, data2));

    }

    /** */
    public void testGetMagic() {
        cert = new GenericDTCertificate();
        cert.setMagic(GenericCertificate.MAGIC_DT_CERTIFICATE);

        assertEquals(GenericCertificate.MAGIC_DT_CERTIFICATE, cert.getMagic());
    }

    /** */
    public void testGetDTCertificateSize() {
        cert = new GenericDTCertificate();
        cert.setDTCertificateSize(99);

        assertEquals(99, cert.getDTCertificateSize());
    }

    /** */
    public void testGetSerialNumber() {
        cert = new GenericDTCertificate();
        cert.setSerialNumber(99);

        assertEquals(99, cert.getSerialNumber());
    }

    /** */
    public void testGetType() {
        cert = new GenericDTCertificate();
        cert.setType(99);

        assertEquals(99, cert.getType());
    }

    /** */
    public void testGetLevel() {
        cert = new GenericDTCertificate();
        cert.setLevel((byte) 99);

        assertEquals(99, cert.getLevel());
    }

    /** */
    public void testGetSecureDebug() {
        cert = new GenericDTCertificate();
        cert.setSecureDebug((byte) 99);

        assertEquals(99, cert.getSecureDebug());
    }

    /** */
    public void testGetApeDebug() {
        cert = new GenericDTCertificate();
        cert.setApeDebug((byte) 99);

        assertEquals(99, cert.getApeDebug());
    }

    /** */
    public void testGetModemDebug() {
        cert = new GenericDTCertificate();
        cert.setModemDebug((byte) 99);

        assertEquals(99, cert.getModemDebug());
    }

    /** */
    public void testGetPRCMUDebug() {
        cert = new GenericDTCertificate();
        cert.setPRCMUDebug((byte) 99);
        assertEquals(99, cert.getPRCMUDebug());
    }

    /** */
    public void testGetSpare() {
        byte[] spareData = new byte[] {
            1, 2};
        cert = new GenericDTCertificate();
        cert.setSpare(spareData);

        assertTrue(Arrays.equals(spareData, cert.getSpare()));
    }

    /** */
    public void testGetSTM() {
        byte stmData = 0x01;
        cert = new GenericDTCertificate();
        cert.setSTM(stmData);

        assertTrue(stmData == cert.getSTM());
    }

    /** */
    public void testGetReferenceValue() {
        cert = new GenericDTCertificate();
        byte[] rData = new byte[] {
            1, 2, 3};
        cert.setReferenceValue(rData);

        assertTrue(Arrays.equals(rData, cert.getReferenceValue()));
        assertEquals(rData.length, cert.getReferenceLength());
    }

    /** */
    public void testGetSignature() {
        cert = new GenericDTCertificate();
        cert.setSignature(null);

        assertNotNull(cert.getSignature());

        byte[] signature = new byte[] {
            1, 2, 3};
        cert.setSignature(signature);

        assertEquals(signature, cert.getSignature());
    }

    /** */
    public void testGetHashType() {
        cert = new GenericDTCertificate();
        cert.setSignatureHashType(HashType.SHA256_HASH);

        assertEquals(HashType.SHA256_HASH.getHashType(), cert.getHashType());
    }
}
