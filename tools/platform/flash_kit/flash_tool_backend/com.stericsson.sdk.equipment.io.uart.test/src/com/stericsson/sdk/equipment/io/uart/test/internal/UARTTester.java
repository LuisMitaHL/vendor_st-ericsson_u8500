package com.stericsson.sdk.equipment.io.uart.test.internal;

import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_ALIAS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_BAUDRATE;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_DATABITS;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_PARITY;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_STOPBIT;
import static com.stericsson.sdk.common.configuration.ConfigurationOption.PORT_UART_TRIGGER;
import static com.stericsson.sdk.equipment.Activator.getServiceObjects;

import java.util.Arrays;
import java.util.Hashtable;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.IPortProvider;
import com.stericsson.sdk.equipment.io.port.IPortProviderTester;
import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.equipment.io.uart.test.TestFragmentActivator;

/**
 * @author rohacmic
 * 
 */
public class UARTTester extends TestCase {

    private IConfigurationRecord monitor;

    private IConfigurationRecord trigger;

    private IConfigurationRecord dummyConfig;

    private IConfigurationRecord dummyBlankConfig;

    private TestingPortReciever portReciever;

    private ServiceRegistration registredReciever;

    private IPortProviderTester portProviderTester;

    private ServiceReference portProviderTesterRef;

