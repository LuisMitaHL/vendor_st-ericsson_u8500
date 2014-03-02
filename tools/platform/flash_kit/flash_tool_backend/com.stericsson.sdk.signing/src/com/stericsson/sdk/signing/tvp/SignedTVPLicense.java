package com.stericsson.sdk.signing.tvp;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.IPayload;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * 
 * @author xolabju
 * 
 */
public class SignedTVPLicense implements IPayload {

    /** */
    protected Logger logger;

    /** */
    protected File payloadFile;

    private long payloadLength;

    private byte[] hashValue;

    private byte[] signature;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     * @throws SignerException
     *             on errors
     */
    public SignedTVPLicense(GenericSignerSettings signerSettings) throws SignerException {
        logger = Logger.getLogger(getClass().getName());
        payloadFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        if (!payloadFile.exists()) {
            throw new SignerException("Failed to read file "
                + signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        } else if (payloadFile.isDirectory()) {
            throw new SignerException(signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE)
                + " is a directory");
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
            ByteBuffer payloadContent = readPayloadContent();
            ByteBuffer signatureContent = readSignatureContent();
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
}
