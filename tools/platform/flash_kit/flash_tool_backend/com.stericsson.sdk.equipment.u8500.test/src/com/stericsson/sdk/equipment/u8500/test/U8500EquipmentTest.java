package com.stericsson.sdk.equipment.u8500.test;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import com.stericsson.sdk.equipment.EquipmentModel;
import com.stericsson.sdk.equipment.u8500.internal.U8500Equipment;
import com.stericsson.sdk.equipment.u8500.internal.U8500Platform;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.SupportedCommandsType;
import com.stericsson.sdk.loader.communication.types.subtypes.Device;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * @author xdancho
 * 
 */
public class U8500EquipmentTest extends TestCase {

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
    public void testU8500Equipment() throws Exception {

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);
        if (eq == null) {
            throw new Exception("Got unexpected null value");
        }

        assertEquals(U8500Platform.PLATFORM + "@" + Activator.getPort().getPortName(), eq.toString());

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
    public void testU8500EquipmentUpdateFlashModel() throws Exception {

        ArrayList<Device> deviceList = new ArrayList<Device>();

        ListDevicesType deviceListSuccess = new ListDevicesType(0, 5);
        ListDevicesType deviceListError = new ListDevicesType(1, 5);

        U8500Equipment eq = new U8500Equipment(Activator.getPort(), null);

        assertEquals(U8500Platform.PLATFORM + "@" + Activator.getPort().getPortName(), eq.toString());

        eq.initLoaderCommunication();

        eq.setTimeouts("type", TIMEOUT);
        Activator.getLoaderCommunicationMock().setResult(TIMEOUT);
        assertEquals(TIMEOUT, eq.getTimeouts("type"));

        deviceList.add(new Device("PATH", 3, "type", 4, 123L, 10000L, 99999L));

        deviceListSuccess.addDevice("PATH", 3, "type", 4, 123L, 10000L, 99999L);

        deviceListError.addDevice("PATH", 3, "type", 4, 123L, 10000L, 99999L);

        Activator.getLoaderCommunicationMock().setResult(deviceListSuccess);
        eq.updateModel(EquipmentModel.FLASH);

        Activator.getLoaderCommunicationMock().setResult(deviceListError);
        eq.updateModel(EquipmentModel.FLASH);

        Activator.getLoaderCommunicationMock().setResult(true);
        assertTrue(eq.deinitLoaderCommunication());

    }
}
