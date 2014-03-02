package com.stericsson.sdk.equipment.u5500.warm.test;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.u5500.warm.internal.U5500WarmEquipment;
import com.stericsson.sdk.equipment.ux500.warm.tasks.UX500WarmEquipmentShutdownTask;

/**
 * @author eolabor
 * 
 */
public class U5500WarmEquipmentShutdownTaskTest extends TestCase {

    UX500WarmEquipmentShutdownTask shutdownTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 1;

    static final String RESULT_MSG = " completed.";

    private U5500WarmEquipment eq;

    /**
     * Init
     */
    @Before
    public void init() {
        eq = new U5500WarmEquipment(Activator.getPort(), null);
        shutdownTask = new UX500WarmEquipmentShutdownTask(eq);
    }

    /**
     * test shut down
     */
    @Test
    public void testU5500ShutDownSuccess() {
        EquipmentTaskResult result = shutdownTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(shutdownTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(shutdownTask.isCancelable());

        assertEquals("SHUTDOWN_WARM", shutdownTask.getId());

        assertEquals(shutdownTask.getId() + "@" + eq.toString(), shutdownTask.toString());

        assertFalse(result.isFatal());

    }

    /**
     * Testcase
     */
    @Test
    public void testU5500ShutDownFails() {
        Activator.setThrowException(true);
        EquipmentTaskResult result = shutdownTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(shutdownTask.getId()) + RESULT_MSG, result.getResultMessage());
    }
}
