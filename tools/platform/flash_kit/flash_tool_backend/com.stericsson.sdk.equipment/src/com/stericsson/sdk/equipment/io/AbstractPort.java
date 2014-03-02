package com.stericsson.sdk.equipment.io;

import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortDataListener;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author tomas
 */
public abstract class AbstractPort implements ILCPort {

    private byte[] singleByteBuffer;

    private String portName;

    /** */
    public static final int DEFAULT_RX_TIMEOUT = 50;

    /** */
    public static final int DEFAULT_TX_TIMEOUT = 100000;

    private final Integer portId = IPort.PortIdGenerator.INSTANCE.getNextId();

    // used to simulate default behavior of open(), close(), isClosed() methods
    private boolean opened = false;

    /**
     * Constructor.
     * 
     * @param name
     *            Port name (COM1, USB1 ...)
     */
    public AbstractPort(String name) {
        portName = name;
        singleByteBuffer = new byte[1];
    }

    /** 
     * {@inheritDoc}
     */
    public Integer getPortIdentifier() {
        return portId;
    }

    /**
     * {@inheritDoc}
     */
    public String getPortName() {
        return portName;
    }

    /**
     * {@inheritDoc}
     * 
     * @throws PortException
     */
    public int read() throws PortException {
        int bytesRead = read(singleByteBuffer);
        if (bytesRead == 1) {
            return singleByteBuffer[0] & 0xFF;
        }
        return -1;
    }

    /**
     * {@inheritDoc}
     * 
     * @throws PortException
     */
    public int read(byte[] buffer) throws PortException {
        return read(buffer, 0, buffer.length, DEFAULT_RX_TIMEOUT);
    }

    /**
     * {@inheritDoc}
     * 
     * @throws PortException
     */
    public int read(int timeout) throws PortException {
        int bytesRead = read(singleByteBuffer, timeout);
        if (bytesRead == 1) {
            return singleByteBuffer[0] & 0xFF;
        }
        return -1;

    }

    /**
     * {@inheritDoc}
     * 
     * @throws PortException
     */
    public int read(byte[] buffer, int timeout) throws PortException {
        return read(buffer, 0, buffer.length, timeout);
    }

    /**
     * {@inheritDoc}
     */
    public int write(int byteValue) throws PortException {
        singleByteBuffer[0] = (byte) byteValue;
        return write(singleByteBuffer);
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer) throws PortException {
        return write(buffer, 0, buffer.length, DEFAULT_TX_TIMEOUT);
    }

    /**
     * {@inheritDoc}
     */
    public int write(int byteValue, int timeout) throws PortException {
        singleByteBuffer[0] = (byte) byteValue;
        return write(singleByteBuffer, timeout);
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int timout) throws PortException {
        return write(buffer, 0, buffer.length, DEFAULT_TX_TIMEOUT);
    }

    /**
     * @param buffer
     *            Buffer that has been read into
     * @param offset
     *            Offset in buffer
     * @param bytesRead
     *            Number of bytes read
     */
    protected void notifyBytesRead(byte[] buffer, int offset, int bytesRead) {
        if (Activator.getPortDataListenerTracker() == null) {
            return;
        }

        Object[] listeners = Activator.getPortDataListenerTracker().getServices();
        if (listeners == null) {
            return;
        }

        for (Object l : listeners) {
            ((IPortDataListener) l).bytesRead(this, buffer, offset, bytesRead);
        }
    }

    /**
     * @param buffer
     *            Byte buffer that has been written from
     * @param offset
     *            Buffer offset
     * @param bytesWritten
     *            Number of bytes written
     */
    protected void notifyBytesWritten(byte[] buffer, int offset, int bytesWritten) {
        if (Activator.getPortDataListenerTracker() == null) {
            return;
        }

        Object[] listeners = Activator.getPortDataListenerTracker().getServices();
        if (listeners == null) {
            return;
        }
        for (Object l : listeners) {
            ((IPortDataListener) l).bytesWritten(this, buffer, offset, bytesWritten);
        }
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.io.IPort#close()
     */
    public void close() throws PortException {
        opened = false;
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.io.IPort#isOpen()
     * 
     */
    public boolean isOpen() {
        return opened;
    }

    /**
     * {@inheritDoc}
     * 
     * @see com.stericsson.sdk.equipment.io.IPort#open()
     */
    public void open() throws PortException {
        opened = true;
    }

    /**
     * {@inheritDoc}
     * 
     */
    public int write(byte[] buffer, int offset, int length) throws PortException {
        return write(buffer, offset, length, DEFAULT_TX_TIMEOUT);
    }

    /**
     * {@inheritDoc}
     * 
     */
    public int read(byte[] buffer, int offset, int length) throws PortException {
        return read(buffer, offset, length, DEFAULT_RX_TIMEOUT);
    }

    /**
     * @param baudRate
     *            BaudRate to be set
     * @throws PortException
     *             thrown if set of new speed failed
     */
    public void setSpeed(String baudRate) throws PortException {

    }
}
