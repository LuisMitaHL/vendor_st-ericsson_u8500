package com.stericsson.sdk.signing;

import java.io.IOException;
import java.nio.channels.WritableByteChannel;

/**
 * Interface for payload.
 * 
 * @author xtomlju
 */
public interface IPayload {

    /**
     * Writes the payload to specified channel
     * 
     * @param channel
     *            Writable byte channel
     * @throws IOException
     *             If an I/O related error occurred
     */
    void write(WritableByteChannel channel) throws IOException;

    /**
     * Calculates and returns the hash value for the payload
     * 
     * @see DigestDecorator#SHA_1
     * @see DigestDecorator#SHA_256
     * @param hashAlgorithm
     *            Algorithm to use for hashing
     * @return Payload hash
     * @throws SignerException
     *             If an signer related error occured
     */
    byte[] getHashValue(int hashAlgorithm) throws SignerException;

    /**
     * @return Payload length
     */
    long getLength();

    /**
     * Returns the payload destination address
     * 
     * @return Payload destination address
     */
    long getDestinationAddress();
}
