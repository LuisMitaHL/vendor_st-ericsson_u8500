package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentShutdownTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xdancho
 *
 */
public class U8500EquipmentShutdownTaskTest extends TestCase {

    EquipmentShutdownTask shutdownTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    /**
     * test shut down
     */
    public void testU8500ShutDown() {

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        shutdownTask = new EquipmentShutdownTask(eq);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = shutdownTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(shutdownTask.getId()) + RESULT_MSG, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = shutdownTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(shutdownTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(shutdownTask.isCancelable());

        assertEquals(CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), shutdownTask.getId());

        assertEquals(shutdownTask.getId() + "@" + eq.toString(), shutdownTask.toString());

        assertFalse(result.isFatal());

    }

}
