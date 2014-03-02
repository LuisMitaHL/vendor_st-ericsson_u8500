package com.stericsson.sdk.signing.generic.cert;

import java.nio.ByteBuffer;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;

/**
 * @author xtomzap
 * 
 */
public class GenericAuthenticationCertificate extends GenericCertificate {

    /* fields sizes */

    private static final int SIZE_MAGIC = 4;

    private static final int SIZE_AUTH_CERT_SIZE = 2;

    private static final int SIZE_SIGNATURE_SIZE = 2;

    private static final int SIZE_SIGN_HASH_TYPE = 4;

    private static final int SIZE_SIGNATURE_TYPE = 4;

    private static final int SIZE_SERIAL_NUMBER = 4;

    private static final int SIZE_NOT_BEFORE = 4;

    private static final int SIZE_NOT_AFTER = 4;

    private static final int SIZE_AUTHENTICATION_LEVEL = 4;

    private static final int SIZE_SPARE = 4;

    /* fields ids */

    private static final int FIELD_MAGIC = 1;

    private static final int FIELD_AUTH_CERT_SIZE = 2;

    private static final int FIELD_SIGNATURE_SIZE = 3;

    private static final int FIELD_SIGN_HASH_TYPE = 4;

    private static final int FIELD_SIGNATURE_TYPE = 5;

    private static final int FIELD_SERIAL_NUMBER = 6;

    private static final int FIELD_NOT_BEFORE = 7;

    private static final int FIELD_NOT_AFTER = 8;

    private static final int FIELD_AUTHENTICATION_LEVEL = 9;

    private static final int FIELD_SPARE = 10;

    /* fields */

    private GenericISSWCustomerPartKey authenticationKey;

    private byte[] signature;

    /** */
    public GenericAuthenticationCertificate() {
        super();
        fields.put(FIELD_MAGIC, allocateField(SIZE_MAGIC));
        fields.put(FIELD_AUTH_CERT_SIZE, allocateField(SIZE_AUTH_CERT_SIZE));
        fields.put(FIELD_SIGNATURE_SIZE, allocateField(SIZE_SIGNATURE_SIZE));
        fields.put(FIELD_SIGN_HASH_TYPE, allocateField(SIZE_SIGN_HASH_TYPE));
        fields.put(FIELD_SIGNATURE_TYPE, allocateField(SIZE_SIGNATURE_TYPE));
        fields.put(FIELD_SERIAL_NUMBER, allocateField(SIZE_SERIAL_NUMBER));
        fields.put(FIELD_NOT_BEFORE, allocateField(SIZE_NOT_BEFORE));
        fields.put(FIELD_NOT_AFTER, allocateField(SIZE_NOT_AFTER));
        fields.put(FIELD_AUTHENTICATION_LEVEL, allocateField(SIZE_AUTHENTICATION_LEVEL));
        fields.put(FIELD_SPARE, allocateField(SIZE_SPARE));

        authenticationKey = new GenericISSWCustomerPartKey();
        signature = new byte[0];

        setMagic(GenericCertificate.MAGIC_AUTHENTICATION_CERTIFICATE);
    }

    /**
     * @param value
     *            exponent
     */
    public void setExponent(int value) {
        authenticationKey.setExponent(value);
    }

    /**
     * @return exponent
     */
    public int getExponent() {
        return authenticationKey.getExponent();
    }

    /**
     * @param value
     *            dummy
     */
    public void setDummy(short value) {
        authenticationKey.setDummy(value);
    }

    /**
     * @return dummy
     */
    public short getDummy() {
        return authenticationKey.getDummy();
    }

    /**
     * @param value
     *            modulus size
     */
    public void setModulusSize(short value) {
        authenticationKey.setModulusSize(value);
    }

    /**
     * @return modulus size
     */
    public short getModulusSize() {
        return authenticationKey.getModulusSize();
    }

    /**
     * @param data
     *            modulus
     */
    public void setModulus(byte[] data) {
        authenticationKey.setModulus(data);
    }

