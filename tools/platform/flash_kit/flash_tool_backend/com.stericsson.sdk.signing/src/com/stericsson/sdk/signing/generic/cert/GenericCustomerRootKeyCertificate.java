package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * @author xtomzap
 * 
 *         This class represents signed customer root key certificate
 */
public class GenericCustomerRootKeyCertificate extends GenericCertificate {

    private static final int SIZE_MAGIC = 4;

    private static final int SIZE_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE = 4;

    private static final int SIZE_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE = 4;

    private static final int SIZE_RESERVED = 20;

    private static final int SIZE_ROOT_KEY_CERTIFICATE_DATA = 832;

    private static final int SIZE_CUSTOMER_ROOT_KEY_HASH = 64;

    private static final int SIZE_SIGNATURE = 256;

    // /////////

    private static final int FIELD_MAGIC = 1;

    private static final int FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE = 2;

    private static final int FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE = 3;

    private static final int FIELD_RESERVED = 4;

    private static final int FIELD_ROOT_KEY_CERTIFICATE_DATA = 5;

    private static final int FIELD_CUSTOMER_ROOT_KEY_HASH = 6;

    private static final int FIELD_SIGNATURE = 7;

    /**
     * 
     */
    public GenericCustomerRootKeyCertificate() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE, allocateField(SIZE_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE));
        fields.put(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE,
            allocateField(SIZE_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE));
        fields.put(FIELD_RESERVED, allocateField(SIZE_RESERVED));
        fields.put(FIELD_ROOT_KEY_CERTIFICATE_DATA, allocateField(SIZE_ROOT_KEY_CERTIFICATE_DATA));
        fields.put(FIELD_CUSTOMER_ROOT_KEY_HASH, allocateField(SIZE_CUSTOMER_ROOT_KEY_HASH));
        fields.put(FIELD_SIGNATURE, allocateField(SIZE_SIGNATURE));

        setMagic(MAGIC_CUSTOMER_ROOT_KEY_CERTIFICATE_HEADER);
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
    public int getMagic() {
        return fields.get(FIELD_MAGIC).getInt(0);
    }

    /**
     * @param value
     *            customer root key signature type
     */
    public void setCustomerRootKeySignatureType(SignatureType value) {
        fields.get(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE).putInt(0, value.getSignatureType());
    }

    /**
     * @return customer root signature type
     */
    public int getCustomerRootKeySignatureType() {
        return fields.get(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_TYPE).getInt(0);
    }

    /**
     * @param type
     *            customer root key signature type
     */
    public void setCustomerRootKeySignatureHashType(HashType type) {
        fields.get(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE).putInt(0, type.getHashType());
    }

    /**
     * @return customer root key signature hash type
     */
    public int getCustomerRootKeySignatureHashType() {
        return fields.get(FIELD_CUSTOMER_ROOT_KEY_SIGNATURE_HASH_TYPE).getInt(0);
    }

    /**
     * @param data
     *            root key certificate data
     */
    public void setRootKeyCertificateData(byte[] data) {
        fields.get(FIELD_ROOT_KEY_CERTIFICATE_DATA).position(0);
        fields.get(FIELD_ROOT_KEY_CERTIFICATE_DATA).put(data);
    }

    /**
     * @return root key certificate data
     */
    public byte[] getRootKeyCertificateData() {
        ByteBuffer rootKeyBuffer = fields.get(FIELD_ROOT_KEY_CERTIFICATE_DATA);
        rootKeyBuffer.position(0);
        byte[] rootKeyArray = new byte[rootKeyBuffer.capacity()];
        rootKeyBuffer.get(rootKeyArray);
        return rootKeyArray;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setHash(byte[] data) {
        fields.get(FIELD_CUSTOMER_ROOT_KEY_HASH).position(0);
        fields.get(FIELD_CUSTOMER_ROOT_KEY_HASH).put(data);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getHash() {
        ByteBuffer customerRootKeyHashBuffer = fields.get(FIELD_CUSTOMER_ROOT_KEY_HASH);
        customerRootKeyHashBuffer.position(0);
        byte[] customerRootKeyHashArray = new byte[customerRootKeyHashBuffer.capacity()];
        customerRootKeyHashBuffer.get(customerRootKeyHashArray);
        return customerRootKeyHashArray;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getSignableData() {
        if (getLength() == 0) {
            return new byte[0];
        }

        ByteBuffer result = ByteBuffer.allocate(getLength() - SIZE_SIGNATURE);

        for (Integer key : fields.keySet()) {
            if (key.intValue() != FIELD_SIGNATURE) {
                ByteBuffer b = fields.get(key);
                b.rewind();
                result.put(b);
            }
        }

        result.order(BYTE_ORDER);
        return result.array();
    }

    /**
     * Sets signature data.
     * 
     * @param data
     *            Signature data.
     */
    public void setSignature(byte[] data) {
        fields.get(FIELD_SIGNATURE).position(0);
        fields.get(FIELD_SIGNATURE).put(data);
    }

    /**
     * Returns signature data.
     * 
     * @return Signature data.
     */
    public byte[] getSignature() {
        ByteBuffer signatureBuffer = fields.get(FIELD_SIGNATURE);
        signatureBuffer.position(0);
        byte[] signatureArray = new byte[signatureBuffer.capacity()];
        signatureBuffer.get(signatureArray);
        return signatureArray;
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
    public void setData(byte[] data) throws Exception {
        if (data == null) {
            throw new Exception("Certificate data is null");
        }

        byte[] magicData = new byte[4];
        System.arraycopy(data, 0, magicData, 0, magicData.length);
        ByteBuffer magicBuffer = ByteBuffer.wrap(magicData);
        magicBuffer.order(ByteOrder.LITTLE_ENDIAN);
        int magic = magicBuffer.getInt(0);

        if (magic != GenericCertificate.MAGIC_CUSTOMER_ROOT_KEY_CERTIFICATE_HEADER) {
            throw new Exception("Customer root key certificate magic doesn't match.");
        }
        super.setData(data);
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public SignatureType getTypeOfKey(ISignerSettings signerSettings) throws SignerException {
        SignatureType typeToReturn = SignatureType.getByValue(getCustomerRootKeySignatureType());

        if (typeToReturn == null) {
            throw new SignerException("Signature type (" + getCustomerRootKeySignatureType()
                + ") specified in Customer Root Key Certificate is not supported");
        }

        return typeToReturn;
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {
        setCustomerRootKeySignatureType((SignatureType) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE));
    }
}
