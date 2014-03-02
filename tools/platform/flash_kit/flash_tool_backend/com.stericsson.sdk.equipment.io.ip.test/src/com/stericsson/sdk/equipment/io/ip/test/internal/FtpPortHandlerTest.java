package com.stericsson.sdk.equipment.io.ip.test.internal;

import java.io.File;

import javax.jmdns.ServiceEvent;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortProvider;

/**
 * @author eolabor
 * 
 */
public class FtpPortHandlerTest extends TestCase {

    FtpPortHandler portHandler;

    String ipAdress = "TestIP";

    private static final String DEFAULT_DOWNLOAD_DIRECTORY = "AutoDownloadedDumps";

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {
        portHandler = FtpPortHandler.getInstance();
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void tearDown() throws Exception {
        portHandler = null;
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#getInstance()}.
     */
    @Test
    public void testGetInstance() {
        FtpPortHandler tempPortHandler = FtpPortHandler.getInstance();
        assertTrue(tempPortHandler.equals(portHandler));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#addDevice()}.
     */
    @Test
    public void testAddDeviceNoDeviceInList() {
        portHandler.addDevice(ipAdress);
        assertTrue(portHandler.getIpDevices().containsKey(ipAdress));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#addDevice()}.
     */
    @Test
    public void testAddDeviceDeviceExist() {
        FtpPortProvider portProvider = new FtpPortProvider(ipAdress);
        portHandler.getIpDevices().clear();
        portHandler.getIpDevices().put(ipAdress, portProvider);
        portHandler.addDevice(ipAdress);
        assertTrue(portHandler.getIpDevices().size() == 1);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#dumpingDevice()}.
     */
    @Test
    public void testDumpingDeviceDeviceExists() {
        portHandler.dumpingDevice(ipAdress);
        assertTrue(portHandler.getIpDevices().get(ipAdress).isDumping());
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#dumpingDevice()}.
     */
    @Test
    public void testDumpingDeviceDeviceNotExisting() {
        portHandler.dumpingDevice("NewIP");
        assertTrue(portHandler.getIpDevices().get("NewIP").isDumping());
    }

    /**
     * Test method
     */
    @Test
    public void testFtpPortProviderDumpingFalse() {
        portHandler.getIpDevices().get("NewIP").dumping(false);
        assertFalse(portHandler.getIpDevices().get("NewIP").isDumping());
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#removeDevice()}.
     */
    @Test
    public void testRemoveDevice() {
        JmDNSStub stub = new JmDNSStub();
        ServiceEvent event = stub.getEvent();
        portHandler.removeDevice(event);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#removeDevice()}.
     */
    @Test
    public void testRemoveDeviceFail() {
        portHandler.getIpDevices().clear();
        JmDNSStub stub = new JmDNSStub();
        ServiceEvent event = stub.getEvent();
        portHandler.removeDevice(event);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#setAutoDelete()}.
     */
    @Test
    public void testSetAuto() {
        portHandler.setAutoDownload(false);
        assertFalse(portHandler.setAutoDelete(true));
        portHandler.setAutoDownload(true);
        assertTrue(portHandler.setAutoDelete(true));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#getAutoDownloadDirectory()}
     * .
     */
    @Test
    public void testgetAutoDownloadDirectoryNull() {
        portHandler.setAutoDownloadDirectory(null);
        String result = portHandler.getAutoDownloadDirectory();
        String expected =
            new File(System.getProperty("user.home") + "/" + DEFAULT_DOWNLOAD_DIRECTORY).getAbsolutePath();
        assertEquals(expected, result);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.internal.FtpPortHandler#getAutoDownloadDirectory()}
     * .
     */
    @Test
    public void testgetAutoDownloadDirectory() {
        portHandler.setAutoDownloadDirectory("testPath");
        String result = portHandler.getAutoDownloadDirectory();
        assertEquals("testPath", result);
    }

    /**
     * Test method
     */
    @Test
    public void testFtpPortProviderDumpingNoEquipment() {
        FtpPortProvider portProvider = new FtpPortProvider("UnknownIP");
        portProvider.dumping(true);
        assertFalse(portProvider.isDumping());
    }

}
