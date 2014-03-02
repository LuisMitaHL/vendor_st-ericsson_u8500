package com.stericsson.sdk.equipment.tasks;

import java.util.Arrays;

/**
 * Chip ID enumeration.
 * 
 * @author pkutac01
 * 
 */
public enum ChipID {
    /** */
    L9540(new byte[] {
        0x00, (byte) 0x95, 0x40}, 0x55),
    /** */
    M7400(new byte[] {
        0x00, 0x74, 0x00}),
    /** */
    M7400_V2(new byte[] {
        0x00, 0x05, 0x00}),
    /** */
    T5008(new byte[] {
        0x00, 0x50, 0x08}),
    /** */
    T5008_NOKIA2(new byte[] {
        0x00, 0x00, 0x28}),
    /** */
    U5500(new byte[] {
        0x00, 0x55, 0x00}),
    /** */
    U8500(new byte[] {
        0x00, (byte) 0x85, 0x00}),
    /** */
    U9500(new byte[] {
        0x00, (byte) 0x95, 0x00}),
    /** */
    U8500_NOKIA2(new byte[] {
        0x00, 0x02, 0x18}),
    /** */
    U8520(new byte[] {
        0x00, (byte) 0x85, 0x20}),
    /** */
    MEITP(new byte[] {
        0x00, 0x23, 0x27}),
    /** */
    DUMPING(new byte[] {
        0x00, 0x10, 0x0E});

    private byte[] data;

    private int maxLastByteValue;

    /**
     * Constructor.
     * 
     * @param pData
     *            Chip ID data.
     */
    private ChipID(byte[] pData) {
        data = pData;
        maxLastByteValue = pData[2] & 0xFF;
    }

    /**
     * Constructor. Can be used for a series of chip IDs, e.g. 0x001200, 0x001201 ... 0x001209,
     * instead of specifying each chip ID separately.
     * 
     * @param pData
     *            Chip ID data of the initial chip ID in the series.
     * @param pMaxLastByteValue
     *            Maximal last byte value of the chip ID in the series.
     */
    private ChipID(byte[] pData, int pMaxLastByteValue) {
        data = pData;
        maxLastByteValue = pMaxLastByteValue;
    }

    /**
     * Returns chip ID data.
     * 
     * @return Chip ID data.
     */
    public byte[] getData() {
        return data;
    }

    /**
     * Returns maximal chip ID data last byte value.
     * 
     * @return Maximal chip ID data last byte value.
     */
    public int getMaxLastByteValue() {
        return maxLastByteValue;
    }

    /**
     * Returns chip ID by its data.
     * 
     * @param pData
     *            Chip ID data.
     * @return Chip ID.
     */
    public static ChipID getByData(byte[] pData) {
        for (ChipID chipID : ChipID.values()) {
            byte[] buffer = new byte[chipID.getData().length];
            System.arraycopy(chipID.getData(), 0, buffer, 0, buffer.length);

            for (int i = buffer[2] & 0xFF; i <= chipID.getMaxLastByteValue(); i++) {
                buffer[2] = (byte) i;

                if (Arrays.equals(buffer, pData)) {
                    return chipID;
                }
            }
        }
        return null;
    }
}
