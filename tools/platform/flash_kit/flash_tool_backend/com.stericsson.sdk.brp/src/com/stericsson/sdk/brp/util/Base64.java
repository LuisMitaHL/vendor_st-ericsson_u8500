package com.stericsson.sdk.brp.util;

/**
 * Methods for Base64 encoding and decoding.
 * 
 * @author pkutac01
 * 
 */
public final class Base64 {

    private static final char[] BASE64 =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".toCharArray();

    private static int[] indexes = new int[128];

    static {
        for (int i = 0; i < BASE64.length; i++) {
            indexes[BASE64[i]] = i;
        }
    }

    private Base64() {

    }

    /**
     * Encodes byte array to Base64 string.
     * 
     * @param pData
     *            Byte array.
     * @return Base64 string.
     */
    public static String encode(byte[] pData) {
        StringBuffer buffer = new StringBuffer(((pData.length + 2) / 3) * 4);

        for (int i = 0; i < pData.length; i += 3) {
            int delta = pData.length - i;
            int bits = 0;
            if (delta > 2) {
                bits |= ((int) pData[i + 2]) & 0xFF;
            }
            if (delta > 1) {
                bits |= (((int) pData[i + 1]) & 0xFF) << 8;
            }
            bits |= (((int) pData[i]) & 0xFF) << 16;

            buffer.append(BASE64[(bits >> 18) & 0x3F]);
            buffer.append(BASE64[(bits >> 12) & 0x3F]);
            if (delta > 1) {
                buffer.append(BASE64[(bits >> 6) & 0x3F]);
            }
            if (delta > 2) {
                buffer.append(BASE64[bits & 0x3F]);
            }
        }

        switch (pData.length % 3) {
            case 1:
                buffer.append("==");
                break;
            case 2:
                buffer.append("=");
                break;
            default:
                break;
        }

        return buffer.toString();
    }

    /**
     * Decodes Base64 string to byte array.
     * 
     * @param pBase64
     *            Base64 string.
     * @return Byte array.
     */
    public static byte[] decode(String pBase64) {
        int padding = pBase64.endsWith("==") ? 2 : (pBase64.endsWith("=") ? 1 : 0);
        byte[] buffer = new byte[((pBase64.length() * 3) / 4) - padding];

        int index = 0;
        for (int i = 0; i < pBase64.length(); i += 4) {
            int i0 = indexes[pBase64.charAt(i)];
            int i1 = indexes[pBase64.charAt(i + 1)];
            buffer[index++] = (byte) (((i0 << 2) | (i1 >> 4)) & 0xFF);
            if (index >= buffer.length) {
                return buffer;
            }
            int i2 = indexes[pBase64.charAt(i + 2)];
            buffer[index++] = (byte) (((i1 << 4) | (i2 >> 2)) & 0xFF);
            if (index >= buffer.length) {
                return buffer;
            }
            int i3 = indexes[pBase64.charAt(i + 3)];
            buffer[index++] = (byte) (((i2 << 6) | i3) & 0xFF);
        }

        return buffer;
    }

}
