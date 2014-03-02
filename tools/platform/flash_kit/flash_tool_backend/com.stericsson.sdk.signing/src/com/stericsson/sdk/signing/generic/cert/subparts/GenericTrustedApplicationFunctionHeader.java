package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 *
 */
public class GenericTrustedApplicationFunctionHeader extends AbstractByteSequence {

    /* fields sizes */

    private static final int SIZE_ID = 4;

    private static final int SIZE_START = 4;

    /* fields ids */

    private static final int FIELD_ID = 1;

    private static final int FIELD_START = 2;

    /** */
    public GenericTrustedApplicationFunctionHeader() {
        super();
        fields.put(FIELD_ID, allocateField(SIZE_ID));
        fields.put(FIELD_START, allocateField(SIZE_START));
    }

    /**
     * @param value
     *            id
     */
    public void setID(int value) {
        fields.get(FIELD_ID).putInt(0, value);
    }

    /**
     * @return id
     */
    public int getID() {
        return fields.get(FIELD_ID).getInt(0);
    }

    /**
     * @param value
     *            start
     */
    public void setStart(int value) {
        fields.get(FIELD_START).putInt(0, value);
    }

    /**
     * @return start
     */
    public int getStart() {
        return fields.get(FIELD_START).getInt(0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setData(byte[] data) throws Exception {
        int pos = 0;
        for (ByteBuffer b : fields.values()) {
            b.rewind();
            byte[] temp = new byte[b.capacity()];
            System.arraycopy(data, pos, temp, 0, b.capacity());
            b.put(temp);
            b.order(BYTE_ORDER);
            pos += b.capacity();
        }
    }
}