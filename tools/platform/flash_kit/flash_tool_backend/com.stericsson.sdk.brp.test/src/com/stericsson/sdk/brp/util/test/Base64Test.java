package com.stericsson.sdk.brp.util.test;

import static org.junit.Assert.*;

import org.junit.Test;

import com.stericsson.sdk.brp.util.Base64;

import junit.framework.TestCase;

/**
 * 
 * @author pkutac01
 * 
 */
public class Base64Test extends TestCase {

    /**
     * 
     */
    @Test
    public void testEncodeAndDecodeByteArray() {
        byte[] data = new byte[] {
            1, 2, 3, 4, 5, 6};
        String expected = "AQIDBAUG";
        String encoded = Base64.encode(data);
        assertEquals(expected, encoded);
        byte[] decoded = Base64.decode(encoded);
        assertArrayEquals(data, decoded);

        data = new byte[] {
            1, 2, 3, 4, 5, 6, 7};
        expected = "AQIDBAUGBw==";
        encoded = Base64.encode(data);
        assertEquals(expected, encoded);
        decoded = Base64.decode(encoded);
        assertArrayEquals(data, decoded);

        data = new byte[] {
            1, 2, 3, 4, 5, 6, 7, 8};
        expected = "AQIDBAUGBwg=";
        encoded = Base64.encode(data);
        assertEquals(expected, encoded);
        decoded = Base64.decode(encoded);
        assertArrayEquals(data, decoded);

        data = new byte[] {
            0, 0, 0, 0, 4, -128};
        expected = "AAAAAASA";
        encoded = Base64.encode(data);
        assertEquals(expected, encoded);
        decoded = Base64.decode(encoded);
        assertArrayEquals(data, decoded);
    }

}
