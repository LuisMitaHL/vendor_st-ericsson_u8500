package com.stericsson.sdk.assembling.utilities;

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
    private static final char[] HEX_CHAR_TABLE = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    private static final String HEX_PREFIX = "0x";

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
            buf.append("00");
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
     * Convenience method for converting a decimal or hex number to a hex number representing a
     * specific number of bytes.
     * 
     * @param value
     *            The String, if it starts with 0x it is assumed to already be hex otherwise it is
     *            converted to hex from base 10.
     * @param bytes
     *            The number of bytes the number should represent.
     * @return A string starting with 0x padded with 0 so that the number has the length 2 * bytes
     */
    public static String toHexString(String value, int bytes) {
        StringBuffer resultBuf;
        if (!value.startsWith("0x")) {
            resultBuf = new StringBuffer(Integer.toHexString(Integer.parseInt(value)));
        } else {
            resultBuf = new StringBuffer(value.substring(2));
        }

        while (resultBuf.length() < 2 * bytes) {
            resultBuf.insert(0, '0');
        }

        resultBuf.insert(0, "0x");
        return resultBuf.toString();
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
        return value.toLowerCase(Locale.getDefault()).startsWith(HEX_PREFIX);
    }

    /**
     * Check if input value is hexadecimal digit
     * 
     * @param value
     *            input value
     * @return true if input value is hexadecimal
     */
    public static boolean isHexDigit(String value) {
        boolean isHex = hasHexPrefix(value);
        String digit = removeHexPrefix(value);

        char[] hexDigitArray = digit.toCharArray();

        for (char c : hexDigitArray) {
            isHex = Character.digit(c, 16) != -1;
            if (!isHex) {
                return false;
            }
        }
        return isHex;
    }

    /**
     * Returns hex string value as an integer value.
     * 
     * @param value
     *            Hex string value.
     * @return Integer value.
     */
    public static int hexStringToInt(String value) {
        if (value != null) {
            if (isHexDigit(value)) {
                return Integer.parseInt(removeHexPrefix(value), 16);
            } else {
                throw new NumberFormatException("Invalid hex number: " + value);
            }
        } else {
            throw new NullPointerException();
        }
    }

    /**
     * Returns hex string value as a long integer value.
     * 
     * @param value
     *            Hex string value.
     * @return Long integer value.
     */
    public static long hexStringToLong(String value) {
        if (value != null) {
            if (isHexDigit(value)) {
                return Long.parseLong(removeHexPrefix(value), 16);
            } else {
                throw new NumberFormatException("Invalid hex number: " + value);
            }
        } else {
            throw new NullPointerException();
        }
    }

    /**
     * Returns hex string value as an array of bytes.
     * 
     * @param value
     *            Hex string value.
     * @return Array of bytes.
     */
    public static byte[] hexStringToByteArray(String value) {
        if (value != null) {
            if (isHexDigit(value)) {
                String hexString = removeHexPrefix(value);
                int padding = hexString.length() % 2;
                if (padding > 0) {
                    hexString = "0" + hexString;
                }
                byte[] byteArray = new byte[(hexString.length() + padding) / 2];
                for (int i = 0; i < byteArray.length; i++) {
                    byteArray[i] = Integer.decode("0x" + hexString.substring(i * 2, i * 2 + 2)).byteValue();
                }
                return byteArray;
            } else {
                throw new NumberFormatException("Invalid hex number: " + value);
            }
        } else {
            throw new NullPointerException();
        }
    }

    private HexUtilities() {

    }
}
