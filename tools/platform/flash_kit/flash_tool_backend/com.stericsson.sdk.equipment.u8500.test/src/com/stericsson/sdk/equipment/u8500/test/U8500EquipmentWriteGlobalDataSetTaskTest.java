package com.stericsson.sdk.equipment.u8500.test;

import java.io.File;
import java.io.IOException;
import java.net.URL;

import junit.framework.TestCase;

import org.eclipse.core.runtime.FileLocator;
import org.junit.Before;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteGlobalDataSetTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xmilhan
 * 
 */
public class U8500EquipmentWriteGlobalDataSetTaskTest extends TestCase {

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final int RESULT_EXCEPTION = 1;

    static final String RESULT_MSG = " completed.";

    static final String SOURCE_PATH = "/res/GDFS.gdf";

    static final String NONEXISTING_FILE__MESSAGE = "Given path : NONEXISTINGFILE does not exist or it is not a file!";

    String[] argumenents;

    String[] bulkArguments;

    File file;

    long lengthFile;

    EquipmentWriteGlobalDataSetTask writeGlobalDataSetTask = null;

    EquipmentTaskResult result;

    U8500Equipment eq;

    /**
     * Sets up test environment
     */
    @Before
    public void setUp() {
        file = new File(findResourcePath());
        lengthFile = file.length();
        argumenents =
            new String[] {
                "PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET", "/flash0/GDFS", String.valueOf(lengthFile),
                "PC" + file.getAbsolutePath()};
        bulkArguments =
            new String[] {
                "PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET", "/flash0/GDFS", String.valueOf(lengthFile),
                "ME" + file.getAbsolutePath()};
        eq = new U8500Equipment(Activator.getPort(), null);
        writeGlobalDataSetTask = new EquipmentWriteGlobalDataSetTask(eq);
    }

    private String findResourcePath() {
        URL url = getClass().getResource(SOURCE_PATH);
        URL find = null;

        try {
            find = FileLocator.toFileURL(url);
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Cannot find " + SOURCE_PATH);
        }

        return find.getFile();
    }

    /**
     * Test of standard PC arguments with correct path
     */
    public void testPCArguments() {
        writeGlobalDataSetTask.setArguments(argumenents);
        result = writeGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(writeGlobalDataSetTask.getId()) + RESULT_MSG, result.getResultMessage());
    }

    /**
     * Test of bulk arguments (file on ME)
     */
    public void testMEArguments() {
        writeGlobalDataSetTask.setArguments(bulkArguments);
        result = writeGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(writeGlobalDataSetTask.getId()) + RESULT_MSG, result.getResultMessage());
    }

    /**
     * Test of standard arguments with wrong path
     */
    public void testArgumentsNonExistingFile() {
        argumenents =
            new String[] {
                "PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET", "/flash0/GDFS", String.valueOf(lengthFile),
                "PC" + "NONEXISTINGFILE"};

        writeGlobalDataSetTask.setArguments(argumenents);
        result = writeGlobalDataSetTask.execute();
        assertEquals(RESULT_EXCEPTION, result.getResultCode());
        assertNotSame(NONEXISTING_FILE__MESSAGE, result.getResultMessage());
    }

    /**
     * Test of failed communication
     */
    public void testwriteGlobalDataSetFailed() {
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        writeGlobalDataSetTask.setArguments(argumenents);
        result = writeGlobalDataSetTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(writeGlobalDataSetTask.getId()) + RESULT_MSG, result.getResultMessage());
    }

    /**
     * Test of ResultObject
     */
    public void getResultObject() {
        assertNull(result.getResultObject());
    }

    /**
     * Test isCancelable
     */
    public void testisCancelable() {
        assertFalse(writeGlobalDataSetTask.isCancelable());
    }

    /**
     * Test getID
     */
    public void testGetId() {
        assertEquals(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(), writeGlobalDataSetTask.getId());
    }

    /**
     * Test of String value
     */
    public void testToString() {
        assertEquals(writeGlobalDataSetTask.getId() + "@" + eq.toString(), writeGlobalDataSetTask.toString());
    }

    /**
     * Test isFatal
     */
    public void testIsFatal() {
        testwriteGlobalDataSetFailed();
        assertFalse(result.isFatal());
    }

}