    /**
     * {@inheritDoc}
     */
    @Before
    public void init() throws Exception {
        // create and register port reciever
        Hashtable<String, String> props = new Hashtable<String, String>();
        props.put("port.reciever.name", IPortProviderTester.UART_PORT_RECIEVER_NAME);
        portReciever = new TestingPortReciever();
        registredReciever =
            TestFragmentActivator.getBundleContext().registerService(IPortReceiver.class.getName(), portReciever, props);

        monitor = new MESPConfigurationRecord();
        monitor.setName(MONITOR);
        monitor.setValue(MONITOR_ACCEPT_EQUIPMENTS, "" + true);

        trigger = new MESPConfigurationRecord();
        trigger.setName(PORT_UART);
        trigger.setValue(PORT_UART_ALIAS, IPortProviderTester.DUMMY_SERIAL_PORT);
        trigger.setValue(PORT_UART_TRIGGER, "" + true);

        dummyBlankConfig = new MESPConfigurationRecord();
        dummyBlankConfig.setName(PORT_UART);
        dummyBlankConfig.setValue(PORT_UART_ALIAS, IPortProviderTester.DUMMY_SERIAL_PORT);

        dummyConfig = new MESPConfigurationRecord();
        dummyConfig.setName(PORT_UART);
        dummyConfig.setValue(PORT_UART_ALIAS, IPortProviderTester.DUMMY_SERIAL_PORT);
        dummyConfig.setValue(PORT_UART_BAUDRATE, "115200");
        dummyConfig.setValue(PORT_UART_DATABITS, "8");
        dummyConfig.setValue(PORT_UART_PARITY, "0");
        dummyConfig.setValue(PORT_UART_STOPBIT, "1");

        setMonitor(false);

        ServiceReference[] refs =
            TestFragmentActivator.getBundleContext().getServiceReferences(IPortProviderTester.class.getName(),
                "(port.provider.tester=UART)");

        if (refs == null) {
            throw new RuntimeException("Cannot find any UART port provider tester!");
        } else if (refs.length != 1) {
            for (ServiceReference ref : refs) {
                TestFragmentActivator.getBundleContext().ungetService(ref);
            }
            throw new RuntimeException("Found " + refs.length + " UART port provider testers, expected only one!");
        } else {
            portProviderTester = (IPortProviderTester) TestFragmentActivator.getBundleContext().getService(refs[0]);
        }
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void deinit() throws Exception {
        if (portProviderTesterRef != null) {
            TestFragmentActivator.getBundleContext().ungetService(portProviderTesterRef);
            portProviderTesterRef = null;
        }
        if (registredReciever != null) {
            registredReciever.unregister();
        }
        portReciever = null;
        portProviderTester = null;
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test
    public void nullBaudrateTest() throws Exception {
        dummyConfig.setValue(PORT_UART_BAUDRATE, null);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyConfig));
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test
    public void nullDataBitsTest() throws Exception {
        dummyConfig.setValue(PORT_UART_DATABITS, null);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyConfig));
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test
    public void nullStopbitTest() throws Exception {
        dummyConfig.setValue(PORT_UART_STOPBIT, null);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyConfig));
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test
    public void blankConfigurationsTest() throws Exception {
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test
    public void validConfigurationWithTriggering() throws Exception {
        setMonitor(true);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
        syncTrigger();
        assertFalse("Port shouldn't be delivered!", portReciever.wasDelivered());

        assertTrue("UART port should be enabled!", setAndCheckConfig(dummyConfig));
        syncTrigger();
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());

    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test(expected = PortException.class)
    public void testUARTPortExceptions() throws Exception {
        setMonitor(true);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
        portProviderTester.simulateUARTDevice(null, null);
        assertTrue("UART port should be enabled!", setAndCheckConfig(dummyConfig));
        syncTrigger();
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());

        AbstractPort port = portReciever.getDeliveredPort();
        assertFalse("Port shouldn't be open since configuration!", port.isOpen());
        port.read();
    }

    /**
     * @throws Exception
     *             When test fails.
     */
    @Test(expected = PortException.class)
    public void testReadingPortFailure() throws Exception {
        setMonitor(true);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
        portProviderTester.simulateUARTDevice(null, null);
        assertTrue("UART port should be enabled!", setAndCheckConfig(dummyConfig));
        syncTrigger();
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());

        AbstractPort port = portReciever.getDeliveredPort();
        assertFalse("Port shouldn't be open because of setting configuration!", port.isOpen());
        port.close();
        port.read();
    }

    /**
     * @throws Exception
     *             TBD
     */
    @Test(expected = PortException.class)
    public void testWritingPortFailure() throws Exception {
        setMonitor(true);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
        portProviderTester.simulateUARTDevice(null, null);
        assertTrue("UART port should be enabled!", setAndCheckConfig(dummyConfig));
        syncTrigger();
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());

        AbstractPort port = portReciever.getDeliveredPort();
        assertFalse("Port shouldn't be open because of setting configuration!", port.isOpen());
        port.close();
        port.write(new byte[0]);
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

        setMonitor(true);
        assertFalse("UART port should be disabled!", setAndCheckConfig(dummyBlankConfig));
        portProviderTester.simulateUARTDevice(readData, writeData);
        assertTrue("UART port should be enabled!", setAndCheckConfig(dummyConfig));
        syncTrigger();
        assertTrue("Port hasn't been delivered!", portReciever.wasDelivered());

        AbstractPort port = portReciever.getDeliveredPort();
        assertFalse("Port shouldn't be open because of setting configuration!", port.isOpen());
        port.open();
        assertEquals("Z-protocol start character was expected!", port.read(), 'z');
        assertTrue("One byte was expected to be sent!", port.write('?') == 1);
        byte[] readBuf = new byte[10];
        int ret = port.read(readBuf);

        assertTrue("Bad amount of data has been read!", ret == fakeChipID.length);
        assertTrue("Bad data has been read!", Arrays.equals(copyOfRange(readBuf, 0, ret), fakeChipID));

        port.close();
    }

    private boolean setAndCheckConfig(IConfigurationRecord record) throws Exception {
        notifyConfigurationListeners(null, record);
        for (IPortProvider provider : TestFragmentActivator.getServiceObjects(IPortProvider.class, null)) {
            if (provider.getIdentifier().equals(record.getValue(PORT_UART_ALIAS))) {
                return true;
            }
        }
        return false;
    }

    /**
     * call trigger and wait till it is done.
     */
    private void syncTrigger() throws Exception {
        int t = Thread.activeCount();
        notifyConfigurationListeners(null, trigger);
        while(t!=Thread.activeCount()){
            Thread.yield();
        }
    }

    private void setMonitor(boolean enable) throws Exception {
        monitor.setValue("AcceptEquipments", "" + enable);
        notifyConfigurationListeners(null, monitor);
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
