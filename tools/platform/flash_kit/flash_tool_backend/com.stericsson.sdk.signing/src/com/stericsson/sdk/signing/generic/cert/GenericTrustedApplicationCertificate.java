package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;
import java.util.ArrayList;

import com.stericsson.sdk.signing.AbstractByteSequence;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericTrustedApplicationFunctionHeader;

/**
 * @author xtomzap
 * 
 */
public class GenericTrustedApplicationCertificate extends AbstractByteSequence {

    /** */
    public static final int HASH_LIST_PAGE_SIZE = 4096;

    private static final int SIZE_STATIC_FIELDS = 40;

    /* fields size */
    private static final int SIZE_TEEC_UUID = 16;

    private static final int SIZE_NUMBER_OF_TAFS = 4;

    private static final int SIZE_RO_SIZE = 4;

    private static final int SIZE_RW_SIZE = 4;

    private static final int SIZE_ZI_SIZE = 4;

    private static final int SIZE_FLAGS = 4;

    private static final int SIZE_HASH_TYPE = 4;

    /* fields id */
    private static final int FIELD_TEEC_UUID = 1;

    private static final int FIELD_NUMBER_OF_TAFS = 2;

    private static final int FIELD_RO_SIZE = 3;

    private static final int FIELD_RW_SIZE = 4;

    private static final int FIELD_ZI_SIZE = 5;

    private static final int FIELD_FLAGS = 6;

    private static final int FIELD_HASH_TYPE = 7;

    /* fields */

    private ArrayList<GenericTrustedApplicationFunctionHeader> tafHeadersList;

    private byte[] trustedAppCodeAndData;

    private ArrayList<byte[]> payloadHashList;

    /** */
    public GenericTrustedApplicationCertificate() {
        super();
        fields.put(FIELD_TEEC_UUID, allocateField(SIZE_TEEC_UUID));
        fields.put(FIELD_NUMBER_OF_TAFS, allocateField(SIZE_NUMBER_OF_TAFS));
        fields.put(FIELD_RO_SIZE, allocateField(SIZE_RO_SIZE));
        fields.put(FIELD_RW_SIZE, allocateField(SIZE_RW_SIZE));
        fields.put(FIELD_ZI_SIZE, allocateField(SIZE_ZI_SIZE));
        fields.put(FIELD_FLAGS, allocateField(SIZE_FLAGS));
        fields.put(FIELD_HASH_TYPE, allocateField(SIZE_HASH_TYPE));
        tafHeadersList = new ArrayList<GenericTrustedApplicationFunctionHeader>();
        trustedAppCodeAndData = new byte[0];
        payloadHashList = new ArrayList<byte[]>();

    }

    /**
     * @param data
     *            header data
     */
    public void setTeecUuid(byte[] data) {
        fields.get(FIELD_TEEC_UUID).clear();
        fields.get(FIELD_TEEC_UUID).put(data);
    }

    /**
     * 
     * @return TEEC UUID
     */
    public byte[] getTeecUuid() {
        return fields.get(FIELD_TEEC_UUID).array();
    }

    /**
     * 
     * @param value
     *            number of trusted application funtions
     */
    public void setNumberOfTafs(int value) {
        fields.get(FIELD_NUMBER_OF_TAFS).putInt(0, value);
    }

    /**
     * 
     * @return number of trusted application funtions
     */
    public int getNumberOfTafs() {
        return fields.get(FIELD_NUMBER_OF_TAFS).getInt(0);
    }

    /**
     * 
     * @param value of read only data
     */
    public void setRoSize(int value) {
        fields.get(FIELD_RO_SIZE).putInt(0, value);
    }

    /**
     * 
     * @return size of read only data
     */
    public int getRoSize() {
        return fields.get(FIELD_RO_SIZE).getInt(0);
    }

    /**
     * @param value size of read write data
     */
    public void setRwSize(int value) {
        fields.get(FIELD_RW_SIZE).putInt(0, value);
    }

    /**
     * 
     * @return size of read write data
     */
    public int getRwSize() {
        return fields.get(FIELD_RW_SIZE).getInt(0);
    }

    /**
     * @param value size of zi data
     */
    public void setZiSize(int value) {
        fields.get(FIELD_ZI_SIZE).putInt(0, value);
    }

    /**
     * 
     * @return size of zi data
     */
    public int getZiSize() {
        return fields.get(FIELD_ZI_SIZE).getInt(0);
    }

