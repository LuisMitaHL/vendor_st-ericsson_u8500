package com.stericsson.sdk.common.gdfs;


/**
 * GDData represents a GDFS parameter in the GDFS area.
 * 
 * @author ecspasc
 */
public class GDData implements Comparable<GDData> {

    /**
     * The alias of this GDFS param.
     */
    private String mAlias;

    /**
     * The GDFS block it is stored in.
     */
    private int mBlock;

    /**
     * The GDFS index of this param.
     */
    private int mIndex;

    /**
     * The data/value of this param.
     */
    private byte[] mData = new byte[0];

    /**
     * Constructor, creates an empty GDFS param with empty alias, no data in index and block -1.
     */
    public GDData() {
    }

    /**
     * Creates a GDFS param with all values set.
     * 
     * @param block
     *            Block of the GD data.
     * @param index
     *            Index of the GD data.
     * @param data
     *            Data part of the GD data.
     */
    public GDData(int block, int index, byte[] data) {
        mBlock = block;
        mIndex = index;
        mData = data;
    }

    /**
     * Compares one GDData to another.
     * 
     * @param o
     *            The GDData to compare with.
     * @return comparison result.
     */
    public int compareTo(GDData o) {
        return Long.valueOf(getIndex() & 0xFFFFFFFFL).compareTo(o.getIndex() & 0xFFFFFFFFL);
    }

    /**
     * Sets the alias for this data.
     * 
     * @param alias
     *            The alias to use.
     */
    public void setAlias(String alias) {
        mAlias = alias;
    }

    /**
     * Returns the alias for this param.
     * 
     * @return The alias.
     */
    public String getAlias() {
        return mAlias;
    }

    /**
     * Sets the block indicator for this param.
     * 
     * @param block
     *            The block number.
     */
    public void setBlock(int block) {
        mBlock = block;
    }

    /**
     * Returns the block number for this param.
     * 
     * @return The block number.
     */
    public int getBlock() {
        return mBlock;
    }

    /**
     * Sets the GDFS index of this param.
     * 
     * @param index
     *            The GDFS index.
     */
    public void setIndex(int index) {
        mIndex = index;
    }

    /**
     * Returns the GDFS index for this param.
     * 
     * @return The GDFS index.
     */
    public int getIndex() {
        return mIndex;
    }

    /**
     * Sets the data for this GDFS param.
     * 
     * @param data
     *            The data to set.
     */
    public void setData(byte[] data) {
        mData = data;
    }

    /**
     * Returns the data for this param.
     * 
     * @return The data.
     */
    public byte[] getData() {
        return mData;
    }

    /**
     * Returns the data length of this param.
     * 
     * @return The length of the data.
     */
    public int getLength() {
        return mData.length;
    }

    /**
     * Returns a text representation of this param.
     * 
     * @return This param as text.
     */
    public String toString() {
        String unit = Integer.toHexString(getIndex());
        StringBuffer prefixBuf = new StringBuffer();
        while (unit.length() + prefixBuf.length() < 8) {
            prefixBuf.append("0");
        }
        unit = prefixBuf.toString() + unit;

        String length = Integer.toString(getLength());

        String block = Integer.toHexString(getBlock());
        prefixBuf = new StringBuffer();
        while (block.length() + prefixBuf.length() < 4) {
            prefixBuf.append("0");
        }
        block = prefixBuf.toString() + block;

        StringBuffer sb = new StringBuffer();

        sb.append(unit);
        sb.append(" ");
        sb.append(length);
        sb.append(" ");
        sb.append(block);

        byte[] data = getData();
        int written = 0;
        while (written < data.length) {
            sb.append("\n\t");
            for (int i = 0; i < 16 && written < data.length; i++) {
                sb.append(" ");
                String val = Integer.toHexString((data[written++] & 0xFF));
                if (val.length() < 2) {
                    val = "0" + val;
                }
                sb.append(val);
            }
        }

        return sb.toString();
    }
}
