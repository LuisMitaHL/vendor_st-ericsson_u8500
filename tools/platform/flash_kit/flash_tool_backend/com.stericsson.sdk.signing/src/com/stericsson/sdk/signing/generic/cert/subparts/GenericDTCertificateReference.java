package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 *
 */
public class GenericDTCertificateReference extends AbstractByteSequence {

    /* field size */
    private static final int SIZE_REFERENCE_LENGTH = 4;

    /* field id */
    private static final int FIELD_REFERENCE_LENGTH = 1;

    private byte[] valueArray;

    /** */
    public GenericDTCertificateReference() {
        super();
        fields.put(FIELD_REFERENCE_LENGTH, allocateField(SIZE_REFERENCE_LENGTH));
        valueArray = new byte[0];
    }

    /**
     * @param value reference length
     */
    public void setReferenceLength(int value) {
        fields.get(FIELD_REFERENCE_LENGTH).putInt(0, value);
    }

    /**
     * @return reference length
     */
    public int getReferenceLength() {
        return fields.get(FIELD_REFERENCE_LENGTH).getInt(0);
    }

    /**
     * @param data reference value
     */
    public void setReferenceValue(byte[] data) {
        if (data == null) {
            data = new byte[0];
        }
        valueArray = new byte[data.length];
        System.arraycopy(data, 0, valueArray, 0, data.length);
    }

    /**
     * @return reference value
     */
    public byte[] getReferenceValue() {
        return valueArray;
    }

    /**
     * @param data data
     * @param pPos position
     * @throws Exception if data is null or if data.length differs from the length of the byte sequence
     */
    public void setData(byte[] data, int pPos) throws Exception {
        int pos = pPos;
        for (ByteBuffer b : fields.values()) {
            b.rewind();
            byte[] temp = new byte[b.capacity()];
            System.arraycopy(data, pos, temp, 0, b.capacity());
            b.put(temp);
            b.order(BYTE_ORDER);
            pos += b.capacity();
        }
        int valueLength = getReferenceLength();
        valueArray = new byte[valueLength];
        System.arraycopy(data, pos, valueArray, 0, valueLength);
    }

    /**
     * {@inheritDoc}
     */
    public void setData(byte[] data) throws Exception {
        setData(data, 0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();
        System.arraycopy(valueArray, 0, data, length - valueArray.length, valueArray.length);
        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + valueArray.length;
    }
}