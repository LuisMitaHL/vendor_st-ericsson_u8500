package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWUncheckedBlob extends AbstractByteSequence {

    /* fields id */
    private static final int FIELD_UNCHECKED_BLOB = 1;

    private int sizeUncheckedBlob;

    /**
     * @param size
     *            of blob
     */
    public GenericISSWUncheckedBlob(int size) {
        super();
        sizeUncheckedBlob = size;
        if (size > 0) {
            fields.put(FIELD_UNCHECKED_BLOB, allocateField(sizeUncheckedBlob));
        }
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
