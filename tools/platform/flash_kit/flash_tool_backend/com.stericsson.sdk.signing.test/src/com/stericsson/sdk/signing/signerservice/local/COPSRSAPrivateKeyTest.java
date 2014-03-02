package com.stericsson.sdk.signing.signerservice.local;

import java.math.BigInteger;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.signerservice.local.keys.COPSRSAPrivateKey;

/**
 * 
 * @author xolabju
 * 
 */
public class COPSRSAPrivateKeyTest extends TestCase{
    /**
     * 
     */
    @Test
    public void testCOPSRSAPrivateKeyBigIntegerBigInteger() {
        BigInteger mod = new BigInteger("123456789");
        BigInteger exp = new BigInteger("987654321");
        COPSRSAPrivateKey key = new COPSRSAPrivateKey(mod, exp);
        assertEquals(mod, key.getModulus());
        assertEquals(exp, key.getPrivateExponent());
    }

    /**
     * 
     */
    @Test
    public void testCOPSRSAPrivateKey() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        assertNull(key.getModulus());
        assertNull(key.getPrivateExponent());
    }

    /**
     * 
     */
    @Test
    public void testGetAlgorithm() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        assertEquals("RSA", key.getAlgorithm());
    }

    /**
     * 
     */
    @Test
    public void testGetEncoded() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        assertNull(key.getEncoded());
    }

    /**
     * 
     */
    @Test
    public void testGetFormat() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        assertEquals("???", key.getFormat());
    }

    /**
     * 
     */
    @Test
    public void testSetModulus() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        BigInteger mod = new BigInteger("123456789");
        key.setModulus(mod);
        assertEquals(mod, key.getModulus());
    }

    /**
     * 
     */
    @Test
    public void testSetPrivateExponent() {
        COPSRSAPrivateKey key = new COPSRSAPrivateKey();
        BigInteger exp = new BigInteger("987654321");
        key.setPrivateExponent(exp);
        assertEquals(exp, key.getPrivateExponent());
    }

}
