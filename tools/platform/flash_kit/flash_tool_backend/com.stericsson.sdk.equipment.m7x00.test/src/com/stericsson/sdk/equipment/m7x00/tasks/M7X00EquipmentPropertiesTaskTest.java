/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import java.sql.Time;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.types.FSPropertiesType;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class M7X00EquipmentPropertiesTaskTest extends TestCase {

    private static final int RESULT_ERROR = 1;

    M7X00EquipmentPropertiesTask task;

    private M7x00Equipment equipment;

    /**
     * 
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentPropertiesTask(equipment);
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResult() {
        int mode = 0xC000;
        int size = 1;
        int modificationTime = 2;
        int accessTime = 3;
        int createTime = 4;
        FSPropertiesType resultObj = new FSPropertiesType(0, mode, size, modificationTime, accessTime, createTime);
        StringBuffer result = new StringBuffer();
        result.append("Type:     ");
        result.append("Unix domain socket");
        result.append("\nSize:     ");
        result.append(size + " B");
        result.append("\nCreated:  ");
        result.append(new Time(modificationTime));
        result.append("\nModified: ");
        result.append(new Time(accessTime));
        result.append("\nAccessed: ");
        result.append(new Time(createTime));
        String expectedMessage = result.toString();
        task.updateResult(resultObj);
        assertEquals(expectedMessage, task.getResultMessage());
        assertEquals(0, task.getResultCode());
    }

    /**
     * Test of method updateResult.
     */
    @Test
    public void testUpdateResultError() {
        int mode = 0xC000;
        int size = 1;
        int modificationTime = 2;
        int accessTime = 3;
        int createTime = 4;
        FSPropertiesType resultObj =
            new FSPropertiesType(RESULT_ERROR, mode, size, modificationTime, accessTime, createTime);
        String expectedMessage = equipment.getLoaderCommunicationService().getLoaderErrorDesc(RESULT_ERROR);
        task.updateResult(resultObj);
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
