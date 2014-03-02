package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteGlobalDataUnitTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xdancho
 * 
 */
public class U5500EquipmentWriteGlobalDataUnitTaskTest extends TestCase {

    EquipmentWriteGlobalDataUnitTask writeGlobalDataUnitTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String DATA = "CAFEBABE";

    static final String[] ARGUMENTS = new String[] {
        CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), "test", "/flash0/GDFS", "1", DATA};

    /**
     * test send raw data
     */
    public void testU5500WriteGlobalDataUnit() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        writeGlobalDataUnitTask = new EquipmentWriteGlobalDataUnitTask(eq);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        writeGlobalDataUnitTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = writeGlobalDataUnitTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(writeGlobalDataUnitTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = writeGlobalDataUnitTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(writeGlobalDataUnitTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(writeGlobalDataUnitTask.isCancelable());

        assertEquals(CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), writeGlobalDataUnitTask.getId());

        assertEquals(writeGlobalDataUnitTask.getId() + "@" + eq.toString(), writeGlobalDataUnitTask.toString());

        assertFalse(result.isFatal());

    }

}
