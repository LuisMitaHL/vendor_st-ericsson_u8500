package com.stericsson.sdk.equipment.io.uart.internal;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.uart.internal.SerialPortConfiguration.BAUD_RATE;

/**
 * This class implements a proxy pattern for the SerialPort class.
 * 
 * @author rohacmic
 * 
 */
public class SerialPortProxy extends AbstractPort {

    private SerialPort port;

    /**
     * Bytes will be written to serial port in chunks of maximum WRITE_BLOCK bytes. It should take
     * about 0.5s to write 5*1024 bytes on 115200bps. This is introduced to have a fairly good
     * responsibility when retriggering UART port during a transfer. Because on windows it is not
     * possible to drop UART buffer (flush method is waiting for all transfers to end instead) big
     * transfers have to be split so that these can be interrupted in between. To give a relatively
     * good user experience we have to split data transfers to chunks that fit in ~0.5s.
     * 
     * 64*1024 is optimal size due to flashing it is size of flashing blocks it cause maximally
     * delay about 6s lower block size cause slow down of flashing which is not acceptable.
     * 
     * @see SerialPort#trigger()
     */
    private static final int WRITE_BLOCK = 64 * 1024;

    private static final Logger log = Logger.getLogger(SerialPortProxy.class);

    /**
     * Constructs instance of this class with given serial port implementation.
     * 
     * @param pPort
     *            Serial port object which provides basic functionality.
     */
    public SerialPortProxy(SerialPort pPort) {
        super(pPort.getIdentifier());
        this.port = pPort;
    }

    /**
     * 
     * {@inheritDoc}
     * 
     * @throws PortException
     */
    @Override
    public void setSpeed(String baudRate) throws PortException {
        BAUD_RATE br = BAUD_RATE.getByString(baudRate);
        SerialPortConfiguration pc = port.getConfiguration();
        pc.setBaudRate(br);

        try {
            port.set(this, pc);
        } catch (UARTException e) {
            log.error("Can not set serial port configuration");
            PortException exception = new PortException("UART port set speed operation failed!");
            exception.initCause(e);
            throw exception;
        }

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void open() throws PortException {
        try {
            port.open(this);
        } catch (UARTException e) {
            PortException exception = new PortException("UART port open operation failed!");
            exception.initCause(e);
            throw exception;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isOpen() {
        return port.isOpen(this);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void close() throws PortException {
        try {
            port.close(this);
        } catch (UARTException e) {
            throw new PortException(e);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int read(byte[] pBuffer, int pOffset, int pLength) throws PortException {
        try {
            int read = port.read(this, pBuffer, pOffset, pLength);
            if (read > 0) {
                notifyBytesRead(pBuffer, pOffset, read);
            }
            return read;
        } catch (Exception e) {
            PortException pe = new PortException("UART port read operation failed!", e);
            throw pe;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int write(byte[] pBuffer, int pOffset, int pLength) throws PortException {
        try {
            int written = 0;
            int chunk;
            do {
                int toWrite = pLength - written > WRITE_BLOCK ? WRITE_BLOCK : pLength - written;
                chunk = port.write(this, pBuffer, pOffset + written, toWrite);
                if (chunk > 0) {
                    written += chunk;
                }
            } while ((chunk > 0) && (pLength - written > 0));
            if (written > 0) {
                notifyBytesWritten(pBuffer, pOffset, written);
                return written;
            } else {
                return chunk; // chunk contains an error code here or 0
            }
        } catch (Exception e) {
            PortException pe = new PortException("UART port write operation failed!", e);
            throw pe;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return port.getIdentifier();
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
        return read(buffer, offset, length);
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
        return write(buffer, offset, length);
    }
}
