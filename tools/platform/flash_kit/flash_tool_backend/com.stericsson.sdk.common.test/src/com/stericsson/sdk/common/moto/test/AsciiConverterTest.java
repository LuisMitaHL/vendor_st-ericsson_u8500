package com.stericsson.sdk.common.moto.test;

import static org.junit.Assert.*;

import org.junit.Test;

import com.stericsson.sdk.common.moto.AsciiConverter;

import junit.framework.TestCase;

/**
 * 
 * @author pkutac01
 * 
 */
public class AsciiConverterTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testConvert() {
        try {
            byte[] data = new byte[2];
            AsciiConverter.convert("0A0b", data);

            assertArrayEquals(new byte[] {
                0x0A, 0x0B}, data);
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testConvertInvalidInput() {
        try {
            String hex = "0A02";
            byte[] data = new byte[1];
            AsciiConverter.convert(hex, data);

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A02";
            byte[] data = new byte[8];
            AsciiConverter.convert(hex, 2, hex.length(), data);

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A02";
            byte[] data = new byte[2];
            AsciiConverter.convert(hex, -1, hex.length(), data);

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0*02";
            byte[] data = new byte[2];
            AsciiConverter.convert(hex, data);

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testToLongInvalidInput() {
        try {
            String hex = "0A02";
            AsciiConverter.toLong(hex, 2, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A02";
            AsciiConverter.toLong(hex, -1, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A020B0100";
            AsciiConverter.toLong(hex, 0, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testToIntInvalidInput() {
        try {
            String hex = "0A02";
            AsciiConverter.toInt(hex, 2, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A02";
            AsciiConverter.toInt(hex, -1, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try {
            String hex = "0A020B0100";
            AsciiConverter.toInt(hex, 0, hex.length());

            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testBinaryToStringNullInput() {
        assertNull(AsciiConverter.binaryToString(null));
    }

}
