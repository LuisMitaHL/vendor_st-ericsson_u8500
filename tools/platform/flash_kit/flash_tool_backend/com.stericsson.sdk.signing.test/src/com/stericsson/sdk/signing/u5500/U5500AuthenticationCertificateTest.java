package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

/**
 * @author xtomzap
 * 
 */
public class U5500AuthenticationCertificateTest extends TestCase {

    GenericAuthenticationCertificate authCertificate;

    /***/
    public void testSetData() {
        authCertificate = new GenericAuthenticationCertificate();
        Random random = new Random();

        byte[] pSignature = new byte[32];
        random.nextBytes(pSignature);

        byte[] pModulus = new byte[32];
        random.nextBytes(pModulus);

        authCertificate.setAuthCertSize((short) 99);
        authCertificate.setSignatureSize((short) pSignature.length);
        authCertificate.setSignatureHashType(HashType.SHA256_HASH);
        authCertificate.setSignatureType(SignatureType.RSASSA_PKCS_V1_5);
        authCertificate.setSerialNumber(1);
        authCertificate.setNotBefore(2);
        authCertificate.setNotAfter(3);
        authCertificate.setAuthenticationLevel(4);
        authCertificate.setSpare(5);
        authCertificate.setKeySize(48);
        authCertificate.setKeyTypeWithRevocationHashTypes(
                new SignatureTypeWithRevocationHashTypes(SignatureType.RSASSA_PKCS_V1_5));
        authCertificate.setExponent(6);
        authCertificate.setDummy((short) 4);
        authCertificate.setModulusSize((short) pModulus.length);
        authCertificate.setModulus(pModulus);
        authCertificate.setSignature(pSignature);


        byte[] data1 = authCertificate.getData();
        try {
            authCertificate.setData(data1);
        } catch (Exception e) {
            fail(e.getMessage());
        }
        byte[] data2 = authCertificate.getData();

        assertTrue(Arrays.equals(data1, data2));

        byte[] signableData = authCertificate.getSignableData();
        byte[] dataWithoutSignature = new byte[data2.length - authCertificate.getSignature().length];
        System.arraycopy(data2, 0, dataWithoutSignature, 0, dataWithoutSignature.length);

        assertTrue(Arrays.equals(dataWithoutSignature, signableData));
    }

    /** */
    public void testGetExponent() {
        short value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setExponent(value);
        assertEquals(value, authCertificate.getExponent());
    }

    /** */
    public void testGetDummy() {
        short value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setDummy(value);
        assertEquals(value, authCertificate.getDummy());
    }

    /** */
    public void testGetModulusSize() {
        short value = 32;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setModulusSize(value);
        assertEquals(value, authCertificate.getModulusSize());
    }

    /** */
    public void testGetModulus() {
        byte[] values = new byte[]{1,2,3};
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setModulus(values);
        assertTrue(Arrays.equals(values, authCertificate.getModulus()));
    }

    /** */
    public void testGetMagic() {
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setMagic(GenericCertificate.MAGIC_AUTHENTICATION_CERTIFICATE);
        assertEquals(GenericCertificate.MAGIC_AUTHENTICATION_CERTIFICATE, authCertificate.getMagic());
    }

    /** */
    public void testGetAuthCertSize() {
        short value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setAuthCertSize(value);
        assertEquals(value, authCertificate.getAuthCertSize());
    }

    /** */
    public void testGetSignatureHashType() {
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setSignatureHashType(HashType.SHA256_HASH);
        assertEquals(HashType.SHA256_HASH.getHashType(), authCertificate.getSignatureHashType());
    }

    /** */
    public void testGetSignatureType() {
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setSignatureType(SignatureType.RSASSA_PKCS_V1_5);
        assertEquals(SignatureType.RSASSA_PKCS_V1_5.getSignatureType(), authCertificate.getSignatureType());
    }

    /** */
    public void testGetSerialNumber() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setSerialNumber(value);
        assertEquals(value, authCertificate.getSerialNumber());
    }

    /** */
    public void testGetNotBefore() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setNotBefore(value);
        assertEquals(value, authCertificate.getNotBefore());
    }

    /** */
    public void testGetNotAfter() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setNotAfter(value);
        assertEquals(value, authCertificate.getNotAfter());
    }

    /** */
    public void testGetaAuthenticationLevel() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setAuthenticationLevel(value);
        assertEquals(value, authCertificate.getAuthenticationLevel());
    }

    /** */
    public void testGetKeySize() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setKeySize(value);
        assertEquals(value, authCertificate.getKeySize());
    }

    /** */
    public void testGetKeyType() {
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setKeyTypeWithRevocationHashTypes(
                new SignatureTypeWithRevocationHashTypes(SignatureType.RSASSA_PKCS_V1_5));
        assertEquals(SignatureType.RSASSA_PKCS_V1_5.getSignatureType(), authCertificate.getKeyType());
    }

    /** */
    public void testGetSpare() {
        int value = 99;
        authCertificate = new GenericAuthenticationCertificate();
        authCertificate.setSpare(value);
        assertEquals(value, authCertificate.getSpare());
    }
}