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
public class M7X00EquipmentCreateDirectoryTaskTest extends TestCase {

    private M7x00Equipment equipment;

    private M7X00EquipmentCreateDirectoryTask task;

    /**
     *
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentCreateDirectoryTask(equipment);
    }

    /**
     * Test Constructor;
     */
    @Test
    public void testM7X00EquipmentCreateDirectoryTask() {
        assertEquals(LCDefinitions.METHOD_NAME_FS_CREATE_DIRECTORY, task.getMethodName());
    }

}
