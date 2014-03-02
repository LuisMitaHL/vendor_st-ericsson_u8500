package com.stericsson.sdk.equipment.io.usb.test.internal;

import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_USB_ENABLED;
import static com.stericsson.sdk.equipment.Activator.getServiceObjects;
import static com.stericsson.sdk.equipment.io.usb.test.Activator.getBundleContext;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.PortChannel;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.usb.test.Activator;

/**
 * @author rohacmic
 * 
 */
public class PortProviderTest extends TestCase {

    private static final int VID = 0xbdb;

    private static final int PID = 0x100e;

    private TestingPortReciever portReciever;

    private IPortProvider portProvider;

    private IBackendService backendService;

    private IPortProviderTester portProviderTester;

    private ServiceRegistration registredReciever;

    private IConfigurationRecord usbConfig;

    private ServiceReference backendServiceRef;

    private ServiceReference portProviderRef;

    private ServiceReference portProviderTesterRefs;

    /**
     * {@inheritDoc}
     */
    @Before
    public void init() throws Exception {
        // create and register port reciever
        Hashtable<String, String> props = new Hashtable<String, String>();
        props.put("port.reciever.name", IPortProviderTester.USB_PORT_RECIEVER_NAME);
        portReciever = new TestingPortReciever();
        registredReciever = getBundleContext().registerService(IPortReceiver.class.getName(), portReciever, props);

        // look for backend service
        backendServiceRef = Activator.getBundleContext().getServiceReference(IBackendService.class.getName());
        if (backendServiceRef != null) {
            backendService = (IBackendService) Activator.getBundleContext().getService(backendServiceRef);
        } else {
            throw new NullPointerException("Lookup for IBackendService failed!");
        }

        usbConfig = MESPConfigurationRecord.parse("USB enabled=\"true\"");
        setMonitor(false);
        notifyConfigurationListeners(null, usbConfig);

        // look for port provider
        ServiceReference[] refs =
            Activator.getBundleContext()
                .getServiceReferences(IPortProvider.class.getName(), "("+IPortProvider.PORT_PROVIDER_NAME+"=USB)");

        if (refs == null) {
            throw new RuntimeException("Cannot find any USB port provider!");
        } else if (refs.length != 1) {
            for (ServiceReference ref : refs) {
                Activator.getBundleContext().ungetService(ref);
            }
            throw new RuntimeException("Found " + refs.length + " USB port providers, expected only one!");
        } else {
            portProviderRef = refs[0];
            portProvider = (IPortProvider) Activator.getBundleContext().getService(portProviderRef);
            if (portProvider.isTriggered()) {
                portProvider.release();
            }
        }

        refs =
            Activator.getBundleContext().getServiceReferences(IPortProviderTester.class.getName(),
                "(port.provider.tester=USB)");
        if (refs == null) {
            throw new RuntimeException("Cannot find any USB port provider tester!");
        } else if (refs.length != 1) {
            for (ServiceReference ref : refs) {
                Activator.getBundleContext().ungetService(ref);
            }
            throw new RuntimeException("Found " + refs.length + " USB port provider testers, expected only one!");
        } else {
            portProviderTesterRefs = refs[0];
            portProviderTester = (IPortProviderTester) Activator.getBundleContext().getService(portProviderTesterRefs);
        }
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void deinit() throws InvalidSyntaxException {
        if (portProviderRef != null) {
            Activator.getBundleContext().ungetService(portProviderRef);
            portProviderRef = null;
        }

        if (portProviderTesterRefs != null) {
            Activator.getBundleContext().ungetService(portProviderTesterRefs);
            portProviderTesterRefs = null;
        }
        if (registredReciever != null) {
            registredReciever.unregister();
        }
        portProviderTester = null;
        portReciever = null;
        portProvider = null;
    }

    /**
     * @throws Throwable
     *             When something went wrong.
     */
    @Test
    public void testBusPoolingTest() throws Throwable {
        setMonitor(false);
        portProvider.trigger();
        Thread.sleep(500);
        assertTrue("Port provider should alredy be triggered here!", portProvider.isTriggered());
        portProvider.release();
        assertFalse("Port provider should already be released here!", portProvider.isTriggered());
    }

    /**
     * @throws Exception
     *             When something went wrong.
     */
    @Test
    public void testPortDelivery() throws Exception {
        setMonitor(false);
        usbConfig.setValue(PORT_USB_ENABLED, "false");
        notifyConfigurationListeners(null, usbConfig);
        setMonitor(true);

        portProviderTester.simulateUSBDevice(VID, PID, null, null);
        assertFalse("Port shouldn't been delivered!", portReciever.wasDelivered());

        usbConfig.setValue(PORT_USB_ENABLED, "true");
        notifyConfigurationListeners(null, usbConfig);
        portProviderTester.simulateUSBDevice(VID, PID, null, null);
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());
        assertEquals("Port hasn't been delivered by expected port provider!", portReciever.getPortDeliveryState(),
            PortDeliveryState.SERVICE);
        portReciever.getDeliveredPort().toString();
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testPortReadWithoutOpening() throws Exception {
        setMonitor(false);
        usbConfig.setValue(PORT_USB_ENABLED, "true");
        notifyConfigurationListeners(null, usbConfig);
        setMonitor(true);

        portProviderTester.simulateUSBDevice(VID, PID, null, null);
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());
        assertEquals("Port hasn't been delivered by expected port provider!", portReciever.getPortDeliveryState(),
            PortDeliveryState.SERVICE);
        try {
            portReciever.getDeliveredPort().read();
        } catch (PortException e) {
            return;
        }
        assertTrue("Throwing PortException was expected!", false);
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testReadingPort() throws Exception {
        byte[] fakeChipID = new byte[] {
            0x6, 0x6, 0x6};
        byte[][] readData = new byte[][] {
            new byte[] {
                'z'}, fakeChipID};
        byte[][] writeData = new byte[][] {
            new byte[] {
                '?'}};

        setMonitor(false);
        usbConfig.setValue(PORT_USB_ENABLED, "true");
        notifyConfigurationListeners(null, usbConfig);
        setMonitor(true);

        portProviderTester.simulateUSBDevice(VID, PID, readData, writeData);
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());
        assertEquals("Port hasn't been delivered by expected port provider!", portReciever.getPortDeliveryState(),
            PortDeliveryState.SERVICE);

        AbstractPort port = portReciever.getDeliveredPort();

        port.open();

        assertEquals("Z-protocol start character was expected!", port.read(), 'z');
        assertTrue("One byte was expected to be sent!", port.write('?') == 1);
        byte[] readBuf = new byte[3];
        int ret = port.read(readBuf);

        assertTrue("Bad amount of data has been read!", ret == fakeChipID.length);
        assertTrue("Bad data has been read!", Arrays.equals(copyOfRange(readBuf, 0, ret), fakeChipID));

        port.close();
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test
    public void testScatteringGatheringIO() throws Exception {
        setMonitor(false);
        usbConfig.setValue(PORT_USB_ENABLED, "true");
        notifyConfigurationListeners(null, usbConfig);
        setMonitor(true);

        ByteBuffer testingPacket = ByteBuffer.allocate(1024).order(ByteOrder.LITTLE_ENDIAN);

        byte[] data = new byte[255];
        for (int i = 0; i < data.length; i++) {
            data[i] = (byte) i;
        }

        testingPacket.put((byte) 0xAA); // packet start
        testingPacket.put(new byte[] {
            0x01, 0x02, 0x03, 0x04}); // some 4 bytes header field
        testingPacket.put(new byte[] {
            0x01, 0x02, 0x03, 0x04}); // some 4 bytes header field
        testingPacket.put(new byte[] {
            0x01, 0x02, 0x03, 0x04}); // some 4 bytes header field
        testingPacket.put(new byte[] {
            0x01, 0x02, 0x03, 0x04}); // some 4 bytes header field
        testingPacket.putInt(data.length);
        int fixedPartLength = testingPacket.position();
        testingPacket.put(data); // data of dynamic lenght
        testingPacket.put((byte) 0xCC); // one byte CRC

        byte[] rawPacketData = new byte[testingPacket.position()];
        System.arraycopy(testingPacket.array(), 0, rawPacketData, 0, rawPacketData.length);

        portProviderTester.simulateUSBDevice(VID, PID, new byte[][] {
            rawPacketData}, new byte[][] {
            rawPacketData});
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());
        assertEquals("Port hasn't been delivered by expected port provider!", portReciever.getPortDeliveryState(),
            PortDeliveryState.SERVICE);

        IPort port = null;
        try {
            port = portReciever.getDeliveredPort();
            port.open();
            PortChannel portChannel = new PortChannel(port);

            ByteBuffer fixedPacketPart = ByteBuffer.allocate(fixedPartLength).order(ByteOrder.LITTLE_ENDIAN);
            portChannel.read(fixedPacketPart);
            ByteBuffer dynamicData =
                ByteBuffer.allocate(fixedPacketPart.getInt(fixedPacketPart.position() - 4)).order(
                    ByteOrder.LITTLE_ENDIAN);
            ByteBuffer crc = ByteBuffer.allocate(1);
            portChannel.read(new ByteBuffer[] {
                dynamicData, crc});

            byte[] recievedPacketData = new byte[fixedPacketPart.position() + dynamicData.position() + crc.position()];
            int pos = 0;
            System.arraycopy(fixedPacketPart.array(), 0, recievedPacketData, pos, fixedPacketPart.position());
            pos += fixedPacketPart.position();
            System.arraycopy(dynamicData.array(), 0, recievedPacketData, pos, dynamicData.position());
            pos += dynamicData.position();
            System.arraycopy(crc.array(), 0, recievedPacketData, pos, crc.position());

            assertTrue(Arrays.equals(rawPacketData, recievedPacketData));

            fixedPacketPart.flip();
            dynamicData.flip();
            crc.flip();
            portChannel.write(new ByteBuffer[] {
                fixedPacketPart, dynamicData, crc});
        } finally {
            port.close();
        }

    }

