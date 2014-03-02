package com.stericsson.sdk.signing.generic.cert;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.interfaces.RSAPublicKey;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.generic.SignatureTypeWithRevocationHashTypes;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPart;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.util.DigestDecorator;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * @author xtomzap
 * 
 */
public final class KeyHashFileWriter {

    private static Logger logger = Logger.getLogger(KeyHashFileWriter.class.getName());

    /** */
    private KeyHashFileWriter() {

    }

    /**
     * Writes hash of used key to key hash file
     * 
     * @param keyHashFilePath
     *            path to key hash file
     * @param payloadPath
     *            payload path
     * @param settings
     *            signer settings
     * @param keyHash
     *            hash of a key
     */
    public static void writeKeyHash(String keyHashFilePath, String payloadPath, ISignerSettings settings, byte[] keyHash) {
        FileOutputStream fileoutputstream = null;

        try {
            if (keyHashFilePath != null && !keyHashFilePath.trim().equals("")) {
                File keyHashFile = new File(keyHashFilePath);

                if (keyHashFile.isDirectory()) {
                    throw new Exception("Specified key hash file is a directory.");
                }

                fileoutputstream = new FileOutputStream(keyHashFile);
                fileoutputstream.write(keyHash);
            }
        } catch (Exception e) {
            logger.error("Failed to write key hash contents for " + payloadPath + ": " + e.getMessage());
        } finally {
            if (fileoutputstream != null) {
                try {
                    fileoutputstream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private static byte[] getModulusOfKey(RSAPublicKey key) {
        byte[] modulusWithSignBit = key.getModulus().toByteArray();
        byte[] modulus = new byte[modulusWithSignBit.length - 1];
        System.arraycopy(modulusWithSignBit, 1, modulus, 0, modulus.length);
        return modulus;
    }

    /**
     * Calculates hash of given key.
     * 
     * @param settings
     *            signer settings.
     * @return hash of a key.
     * @throws IOException
     *             if some problem occurs with reading of key file.
     * @throws SignerException
     *             generic signer exception.
     */
    public static byte[] calculateKeyHash(ISignerSettings settings) throws IOException, SignerException {

        if (settings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY) != null) {
           RSAPublicKey key = PEMParser.parseRSAPublicKey(
                    settings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY).toString());

           if (key == null) {
               throw new SignerException("Key error: Specified key path is not valid");
           }

           GenericISSWCustomerPartKey customerKey = new GenericISSWCustomerPartKey();
            byte[] modulus = getModulusOfKey(key);
            customerKey.setModulusSize((short) modulus.length);
            customerKey.setKeySize(modulus.length + 2 + 2 + 4);
            customerKey.setModulus(LittleEndianByteConverter.reverse(modulus));
            customerKey.setExponent(key.getPublicExponent().intValue());
            customerKey.setKeyTypeWithRevocationHashTypes(
                new SignatureTypeWithRevocationHashTypes((
                    SignatureType)settings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE)));
            DigestDecorator digester = new DigestDecorator(DigestDecorator.SHA_256);
            digester.update(customerKey.getKey().getData());
            return digester.digest();
        } else {
            Object signPackageObject = settings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);
            if (signPackageObject instanceof GenericSignPackage) {
                GenericSignPackage signPackage = (GenericSignPackage) signPackageObject;
                GenericCertificate genericCert = signPackage.getCertificate();

                if (genericCert instanceof GenericISSWCertificate) {
                    GenericISSWCustomerPart customerPart = ((GenericISSWCertificate) genericCert).getCustomerPart();
                    GenericISSWCustomerPartKey key = customerPart.getKey(GenericSoftwareType.TRUSTED);
                    checkKey(key);
                    byte[] keyData = key.getKey().getData();

                    DigestDecorator digester = new DigestDecorator(DigestDecorator.SHA_256);
                    digester.update(keyData);

                    return digester.digest();
                } else {
                    throw new SignerException(
                        "Key hash calculation error: Failed to get issw certificate from sign package.");
                }
            } else {
                throw new SignerException("Key hash calculation error: Failed to get generic sign package from settings.");
            }
        }

    }

    /**
     * @param key
     * @throws SignerException
     */
    private static void checkKey(GenericISSWCustomerPartKey key)
        throws SignerException {
        if (key == null) {
            throw new SignerException(
                "Key hash calculation error: Could not get key from certificate based on software type "
                    + GenericSoftwareType.TRUSTED);
        }
    }
}
