package com.stericsson.sdk.equipment.dummy.internal;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * 
 * @author xolabju
 * 
 */
public class DummyPort implements IPort {

    /**
     * test port
     */
    private final String portName;

    private final Integer id = IPort.PortIdGenerator.INSTANCE.getNextId();

    /**
     * Initializes a newly created DummyPort with given port name.
     * @param pPortName port name
     */
    public DummyPort(String pPortName) {
        portName = pPortName;
    }

    /**
     * {@inheritDoc}
     */
    public void close() throws PortException {
    }

    /**
     * {@inheritDoc}
     */
    public String getPortName() {
        return portName;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isOpen() {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void open() throws PortException {
    }

    /**
     * {@inheritDoc}
     */
    public int read() throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public void readFully(byte[] buffer, int offset, int length) throws PortException {
    }

    /**
     * {@inheritDoc}
     */
    public int write(int byteValue) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length) throws PortException {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public void writeFully(byte[] buffer, int offset, int length) throws PortException {
    }

    /**
     * {@inheritDoc}
     */
    public int read(int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(int byteValue, int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
        // TODO Auto-generated method stub
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public Integer getPortIdentifier() {
        return id;
    }

}
