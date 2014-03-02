package com.stericsson.sdk.common.security;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author xdancho
 * 
 */
public class ImeiConverterTest extends TestCase {

    String readableString = "12345678901234";

    byte[] imeiBytesValid = new byte[] {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4};

    byte[] imeiBytesInvalid = new byte[] {
        -1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4};

    byte[] imeiBytesInvalid2 = new byte[] {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 15};

    /**
     * 
     */
    @Test
    public void testEncodeDecodeImei() {
        byte[] compressedImei = ImeiConverter.encodeIMEI(readableString);
        assertEquals(readableString, ImeiConverter.decodeIMEI(compressedImei));
    }

    /**
     * 
     */
    @Test
    public void testStringByteConvertion() {
        try {
            ImeiConverter.getImeiAsString(imeiBytesInvalid);
            fail();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            assertNotNull(e);
        }

        try {
            ImeiConverter.getImeiAsString(imeiBytesInvalid2);
            fail();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            assertNotNull(e);
        }

        try {
            String imeiString = ImeiConverter.getImeiAsString(imeiBytesValid);
            assertEquals(readableString, imeiString);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            fail();
        }

        byte[] imeiBytes = ImeiConverter.getImeiAsBytes(readableString);
        assertArrayEquals(imeiBytesValid, imeiBytes);

    }
}
