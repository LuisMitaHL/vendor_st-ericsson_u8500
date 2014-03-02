package com.stericsson.sdk.equipment.io.port;

import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

/**
 * @author rohacmic
 * 
 */
public class IOSimulator {
    Iterator<byte[]> readDataIterator;

    Iterator<byte[]> writeDataIterator;

    int nextReadPosition = 0;

    int nextWritePosition = 0;

    byte[] currentReadChunk = null;

    byte[] currentWriteChunk = null;

    /**
     * @param pReadData
     *            TBD
     * @param pWriteData
     *            TBD
     */
    public IOSimulator(List<byte[]> pReadData, List<byte[]> pWriteData) {
        readDataIterator = pReadData.iterator();
        if (readDataIterator.hasNext()) {
            currentReadChunk = readDataIterator.next();
        }

        writeDataIterator = pWriteData.iterator();
        if (writeDataIterator.hasNext()) {
            currentWriteChunk = writeDataIterator.next();
        }
    }

    /**
     * @param data
     *            TBD
     * @param offset
     *            TBD
     * @param length
     *            TBD
     * @return TBD
     * @throws PortException
     *             TBD
     */
    public int read(byte[] data, int offset, int length) throws PortException {
        if (currentReadChunk == null) {
            throw new PortException("No more data ready to read!");
        }
        int len;
        if (nextReadPosition < currentReadChunk.length) {
            len = Math.min(currentReadChunk.length - nextReadPosition, length);
        } else if (readDataIterator.hasNext()) {
            nextReadPosition = 0;
            currentReadChunk = readDataIterator.next();
            len = Math.min(currentReadChunk.length, length);
        } else {
            return 0;
        }

        System.arraycopy(currentReadChunk, nextReadPosition, data, offset, len);
        nextReadPosition += len;
        return len;
    }

    /**
     * @param data
     *            TBD
     * @param offset
     *            TBD
     * @param length
     *            TBD
     * @return TBD
     * @throws PortException
     *             TBD
     */
    public int write(byte[] data, int offset, int length) throws PortException {
        if (currentWriteChunk == null) {
            throw new PortException("No more data ready to read!");
        }
        int len;
        if (nextWritePosition < currentWriteChunk.length) {
            len = Math.min(currentWriteChunk.length - nextWritePosition, length);
        } else if (writeDataIterator.hasNext()) {
            nextWritePosition = 0;
            currentWriteChunk = writeDataIterator.next();
            len = Math.min(currentWriteChunk.length, length);
        } else {
            return 0;
        }

        if (!Arrays.equals(copyOfRange(currentWriteChunk, nextWritePosition, nextWritePosition + len), copyOfRange(
            data, offset, offset + len))) {
            throw new RuntimeException("assertArrayEquals: Expected data for writing to USBDevice doesn't match!");
        }
        // Will need junit what's is not good in this bundle, throw RuntimeException instead..
        // Assert.assertArrayEquals("Expected data for writing to USBDevice doesn't match!",
        // Arrays.copyOfRange(currentWriteChunk, nextWritePosition, nextWritePosition + len),
        // Arrays.copyOfRange(data, offset, offset + len));

        nextWritePosition += len;
        return len;
    }

    private byte[] copyOfRange(byte[] original, int from, int to) {
        int newLength = to - from;
        if (newLength < 0) {
            throw new IllegalArgumentException(from + " > " + to);
        }
        byte[] copy = new byte[newLength];
        System.arraycopy(original, from, copy, 0, Math.min(original.length - from, newLength));
        return copy;
    }

}
