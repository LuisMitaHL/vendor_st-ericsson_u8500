package com.stericsson.sdk.signing.generic.cert.u8500;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Set;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericDTCertificate;

/**
 * U8500 generic fuse configuration D&T certificate
 * 
 * @author xtomzap
 * 
 */
public class U8500DTCertificate extends GenericDTCertificate {



    /** */
    public U8500DTCertificate() {
        super();
        fields.put(FIELD_FLAGS, allocateField(SIZE_FLAGS));
        fields.put(FIELD_RESERVED, allocateField(SIZE_RESERVED));

        setMagic(GenericCertificate.MAGIC_U8500_DT_CERTIFICATE);
    }

    /**
     * @param value
     *            flags
     */
    public void setFlags(int value) {
        fields.get(FIELD_FLAGS).putInt(0, value);
    }

    /**
     * @return flags
     */
    public int getFlags() {
        return fields.get(FIELD_FLAGS).getInt(0);
    }

    /**
     * @param data
     *            reserved
     */
    public void setReserved(byte[] data) {
        fields.get(FIELD_RESERVED).clear();
        fields.get(FIELD_RESERVED).put(data);
    }

    /**
     * @return hash of magic
     */
    public byte[] getReserved() {
        ByteBuffer reservedBuffer = fields.get(FIELD_RESERVED);
        reservedBuffer.position(0);
        byte[] arrayArray = new byte[reservedBuffer.capacity()];
        reservedBuffer.get(arrayArray);
        return arrayArray;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getSignableData() {
        int length = getLength();
        byte[] dataWithSignature = getCertData();

        // remove signature
        byte[] data = removeSignature(dataWithSignature);
        // add reference
        int actualPosition = addReference(length, data);
        // add key replacement list
        actualPosition = addKeyReplacementList(data, actualPosition);

        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setData(byte[] data) throws Exception {
        int pos = 0;
        Integer[] keyArray = getSortedKeyArray();
        for (Integer i : keyArray) {
            ByteBuffer b = fields.get(i);
            b.rewind();
            byte[] temp = new byte[b.capacity()];
            System.arraycopy(data, pos, temp, 0, b.capacity());
            b.put(temp);
            b.order(BYTE_ORDER);
            pos += b.capacity();
        }

        // set reference
        pos = setReferenceData(data, pos);
        // reset keyList and fill it again
        pos = setKeyListData(data, pos);
        // set signature
        setSignatureData(data, pos);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = getCertData();

        // add reference
        int actualPosition = addReference(length, data);
        // add key replacement list
        actualPosition = addKeyReplacementList(data, actualPosition);
        // add signature
        actualPosition = addSignature(data, actualPosition);

        return data;
    }

    private byte[] getCertData() {
        if (getLength() == 0) {
            return new byte[0];
        }

        ByteBuffer result = ByteBuffer.allocate(getLength());

        Integer[] keyArray = getSortedKeyArray();
        for (Integer i : keyArray) {
            ByteBuffer b = fields.get(i);
            b.rewind();
            result.put(b);
        }
        result.order(BYTE_ORDER);
        return result.array();
    }

    private Integer[] getSortedKeyArray() {
        Set<Integer> keySet = fields.keySet();
        Integer[] keyArray = new Integer[keySet.size()];
        keySet.toArray(keyArray);
        Arrays.sort(keyArray);
        return keyArray;
    }
}
