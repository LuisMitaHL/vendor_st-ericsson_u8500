/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLCEquipmentTask;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.types.VolumePropertiesType;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class M7X00EquipmentVolumePropertiesTaskTest extends TestCase {

    M7X00EquipmentVolumePropertiesTask task;

    private M7x00Equipment equipment;

    /**
     * 
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentVolumePropertiesTask(equipment);
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResult() {
        String type = "type";
        int size = 0;
        int free = 0;
        VolumePropertiesType resultObj = new VolumePropertiesType(0, type, type.length(), size, free);
        task.updateResult(resultObj);
        int resultCode = task.getResultCode();
        assertEquals(AbstractLCEquipmentTask.ERROR_NONE, resultCode);
        String resultMessage = task.getResultMessage();
        assertNotNull(resultMessage);
        assertTrue(resultMessage.length() > 0);
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResultError() {
        String type = "type";
        int size = 0;
        int free = 0;
        VolumePropertiesType resultObj =
            new VolumePropertiesType(AbstractLCEquipmentTask.ERROR, type, type.length(), size, free);
        task.updateResult(resultObj);
        String expectedMessage =
            equipment.getLoaderCommunicationService().getLoaderErrorDesc(AbstractLCEquipmentTask.ERROR);
        assertEquals(expectedMessage, task.getResultMessage());
        assertEquals(AbstractLCEquipmentTask.ERROR, task.getResultCode());
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
