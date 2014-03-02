package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500SignedHeaderTest extends TestCase {

    GenericSignedHeader header;

    /**
     * 
     */
    public void testSetData() {
        header = new GenericSignedHeader();
        byte[] hash = new byte[32];
        Random r = new Random();
        r.nextBytes(hash);
        header.setHash(hash);
        header.setHashType(HashType.SHA256_HASH);
        header.setDMAAcceleration(true);
        header.setSpare(4);
        byte[] data1 = header.getData();
        try {
            header.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        byte[] data2 = header.getData();
        assertTrue(Arrays.equals(data1, data2));
    }

    /**
     * 
     */
    public void testSetHash() {
        header = new GenericSignedHeader();
        byte[] hash = new byte[32];
        Random r = new Random();
        r.nextBytes(hash);
        header.setHash(hash);

        byte[] hash2 = header.getHash();
        assertTrue(Arrays.equals(hash, hash2));
    }

    /**
     * 
     */
    public void testGetSignableData() {
        header = new GenericSignedHeader();
        int pre = header.getSignableData().length;
        byte[] hash = new byte[32];
        Random r = new Random();
        r.nextBytes(hash);
        header.setHash(hash);
        int post = header.getSignableData().length;
        assertEquals(hash.length, post - pre);
    }

    /**
     * 
     */
    public void testGetMagic() {
        header = new GenericSignedHeader();
        header.setMagic(12345678);
        assertEquals(12345678, header.getMagic());
    }

    /**
     * 
     */
    public void testSetSizeOfSignedHeader() {
        header = new GenericSignedHeader();
        short size = 12347;
        header.setSizeOfSignedHeader(size);
        assertEquals(size, header.getSizeOfSignedHeader());
    }

    /**
     * 
     */
    public void testSetSizeOfSignature() {
        header = new GenericSignedHeader();
        short size = 21333;
        header.setSizeOfSignature(size);
        assertEquals(size, header.getSizeOfSignature());
    }

    /**
     * 
     */
    public void testSetSignHashType() {
        header = new GenericSignedHeader();
        header.setSignHashType(HashType.SHA256_HASH);
        assertEquals(GenericCertificate.SHA256_HASH_TYPE, header.getSignHashType());
    }

    /**
     * 
     */
    public void testSetSignatureType() {
        header = new GenericSignedHeader();
        header.setSignatureType(SignatureType.RSASSA_PKCS_V1_5);
        assertEquals(1, header.getSignatureType());
    }

    /**
     * 
     */
    public void testSetHashType() {
        header = new GenericSignedHeader();
        header.setHashType(HashType.SHA1_HASH);
        assertEquals(GenericCertificate.SHA1_HASH_TYPE, header.getHashType());
    }

    /**
     * 
     */
    public void testSetPayloadType() {
        header = new GenericSignedHeader();
        header.setPayloadType(GenericSoftwareType.MEM_INIT);
        assertEquals(GenericSoftwareType.MEM_INIT.getSoftwareId(), header.getPayloadType());
    }

    /**
     * 
     */
    public void testSetFlags() {
        header = new GenericSignedHeader();
        header.setFlags(453453453);
        assertEquals(453453453, header.getFlags());
    }

    /**
     * 
     */
    public void testSetSwVersion() {
        header = new GenericSignedHeader();
        header.setSwVersion(456453245);
        assertEquals(456453245, header.getSwVersion());
    }

    /**
     * 
     */
    public void testSetLoadAddress() {
        header = new GenericSignedHeader();
        int intVal = 0xFFFFFFFF;
        long longVal = 0xFFFFFFFFL;
        header.setLoadAddress(longVal);
        assertEquals(intVal, header.getLoadAddress());
    }

    /**
     * 
     */
    public void testSetStartupAddress() {
        header = new GenericSignedHeader();
        int intVal = 0xFFFFFFFF;
        long longVal = 0xFFFFFFFFL;
        header.setStartupAddress(longVal);
        assertEquals(intVal, header.getStartupAddress());
    }

    /**
     * 
     */
    public void testSetSpare() {
        header = new GenericSignedHeader();
        header.setSpare(354532411);
        assertEquals(354532411, header.getSpare());
    }

    /**
     * 
     */
    public void testSetSignatureSize() {
        header = new GenericSignedHeader();
        short value = 31342;
        header.setSignatureSize(value);
        assertEquals(value, header.getSignatureSize());
    }

    /**
     * 
     */
    public void testSetDMAAcceleration() {
        header = new GenericSignedHeader();
        assertFalse(header.getDMAAcceleration());
        header.setDMAAcceleration(true);
        assertTrue(header.getDMAAcceleration());
        header.setDMAAcceleration(false);
        assertFalse(header.getDMAAcceleration());
    }

    /**
     * 
     */
    public void testSetPKAAcceleration() {
        header = new GenericSignedHeader();
        assertFalse(header.getPKAAcceleration());
        header.setPKAAcceleration(true);
        assertTrue(header.getPKAAcceleration());
        header.setPKAAcceleration(false);
        assertFalse(header.getPKAAcceleration());
    }

}
