package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Signed U5500 Authentication Certificate
 * 
 * @author xolabju
 * 
 */
public class GenericSignedAuthenticationCertificate implements IGenericSignablePayload {

    /** */
    protected Logger logger;

    private String payloadPath;

    private byte[] hashValue;

    private GenericCertificate certificate;

    private byte[] signature;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedAuthenticationCertificate(GenericSignerSettings signerSettings) throws SignerException {
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
        GenericAuthenticationCertificate authCert = new GenericAuthenticationCertificate();
        try {
            ByteBuffer content = readPayloadContent();
            authCert.setData(content.array());
            if (authCert.getMagic() != GenericCertificate.MAGIC_AUTHENTICATION_CERTIFICATE) {
                throw new SignerException("The authentication certificate is invalid (unexpected magic number).");
            }
            if (authCert.getSignature().length != 0) {
                logger.info("Removing pre-existing signature");
                authCert.setSignatureSize((short) 0);
                authCert.setSignature(new byte[0]);
            }
        } catch (Exception e) {
            throw new SignerException("Failed to read authentication certificate content: " + e.getMessage());
        }
        certificate = authCert;
    }

    /**
     * 
     * @param data
     *            the signature data
     */
    public void setSignature(byte[] data) {
        this.signature = data;
    }

    /**
     * 
     * @return the signature
     */
    public byte[] getSignature() {
        return signature;
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
     */
    public void setCertificate(GenericCertificate cert) {
        this.certificate = cert;
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
            DigestDecorator digester = new DigestDecorator(hashAlgorithm);
            digester.update(certificate.getData());
            hashValue = digester.digest();
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
            ByteBuffer signatureContent = readSignatureContent();

            if (certificateContent != null) {
                channel.write(certificateContent);
            }

            if (signatureContent != null) {
                channel.write(signatureContent);
            }

        } catch (IOException ioe) {
            logger.error("Failed to write payload contents for " + payloadPath + ": " + ioe.getMessage());
        }
    }

    /**
     * 
     * @return the read signature
     */
    protected ByteBuffer readSignatureContent() {
        ByteBuffer buf = null;

        if (signature != null) {
            buf = ByteBuffer.allocate(signature.length);
            buf.put(signature, 0, signature.length);
            buf.rewind();
        }

        return buf;
    }

    /**
     * 
     * @return the certificate content
     */
    protected ByteBuffer readCertificateContent() {
        ByteBuffer buf = null;

        if (certificate != null) {
            buf = ByteBuffer.allocate(certificate.getLength());
            buf.put(certificate.getData(), 0, certificate.getLength());
            buf.rewind();
        }

        return buf;
    }

    /**
     * 
     * @return the payload as a byte buffer
     * @throws IOException
     *             on errors
     */
    protected ByteBuffer readPayloadContent() throws IOException {
        FileChannel channel = null;
        FileInputStream fis = null;
        ByteBuffer payloadBuffer = null;
        try {
            fis = new FileInputStream(payloadPath);
            channel = fis.getChannel();
            payloadBuffer = ByteBuffer.allocate((int) channel.size());
            channel.read(payloadBuffer);
            payloadBuffer.rewind();
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
        return payloadBuffer;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasPreBuiltHeader() {
        return false;
    }
}
