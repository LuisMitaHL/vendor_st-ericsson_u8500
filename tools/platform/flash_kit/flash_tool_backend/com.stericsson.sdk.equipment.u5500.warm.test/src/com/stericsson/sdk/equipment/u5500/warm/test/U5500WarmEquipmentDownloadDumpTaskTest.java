package com.stericsson.sdk.equipment.u5500.warm.test;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.u5500.warm.internal.U5500WarmEquipment;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentDownloadDumpTask;

/**
 * @author eolabor
 * 
 */
public class U5500WarmEquipmentDownloadDumpTaskTest extends TestCase {

    UX500WarmEquipmentDownloadDumpTask downloadDumpTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 1;

    static final String RESULT_MSG = " completed.";

    private U5500WarmEquipment eq;

    /**
     * Testcase
     */
    @Before
    public void init() {
        eq = new U5500WarmEquipment(Activator.getPort(), null);
        downloadDumpTask = new UX500WarmEquipmentDownloadDumpTask(eq);
    }

    /**
     * test shut down
     */
    @Test
    public void testU5500ownloadDumpTaskSuccess() {
        Activator.setThrowException(false);

        String[] args = new String[] {
            "arg0", "arg1", "CoreDump1", "Path", "false"};
        downloadDumpTask.setArguments(args);
        EquipmentTaskResult result = downloadDumpTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(downloadDumpTask.getId()) + RESULT_MSG,
            result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(downloadDumpTask.isCancelable());

        assertEquals(CommandName.COREDUMP_DOWNLOAD_DUMP.name(), downloadDumpTask.getId());

        assertEquals(downloadDumpTask.getId() + "@" + eq.toString(), downloadDumpTask.toString());

        assertFalse(result.isFatal());

    }

    /**
     * Testcase
     */
    @Test
    public void testU5500ownloadDumpTaskFails() {
        Activator.setThrowException(true);
        EquipmentTaskResult result = downloadDumpTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(downloadDumpTask.getId()) + RESULT_MSG,
            result.getResultMessage());
    }

    /**
     * Testcase
     */
    @Test
    public void testsetArgumentsFails() {
        String expectedMessage = "Missing arguments. Probably some of the arguments are not provided is not provided.";
        downloadDumpTask.setArguments(new String[] {
            "arg1", "arg2"});
        EquipmentTaskResult result = downloadDumpTask.execute();
        assertEquals(expectedMessage, result.getResultMessage());
    }
}
