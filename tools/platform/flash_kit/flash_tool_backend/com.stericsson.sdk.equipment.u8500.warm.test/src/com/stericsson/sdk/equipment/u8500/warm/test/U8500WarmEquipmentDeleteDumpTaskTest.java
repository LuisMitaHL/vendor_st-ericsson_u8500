package com.stericsson.sdk.equipment.u8500.warm.test;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.u8500.warm.internal.U8500WarmEquipment;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentDeleteDumpTask;

/**
 * @author esrimpa
 * 
 */
public class U8500WarmEquipmentDeleteDumpTaskTest extends TestCase {

    UX500WarmEquipmentDeleteDumpTask deleteDumpTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 1;

    static final String RESULT_MSG = " completed.";

    private U8500WarmEquipment eq;

    /**
     * Init
     */
    @Before
    public void init() {
        eq = new U8500WarmEquipment(Activator.getPort(), null);
        deleteDumpTask = new UX500WarmEquipmentDeleteDumpTask(eq);
    }

    /**
     * test shut down
     */
    @Test
    public void testU8500DeleteDumpTaskSuccess() {
        Activator.setThrowException(false);

        String[] args = new String[] {
            "arg0", "arg1", "CoreDump1"};
        deleteDumpTask.setArguments(args);
        EquipmentTaskResult result = deleteDumpTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(deleteDumpTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(deleteDumpTask.isCancelable());

        assertEquals(CommandName.COREDUMP_DELETE_DUMP.name(), deleteDumpTask.getId());

        assertEquals(deleteDumpTask.getId() + "@" + eq.toString(), deleteDumpTask.toString());

        assertFalse(result.isFatal());

    }

    /**
     * Testcase
     */
    @Test
    public void testU8500DeleteDumpTaskFails() {
        Activator.setThrowException(true);
        EquipmentTaskResult result = deleteDumpTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(deleteDumpTask.getId()) + RESULT_MSG,  result.getResultMessage());
    }

    /**
     * Testcase
     */
    @Test
    public void testsetArgumentsFails() {
        String expectedMessage = "Missing arguments. Probably dump file name is not provided.";
        deleteDumpTask.setArguments(new String[] {
            "arg1", "arg2"});
        EquipmentTaskResult result = deleteDumpTask.execute();
        assertEquals(expectedMessage, result.getResultMessage());
    }
}
