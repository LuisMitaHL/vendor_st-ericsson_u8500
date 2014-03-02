package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentShutdownTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xdancho
 *
 */
public class U5500EquipmentShutdownTaskTest extends TestCase {

    EquipmentShutdownTask shutdownTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    /**
     * test shut down
     */
    public void testU5500ShutDown() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
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
