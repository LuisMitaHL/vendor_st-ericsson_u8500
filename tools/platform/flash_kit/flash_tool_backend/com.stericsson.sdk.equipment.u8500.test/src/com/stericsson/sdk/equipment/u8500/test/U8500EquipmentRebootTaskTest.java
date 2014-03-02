package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentRebootTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author ezaptom
 *
 */
public class U8500EquipmentRebootTaskTest extends TestCase {

    EquipmentRebootTask rebootTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String[] ARGUMENTS_DEC_MODE = new String[] {
        CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), "<equipment-id>", "1"};

    static final String[] ARGUMENTS_HEX_MODE = new String[] {
        CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), "<equipment-id>", "0x1"};

    /**
     * 
     */
    public void testU8500RebootTask() {
        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        rebootTask = new EquipmentRebootTask(eq);

        rebootTask.setArguments(ARGUMENTS_DEC_MODE);
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = rebootTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(rebootTask.getId() + RESULT_MSG), result.getResultMessage());

        rebootTask.setArguments(ARGUMENTS_HEX_MODE);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = rebootTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(rebootTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(rebootTask.isCancelable());

        assertEquals(CommandName.SYSTEM_REBOOT_EQUIPMENT.name(), rebootTask.getId());

        assertEquals(CommandName.SYSTEM_REBOOT_EQUIPMENT.name() + "@" + eq, rebootTask.toString());

        assertFalse(result.isFatal());
    }
}
