package com.stericsson.sdk.equipment.io.test;

import com.stericsson.sdk.equipment.io.DeadPort;
import com.stericsson.sdk.equipment.io.port.PortException;

import junit.framework.TestCase;

/**
 * 
 * @author xolabju
 * 
 */
public class DeadPortTest extends TestCase {

    /**
     * test
     */
    public void testOpen() {
        DeadPort port = new DeadPort();
        try {
            port.open();
            assertFalse(port.isOpen());
        } catch (PortException e) {
            fail(e.getMessage());
        }
    }

    /**
     * test
     */
    public void testRead() {
        DeadPort port = new DeadPort();
        try {
            assertEquals(0, port.read(new byte[0], 0, 0, 0));
        } catch (PortException e) {
            fail(e.getMessage());
        }
    }

    /**
     * test
     */
    public void testWrite() {
        DeadPort port = new DeadPort();
        try {
            assertEquals(0, port.write(new byte[0], 0, 0, 0));
        } catch (PortException e) {
            fail(e.getMessage());
        }
    }

}
