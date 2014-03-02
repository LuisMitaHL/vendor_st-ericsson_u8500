package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentEraseAreaTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * @author xdancho
 *
 */
public class U5500EquipmentEraseAreaTaskTest extends TestCase {

    EquipmentEraseAreaTask eraseAreaTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String[] ARGUMENTS_HEX = new String[] {
        "FLASH_ERASE_AREA", "<equipment-id>", "/flash0/", "0x0", "0x1000"};

    static final String[] ARGUMENTS_DEC = new String[] {
        "FLASH_ERASE_AREA", "<equipment-id>", "/flash0/", "0", "1000"};

    /**
     * test erase area
     */
    public void testU5500EraseArea() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        eraseAreaTask = new EquipmentEraseAreaTask(eq);

        eraseAreaTask.setArguments(ARGUMENTS_HEX);
        EquipmentTaskResult result = eraseAreaTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(eraseAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        eraseAreaTask.setArguments(ARGUMENTS_HEX);
        result = eraseAreaTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(eraseAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(eraseAreaTask.isCancelable());

        assertEquals(CommandName.FLASH_ERASE_AREA.name(), eraseAreaTask.getId());

        assertEquals(CommandName.FLASH_ERASE_AREA.name() + "@" + eq.toString(), eraseAreaTask.toString());

        assertFalse(result.isFatal());

    }

}
