package com.stericsson.sdk.signing.cops;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

/**
 * Hash list container used in COPS
 */
public class COPSHashList {

    private ByteBuffer hashListBuffer;

    private ByteArrayOutputStream byteOutput;

    private int hashListLength;

    private int numberOfHashValues;

    /**
     * Constructor that creates an empty hash list
     */
    public COPSHashList() {
        byteOutput = new ByteArrayOutputStream();
        hashListLength = 0;
        numberOfHashValues = 0;
    }

    /**
     * Constructor
     * 
     * @param channel
     *            Channel to read hash list data from
     * @param controlFields
     *            Control fields
     * @throws IOException
     *             If an I/O related error occurred
     */
    public COPSHashList(ReadableByteChannel channel, COPSControlFields controlFields) throws IOException {
        hashListBuffer = ByteBuffer.allocate(controlFields.getHashListLength());
        channel.read(hashListBuffer);
    }

    /**
     * @return The hash list as a byte array
     */
    public byte[] getBytes() {
        hashListBuffer = ByteBuffer.wrap(byteOutput.toByteArray());
        return hashListBuffer.array();
    }

    /**
     * @param output
     *            Writable byte channel to write to
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel output) throws IOException {
        hashListBuffer = ByteBuffer.wrap(byteOutput.toByteArray());
        output.write(hashListBuffer);
    }

    /**
     * @return The hash list length
     */
    public int getLength() {
        return hashListLength;
    }

    /**
     * @param hash
     *            Hash value to append to list
     */
    public void append(byte[] hash) {

        try {
            byteOutput.write(hash);
            hashListLength += hash.length;
            numberOfHashValues++;
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * @return Number of hash values appended
     */
    public int getNumberOfHashValues() {
        return numberOfHashValues;
    }

}
