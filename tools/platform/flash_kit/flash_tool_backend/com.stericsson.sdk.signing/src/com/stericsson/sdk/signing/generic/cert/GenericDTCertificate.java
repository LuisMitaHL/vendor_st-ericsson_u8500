package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;
import java.util.ArrayList;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericDTCertificateReference;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;

/**
 * @author xtomzap
 * 
 */
public class GenericDTCertificate extends GenericCertificate implements IDTCertificateConstants {

    private byte[] signature;

    private ArrayList<GenericISSWCustomerPartKey> keyReplacementList;

    private GenericDTCertificateReference reference;

    /** */
    public GenericDTCertificate() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_DT_CERT_SIZE, allocateField(SIZE_DT_CERT_SIZE));
        fields.put(FIELD_SIGNATURE_SIZE, allocateField(SIZE_SIGNATURE_SIZE));
        fields.put(FIELD_SIGNATURE_HASH_TYPE, allocateField(SIZE_SIGNATURE_HASH_TYPE));
        fields.put(FIELD_SERIAL_NUMBER, allocateField(SIZE_SERIAL_NUMBER));
        fields.put(FIELD_TYPE, allocateField(SIZE_TYPE));
        fields.put(FIELD_LEVEL, allocateField(SIZE_LEVEL));
        fields.put(FIELD_SECURE_DEBUG, allocateField(SIZE_SECURE_DEBUG));
        fields.put(FIELD_APE_DEBUG, allocateField(SIZE_APE_DEBUG));
        fields.put(FIELD_MODEM_DEBUG, allocateField(SIZE_MODEM_DEBUG));
        fields.put(FIELD_PRCMU_DEBUG, allocateField(SIZE_PRCMU_DEBUG));
        fields.put(FIELD_STM, allocateField(SIZE_STM));
        fields.put(FIELD_SPARE, allocateField(SIZE_SPARE));
        fields.put(FIELD_KEYS_TO_REPLACE, allocateField(SIZE_KEYS_TO_REPLACE));

        reference = new GenericDTCertificateReference();
        keyReplacementList = new ArrayList<GenericISSWCustomerPartKey>();
        signature = new byte[0];

        setMagic(GenericCertificate.MAGIC_DT_CERTIFICATE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setMagic(int value) {
        fields.get(FIELD_MAGIC).putInt(0, value);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getMagic() {
        return fields.get(FIELD_MAGIC).getInt(0);
    }

    /**
     * @param value
     *            D&T certificate size
     */
    public void setDTCertificateSize(int value) {
        fields.get(FIELD_DT_CERT_SIZE).putInt(0, value);
    }

    /**
     * @return D&T certificate size
     */
    public int getDTCertificateSize() {
        return fields.get(FIELD_DT_CERT_SIZE).getInt(0);
    }

    /**
     * @param value
     *            signature size
     */
    public void setSignatureSize(int value) {
        fields.get(FIELD_SIGNATURE_SIZE).putInt(0, value);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getSignatureSize() {
        return fields.get(FIELD_SIGNATURE_SIZE).getInt(0);
    }

    /**
     * @param value
     *            serial number
     */
    public void setSerialNumber(int value) {
        fields.get(FIELD_SERIAL_NUMBER).putInt(0, value);
    }

    /**
     * @return serial number
     */
    public int getSerialNumber() {
        return fields.get(FIELD_SERIAL_NUMBER).getInt(0);
    }

    /**
     * @param value
     *            type
     */
    public void setType(int value) {
        fields.get(FIELD_TYPE).putInt(0, value);
    }

    /**
     * @return type
     */
    public int getType() {
        return fields.get(FIELD_TYPE).getInt(0);
    }

    /**
     * @param value
     *            level
     */
    public void setLevel(byte value) {
        fields.get(FIELD_LEVEL).put(0, value);
    }

    /**
     * @return level
     */
    public byte getLevel() {
        return fields.get(FIELD_LEVEL).get(0);
    }

    /**
     * @param value
     *            secure debug
     */
    public void setSecureDebug(byte value) {
        fields.get(FIELD_SECURE_DEBUG).put(0, value);
    }

    /**
     * @return secure debug
     */
    public byte getSecureDebug() {
        return fields.get(FIELD_SECURE_DEBUG).get(0);
    }

    /**
     * @param value
     *            ape debug
     */
    public void setApeDebug(byte value) {
        fields.get(FIELD_APE_DEBUG).put(0, value);
    }

    /**
     * @return ape debug
     */
    public byte getApeDebug() {
        return fields.get(FIELD_APE_DEBUG).get(0);
    }

    /**
     * @param value
     *            modem debug
     */
    public void setModemDebug(byte value) {
        fields.get(FIELD_MODEM_DEBUG).put(0, value);
    }

    /**
     * @return modem debug
     */
    public byte getModemDebug() {
        return fields.get(FIELD_MODEM_DEBUG).get(0);
    }

    /**
     * @param value
     *            PRCMU debug
     */
    public void setPRCMUDebug(byte value) {
        fields.get(FIELD_PRCMU_DEBUG).put(0, value);
    }

    /**
     * @return PRCMU debug
     */
    public byte getPRCMUDebug() {
        return fields.get(FIELD_PRCMU_DEBUG).get(0);
    }

    /**
     * @param value
     *            STM
     * 
     *            0x00: trace for APE and modem disabled; 0x01: trace for APE disabled, modem
     *            enabled; 0x10: trace for APE enabled, modem disabled; 0x11: trace for APE enabled,
     *            modem enabled;
     */
    public void setSTM(byte value) {
        fields.get(FIELD_STM).put(0, value);
    }

    /**
     * @return STM
     */
    public byte getSTM() {
        return fields.get(FIELD_STM).get(0);
    }

    /**
     * @param data
     *            spare
     */
    public void setSpare(byte[] data) {
        fields.get(FIELD_SPARE).clear();
        fields.get(FIELD_SPARE).put(data);
    }

    /**
     * @return spare
     */
    public byte[] getSpare() {
        ByteBuffer spareBuffer = fields.get(FIELD_SPARE);
        spareBuffer.position(0);
        byte[] arrayArray = new byte[spareBuffer.capacity()];
        spareBuffer.get(arrayArray);
        return arrayArray;
    }

    /**
     * @return number of keys to replace
     */
    public int getNumberOfKeysToReplace() {
        return Integer.bitCount(fields.get(FIELD_KEYS_TO_REPLACE).getInt(0));
    }

    /**
     * @return key to replace
     */
    public int getSizeOfKeysToReplace() {
        return fields.get(FIELD_KEYS_TO_REPLACE).getInt(0);
    }

    /**
     * @return reference length
     */
    public int getReferenceLength() {
        return reference.getReferenceLength();
    }

    /**
     * @param data
     *            reference value
     */
    public void setReferenceValue(byte[] data) {
        reference.setReferenceLength(data.length);
        reference.setReferenceValue(data);

    }

    /**
     * @return reference value
     */
    public byte[] getReferenceValue() {
        return reference.getReferenceValue();
    }

    /**
     * 
     * @param key
     *            key
     * @param constraint
     *            constraint
     */
    public void addKeyToReplace(GenericISSWCustomerPartKey key, int constraint) {
        fields.get(FIELD_KEYS_TO_REPLACE).rewind();
        fields.get(FIELD_KEYS_TO_REPLACE).putInt(0,
            fields.get(FIELD_KEYS_TO_REPLACE).getInt() | (1 << (constraint - 1)));
        keyReplacementList.add(key);
    }

    /**
     * @param data
     *            signature
     */
    public void setSignature(byte[] data) {
        if (data == null) {
            data = new byte[0];
        }
        signature = data;
        setSignatureSize(data.length);
    }

    /**
     * @return signature
     */
    public byte[] getSignature() {
        return signature;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setHash(byte[] pData) {
        // not used
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getHash() {
        // not used
        return null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getSignableData() {
        int length = getLength();
        byte[] dataWithSignature = super.getData();

        // remove signature
        byte[] data = removeSignature(dataWithSignature);
        // add reference
        int actualPosition = addReference(length, data);
        // add key replacement list
        actualPosition = addKeyReplacementList(data, actualPosition);

        return data;
    }

    /**
     * Removes bytes representing signature from byte array containing signed dnt certificate, i.e.
     * dnt certificate plus signature.
     * 
     * @param dataWithSignature
     *            data with signature
     * @return data without signature
     */
    protected byte[] removeSignature(byte[] dataWithSignature) {
        byte[] data = new byte[dataWithSignature.length - signature.length];
        System.arraycopy(dataWithSignature, 0, data, 0, data.length);
        return data;
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

        // set reference
        pos = setReferenceData(data, pos);
        // reset keyList and fill it again
        pos = setKeyListData(data, pos);
        // set signature
        setSignatureData(data, pos);
    }

    /**
     * Set signature from input data containing signed dnt certificate.
     * 
     * @param data
     *            input data, dnt certificate plus signature
     * @param pos
     *            position in input data pointing to beginning of signature bytes
     */
    protected void setSignatureData(byte[] data, int pos) {
        signature = new byte[getSignatureSize()];
        System.arraycopy(data, pos, signature, 0, getSignatureSize());
    }

    /**
     * Set key list data field from input data containing complete signed dnt certificate.
     * 
     * @param data
     *            input data
     * @param pos
     *            position in input data
     * @return actual position
     * @throws Exception
     *             exception
     */
    protected int setKeyListData(byte[] data, int pos) throws Exception {
        keyReplacementList = new ArrayList<GenericISSWCustomerPartKey>();
        int numberOfKeys = getNumberOfKeysToReplace();
        for (int i = 0; i < numberOfKeys; i++) {
            GenericISSWCustomerPartKey key = new GenericISSWCustomerPartKey();
            key.setData(data, pos);
            keyReplacementList.add(key);
            pos += key.getLength();
        }
        return pos;
    }

    /**
     * Set reference data field from input data containing complete signed dnt certificate.
     * 
     * @param data
     *            input data
     * @param pos
     *            position in input data
     * @return actual position
     * @throws Exception
     *             exception
     */
    protected int setReferenceData(byte[] data, int pos) throws Exception {
        reference.setData(data, pos);
        pos += reference.getLength();
        return pos;
    }

    /**
     * 
     * @param type
     *            hash type
     */
    public void setSignatureHashType(HashType type) {
        fields.get(FIELD_SIGNATURE_HASH_TYPE).putInt(0, type.getHashType());
    }

    /**
     * 
     * @return hash type
     */
    public int getHashType() {
        return fields.get(FIELD_SIGNATURE_HASH_TYPE).getInt(0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();

        // add reference
        int actualPosition = addReference(length, data);
        // add key replacement list
        actualPosition = addKeyReplacementList(data, actualPosition);
        // add signature
        actualPosition = addSignature(data, actualPosition);

        return data;
    }

    /**
     * Adds reference field to output dnt certificate
     * 
     * @param data
     *            output data representing output dnt certificate
     * @param length
     *            total length of certificate
     * @return data updated with reference
     */
    protected int addReference(int length, byte[] data) {
        int actualPosition = length - reference.getLength() - getKeysLength() - signature.length;
        System.arraycopy(reference.getData(), 0, data, actualPosition, reference.getData().length);
        actualPosition += reference.getData().length;
        return actualPosition;
    }

    /**
     * Adds key replacement list to output data
     * 
     * @param data
     *            output data
     * @param actualPosition
     *            position in output data
     * @return data updated with key replacement list
     */
    protected int addKeyReplacementList(byte[] data, int actualPosition) {
        byte[] keyData = getKeysData();
        System.arraycopy(keyData, 0, data, actualPosition, keyData.length);
        actualPosition += keyData.length;
        return actualPosition;
    }

    /**
     * Adds signature to output data
     * 
     * @param data
     *            output data
     * @param actualPosition
     *            position in output data
     * @return data updated with signature
     */
    protected int addSignature(byte[] data, int actualPosition) {
        System.arraycopy(signature, 0, data, actualPosition, signature.length);
        actualPosition += signature.length;
        return actualPosition;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + reference.getLength() + getKeysLength() + signature.length;
    }

    private int getKeysLength() {
        int length = 0;
        for (GenericISSWCustomerPartKey key : keyReplacementList) {
            length += key.getLength();
        }
        return length;
    }

    private byte[] getKeysData() {
        ByteBuffer keyData = ByteBuffer.allocate(getKeysLength());
        for (GenericISSWCustomerPartKey key : keyReplacementList) {
            keyData.put(key.getData());
        }
        return keyData.array();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {
        short signatureSize =
            (Short) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE);
        setSignatureHashType((HashType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE));
        setSignatureSize(signatureSize);
        setDTCertificateSize(signatureSize + getLength());
    }
}
