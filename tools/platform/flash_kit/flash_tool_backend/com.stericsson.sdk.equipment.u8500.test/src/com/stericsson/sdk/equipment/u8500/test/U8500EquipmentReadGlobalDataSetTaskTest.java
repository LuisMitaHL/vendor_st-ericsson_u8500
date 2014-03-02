package com.stericsson.sdk.equipment.u8500.test;

import java.io.File;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentReadGlobalDataSetTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xmilhan
 * 
 */
public class U8500EquipmentReadGlobalDataSetTaskTest extends TestCase {

    EquipmentReadGlobalDataSetTask readGlobalDataSetTask = null;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String SOURCE_PATH =
        System.getProperty("java.io.tmpdir") + System.getProperty("file.separator") + "flashArchive.zip";

    static File file = new File(SOURCE_PATH);

    static long lengthFile = file.length();

    static final String[] ARGUMENTS = new String[] {
        "PARAMETER_STORAGE_READ_GLOBAL_DATA_SET", "test", "/flash0/GDFS", "PC" + SOURCE_PATH};

    static final String[] ARGUMENTS_BULK = new String[] {
        "PARAMETER_STORAGE_READ_GLOBAL_DATA_SET", "test", "/flash0/GDFS", "ME" + SOURCE_PATH};

    /**
     * test process file
     */
    public void testU8500ReadGlobalDataSet() {

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        readGlobalDataSetTask = new EquipmentReadGlobalDataSetTask(eq);

        readGlobalDataSetTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = readGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(readGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        readGlobalDataSetTask.setArguments(ARGUMENTS_BULK);
        result = readGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(readGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        readGlobalDataSetTask.setArguments(ARGUMENTS);
        result = readGlobalDataSetTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(readGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(readGlobalDataSetTask.isCancelable());

        assertEquals(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_SET.name(), readGlobalDataSetTask.getId());

        assertEquals(readGlobalDataSetTask.getId() + "@" + eq.toString(), readGlobalDataSetTask.toString());

        assertFalse(result.isFatal());

    }

}