    /**
     * @return modulus
     */
    public byte[] getModulus() {
        return authenticationKey.getModulus();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setMagic(int pMagic) {
        fields.get(FIELD_MAGIC).putInt(0, pMagic);
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
     *            size of authentication certificate (including signature)
     */
    public void setAuthCertSize(short value) {
        fields.get(FIELD_AUTH_CERT_SIZE).putShort(0, value);
    }

    /**
     * @return size of authentication certificate (including signature)
     */
    public short getAuthCertSize() {
        return fields.get(FIELD_AUTH_CERT_SIZE).getShort(0);
    }

    /**
     * @param value
     *            size of signature
     */
    public void setSignatureSize(short value) {
        fields.get(FIELD_SIGNATURE_SIZE).putShort(0, value);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getSignatureSize() {
        return fields.get(FIELD_SIGNATURE_SIZE).getShort(0);
    }

    /**
     * @param value
     *            signature hash type
     */
    public void setSignatureHashType(HashType value) {
        fields.get(FIELD_SIGN_HASH_TYPE).putInt(0, value.getHashType());
    }

    /**
     * @return signature hash type
     */
    public int getSignatureHashType() {
        return fields.get(FIELD_SIGN_HASH_TYPE).getInt(0);
    }

    /**
     * @param value
     *            signature type
     */
    public void setSignatureType(SignatureType value) {
        fields.get(FIELD_SIGNATURE_TYPE).putInt(0, value.getSignatureType());
    }

    /**
     * @return signature type
     */
    public int getSignatureType() {
        return fields.get(FIELD_SIGNATURE_TYPE).getInt(0);
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
     *            not before
     */
    public void setNotBefore(long value) {
        fields.get(FIELD_NOT_BEFORE).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * @return not before
     */
    public int getNotBefore() {
        return fields.get(FIELD_NOT_BEFORE).getInt(0);
    }

    /**
     * @param value
     *            not after
     */
    public void setNotAfter(long value) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_NOT_AFTER).putInt(0, (int) (value & 0xFFFFFFFF));
    }

    /**
     * @return not after
     */
    public int getNotAfter() {
        return fields.get(FIELD_NOT_AFTER).getInt(0);
    }

    /**
     * @param value
     *            authentication level
     */
    public void setAuthenticationLevel(int value) {
        fields.get(FIELD_AUTHENTICATION_LEVEL).putInt(0, value);
    }

    /**
     * @return authentication level
     */
    public int getAuthenticationLevel() {
        return fields.get(FIELD_AUTHENTICATION_LEVEL).getInt(0);
    }

    /**
     * @param value
     *            key size
     */
    public void setKeySize(int value) {
        authenticationKey.setKeySize(value);
    }

    /**
     * @return key size
     */
    public int getKeySize() {
        return authenticationKey.getKeySize();
    }

    /**
     * @param value
     *            key type
     */
    public void setKeyTypeWithRevocationHashTypes(SignatureTypeWithRevocationHashTypes value) {

        authenticationKey.setKeyTypeWithRevocationHashTypes(value);
    }

    /**
     * @return key type
     */
    public int getKeyType() {
        return authenticationKey.getKeyType();
    }

    /**
     * @param value
     *            spare
     */
    public void setSpare(int value) {
        fields.get(FIELD_SPARE).putInt(0, value);
    }

    /**
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
        // not used
        return null;
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
    public byte[] getSignableData() {
        int length = getLength();
        byte[] dataWithSignature = super.getData();

        // remove signature
        byte[] data = new byte[dataWithSignature.length - signature.length];
        System.arraycopy(dataWithSignature, 0, data, 0, data.length);

        // add key
        int actualPosition = length - authenticationKey.getLength() - signature.length;
        byte[] keyData = authenticationKey.getData();
        System.arraycopy(keyData, 0, data, actualPosition, keyData.length);
        actualPosition += keyData.length;

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

        // set authentication key
        authenticationKey.setData(data, pos);
        pos += authenticationKey.getLength();

        // set signature
        signature = new byte[getSignatureSize()];
        System.arraycopy(data, pos, signature, 0, getSignatureSize());
        pos += signature.length;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getData() {
        int length = getLength();
        byte[] data = super.getData();

        // add key
        int actualPosition = length - authenticationKey.getLength() - signature.length;
        byte[] keyData = authenticationKey.getData();
        System.arraycopy(keyData, 0, data, actualPosition, keyData.length);
        actualPosition += keyData.length;

        // add signature
        System.arraycopy(signature, 0, data, actualPosition, signature.length);
        actualPosition += signature.length;

        return data;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getLength() {
        return super.getLength() + authenticationKey.getLength() + signature.length;
    }

    /**
     * 
     * @return signature
     */
    public byte[] getSignature() {
        return signature;
    }

    /**
     * 
     * @param sign
     *            signature
     */
    public void setSignature(byte[] sign) {
        signature = sign == null ? new byte[0] : sign;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public SignatureType getTypeOfKey(ISignerSettings signerSettings) throws SignerException {

        SignatureType typeToReturn = SignatureType.getByValue(getKeyType());

        if (typeToReturn == null) {
            throw new SignerException("Key type (" + getKeyType()
                + ") specified in Authentication Certificate is not supported");
        }

        return typeToReturn;
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {
        short signatureSize =
            (Short) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE);
        setAuthCertSize((short) (getLength() + signatureSize));
        setSignatureSize(signatureSize);
        setSignatureHashType((HashType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE));
        setSignatureType((SignatureType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE));
    }
}
