package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentDumpAreaTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * @author xdancho
 * 
 */
public class U8500EquipmentDumpAreaTaskTest extends TestCase {

    EquipmentDumpAreaTask dumpAreaTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static String[] argumentsHex =
        new String[] {
            "FLASH_DUMP_AREA", "<equipment-id>", "/flash0/", "0x0", "0x1000", System.getProperty("java.io.tmpdir"),
            "true", "false"};

    static String[] argumentsDec =
        new String[] {
            "FLASH_DUMP_AREA", "<equipment-id>", "/flash0/", "0", "1000", System.getProperty("java.io.tmpdir"), "true",
            "true"};

    static String[] argumentsAll =
        new String[] {
            "FLASH_DUMP_AREA", "<equipment-id>", "/flash0/", "0", "ALL", System.getProperty("java.io.tmpdir"), "false",
            "true"};

    static String[] argumentsAllNoRedundant =
        new String[] {
            "FLASH_DUMP_AREA", "<equipment-id>", "/flash0/", "0", "ALL", System.getProperty("java.io.tmpdir"), "false",
            "false"};

    /**
     * dump area test
     */
    public void testU8500DumpArea() {

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        dumpAreaTask = new EquipmentDumpAreaTask(eq);

        dumpAreaTask.setArguments(argumentsDec);
        EquipmentTaskResult result = dumpAreaTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(dumpAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        dumpAreaTask.setArguments(argumentsAll);
        result = dumpAreaTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(dumpAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        dumpAreaTask.setArguments(argumentsAllNoRedundant);
        result = dumpAreaTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(dumpAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        dumpAreaTask.setArguments(argumentsHex);
        result = dumpAreaTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(dumpAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(dumpAreaTask.isCancelable());

        assertEquals(CommandName.FLASH_DUMP_AREA.name(), dumpAreaTask.getId());

        assertEquals(CommandName.FLASH_DUMP_AREA.name() + "@" + eq.toString(), dumpAreaTask.toString());

        assertFalse(result.isFatal());

    }

}
