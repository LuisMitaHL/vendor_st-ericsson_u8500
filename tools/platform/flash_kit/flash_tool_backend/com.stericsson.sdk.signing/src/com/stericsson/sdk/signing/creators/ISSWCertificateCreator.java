package com.stericsson.sdk.signing.creators;

import java.io.FileOutputStream;
import java.security.interfaces.RSAPublicKey;
import java.util.HashMap;
import java.util.List;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.signing.data.ISSWCertificateData;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.data.KeyData;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPart;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWSecurityRomHeader;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * Creates ISSW certificates.
 * 
 * @author TSIKOR01
 * 
 */
public class ISSWCertificateCreator {
    private static final int EXPONENT_SIZE = 4;

    private static final int DUMMY_SIZE = 2;

    private static final int MOD_SIZE = 2;

    /**
     * Class constructor.
     */
    public ISSWCertificateCreator() {

    }

    /**
     * Creates ISSW certificate based on provided ISSW certificate data.
     * 
     * @param iSSWCertificateData
     *            ISSW certificate data.
     * @throws Exception
     *             Exception.
     */
    public void create(ISSWCertificateData iSSWCertificateData) throws Exception {
        GenericISSWCertificate cert = null;
        if (iSSWCertificateData == null) {
            throw new NullPointerException();
        }

        List<KeyData> keys = iSSWCertificateData.getKey();
        checkKeyNumbers(iSSWCertificateData.getType(), keys);
        cert = createGenericISSWCertificate(iSSWCertificateData);
        writteCertificateToLocation(iSSWCertificateData.getPath(), cert);
    }

    private void checkKeyNumbers(ISSWCertificateType type, List<KeyData> keys) throws Exception {
        if (keys == null || keys.size() == 0) {
            throw new Exception("Number of keys must not be 0.");
        }
        if (type == ISSWCertificateType.TYPE1 && keys.size() > 16) {
            throw new Exception("Number of keys must be in the range 1 to 16.");
        }
        if (type == ISSWCertificateType.TYPE2 && keys.size() > 64) {
            throw new Exception("Number of keys must be in the range 1 to 64.");
        }
    }

    private GenericISSWCertificate createGenericISSWCertificate(ISSWCertificateData iSSWCertificateData)
        throws Exception {
        KeyData rootKeyData = iSSWCertificateData.getRootKey();
        if (rootKeyData == null) {
            throw new NullPointerException("Root key data must not be null.");
        }
        RSAPublicKey key = generateRSAKey(rootKeyData.getPath());
        byte[] modulus = getModulusOfKey(key);
        GenericKey rootKey = createRootKeyPart(modulus, key);
        GenericISSWCustomerPart customerPart = createCustomerPart(iSSWCertificateData);
        GenericISSWSecurityRomHeader secRomHeader = generateSecRomHeader(modulus, customerPart, rootKeyData.getType());
        return setGenericISSWCertificateParts(secRomHeader, rootKey, customerPart);
    }

    private RSAPublicKey generateRSAKey(String keyPath) throws Exception {
        RSAPublicKey publicKey = PEMParser.parseRSAPublicKey(keyPath);
        if (publicKey == null) {
            throw new Exception("Generated RSA public key must not be null.");
        }
        return publicKey;
    }

    private byte[] getModulusOfKey(RSAPublicKey key) {
        byte[] modulusWithSignBit = key.getModulus().toByteArray();
        byte[] modulus = new byte[modulusWithSignBit.length - 1];
        System.arraycopy(modulusWithSignBit, 1, modulus, 0, modulus.length);
        return modulus;
    }

    private GenericKey createRootKeyPart(byte[] modulus, RSAPublicKey key) {
        GenericKey rootKeyPart = new GenericKey();
        rootKeyPart.setModulusSize((short) modulus.length);
        rootKeyPart.setModulus(LittleEndianByteConverter.reverse(modulus));
        rootKeyPart.setPublicExponent(key.getPublicExponent().intValue());
        return rootKeyPart;
    }

    private GenericISSWCustomerPart createCustomerPart(ISSWCertificateData iSSWCertificateData) throws Exception {
        GenericISSWCustomerPart customerPart;
        if (iSSWCertificateData.getType() == ISSWCertificateType.TYPE1) {
            customerPart = createGenericISSWCustomerPart(iSSWCertificateData);
        } else {
            customerPart = createExtendedISSWCustomerPart(iSSWCertificateData);
        }
        return customerPart;
    }

