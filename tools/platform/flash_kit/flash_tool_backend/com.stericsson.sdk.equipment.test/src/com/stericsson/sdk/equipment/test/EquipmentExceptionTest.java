package com.stericsson.sdk.equipment.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentException;

/**
 * @author xtomlju
 */
public class EquipmentExceptionTest extends TestCase {

    private static final String EXCEPTION_MESSAGE = "Equipment Exception Message";

    /**
     * 
     */
    @Test
    public void testEquipmentException() {
        EquipmentException ee = null;
        try {
            throw new EquipmentException(EXCEPTION_MESSAGE);
        } catch (EquipmentException e) {
            ee = e;
        }

        assertNotNull(ee);
        assertEquals(EXCEPTION_MESSAGE, ee.getMessage());
    }

}
