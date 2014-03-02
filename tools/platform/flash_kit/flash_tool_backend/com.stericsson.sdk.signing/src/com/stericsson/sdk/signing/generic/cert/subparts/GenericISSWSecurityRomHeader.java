package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWSecurityRomHeader extends AbstractByteSequence {

    /** */
    public static final int SIZE_GENERIC = 392;

    /* field sizes */

    private static final int SIZE_MAGIC = 4;

    private static final int SIZE_ROOT_KEY_SIZE = 4;

    private static final int SIZE_CUSTOMER_PART_SIZE = 4;

    private static final int SIZE_UNCHECKED_BLOB_SIZE = 4;

    private static final int SIZE_SIGNATURE_SIZE = 4;

    private static final int SIZE_SPEEDUP_MAGIC = 4;

    private static final int SIZE_SPEEDUP_MASK = 4;

    private static final int SIZE_SPEEDUP_DATA = 256;

    private static final int SIZE_SPEEDUP_POLL = 4;

    private static final int SIZE_SPEEDUP_SPARE = 4;

    private static final int SIZE_ISSW_HASH_TYPE = 4;

    private static final int SIZE_ISSW_HASH = 64;

    private static final int SIZE_CODE_LENGTH = 4;

    private static final int SIZE_RW_DATA_LENGTH = 4;

    private static final int SIZE_ZI_DATA_LENGTH = 4;

    private static final int SIZE_ISSW_LOAD_LOCATION = 4;

    private static final int SIZE_ISSW_START_ADDR = 4;

    private static final int SIZE_ROOT_KEY_TYPE = 4;

    private static final int SIZE_ROOT_KEY_HASH_TYPE = 4;

    private static final int SIZE_SIGNATURE_HASH_TYPE = 4;

    /* field ids */

    private static final int FIELD_MAGIC = 1;

    private static final int FIELD_ROOT_KEY_SIZE = 2;

    private static final int FIELD_CUSTOMER_PART_SIZE = 3;

    private static final int FIELD_UNCHECKED_BLOB_SIZE = 4;

    private static final int FIELD_SIGNATURE_SIZE = 5;

    private static final int FIELD_SPEEDUP_MAGIC = 6;

    private static final int FIELD_SPEEDUP_MASK = 7;

    private static final int FIELD_SPEEDUP_DATA = 8;

    private static final int FIELD_SPEEDUP_POLL = 9;

    private static final int FIELD_SPEEDUP_SPARE = 10;

    private static final int FIELD_ISSW_HASH_TYPE = 11;

    private static final int FIELD_ISSW_HASH = 12;

    private static final int FIELD_CODE_LENGTH = 13;

    private static final int FIELD_RW_DATA_LENGTH = 14;

    private static final int FIELD_ZI_DATA_LENGTH = 15;

    private static final int FIELD_ISSW_LOAD_LOCATION = 16;

    private static final int FIELD_ISSW_START_ADDR = 17;

    private static final int FIELD_ROOT_KEY_TYPE = 18;

    private static final int FIELD_ROOT_KEY_HASH_TYPE = 19;

    private static final int FIELD_SIGNATURE_HASH_TYPE = 20;

    /** */
    public GenericISSWSecurityRomHeader() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_ROOT_KEY_SIZE, allocateField(SIZE_ROOT_KEY_SIZE));
        fields.put(FIELD_CUSTOMER_PART_SIZE, allocateField(SIZE_CUSTOMER_PART_SIZE));
        fields.put(FIELD_UNCHECKED_BLOB_SIZE, allocateField(SIZE_UNCHECKED_BLOB_SIZE));
        fields.put(FIELD_SIGNATURE_SIZE, allocateField(SIZE_SIGNATURE_SIZE));
        fields.put(FIELD_SPEEDUP_MAGIC, allocateField(SIZE_SPEEDUP_MAGIC));
        fields.put(FIELD_SPEEDUP_MASK, allocateField(SIZE_SPEEDUP_MASK));
        fields.put(FIELD_SPEEDUP_DATA, allocateField(SIZE_SPEEDUP_DATA));
        fields.put(FIELD_SPEEDUP_POLL, allocateField(SIZE_SPEEDUP_POLL));
        fields.put(FIELD_SPEEDUP_SPARE, allocateField(SIZE_SPEEDUP_SPARE));
        fields.put(FIELD_ISSW_HASH_TYPE, allocateField(SIZE_ISSW_HASH_TYPE));
        fields.put(FIELD_ISSW_HASH, allocateField(SIZE_ISSW_HASH));
        fields.put(FIELD_CODE_LENGTH, allocateField(SIZE_CODE_LENGTH));
        fields.put(FIELD_RW_DATA_LENGTH, allocateField(SIZE_RW_DATA_LENGTH));
        fields.put(FIELD_ZI_DATA_LENGTH, allocateField(SIZE_ZI_DATA_LENGTH));
        fields.put(FIELD_ISSW_LOAD_LOCATION, allocateField(SIZE_ISSW_LOAD_LOCATION));
        fields.put(FIELD_ISSW_START_ADDR, allocateField(SIZE_ISSW_START_ADDR));
        fields.put(FIELD_ROOT_KEY_TYPE, allocateField(SIZE_ROOT_KEY_TYPE));
        fields.put(FIELD_ROOT_KEY_HASH_TYPE, allocateField(SIZE_ROOT_KEY_HASH_TYPE));
        fields.put(FIELD_SIGNATURE_HASH_TYPE, allocateField(SIZE_SIGNATURE_HASH_TYPE));

        setMagic(GenericISSWCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED);
        setUncheckedBlobSize(0x0);
        setSpeedUpMagic(GenericISSWCertificate.MAGIC_ISSW_SPEEDUP);
        setISSWHashType(HashType.SHA1_HASH);
        setRootKeyHashType(HashType.SHA1_HASH);
        setSignatureHashType(HashType.SHA1_HASH);
        setRootKeyType(SignatureType.RSASSA_PKCS_V1_5);
    }

    /**
     * @param value
     *            magic
     */
    public void setMagic(int value) {
        fields.get(FIELD_MAGIC).putInt(0, value);
    }

    /**
     * @return magic
     */
    public int getMagic() {
        return fields.get(FIELD_MAGIC).getInt(0);
    }

    /**
     * @param value
     *            root key size
     */
    public void setRootKeySize(int value) {
        fields.get(FIELD_ROOT_KEY_SIZE).putInt(0, value);
    }

    /**
     * @return root key size
     */
    public int getRootKeySize() {
        return fields.get(FIELD_ROOT_KEY_SIZE).getInt(0);
    }

    /**
     * @param value
     *            customer part size
     */
    public void setCustomerPartSize(int value) {
        fields.get(FIELD_CUSTOMER_PART_SIZE).putInt(0, value);
    }

    /**
     * @return customer part size
     */
    public int getCustomerPartSize() {
        return fields.get(FIELD_CUSTOMER_PART_SIZE).getInt(0);
    }

    /**
     * @param value
     *            unchecked blob size
     */
    public void setUncheckedBlobSize(int value) {
        fields.get(FIELD_UNCHECKED_BLOB_SIZE).putInt(0, value);
    }

    /**
     * @return unchecked blob size
     */
    public int getUncheckedBlobSize() {
        return fields.get(FIELD_UNCHECKED_BLOB_SIZE).getInt(0);
    }

    /**
     * @param value
     *            signature size
     */
    public void setSignatureSize(int value) {
        fields.get(FIELD_SIGNATURE_SIZE).putInt(0, value);
    }

    /**
     * 
     * @return signature size
     */
    public int getSignatureSize() {
        return fields.get(FIELD_SIGNATURE_SIZE).getInt(0);
    }

    /**
     * @param value
     *            speedup magic
     */
    public void setSpeedUpMagic(int value) {
        fields.get(FIELD_SPEEDUP_MAGIC).putInt(0, value);
    }

    /**
     * @return speedup magic
     */
    public int getSpeedUpMagic() {
        return fields.get(FIELD_SPEEDUP_MAGIC).getInt(0);
    }

    /**
     * @param value
     *            speedup mask
     */
    public void setSpeedUpMask(int value) {
        fields.get(FIELD_SPEEDUP_MASK).putInt(0, value);
    }

    /**
     * @return speedup mask
     */
    public int getSpeedUpMask() {
        return fields.get(FIELD_SPEEDUP_MASK).getInt(0);
    }

    /**
     * @param data
     *            speedup data
     */
    public void setSpeedUpData(byte[] data) {
        ByteBuffer buffer = fields.get(FIELD_SPEEDUP_DATA);
        buffer.clear();
        buffer.put(data);
    }

    /**
     * @return speedup data
     */
    public byte[] getSpeedUpData() {
        ByteBuffer speedUpBuffer = fields.get(FIELD_SPEEDUP_DATA);
        speedUpBuffer.position(0);
        byte[] speedUpArray = new byte[speedUpBuffer.capacity()];
        speedUpBuffer.get(speedUpArray);
        return speedUpArray;
    }

    /**
     * @param data
     *            speedup pool
     */
    public void setSpeedUpPool(byte[] data) {
        ByteBuffer buffer = fields.get(FIELD_SPEEDUP_POLL);
        buffer.clear();
        buffer.put(data);
    }

    /**
     * @return speedup pool
     */
    public byte[] getSpeedUpPool() {
        ByteBuffer speedUpBuffer = fields.get(FIELD_SPEEDUP_POLL);
        speedUpBuffer.position(0);
        byte[] speedUpArray = new byte[speedUpBuffer.capacity()];
        speedUpBuffer.get(speedUpArray);
        return speedUpArray;
    }

    /**
     * @param data
     *            speedup spare
     */
    public void setSpeedUpSpare(byte[] data) {
        ByteBuffer buffer = fields.get(FIELD_SPEEDUP_SPARE);
        buffer.clear();
        buffer.put(data);
    }

    /**
     * @return speedup spare
     */
    public byte[] getSpeedUpSpare() {
        ByteBuffer speedUpBuffer = fields.get(FIELD_SPEEDUP_SPARE);
        speedUpBuffer.position(0);
        byte[] speedUpArray = new byte[speedUpBuffer.capacity()];
        speedUpBuffer.get(speedUpArray);
        return speedUpArray;
    }

    /**
     * @param type
     *            issw hash type
     */
    public void setISSWHashType(HashType type) {
        fields.get(FIELD_ISSW_HASH_TYPE).putInt(0, type.getHashType());
    }

    /**
     * @return issw hash type
     */
    public int getISSWHashType() {
        return fields.get(FIELD_ISSW_HASH_TYPE).getInt(0);
    }

    /**
     * @param pData
     *            hash
     */
    public void setHash(byte[] pData) {
        ByteBuffer buffer = fields.get(FIELD_ISSW_HASH);
        buffer.clear();
        buffer.put(pData);
    }

    /**
     * @return hash
     */
    public byte[] getHash() {
        ByteBuffer hashBuffer = fields.get(FIELD_ISSW_HASH);
        hashBuffer.position(0);
        byte[] hashArray = new byte[hashBuffer.capacity()];
        hashBuffer.get(hashArray);
        return hashArray;
    }

    /**
     * @param value
     *            code length
     */
    public void setCodeLength(int value) {
        fields.get(FIELD_CODE_LENGTH).putInt(0, value);
    }

    /**
     * @return code length
     */
    public int getCodeLength() {
        return fields.get(FIELD_CODE_LENGTH).getInt(0);
    }

    /**
     * @param value
     *            rw data length
     */
    public void setRWDataLength(int value) {
        fields.get(FIELD_RW_DATA_LENGTH).putInt(0, value);
    }

    /**
     * @return rw data length
     */
    public int getRWDataLength() {
        return fields.get(FIELD_RW_DATA_LENGTH).getInt(0);
    }

    /**
     * @param value
     *            zi data length
     */
    public void setZIDataLength(int value) {
        fields.get(FIELD_ZI_DATA_LENGTH).putInt(0, value);
    }

    /**
     * @return zi data length
     */
    public int getZIDataLength() {
        return fields.get(FIELD_ZI_DATA_LENGTH).getInt(0);
    }

    /**
     * @param value
     *            issw load location
     */
    public void setISSWLoadLocation(int value) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_ISSW_LOAD_LOCATION).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * @return issw load location
     */
    public int getISSWLoadLocation() {
        return fields.get(FIELD_ISSW_LOAD_LOCATION).getInt(0);
    }

    /**
     * @param value
     *            issw start addr
     */
    public void setISSWStartAddr(int value) {
        fields.get(FIELD_ISSW_START_ADDR).putInt(0, value);
    }

    /**
     * @return issw start addr
     */
    public int getISSWStartAddr() {
        return fields.get(FIELD_ISSW_START_ADDR).getInt(0);
    }

    /**
     * @param type
     *            root key type
     */
    public void setRootKeyType(SignatureType type) {
        fields.get(FIELD_ROOT_KEY_TYPE).putInt(0, type.getSignatureType());
    }

    /**
     * @return root key type
     */
    public int getRootKeyType() {
        return fields.get(FIELD_ROOT_KEY_TYPE).getInt(0);
    }

    /**
     * @param type
     *            root key hash type
     */
    public void setRootKeyHashType(HashType type) {
        fields.get(FIELD_ROOT_KEY_HASH_TYPE).putInt(0, type.getHashType());
    }

    /**
     * @return root key hash type
     */
    public int getRootKeyHashType() {
        return fields.get(FIELD_ROOT_KEY_HASH_TYPE).getInt(0);
    }

    /**
     * @param type
     *            signature hash type
     */
    public void setSignatureHashType(HashType type) {
        fields.get(FIELD_SIGNATURE_HASH_TYPE).putInt(0, type.getHashType());
    }

    /**
     * @return signature hash type
     */
    public int getSignatureHashType() {
        return fields.get(FIELD_SIGNATURE_HASH_TYPE).getInt(0);
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
