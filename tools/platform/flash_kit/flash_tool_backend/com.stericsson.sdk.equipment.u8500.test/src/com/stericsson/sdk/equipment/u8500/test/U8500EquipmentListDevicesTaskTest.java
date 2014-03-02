package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentListDevicesTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;

/**
 * @author zapletom
 * 
 */
public class U8500EquipmentListDevicesTaskTest extends TestCase {

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = -1;

    static final int NUMBER_OF_DEVICES = 0;

    /** */
    public void testListDevicesTask() {
        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), null);
        EquipmentListDevicesTask equipmentListDevicesTask = new EquipmentListDevicesTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(new ListDevicesType(RESULT_SUCCESS, NUMBER_OF_DEVICES));
        EquipmentTaskResult result = equipmentListDevicesTask.execute();

        assertNotNull(result);
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertNull(result.getResultObject());
        assertFalse(equipmentListDevicesTask.isCancelable());
        assertEquals(CommandName.FLASH_LIST_DEVICES.name(), equipmentListDevicesTask.getId());
        assertEquals(equipmentListDevicesTask.getId() + "@" + equipment.toString(), equipmentListDevicesTask.toString());
        assertFalse(result.isFatal());

        Activator.getLoaderCommunicationMock().setResult(new ListDevicesType(RESULT_FAIL, NUMBER_OF_DEVICES));
        result = equipmentListDevicesTask.execute();
        assertNotNull(result);
        assertEquals(RESULT_FAIL, result.getResultCode());
    }
}
