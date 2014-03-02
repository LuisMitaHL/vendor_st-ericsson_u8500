package com.stericsson.sdk.common.test;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.BeforeClass;
import org.junit.Test;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * @author xdancho
 * 
 */
public class LittleEndianByteConverterTest extends TestCase {
    /**
     * test
     */
    @BeforeClass
    public static void setUpBeforeClass() {
    }

    /**
     * test
     */
    @Test
    public void testReverse() {

        byte[] a = {
            (byte) 0xAA, (byte) 0xBB};
        byte[] b = {
            (byte) 0xBB, (byte) 0xAA};

        assertArrayEquals(LittleEndianByteConverter.reverse(a), b);
    }

    /**
     * test
     */
    @Test
    public void testByteArrayToInt() {

        byte[] a = {
            1, 0, 0, 0};

        assertEquals(1, LittleEndianByteConverter.byteArrayToInt(a, 0, a.length));

        assertEquals(1, LittleEndianByteConverter.byteArrayToInt(a));

    }

    /**
     * test
     */
    @Test
    public void testByteArrayToShort() {

        byte[] a = {
            1, 0};

        assertEquals(1, LittleEndianByteConverter.byteArrayToShort(a));

    }

    /**
     * test
     */
    @Test
    public void testIntToByteArray() {

        byte[] a = {
            1, 0, 0, 0};

        byte[] b = {
            1, 0};

        assertArrayEquals(a, LittleEndianByteConverter.intToByteArray(1, 4));

        assertArrayEquals(b, LittleEndianByteConverter.intToByteArray(1, 2));
    }

    /**
     * test
     */
    @Test
    public void testByteArrayToLong() {

        byte[] b = {
            1, 0, 0, 0, 0, 0, 0, 0};

        assertEquals(1L, LittleEndianByteConverter.byteArrayToLong(b));

        assertEquals(1L, LittleEndianByteConverter.byteArrayToLong(b, 0, b.length));

    }

    /**
     * 
     */
    @Test
    public void testLongToBytes() {

        byte[] a = {
            1, 0, 0, 0, 0, 0, 0, 0};
        byte[] b = {
            1, 0, 0, 0};

        assertArrayEquals(b, LittleEndianByteConverter.longTo4Bytes(1));
        assertArrayEquals(a, LittleEndianByteConverter.longTo8Bytes(1));
    }

    /**
     * 
     */
    @Test
    public void testValueToByteArray() {

        byte[] a = {
            1, 0, 0, 0, 0, 0, 0, 0};
        byte[] b = {
            1, 0, 0, 0};

        assertArrayEquals(b, LittleEndianByteConverter.valueToByteArray(1));
        assertArrayEquals(a, LittleEndianByteConverter.valueToByteArray(1, 8));
    }
}
