package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCustomerRootKeyCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;
import com.stericsson.sdk.signing.util.DigestDecorator;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * Signed Generic Customer Root Key Certificate
 * 
 * @author xolabju
 * 
 */
public class GenericSignedCustomerRootKeyCertificate implements IGenericSignablePayload {

    /** */
    protected Logger logger;

    private String payloadPath;

    private byte[] hashValue;

    private GenericCustomerRootKeyCertificate certificate;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedCustomerRootKeyCertificate(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(getClass().getName());
        File inputFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        if (!inputFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (inputFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }

        payloadPath = inputFile.getAbsolutePath();
    }

    /**
     * 
     * @param data
     *            the signature data
     */
    public void setSignature(byte[] data) {
        certificate.setSignature(data);
    }

    /**
     * 
     * @return the signature
     */
    public byte[] getSignature() {
        return certificate.getSignature();
    }

    /**
     * @return the certificate
     */
    public GenericCertificate getCertificate() {
        return certificate;
    }

    /**
     * @param cert
     *            the certificate to set
     * 
     */
    public void setCertificate(GenericCertificate cert) {
        if (cert instanceof GenericCustomerRootKeyCertificate) {
            this.certificate = (GenericCustomerRootKeyCertificate) cert;
        }
    }

    /**
     * @return Payload destination address
     */
    public long getDestinationAddress() {
        // not used
        return 0;
    }

    /**
     * Calculates and returns the hash value for the payload
     * 
     * @see DigestDecorator#SHA_1
     * @see DigestDecorator#SHA_256
     * @param hashAlgorithm
     *            Algorithm to use for hashing
     * @return Payload hash
     */
    public byte[] getHashValue(int hashAlgorithm) {
        if (hashValue == null || hashValue.length <= 0) {
            try {
                GenericKey key = PEMParser.parseU5500Key(payloadPath);
                DigestDecorator digester = new DigestDecorator(hashAlgorithm);
                digester.update(key.getData());
                hashValue = digester.digest();
            } catch (IOException ioe) {
                logger.error("Failed to generate payload hash value for " + payloadPath + ": " + ioe.getMessage());
            }
        }
        return hashValue;
    }

    /**
     * @return Payload length
     */
    public long getLength() {
        return certificate.getLength();
    }

    /**
     * Writes the payload to specified channel
     * 
     * @param channel
     *            Writable byte channel
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel channel) throws IOException {
        try {
            ByteBuffer certificateContent = readCertificateContent();

            if (certificateContent != null) {
                channel.write(certificateContent);
            }

        } catch (IOException ioe) {
            logger.error("Failed to write payload contents for " + payloadPath + ": " + ioe.getMessage());
        }
    }

    /**
     * 
     * @return the certificate content
     */
    private ByteBuffer readCertificateContent() {
        ByteBuffer buf = null;

        if (certificate != null) {
            buf = ByteBuffer.allocate(certificate.getLength());
            buf.put(certificate.getData(), 0, certificate.getLength());
            buf.rewind();
        }

        return buf;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasPreBuiltHeader() {
        return false;
    }
}
