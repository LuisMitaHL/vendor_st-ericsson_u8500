package com.stericsson.sdk.common;

/**
 * @author NONAME
 * 
 */
public final class LittleEndianByteConverter {

    private LittleEndianByteConverter() {
    }

    /**
     * Creates a new byte array with reverse order of the bytes.
     * 
     * @param byteArray
     *            The array to reverse
     * @return A new reversed array
     *         <p>
     *         FIME: THIS IS DOUBLE MAINTANENCE THIS METHOD EXISTS IN HEXUTILS.JAVA TO!!!!!!!!!!!
     */
    public static byte[] reverse(byte[] byteArray) {
        byte[] result = new byte[byteArray.length];
        for (int i = 0; i < result.length; i++) {
            result[result.length - i - 1] = byteArray[i];
        }
        return result;
    }

    /** Converts a maximum 4 byte little endian byte[] to an int value */
    // Argument byte[]bytes :
    /**
     * A byte array containing a little endian value
     * 
     * @param bytes
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLMEOUT.
     */
    public static int byteArrayToInt(byte[] bytes) {
        if (bytes.length > 4) {
            throw new ArithmeticException("Cannot convert numbers larger than 32 bit to int");
        }
        int value = 0;
        for (int i = 0; i < bytes.length; i++) {
            value |= (bytes[i] & 0xFF) << (i * 8);
        }
        return value;
    }

    /**
     * A byte array containing a little endian value
     * 
     * @param bytes
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLMEOUT.
     */
    public static short byteArrayToShort(byte[] bytes) {
        if (bytes.length > 2) {
            throw new ArithmeticException("Cannot convert numbers larger than 16 bit to short");
        }
        int value = 0;
        for (int i = 0; i < bytes.length; i++) {
            value |= (bytes[i] & 0xFF) << (i * 8);
        }
        return (short) value;
    }

    /**
     * Converts up to four byte little endian value to an int.
     * 
     * @param bytes
     *            The array holding the bytes to convert
     * @param startPos
     *            The start position in the array
     * @param length
     *            The number of bytes to use in the calculation, note that this value cannot exceed
     *            4.
     * @return TODO FILLMEOUT.
     */
    public static int byteArrayToInt(byte[] bytes, int startPos, int length) {
        int value = 0;
        if (length > 4) {
            throw new ArithmeticException("Cannot convert numbers larger than 32bits to int");
        }
        for (int i = startPos; i < startPos + length; i++) {
            value |= (bytes[i] & 0xFF) << ((i - startPos) * 8);
        }
        return value;
    }

    /**
     * Converts an integer to a 4 or 2 byte little endian byte array
     * 
     * @param size
     *            TODO FILLMEOUT.
     * @param value
     *            TODO FILLMEOUT.
     * @return TODO FILLME OUT.
     */
    public static byte[] intToByteArray(int value, int size) {
        byte[] data = new byte[size];
        for (int i = 0; i < size; i++) {
            data[i] = (byte) ((value >> (i * 8)) & 0xFF);
        }
        return data;
    }

    /**
     * Converts a maximum 8 byte little endian byte[] to an int value A byte array containing a
     * little endian value
     * 
     * @param bytes
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     * 
     */
    public static long byteArrayToLong(byte[] bytes) {
        if (bytes.length > 8) {
            throw new ArithmeticException("Cannot convert numbers larger than 64 bit to long");
        }
        long value = 0;
        for (int i = 0; i < bytes.length; i++) {
            value |= ((long) bytes[i] & 0xFF) << (i * 8);
        }
        return value;
    }

    /**
     * Converts a maximum 8 byte little endian byte[] to an int value A byte array containing a
     * little endian value
     * 
     * @param bytes
     *            TODO FILLMEOUT.
     * @param startPos
     *            TODO FILLMEOUT.
     * @param length
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     */
    public static long byteArrayToLong(byte[] bytes, int startPos, int length) {
        if (length > 8) {
            throw new ArithmeticException("Cannot convert numbers larger than 64 bit to long");
        }
        long value = 0;
        for (int i = 0; i < length; i++) {
            value |= ((long) bytes[startPos + i] & 0xFF) << (i * 8);
        }
        return value;
    }

    /**
     * Converts a long value to a 4 byte little endian array, this may result in truncation return
     * 
     * @param value
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     */
    public static byte[] longTo4Bytes(long value) {
        byte[] data = new byte[4];
        for (int i = 0; i < 4; i++) {
            data[i] = (byte) ((value >> (i * 8)) & 0xFF);
        }
        return data;
    }

    /**
     * Converts a long value to an 8 byte little endian array
     * 
     * @param value
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     */
    public static byte[] longTo8Bytes(long value) {
        byte[] data = new byte[8];
        for (int i = 0; i < 8; i++) {
            data[i] = (byte) ((value >> (i * 8)) & 0xFF);
        }
        return data;
    }

    /**
     * Converts an int value to a 4 byte little endian array
     * 
     * @param value
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     */
    public static byte[] valueToByteArray(int value) {
        return valueToByteArray(value, 4);
    }

    /**
     * Converts an int value to a byteCnt byte little endian array
     * 
     * @param value
     *            TODO FILLMEOUT.
     * @param byteCnt
     *            TODO FILLMEOUT.
     * 
     * @return TODO FILLME OUT.
     */
    public static byte[] valueToByteArray(int value, int byteCnt) {
        byte[] data = new byte[byteCnt];
        for (int i = 0; i < data.length; i++) {
            if (i > 3) {
                data[i] = 0; // only four bytes can be found in int value
            } else {
                data[i] = (byte) ((value >> (i * 8)) & 0xFF);
            }
        }
        return data;
    }

    /**
     * Method swaps endianess in byte array
     * 
     * @param bytes
     *            input byte array
     * @return output byte array
     */
    public static byte[] swapEndianess(byte[] bytes) {
        int mod = bytes.length % 4;
        if (mod != 0) {
            throw new IllegalArgumentException("Input array of bytes must be divisible by 4.");
        }
        byte[] tmpBits = new byte[bytes.length];
        for (int i = 0; i < bytes.length - 3; i += 4) {
            tmpBits[i] = bytes[i + 3];
            tmpBits[i + 1] = bytes[i + 2];
            tmpBits[i + 2] = bytes[i + 1];
            tmpBits[i + 3] = bytes[i];
        }
        return tmpBits;
    }

}