    /**
     * @param value flags
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
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        byte[] data = super.getData();
        int pos = SIZE_STATIC_FIELDS;

        // add TAFs
        for (GenericTrustedApplicationFunctionHeader header : tafHeadersList) {
            byte[] headerData = header.getData();
            System.arraycopy(headerData, 0, data, pos, headerData.length);
            pos += header.getLength();
        }

        // add app code+data
        System.arraycopy(trustedAppCodeAndData, 0, data, pos, trustedAppCodeAndData.length);
        pos += trustedAppCodeAndData.length;

        // add hash list
        for (byte[] b : payloadHashList) {
            if (b != null) {
                System.arraycopy(b, 0, data, pos, b.length);
                pos += b.length;
            }
        }

        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + getHeadersLength() + getPayloadHashesLength() + trustedAppCodeAndData.length;
    }

    private int getPayloadHashesLength() {
        int length = 0;
        for (byte[] b : payloadHashList) {
            if (b != null) {
                length += b.length;
            }
        }
        return length;
    }

    private int getHeadersLength() {
        int length = 0;
        for (GenericTrustedApplicationFunctionHeader header : tafHeadersList) {
            length += header.getLength();
        }
        return length;
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

        // reset tafHeaderList and fill it again
        tafHeadersList = new ArrayList<GenericTrustedApplicationFunctionHeader>();
        int numberOfTAFs = fields.get(FIELD_NUMBER_OF_TAFS).getInt(0);
        byte[] tafHeaderData;
        GenericTrustedApplicationFunctionHeader hdr;
        for (int i = 0; i < numberOfTAFs; i++) {
            hdr = new GenericTrustedApplicationFunctionHeader();
            tafHeaderData = new byte[hdr.getLength()];
            System.arraycopy(data, pos, tafHeaderData, 0, tafHeaderData.length);
            hdr.setData(tafHeaderData);
            tafHeadersList.add(hdr);
            pos += hdr.getLength();
        }

        // set trusted code and data
        int trustedAppCodeAndDataSize = getRoSize() + getRwSize();
        trustedAppCodeAndData = new byte[trustedAppCodeAndDataSize];
        System.arraycopy(data, pos, trustedAppCodeAndData, 0, trustedAppCodeAndDataSize);
        pos += trustedAppCodeAndDataSize;

        // hash list
        payloadHashList = new ArrayList<byte[]>();
        int hashSize = HashType.getHashSize(getHashType());
        if (hashSize != 0) {
            if ((data.length - pos) % hashSize != 0) {
                throw new Exception("Bad hash list alignment. Hash size should be " + hashSize + " bytes");
            } else {
                while (pos < (data.length - 1)) {
                    byte[] hashData = new byte[hashSize];
                    System.arraycopy(data, pos, hashData, 0, hashSize);
                    payloadHashList.add(hashData);
                    pos += hashSize;
                }
            }
        }
    }

    /**
     * 
     * @param value
     *            hash algorithm used for hashing the payload
     */
    public void setHashType(HashType value) {
        fields.get(FIELD_HASH_TYPE).putInt(0, value.getHashType());
    }

    /**
     * 
     * @return hash algorithm used for hashing the payload
     */
    public int getHashType() {
        return fields.get(FIELD_HASH_TYPE).getInt(0);
    }

    /**
     * 
     * @return code+data
     */
    public byte[] getCodeAndData() {
        return trustedAppCodeAndData;
    }

    /**
     * @param data code and data
     */
    public void setCodeAndData(byte[] data) {
        trustedAppCodeAndData = data;
    }

    /**
     * 
     * @return the part of the trusted application that will be used as payload in the signed header
     */
    public byte[] getPayload() {
        int length = getLength() - trustedAppCodeAndData.length;
        byte[] staticData = getStaticData();
        byte[] data = new byte[length];
        System.arraycopy(staticData, 0, data, 0, staticData.length);
        int pos = staticData.length;

        // add TAFs
        for (GenericTrustedApplicationFunctionHeader header : tafHeadersList) {
            byte[] headerData = header.getData();
            System.arraycopy(headerData, 0, data, pos, headerData.length);
            pos += headerData.length;
        }

        // add hash list
        for (byte[] b : payloadHashList) {
            if (b != null) {
                System.arraycopy(b, 0, data, pos, b.length);
                pos += b.length;
            }
        }
        return data;
    }

    /**
     * 
     * @param hashList
     *            hashlist
     */
    public void setHashList(ArrayList<byte[]> hashList) {
        payloadHashList = hashList;
    }

    private byte[] getStaticData() {
        ByteBuffer result = ByteBuffer.allocate(SIZE_STATIC_FIELDS);

        for (ByteBuffer b : fields.values()) {
            b.rewind();
            result.put(b);
        }
        result.order(BYTE_ORDER);
        return result.array();
    }

    /**
     * @param pHeader U5500TrustedApplicationFunctionHeader
     */
    public void addHeader(GenericTrustedApplicationFunctionHeader pHeader) {
        tafHeadersList.add(pHeader);
    }
}
