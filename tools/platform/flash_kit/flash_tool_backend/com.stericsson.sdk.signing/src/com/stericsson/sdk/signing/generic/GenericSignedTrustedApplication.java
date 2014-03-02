package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;
import java.util.ArrayList;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericTrustedApplicationCertificate;
import com.stericsson.sdk.signing.generic.cert.KeyHashFileWriter;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * Signed U5500 Trusted Application
 * 
 * @author xolabju
 * 
 */
public class GenericSignedTrustedApplication implements IGenericSignablePayload {

    /** */
    protected Logger logger;

    /** */
    private GenericTrustedApplicationCertificate trustedApplication;

    private String payloadPath;

    private byte[] hashValue;

    private GenericCertificate certificate;

    private byte[] signature;

    private int magic;

    private GenericSignerSettings sSettings;

    private String keyHashFilePath;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public GenericSignedTrustedApplication(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(getClass().getName());
        sSettings = signerSettings;
        File inputFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        if (!inputFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (inputFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
        }

        if (hasPreBuiltHeader()) {
            throw new SignerException("The trusted application is already signed.");
        }
        payloadPath = inputFile.getAbsolutePath();
        trustedApplication = new GenericTrustedApplicationCertificate();
        try {
            ByteBuffer content = readPayloadContent();
            trustedApplication.setData(content.array());
        } catch (Exception e) {
            throw new SignerException("Failed to read trusted application content: " + e.getMessage());
        }
        updateTAHash();

        keyHashFilePath = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_HASH_FILE);
        if (keyHashFilePath != null && !keyHashFilePath.endsWith(".bin")) {
            keyHashFilePath += ".bin";
        }
    }

    /**
     * Updates the trusted application hash list
     */
    private void updateTAHash() {
        // set hash type
        trustedApplication.setHashType(HashType.SHA256_HASH);
        byte[] codeAndData = trustedApplication.getCodeAndData();
        if (codeAndData != null && codeAndData.length > 0) {
            ArrayList<byte[]> hashList = new ArrayList<byte[]>();
            int numHash = codeAndData.length / GenericTrustedApplicationCertificate.HASH_LIST_PAGE_SIZE + 1;
            int sizeOfLastPage = codeAndData.length % GenericTrustedApplicationCertificate.HASH_LIST_PAGE_SIZE;
            int pos = 0;
            for (int i = 0; i < numHash; i++) {
                byte[] page =
                    new byte[i == numHash - 1 ? sizeOfLastPage
                        : GenericTrustedApplicationCertificate.HASH_LIST_PAGE_SIZE];
                System.arraycopy(codeAndData, pos, page, 0, page.length);
                pos += page.length;
                DigestDecorator digest = new DigestDecorator(DigestDecorator.SHA_256);
                digest.update(page);
                hashList.add(digest.digest());
            }
            trustedApplication.setHashList(hashList);
        } else {
            logger.warn("Trusted application does not contain any code & data");
        }
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
            digester.update(trustedApplication.getPayload());
            hashValue = digester.digest();
        }
        return hashValue;
    }

    /**
     * @return Payload length
     */
    public long getLength() {
        return trustedApplication.getPayload().length;
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
            ByteBuffer payloadContent = ByteBuffer.wrap(trustedApplication.getData());
            ByteBuffer signatureContent = readSignatureContent();

            if (certificateContent != null) {
                channel.write(certificateContent);
            }
            if (signatureContent != null) {
                channel.write(signatureContent);
            }
            channel.write(payloadContent);

            // add keyHash to the end of signed trusted application
            byte[] keyHash = KeyHashFileWriter.calculateKeyHash(sSettings);
            ByteBuffer keyBuff = ByteBuffer.wrap(keyHash);
            channel.write(keyBuff);

            if (keyHashFilePath != null && !keyHashFilePath.trim().equals("")) {
                KeyHashFileWriter.writeKeyHash(keyHashFilePath, payloadPath, sSettings, keyHash);
            }
        } catch (SignerException se) {
            logger.error("Failed to write payload contents for " + payloadPath + ": " + se.getMessage());
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
        switch (magic) {
            case GenericCertificate.MAGIC_SIGNED_HEADER:
                return true;
            default:
                return false;
        }
    }
}
