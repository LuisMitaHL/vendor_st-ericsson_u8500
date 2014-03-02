package com.stericsson.sdk.equipment.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentStatus;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;


/**
 * @author Xxvs0002
 *
 */
public class EquipmentBootingDoneTaskTest extends TestCase{

    private AbstractLoaderCommunicationEquipment eq;

    private EquipmentState state;

    /** */
    int noError = 0;

    /** */
    int error = 1;

    /**
     *
     */
    @Before
    public void setUp(){
        eq = new AbstractLoaderCommunicationEquipment(TestFragmentActivator.getPort(), null) {

            public EquipmentType getEquipmentType() {
                // TODO Auto-generated method stub
                return null;
            }

            @Override
            public String toString() {
                // TODO Auto-generated method stub
                return null;
            }

            @Override
            public EquipmentStatus getStatus() {
                return new EquipmentStatus(state, "Not initialized");
            }

            @Override
            public void setStatus(EquipmentState equipmentState) {
               state = equipmentState;
            }
        };
    }

    /**
     * 
     */
    @Test
    public void testTask(){
        state = EquipmentState.LC_NOT_INITIALIZED;
        eq.setStatus(state);
        EquipmentBootingDoneTask bootingDoneTask = new EquipmentBootingDoneTask(eq);

        assertEquals("BOOTING_DONE", bootingDoneTask.getId());
        assertEquals(false, bootingDoneTask.isCancelable());

        EquipmentTaskResult equipmentTaskResult = bootingDoneTask.execute();
        assertEquals(noError, equipmentTaskResult.getResultCode());

        bootingDoneTask.cancel();
        bootingDoneTask.setArguments(new String[]{""});
    }

    /**
     * 
     */
    @Test
    public void testTaskIdle(){
        state = EquipmentState.IDLE;
        eq.setStatus(state);
        EquipmentBootingDoneTask bootingDoneTask = new EquipmentBootingDoneTask(eq);
        EquipmentTaskResult equipmentTaskResult = bootingDoneTask.execute();
        assertEquals(noError, equipmentTaskResult.getResultCode());
    }

    /**
     * 
     */
    @Test
    public void testTaskError(){
        state = EquipmentState.AUTHENTICATING;
        eq.setStatus(state);
        EquipmentBootingDoneTask bootingDoneTask = new EquipmentBootingDoneTask(eq);
        EquipmentTaskResult equipmentTaskResult = bootingDoneTask.execute();
        assertEquals(error, equipmentTaskResult.getResultCode());
    }
}
