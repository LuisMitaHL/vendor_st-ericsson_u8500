package com.stericsson.sdk.equipment.test;

import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentTaskResult;


import junit.framework.TestCase;

/**
 * Test of EquipmentTaskResult task.
 * @author xadazim
 *
 */
public class EquipmentTaskResultTest extends TestCase {

    /**
     * Test of constructor and all the getters.
     */
    @Test
    public void testEquipmentTaskResult() {
        EquipmentTaskResult etr = new EquipmentTaskResult(1, "message", Integer.valueOf(2), true);
        assertEquals(1, etr.getResultCode());
        assertEquals("message", etr.getResultMessage());
        assertEquals(Integer.valueOf(2), (Integer)etr.getResultObject());
        assertTrue(etr.isFatal());

        etr = new EquipmentTaskResult(1, "message", Integer.valueOf(2), false);
        assertFalse(etr.isFatal());
    }
}
