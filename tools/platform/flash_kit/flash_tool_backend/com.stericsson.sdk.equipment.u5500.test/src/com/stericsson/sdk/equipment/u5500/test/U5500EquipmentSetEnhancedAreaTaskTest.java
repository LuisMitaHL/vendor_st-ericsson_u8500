package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentSetEnhancedAreaTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;

/**
 * 
 * @author mbocek01
 * 
 */
public class U5500EquipmentSetEnhancedAreaTaskTest extends TestCase {

    EquipmentSetEnhancedAreaTask setEnhancedAreaTask = null;

    boolean wait = true;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final String RESULT_MSG = " completed.";

    static final String[] ARGUMENTS_HEX = new String[] {
        "FLASH_SET_ENHANCED_AREA", "<equipment-id>", "/flash0/", "0x0", "0x1000"};

    static final String[] ARGUMENTS_DEC = new String[] {
        "FLASH_SET_ENHANCED_AREA", "<equipment-id>", "/flash0/", "0", "1000"};

    /**
     * test erase area
     */
    public void testU5500SetEnhancedArea() {

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);
        setEnhancedAreaTask = new EquipmentSetEnhancedAreaTask(eq);

        setEnhancedAreaTask.setArguments(ARGUMENTS_HEX);
        EquipmentTaskResult result = setEnhancedAreaTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(CommandName.getCLISyntax(setEnhancedAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setEnhancedAreaTask.setArguments(ARGUMENTS_HEX);
        result = setEnhancedAreaTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotSame(CommandName.getCLISyntax(setEnhancedAreaTask.getId()) + RESULT_MSG, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(setEnhancedAreaTask.isCancelable());

        assertEquals(CommandName.FLASH_SET_ENHANCED_AREA.name(), setEnhancedAreaTask.getId());

        assertEquals(CommandName.FLASH_SET_ENHANCED_AREA.name() + "@" + eq.toString(), setEnhancedAreaTask.toString());

        assertFalse(result.isFatal());

    }

}
