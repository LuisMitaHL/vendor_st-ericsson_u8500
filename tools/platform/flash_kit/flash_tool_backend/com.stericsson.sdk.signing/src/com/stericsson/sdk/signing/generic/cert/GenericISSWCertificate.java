package com.stericsson.sdk.signing.generic.cert;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.generic.GenericSignedPayload;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.IGenericSignablePayload;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPart;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWSecurityRomHeader;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWSignature;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWUncheckedBlob;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;

/**
 * @author xtomzap
 * 
 */
public class GenericISSWCertificate extends GenericCertificate {

    private GenericISSWSecurityRomHeader secRomHeader;

    private GenericKey rootKey;

    private GenericISSWCustomerPart customerPart;

    private GenericISSWUncheckedBlob uncheckedBlob;

    private GenericISSWSignature signature;

    /**
     * 
     */
    public GenericISSWCertificate() {
        super();
        secRomHeader = new GenericISSWSecurityRomHeader();
        rootKey = new GenericKey();
        customerPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE1);
        uncheckedBlob = new GenericISSWUncheckedBlob(secRomHeader.getUncheckedBlobSize());
        signature = new GenericISSWSignature(secRomHeader.getSignatureSize());
        setMagic(GenericCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED);
    }

    /** {@inheritDoc} */
    @Override
    public void read(ReadableByteChannel input) throws IOException {

    }

    /** {@inheritDoc} */
    @Override
    public void setData(byte[] data) throws Exception {
        if (data == null) {
            throw new Exception("Data is null");
        } else if (data.length < GenericISSWSecurityRomHeader.SIZE_GENERIC) {
            throw new Exception("Length of data is " + data.length + ", minumum length of byte sequence is "
                + GenericISSWSecurityRomHeader.SIZE_GENERIC);
        } else {
            int pos = 0;

            pos = setSecRomHeaderData(data);

            pos = setCustomerData(data, pos);

            pos = setUcheckedBlobData(data, pos);

            setSignatureData(data, pos);
        }
    }

    private void setSignatureData(byte[] data, int pos) throws Exception {
        byte[] temp;
        // set signature
        temp = new byte[getSignatureSize()];
        if (temp.length != 0) {
            if (data.length - pos < temp.length) {
                throw new Exception("Expected " + temp.length + " bytes of signature but reached EOF");
            }
            System.arraycopy(data, pos, temp, 0, temp.length);
            signature = new GenericISSWSignature(getSignatureSize());
            signature.setData(temp);
            pos = pos + signature.getLength();

        }
    }

    private int setUcheckedBlobData(byte[] data, int pos) throws Exception {
        byte[] temp;
        // set unchecked blob
        temp = new byte[getUncheckedBlobSize()];
        if (temp.length != 0) {
            if (data.length - pos < temp.length) {
                throw new Exception("Expected " + temp.length + " bytes of unchecked blob but reached EOF");
            }
            System.arraycopy(data, pos, temp, 0, temp.length);
            uncheckedBlob = new GenericISSWUncheckedBlob(getUncheckedBlobSize());
            uncheckedBlob.setData(temp);
            pos = pos + uncheckedBlob.getLength();
        }
        return pos;
    }

    private int setSecRomHeaderData(byte[] data) throws Exception {
        int pos;
        byte[] temp;
        // set security rom header
        secRomHeader.setData(data);
        pos = secRomHeader.getLength();

        // set root key
        temp = new byte[getRootKeyDataSize()];

        if (temp.length != 0) {
            if (data.length - pos < temp.length) {
                throw new Exception("Expected " + temp.length + " bytes of root key data but reached EOF");
            }
            System.arraycopy(data, pos, temp, 0, temp.length);
            rootKey.setData(temp);
            pos = pos + rootKey.getLength();
        }
        return pos;
    }

    private int setCustomerData(byte[] data, int pos) throws Exception {
        byte[] temp = new byte[getCustomerPartSize()];
        if (temp.length != 0) {
            if (data.length - pos < temp.length) {
                throw new Exception("Expected " + temp.length + " bytes of customer part but reached EOF");
            }
            System.arraycopy(data, pos, temp, 0, temp.length);

            if (isExtendedCustomerPart(temp)) {
                customerPart = new GenericISSWCustomerPart(ISSWCertificateType.TYPE2);
            }

            customerPart.setData(temp);
            pos = pos + customerPart.getLength();
        }
        return pos;
    }

    private boolean isExtendedCustomerPart(byte[] temp) {
        ByteBuffer buffer = ByteBuffer.allocate(4);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < 4; i++) {
            buffer.put(temp[i]);
        }
        buffer.rewind();
        return buffer.getInt() == GenericCertificate.MAGIC_EXTENDED_CUSTOMER_PART;
    }

    /** {@inheritDoc} */
    @Override
    public byte[] getData() {
        int offset = 0;
        byte[] allData =
            new byte[secRomHeader.getLength() + rootKey.getLength() + customerPart.getLength()
                + uncheckedBlob.getLength()];
        System.arraycopy(secRomHeader.getData(), 0, allData, offset, secRomHeader.getLength());
        offset += secRomHeader.getLength();
        System.arraycopy(rootKey.getData(), 0, allData, offset, rootKey.getLength());
        offset += rootKey.getLength();
        System.arraycopy(customerPart.getData(), 0, allData, offset, customerPart.getLength());
        offset += customerPart.getLength();
        if (uncheckedBlob.getLength() > 0) {
            System.arraycopy(uncheckedBlob, 0, allData, offset, uncheckedBlob.getLength());
            offset += uncheckedBlob.getLength();
        }
        if (signature.getLength() > 0) {
            System.arraycopy(signature, 0, allData, offset, signature.getLength());
            offset += signature.getLength();
        }
        return allData;
    }

    /**
     * @param swType
     *            swType
     * @return key
     */
    public GenericISSWCustomerPartKey getKey(GenericSoftwareType swType) {
        return customerPart.getKey(swType);
    }

    /** {@inheritDoc} */
    @Override
    public int getLength() {
        return getData().length;
    }

    /**
     * @return customer part size
     */
    public int getCustomerPartSize() {
        return secRomHeader.getCustomerPartSize();
    }

    /**
     * @return root key data size
     */
    public int getRootKeyDataSize() {
        return secRomHeader.getRootKeySize();
    }

    /**
     * @return size of unchecked blob
     */
    public int getUncheckedBlobSize() {
        return secRomHeader.getUncheckedBlobSize();
    }

    /**
     * @param data
     *            issw hash
     */
    @Override
    public void setHash(byte[] data) {
        secRomHeader.setHash(data);
    }

    /**
     * @return issw hash
     */
    @Override
    public byte[] getHash() {
        return secRomHeader.getHash();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte[] getSignableData() {
        int nonSignable = getUncheckedBlobSize() + signature.getLength();
        byte[] data = getData();
        byte[] signableData = new byte[data.length - nonSignable];
        System.arraycopy(data, 0, signableData, 0, signableData.length);
        return signableData;
    }

    /**
     * @return signature size
     */
    @Override
    public int getSignatureSize() {
        return secRomHeader.getSignatureSize();
    }

    /**
     * @param value
     *            signature size
     */
    public void setSignatureSize(int value) {
        secRomHeader.setSignatureSize(value);
    }

    /**
     * @param value
     *            unchecked blob size
     */
    public void setUncheckedBlobSize(int value) {
        secRomHeader.setUncheckedBlobSize(value);
    }

    /**
     * @return ISSW signature
     */
    public GenericISSWSignature getSignature() {
        return signature;
    }

    /**
     * @return Unchecked blob
     */
    public GenericISSWUncheckedBlob getUncheckedBlob() {
        return uncheckedBlob;
    }

    /**
     * @param value
     *            magic
     */
    @Override
    public void setMagic(int value) {
        secRomHeader.setMagic(value);
    }

    /**
     * @return magic
     */
    @Override
    public int getMagic() {
        return secRomHeader.getMagic();
    }

    /**
     * @param type
     *            root key type
     */
    public void setRootKeyType(SignatureType type) {
        secRomHeader.setRootKeyType(type);
    }

    /**
     * @return root key type
     */
    public int getRootKeyType() {
        return secRomHeader.getRootKeyType();
    }

    /**
     * @return sec rom header
     */
    public GenericISSWSecurityRomHeader getSecRomHeader() {
        return secRomHeader;
    }

    /**
     * @return get root key
     */
    public GenericKey getRootKey() {
        return rootKey;
    }

    /**
     * @return get customer part
     */
    public GenericISSWCustomerPart getCustomerPart() {
        return customerPart;
    }

    /**
     * Set sec rom header.
     * 
     * @param pSecRomHeader
     *            Sec rom header to set.
     */
    public void setSecRomHeader(GenericISSWSecurityRomHeader pSecRomHeader) {
        pSecRomHeader.setMagic(this.getMagic());
        this.secRomHeader = pSecRomHeader;
    }

    /**
     * Set root key.
     * 
     * @param pRootKey
     *            Root key to set.
     */
    public void setRootKey(GenericKey pRootKey) {
        this.rootKey = pRootKey;
    }

    /**
     * Set customer part.
     * 
     * @param pCustomerPart
     *            Customer part to set.
     */
    public void setCustomerPart(GenericISSWCustomerPart pCustomerPart) {
        this.customerPart = pCustomerPart;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {

        return "" + getMagic();
    }

    /**
     * Method return true if an input file is ISSW
     * 
     * @param data
     *            first 4 bytes from input file
     * @return true if provided bytes are equal to ISSW header
     */
    public boolean isISSW(byte[] data) {

        if (ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN).getInt() == GenericCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED) {
            return true;
        }
        return false;

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public SignatureType getTypeOfKey(ISignerSettings signerSettings) throws SignerException {
        SignatureType typeToReturn = SignatureType.getByValue(getRootKeyType());

        if (typeToReturn == null) {
            throw new SignerException("Root key type (" + getRootKeyType()
                + ") specified in ISSW Certificate is not supported");
        }

        return typeToReturn;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void updateCertificate(ISignerSettings signerSettings, IGenericSignablePayload payload) {

        GenericSignedPayload signedPayload = (GenericSignedPayload) payload;

        secRomHeader.setSignatureSize(getRootKey().getModulusSize());
        secRomHeader.setISSWHashType((HashType) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_PAYLOAD_HASH_TYPE));
        secRomHeader.setRootKeyType((SignatureType) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE));
        secRomHeader.setRootKeyHashType((HashType) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_ROOT_KEY_HASH_TYPE));
        secRomHeader.setSignatureHashType((HashType) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE));

        customerPart.setMajorBuildVersion((Short) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_MAJOR_BUILD_VERSION));
        customerPart.setMinorBuildVersion((Short) signerSettings
            .getSignerSetting(IGenericSignerSettings.KEY_MINOR_BUILD_VERSION));
        customerPart.setFlags((Integer) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_FLAGS));

        // Merge with pre-built header
        GenericISSWCertificate preBuiltHeader = (GenericISSWCertificate) signedPayload.getPreBuiltHeader();
        setPrebuiltHeaderData(preBuiltHeader);

        secRomHeader.setMagic(GenericCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED);
    }

    private void setPrebuiltHeaderData(GenericISSWCertificate preBuiltHeader) {
        if (preBuiltHeader != null) {
            GenericISSWSecurityRomHeader secRomHeaderPrebuilt = preBuiltHeader.getSecRomHeader();

            secRomHeader.setSpeedUpMagic(secRomHeaderPrebuilt.getSpeedUpMagic());
            secRomHeader.setSpeedUpMask(secRomHeaderPrebuilt.getSpeedUpMask());
            secRomHeader.setSpeedUpData(secRomHeaderPrebuilt.getSpeedUpData());
            secRomHeader.setSpeedUpPool(secRomHeaderPrebuilt.getSpeedUpPool());
            secRomHeader.setSpeedUpSpare(secRomHeaderPrebuilt.getSpeedUpSpare());
            secRomHeader.setCodeLength(secRomHeaderPrebuilt.getCodeLength());
            secRomHeader.setRWDataLength(secRomHeaderPrebuilt.getRWDataLength());
            secRomHeader.setZIDataLength(secRomHeaderPrebuilt.getZIDataLength());
            secRomHeader.setISSWLoadLocation(secRomHeaderPrebuilt.getISSWLoadLocation());
            secRomHeader.setISSWStartAddr(secRomHeaderPrebuilt.getISSWStartAddr());
        }
    }
}
