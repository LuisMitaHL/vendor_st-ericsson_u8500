package com.stericsson.sdk.equipment.u8500.warm.test;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.u8500.warm.internal.U8500WarmEquipment;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentListFileTask;

/**
 * @author esrimpa
 * 
 */
public class U8500WarmEquipmentListFileTaskTest extends TestCase {

    UX500WarmEquipmentListFileTask listFileTask = null;

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
        listFileTask = new UX500WarmEquipmentListFileTask(eq);
    }

    /**
     * TestCase
     */
    @Test
    public void testU8500ListFileSuccess() {
        Activator.setThrowException(false);

        EquipmentTaskResult result = listFileTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals("CoreDump1|Not Downloaded\nCoreDump2|Not Downloaded\n", result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(listFileTask.isCancelable());

        assertEquals(CommandName.COREDUMP_LIST_FILE.name(), listFileTask.getId());

        assertEquals(listFileTask.getId() + "@" + eq.toString(), listFileTask.toString());

        assertFalse(result.isFatal());

    }

    /**
     * TestCase
     */
    @Test
    public void testU8500ListFilesFails() {
        Activator.setThrowException(true);
        EquipmentTaskResult result = listFileTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(listFileTask.getId()) + RESULT_MSG, result.getResultMessage());
    }
}
