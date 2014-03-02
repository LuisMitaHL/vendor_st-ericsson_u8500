package com.stericsson.sdk.common.test;

import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.common.HexUtilities;

/**
 * @author xtomlju
 */
public class HexUtilitiesTest extends TestCase {

    /**
     * @throws Exception
     *             TBD
     */
    @Override
    @Before
    public void setUp() throws Exception {
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Override
    @After
    public void tearDown() throws Exception {
    }

    /**
     * 
     */
    @Test
    public void testToHexString() {

        byte[] buffer = new byte[] {
            (byte) 0x11, (byte) 0x22, (byte) 0x33, (byte) 0x44, (byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD};
        String reference1 = "11223344AABBCCDD";
        String reference2 = "11 22 33 44 AA BB CC DD";
        String reference3 = "11 22 33 44\nAA BB CC DD";

        String hexString1 = HexUtilities.toHexString(buffer, 0, buffer.length, 0, false);
        String hexString2 = HexUtilities.toHexString(buffer, 0, buffer.length, 0, true);
        String hexString3 = HexUtilities.toHexString(buffer, 0, buffer.length, 4, true);

        assertTrue(hexString1.compareTo(reference1) == 0);
        assertTrue(hexString2.compareTo(reference2) == 0);
        assertTrue(hexString3.compareTo(reference3) == 0);
    }

    /**
     * 
     */
    @Test
    public void testToByteArray() {
        byte[] reference1 = new byte[] {
            (byte) 0x11, (byte) 0x22, (byte) 0x33, (byte) 0x44, (byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD};
        byte[] reference2 = new byte[] {
            (byte) 0x01, (byte) 0x22, (byte) 0x33, (byte) 0x44, (byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD};

        String hexString1 = "11223344AABBCCDD";
        String hexString2 = "1223344AABBCCDD";

        byte[] buffer1 = HexUtilities.toByteArray(hexString1);
        byte[] buffer2 = HexUtilities.toByteArray(hexString2);

        assertTrue(buffer1.length == reference1.length);
        assertTrue(buffer2.length == reference2.length);

        assertTrue(Arrays.equals(reference1, buffer1));
        assertTrue(Arrays.equals(reference2, buffer2));
    }

    /**
     * 
     */
    @Test
    public void testByteToHexString() {
        String hexA = "0x0A";
        String hexB = "0x0B";
        String hexC = "0x0C";
        String hexD = "0xD";
        String hexE = "0xE";
        String hexF = "0xF";
        String hex0 = "0x0";

        byte byteA = 10;
        byte byteB = 11;
        byte byteC = 12;
        byte byteD = 13;
        byte byteE = 14;
        byte byteF = 15;
        byte byte0 = 0;

        String refA = HexUtilities.toHexString(byteA, true);
        String refB = HexUtilities.toHexString(byteB, true);
        String refC = HexUtilities.toHexString(byteC, true);
        String refD = HexUtilities.toHexString(byteD);
        String refE = HexUtilities.toHexString(byteE);
        String refF = HexUtilities.toHexString(byteF);
        String ref0 = HexUtilities.toHexString(byte0);

        assertTrue(hexA.equals(refA));
        assertTrue(hexB.equals(refB));
        assertTrue(hexC.equals(refC));
        assertTrue(hexD.equals(refD));
        assertTrue(hexE.equals(refE));
        assertTrue(hexF.equals(refF));
        assertTrue(hex0.equals(ref0));

    }

    /**
     * 
     */
    @Test
    public void testIntToHexString() {
        String hexA = "0x0000000A";
        String hexB = "0x0000000B";
        String hexC = "0x0000000C";
        String hexD = "0xD";
        String hexE = "0xE";
        String hexF = "0xF";
        String hex0 = "0x00";

        int intA = 10;
        int intB = 11;
        int intC = 12;
        int intD = 13;
        int intE = 14;
        int intF = 15;
        int int0 = 0;

        String refA = HexUtilities.toHexString(intA, true);
        String refB = HexUtilities.toHexString(intB, true);
        String refC = HexUtilities.toHexString(intC, true);
        String refD = HexUtilities.toHexString(intD);
        String refE = HexUtilities.toHexString(intE);
        String refF = HexUtilities.toHexString(intF);
        String ref0 = HexUtilities.toHexString(int0);

        assertTrue(hexA.equals(refA));
        assertTrue(hexB.equals(refB));
        assertTrue(hexC.equals(refC));
        assertTrue(hexD.equals(refD));
        assertTrue(hexE.equals(refE));
        assertTrue(hexF.equals(refF));
        assertTrue(hex0.equals(ref0));
    }

    /**
     * 
     */
    @Test
    public void testLongToHexString() {
        String hexA = "0x000000000000000A";
        String hexB = "0x000000000000000B";
        String hexC = "0x000000000000000C";
        String hexD = "0xD";
        String hexE = "0xE";
        String hexF = "0xF";
        String hex0 = "0x00";

        long intA = 10;
        long intB = 11;
        long intC = 12;
        long intD = 13;
        long intE = 14;
        long intF = 15;
        long int0 = 0;

        String refA = HexUtilities.toHexString(intA, true);
        String refB = HexUtilities.toHexString(intB, true);
        String refC = HexUtilities.toHexString(intC, true);
        String refD = HexUtilities.toHexString(intD);
        String refE = HexUtilities.toHexString(intE);
        String refF = HexUtilities.toHexString(intF);
        String ref0 = HexUtilities.toHexString(int0);

        assertTrue(hexA.equals(refA));
        assertTrue(hexB.equals(refB));
        assertTrue(hexC.equals(refC));
        assertTrue(hexD.equals(refD));
        assertTrue(hexE.equals(refE));
        assertTrue(hexF.equals(refF));
        assertTrue(hex0.equals(ref0));
    }

    /**
     * 
     */
    @Test
    public void testHasHexPrefix() {
        String digitWithPrefix = "0xFFFF";
        String digitWithoutPrefix = "FFFF";

        assertTrue(HexUtilities.hasHexPrefix(digitWithPrefix));
        assertFalse(HexUtilities.hasHexPrefix(digitWithoutPrefix));
    }

    /**
     * 
     */
    @Test
    public void testRemoveHexPrefix() {
        String digitWithPrefix = "0xFFFF";
        String digitWithoutPrefix = "FFFF";

        String digitWithRemovedPrefix = HexUtilities.removeHexPrefix(digitWithPrefix);
        assertTrue(digitWithoutPrefix.equalsIgnoreCase(digitWithRemovedPrefix));

        digitWithRemovedPrefix = HexUtilities.removeHexPrefix(digitWithoutPrefix);
        assertTrue(digitWithoutPrefix.equalsIgnoreCase(digitWithRemovedPrefix));
    }

    /**
     * 
     */
    @Test
    public void testIsHexDigit() {
        String hexDigitWithPrefix = "0xFFFF";
        String hexDigitWithoutPrefix = "FFFF";
        String notHexDigitWithoutPrefix = "ANDROID";
        String notHexDigitWithPrefix = "0xANDROID";

        assertTrue(HexUtilities.isHexDigit(hexDigitWithPrefix));
        assertTrue(HexUtilities.isHexDigit(hexDigitWithoutPrefix));
        assertFalse(HexUtilities.isHexDigit(notHexDigitWithPrefix));
        assertFalse(HexUtilities.isHexDigit(notHexDigitWithoutPrefix));
    }

    /**
     * 
     */
    @Test
    public void testperseHexDigit() {
        String hexDigitWithPrefix = "0x0FF0";
        String hexDigitWithoutPrefix = "0FF0";
        String notHexDigitWithoutPrefix = "ANDROID";
        String notHexDigitWithPrefix = "0xANDROID";

        assertEquals(4080, HexUtilities.parseHexDigit(hexDigitWithPrefix));
        assertEquals(4080, HexUtilities.parseHexDigit(hexDigitWithoutPrefix));

        try {
            HexUtilities.parseHexDigit(notHexDigitWithoutPrefix);
            assertTrue(false);
        } catch (NumberFormatException e) {
            assertTrue(true);
        }
        try {
            HexUtilities.parseHexDigit(notHexDigitWithPrefix);
            assertTrue(false);
        } catch (NumberFormatException e) {
            assertTrue(true);
        }
    }

    /**
     * 
     */
    @Test
    public void testperseLongHexDigit() {
        String hexDigitWithPrefix = "0xFFFFFFFFFC";
        String hexDigitWithoutPrefix = "FFFFFFFFFC";
        String notHexDigitWithoutPrefix = "ANDROID";
        String notHexDigitWithPrefix = "0xANDROID";

        assertEquals(1099511627772L, HexUtilities.parseLongHexDigit(hexDigitWithPrefix));
        assertEquals(1099511627772L, HexUtilities.parseLongHexDigit(hexDigitWithoutPrefix));

        try {
            HexUtilities.parseLongHexDigit(notHexDigitWithoutPrefix);
            assertTrue(false);
        } catch (NumberFormatException e) {
            assertTrue(true);
        }
        try {
            HexUtilities.parseLongHexDigit(notHexDigitWithPrefix);
            assertTrue(false);
        } catch (NumberFormatException e) {
            assertTrue(true);
        }
    }

}
