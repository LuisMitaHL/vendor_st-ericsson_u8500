package com.stericsson.sdk.equipment.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;

/**
 * GetAuthenticationType task test
 * 
 * @author tsikor01
 * 
 */
public class GetAuthenticationTypeTaskTest extends TestCase {

    private static String[] pArguments = new String[] {
        "SYSTEM_GET_AUTHENTICATION_STATE", "<equipment-id>"};

    private AbstractLoaderCommunicationEquipment eq;

    private EquipmentGetAuthenticationStateTask authenticationTask;

    private static final String COMPLETED = "get_authentication_state completed.";

    private static final int RESULT_SUCCESS = 0;

    private static final int RESULT_FAILURE = -1;

    /**
     * @throws Exception .
     */
    @Before
    public void setUp() throws Exception {
        eq = new AbstractLoaderCommunicationEquipment(TestFragmentActivator.getPort(), null) {

            @Override
            public String toString() {
                // N/A
                return null;
            }

            public EquipmentType getEquipmentType() {
                // N/A
                return null;
            }

        };
        authenticationTask = new EquipmentGetAuthenticationStateTask(eq);
    }

    /**
     * Tests successful task execution
     */
    @Test
    public void testSuccess() {
        TestFragmentActivator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArguments);
        EquipmentTaskResult result = authenticationTask.execute();

        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * Tests unsuccessful task execution
     */
    @Test
    public void testFaulure() {
        TestFragmentActivator.getLoaderCommunicationMock().setResult(RESULT_FAILURE);
        authenticationTask.setArguments(pArguments);
        EquipmentTaskResult result = authenticationTask.execute();

        assertEquals(RESULT_FAILURE, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

}
