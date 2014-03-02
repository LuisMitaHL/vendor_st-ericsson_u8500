package com.stericsson.sdk.equipment.u5500.test;

import junit.framework.TestCase;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentPermanentAuthenticationTask;
import com.stericsson.sdk.equipment.u5500.internal.U5500Equipment;


/**
 * @author Xxvs0002
 *
 */
public class U5500EquipmentPermanentAuthenticationTaskTest extends TestCase{

    private EquipmentPermanentAuthenticationTask equipmentPermanentAuthenticationTask;

    static String[] arguments = new String[]{};

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 5;

    static final int ERROR = 1;

    /**
     * 
     */
    public void testEquipmentPermanentAuthenticationTask(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentPermanentAuthenticationTask = new EquipmentPermanentAuthenticationTask(eq);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult equipmentTaskResult = equipmentPermanentAuthenticationTask.execute();
        assertEquals(RESULT_SUCCESS, equipmentTaskResult.getResultCode());

        equipmentPermanentAuthenticationTask.setArguments(arguments);

        assertEquals(CommandName.SYSTEM_PERMANENT_AUTHENTICATION.name(), equipmentPermanentAuthenticationTask.getId());

        assertNull(equipmentPermanentAuthenticationTask.getChallengeResponse(null));

        assertFalse(equipmentPermanentAuthenticationTask.isCancelable());

        assertNull(equipmentPermanentAuthenticationTask.getControlKeys());

        equipmentPermanentAuthenticationTask.cancel();
    }

    /**
     * 
     */
    public void testEquipmentPermanentAuthenticationTaskFail(){
        U5500Equipment eq = new U5500Equipment(Activator.getPort(), null);

        equipmentPermanentAuthenticationTask = new EquipmentPermanentAuthenticationTask(eq);

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        EquipmentTaskResult equipmentTaskResult = equipmentPermanentAuthenticationTask.execute();
        assertEquals(RESULT_FAIL, equipmentTaskResult.getResultCode());
    }
}
