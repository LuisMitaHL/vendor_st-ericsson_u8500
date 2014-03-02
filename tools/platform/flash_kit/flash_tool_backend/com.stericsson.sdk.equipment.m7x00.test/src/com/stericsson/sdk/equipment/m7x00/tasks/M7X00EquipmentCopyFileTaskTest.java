/**
 * 
 */
package com.stericsson.sdk.equipment.m7x00.tasks;

import static org.junit.Assert.assertArrayEquals;

import java.io.File;
import java.io.IOException;

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
public class M7X00EquipmentCopyFileTaskTest extends TestCase {

    M7X00EquipmentCopyFileTask task;

    private M7x00Equipment equipment;

    private static final String SOURCEPATH_WIN = "C:\\work\\file";

    private static final String SOURCEPATH_LINUX = "/work/file";

    private static final String DESTINATIONPATH = "/flash0/";

    private static final String PCPREFIX = "PC:";

    private static final String DEVICEPREFIX = "";

    private static final int REGULAR_FILE = 0x8000; // 0100000;

    ListDirectoryType listDirectoryResult;

    /**
     * 
     */
    @Before
    public void setUp() {

        listDirectoryResult = new ListDirectoryType(0, 1);
        listDirectoryResult.addDirectory("some", 4, 100, REGULAR_FILE, 11);
 
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);

        task = new M7X00EquipmentCopyFileTask(equipment);
        TestFragmentActivator.getLoaderCommunicationMock().setResult(listDirectoryResult);

    }

    /**
     * Test set copy from device to computer.
     */
    @Test
    public void testSetArgumentsDevice2PcWin() {
        task.setArguments(new String[] {
            null, null, DEVICEPREFIX + SOURCEPATH_WIN, PCPREFIX + DESTINATIONPATH});
        Object[] expecteds = new Object[] {
            SOURCEPATH_WIN, DESTINATIONPATH, false, true};
        assertArrayEquals(expecteds, task.getArguments());
    }

    /**
     * Test set copy from device to computer with linux path format.
     */
    @Test
    public void testSetArgumentsDevice2PcLIN() {
        task.setArguments(new String[] {
            null, null, DEVICEPREFIX + SOURCEPATH_LINUX, PCPREFIX + DESTINATIONPATH});
        Object[] expecteds = new Object[] {
            SOURCEPATH_LINUX, DESTINATIONPATH, false, true};
        assertArrayEquals(expecteds, task.getArguments());
    }

    /**
     * Test set copy from device to computer with file separator on the end.
     */
    @Test
    public void testSetArgumentsDeviceSeparatorOnEnd() {
        task.setArguments(new String[] {
            null, null, DEVICEPREFIX + SOURCEPATH_LINUX, PCPREFIX + DESTINATIONPATH + "/"});
        Object[] expecteds = new Object[] {
            SOURCEPATH_LINUX, DESTINATIONPATH + "/", false, true};
        assertArrayEquals(expecteds, task.getArguments());
    }

    /**
     * Test set copy from device to computer with file separator on the end.
     */
    @Test
    public void testExistingFile() {
        File myFile;
        try {
            myFile = File.createTempFile("some", "file");

            myFile.deleteOnExit();
            task.setArguments(new String[] {
                null, null, PCPREFIX + myFile.getAbsolutePath(), PCPREFIX + myFile.getParent()});
            Object[] expecteds = new Object[] {
                myFile.getAbsolutePath(), myFile.getAbsolutePath(), true, true};
            assertArrayEquals(expecteds, task.getArguments());
        } catch (IOException e) {
            assertFalse(true);
            e.printStackTrace();
        }
    }

    /**
     *Test set copy from computer to device.
     */
    @Test
    public void testSetArgumentsPC2Device() {
        task.setArguments(new String[] {
            null, null, PCPREFIX + SOURCEPATH_WIN, DEVICEPREFIX + DESTINATIONPATH});
        Object[] expecteds = new Object[] {
            SOURCEPATH_WIN, DESTINATIONPATH+"file", true, false};
        assertArrayEquals(expecteds, task.getArguments());
    }
}