    private GenericISSWCustomerPart createGenericISSWCustomerPart(ISSWCertificateData iSSWCertificateData)
        throws Exception {
        GenericISSWCustomerPart customerPart = new GenericISSWCustomerPart(iSSWCertificateData.getType());
        customerPart.setModelId(iSSWCertificateData.getModelID());
        List<KeyData> keys = iSSWCertificateData.getKey();
        for (int keyIndex = 0; keyIndex < keys.size(); keyIndex++) {
            customerPart.addKey(generateCustomerKey(keyIndex, keys.get(keyIndex)));
        }
        customerPart.setSigningConstraints(fillSigningConstraints(iSSWCertificateData.getSigningConstraints(),
            iSSWCertificateData.getType()));
        return customerPart;
    }

    private GenericISSWCustomerPart createExtendedISSWCustomerPart(ISSWCertificateData iSSWCertificateData)
        throws Exception {
        GenericISSWCustomerPart customerPart = createGenericISSWCustomerPart(iSSWCertificateData);
        if (iSSWCertificateData.getReserved().length > 16) {
            throw new Exception("Reserved bytes array length must be in the range 0 to 16.");
        }
        customerPart.setReserved(iSSWCertificateData.getReserved());
        return customerPart;
    }

    private GenericISSWCustomerPartKey generateCustomerKey(int keyIndex, KeyData keyData) throws Exception {
        if (keyData == null) {
            throw new Exception("Key data must not be null.");
        }
        GenericISSWCustomerPartKey customerKey = new GenericISSWCustomerPartKey();
        RSAPublicKey key = generateRSAKey(keyData.getPath());
        byte[] modulus = getModulusOfKey(key);
        customerKey.setModulusSize((short) modulus.length);
        customerKey.setKeySize(modulus.length + DUMMY_SIZE + MOD_SIZE + EXPONENT_SIZE);
        customerKey.setModulus(LittleEndianByteConverter.reverse(modulus));
        customerKey.setExponent(key.getPublicExponent().intValue());
        customerKey.setKeyTypeWithRevocationHashTypes(keyData.getTypeWithRevocationHashTypes());
        return customerKey;
    }

    private byte[] fillSigningConstraints(HashMap<GenericSoftwareType, Integer> selectedConstraints,
        ISSWCertificateType certType) throws Exception {
        byte[] signingConstraints = new byte[16];
        if (certType == ISSWCertificateType.TYPE2) {
            signingConstraints = new byte[64];
        }

        for (GenericSoftwareType type : GenericSoftwareType.values()) {
            if (selectedConstraints.get(type) != null) {
                if (type.getPosition() >= signingConstraints.length) {
                    throw new Exception("Software type '" + type.name()
                        + "' is not supported for ISSW certificate type '" + certType.name() + "'.");
                }
                signingConstraints[type.getPosition()] = (byte) (selectedConstraints.get(type).intValue() + 1);
            }
        }
        return signingConstraints;
    }

    private GenericISSWSecurityRomHeader generateSecRomHeader(byte[] modulus, GenericISSWCustomerPart customerPart,
        SignatureType rootKeyType) {
        GenericISSWSecurityRomHeader secRomHeader = new GenericISSWSecurityRomHeader();
        secRomHeader.setRootKeySize(modulus.length + DUMMY_SIZE + MOD_SIZE + EXPONENT_SIZE);
        secRomHeader.setCustomerPartSize(customerPart.getData().length);
        secRomHeader.setRootKeyType(rootKeyType);
        return secRomHeader;
    }

    private GenericISSWCertificate setGenericISSWCertificateParts(GenericISSWSecurityRomHeader secRomHeader,
        GenericKey rootKey, GenericISSWCustomerPart customerPart) {
        GenericISSWCertificate isswCert = new GenericISSWCertificate();
        isswCert.setSecRomHeader(secRomHeader);
        isswCert.setRootKey(rootKey);
        isswCert.setCustomerPart(customerPart);
        return isswCert;
    }

    private void writteCertificateToLocation(String outputPath, GenericISSWCertificate isswCert) throws Exception {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(outputPath);
            fos.write(isswCert.getData());
        } finally {
            if (fos != null) {
                fos.close();
            }
        }
    }
}
