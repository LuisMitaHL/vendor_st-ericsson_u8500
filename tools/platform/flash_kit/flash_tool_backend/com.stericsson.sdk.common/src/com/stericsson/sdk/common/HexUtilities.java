package com.stericsson.sdk.common;

import java.util.Locale;

/**
 * @author tomas
 */
/**
 * @author emicroh
 *
 */
public final class HexUtilities {

    /** Holds the character values */
    public static final char[] HEX_CHAR_TABLE = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /**
     * prefix of header hexadecimal
     */
    public static final String HEX_PREFIX = "0x";

    /**
     * Converts a series of byte into a string of hex characters. It is possible to specify a
     * sub-set of the byte array to convert and also insert a space between each byte.
     *
     * @param raw
     *            TBD
     * @param offset
     *            TBD
     * @param length
     *            TBD
     * @param columns
     *            TBD
     * @param space
     *            TBD
     * @return TBD
     */
    public static String toHexString(byte[] raw, int offset, int length, int columns, boolean space) {

        StringBuffer buffer = new StringBuffer();
        int col = 0;

        for (int i = 0; i < length; i++) {

            int v = raw[offset + i] & 0xFF;

            char c1 = HEX_CHAR_TABLE[v >>> 4];
            char c2 = HEX_CHAR_TABLE[v & 0xF];

            if ((col > 0) && space) {
                buffer.append(' ');
            }

            buffer.append(c1);
            buffer.append(c2);

            col++;

            if ((i < (length - 1)) && (columns > 0) && (col == columns)) {
                buffer.append('\n');
                col = 0;
            }

        }

        return buffer.toString();
    }

    /**
     * return a hex encoded string from a byte array
     *
     * @param data
     *            the byte array to encode
     * @return hex string
     */
    public static String toHexString(byte[] data) {
        return toHexString(data, 0, data.length, 0, false);
    }

    /**
     * return a long number from a byte array
     *
     * @param b
     *            the byte array from which the long number should be decoded.
     * @return long number
     * */
    public static long byteArrayToLong(byte[] b) {
        long value = 0;
        if (b.length > 8) {
            throw new NumberFormatException("Cannot create long from " + b.length + " bytes.");
        }

        for (int i = 0; i < b.length; i++) {
            value |= (long) (b[i] & 0xff) << i * 8;
        }

        return value;
    }

    /**
     * Converts a hexadecimal string to bytes.
     *
     * @param hex
     *            The string representations
     * @param numberOfBytes
     *            The number of bytes that the result should contain
     * @return The corresponding byte array
     */
    public static byte[] toByteArray(String hex, int numberOfBytes) {
        StringBuffer buffer = new StringBuffer(hex);
        if (hex.startsWith("0x")) {
            buffer.delete(0, 2);
        }
        while (buffer.length() < numberOfBytes * 2) {
            buffer.insert(0, "0");
        }
        if (buffer.length() > numberOfBytes * 2) {
            throw new IllegalArgumentException("Too many bytes, expected " + numberOfBytes + " bytes but got  "
                + buffer.length() / 2 + " bytes.");
        }
        byte[] ab = new byte[buffer.length() / 2];
        for (int i = 0; i < ab.length; i++) {
            ab[i] = Integer.decode("0x" + buffer.toString().substring(i * 2, i * 2 + 2)).byteValue();
        }
        return ab;
    }

    /**
     * @param hex
     *            TBD
     * @return TBD
     */
    public static byte[] toByteArray(String hex) {

        int padding = hex.length() % 2;
        int arrayLength = hex.length() / 2 + padding;
        byte[] result = new byte[arrayLength];

        int byteArrayIndex = 0;
        int hexStringIndex = 0;

        if (padding > 0) {
            result[byteArrayIndex++] = (byte) Character.digit(hex.charAt(hexStringIndex++), 16);
        }

        while (byteArrayIndex < arrayLength) {
            result[byteArrayIndex++] =
                (byte) ((Character.digit(hex.charAt(hexStringIndex++), 16) << 4) | Character.digit(hex
                    .charAt(hexStringIndex++), 16));
        }

        return result;
    }

    /**
     * Will represent given byte in hex format.
     *
     * @param c
     *            Byte to represent in hex format.
     * @return Hex representation of given byte.
     */
    public static String toHexString(byte c) {
        return toHexString(c, false);
    }

    /**
     * Will represent given byte in hex format. If given flag entire is set, additional zeroes will
     * be added if needed.
     *
     * @param c
     *            Byte to represent in hex format.
     * @param entire
     *            True means that additional zeroes will be added if needed.
     *
     * @return Hex representation of given byte.
     */
    public static String toHexString(byte c, boolean entire) {
        StringBuffer buf = new StringBuffer("0x");
        for (int i = 1; i >= 0; i--) {
            int index = (int) (((c >> (i * 4)) & 0xF));
            if (entire) {
                buf.append(HEX_CHAR_TABLE[index]);
            } else if (index == 0) {
                continue;
            } else {
                buf.append(HEX_CHAR_TABLE[index]);
                entire = true;
            }
        }
        if (buf.length() == 2) {
            if (entire) {
                buf.append("00");
            } else {
                buf.append("0");
            }
        }
        return buf.toString();
    }

