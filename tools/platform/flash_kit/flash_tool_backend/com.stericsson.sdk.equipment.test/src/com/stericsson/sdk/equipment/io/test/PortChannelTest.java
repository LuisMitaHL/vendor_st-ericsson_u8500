package com.stericsson.sdk.equipment.io.test;

import static org.junit.Assert.assertArrayEquals;

import java.io.IOException;
import java.nio.ByteBuffer;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.PortChannel;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class PortChannelTest extends TestCase {

    private static final String TEST_PORT_NAME = "TESTPORT";

    /**
     * 
     */
    @Test
    public void testRead() {
        final byte[] byteArray = new byte[] {
            1, 2, 3, 4, 5};

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            public int write(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length) {
                System.arraycopy(byteArray, 0, buffer, offset, length);
                return length;
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

        PortChannel channel = new PortChannel(port);
        ByteBuffer buffer = ByteBuffer.allocateDirect(5);
        IOException ie = null;

        try {
            channel.read(buffer);
        } catch (IOException e) {
            ie = e;
        }

        assertNotNull(ie);

        buffer = ByteBuffer.allocate(5);

        try {
            channel.read(buffer);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(0, buffer.remaining());
        assertArrayEquals(byteArray, buffer.array());
    }

    /**
     * 
     */
    @Test
    public void testReadByteBufferArray() {
        final byte[] byteArray = new byte[] {
            1, 2, 3, 4, 5};

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            private int pos = 0;

            public int write(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length) {
                System.arraycopy(byteArray, pos, buffer, offset, length);
                pos++;
                return length;
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

        PortChannel channel = new PortChannel(port);
        ByteBuffer[] buffers = new ByteBuffer[5];
        buffers[0] = ByteBuffer.allocate(1);
        buffers[1] = ByteBuffer.allocate(1);
        buffers[2] = ByteBuffer.allocate(1);
        buffers[3] = ByteBuffer.allocate(1);
        buffers[4] = ByteBuffer.allocate(1);

        try {
            channel.read(buffers);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(1, buffers[0].array()[0]);
        assertEquals(2, buffers[1].array()[0]);
        assertEquals(3, buffers[2].array()[0]);
        assertEquals(4, buffers[3].array()[0]);
        assertEquals(5, buffers[4].array()[0]);
    }

    /**
     * 
     */
    @Test
    public void testReadByteBufferArrayOffsetLength() {
        final byte[] byteArray = new byte[] {
            1, 2, 3, 4, 5};

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            private int pos = 0;

            public int write(byte[] buffer, int offset, int length) {
                return -1;
            }

            public int read(byte[] buffer, int offset, int length) {
                System.arraycopy(byteArray, pos, buffer, offset, length);
                pos++;
                return length;
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

        PortChannel channel = new PortChannel(port);
        ByteBuffer[] buffers = new ByteBuffer[5];
        buffers[0] = ByteBuffer.allocate(1);
        buffers[1] = ByteBuffer.allocate(1);
        buffers[2] = ByteBuffer.allocate(1);
        buffers[3] = ByteBuffer.allocate(1);
        buffers[4] = ByteBuffer.allocate(1);

        Exception ee = null;

        try {
            channel.read(buffers, 10, 14);
        } catch (ArrayIndexOutOfBoundsException e) {
            ee = e;
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertNotNull(ee);

        try {
            channel.read(buffers, 1, 4);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(1, buffers[1].array()[0]);
        assertEquals(2, buffers[2].array()[0]);
        assertEquals(3, buffers[3].array()[0]);
    }

    /**
     * 
     */
    @Test
    public void testClose() {
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

        PortChannel channel = new PortChannel(port);
        assertTrue(channel.isOpen());
        try {
            channel.close();
        } catch (IOException e) {
            fail(e.getMessage());
        }
        assertFalse(channel.isOpen());
    }

    /**
     * 
     */
    @Test
    public void testWrite() {
        final byte[] byteArray1 = new byte[5];
        final byte[] byteArray2 = new byte[] {
            1, 2, 3, 4, 5};

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            public int write(byte[] buffer, int offset, int length) {
                System.arraycopy(buffer, offset, byteArray1, 0, length);
                return length;
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

        PortChannel channel = new PortChannel(port);
        ByteBuffer buffer = ByteBuffer.allocateDirect(5);
        IOException ie = null;

        try {
            channel.write(buffer);
        } catch (IOException e) {
            ie = e;
        }

        assertNotNull(ie);

        buffer = ByteBuffer.wrap(byteArray2);

        try {
            channel.write(buffer);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(0, buffer.remaining());
        assertArrayEquals(buffer.array(), byteArray1);
    }

    /**
     * 
     */
    @Test
    public void testWriteByteBufferArray() {

        final byte[] byteArray1 = new byte[] {1, 2};
        final byte[] byteArray2 = new byte[] {3, 4};
        final byte[] byteArray3 = new byte[] {5};
        final byte[] byteArray5 = new byte[5];

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            private int pos = 0;

            public int write(byte[] buffer, int offset, int length) {
                System.arraycopy(buffer, offset, byteArray5, pos, length);
                pos += length;
                return length;
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

        ByteBuffer[] buffers = new ByteBuffer[3];
        buffers[0] = ByteBuffer.wrap(byteArray1);
        buffers[1] = ByteBuffer.wrap(byteArray2);
        buffers[2] = ByteBuffer.wrap(byteArray3);

        PortChannel channel = new PortChannel(port);

        try {
            channel.write(buffers);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(0, buffers[0].remaining());
        assertEquals(0, buffers[1].remaining());
        assertEquals(0, buffers[2].remaining());
        assertArrayEquals(new byte[] {1, 2, 3, 4, 5}, byteArray5);
    }

    /**
     * 
     */
    @Test
    public void testWriteByteBufferArrayOffsetLength() {
        final byte[] byteArray1 = new byte[] {1, 2};
        final byte[] byteArray2 = new byte[] {3, 4};
        final byte[] byteArray3 = new byte[] {5};
        final byte[] byteArray5 = new byte[5];

        IPort port = new AbstractPort(TEST_PORT_NAME) {

            private int pos = 0;

            public int write(byte[] buffer, int offset, int length) {
                System.arraycopy(buffer, offset, byteArray5, pos, length);
                pos += length;
                return length;
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

        ByteBuffer[] buffers = new ByteBuffer[3];
        buffers[0] = ByteBuffer.wrap(byteArray1);
        buffers[1] = ByteBuffer.wrap(byteArray2);
        buffers[2] = ByteBuffer.wrap(byteArray3);

        PortChannel channel = new PortChannel(port);

        try {
            channel.write(buffers, 1, 2);
        } catch (IOException e) {
            fail(e.getMessage());
        }

        assertEquals(2, buffers[0].remaining());
        assertEquals(0, buffers[1].remaining());
        assertEquals(1, buffers[2].remaining());
        assertArrayEquals(new byte[] {3, 4, 0, 0, 0}, byteArray5);
    }
}
