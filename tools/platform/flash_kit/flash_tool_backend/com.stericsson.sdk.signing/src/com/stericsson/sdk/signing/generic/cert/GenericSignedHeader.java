package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;

/**
 * 
 * @author xolabju
 * 
 */
public class GenericSignedHeader extends GenericCertificate {

    // field sizes
    private static final int SIZE_MAGIC = 4;

    private static final int SIZE_SIZE_SIGNED_HEADER = 2;

    private static final int SIZE_SIZE_SIGNATURE = 2;

    private static final int SIZE_SIGN_HASH_TYPE = 4;

    private static final int SIZE_SIGNATURE_TYPE = 4;

    private static final int SIZE_HASH_TYPE = 4;

    private static final int SIZE_PAYLOAD_TYPE = 4;

    private static final int SIZE_FLAGS = 4;

    private static final int SIZE_SIZE_PAYLOAD = 4;

    private static final int SIZE_SW_VERSION_NUMBER = 4;

    private static final int SIZE_LOAD_ADDRESS = 4;

    private static final int SIZE_STARTUP_ADDRESS = 4;

    private static final int SIZE_SPARE = 4;

    // field ids

    private static final int FIELD_MAGIC = 1;

    private static final int FIELD_SIZE_SIGNED_HEADER = 2;

    private static final int FIELD_SIZE_SIGNATURE = 3;

    private static final int FIELD_SIGN_HASH_TYPE = 4;

    private static final int FIELD_SIGNATURE_TYPE = 5;

    private static final int FIELD_HASH_TYPE = 6;

    private static final int FIELD_PAYLOAD_TYPE = 7;

    private static final int FIELD_FLAGS = 8;

    private static final int FIELD_SIZE_PAYLOAD = 9;

    private static final int FIELD_SW_VERSION_NUMBER = 10;

    private static final int FIELD_LOAD_ADDRESS = 11;

    private static final int FIELD_STARTUP_ADDRESS = 12;

    private static final int FIELD_SPARE = 13;

    // hash array
    private byte[] hashValue = new byte[0];

    // masks
    private static final int MASK_DMA_ACCELERATION = 0x00000001;

    private static final int MASK_PKA_ACCELERATION = 0x00000002;

