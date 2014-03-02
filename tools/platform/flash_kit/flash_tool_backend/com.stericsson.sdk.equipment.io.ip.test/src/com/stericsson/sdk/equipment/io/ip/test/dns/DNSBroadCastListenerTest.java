package com.stericsson.sdk.equipment.io.ip.test.dns;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;
import com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.test.internal.JmDNSStub;

/**
 * @author eolabor
 * 
 */
public class DNSBroadCastListenerTest extends TestCase {

    private DNSBroadcastListener mClassUnderTest;

    private JmDNSStub stub;

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {
        mClassUnderTest = new DNSBroadcastListener();
        stub = new JmDNSStub();
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void tearDown() throws Exception {
        mClassUnderTest = null;
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceAdded}.
     */
    @Test
    public void testServiceAddedAvailable() {
        stub.setServiceName(DNSConfiguration.AVAILABLE_ME);
        mClassUnderTest.serviceAdded(stub.getEvent());
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceAdded}.
     */
    @Test
    public void testServiceAddedDump() {
        stub.setServiceName(DNSConfiguration.DUMPING_ME);
        mClassUnderTest.serviceAdded(stub.getEvent());
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceAdded}.
     */
    @Test
    public void testServiceAddedDummy() {
        stub.setServiceName("Dummy");
        mClassUnderTest.serviceAdded(stub.getEvent());
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceResolved}.
     */
    @Test
    public void testServiceResolvedAvailable() {
        stub.setServiceName(DNSConfiguration.AVAILABLE_ME);
        stub.setHostadress("NewIP");
        mClassUnderTest.serviceResolved(stub.getEvent());
        assertTrue(FtpPortHandler.getInstance().getIpDevices().containsKey("NewIP"));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceResolved}.
     */
    @Test
    public void testServiceResolvedDump() {
        stub.setServiceName(DNSConfiguration.DUMPING_ME);
        stub.setHostadress("IpAdress");
        mClassUnderTest.serviceResolved(stub.getEvent());
        boolean dumping = FtpPortHandler.getInstance().getIpDevices().get("IpAdress").isDumping();
        assertTrue(dumping);
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceResolved}.
     */
    @Test
    public void testServiceResolvedDummy() {
        stub.setServiceName("dummy");
        stub.setHostadress("TestIp");
        mClassUnderTest.serviceResolved(stub.getEvent());
        assertFalse(FtpPortHandler.getInstance().getIpDevices().containsKey("TestIp"));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceRemoved}.
     */
    @Test
    public void testServiceDummyRemoved() {
        stub.setServiceName("Dummy");
        stub.setHostadress("NewIP");
        mClassUnderTest.serviceRemoved(stub.getEvent());
        assertTrue(FtpPortHandler.getInstance().getIpDevices().containsKey("IpAdress"));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastListener#serviceRemoved}.
     */
    @Test
    public void testServiceRemoved() {
        stub.setServiceName(DNSConfiguration.AVAILABLE_ME);
        stub.setHostadress("NewIP");
        mClassUnderTest.serviceRemoved(stub.getEvent());
        assertFalse(FtpPortHandler.getInstance().getIpDevices().containsKey("IpAdress"));
    }

}
