package com.stericsson.sdk.equipment.io.test;

import static org.junit.Assert.assertArrayEquals;

import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortDataListener;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class AbstractPortTest extends TestCase implements IPortDataListener {

    private static final String TEST_PORT_NAME = "TESTPORT";

    private static final byte[] BUFFER = {
        0x1, (byte) 0xFF, 0x3, 0x4, 0x5, 0x6};

    /**
     * 
     */
    @Test
    public void testGetIdentifier() {
        IPort port = new AbstractPort(TEST_PORT_NAME) {

            public int write(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }
        };

        assertEquals(TEST_PORT_NAME, port.getPortName());
    }

    /**
     * @throws PortException
     *             TBD
     * 
     */
    @Test
    public void testRead() throws PortException {
        ILCPort port = new AbstractPort(TEST_PORT_NAME) {

            private int position = 0;

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                System.arraycopy(BUFFER, position, buffer, offset, length);
                position = position + length;
                return length;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }
        };

        assertEquals(1, port.read());
        assertEquals(255, port.read());
    }

    /**
     * @throws PortException
     *             TBD
     * 
     */
    @Test
    public void testReadByteArray() throws PortException {

        Activator.getBundleContext().registerService(IPortDataListener.class.getName(), this,
            new Hashtable<String, String>());

        ILCPort port = new AbstractPort(TEST_PORT_NAME) {

            private int position = 0;

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                System.arraycopy(BUFFER, position, buffer, offset, length);
                position = position + length;
                notifyBytesRead(buffer, offset, length);
                return length;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }
        };

        byte[] buffer = new byte[3];
        assertTrue(port.read(buffer) == 3);
        assertArrayEquals(new byte[] {
            0x1, (byte) 0xFF, 0x3}, buffer);
    }

    /**
     * @throws PortException
     *             TBD
     * 
     */
    @Test
    public void testWriteInt() throws PortException {

        final byte[] finalBuffer = new byte[] {
            0, 0, 0};

        ILCPort port = new AbstractPort(TEST_PORT_NAME) {

            private int position = 0;

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                System.arraycopy(buffer, offset, finalBuffer, position, length);
                position = position + length;
                return length;
            }
        };

        assertTrue(port.write(1) == 1);
        assertArrayEquals(new byte[] {
            1, 0, 0}, finalBuffer);
        assertTrue(port.write(255) == 1);
        assertArrayEquals(new byte[] {
            1, (byte) 0xFF, 0}, finalBuffer);
    }

    /**
     * @throws PortException
     *             TBD
     * 
     */
    @Test
    public void testWriteByteArray() throws PortException {

        final byte[] finalBuffer = new byte[] {
            0, 0, 0, 0, 0, 0};

        ILCPort port = new AbstractPort(TEST_PORT_NAME) {

            private int position = 0;

            public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
                // TODO Auto-generated method stub
                return 0;
            }

            public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
                System.arraycopy(buffer, offset, finalBuffer, position, length);
                position = position + length;
                notifyBytesWritten(buffer, offset, length);
                return length;
            }
        };

        byte[] buffer = new byte[] {
            1, 2, (byte) 0xFF};
        assertTrue(port.write(buffer) == 3);
        assertArrayEquals(new byte[] {
            1, 2, (byte) 0xFF, 0, 0, 0}, finalBuffer);
    }

    /**
     * {@inheritDoc}
     */
    public void bytesRead(IPort port, byte[] buffer, int offset, int count) {
        System.out.println(port.getPortName() + " - bytes read: " + count);
    }

    /**
     * {@inheritDoc}
     */
    public void bytesWritten(IPort port, byte[] buffer, int offset, int count) {
        System.out.println(port.getPortName() + " - bytes written: " + count);
    }

}
