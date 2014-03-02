/**
 * 
 */
package com.stericsson.sdk.equipment.u8500.test;

import junit.framework.TestCase;

import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.IEquipmentProfileManagerService;
import com.stericsson.sdk.equipment.tasks.EquipmentBindPropertiesTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetPropertiesTask;
import com.stericsson.sdk.equipment.tasks.EquipmentGetPropertyTask;
import com.stericsson.sdk.equipment.tasks.EquipmentSetPropertyTask;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;

/**
 * @author cizovhel
 * 
 */
public class U8500EquipmentPropertiesTaskTest extends TestCase {

    static final String PROPERTY_NAME = "IMEI";

    static final String PROPERTY_ID = "0x01";

    static final String PROPERTY_ID_INVALID = "0x00";

    static final String PROPERTY_VALUE = "000102030405060708090001020304";

    static final String[] ARGUMENTS_SET_PROPERTY =
        new String[] {
            CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(),
            PROPERTY_NAME + "=" + PROPERTY_VALUE};

    static final String[] ARGUMENTS_SET_PROPERTY_INVALID = new String[] {
        CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(), PROPERTY_NAME};

    static final String[] ARGUMENTS_SET_PROPERTY_ID =
        new String[] {
            CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(),
            PROPERTY_ID + "=" + PROPERTY_VALUE};

    static final String[] ARGUMENTS_SET_PROPERTY_ID_INVALID =
        new String[] {
            CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(),
            PROPERTY_ID_INVALID + "=" + PROPERTY_VALUE};

    static final String[] ARGUMENTS_GET_PROPERTY = new String[] {
        CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(), PROPERTY_NAME};

    static final String[] ARGUMENTS_GET_PROPERTY_ID =
        new String[] {
            CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(),
            String.valueOf(PROPERTY_ID)};

    static final String[] ARGUMENTS_GET_PROPERTY_INVALID =
        new String[] {
            CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), Activator.getPort().toString(),
            String.valueOf(PROPERTY_ID_INVALID)};

    static final SecurityPropertiesType RESULT_SECURITY_PROPERTY = new SecurityPropertiesType(0, new byte[] {
        0, 1, 2, 3, 4});

    static final int RESULT_SUCCESS = 0;

    static final int RESULT_FAIL = 1;

    static final String RESULT_MSG = "Operation successful. Operation finished successfully.";// "No error";

    private EquipmentGetPropertiesTask getPropertiesTask;

    private EquipmentGetPropertyTask getPropertyTask;

    private EquipmentSetPropertyTask setPropertyTask;

    private EquipmentBindPropertiesTask bindPropertiesTask;

    /**
     * Test EquipmentGetProperties task
     */
    @Test
    public void testU8500EquipmentGetPropertiesTask() {

        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), lookupProfile(findActiveProfileAlias()));
        getPropertiesTask = new EquipmentGetPropertiesTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        EquipmentTaskResult result = getPropertiesTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_SECURITY_PROPERTY);
        result = getPropertiesTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(getPropertiesTask.isCancelable());

        assertEquals(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name(), getPropertiesTask.getId());

        assertEquals(CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name() + "@" + equipment.toString(),
            getPropertiesTask.toString());

        assertFalse(result.isFatal());

    }

    /**
     * Test EquipmentGetProperty task
     */
    @Test
    public void testU8500EquipmentGetPropertyTask() {

        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), lookupProfile(findActiveProfileAlias()));
        getPropertyTask = new EquipmentGetPropertyTask(equipment);

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        getPropertyTask.setArguments(ARGUMENTS_GET_PROPERTY);
        EquipmentTaskResult result = getPropertyTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        getPropertyTask.setArguments(ARGUMENTS_GET_PROPERTY_ID);
        result = getPropertyTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        getPropertyTask.setArguments(ARGUMENTS_GET_PROPERTY_INVALID);
        result = getPropertyTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_SECURITY_PROPERTY);
        getPropertyTask.setArguments(ARGUMENTS_GET_PROPERTY);
        result = getPropertyTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(getPropertyTask.isCancelable());

        assertEquals(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), getPropertyTask.getId());

        assertEquals(CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name() + "@" + equipment.toString(), getPropertyTask
            .toString());

        assertFalse(result.isFatal());

    }

    /**
     * Test EquipmentSetProperty task
     */
    @Test
    public void testU8500EquipmentSetPropertyTask() {

        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), lookupProfile(findActiveProfileAlias()));
        setPropertyTask = new EquipmentSetPropertyTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);

        setPropertyTask.setArguments(ARGUMENTS_SET_PROPERTY);
        EquipmentTaskResult result = setPropertyTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setPropertyTask.setArguments(ARGUMENTS_SET_PROPERTY_INVALID);
        result = setPropertyTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);
        setPropertyTask.setArguments(ARGUMENTS_SET_PROPERTY_ID);
        result = setPropertyTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        Activator.getLoaderCommunicationMock().setResult(RESULT_FAIL);
        setPropertyTask.setArguments(ARGUMENTS_SET_PROPERTY_ID_INVALID);
        result = setPropertyTask.execute();
        assertEquals(RESULT_FAIL, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(setPropertyTask.isCancelable());

        assertEquals(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), setPropertyTask.getId());

        assertEquals(CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name() + "@" + equipment.toString(), setPropertyTask
            .toString());

        assertFalse(result.isFatal());

    }

    /**
     * Test EquipmentGetProperties task
     * 
     */
    @Test
    public void testU8500EquipmentBindPropertiesTask() {

        U8500Equipment equipment = new U8500Equipment(Activator.getPort(), lookupProfile(findActiveProfileAlias()));
        bindPropertiesTask = new EquipmentBindPropertiesTask(equipment);
        Activator.getLoaderCommunicationMock().setResult(RESULT_SUCCESS);

        EquipmentTaskResult result = bindPropertiesTask.execute();
        assertEquals(RESULT_SUCCESS, result.getResultCode());

        assertNull(result.getResultObject());

        assertFalse(bindPropertiesTask.isCancelable());

        assertEquals(CommandName.SECURITY_BIND_PROPERTIES.name(), bindPropertiesTask.getId());

        assertEquals(CommandName.SECURITY_BIND_PROPERTIES.name() + "@" + equipment.toString(), bindPropertiesTask
            .toString());

        assertFalse(result.isFatal());

    }

    private IEquipmentProfile lookupProfile(String profileAlias) {
        IEquipmentProfile profile = null;
        ServiceReference reference = null;

        reference = Activator.getBundleContext().getServiceReference(IEquipmentProfileManagerService.class.getName());

        IEquipmentProfileManagerService service =
            (IEquipmentProfileManagerService) Activator.getBundleContext().getService(reference);

        profile = service.getProfile(profileAlias, true);

        Activator.getBundleContext().ungetService(reference);

        return profile;
    }

    private String findActiveProfileAlias() {
        String profileAlias = null;
        ServiceReference[] references = null;

        try {
            references =
                Activator.getBundleContext().getServiceReferences(IConfigurationService.class.getName(),
                    "(type=backend)");
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        if (references != null) {
            IConfigurationService service =
                (IConfigurationService) Activator.getBundleContext().getService(references[0]);
            IConfigurationRecord[] records = service.getRecords(new String[] {
                "ActiveProfile"});
            if (records.length > 0) {
                profileAlias = records[0].getValue("alias");
            }

            Activator.getBundleContext().ungetService(references[0]);

        }
        return profileAlias;
    }
}
