package com.stericsson.sdk.signing.generic.cert.subparts;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.AbstractByteSequence;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWSecurityRomCustExtSizeInfo extends AbstractByteSequence {

    /* field sizes */

    private static final int SIZE_ROOT_KEY_DATA_SIZE = 4;

    private static final int SIZE_CUST_ROOT_KEY_CERT_SIZE = 4;

    private static final int SIZE_MODEL_ID_LIST_SIZE = 4;

    /* field ids */

    private static final int FIELD_ROOT_KEY_DATA_SIZE = 1;

    private static final int FIELD_CUST_ROOT_KEY_CERT_SIZE = 2;

    private static final int FIELD_MODEL_ID_LIST_SIZE = 3;

    /** */
    public GenericISSWSecurityRomCustExtSizeInfo() {
        super();
        fields.put(FIELD_ROOT_KEY_DATA_SIZE, allocateField(SIZE_ROOT_KEY_DATA_SIZE));
        fields.put(FIELD_CUST_ROOT_KEY_CERT_SIZE, allocateField(SIZE_CUST_ROOT_KEY_CERT_SIZE));
        fields.put(FIELD_MODEL_ID_LIST_SIZE, allocateField(SIZE_MODEL_ID_LIST_SIZE));
    }

    /**
     * @return model id list size
     */
    public int getModelIdListSize() {
        return fields.get(FIELD_MODEL_ID_LIST_SIZE).getInt(0);
    }

    /**
     * @return cust root key certificate size
     */
    public int getCustRootKeyCertSize() {
        return fields.get(FIELD_CUST_ROOT_KEY_CERT_SIZE).getInt(0);
    }

    /**
     * @return root key data size
     */
    public int getRootKeyDataSize() {
        return fields.get(FIELD_ROOT_KEY_DATA_SIZE).getInt(0);
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
