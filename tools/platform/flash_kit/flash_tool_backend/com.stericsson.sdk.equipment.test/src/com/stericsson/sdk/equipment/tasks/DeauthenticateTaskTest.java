package com.stericsson.sdk.equipment.tasks;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.equipment.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.internal.LoaderCommunicationMock;

/**
 * Deauthentication test
 * 
 * @author tsikor01
 * 
 */
public class DeauthenticateTaskTest extends TestCase {

    private static String[] pArguments = new String[] {
        "SYSTEM_DEAUTHENTICATE", "<equipment-id>"};

    private AbstractLoaderCommunicationEquipment eq;

    private EquipmentDeauthenticateTask deauthenticateTask;

    private static final String COMPLETED = "deauthenticate completed.";

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

            @Override
            public ILoaderCommunicationService getLoaderCommunicationService() {
                return new LoaderCommunicationMock();
            }
        };
        deauthenticateTask = new EquipmentDeauthenticateTask(eq);
    }

    /**
     * Tests successful task execution
     */
    @Test
    public void testDeauthenticationSuccess() {
        String[] arguments = new String[] {
                deauthenticateTask.getId(), deauthenticateTask.getEquipment().getPort().getPortName(), "true"};

        deauthenticateTask.setArguments(arguments);
        EquipmentTaskResult result = deauthenticateTask.execute();

        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * Tests unsuccessful task execution
     */
    @Test
    public void testDeauthenticationFaulure() {
        deauthenticateTask.setArguments(pArguments);
        EquipmentTaskResult result = deauthenticateTask.execute();

        assertEquals(RESULT_FAILURE, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

}
