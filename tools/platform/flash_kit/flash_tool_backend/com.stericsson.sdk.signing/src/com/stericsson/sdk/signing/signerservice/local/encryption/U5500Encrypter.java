package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.security.Signature;
import java.security.interfaces.RSAPrivateKey;

import org.apache.log4j.Logger;
import org.bouncycastle.crypto.Digest;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.crypto.digests.SHA256Digest;
import org.bouncycastle.crypto.digests.SHA384Digest;
import org.bouncycastle.crypto.digests.SHA512Digest;
import org.bouncycastle.crypto.engines.RSAEngine;
import org.bouncycastle.crypto.params.RSAKeyParameters;
import org.bouncycastle.crypto.signers.PSSSigner;

import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * Class used to calculate the signature for U5500 software
 * 
 * @author xolabju
 * 
 */
public class U5500Encrypter {

    private Logger logger;

    private static final String ALGORITHM_SHA1withRSA = "SHA1withRSA";

    private static final String ALGORITHM_SHA256withRSA = "SHA256withRSA";

    private static final String ALGORITHM_SHA384withRSA = "SHA384withRSA";

    private static final String ALGORITHM_SHA512withRSA = "SHA512withRSA";

    /**
     * Constructor Creates a new instance of U5500Encrypter
     */
    public U5500Encrypter() {
        logger = Logger.getLogger(getClass().getName());
    }

    /**
     * Calculates the signature for a given byte array of data
     * 
     * @param data
     *            the data to be signed
     * @param privateKey
     *            the private key to use
     * @param signatureHashTypeAlgoritm
     *            signature hash algorithm
     * @return the signed data
     * @throws SignerServiceException
     *             on errors
     */
    public byte[] calculateU5500Signature(byte[] data, RSAPrivateKey privateKey, int signatureHashTypeAlgoritm)
        throws SignerServiceException {
        byte[] signatureBytes = null;
        Signature signature = null;
        boolean invertable = true;

        do {
            try {
                signature = Signature.getInstance(getAlgorithm(signatureHashTypeAlgoritm));
                signature.initSign(privateKey);
                signature.update(data);
                signatureBytes = signature.sign();
                invertable = true;
            } catch (Exception e) {
                if (e.getMessage().contains("BigInteger not invertible")) {
                    logger.info(e.getMessage() + "New attempt.");
                    invertable = false;
                } else {
                    throw new SignerServiceException("Error when signing U5500 certificate: " + e.getMessage());
                }
            }
        } while (!invertable);

        return signatureBytes;
    }

    private String getAlgorithm(int signatureHashTypeAlgoritm) {

        if (signatureHashTypeAlgoritm == HashType.SHA1_HASH.getHashType()) {
            return ALGORITHM_SHA1withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA256_HASH.getHashType()) {
            return ALGORITHM_SHA256withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA384_HASH.getHashType()) {
            return ALGORITHM_SHA384withRSA;
        } else if (signatureHashTypeAlgoritm == HashType.SHA512_HASH.getHashType()) {
            return ALGORITHM_SHA512withRSA;
        }
        return null;
    }

    /**
     * @param data unsigned data
     * @param key private key
     * @param signatureHashType type of signature's hash
     * @return signature
     * @throws SignerServiceException Error when calculating signature
     */
    public byte[] calculateU5500PSSSignature(byte[] data, RSAPrivateKey key, int signatureHashType)
        throws SignerServiceException {

        try {

//            Signature rsaPss = Signature.getInstance("RSASSA-PSS");
//            rsaPss.initSign(key);
//            rsaPss.update(data);
//            return rsaPss.sign();

             RSAEngine eng = new RSAEngine();

             RSAKeyParameters keyParam = new RSAKeyParameters(true, key.getModulus(),
             key.getPrivateExponent());

             PSSSigner signer = new PSSSigner(eng, getDigest(signatureHashType), HashType.getHashSize(signatureHashType));
             signer.init(true, keyParam);
             signer.update(data, 0, data.length);

             return signer.generateSignature();
        } catch (Exception e) {
            throw new SignerServiceException("Error when calculating signature: " + e.getMessage());
        }
    }

    private Digest getDigest(int signatureHashType) {
        Digest digest;
        switch (signatureHashType) {
            case GenericCertificate.SHA1_HASH_TYPE:
            case GenericCertificate.SHA1_HMAC_TYPE:
                digest = new SHA1Digest();
                break;
            case GenericCertificate.SHA256_HASH_TYPE:
            case GenericCertificate.SHA256_HMAC_TYPE:
                digest = new SHA256Digest();
                break;
            case GenericCertificate.SHA384_HASH_TYPE:
            case GenericCertificate.SHA384_HMAC_TYPE:
                digest = new SHA384Digest();
                break;
            case GenericCertificate.SHA512_HASH_TYPE:
            case GenericCertificate.SHA512_HMAC_TYPE:
                digest = new SHA512Digest();
                break;
            default:
                digest = new SHA256Digest();
                break;
        }
        return digest;
    }

}
