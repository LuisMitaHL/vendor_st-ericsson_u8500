package com.stericsson.sdk.equipment.u8500.test;

import java.io.File;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentProcessFileTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xdancho
 * 
 */
public class U8500EquipmentProcessFileTaskTest extends TestCase {

    EquipmentProcessFileTask processFileTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String FILE_PATH =
        System.getProperty("java.io.tmpdir") + System.getProperty(File.separator) + "flashArchive.zip";

    static final String[] ARGUMENTS = new String[] {
        "FLASH_PROCESS_FILE", "<equipment-id>", FILE_PATH};

    /**
     * test process file
     */
    public void testU8500ProcessFile() {

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        processFileTask = new EquipmentProcessFileTask(eq);

        processFileTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = processFileTask.execute();

        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(processFileTask.getId()) + RESULT_MSG, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        processFileTask.setArguments(ARGUMENTS);
        result = processFileTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(processFileTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertTrue(processFileTask.isCancelable());

        assertEquals(CommandName.FLASH_PROCESS_FILE.name(), processFileTask.getId());

        assertEquals(CommandName.FLASH_PROCESS_FILE.name() + "@" + eq.toString(), processFileTask.toString());

        assertFalse(result.isFatal());

    }

}
