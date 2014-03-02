package com.stericsson.sdk.equipment.m7x00.tasks;

import java.io.File;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;

import junit.framework.TestCase;

/**
 * @author xtomzap
 *
 */
public class M7X00EquipmentCheckCompatibilityTaskTest extends TestCase {

    private M7x00Equipment equipment;
    private M7X00EquipmentCheckCompatibilityTask  task;
    private static final int EXPECTEDRESULT = 0;
    private static final int EXPECTEDFAILRESULT = -1;
    private static final String LOAD_MODULE = "test_load_module.elf";

    /**
     * 
     * {@inheritDoc}
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentCheckCompatibilityTask(equipment);
    }

    /***/
    @After
    public void tearDown() {
        equipment = null;
    }

    /***/
    @Test
    public void testM7X00EquipmentCheckCompatibilityTaskTest() {
        File loadModule;
        try {
            loadModule = new File(TestFragmentActivator.getResourcesPath(), LOAD_MODULE);
            task.setArguments(new String[]{"a", "b", loadModule.getAbsolutePath()});
            EquipmentTaskResult result = task.execute();
            assertEquals(EXPECTEDRESULT, result.getResultCode());
        } catch (Exception e) {
            fail();
        }
    }

    /***/
    @Test
    public void testNullM7X00EquipmentCheckCompatibilityTaskTest() {
        try {
            task.setArguments(null);
            EquipmentTaskResult result = task.execute();
            assertEquals(EXPECTEDFAILRESULT, result.getResultCode());
        } catch (Exception e) {
            fail();
        }
    }

    /***/
    @Test
    public void testWrongNumberOfArgumentsM7X00EquipmentCheckCompatibilityTaskTest() {
        try {
            task.setArguments(new String[]{"a", "b"});
            EquipmentTaskResult result = task.execute();
            assertEquals(EXPECTEDFAILRESULT, result.getResultCode());
        } catch (Exception e) {
            fail();
        }
    }

    /** */
    @Test
    public void testGetId() {
        assertEquals(M7X00EquipmentCheckCompatibilityTask.TASK_ID, task.getId());
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
