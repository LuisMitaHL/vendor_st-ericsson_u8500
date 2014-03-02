package com.stericsson.sdk.equipment.m7x00.internal;


import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import junit.framework.TestCase;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.m7x00.Activator;
import com.stericsson.sdk.equipment.m7x00.tasks.M7X00EquipmentDeleteFileTask;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xdancho
 * 
 */
public class M7X00EquipmentTest extends TestCase {

    private static final long TIMEOUT = 1234;

    private List<SupportedCommand> supportedCommands = new ArrayList<SupportedCommand>();

    private SupportedCommandsType typeSuccess = new SupportedCommandsType(0, 1);

    private SupportedCommandsType typefail = new SupportedCommandsType(1, 1);

    private M7x00Equipment equipment;

    /**
     * 
     * {@inheritDoc}
     */
    @Before
    public void setUp() {
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
    }

    /**
     * test equipment
     * 
     * @throws Exception
     *             TBD
     */
    @Test
    public void testU8500Equipment() throws Exception {

        assertNotNull(equipment);
        assertEquals(M7x00Platform.PLATFORM + "@" + TestFragmentActivator.getPort().getPortName(), equipment
            .toString());

        equipment.initLoaderCommunication();

        equipment.setTimeouts("type", TIMEOUT);
        TestFragmentActivator.getLoaderCommunicationMock().setResult(TIMEOUT);
        assertEquals(TIMEOUT, equipment.getTimeouts("type"));

        supportedCommands.add(new SupportedCommand(1, 1, 1));

        typeSuccess.addSupportedCommand(1, 1, 1);

        typefail.addSupportedCommand(1, 1, 1);

        TestFragmentActivator.getLoaderCommunicationMock().setResult(typefail);
        equipment.updateModel(EquipmentModel.COMMAND);

        TestFragmentActivator.getLoaderCommunicationMock().setResult(typeSuccess);
        equipment.updateModel(EquipmentModel.COMMAND);

        TestFragmentActivator.getLoaderCommunicationMock().setResult(supportedCommands);

        List<SupportedCommand> supportedCommands2 = equipment.getSupportedCommands();
        if (supportedCommands2 == null) {
            throw new Exception("Got unexpected null value");
        }
        assertEquals(supportedCommands.size(), supportedCommands2.size());

        TestFragmentActivator.getLoaderCommunicationMock().setResult(true);
        assertTrue(equipment.deinitLoaderCommunication());
        equipment.loaderCommunicationError(null, null);
        equipment.loaderCommunicationMessage(null, null, false);
        equipment.loaderCommunicationProgress(0, 0);
        new M7X00EquipmentDeleteFileTask(equipment).execute();
        equipment.getRuntimeExecutor();
        equipment.updateModel(EquipmentModel.FLASH);
        equipment.updateModel(null);
    }

    /**
     * 
     */
    @Test
    public void testAddingService(){
        ServiceReference serviceReference;
        try {
            serviceReference = Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                "(type=mock)")[0];
            Object serviceObject = equipment.addingService(serviceReference);
            equipment.addingService(serviceReference);
            equipment.modifiedService(serviceReference, serviceObject);
            equipment.removedService(serviceReference, serviceObject);
        } catch (InvalidSyntaxException e) {
            fail(e.getMessage());
        }
    }

}
