package com.stericsson.sdk.equipment.io.usb.internal;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.NativeException;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author emicroh
 * 
 */
public final class USBPort extends AbstractPort {

    private final USBNativeDevice usbDevice;

    /**
     * Constructs instance of this class with given native usb device object.
     * 
     * @param device
     *            Native usb device object which provides basic functionality.
     */
    public USBPort(USBNativeDevice device) {
        super(device.getIdentifier());
        this.usbDevice = device;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void open() throws PortException {
        try {
            usbDevice.openWrapper();
            super.open(); // set flag opened only
        } catch (NativeException e) {
            PortException exception = new PortException(e.getMessage());
            exception.initCause(e);
            throw exception;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void close() throws PortException {
        try {
            usbDevice.closeWrapper();
        } catch (Exception e) {
            PortException exception = new PortException("USB port close operation failed!");
            exception.initCause(e);
            throw exception;
        } finally {
            super.close(); // set flag closed only
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isOpen() {
        return usbDevice.isOpen();
    }

    // A size of 1.5MB should be sufficient for most types of operations
    private static final int BYTE_BUFFER_SIZE = 1024 * 1024 + 1024 * 512;

    // Should only read a multiple of 512 bytes of data from USB
    private static final int MAX_BYTES_TO_READ = 131072;

    // Different error values for Window and Linux and Mac
    private static final int TIMEOUT_WINDOWS = -116;

    private static final int TIMEOUT_LINUX = -110;

    private static final int TIMEOUT_MAC = -60;

    // Allocate an buffer that will be filled with data from a USB reads
    private final byte[] byteBuffer = new byte[BYTE_BUFFER_SIZE];

    // Number of valid bytes in byteBuffer
    private int byteBufferPosition = 0;

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {

        while (length > byteBufferPosition) {
            try {

                int read = usbDevice.bulkRead(byteBuffer, byteBufferPosition, MAX_BYTES_TO_READ, timeout);
                if (read > 0) {
                    byteBufferPosition = byteBufferPosition + read;
                } else if ((read < 0) && (read != TIMEOUT_WINDOWS) && (read != TIMEOUT_LINUX) && (read != TIMEOUT_MAC)) {
                    // Only throw exception if not read timeout error
                    throw new PortException("Port unavailable (error code " + read + ")");
                } else {
                    length = 0;
                    break;
                }
            } catch (IOTimeoutException e) {
                throw new PortException(e);
            } catch (NativeException e) {
                throw new PortException(e);
            }
        }

        if (length > byteBufferPosition) {
            length = byteBufferPosition;
        }

        if (length > 0) {
            System.arraycopy(byteBuffer, 0, buffer, offset, length);
            System.arraycopy(byteBuffer, length, byteBuffer, 0, byteBufferPosition - length);
            notifyBytesRead(buffer, offset, length);
            byteBufferPosition -= length;
        }

        return length;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
        try {
            int written = usbDevice.bulkWrite(buffer, offset, length, timeout);
            if (written > 0) {
                notifyBytesWritten(buffer, offset, written);
            }
            return written;
        } catch (Throwable t) {
            PortException pe = new PortException("USB port write operation failed!");
            pe.initCause(t);
            throw pe;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return usbDevice.getDeviceInfo();
    }
}
