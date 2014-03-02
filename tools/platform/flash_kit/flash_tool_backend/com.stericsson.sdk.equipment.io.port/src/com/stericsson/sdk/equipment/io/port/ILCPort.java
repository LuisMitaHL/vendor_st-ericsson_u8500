package com.stericsson.sdk.equipment.io.port;

/**
 * Add all generic LoaderCommunication port functionality in this interface
 *
 * @author xdancho
 */
public interface ILCPort extends IPort {

    /**
     * Read a single byte from the port.
     *
     * @return An integer ranging from 0 to 255 or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     *
     */
    int read() throws PortException;

    /**
     * Read a single byte from the port.
     *
     * @param timeout
     *            timeout value
     * @return An integer ranging from 0 to 255 or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     *
     */
    int read(int timeout) throws PortException;

    /**
     * Read at most buffer.length bytes into specified buffer.
     *
     * @param buffer
     *            Byte buffer to read into
     * @return The actual number of bytes read or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int read(byte[] buffer) throws PortException;

    /**
     * Read at most buffer.length bytes into specified buffer.
     *
     * @param timeout
     *            timeout value
     * @param buffer
     *            Byte buffer to read into
     * @return The actual number of bytes read or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int read(byte[] buffer, int timeout) throws PortException;

    /**
     * Read at most length bytes into specified buffer starting at specified offset.
     *
     * @param buffer
     *            Byte buffer to read into
     * @param offset
     *            Offset into buffer
     * @param length
     *            Max number of bytes to read
     * @return The actual number of bytes read or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int read(byte[] buffer, int offset, int length) throws PortException;

    /**
     * Read at most length bytes into specified buffer starting at specified offset.
     *
     * @param buffer
     *            Byte buffer to read into
     * @param offset
     *            Offset into buffer
     * @param length
     *            Max number of bytes to read
     * @param timeout
     *            timeout value
     * @return The actual number of bytes read or a negative value if an error occurred.
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int read(byte[] buffer, int offset, int length, int timeout) throws PortException;

    /**
     * Write a single byte value in the range of 0..255.
     *
     * @param byteValue
     *            Value to write
     * @return 1 if the byteValue could be written or -1 if not
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(int byteValue) throws PortException;

    /**
     * Write a single byte value in the range of 0..255.
     *
     * @param byteValue
     *            Value to write
     * @param timeout
     *            timeout value
     * @return 1 if the byteValue could be written or -1 if not
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(int byteValue, int timeout) throws PortException;

    /**
     * Write the contents of the specified buffer to the port.
     *
     * @param buffer
     *            Byte buffer
     * @return Number of bytes actually written
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(byte[] buffer) throws PortException;

    /**
     * Write the contents of the specified buffer to the port.
     *
     * @param buffer
     *            Byte buffer
     * @param timeout
     *            timeout value
     * @return Number of bytes actually written
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(byte[] buffer, int timeout) throws PortException;

    /**
     * @param buffer
     *            Buffer with data to write.
     * @param offset
     *            Offset into given buffer from which data will be written.
     * @param length
     *            Number of bytes to be written.
     * @return Number of bytes actually written.
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(byte[] buffer, int offset, int length) throws PortException;

    /**
     * @param buffer
     *            Buffer with data to write.
     * @param offset
     *            Offset into given buffer from which data will be written.
     * @param length
     *            Number of bytes to be written.
     * @return Number of bytes actually written.
     * @param timeout
     *            timeout value
     * @throws PortException
     *             When I/O error on port occurs.
     */
    int write(byte[] buffer, int offset, int length, int timeout) throws PortException;

}
