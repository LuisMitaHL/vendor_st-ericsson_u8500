package com.stericsson.sdk.equipment.io.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomlju
 */
public class PortExceptionTest extends TestCase {

    private static final String EXCEPTION_MESSAGE = "Could not read bytes";

    /**
     * 
     */
    @Test
    public void testPortException() {
        PortException pe = null;

        try {
            throw new PortException(EXCEPTION_MESSAGE);
        } catch (PortException e) {
            pe = e;
        }

        assertNotNull(pe);
        assertEquals(EXCEPTION_MESSAGE, pe.getMessage());
    }

}