    /**
     * Will represent given integer value in hex format.
     *
     * @param c
     *            Integer value to represent in hex format.
     * @return Hex representation of given integer value.
     */
    public static String toHexString(int c) {
        return toHexString(c, false);
    }

    /**
     * Will represent given integer value in hex format. If given flag entire is set, additional
     * zeroes will be added if needed.
     *
     * @param c
     *            Integer value to represent in hex format.
     * @param entire
     *            True means that additional zeroes will be added if needed.
     *
     * @return Hex representation of given integer value.
     */
    public static String toHexString(int c, boolean entire) {
        StringBuffer buf = new StringBuffer("0x");
        for (int i = 7; i >= 0; i--) {
            int index = (int) (((c >> (i * 4)) & 0xF));
            if (entire) {
                buf.append(HEX_CHAR_TABLE[index]);
            } else if (index == 0) {
                continue;
            } else {
                buf.append(HEX_CHAR_TABLE[index]);
                entire = true;
            }
        }
        if (buf.length() == 2) {
            buf.append("00");
        }
        return buf.toString();
    }

    /**
     * Will represent given long value in hex format.
     *
     * @param c
     *            Long value to represent in hex format.
     * @return Hex representation of given long value.
     */
    public static String toHexString(long c) {
        return toHexString(c, false);
    }

    /**
     * Will represent given long value in hex format. If given flag entire is set, additional zeroes
     * will be added if needed.
     *
     * @param c
     *            Long value to represent in hex format.
     * @param entire
     *            True means that additional zeroes will be added if needed.
     *
     * @return Hex representation of given long value.
     */
    public static String toHexString(long c, boolean entire) {
        StringBuffer buf = new StringBuffer("0x");
        for (int i = 15; i >= 0; i--) {
            int index = (int) (((c >> (i * 4)) & 0xF));
            if (entire) {
                buf.append(HEX_CHAR_TABLE[index]);
            } else if (index == 0) {
                continue;
            } else {
                buf.append(HEX_CHAR_TABLE[index]);
                entire = true;
            }
        }
        if (buf.length() == 2) {
            buf.append("00");
        }
        return buf.toString();
    }

    /**
     * Returns a string without the '0x' prefix if there is one
     *
     * @param value
     *            hexadecimal value with '0x' prefix
     *
     * @return returns a string without the '0x' prefix if there is one
     */
    public static String removeHexPrefix(String value) {
        if (hasHexPrefix(value)) {
            return value.substring(HEX_PREFIX.length()).toUpperCase(Locale.getDefault());
        } else {
            return value.toUpperCase(Locale.getDefault());
        }
    }

    /**
     * Check if input value has hexadecimal prefix '0x'
     *
     * @param value
     *            input value
     * @return true if value has hexadecimal prefix else false
     */
    public static boolean hasHexPrefix(String value) {
        return value.startsWith(HEX_PREFIX);
    }

    /**
     * Check if input value is hexadecimal digit
     *
     * @param value
     *            input value
     * @return true if input value is hexadecimal
     */
    public static boolean isHexDigit(String value) {
        String digit = removeHexPrefix(value);

        char[] hexDigitArray = digit.toCharArray();
        boolean isNotHex = false;

        for (char c : hexDigitArray) {
            isNotHex = Character.digit(c, 16) == -1;
            if (isNotHex) {
                return false;
            }
        }
        return !isNotHex;
    }

    /**
     * Parse Hexadecimal number in string with or without 0x prefix into integer.
     *
     * @param pValue
     *            input hexadecima string
     * @throws NumberFormatException
     *             bad format
     * @return converted number
     */
    public static int parseHexDigit(String pValue) throws NumberFormatException {
        if (HexUtilities.hasHexPrefix(pValue)) {
            return Integer.parseInt(HexUtilities.removeHexPrefix(pValue), 16);
        } else {
            return Integer.parseInt(pValue, 16);
        }
    }

    /**
     * Parse Hexadecimal long number in string with or without 0x prefix into longint.
     *
     * @param pValue
     *            input hexadecima string
     * @throws NumberFormatException
     *             bad format
     * @return converted number
     */
    public static long parseLongHexDigit(String pValue) throws NumberFormatException {
        if (HexUtilities.hasHexPrefix(pValue)) {
            return Long.parseLong(HexUtilities.removeHexPrefix(pValue), 16);
        } else {
            return Long.parseLong(pValue, 16);
        }
    }

    private HexUtilities() {

    }
}
