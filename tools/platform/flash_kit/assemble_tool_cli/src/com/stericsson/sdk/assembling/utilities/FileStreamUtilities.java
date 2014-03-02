package com.stericsson.sdk.assembling.utilities;

import java.io.FileInputStream;
import java.io.IOException;

/**
 * Provides methods for file stream operations.
 * 
 * @author pkutac01
 * 
 */
public final class FileStreamUtilities {

    private FileStreamUtilities() {

    }

    /**
     * Reads 16-bit unsigned integer value from provided file input stream.
     * 
     * @param inputStream
     *            File input stream to read value from.
     * @return 16-bit unsigned integer value.
     * @throws IOException
     *             I/O Exception.
     */
    public static int read16BitUnsignedInteger(FileInputStream inputStream) throws IOException {
        byte[] buffer = new byte[2];
        int read = inputStream.read(buffer);
        if (read < buffer.length) {
            throw new IOException("Invalid number of bytes was read from a file stream (expected " + buffer.length
                + ", read " + read + ")");
        }

        int value = ((int) buffer[0]) & 0xFF;
        value |= (((int) buffer[1]) & 0xFF) << 8;

        return value;
    }

    /**
     * Reads 32-bit unsigned integer value from provided file input stream.
     * 
     * @param inputStream
     *            File input stream to read value from.
     * @return 32-bit unsigned integer value.
     * @throws IOException
     *             I/O Exception.
     */
    public static long read32BitUnsignedInteger(FileInputStream inputStream) throws IOException {
        byte[] buffer = new byte[4];
        int read = inputStream.read(buffer);
        if (read < buffer.length) {
            throw new IOException("Invalid number of bytes was read from a file stream (expected " + buffer.length
                + ", read " + read + ")");
        }

        long value = ((long) buffer[0]) & 0xFF;
        value |= (((long) buffer[1]) & 0xFF) << 8;
        value |= (((long) buffer[2]) & 0xFF) << 16;
        value |= (((long) buffer[3]) & 0xFF) << 24;

        return value;
    }

    /**
     * Reads bytes from provided file input stream to provided byte array. Throws exception when
     * number of read bytes is not equal to byte array length.
     * 
     * @param inputStream
     *            File input stream to read bytes from.
     * @param byteArray
     *            Byte array to read bytes into.
     * @throws IOException
     *             I/O Exception.
     */
    public static void readByteArrayFully(FileInputStream inputStream, byte[] byteArray) throws IOException {
        int read = inputStream.read(byteArray);
        if (read < byteArray.length) {
            throw new IOException("Invalid number of bytes was read from a file stream (expected " + byteArray.length
                + ", read " + read + ")");
        }
    }

    /**
     * Skips provided number of bytes from provided file input stream. Throws exception when number
     * of skipped bytes is not equal to required number.
     * 
     * @param inputStream
     *            File input stream to skip bytes from.
     * @param bytes
     *            Number of bytes to skip.
     * @throws IOException
     *             I/O Exception.
     */
    public static void skip(FileInputStream inputStream, long bytes) throws IOException {
        long skip = inputStream.skip(bytes);
        if (skip < bytes) {
            throw new IOException("Invalid number of bytes was skipped from a file stream (expected " + bytes
                + ", skipped " + skip + ")");
        }
    }

}
