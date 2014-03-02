package com.stericsson.sdk.signing.cops;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

/**
 * Padding used in COPS header
 * 
 * @author xtomlju
 */
public class COPSPadding {

    private ByteBuffer paddingBuffer;

    private int multipleOfBytes = 4;

    /**
     * Constructor
     * 
     * @param channel
     *            Readable byte channel to read padding information from
     * @param controlFields
     *            Control fields
     * @throws IOException
     *             If an I/O error occurred
     */
    public COPSPadding(ReadableByteChannel channel, COPSControlFields controlFields) throws IOException {
        paddingBuffer = ByteBuffer.allocate(controlFields.getPaddingLength());
        channel.read(paddingBuffer);
    }

    /**
     * Constructor
     * 
     * @param controlFields
     *            Control fields
     */
    public COPSPadding(COPSControlFields controlFields) {

        int headerLength = controlFields.getHeaderLength();

        int paddingLength = headerLength % multipleOfBytes;
        if (paddingLength > 0) {
            paddingLength = multipleOfBytes - paddingLength;
        }

        paddingBuffer = ByteBuffer.allocate(paddingLength);
    }

    /**
     * @return Padding information as a byte array
     */
    public byte[] getBytes() {
        return paddingBuffer.array();
    }

    /**
     * @param output
     *            Writable byte channel to write to
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel output) throws IOException {
        paddingBuffer.rewind();
        output.write(paddingBuffer);
    }
}
