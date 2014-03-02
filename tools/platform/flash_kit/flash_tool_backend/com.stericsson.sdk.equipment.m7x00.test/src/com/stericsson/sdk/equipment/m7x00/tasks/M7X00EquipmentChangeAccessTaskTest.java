/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import static org.junit.Assert.assertArrayEquals;
import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class M7X00EquipmentChangeAccessTaskTest extends TestCase {

    M7X00EquipmentChangeAccessTask task;

    private M7x00Equipment equipment;

    /**
     * 
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        task = new M7X00EquipmentChangeAccessTask(equipment);
    }

    /**
     * Test of setArguments.
     */
    @Test
    public void testSetArguments() {
        task.setArguments(new String[] {
            null, null,"test", "13"});
        assertArrayEquals(new Object[] {
            "test", 13}, task.getArguments());
    }
}