    /**
     * Constructor
     */
    public GenericSignedHeader() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_SIZE_SIGNED_HEADER, allocateField(SIZE_SIZE_SIGNED_HEADER));
        fields.put(FIELD_SIZE_SIGNATURE, allocateField(SIZE_SIZE_SIGNATURE));
        fields.put(FIELD_SIGN_HASH_TYPE, allocateField(SIZE_SIGN_HASH_TYPE));
        fields.put(FIELD_SIGNATURE_TYPE, allocateField(SIZE_SIGNATURE_TYPE));
        fields.put(FIELD_HASH_TYPE, allocateField(SIZE_HASH_TYPE));
        fields.put(FIELD_PAYLOAD_TYPE, allocateField(SIZE_PAYLOAD_TYPE));
        fields.put(FIELD_FLAGS, allocateField(SIZE_FLAGS));
        fields.put(FIELD_SIZE_PAYLOAD, allocateField(SIZE_SIZE_PAYLOAD));
        fields.put(FIELD_SW_VERSION_NUMBER, allocateField(SIZE_SW_VERSION_NUMBER));
        fields.put(FIELD_LOAD_ADDRESS, allocateField(SIZE_LOAD_ADDRESS));
        fields.put(FIELD_STARTUP_ADDRESS, allocateField(SIZE_STARTUP_ADDRESS));
        fields.put(FIELD_SPARE, allocateField(SIZE_SPARE));

        // set magic
        setMagic(GenericCertificate.MAGIC_SIGNED_HEADER);
    }

    /**
     * 
     * @param value
     *            size of signed header (incl. signature)
     */
    public void setSizeOfSignedHeader(short value) {
        fields.get(FIELD_SIZE_SIGNED_HEADER).putShort(0, value);
    }

    /**
     * 
     * @return size of signed header (incl. signature)
     */
    public short getSizeOfSignedHeader() {
        return fields.get(FIELD_SIZE_SIGNED_HEADER).getShort(0);
    }

    /**
     * 
     * @param value
     *            size of signature
     */
    public void setSizeOfSignature(short value) {
        fields.get(FIELD_SIZE_SIGNATURE).putShort(0, value);
    }

    /**
     * 
     * @return size of signature
     */
    public short getSizeOfSignature() {
        return fields.get(FIELD_SIZE_SIGNATURE).getShort(0);
    }

    /**
     * 
     * @param value
     *            hash algorithm used for hashing the header
     */
    public void setSignHashType(HashType value) {
        fields.get(FIELD_SIGN_HASH_TYPE).putInt(0, value.getHashType());
    }

    /**
     * 
     * @return hash algorithm used for hashing the header
     */
    public int getSignHashType() {
        return fields.get(FIELD_SIGN_HASH_TYPE).getInt(0);
    }

    /**
     * 
     * @param value
     *            signing algorithm
     */
    public void setSignatureType(SignatureType value) {
        fields.get(FIELD_SIGNATURE_TYPE).putInt(0, value.getSignatureType());
    }

    /**
     * 
     * @return signing algorithm
     */
    public int getSignatureType() {
        return fields.get(FIELD_SIGNATURE_TYPE).getInt(0);
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
     * @param value
     *            payload type
     */
    public void setPayloadType(GenericSoftwareType value) {
        fields.get(FIELD_PAYLOAD_TYPE).putInt(0, value.getSoftwareId());
    }

    /**
     * 
     * @return payload type
     */
    public int getPayloadType() {
        return fields.get(FIELD_PAYLOAD_TYPE).getInt(0);
    }

    /**
     * 
     * @param value
     *            flags
     */
    public void setFlags(int value) {
        fields.get(FIELD_FLAGS).putInt(0, value);
    }

    /**
     * 
     * @return flags
     */
    public int getFlags() {
        return fields.get(FIELD_FLAGS).getInt(0);
    }

    /**
     * @param value
     *            Size of payload
     */
    public void setSizeOfPayload(int value) {
        fields.get(FIELD_SIZE_PAYLOAD).putInt(0, value);
    }

    /**
     * @return Size of payload
     */
    public int getSizeOfPayload() {
        return fields.get(FIELD_SIZE_PAYLOAD).getInt(0);
    }

    /**
     * 
     * @param value
     *            software version number
     */
    public void setSwVersion(long value) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_SW_VERSION_NUMBER).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * 
     * @return software version number
     */
    public int getSwVersion() {
        return fields.get(FIELD_SW_VERSION_NUMBER).getInt(0);
    }

    /**
     * 
     * @param value
     *            load address
     */
    public void setLoadAddress(long value) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_LOAD_ADDRESS).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * 
     * @return load address
     */
    public int getLoadAddress() {
        return fields.get(FIELD_LOAD_ADDRESS).getInt(0);
    }

    /**
     * 
     * @param value
     *            startup address. must be in the range [load_address, load_address+size_of_payload]
     */
    public void setStartupAddress(long value) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_STARTUP_ADDRESS).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * 
     * @return startup address
     */
    public int getStartupAddress() {
        return fields.get(FIELD_STARTUP_ADDRESS).getInt(0);
    }

    /**
     * 
     * @param value
     *            spare
     */
    public void setSpare(int value) {
        fields.get(FIELD_SPARE).putInt(0, value);
    }

    /**
     * 
     * @return spare
     */
    public int getSpare() {
        return fields.get(FIELD_SPARE).getInt(0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getHash() {
        return hashValue;
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
        return getData();
    }

    /**
     * 
     * @param value
     *            size of signature
     */
    public void setSignatureSize(short value) {
        fields.get(FIELD_SIZE_SIGNATURE).putShort(0, value);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getSignatureSize() {
        return fields.get(FIELD_SIZE_SIGNATURE).getShort(0);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setHash(byte[] data) {
        hashValue = data;

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
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();
        System.arraycopy(hashValue, 0, data, length - hashValue.length, hashValue.length);
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
        int sizeOfHash = data.length - pos - getSignatureSize();
        hashValue = new byte[sizeOfHash];
        System.arraycopy(data, pos, hashValue, 0, sizeOfHash);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + hashValue.length;
    }

    /**
     * 
     * @param dma
     *            true/false
     */
    public void setDMAAcceleration(boolean dma) {
        int flags = getFlags();
        if (dma) {
            setFlags(flags | MASK_DMA_ACCELERATION);
        } else {
            setFlags(flags & ~MASK_DMA_ACCELERATION);
        }
    }

    /**
     * 
     * @return true/false
     */
    public boolean getDMAAcceleration() {
        return (getFlags() & MASK_DMA_ACCELERATION) == MASK_DMA_ACCELERATION;
    }

    /**
     * 
     * @param pka
     *            true/false
     */
    public void setPKAAcceleration(boolean pka) {
        int flags = getFlags();
        if (pka) {
            setFlags(flags | MASK_PKA_ACCELERATION);
        } else {
            setFlags(flags & ~MASK_PKA_ACCELERATION);
        }
    }

    /**
     * 
     * @return true/false
     */
    public boolean getPKAAcceleration() {
        return (getFlags() & MASK_PKA_ACCELERATION) == MASK_PKA_ACCELERATION;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public SignatureType getTypeOfKey(ISignerSettings signerSettings) throws SignerException {
        GenericSignPackage pkg = (GenericSignPackage) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);
        GenericSoftwareType swType =
            (GenericSoftwareType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_TYPE);
        GenericCertificate pkgCertificate = pkg.getCertificate();
        if (pkgCertificate instanceof GenericISSWCertificate) {
            GenericISSWCertificate isswCertificate = (GenericISSWCertificate) pkgCertificate;
            GenericISSWCustomerPartKey key = isswCertificate.getKey(swType);
            if (key == null) {
                throw new SignerException("Can't get key from ISSW Certificate");
            }
            int keyType = key.getKeyType();
            SignatureType typeToReturn = SignatureType.getByValue(keyType);

            if (typeToReturn == null) {
                throw new SignerException("Key type (" + keyType + ") specified in ISSW Certificate is not supported");
            }

            return typeToReturn;
        } else {
            return pkgCertificate.getTypeOfKey(signerSettings);
        }
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {
        setSignatureSize((Short) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE));
        setSignHashType((HashType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE));
        setSignatureType((SignatureType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE));
        setHashType((HashType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_PAYLOAD_HASH_TYPE));
        setPayloadType((GenericSoftwareType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_TYPE));
        setDMAAcceleration(signerSettings.getSignerSetting(IGenericSignerSettings.KEY_DMA_FLAG) != null
            && (Boolean) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_DMA_FLAG));
        setPKAAcceleration(signerSettings.getSignerSetting(IGenericSignerSettings.KEY_PKA_FLAG) != null
            && (Boolean) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_PKA_FLAG));
        setSizeOfPayload((int) payload.getLength());
        setLoadAddress(signerSettings.getSignerSetting(IGenericSignerSettings.KEY_LOAD_ADDRESS) == null ? 0
            : (Long) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_LOAD_ADDRESS));
        setStartupAddress(signerSettings.getSignerSetting(IGenericSignerSettings.KEY_START_ADDRESS) == null ? 0
            : (Long) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_START_ADDRESS));
        setSizeOfSignedHeader((short) (getLength() + getSignatureSize()));
        setSwVersion(signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_VERSION) == null ? 0
            : (Long) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_VERSION));
    }
}
