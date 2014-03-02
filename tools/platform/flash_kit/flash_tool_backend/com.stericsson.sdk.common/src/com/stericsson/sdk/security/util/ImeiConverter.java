package com.stericsson.sdk.security.util;


/**
 * @author xdancho
 * 
 */
public final class ImeiConverter {

    private ImeiConverter() {

    }

    /**
     * Returns specified IMEI as a String.
     * 
     * @param imei
     *            The IMEI number in a byte[].
     * @return A String representation of the specified byte[].
     * @throws Exception exception
     */
    public static String getImeiAsString(byte[] imei) throws Exception {
        StringBuffer sb = new StringBuffer();

        for (int i = 0; i < imei.length; i++) {
            if(imei[i] < 10 && imei[i] > -1) {
                sb.append(imei[i]);
            } else {
                throw new Exception("Not a valid IMEI number");
            }
        }

        return sb.toString();
    }

    /**
     * Strips, checks IMEI and converts it to byte representation.
     * 
     * @param imei
     *            IMEI
     * @return IMEI in byte representation
     */
    public static byte[] getImeiAsBytes(String imei) {

        byte[] temp = new byte[imei.length()];
        for (int i = 0; i < imei.length(); i++) {
            temp[i] = Integer.decode(imei.substring(i, i + 1)).byteValue();
        }

        return temp;
    }



    /**
     * @param imei
     *            in string format
     * @return encoded in compressed format
     */
    public static byte[] encodeIMEI(String imei) {

        byte[] result = new byte[6];

        // pad
        StringBuffer imeiBuffer = new StringBuffer(imei);
        while (imeiBuffer.length() < 14) {
            imeiBuffer.insert(0, "0");
        }
        imei = imeiBuffer.toString();
        // divide
        String tac = imei.substring(0, 6);
        String fac = imei.substring(6, 8);
        String srn = imei.substring(8, 14);

        int tacInt = Integer.parseInt(tac);
        int facInt = Integer.parseInt(fac);
        int srnInt = Integer.parseInt(srn);

        result[0] = (byte) (tacInt & (int) 0x000000FF);
        result[1] = (byte) ((tacInt & (int) 0x0000FF00) >> 8);
        result[2] = (byte) ((tacInt & (int) 0x000F0000) >> 16);
        result[2] |= (byte) ((facInt & (int) 0x0000000F) << 4);
        result[3] = (byte) ((facInt & (int) 0x000000F0) >> 4);
        result[3] |= (byte) ((srnInt & (int) 0x0000000F) << 4);
        result[4] = (byte) ((srnInt & (int) 0x00000FF0) >> 4);
        result[5] = (byte) ((srnInt & (int) 0x000FF000) >> 12);

        return result;

    }

    /**
     * @param imeiData
     *            imei in compressed format
     * @return imei decoded to String format
     */
    public static String decodeIMEI(byte[] imeiData) {

        String result;

        int tacInt;
        int facInt;
        int srnInt;

        tacInt = (imeiData[0] & 0xFF);

        tacInt += (imeiData[1] & 0xFF) << 8;
        tacInt += ((imeiData[2] & 0xFF) & 0x0F) << 16;

        facInt = ((imeiData[2] & 0xFF) & 0xF0) >> 4;
        facInt += ((imeiData[3] & 0xFF) & 0x0F) << 4;

        srnInt = ((imeiData[3] & 0xFF) & 0xF0) >> 4;
        srnInt += (imeiData[4] & 0xFF) << 4;
        srnInt += (imeiData[5] & 0xFF) << 12;

        StringBuffer tacBuffer = new StringBuffer(Integer.toString(tacInt));
        while (tacBuffer.length() < 6) {
            tacBuffer.insert(0, "0");
        }
        String tac = tacBuffer.toString();

        StringBuffer facBuffer = new StringBuffer(Integer.toString(facInt));
        while (facBuffer.length() < 2) {
            facBuffer.insert(0, "0");
        }
        String fac = facBuffer.toString();

        StringBuffer srnBuffer = new StringBuffer(Integer.toString(srnInt));
        while (srnBuffer.length() < 6) {
            srnBuffer.insert(0, "0");
        }

        String srn = srnBuffer.toString();
        result = tac + fac + srn;

        return result;

    }

}
