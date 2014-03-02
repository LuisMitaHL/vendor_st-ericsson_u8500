package com.stericsson.sdk.signing.u8500;

import java.util.Arrays;
import java.util.Random;

import org.junit.Test;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.u8500.U8500DTCertificate;

import junit.framework.TestCase;

/**
 * @author Xxvs0002
 *
 */
public class U8500DTCertificateTest extends TestCase{

    private U8500DTCertificate certificate;

    /**
     * 
     */
    @Test
    public void testSetData(){
        Random random = new Random();
        byte[] pSignature = new byte[32];
        random.nextBytes(pSignature);

        certificate = new U8500DTCertificate();

        certificate.setDTCertificateSize(99);
        certificate.setSignatureSize(32);
        certificate.setSignatureHashType(HashType.SHA256_HASH);
        certificate.setSerialNumber(99);
        certificate.setType(99);

        certificate.setLevel((byte) 99);
        certificate.setSecureDebug((byte) 99);
        certificate.setApeDebug((byte) 99);
        certificate.setModemDebug((byte) 99);
        certificate.setPRCMUDebug((byte) 99);
        certificate.setSTM((byte) 0x01);
        certificate.setSpare(new byte[] {
            1, 2});

        certificate.setReferenceValue(new byte[] {
            1, 2, 3});
        certificate.addKeyToReplace(new GenericISSWCustomerPartKey(), 9);
        certificate.setSignature(pSignature);

        certificate.setReserved(new byte[]{1, 2, 3, 4});

        byte[] data1 = certificate.getData();
        try {
            certificate.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        byte[] data2 = certificate.getData();

        assertTrue(Arrays.equals(data1, data2));

        byte[] signableData = certificate.getSignableData();
        byte[] signature = certificate.getSignature();
        byte[] data3 = new byte[signableData.length + signature.length];
        System.arraycopy(signableData, 0, data3, 0, signableData.length);
        System.arraycopy(signature, 0, data3, signableData.length, signature.length);

        assertTrue(Arrays.equals(data3, data2));
    }

    /**
     * 
     */
    @Test
    public void testSetFlags(){
        certificate = new U8500DTCertificate();
        certificate.setFlags(1234);

        assertEquals(1234, certificate.getFlags());
    }

    /**
     * 
     */
    @Test
    public void testSetReserved(){
        certificate = new U8500DTCertificate();
        byte[] reserved = new byte[32];
        for(int i = 0; i < 4; i++){
            reserved[i] = (byte)(i+1);
        }
        certificate.setReserved(reserved);
        byte[] reservedTest = certificate.getReserved();

        assertTrue(Arrays.equals(reserved, reservedTest));
    }

}
