package com.stericsson.sdk.signing.cops;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.util.DigestDecorator;

/**
 * @author xolabju
 */
public class ArchivePayload implements ICOPSPayload {

    private Logger logger;

    private File payloadFile;

    private long payloadLength;

    private byte[] hashValue;

    /**
     * Constructor
     * 
     * @param signerSettings
     *            Signer settings instance
     */
    public ArchivePayload(A2SignerSettings signerSettings) {
        logger = Logger.getLogger(getClass().getName());
        payloadFile = new File((String) signerSettings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE));
        payloadLength = payloadFile.length();
    }

    /**
     * Returns the hash list digested with the specified hash algorithm
     * 
     * @see DigestDecorator#SHA_1
     * @see DigestDecorator#SHA_256
     * @param hashAlgorithm
     *            the hash algorithm (i.e. SHA-1 or SHA-256)
     * @return the hash list as a byte array
     */
    public byte[] getHashList(int hashAlgorithm) {
        return null;
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
            channel.write(payloadContent);
        } catch (IOException ioe) {
            logger.error("Failed write payload contents for " + payloadFile.getAbsolutePath() + ": "
                + ioe.getMessage());
        }
    }

    private ByteBuffer readPayloadContent() throws IOException {
        FileChannel channel = null;
        ByteBuffer payloadBuffer = null;
        FileInputStream fis = null;
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
     * @param fields
     *            TBD
     */
    public void updateCOPSControlFields(COPSControlFields fields) {

    }

}
