package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 * 
 */
public class GenericKey extends AbstractByteSequence {

    /* field sizes */

    private static final int SIZE_PUBLIC_EXPONENT = 4;

    private static final int SIZE_DUMMY = 2;

    private static final int SIZE_MODULUS_SIZE = 2;

    /* field ids */

    private static final int FIELD_PUBLIC_EXPONENT = 1;

    private static final int FIELD_DUMMY = 2;

    private static final int FIELD_SIZE_MODULUS = 3;

    /* fields */

    private byte[] modulus;

    /** */
    public GenericKey() {
        super();
        fields.put(FIELD_PUBLIC_EXPONENT, allocateField(SIZE_PUBLIC_EXPONENT));
        fields.put(FIELD_DUMMY, allocateField(SIZE_DUMMY));
        fields.put(FIELD_SIZE_MODULUS, allocateField(SIZE_MODULUS_SIZE));

        modulus = new byte[0];
    }

    /**
     * @param value
     *            public exponent
     */
    public void setPublicExponent(int value) {
        fields.get(FIELD_PUBLIC_EXPONENT).putInt(0, value);
    }

    /**
     * @return public exponent
     */
    public int getPublicExponent() {
        return fields.get(FIELD_PUBLIC_EXPONENT).getInt(0);
    }

    /**
     * @param value
     *            dummy
     */
    public void setDummy(short value) {
        fields.get(FIELD_DUMMY).putShort(0, value);
    }

    /**
     * @return dummy
     */
    public short getDummy() {
        return fields.get(FIELD_DUMMY).getShort(0);
    }

    /**
     * @param value
     *            modulus size
     */
    public void setModulusSize(short value) {
        // dividing the value by 2 since the ROM code read modulus by two bytes (uint16)
        // example: if modulus has 256 bytes then modulus size has to be 256/2 = 128, because
        // ROM code reads 128 * two bytes (uint16), i.e. 256
        fields.get(FIELD_SIZE_MODULUS).putShort(0, (short) (value / 2));
    }

    /**
     * @return modulus size
     */
    public short getModulusSize() {
        // multiplying by 2 to get the actual number of bytes
        // see setModulusSize(short value)
        return (short) (fields.get(FIELD_SIZE_MODULUS).getShort(0) * 2);
    }

    /**
     * @param data
     *            modulus
     */
    public void setModulus(byte[] data) {
        if (data == null) {
            data = new byte[0];
        }
        modulus = new byte[data.length];
        System.arraycopy(data, 0, modulus, 0, data.length);
    }

    /**
     * @return modulus
     */
    public byte[] getModulus() {
        return modulus;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setData(byte[] data) throws Exception {
        setData(data, 0);
    }

    /**
     * @param data
     *            data
     * @param pPos
     *            position
     * @throws Exception
     *             if data is null or if data.length differs from the length of the byte sequence
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

        modulus = new byte[getModulusSize()];
        System.arraycopy(data, pos, modulus, 0, getModulusSize());
        pos += modulus.length;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();
        System.arraycopy(modulus, 0, data, length - modulus.length, modulus.length);
        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + modulus.length;
    }
}
