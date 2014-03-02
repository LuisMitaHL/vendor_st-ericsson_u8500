package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentReadGlobalDataUnitTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;
import com.stericsson.sdk.loader.communication.types.PSReadGlobalDataUnitType;

/**
 * @author xdancho
 * 
 */
public class U5500EquipmentReadGlobalDataUnitTaskTest extends TestCase {

    EquipmentReadGlobalDataUnitTask readGlobalDataUnitTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = "No error";

    static final byte[] DATA = HexUtilities.toByteArray("CAFEBABE");

    static final String RETURN_DATA = "CAFEBABE";

    static final String[] ARGUMENTS = new String[] {
        CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(), "test", "/flash0/GDFS", "1"};

    /**
     * test send raw data
     */
    public void testU5500ReadGlobalDataUnit() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        readGlobalDataUnitTask = new EquipmentReadGlobalDataUnitTask(eq);

        Activator.getLoaderCommunicationMock().setResult(new PSReadGlobalDataUnitType(RESULT_SUCCESS, DATA));
        readGlobalDataUnitTask.setArguments(ARGUMENTS);
        EquipmentTaskResult result = readGlobalDataUnitTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(RETURN_DATA, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(new PSReadGlobalDataUnitType(RESULT_FAIL, DATA));
        result = readGlobalDataUnitTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(RETURN_DATA, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(readGlobalDataUnitTask.isCancelable());

        assertEquals(CommandName.PARAMETER_STORAGE_READ_GLOBAL_DATA_UNIT.name(), readGlobalDataUnitTask.getId());

        assertEquals(readGlobalDataUnitTask.getId() + "@" + eq.toString(), readGlobalDataUnitTask.toString());

        assertFalse(result.isFatal());

    }

}
