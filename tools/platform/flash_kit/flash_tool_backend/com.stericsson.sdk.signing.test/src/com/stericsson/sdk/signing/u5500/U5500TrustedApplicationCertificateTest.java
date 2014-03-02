package com.stericsson.sdk.signing.u5500;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.GenericTrustedApplicationCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericTrustedApplicationFunctionHeader;

/**
 * @author xtomzap
 * 
 */
public class U5500TrustedApplicationCertificateTest extends TestCase {

    private GenericTrustedApplicationCertificate certificate;

    /***/
    public void testSetData() {
        certificate = new GenericTrustedApplicationCertificate();

        Random random = new Random();
        byte[] teecUUID = new byte[16];
        random.nextBytes(teecUUID);

        certificate.setTeecUuid(teecUUID);
        certificate.setNumberOfTafs(2);
        certificate.setRoSize(16);
        certificate.setRwSize(16);
        certificate.setZiSize(2);
        certificate.setFlags(32);
        certificate.setHashType(HashType.SHA512_HASH);

        GenericTrustedApplicationFunctionHeader header = new GenericTrustedApplicationFunctionHeader();
        header.setID(1);
        header.setStart(0);

        certificate.addHeader(header);

        header = new GenericTrustedApplicationFunctionHeader();
        header.setID(2);
        header.setStart(4);

        certificate.addHeader(header);

        byte[] codeAndData = new byte[certificate.getRoSize() + certificate.getRwSize()];
        random.nextBytes(codeAndData);

        certificate.setCodeAndData(codeAndData);

        ArrayList<byte[]> hashList = new ArrayList<byte[]>();
        byte[] payloadHash = new byte[HashType.getHashSize(HashType.SHA512_HASH.getHashType())];
        random.nextBytes(payloadHash);
        hashList.add(payloadHash);
        random.nextBytes(payloadHash);
        hashList.add(payloadHash);

        certificate.setHashList(hashList);

        byte[] data1 = certificate.getData();
        certificate = new GenericTrustedApplicationCertificate();
        try {
            certificate.setData(data1);
        } catch (Exception e) {
            assertTrue(false);
        }
        byte[] data2 = certificate.getData();

        assertTrue(Arrays.equals(data1, data2));

        byte[] payloadData = certificate.getPayload();
        certificate.setCodeAndData(new byte[0]);
        byte[] data3 = certificate.getData();

        assertTrue(Arrays.equals(payloadData, data3));
    }

    /***/
    public void testGetTeecUuid() {
        certificate = new GenericTrustedApplicationCertificate();
        Random random = new Random();
        byte[] teecUUID = new byte[16];
        random.nextBytes(teecUUID);

        certificate.setTeecUuid(teecUUID);

        assertTrue(Arrays.equals(teecUUID, certificate.getTeecUuid()));
    }

    /***/
    public void testGetNumberOfTafs() {
        certificate = new GenericTrustedApplicationCertificate();
        certificate.setNumberOfTafs(32);

        assertEquals(32, certificate.getNumberOfTafs());
    }

    /***/
    public void testGetZiSize() {
        certificate = new GenericTrustedApplicationCertificate();
        certificate.setZiSize(32);

        assertEquals(32, certificate.getZiSize());
    }

    /***/
    public void testGetFlags() {
        certificate = new GenericTrustedApplicationCertificate();
        certificate.setFlags(32);

        assertEquals(32, certificate.getFlags());
    }

    /***/
    public void testGetCodeAndData() {
        certificate = new GenericTrustedApplicationCertificate();
        Random random = new Random();
        byte[] codeAndData = new byte[16];
        random.nextBytes(codeAndData);

        certificate.setCodeAndData(codeAndData);

        assertTrue(Arrays.equals(codeAndData, certificate.getCodeAndData()));
    }
}
