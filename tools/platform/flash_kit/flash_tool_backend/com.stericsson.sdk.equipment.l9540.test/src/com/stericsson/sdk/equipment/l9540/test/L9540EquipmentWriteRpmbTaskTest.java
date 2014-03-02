package com.stericsson.sdk.equipment.l9540.test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.l9540.internal.L9540Equipment;
import com.stericsson.sdk.equipment.tasks.EquipmentWriteRpmbKeyTask;

import junit.framework.TestCase;

/**
 * @author mbocek01
 * 
 */
public class L9540EquipmentWriteRpmbTaskTest extends TestCase {

    static final int RESULT_SUCCESS = 1234;

    static final int RESULT_FAIL = -1;

    /** */
    public void testWriteRpmbTask() {
        L9540Equipment equipment = new L9540Equipment(Activator.getPort(), null);
        EquipmentWriteRpmbKeyTask equipmentWriteRpmbTask = new EquipmentWriteRpmbKeyTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = equipmentWriteRpmbTask.execute();

        assertNotNull(result);
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertNull(result.getResultObject());
        assertFalse(equipmentWriteRpmbTask.isCancelable());
        assertEquals(CommandName.SECURITY_WRITE_RPMB_KEY.name(), equipmentWriteRpmbTask.getId());
        assertEquals(equipmentWriteRpmbTask.getId() + "@" + equipment.toString(), equipmentWriteRpmbTask.toString());
        assertFalse(result.isFatal());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = equipmentWriteRpmbTask.execute();
        assertNotNull(result);
        assertEquals(RESULT_FAIL, result.getResultCode());
    }

}
