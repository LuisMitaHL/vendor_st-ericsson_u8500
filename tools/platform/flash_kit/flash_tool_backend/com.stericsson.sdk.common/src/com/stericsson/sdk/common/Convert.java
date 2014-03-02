package com.stericsson.sdk.common;

/**
 * Methods for conversion between data types.
 * 
 * @author mbodan01
 * 
 */
public final class Convert {

    private Convert() {

    }

    /**
     * Converts uint value stored in Java int to Java long value.
     * 
     * @param value
     *            uint value stored in Java int
     * @return Java long value
     */
    public static long uintToLong(int value) {
        return (0xffffffffL & value);
    }

    /**
     * Converts uint8 value stored in Java byte to Java int value.
     * 
     * @param value
     *            uint8 value stored in Java byte
     * @return Java int value
     */
    public static int uint8ToInt(byte value) {
        return (0xff & value);
    }

}
