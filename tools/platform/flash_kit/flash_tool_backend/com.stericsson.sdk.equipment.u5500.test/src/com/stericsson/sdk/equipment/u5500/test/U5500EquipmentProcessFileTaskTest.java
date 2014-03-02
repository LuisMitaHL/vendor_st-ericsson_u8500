package com.stericsson.sdk.equipment.u5500.test;

import java.io.File;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentProcessFileTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xdancho
 * 
 */
public class U5500EquipmentProcessFileTaskTest extends TestCase {

    EquipmentProcessFileTask processFileTask = null;


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
    public void testU5500ProcessFile() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
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

        assertEquals(processFileTask.getId() + "@" + eq.toString(), processFileTask.toString());

        assertFalse(result.isFatal());

    }

}
