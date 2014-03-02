package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentDeauthenticateTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;


/**
 * @author Xxvs0002
 *
 */
public class U5500EquipmentDeauthenticateTaskTest extends TestCase{

    EquipmentDeauthenticateTask equipmentDeauthenticateTask;

    String[] arguments;

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final int ERROR = 1;

    /**
     * 
     */
    public void testDeauthenticateTask(){
        arguments = new String[] {
            CommandName.SYSTEM_DEAUTHENTICATE.name(), "<equipment-id>", "true"};

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentDeauthenticateTask = new EquipmentDeauthenticateTask(eq);
        equipmentDeauthenticateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = equipmentDeauthenticateTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        assertEquals(CommandName.SYSTEM_DEAUTHENTICATE.name(), equipmentDeauthenticateTask.getId());

        assertFalse(equipmentDeauthenticateTask.isCancelable());

        equipmentDeauthenticateTask.cancel();
    }

    /**
     * 
     */
    public void testDeauthenticateTaskWithFlase(){
        arguments = new String[] {
            CommandName.SYSTEM_DEAUTHENTICATE.name(), "<equipment-id>", "false"};

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentDeauthenticateTask = new EquipmentDeauthenticateTask(eq);
        equipmentDeauthenticateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        EquipmentTaskResult result = equipmentDeauthenticateTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
    }

    /**
     * 
     */
    public void testDeauthenticateTaskWithNull(){
        arguments = new String[] {
            CommandName.SYSTEM_DEAUTHENTICATE.name(), "<equipment-id>", "null"};

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentDeauthenticateTask = new EquipmentDeauthenticateTask(eq);
        equipmentDeauthenticateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = equipmentDeauthenticateTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
    }

    /**
     * 
     */
    public void testDeauthenticateTaskWithError(){
        arguments = new String[] {
            CommandName.SYSTEM_DEAUTHENTICATE.name(), "<equipment-id>", "error"};

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentDeauthenticateTask = new EquipmentDeauthenticateTask(eq);
        equipmentDeauthenticateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(ERROR);
        EquipmentTaskResult result = equipmentDeauthenticateTask.execute();
        assertEquals(ERROR, result.getResultCode());
    }
}
