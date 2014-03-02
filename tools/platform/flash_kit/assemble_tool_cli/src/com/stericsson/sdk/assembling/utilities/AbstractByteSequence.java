package com.stericsson.sdk.assembling.utilities;

import java.io.EOFException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;
import java.util.LinkedHashMap;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class AbstractByteSequence {

    /**
     * A linked hashmap that holds the fields in the sequence referenced by integers
     */
    protected LinkedHashMap<Integer, ByteBuffer> fields;

    /** The byte order of the created byte buffers */
    protected static final ByteOrder BYTE_ORDER = ByteOrder.LITTLE_ENDIAN;

    /**
     * Constructor
     */
    public AbstractByteSequence() {
        fields = new LinkedHashMap<Integer, ByteBuffer>();
    }

    /**
     * Reads a byte channel and puts the data in the sequence
     * 
     * @param input
     *            the readable channel
     * 
     * @throws IOException
     *             if the reading failed
     */
    public void read(ReadableByteChannel input) throws IOException {

        for (ByteBuffer b : fields.values()) {
            b.rewind();
            while (b.remaining() > 0) {
                if (input.read(b) == -1) {
                    throw new EOFException("Could not read field fully");
                }
            }
        }
    }

    /**
     * Gets the sequence data as a byte array
     * 
     * @return the bytes contained in the sequence
     */
    public byte[] getData() {

        ByteBuffer result = ByteBuffer.allocate(getLength());

        for (ByteBuffer b : fields.values()) {
            b.rewind();
            result.put(b);
        }
        result.order(BYTE_ORDER);
        return result.array();
    }

    /**
     * Gets the length of the sequence
     * 
     * @return the length of the sequence
     */
    public int getLength() {

        int result = 0;

        for (ByteBuffer b : fields.values()) {
            result += b.capacity();
        }

        return result;
    }

    /**
     * Replaces the sequence data with the data contained in <code>data</code>
     * 
     * @param data
     *            the data to be inserted in the sequence
     * @throws Exception
     *             if data is null or if data.length differs from the length of the byte sequence
     */
    public void setData(byte[] data) throws Exception {
        if (data == null) {
            throw new Exception("Data is null");
        } else if (data.length != getLength()) {
            throw new Exception("Length of data is " + data.length + "bytes, expected " + getLength() + " bytes.");
        } else {
            int pos = 0;
            for (ByteBuffer b : fields.values()) {
                b.rewind();
                byte[] temp = new byte[b.capacity()];
                System.arraycopy(data, pos, temp, 0, b.capacity());
                b.put(temp);
                b.order(BYTE_ORDER);
                pos += b.capacity();
            }
        }
    }

    /**
     * Allocates a new field in the sequence and returns it
     * 
     * @param size
     *            the size of the field
     * @return the allocated field (as a ByteBuffer)
     */
    protected ByteBuffer allocateField(int size) {
        ByteBuffer result = ByteBuffer.allocate(size);
        result.order(BYTE_ORDER);

        return result;
    }
}
