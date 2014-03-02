package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.EquipmentAuthenticateControlKeysTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;

/**
 * Tests for AuthenticateControlKeysTask.
 * 
 * @author xadazim
 * 
 */
public class U8500EquipmentAuthenticateControlKeysTaskTest extends TestCase {
    private EquipmentAuthenticateControlKeysTask authenticationTask;

    private static String[] pArgumentsOkPermanent =
        new String[] {
            "SYSTEM_AUTHENTICATE_CONTROL_KEYS",
            "<equipment-id>",
            "Network Lock Key=5530592768011169,Network Subset Lock Key=1113474185369446,"
                + "Service Provider Lock Key=0920954133604256,Corporate Lock Key=1330296042918872,"
                + "Flexible ESL Lock Key=5979950341706796", "false"};

    private static String[] pArgumentsOkTemporary =
        new String[] {
            "SYSTEM_AUTHENTICATE_CONTROL_KEYS",
            "<equipment-id>",
            "Network Lock Key=5530592768011169,Network Subset Lock Key=1113474185369446,"
                + "Service Provider Lock Key=0920954133604256,Corporate Lock Key=1330296042918872,"
                + "Flexible ESL Lock Key=5979950341706796", "false"};

    private static String[] pArgumentsOkReversed =
        new String[] {
            "SYSTEM_AUTHENTICATE_CONTROL_KEYS",
            "<equipment-id>",
            "Service Provider Lock Key=0920954133604256,Corporate Lock Key=1330296042918872,"
                + "Network Lock Key=5530592768011169,Network Subset Lock Key=1113474185369446,"
                + "Flexible ESL Lock Key=5979950341706796", "false"};

    // one key definition is missing
    private static String[] pArgumentsBad =
        new String[] {
            "SYSTEM_AUTHENTICATE_CONTROL_KEYS",
            "<equipment-id>",
            "Network Lock Key=5530592768011169,Service Provider Lock Key=0920954133604256,"
                + "Corporate Lock Key=1330296042918872,Flexible ESL Lock Key=5979950341706796", "false"};

    private static final String COMPLETED = "Operation successful. Operation finished successfully.";

    private static final int RESULT_SUCCESS = 0;

    private static final int RESULT_FAILURE = -1;

    private U8500Equipment eq;

    /**
     * @throws Exception .
     */
    @Before
    public void setUp() throws Exception {
        eq = new U8500Equipment(Activator.getPort(), null);
        authenticationTask = new EquipmentAuthenticateControlKeysTask(eq);
    }

    /**
     * 
     * {@inheritDoc}
     */
    @After
    public void tearDown() {
        eq = null;
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateSuccesPermanent() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsOkPermanent);

        assertEquals(authenticationTask.getControlKeys().length, 5);
        assertEquals((String) authenticationTask.getControlKeys()[0], "5530592768011169");
        assertEquals((String) authenticationTask.getControlKeys()[1], "1113474185369446");
        assertEquals((String) authenticationTask.getControlKeys()[2], "0920954133604256");
        assertEquals((String) authenticationTask.getControlKeys()[3], "1330296042918872");
        assertEquals((String) authenticationTask.getControlKeys()[4], "5979950341706796");

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateSuccesTemporary() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsOkTemporary);

        assertEquals(authenticationTask.getControlKeys().length, 5);
        assertEquals((String) authenticationTask.getControlKeys()[0], "5530592768011169");
        assertEquals((String) authenticationTask.getControlKeys()[1], "1113474185369446");
        assertEquals((String) authenticationTask.getControlKeys()[2], "0920954133604256");
        assertEquals((String) authenticationTask.getControlKeys()[3], "1330296042918872");
        assertEquals((String) authenticationTask.getControlKeys()[4], "5979950341706796");

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateSuccesKeysReversed() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsOkReversed);

        assertEquals(authenticationTask.getControlKeys().length, 5);
        assertEquals((String) authenticationTask.getControlKeys()[0], "5530592768011169");
        assertEquals((String) authenticationTask.getControlKeys()[1], "1113474185369446");
        assertEquals((String) authenticationTask.getControlKeys()[2], "0920954133604256");
        assertEquals((String) authenticationTask.getControlKeys()[3], "1330296042918872");
        assertEquals((String) authenticationTask.getControlKeys()[4], "5979950341706796");

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertEquals(COMPLETED, result.getResultMessage());
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateFailGoodKeys() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAILURE);
        authenticationTask.setArguments(pArgumentsOkPermanent);

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(RESULT_FAILURE, result.getResultCode());
        assertTrue(result.getResultMessage().contains("Failed to authenticate."));
    }

    /**
     * @throws Throwable .
     */
    @Test
    public void testU8500AuthenticateFailBadKeys() throws Throwable {
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        authenticationTask.setArguments(pArgumentsBad);

        EquipmentTaskResult result = authenticationTask.execute();
        assertEquals(result.getResultCode(), RESULT_FAILURE);
        assertTrue(result.getResultMessage().contains("Wrong or incomplete sim lock keys were provided"));
    }

    /**
     * 
     */
    @Test
    public void testIsCancelable() {
        assertFalse(authenticationTask.isCancelable());
        authenticationTask.cancel(); // does nothing, increase coverage
    }

    /**
     * 
     */
    @Test
    public void testGetId() {
        assertEquals(authenticationTask.getId(), CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name());
    }

    /**
     * 
     */
    @Test
    public void testGetChallangeResponse() {
        assertNull(authenticationTask.getChallengeResponse(null));
    }

    /**
     * 
     */
    @Test
    public void testToString() {
        String s = authenticationTask.toString();
        assertEquals(s, "SYSTEM_AUTHENTICATE_CONTROL_KEYS@u8500@USB2");
    }

}
