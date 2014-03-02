package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 *
 */
public class GenericISSWSignature extends AbstractByteSequence {

    /* fields id */
    private static final int FIELD_ISSW_SIGNATURE = 1;

    private int sizeIsswSignature;

    /**
     * @param size signature size
     */
    public GenericISSWSignature(int size) {
        super();
        sizeIsswSignature = size;
        fields.put(FIELD_ISSW_SIGNATURE, allocateField(sizeIsswSignature));
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
