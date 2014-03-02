package com.stericsson.sdk.signing.generic.cert.subparts;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.signing.AbstractByteSequence;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWCustomerPart extends AbstractByteSequence {

    /** key list */
    private List<GenericISSWCustomerPartKey> keyList;

    private GenericISSWModelId modelId;

    /**    **/
    private ISSWCertificateType type;

    /**
     * 
     * @param pType
     *            Type of ISSWCertificate
     */
    public GenericISSWCustomerPart(ISSWCertificateType pType) {
        super();
        type = pType;
        if (pType.equals(ISSWCertificateType.TYPE1)) {
            generateCustomerPart();
            setMagic(GenericCertificate.MAGIC_GENERIC_CUSTOMER_PART);
        } else {
            generateExtendedCustomerPart();
            setMagic(GenericCertificate.MAGIC_EXTENDED_CUSTOMER_PART);
        }
        keyList = new ArrayList<GenericISSWCustomerPartKey>();
        modelId = new GenericISSWModelId();
    }

    private void generateCustomerPart() {
        fields.put(CustomerPartFields.MAGIC.getPosition(), allocateField(CustomerPartFields.MAGIC.getSize()));
        fields.put(CustomerPartFields.KEY_LIST.getPosition(), allocateField(CustomerPartFields.KEY_LIST.getSize()));
        fields.put(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getPosition(),
            allocateField(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getSize()));
        fields.put(CustomerPartFields.MINOR_BUILD_VERSION.getPosition(),
            allocateField(CustomerPartFields.MINOR_BUILD_VERSION.getSize()));
        fields.put(CustomerPartFields.MAJOR_BUILD_VERSION.getPosition(),
            allocateField(CustomerPartFields.MAJOR_BUILD_VERSION.getSize()));
        fields.put(CustomerPartFields.UTC_TIME.getPosition(), allocateField(CustomerPartFields.UTC_TIME.getSize()));
        fields.put(CustomerPartFields.FLAGS.getPosition(), allocateField(CustomerPartFields.FLAGS.getSize()));
        fields.put(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getPosition(),
            allocateField(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getSize()));
    }

    private void generateExtendedCustomerPart() {
        fields.put(CustomerPartFields.MAGIC.getPosition(), allocateField(CustomerPartFields.MAGIC.getSize()));
        fields.put(CustomerPartFields.KEY_LIST.getPosition(), allocateField(CustomerPartFields.KEY_LIST.getSize()));
        fields.put(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getPosition(),
            allocateField(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getSize()));
        fields.put(CustomerPartFields.FLAGS.getPosition(), allocateField(CustomerPartFields.FLAGS.getSize()));
        fields.put(CustomerPartFields.EXTENDED_SIGNING_CONSTRAINTS.getPosition(),
            allocateField(CustomerPartFields.EXTENDED_SIGNING_CONSTRAINTS.getSize()));
        fields.put(CustomerPartFields.RESERVED_BYTES.getPosition(), allocateField(CustomerPartFields.RESERVED_BYTES
            .getSize()));
    }

    /**
     * @param value
     *            magic
     */
    public void setMagic(int value) {
        fields.get(CustomerPartFields.MAGIC.getPosition()).putInt(0, value);
    }

    /**
     * @return magic
     */
    public int getMagic() {
        return fields.get(CustomerPartFields.MAGIC.getPosition()).getInt(0);
    }

    /**
     * @param value
     *            key list size
     */
    private void setKeyListSize(int value) {
        fields.get(CustomerPartFields.KEY_LIST.getPosition()).putInt(0, value);
    }

    /**
     * @return key list size
     */
    public int getKeyListSize() {
        return fields.get(CustomerPartFields.KEY_LIST.getPosition()).getInt(0);
    }

    /**
     * @param value
     *            number of keys in key list
     */
    private void setNumberOfKeysInList(int value) {
        fields.get(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getPosition()).putInt(0, value);
    }

    /**
     * @return number of keys in key list
     */
    public int getNumberOfKeysInList() {
        return fields.get(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getPosition()).getInt(0);
    }

    /**
     * Sets minor build version (valid only for ISSW certificate type 1, otherwise ignored).
     * 
     * @param value
     *            minor version
     */
    public void setMinorBuildVersion(short value) {
        if (type == ISSWCertificateType.TYPE1) {
            fields.get(CustomerPartFields.MINOR_BUILD_VERSION.getPosition()).putShort(0, value);
        }
    }

    /**
     * Returns minor build version (valid only for ISSW certificate type 1, otherwise returns 0).
     * 
     * @return minor version
     */
    public short getMinorBuildVersion() {
        if (type == ISSWCertificateType.TYPE1) {
            return fields.get(CustomerPartFields.MINOR_BUILD_VERSION.getPosition()).getShort(0);
        } else {
            return 0;
        }
    }

    /**
     * Sets major build version (valid only for ISSW certificate type 1, otherwise ignored).
     * 
     * @param value
     *            major version
     */
    public void setMajorBuildVersion(short value) {
        if (type == ISSWCertificateType.TYPE1) {
            fields.get(CustomerPartFields.MAJOR_BUILD_VERSION.getPosition()).putShort(0, value);
        }
    }

    /**
     * Returns major build version (valid only for ISSW certificate type 1, otherwise returns 0).
     * 
     * @return major version
     */
    public short getMajorBuildVersion() {
        if (type == ISSWCertificateType.TYPE1) {
            return fields.get(CustomerPartFields.MAJOR_BUILD_VERSION.getPosition()).getShort(0);
        } else {
            return 0;
        }
    }

    /**
     * Sets UTC time (valid only for ISSW certificate type 1, otherwise ignored).
     * 
     * @param value
     *            utc time
     */
    public void setUTCTime(int value) {
        if (type == ISSWCertificateType.TYPE1) {
            fields.get(CustomerPartFields.UTC_TIME.getPosition()).putInt(0, value);
        }
    }

    /**
     * Returns UTC time (valid only for ISSW certificate type 1, otherwise returns 0).
     * 
     * @return utc time
     */
    public int getUTCTime() {
        if (type == ISSWCertificateType.TYPE1) {
            return fields.get(CustomerPartFields.UTC_TIME.getPosition()).getInt(0);
        } else {
            return 0;
        }
    }

    /**
     * @param value
     *            flags
     */
    public void setFlags(int value) {
        fields.get(CustomerPartFields.FLAGS.getPosition()).putInt(0, value);
    }

    /**
     * @return flags
     */
    public int getFlags() {
        return fields.get(CustomerPartFields.FLAGS.getPosition()).getInt(0);
    }

    /**
     * Sets reserved bytes (valid only for ISSW certificate type 2, otherwise ignored).
     * 
     * @param pReserved
     *            reserved fields
     */
    public void setReserved(byte[] pReserved) {
        if (type == ISSWCertificateType.TYPE2) {
            fields.get(CustomerPartFields.RESERVED_BYTES.getPosition()).put(pReserved);
        }
    }

    /**
     * @param sConstraints
     *            SigningConstraints
     */
    public void setSigningConstraints(byte[] sConstraints) {
        fields.get(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getPosition()).clear();
        fields.get(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getPosition()).put(sConstraints);
    }

    /**
     * @return SigningConstraints
     */
    public byte[] getSigningConstraints() {
        return fields.get(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getPosition()).array();
    }

    /**
     * @param idList
     *            List of model ID.
     */
    public void setModelId(List<Short> idList) {
        modelId.setModelId(idList);
    }

    /**
     * @return model ID
     */
    public List<Short> getModelIds() {
        return modelId.getIds();
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

        modelId = new GenericISSWModelId();
        modelId.setData(data, pos);
        pos += modelId.getLength();

        // reset keyList and fill it again
        keyList = new ArrayList<GenericISSWCustomerPartKey>();
        int numberOfKeys = fields.get(CustomerPartFields.NUMBER_OF_KEYS_IN_LIST.getPosition()).getInt(0);
        for (int i = 0; i < numberOfKeys; i++) {
            GenericISSWCustomerPartKey key = new GenericISSWCustomerPartKey();
            key.setData(data, pos);
            keyList.add(key);
            pos += key.getLength();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();

        // add reference
        int actualPosition = length - getKeysLength() - modelId.getLength();

        // add model ids
        byte[] idData = modelId.getData();
        System.arraycopy(idData, 0, data, actualPosition, idData.length);
        actualPosition += modelId.getLength();

        // add key list
        byte[] keyData = getKeysData();
        System.arraycopy(keyData, 0, data, actualPosition, keyData.length);
        actualPosition += keyData.length;

        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + getKeysLength() + modelId.getLength();
    }

    private int getKeysLength() {
        int length = 0;
        for (GenericISSWCustomerPartKey key : keyList) {
            length += key.getLength();
        }
        return length;
    }

    private byte[] getKeysData() {
        ByteBuffer keyData = ByteBuffer.allocate(getKeysLength());
        for (GenericISSWCustomerPartKey key : keyList) {
            keyData.put(key.getData());
        }
        return keyData.array();
    }

    /**
     * @param swType
     *            swType
     * @return key
     */
    public GenericISSWCustomerPartKey getKey(GenericSoftwareType swType) {
        ByteBuffer signConstrains = fields.get(CustomerPartFields.GENERIC_SIGNING_CONSTRAINTS.getPosition());
        if (signConstrains.capacity() <= swType.getPosition()) {
            return null;
        }
        int listPosition = signConstrains.get(swType.getPosition());
        if (keyList.size() < listPosition || listPosition == 0) {
            return null;
        }
        return keyList.get(listPosition - 1);
    }

    /** {@inheritDoc} */
    @Override
    public void read(ReadableByteChannel pInput) throws IOException {
        // not used, use setData instead
    }

    /**
     * @param key
     *            add key to key list
     */
    public void addKey(GenericISSWCustomerPartKey key) {
        keyList.add(key);
        setKeyListSize(getKeyListSize() + key.getLength());
        setNumberOfKeysInList(getNumberOfKeysInList() + 1);
    }

    /**
     * 
     * @return type of ISSW certificate
     */
    public ISSWCertificateType getType() {
        return type;
    }

}
