/**
 * 
 */
package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.tasks.DomainTypes;
import com.stericsson.sdk.equipment.tasks.EquipmentGetDomainTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSetDomainTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.loader.communication.types.DomainType;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;

/**
 * @author mbodan01
 * 
 */
public class U8500EquipmentDomainTaskTest extends TestCase {

    private static final String TASK_NOT_INITIALIZED = "Internal call error: Task not initialized.";

    private static final String DOMAIN_ARGUMENT_MUST_BE_A_NUMBER_BETWEEN =
        "Domain argument must be a number between 0 and 3.";

    private static final String MISSING_ARGUMENTS = "Missing arguments. Probably domain number is not defined.";

    private static final String DOMAIN_ARGUMENT_MUST_BE_A_NUMBER = "Domain argument must be a number.";

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 1;

    static final String RESULT_MSG = "Operation successful. Operation finished successfully.";// "No error";

    static final SecurityPropertiesType RESULT_SECURITY_PROPERTY = new SecurityPropertiesType(0, new byte[] {
        0, 1, 2, 3, 4});

    static final DomainType RESULT_DOMAIN_SUSSESS = new DomainType(0, DomainTypes.SERVICE.getDomainId());

    static final DomainType RESULT_DOMAIN_FAIL = new DomainType(1, -1);

    static final String[] ARGUMENTS_SET_DOMAIN = new String[] {
        CommandName.SECURITY_SET_DOMAIN.name(), Activator.getPort().toString(), "0"};

    static final String[] ARGUMENTS_SET_DOMAIN_INVALID = new String[] {
        CommandName.SECURITY_SET_DOMAIN.name(), Activator.getPort().toString(), "hello"};

    static final String[] ARGUMENTS_SET_DOMAIN_INVALID_FEW = new String[] {
        CommandName.SECURITY_SET_DOMAIN.name(), Activator.getPort().toString()};

    static final String[] ARGUMENTS_SET_DOMAIN_INVALID_ABOVE = new String[] {
        CommandName.SECURITY_SET_DOMAIN.name(), Activator.getPort().toString(), "6"};

    static final String[] ARGUMENTS_SET_DOMAIN_INVALID_BELOWE = new String[] {
        CommandName.SECURITY_SET_DOMAIN.name(), Activator.getPort().toString(), "-5"};

    private EquipmentGetDomainTask getDomainTask;

    private EquipmentSetDomainTask setDomainTask;

    /**
     * Test EquipmentGetDomain task
     */
    @Test
    public void testU8500EquipmentGetDomainTask() {
        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), null);
        getDomainTask = new EquipmentGetDomainTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = getDomainTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());
        assertNotNull(result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = getDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotNull(result.getResultMessage());

        Activator.getLoaderCommunicationMock().setResult(RESULT_DOMAIN_SUSSESS);
        getDomainTask.setArguments(new String[] {});
        result = getDomainTask.execute();
        getDomainTask.cancel();
        assertEquals(RESULT_DOMAIN_SUSSESS.getStatus(), result.getResultCode());
        assertNull(result.getResultObject());

        Activator.getLoaderCommunicationMock().setResult(RESULT_DOMAIN_FAIL);
        result = getDomainTask.execute();
        assertEquals(RESULT_DOMAIN_FAIL.getStatus(), result.getResultCode());
        assertNotNull(result.getResultMessage());

        assertFalse(getDomainTask.isCancelable());

        assertEquals(CommandName.SECURITY_GET_DOMAIN.name(), getDomainTask.getId());
        assertEquals(CommandName.SECURITY_GET_DOMAIN.name() + "@" + equipment.toString(), getDomainTask.toString());

        assertFalse(result.isFatal());

        Activator.getLoaderCommunicationMock().setResult(RESULT_SECURITY_PROPERTY);
        result = getDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertNotNull(result.getResultMessage());
    }

    /**
     * Test EquipmentSetDomain task
     */
    @Test
    public void testU8500EquipmentSetDomainTask() {
        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), null);
        setDomainTask = new EquipmentSetDomainTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        setDomainTask.setArguments(ARGUMENTS_SET_DOMAIN);
        EquipmentTaskResult result = setDomainTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        setDomainTask = new EquipmentSetDomainTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        result = setDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertEquals(TASK_NOT_INITIALIZED, result.getResultMessage());

        setDomainTask = new EquipmentSetDomainTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setDomainTask.setArguments(ARGUMENTS_SET_DOMAIN_INVALID);
        result = setDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertEquals(DOMAIN_ARGUMENT_MUST_BE_A_NUMBER, result.getResultMessage());

        setDomainTask = new EquipmentSetDomainTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setDomainTask.setArguments(ARGUMENTS_SET_DOMAIN_INVALID_FEW);
        result = setDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertEquals(MISSING_ARGUMENTS, result.getResultMessage());

        setDomainTask = new EquipmentSetDomainTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setDomainTask.setArguments(ARGUMENTS_SET_DOMAIN_INVALID_ABOVE);
        result = setDomainTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertEquals(DOMAIN_ARGUMENT_MUST_BE_A_NUMBER_BETWEEN, result.getResultMessage());

        setDomainTask = new EquipmentSetDomainTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setDomainTask.setArguments(ARGUMENTS_SET_DOMAIN_INVALID_BELOWE);
        result = setDomainTask.execute();
        setDomainTask.cancel();
        assertEquals(RESULT_FAIL, result.getResultCode());
        assertEquals(DOMAIN_ARGUMENT_MUST_BE_A_NUMBER_BETWEEN, result.getResultMessage());

        assertNull(result.getResultObject());

        assertFalse(setDomainTask.isCancelable());

        assertEquals(CommandName.SECURITY_SET_DOMAIN.name(), setDomainTask.getId());

        assertEquals(CommandName.SECURITY_SET_DOMAIN.name() + "@" + equipment.toString(), setDomainTask.toString());

        assertFalse(result.isFatal());
    }
}