    /** */
    @Test
    public void testAcceptEquipments() {
        boolean enabled = backendService.isAcceptingEquipments();
        backendService.setAcceptEquipments(!enabled);
        assertTrue("Accept Equipments hasn't been toggled!", enabled ^ portProvider.isTriggered());

        enabled = backendService.isAcceptingEquipments();
        backendService.setAcceptEquipments(!enabled);
        assertTrue("Accept Equipments hasn't been toggled back!", enabled ^ portProvider.isTriggered());
    }

    /** */
    @Test
    public void testReenablingAutosense() {
        backendService.setAcceptEquipments(true);
        assertTrue("Accept Equipments should be triggered!", portProvider.isTriggered());

        backendService.setAcceptEquipments(true);
        assertTrue("Accept Equipments still should be triggered!", portProvider.isTriggered());
    }

    /** */
    @Test
    public void testRedisablingAutosense() {
        backendService.setAcceptEquipments(false);
        assertTrue("Autosense should be disabled!", !portProvider.isTriggered());

        backendService.setAcceptEquipments(false);
        assertTrue("Autosense still should be disabled!", !portProvider.isTriggered());
    }

    private void setMonitor(boolean enable) throws Exception {
        backendService.setAcceptEquipments(false);
    }

    private void notifyConfigurationListeners(IConfigurationRecord oldRecord, IConfigurationRecord newRecord)
        throws InvalidSyntaxException {
        for (IConfigurationServiceListener listener : getServiceObjects(IConfigurationServiceListener.class, null)) {
            listener.configurationChanged(null, oldRecord, newRecord);
        }
    }

    private byte[] copyOfRange(byte[] original, int from, int to) {
        int newLength = to - from;
        if (newLength < 0) {
            throw new IllegalArgumentException(from + " > " + to);
        }
        byte[] copy = new byte[newLength];
        System.arraycopy(original, from, copy, 0, Math.min(original.length - from, newLength));
        return copy;
    }

}
