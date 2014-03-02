package com.stericsson.sdk.loader.communication.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.internal.ReaderTask;
import com.stericsson.sdk.loader.communication.internal.Task;
import com.stericsson.sdk.loader.communication.internal.WriterTask;

/**
 * @author xdancho
 * 
 */
public class TaskTest extends TestCase {

    int expLen = 1;

    long expDataPtr = 2L;

    long expInstancePtr = 3L;

    byte[] expData = new byte[] {
        (byte) 0xBA, (byte) 0xBE};

    String portName = "portName";

    ILCPort p = new ILCPort() {

        final Integer portId = IPort.PortIdGenerator.INSTANCE.getNextId();

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }

        public int write(byte[] buffer, int offset, int length) throws PortException {
            return 0;
        }

        public int write(byte[] buffer, int timeout) throws PortException {
            return expLen;
        }

        public int write(byte[] buffer) throws PortException {
            return 0;
        }

        public int write(int byteValue, int timeout) throws PortException {
            return 0;
        }

        public int write(int byteValue) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return expLen;
        }

        public int read(byte[] buffer, int offset, int length) throws PortException {
            // TODO Auto-generated method stub
            return 0;
        }

        public int read(byte[] buffer, int timeout) throws PortException {
            return 1;
        }

        public int read(byte[] buffer) throws PortException {
            return 0;
        }

        public int read(int timeout) throws PortException {
            return 0;
        }

        public int read() throws PortException {
            return 0;
        }

        public void open() throws PortException {
        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return portName;
        }

        public void close() throws PortException {
        }

        public Integer getPortIdentifier() {
            return portId;
        }
    };

    ILCPort writePortFails = new ILCPort() {

        final Integer portId = IPort.PortIdGenerator.INSTANCE.getNextId();

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }

        public int write(byte[] buffer, int offset, int length) throws PortException {
            return 0;
        }

        public int write(byte[] buffer, int timeout) throws PortException {
            return -1;
        }

        public int write(byte[] buffer) throws PortException {
            return 0;
        }

        public int write(int byteValue, int timeout) throws PortException {
            return 0;
        }

        public int write(int byteValue) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int offset, int length) throws PortException {
            return 0;
        }

        public int read(byte[] buffer, int timeout) throws PortException {
            return 0;
        }

        public int read(byte[] buffer) throws PortException {
            return 0;
        }

        public int read(int timeout) throws PortException {
            return 0;
        }

        public int read() throws PortException {
            return 0;
        }

        public void open() throws PortException {
        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return null;
        }

        public void close() throws PortException {
        }

        public Integer getPortIdentifier() {
            return portId;
        }
    };

    /**
     * Test the commandHandler
     */
    @Test
    public void testTask() {

        Task t = new ReaderTask(expLen, expDataPtr, expInstancePtr, p);
        t.setData(expData);

        assertEquals(expLen, t.getLength());
        assertEquals(expDataPtr, t.getDataPointer());
        assertEquals(expInstancePtr, t.getInstancePointer());
        assertEquals(portName, ((IPort)t.getPort()).getPortName());

    }

    /**
     * Test the commandHandler
     */
    @Test
    public void testWriterTask() {

        Task t = new WriterTask(expLen, expDataPtr, expInstancePtr, p);
        Task tFail = new WriterTask(expLen, expDataPtr, expInstancePtr, writePortFails);

        try {
            t.process();
            assertTrue(t.isDone());
        } catch (Exception e) {
            fail();
        }

        try {
            tFail.process();
            fail();
        } catch (Exception e) {
            assertFalse(tFail.isDone());
        }
    }

    /**
     * Test the commandHandler
     */
    @Test
    public void testReaderTask() {
        Task t = new ReaderTask(expLen, expDataPtr, expInstancePtr, p);

        try {
            t.process();
            assertTrue(t.isDone());
        } catch (Exception e) {
            fail();
        }
    }
}
