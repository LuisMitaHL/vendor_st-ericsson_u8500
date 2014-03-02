/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 *
 */
public class M7X00EquipmentMoveFileTaskTest extends TestCase {

    private M7x00Equipment equipment;
    private M7X00EquipmentMoveFileTask task;

    /**
     *
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentMoveFileTask(equipment);
    }

    /**
     * Test Constructor;
     */
    @Test
    public void testM7X00EquipmentCreateDirectoryTask() {
        assertEquals(LCDefinitions.METHOD_NAME_FS_MOVE_FILE, task.getMethodName());
    }

}
