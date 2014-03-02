package com.stericsson.sdk.equipment.io.test;

import static org.junit.Assert.*;

import org.junit.Test;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.port.PortUtilities;

/**
 * 
 * @author pkutac01
 * 
 */
public class PortUtilitiesTest {

    private IPort portCOM = new IPort() {

        public void open() throws PortException {

        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return "COM1";
        }

        public Integer getPortIdentifier() {
            return IPort.PortIdGenerator.INSTANCE.getNextId();
        }

        public void close() throws PortException {

        }
    };

    private IPort portUSB = new IPort() {

        public void open() throws PortException {

        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return "USB1";
        }

        public Integer getPortIdentifier() {
            return IPort.PortIdGenerator.INSTANCE.getNextId();
        }

        public void close() throws PortException {

        }
    };

    private IPort portttyUSB = new IPort() {

        public void open() throws PortException {

        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return "ttyUSB1";
        }

        public Integer getPortIdentifier() {
            return IPort.PortIdGenerator.INSTANCE.getNextId();
        }

        public void close() throws PortException {

        }
    };

    /**
     * 
     */
    @Test
    public void testIsUSBPort() {
        assertFalse(PortUtilities.isUSBPort(portCOM));
        assertTrue(PortUtilities.isUSBPort(portUSB));
        assertFalse(PortUtilities.isUSBPort(portttyUSB));

        try {
            assertFalse(PortUtilities.isUSBPort(null));
            fail("Should not get here.");
        } catch (NullPointerException e) {
            System.out.println(e.getMessage());
        }
    }

}
