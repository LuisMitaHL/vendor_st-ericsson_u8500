package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentGetAuthenticationStateTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;


/**
 * @author Xxvs0002
 *
 */
public class U5500EquipmentGetAuthenticationStateTaskTest extends TestCase{

    private EquipmentGetAuthenticationStateTask equipmentGetAuthenticationStateTask;

    static String[] arguments = new String[]{};

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final int ERROR = 1;

    /**
     * 
     */
    public void testEquipmentGetAuthenticationStateTask(){

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentGetAuthenticationStateTask = new EquipmentGetAuthenticationStateTask(eq);
        equipmentGetAuthenticationStateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = equipmentGetAuthenticationStateTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        assertEquals(CommandName.SYSTEM_GET_AUTHENTICATION_STATE.name(), equipmentGetAuthenticationStateTask.getId());

        assertFalse(equipmentGetAuthenticationStateTask.isCancelable());

        assertNull(equipmentGetAuthenticationStateTask.getChallengeResponse(null));

        assertNull(equipmentGetAuthenticationStateTask.getControlKeys());

        equipmentGetAuthenticationStateTask.cancel();
    }

    /**
     * 
     */
    public void testEquipmentGetAuthenticationStateTaskFail(){

        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentGetAuthenticationStateTask = new EquipmentGetAuthenticationStateTask(eq);
        equipmentGetAuthenticationStateTask.setArguments(arguments);

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        EquipmentTaskResult result = equipmentGetAuthenticationStateTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
    }
}
