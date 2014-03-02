package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentEraseGlobalDataSetTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xmilhan
 * 
 */
public class U5500EquipmentEraseGlobalDataSetTaskTest extends TestCase {

    EquipmentEraseGlobalDataSetTask eraseGlobalDataSetTask = null;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String[] ARGUMENTS = new String[] {
        "PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET", "test", "/flash0/GDFS"};

    static final String[] ARGUMENTS_BULK = new String[] {
        "PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET", "test", "/flash0/GDFS"};

    /**
     * test process file
     */
    public void testU5500EraseGlobalDataSet() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        eraseGlobalDataSetTask = new EquipmentEraseGlobalDataSetTask(eq);

        eraseGlobalDataSetTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = eraseGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(eraseGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        eraseGlobalDataSetTask.setArguments(ARGUMENTS_BULK);
        result = eraseGlobalDataSetTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(eraseGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        eraseGlobalDataSetTask.setArguments(ARGUMENTS);
        result = eraseGlobalDataSetTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(eraseGlobalDataSetTask.getId()) + RESULT_MSG, result
            .getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(eraseGlobalDataSetTask.isCancelable());

        assertEquals(CommandName.PARAMETER_STORAGE_ERASE_GLOBAL_DATA_SET.name(), eraseGlobalDataSetTask.getId());

        assertEquals(eraseGlobalDataSetTask.getId() + "@" + eq.toString(), eraseGlobalDataSetTask.toString());

        assertFalse(result.isFatal());

    }

}
