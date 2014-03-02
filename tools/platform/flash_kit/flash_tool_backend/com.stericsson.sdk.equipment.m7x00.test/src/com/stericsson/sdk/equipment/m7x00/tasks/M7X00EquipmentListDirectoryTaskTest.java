/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.types.ListDirectoryType;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class M7X00EquipmentListDirectoryTaskTest extends TestCase {

    /** */
    private static final int RESULT_ERROR = 1;

    private M7X00EquipmentListDirectoryTask task;

    private M7x00Equipment equipment;

    /**
     * 
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentListDirectoryTask(equipment);
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResult() {
        String[] dirs = new String[] {
            "a", "b", "c"};
        StringBuilder expectedMessage = new StringBuilder();
        ListDirectoryType resultObj = new ListDirectoryType(0, dirs.length);
        for (String dir : dirs) {
            resultObj.addDirectory(dir, dir.length(), 0, 0x4000, 0);
            expectedMessage.append("\n").append("Dir\t---------\t0\tThu Jan 01 01:00:00 CET 1970\t").append(dir);
        }
        task.updateResult(resultObj);
        assertEquals(expectedMessage.toString(), task.getResultMessage());
        assertEquals(0, task.getResultCode());
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResultError() {
        String[] dirs = new String[] {
            "a", "b", "c"};
        ListDirectoryType resultObj = new ListDirectoryType(RESULT_ERROR, dirs.length);
        for (String dir : dirs) {
            resultObj.addDirectory(dir, dir.length(), 0, 0, 0);
        }
        task.updateResult(resultObj);
        String expectedMessage = equipment.getLoaderCommunicationService().getLoaderErrorDesc(RESULT_ERROR);
        assertEquals(expectedMessage, task.getResultMessage());
        assertEquals(RESULT_ERROR, task.getResultCode());
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResultInteger() {
        int expected = 56;
        task.updateResult(expected);
        assertEquals(equipment.getLoaderCommunicationService().getLoaderErrorDesc(expected), task.getResultMessage());
        assertEquals(expected, task.getResultCode());
    }
}
