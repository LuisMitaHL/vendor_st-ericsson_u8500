package com.stericsson.sdk.common.moto;

import java.io.CharConversionException;

/**
 * Converts basic ASCII a-z, A-F and 0-9 into various binary representations.
 * 
 * @author ecspasc
 * 
 */

public final class AsciiConverter {

    /**
     * A private constructor.
     */
    private AsciiConverter() {
    }

    /**
     * Four byte buffer representing default int size.
     */
    private static final byte[] INTEGER = new byte[4];

    /**
     * Converts the given string of ascii to binary and saves it in provided buffer.
     * 
     * @param asciiBytes
     *            The string to convert.
     * @param target
     *            Buffer to save conversion in.
     */
    public static void convert(final String asciiBytes, final byte[] target) {
        convert(asciiBytes, 0, asciiBytes.length(), target);
    }

    /**
     * Converts string representing a hex value into binary.
     * 
     * @param asciiBytes
     *            The String of Hex characters containing a hex value
     * @param offset
     *            The offset in the asciiBytes from where to start converting
     * @param length
     *            The number of characters in the String to convert
     * @param target
     *            The target byte array that receives the converted value
     */
    public static void convert(final String asciiBytes, final int offset, final int length, final byte[] target) {
        if (length > target.length * 2) {
            throw new IllegalArgumentException("Target lenght does not match values");
        }
        if (length > (asciiBytes.length() - offset) || offset < 0) {
            throw new IllegalArgumentException("Illegal offset/length");
        }
        int start = target.length - length / 2;
        for (int i = start; i < target.length; i++) {
            try {
                target[i] =
                    charSetToBin(asciiBytes.charAt(offset + (2 * (i - start))), asciiBytes.charAt(offset
                        + (2 * (i - start)) + 1));
            } catch (CharConversionException ce) {
                throw new IllegalArgumentException(ce.getMessage());
            }
        }
        for (int i = 0; i < start; i++) {
            target[i] = 0;
        }
    }

    /**
     * Treats the two characters as two characters representing one HEX(0x) value i.e if high == 'A'
     * and low == '3' this would be treated as "(0x)A3" and converts them to a binary
     * representation.
     * 
     * @param high
     *            The high character e.g. "A".
     * @param low
     *            The low character e.g. "3".
     * @throws CharConversionException
     *             if any of the characters do not hold the value of a valid HEX representation i.e.
     *             "(0x)#3".
     */
    private static byte charSetToBin(char high, char low) throws CharConversionException {
        try {
            high = convertCharacter(high);
            low = convertCharacter(low);
            int val = (high << 4) & 0xF0;
            val = val | (low & 0x0F);
            return (byte) (val & 0xFF);
        } catch (CharConversionException e) {
            throw e;
        }
    }

    /**
     * Checks if the value of the given character lies within the range of a valid HEX value
     * character.
     * 
     * @param character
     *            The presumed HEX character to validate and convert.
     * @throws CharConversionException
     *             if the character value is not within the range of a valid HEX character.
     */
    private static char convertCharacter(final char c) throws CharConversionException {
        if (c >= '0' && c <= '9') {
            return (char) (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            return (char) (c - ('A' - 10));
        } else if (c >= 'a' && c <= 'f') {
            return (char) (c - ('a' - 10));
        }
        throw new CharConversionException("Cannot convert '" + c + "' as HEX character!");
    }

    /**
     * Converts string representing a value into long.
     * 
     * @param asciiBytes
     *            String to convert.
     * @param offset
     *            Offset in string.
     * @param length
     *            Number of chars to convert.
     * @return The converted value.
     */
    public static long toLong(final String asciiBytes, final int offset, final int length) {
        if (length > (asciiBytes.length() - offset) || offset < 0) {
            throw new IllegalArgumentException("Illegal offset/length");
        }
        if (length > 8) {
            throw new IllegalArgumentException("Value length must be <= 8");
        }
        byte[] ab = INTEGER;
        for (int i = 0; i < ab.length; i++) {
            ab[i] = 0;
        }
        convert(asciiBytes, offset, length, ab);
        long val = 0;
        for (int i = 0; i < 4; i++) {
            // System.out.print( Integer.toHexString( (int)(ab[i]&0xFF) ) + " " );
            long temp = (long) (ab[i] & 0xFF);
            temp = temp << (8 * (3 - i));
            val |= temp;
        }
        return val;
    }

    /**
     * Converts string representing a value into int.
     * 
     * @param asciiBytes
     *            String to convert.
     * @param offset
     *            Offset in string.
     * @param length
     *            Number of chars to convert.
     * @return The converted value.
     */
    public static int toInt(final String asciiBytes, final int offset, final int length) {
        if (length > (asciiBytes.length() - offset) || offset < 0) {
            throw new IllegalArgumentException("Illegal offset/length");
        }
        if (length > 8) {
            throw new IllegalArgumentException("Value length must be <= 8");
        }
        byte[] ab = INTEGER;
        for (int i = 0; i < ab.length; i++) {
            ab[i] = 0;
        }
        convert(asciiBytes, offset, length, ab);
        int val = 0;
        for (int i = 0; i < 4; i++) {
            // System.out.print( Integer.toHexString( (int)(ab[i]&0xFF) ) + " " );
            int temp = (int) (ab[i] & 0xFF);
            temp = temp << (8 * (3 - i));
            val |= temp;
        }
        return val;
    }

    /**
     * @param data
     *            TBD
     * @return TBD
     */
    public static String binaryToString(byte[] data) {
        if (data == null) {
            return null;
        }

        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < data.length; i++) {
            String s = Integer.toHexString(data[i]);
            if (s.length() == 1) {
                buf.append("0" + s);
            } else {
                buf.append(s);
            }
        }

        return buf.toString();
    }
}
