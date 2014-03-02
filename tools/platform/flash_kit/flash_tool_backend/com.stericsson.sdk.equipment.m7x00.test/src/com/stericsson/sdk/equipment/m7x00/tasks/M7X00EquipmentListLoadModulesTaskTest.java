package com.stericsson.sdk.equipment.m7x00.tasks;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;

/**
 * @author xtomzap
 *
 */
public class M7X00EquipmentListLoadModulesTaskTest extends TestCase {

    private M7x00Equipment equipment;
    private M7X00EquipmentListLoadModulesTask task;
    private static final int EXPECTEDRESULT = 0;

    /**
     * 
     * {@inheritDoc}
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentListLoadModulesTask(equipment);
    }

    /***/
    @After
    public void tearDown() {
        equipment = null;
    }

    /**
     * Test Constructor;
     */
    @Test
    public void testM7X00EquipmentListLoadModulesTast() {
        EquipmentTaskResult result = task.execute();
        assertEquals(EXPECTEDRESULT, result.getResultCode());
    }

    /** */
    @Test
    public void testGetId() {
        assertEquals(M7X00EquipmentListLoadModulesTask.TASK_ID, task.getId());
    }

    /** */
    @Test
    public void testIsCancelable() {
        assertEquals(false, task.isCancelable());
    }

    /** */
    @Test
    public void testCancel() {
        try {
            task.cancel();
            fail();
        } catch (RuntimeException e) {
            assertTrue(true);
        }
    }

    /***/
    @Test
    public void testSetArguments() {
        task.setArguments(null);
    }
}
