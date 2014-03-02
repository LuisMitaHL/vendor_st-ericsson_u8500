package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;

/**
 * @author xtomzap
 * 
 *         This class represents root key certificate
 */
public class GenericRootKeyCertificate extends GenericCertificate {

    private static final int SIZE_MAGIC = 4;

    private static final int SIZE_VENDOR_ROOT_KEY_ID = 4;

    private static final int SIZE_VENDOR_ROOT_KEY_ISSUER_ID = 4;

    private static final int SIZE_SECONDARY_KEY_SIZE = 4;

    private static final int SIZE_SECONDARY_KEY_SIGANTURE_TYPE = 4;

    private static final int SIZE_SECONDARY_KEY_SIGANTURE_HASH_TYPE = 4;

    private static final int SIZE_SECONDARY_KEY_REVOCATION_VERSION = 4;

    private static final int SIZE_SECONDARY_KEY_ID = 4;

    private static final int SIZE_RESERVED = 16;

    private static final int SIZE_VENDOR_ROOT_KEY = 264;

    private static final int SIZE_SECONDARY_KEY = 264;

    private static final int SIZE_SIGNATURE = 256;

    // /////////

    private static final int FIELD_MAGIC = 1;

    private static final int FIELD_VENDOR_ROOT_KEY_ID = 2;

    private static final int FIELD_VENDOR_ROOT_KEY_ISSUER_ID = 3;

    private static final int FIELD_SECONDARY_KEY_SIZE = 4;

    private static final int FIELD_SECONDARY_KEY_SIGANTURE_TYPE = 5;

    private static final int FIELD_SECONDARY_KEY_SIGANTURE_HASH_TYPE = 6;

    private static final int FIELD_SECONDARY_KEY_REVOCATION_VERSION = 7;

    private static final int FIELD_SECONDARY_KEY_ID = 8;

    private static final int FIELD_RESERVED = 9;

    private static final int FIELD_VENDOR_ROOT_KEY = 10;

    private static final int FIELD_SECONDARY_KEY = 11;

    private static final int FIELD_SIGNATURE = 12;

    /**
     * 
     */
    public GenericRootKeyCertificate() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_VENDOR_ROOT_KEY_ID, allocateField(SIZE_VENDOR_ROOT_KEY_ID));
        fields.put(FIELD_VENDOR_ROOT_KEY_ISSUER_ID, allocateField(SIZE_VENDOR_ROOT_KEY_ISSUER_ID));
        fields.put(FIELD_SECONDARY_KEY_SIZE, allocateField(SIZE_SECONDARY_KEY_SIZE));
        fields.put(FIELD_SECONDARY_KEY_SIGANTURE_TYPE, allocateField(SIZE_SECONDARY_KEY_SIGANTURE_TYPE));
        fields.put(FIELD_SECONDARY_KEY_SIGANTURE_HASH_TYPE, allocateField(SIZE_SECONDARY_KEY_SIGANTURE_HASH_TYPE));
        fields.put(FIELD_SECONDARY_KEY_REVOCATION_VERSION, allocateField(SIZE_SECONDARY_KEY_REVOCATION_VERSION));
        fields.put(FIELD_SECONDARY_KEY_ID, allocateField(SIZE_SECONDARY_KEY_ID));
        fields.put(FIELD_RESERVED, allocateField(SIZE_RESERVED));
        fields.put(FIELD_VENDOR_ROOT_KEY, allocateField(SIZE_VENDOR_ROOT_KEY));
        fields.put(FIELD_SECONDARY_KEY, allocateField(SIZE_SECONDARY_KEY));
        fields.put(FIELD_SIGNATURE, allocateField(SIZE_SIGNATURE));

        setMagic(MAGIC_ROOT_KEY_CERTIFICATE_HEADER);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getHash() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getMagic() {
        return fields.get(FIELD_MAGIC).getInt(0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getSignableData() {
        throw new RuntimeException("GenericRootKeyCertificate.getSignableData() should be never called");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getSignatureSize() {
        return SIZE_SIGNATURE;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setHash(byte[] pData) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setMagic(int magic) {
        fields.get(FIELD_MAGIC).putInt(0, magic);
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

        int magic = fields.get(FIELD_MAGIC).getInt(0);

        if (magic != MAGIC_ROOT_KEY_CERTIFICATE_HEADER) {
            throw new Exception("Magic number is invalid in header.");
        }
    }

    /**
     * @return secondary key signature type
     */
    public int getSKSignatureType() {
        return fields.get(FIELD_SECONDARY_KEY_SIGANTURE_TYPE).getInt(0);
    }

    /**
     * @return secondary key signature hash type
     */
    public int getSKSignatureHashType() {
        return fields.get(FIELD_SECONDARY_KEY_SIGANTURE_HASH_TYPE).getInt(0);
    }
}
