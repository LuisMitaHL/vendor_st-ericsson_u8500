package com.stericsson.sdk.signing.cops;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

/**
 * Signature container used in COPS
 * 
 * @author xtomlju
 */
public class COPSSignature {

    private ByteBuffer signatureBuffer;

    /**
     * Constructor
     * 
     * @param channel
     *            Readable byte channel to read from
     * @param controlFields
     *            Control fields
     * @throws IOException
     *             If an I/O related error occurred
     */
    public COPSSignature(ReadableByteChannel channel, COPSControlFields controlFields) throws IOException {
        this(controlFields);
        channel.read(signatureBuffer);
    }

    /**
     * @param controlFields
     *            Control fields
     */
    public COPSSignature(COPSControlFields controlFields) {
        signatureBuffer = ByteBuffer.allocate(controlFields.getSignatureLength());
    }

    /**
     * @return Signature as a byte array
     */
    public byte[] getBytes() {
        return signatureBuffer.array();
    }

    /**
     * @param signature
     *            Signature as a byte array
     */
    public void setBytes(byte[] signature) {
        signatureBuffer.rewind();
        signatureBuffer.put(signature);
    }

    /**
     * @param output
     *            Writable byte channel to write to
     * @throws IOException
     *             If an I/O related error occurred
     */
    public void write(WritableByteChannel output) throws IOException {
        signatureBuffer.rewind();
        output.write(signatureBuffer);
    }

}
