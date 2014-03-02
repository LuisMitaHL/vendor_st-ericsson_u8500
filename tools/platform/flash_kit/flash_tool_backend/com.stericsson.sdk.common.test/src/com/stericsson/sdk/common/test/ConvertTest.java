package com.stericsson.sdk.common.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.Convert;

/**
 * @author mbodan01
 * 
 */
public class ConvertTest extends TestCase {

    /**
     * 
     */
    @Test
    public void testUIntToLong() {
        final int minInteger = Integer.MIN_VALUE;
        final int maxInteger = Integer.MAX_VALUE;
        final int maxIntegerPlusOne = Integer.MAX_VALUE + 1;

        assertEquals(((long) Integer.MIN_VALUE) & 0xffffffffL, Convert.uintToLong(minInteger));
        assertEquals(Integer.MAX_VALUE, Convert.uintToLong(maxInteger));
        assertEquals(((long) Integer.MAX_VALUE) + 1, Convert.uintToLong(maxIntegerPlusOne));
    }

    /**
     * 
     */
    @Test
    public void testUByteToInt() {
        final byte minByte = Byte.MIN_VALUE;
        final byte maxByte = Byte.MAX_VALUE;
        final byte maxBytePlusOne = (byte) (Byte.MAX_VALUE + 1);

        assertEquals(((int) Byte.MIN_VALUE) & 0xff, Convert.uint8ToInt(minByte));
        assertEquals(Byte.MAX_VALUE, Convert.uint8ToInt(maxByte));
        assertEquals(((int) Byte.MAX_VALUE) + 1, Convert.uint8ToInt(maxBytePlusOne));
    }

}
