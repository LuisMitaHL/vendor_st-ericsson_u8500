package com.stericsson.sdk.equipment.l9540.test;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.l9540.internal.L9540Equipment;
import com.stericsson.sdk.equipment.l9540.internal.L9540Platform;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author mbocek01
 * 
 */
public class L9540EquipmentTest extends TestCase {

    private static final long TIMEOUT = 1234;

    private List<SupportedCommand> supportedCommands = new ArrayList<SupportedCommand>();

    private SupportedCommandsType typeSuccess = new SupportedCommandsType(0, 1);

    private SupportedCommandsType typefail = new SupportedCommandsType(1, 1);

    /**
     * test equipment
     * 
     * @throws Exception
     *             TBD
     */
    public void testl9540Equipment() throws Exception {

        L9540Equipment eq = new L9540Equipment(Activator.getPort(), null);
        if (eq == null) {
            throw new Exception("Got unexpected null value");
        }

        assertEquals(L9540Platform.PLATFORM + "@" + Activator.getPort().getPortName(), eq.toString());

        eq.initLoaderCommunication();

        eq.setTimeouts("type", TIMEOUT);
        Activator.getLoaderCommunicationMock().setResult(TIMEOUT);
        assertEquals(TIMEOUT, eq.getTimeouts("type"));

        supportedCommands.add(new SupportedCommand(1, 1, 1));

        typeSuccess.addSupportedCommand(1, 1, 1);

        typefail.addSupportedCommand(1, 1, 1);

        Activator.getLoaderCommunicationMock().setResult(typefail);
        eq.updateModel(EquipmentModel.COMMAND);

        Activator.getLoaderCommunicationMock().setResult(typeSuccess);
        eq.updateModel(EquipmentModel.COMMAND);

        Activator.getLoaderCommunicationMock().setResult(supportedCommands);

        List<SupportedCommand> supportedCommands2 = eq.getSupportedCommands();
        if (supportedCommands2 == null) {
            throw new Exception("Got unexpected null value");
        }
        assertEquals(supportedCommands.size(), supportedCommands2.size());

        Activator.getLoaderCommunicationMock().setResult(true);
        assertTrue(eq.deinitLoaderCommunication());

    }

    /**
     * test equipment
     * 
     * @throws Exception
     *             TBD
     */
    public void testl9540EquipmentBackendMmodeTest() throws Exception {
        L9540Equipment eq = new L9540Equipment(Activator.getPort(), null);
        eq.deinitLoaderCommunication();
        L9540Equipment eq2 = new L9540Equipment(Activator.getPort(), null);
    }

    /**
     * test equipment
     * 
     * @throws Exception
     *             TBD
     */
    public void testl9540EquipmentUpdateFlashModel() throws Exception {
        L9540Equipment eq = new L9540Equipment(Activator.getPort(), null);
        eq.updateModel(EquipmentModel.FLASH);
    }
}
