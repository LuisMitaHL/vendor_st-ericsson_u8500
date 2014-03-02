package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentInitArbTableTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;


/**
 * @author Xxvs0002
 *
 */
public class U5500EquipmentInitArbTableTaskTest extends TestCase{

    EquipmentInitArbTableTask equipmentInitArbTableTask;

    String[] arguments = new String[]{};

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final int ERROR = 1;

    /**
     * 
     */
    public void testEquipmentInitArbTableTaskHex(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentInitArbTableTask = new EquipmentInitArbTableTask(eq);

        arguments = new String[] {
                CommandName.SECURITY_INIT_ARB_TABLE.name(), "<equipment-id>", "0xAABB"};

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);

        equipmentInitArbTableTask.setArguments(arguments);
        EquipmentTaskResult equipmentTaskResult = equipmentInitArbTableTask.execute();
        assertEquals(RESULT_SUCCESS, equipmentTaskResult.getResultCode());

        assertFalse(equipmentInitArbTableTask.isCancelable());

        assertEquals(CommandName.SECURITY_INIT_ARB_TABLE.name(), equipmentInitArbTableTask.getId());

        assertEquals(CommandName.SECURITY_INIT_ARB_TABLE.name() + "@" + eq.toString(), equipmentInitArbTableTask.toString());

        equipmentInitArbTableTask.cancel();
    }

    /**
     * 
     */
    public void testEquipmentInitArbTableTaskDec(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentInitArbTableTask = new EquipmentInitArbTableTask(eq);

        arguments = new String[] {
                CommandName.SECURITY_INIT_ARB_TABLE.name(), "<equipment-id>", "123456"};

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);

        equipmentInitArbTableTask.setArguments(arguments);
        EquipmentTaskResult equipmentTaskResult = equipmentInitArbTableTask.execute();
        assertEquals(RESULT_SUCCESS, equipmentTaskResult.getResultCode());
    }

    /**
     * 
     */
    public void testEquipmentInitArbTableTaskException(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentInitArbTableTask = new EquipmentInitArbTableTask(eq);

        arguments = new String[] {
                CommandName.SECURITY_INIT_ARB_TABLE.name(), "<equipment-id>", "abcd"};

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        try{
            equipmentInitArbTableTask.setArguments(arguments);
            fail("Should not get here");
        }catch(RuntimeException exception){
            exception.getMessage();
        }
    }

    /**
     * 
     */
    public void testEquipmentInitArbTableTaskFail(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentInitArbTableTask = new EquipmentInitArbTableTask(eq);

        arguments = new String[] {
                CommandName.SECURITY_INIT_ARB_TABLE.name(), "<equipment-id>", "123456"};

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);

        equipmentInitArbTableTask.setArguments(arguments);
        EquipmentTaskResult equipmentTaskResult = equipmentInitArbTableTask.execute();
        assertEquals(RESULT_FAIL, equipmentTaskResult.getResultCode());
    }
}
