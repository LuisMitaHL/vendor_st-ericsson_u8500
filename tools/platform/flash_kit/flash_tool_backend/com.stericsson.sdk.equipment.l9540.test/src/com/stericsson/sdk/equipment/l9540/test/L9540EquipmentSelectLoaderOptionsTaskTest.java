package com.stericsson.sdk.equipment.l9540.test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.l9540.internal.L9540Equipment;
import com.stericsson.sdk.equipment.tasks.EquipmentSelectLoaderOptionsTask;

import junit.framework.TestCase;

/**
 * @author mbocek01
 * 
 */
public class L9540EquipmentSelectLoaderOptionsTaskTest extends TestCase {

    private static String[] pArguments = new String[] {
        "FLASH_SELECT_LOADER_OPTIONS", "<equipment-id>", "0", "1"};


    static final int RESULT_SUCCESS = 1234;

    static final int RESULT_FAIL = -1;

    /** */
    public void testWriteRpmbTask() {
        L9540Equipment equipment = new L9540Equipment(Activator.getPort(), null);
        EquipmentSelectLoaderOptionsTask equipmentSelectLoaderOptionsTask = new EquipmentSelectLoaderOptionsTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        equipmentSelectLoaderOptionsTask.setArguments(pArguments);
        EquipmentTaskResult result = equipmentSelectLoaderOptionsTask.execute();

        assertNotNull(result);
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertNull(result.getResultObject());
        assertFalse(equipmentSelectLoaderOptionsTask.isCancelable());
        assertEquals(CommandName.FLASH_SELECT_LOADER_OPTIONS.name(), equipmentSelectLoaderOptionsTask.getId());
        assertEquals(equipmentSelectLoaderOptionsTask.getId() + "@" + equipment.toString(), 
            equipmentSelectLoaderOptionsTask.toString());
        assertFalse(result.isFatal());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = equipmentSelectLoaderOptionsTask.execute();
        assertNotNull(result);
        assertEquals(RESULT_FAIL, result.getResultCode());
    }

}
