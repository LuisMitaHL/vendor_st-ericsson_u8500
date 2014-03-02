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
 * Permanent authentication test
 * 
 * @author tsikor01
 * 
 */
public class PermanentAuthenticationTest extends TestCase {

    private static String[] pArguments = new String[] {
        "SYSTEM_PERMANENT_AUTHENTICATION", "<equipment-id>"};

    private AbstractLoaderCommunicationEquipment eq;

    private EquipmentPermanentAuthenticationTask authenticationTask;

    private static final String COMPLETED = "permanent_authentification completed.";

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
        authenticationTask = new EquipmentPermanentAuthenticationTask(eq);
    }

    /**
     * Tests successful authentication
     */
    @Test
    public void testAuthenticationSuccess() {
        authenticationTask.setArguments(pArguments);
        EquipmentTaskResult result = authenticationTask.execute();

        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * Tests unsuccessful authentication
     */
    @Test
    public void testAuthenticationFaulure() {
        authenticationTask.setArguments(pArguments);
        EquipmentTaskResult result = authenticationTask.execute();

        assertEquals(RESULT_FAILURE, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

}
