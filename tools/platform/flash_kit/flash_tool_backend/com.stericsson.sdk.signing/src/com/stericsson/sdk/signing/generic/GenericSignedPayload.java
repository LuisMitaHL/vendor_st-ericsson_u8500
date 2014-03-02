package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWSecurityRomHeader;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Basic U8500 image
 * 
 * @author xolabju
 * 
 */
public class GenericSignedPayload implements IGenericSignablePayload {

    /** */
    protected Logger logger;

    /** */
    protected File payloadFile;

    private long payloadLength;

    private byte[] hashValue;

    private GenericCertificate certificate;

    private GenericCertificate preBuiltHeader;

    private byte[] signature;

    private int magic;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedPayload(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(getClass().getName());
        payloadFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        if (!payloadFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (payloadFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }

        try {
            magic = readPayloadContent(4).getInt(0);
        } catch (IOException e) {
            logger.warn("Failed to read magic number");
        }
        if (hasPreBuiltHeader()) {
            try {
                separatePreBuiltHeaderFromPayload();
            } catch (Exception e) {
                throw new SignerException("Failed to separate pre built header from payload: " + e.getMessage());
            }
        }
        payloadLength = payloadFile.length();
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
            try {
                DigestDecorator digester = new DigestDecorator(hashAlgorithm);
                ByteBuffer payloadContent = readPayloadContent();
                digester.update(payloadContent.array());
                hashValue = digester.digest();
            } catch (IOException ioe) {
                logger.error("Failed to generate payload hash value for " + payloadFile.getAbsolutePath() + ": "
                    + ioe.getMessage());
            }
        }
        return hashValue;
    }

    /**
     * @return Payload length
     */
    public long getLength() {
        return payloadLength;
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
            ByteBuffer payloadContent = readPayloadContent();
            ByteBuffer signatureContent = readSignatureContent();

            if (certificateContent != null) {
                channel.write(certificateContent);
            }

            if (signatureContent != null) {
                channel.write(signatureContent);
            }
            channel.write(payloadContent);

        } catch (IOException ioe) {
            logger.error("Failed to write payload contents for " + payloadFile.getAbsolutePath() + ": "
                + ioe.getMessage());
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
            fis = new FileInputStream(payloadFile);
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
     * 
     * @param numBytes
     *            the number of bytes to read from the start of the payload
     * @return the read part
     * @throws IOException
     *             on errors
     */
    protected ByteBuffer readPayloadContent(int numBytes) throws IOException {
        FileChannel channel = null;
        ByteBuffer payloadBuffer = null;
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(payloadFile);
            channel = fis.getChannel();
            payloadBuffer = ByteBuffer.allocate(numBytes);
            payloadBuffer.order(ByteOrder.LITTLE_ENDIAN);
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
        switch (magic) {
            case GenericCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED:
                return true;
            case GenericCertificate.MAGIC_SIGNED_HEADER:
                return true;
            default:
                return false;
        }
    }

    private void separatePreBuiltHeaderFromPayload() throws Exception {
        GenericCertificate preBuiltCert = null;
        byte[] certData = null;
        ByteBuffer payloadContent = readPayloadContent();
        logger.info("Separating pre-built header from payload.");
        switch (magic) {
            case GenericCertificate.MAGIC_ISSW_CUSTOMER_REQUESTED:
                certData = new byte[GenericISSWSecurityRomHeader.SIZE_GENERIC];
                payloadContent.get(certData);
                validateCertificateData(certData);
                preBuiltCert = new GenericISSWCertificate();
                preBuiltCert.setData(certData);
                break;
            case GenericCertificate.MAGIC_SIGNED_HEADER:
                throw new Exception("Already signed!");
                // certData = new byte[48];
                // payloadContent.get(certData);
                // preBuiltCert = new U5500SignedHeader();
                // preBuiltCert.setData(certData);
            default:
                break;
        }
        preBuiltHeader = preBuiltCert;

        // get size of signature and cert
        int signSize = 0;
        if (preBuiltCert != null) {
            signSize = preBuiltCert.getSignatureSize();
            logger.info("Removing " + signSize + " of signature bytes");
        }

        // move payload content to temp file
        File tempFile = File.createTempFile("tempPayload", ".bin");
        tempFile.deleteOnExit();
        FileOutputStream out = null;

        try {
            if (payloadContent != null) {
                out = new FileOutputStream(tempFile);
                out.write(payloadContent.array(), payloadContent.position(), payloadContent.capacity()
                    - payloadContent.position() - signSize);
            }
            payloadFile = tempFile;
        } finally {
            if (out != null) {
                out.close();
            }
        }
    }

    private void validateCertificateData(byte[] certData) throws Exception {
        GenericISSWSecurityRomHeader header = new GenericISSWSecurityRomHeader();
        header.setData(certData);
        if (header.getRootKeySize() != 0) {
            throw new Exception("Root key size != 0. Certificate already signed?");
        }

        if (header.getCustomerPartSize() != 0) {
            throw new Exception("Customer part size != 0. Certificate already signed?");
        }

        if (header.getUncheckedBlobSize() != 0) {
            throw new Exception("Unchecked blob size != 0. Certificate already signed?");
        }

        if (header.getSignatureSize() != 0) {
            throw new Exception("Signature size != 0. Certificate already signed?");
        }
    }

    /**
     * 
     * @return the pre built cert or null if the payload was built without a cer
     */
    public GenericCertificate getPreBuiltHeader() {
        return preBuiltHeader;
    }
}
