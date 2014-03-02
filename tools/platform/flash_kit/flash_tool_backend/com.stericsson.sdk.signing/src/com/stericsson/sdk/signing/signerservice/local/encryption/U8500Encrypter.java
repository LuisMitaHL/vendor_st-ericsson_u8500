package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.security.Signature;
import java.security.interfaces.RSAPrivateKey;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * Class used to calculate the signature for U8500 software
 * 
 * @author xolabju
 * 
 */
public class U8500Encrypter {

    private static final String ALGORITHM = "SHA1withRSA";

    /**
     * Constructor Creates a new instance of U8500Encrypter
     */
    public U8500Encrypter() {
    }

    /**
     * Calculates the signature for a given byte array of data
     * 
     * @param data
     *            the data to be signed
     * @param privateKey
     *            the private key to use
     * @return the signed data
     * @throws SignerServiceException
     *             on errors
     */
    public byte[] calculateU8500Signature(byte[] data, RSAPrivateKey privateKey) throws SignerServiceException {
        byte[] signatureBytes = null;
        Signature signature = null;
        try {
            signature = Signature.getInstance(ALGORITHM);
            signature.initSign(privateKey);
            signature.update(data);
            signatureBytes = signature.sign();

        } catch (Exception e) {
            throw new SignerServiceException("Error when signing U8500 certificate: " + e.getMessage());
        }
        return signatureBytes;
    }
}
