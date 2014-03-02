package com.stericsson.sdk.signing.generic;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.security.interfaces.RSAPrivateKey;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.AbstractSigner;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.local.keys.KeyParser;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class GenericSigner extends AbstractSigner {

    private Logger logger;

    /**
     * Constructor
     * 
     * @param pLogger
     *            logger
     */
    public GenericSigner(Logger pLogger) {
        logger = pLogger;
    }

    /**
     * {@inheritDoc}
     */
    public void sign(ISignerSettings settings) throws SignerException {
        GenericSignerSettings signerSettings = (GenericSignerSettings) settings;

        validateInput(signerSettings);

        createOutput(signerSettings);

        StringBuffer buffer = new StringBuffer();
        buffer.append("File '");
        buffer.append((String) settings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        buffer.append("' signed successfully as '");
        buffer.append((String) settings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE));
        buffer.append("'");

        notifyMessage(buffer.toString());

    }

    private void createOutput(GenericSignerSettings signerSettings) throws SignerException {
        String outputFilename = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE);
        FileChannel channel = null;
        FileOutputStream fis = null;

        try {
            fis = new FileOutputStream(outputFilename);
            channel = fis.getChannel();
            GenericSignPackage signPackage = null;
            // get signer service
            ISignerService signerService =
                (ISignerService) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGNER_SERVICE);

            if (signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY) == null) {
                signPackage = GenericSignerHelper.getSignPackage(signerSettings, signerService);
                if (signPackage == null) {
                    throw new SignerException("Failed to get sign package. ");
                }
            }

            String message = "";
            Boolean useCommonFolderBool = (Boolean)signerSettings.getSignerSetting(
                    ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES);
            if(useCommonFolderBool != null && useCommonFolderBool){
                message = "Using the common folder to locate keys and packages...";
            }else{
                message = "Using mirrored structure to locate keys and packages...";
            }

            notifyMessage(message);
            notifyMessage("Fetching payload...");
            IGenericSignablePayload payload = getPayload(signerSettings);
            notifyMessage("Payload fetched");

            // Do the signing
            notifyMessage("Signing...");
            SignatureType signatureType = null;
            GenericCertificate certificate = null;
            if (signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY) != null) {
                certificate = new GenericSignedHeader();
                signatureType = (SignatureType)signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE);
                RSAPrivateKey crtKey = KeyParser.parsePEMPrivateKey(
                        signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY).toString());

                if (crtKey == null) {
                    throw new SignerException("Key error: Specified key path is not valid");
                }
                if (signatureType == null) {
                    throw new SignerException("Signature error: Signature type is not defined");
                }

                signerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE, (short) (crtKey
                        .getModulus().bitLength()/8));
            } else {
                // get certificate from sign package
                certificate = GenericCertificateFactory.getCertificate(signPackage, payload);
                signatureType = getSignatureType(signerSettings, certificate);

            }

            payload.setCertificate(certificate);
            // calculate payload hash
            calculatePayloadHash(signerSettings, certificate, payload);
            // get input signature type, eventually get default RSASSA_PKCS_V1_5

            if (signatureType == null) {
                throw new SignerException("Signature error: Signature type is not specified");
            }

            setSignatureType(signerSettings, signatureType);
            certificate.updateCertificate(signerSettings, payload);
            // calculate signature

            byte[] signature =
                signerService.signU5500(certificate.getSignableData(),
                    getSignatureHashType(certificate, signerSettings), signatureType);

            payload.setSignature(signature);

            notifyMessage("Signing done");

            // Write payload
            notifyMessage("Writing signed payload to output file...");
            payload.write(channel);
        } catch (IOException e) {
            throw new SignerException(e);
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }

    }

    private void calculatePayloadHash(GenericSignerSettings signerSettings, GenericCertificate certificate,
        IGenericSignablePayload payload) throws SignerException {

        int payloadHashType = getPayloadHashType(signerSettings, certificate);
        byte[] payloadHash = payload.getHashValue(payloadHashType);
        certificate.setHash(payloadHash);
    }

    private int getSignatureHashType(GenericCertificate certificate, GenericSignerSettings signerSettings)
        throws SignerSettingsException {
        if (certificate instanceof GenericCustomerRootKeyCertificate) {
            return ((GenericCustomerRootKeyCertificate) certificate).getCustomerRootKeySignatureHashType();
        } else {
            Object hashTypeObj = signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE);
            if (hashTypeObj == null || !(hashTypeObj instanceof HashType)) {
                signerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE, HashType.SHA256_HASH);
            }

            HashType hashType =
                (HashType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_HASH_TYPE);
            return getHashType(hashType);
        }
    }

    private int getPayloadHashType(GenericSignerSettings signerSettings, GenericCertificate certificate)
        throws SignerSettingsException {

        String hashTypeString;
        if (certificate instanceof GenericCustomerRootKeyCertificate) {
            hashTypeString = IGenericSignerSettings.KEY_ROOT_KEY_HASH_TYPE;
        } else {
            hashTypeString = IGenericSignerSettings.KEY_PAYLOAD_HASH_TYPE;
        }

        Object hashTypeObj = signerSettings.getSignerSetting(hashTypeString);
        if (hashTypeObj == null || !(hashTypeObj instanceof HashType)) {
            signerSettings.setSignerSetting(hashTypeString, HashType.SHA256_HASH);
        }

        HashType hashType = (HashType) signerSettings.getSignerSetting(hashTypeString);
        return getHashType(hashType);
    }

    private int getHashType(HashType hashType) {
        int algorithm = DigestDecorator.SHA_256;

        switch (hashType) {
            case SHA1_HASH:
                algorithm = DigestDecorator.SHA_1;
                break;
            case SHA256_HASH:
                algorithm = DigestDecorator.SHA_256;
                break;
            case SHA384_HASH:
                algorithm = DigestDecorator.SHA_384;
                break;
            case SHA512_HASH:
                algorithm = DigestDecorator.SHA_512;
                break;
            default:
                algorithm = DigestDecorator.SHA_256;
                break;
        }
        return algorithm;
    }

    private void setSignatureType(GenericSignerSettings pSignerSettings, SignatureType signatureType)
        throws SignerSettingsException {
        pSignerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE, signatureType);
    }

    private SignatureType getSignatureType(GenericSignerSettings pSignerSettings, GenericCertificate certificate)
        throws SignerException {

        SignatureType typeToReturn = certificate.getTypeOfKey(pSignerSettings);
        if (typeToReturn == null) {
            logger.debug("Setting default signature type PKCS V1.5");
            typeToReturn = SignatureType.RSASSA_PKCS_V1_5;
        }

        if (typeToReturn != SignatureType.RSASSA_PKCS_V1_5 && typeToReturn != SignatureType.RSASSA_PPS) {
            throw new SignerException("Selected signature type is not supported by sign tool.");
        }

        return typeToReturn;
    }

    /**
     * @param signerSettings
     *            signer settings
     * @return payload
     * @throws SignerException
     *             exception
     */
    protected IGenericSignablePayload getPayload(GenericSignerSettings signerSettings) throws SignerException {
        return GenericPayloadFactory.getPayload(signerSettings);
    }

    private void validateInput(GenericSignerSettings signerSettings) {
    }
}
