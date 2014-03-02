package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWCustomerPartKey extends AbstractByteSequence {

    /* field size */

    private static final int SIZE_KEY_SIZE = 4;

    private static final int SIZE_KEY_TYPE = 4;

    /* field id */

    private static final int FIELD_KEY_SIZE = 1;

    private static final int FIELD_KEY_TYPE = 2;

    /* fields */

    private GenericKey key;

    /** */
    public GenericISSWCustomerPartKey() {
        super();
        fields.put(FIELD_KEY_SIZE, allocateField(SIZE_KEY_SIZE));
        fields.put(FIELD_KEY_TYPE, allocateField(SIZE_KEY_TYPE));

        key = new GenericKey();
    }

    /**
     * @param value
     *            key size
     */
    public void setKeySize(int value) {
        fields.get(FIELD_KEY_SIZE).putInt(0, value);
    }

    /**
     * @return key size
     */
    public int getKeySize() {
        return fields.get(FIELD_KEY_SIZE).getInt(0);
    }

    /**
     * @param type
     *            key type
     */
//    public void setKeyType(SignatureType type) {
//        fields.get(FIELD_KEY_TYPE).putInt(0, type.getSignatureType());
//    }

    /**
     * @param type
     *            key type
     */
    public void setKeyTypeWithRevocationHashTypes(SignatureTypeWithRevocationHashTypes type) {
     // set the signature type (bits 0-7) and revocation hash types (bits 8-15)
        fields.get(FIELD_KEY_TYPE).putInt(0, type.getSignatureType().getSignatureType() | type.getRevocationHashTypes()); 
    }

    /**
     * @return key type
     */
    public int getKeyType() {
      //return of the least significant byte (bits 0-7) which represent the signature type
        return fields.get(FIELD_KEY_TYPE).getInt(0) & 0xFF;
    }

    /**
     * @return key type
     */
    public int getKeyTypeWithRevocationHashTypes() {

        return fields.get(FIELD_KEY_TYPE).getInt(0);
    }

    /**
     * @param value
     *            exponent
     */
    public void setExponent(int value) {
        key.setPublicExponent(value);
    }

    /**
     * @return exponent
     */
    public int getExponent() {
        return key.getPublicExponent();
    }

    /**
     * @param value
     *            dummy
     */
    public void setDummy(short value) {
        key.setDummy(value);
    }

    /**
     * @return dummy
     */
    public short getDummy() {
        return key.getDummy();
    }

    /**
     * @param value
     *            modulus size
     */
    public void setModulusSize(short value) {
        key.setModulusSize(value);
    }

    /**
     * @return modulus size
     */
    public short getModulusSize() {
        return key.getModulusSize();
    }

    /**
     * @param data
     *            modulus
     */
    public void setModulus(byte[] data) {
        key.setModulus(data);
    }

    /**
     * @return modulus
     */
    public byte[] getModulus() {
        return key.getModulus();
    }

    /**
     * @return key key
     */
    public GenericKey getKey() {
        return key;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setData(byte[] data) throws Exception {
        setData(data, 0);
    }

    /**
     * Replaces the sequence data with the data contained in <code>data</code>
     * 
     * @param data
     *            the data to be inserted in the sequence
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

        key = new GenericKey();
        key.setData(data, pos);
        pos += key.getLength();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();
        System.arraycopy(key.getData(), 0, data, length - key.getLength(), key.getLength());
        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + key.getLength();
    }
}
