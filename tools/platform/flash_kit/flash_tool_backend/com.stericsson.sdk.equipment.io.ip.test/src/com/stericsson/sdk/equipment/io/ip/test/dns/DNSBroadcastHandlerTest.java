/**
 *
 */
package com.stericsson.sdk.equipment.io.ip.test.dns;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;
import com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastHandler;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.test.internal.JmDNSStub;

/**
 * @author eolabor
 *
 */
public class DNSBroadcastHandlerTest extends TestCase {

    private DNSBroadcastHandler mClassUnderTest;

    private JmDNSStub stub;

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {
        mClassUnderTest = new DNSBroadcastHandler();
        stub = new JmDNSStub();
        stub.setHostadress("NewIP");
    }

    /**
     * {@inheritDoc}
     *
     */
    @After
    public void tearDown() throws Exception {
        mClassUnderTest.close();
        mClassUnderTest = null;
        stub = null;
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastHandler#addAvailableServices()}.
     */
    @Test
    public void testAddAvailableServices() {
        mClassUnderTest.setJmDNS(stub.getJmDNS());
        stub.setServiceName(DNSConfiguration.AVAILABLE_ME);
        mClassUnderTest.addAvailableServices();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        assertTrue(FtpPortHandler.getInstance().getIpDevices().containsKey("NewIP"));
    }

    /**
     * Test method for
     * {@link com.stericsson.sdk.equipment.io.ip.dns.DNSBroadcastHandler#addAvailableServices()}.
     */
    @Test
    public void testAddAvailableServicesDumping() {
        mClassUnderTest.setJmDNS(stub.getJmDNS());
        stub.setServiceName(DNSConfiguration.DUMPING_ME);
        mClassUnderTest.addAvailableServices();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        assertTrue(FtpPortHandler.getInstance().getIpDevices().containsKey("NewIP"));
    }

    /**
     * Tests networkinterface poll timer. Adds jmdns to network interface during first poll and
     * removes it next poll.
     *
     * @throws InterruptedException
     *             If interrupted
     */
    @Test
    public void testStartPollingTimer() throws InterruptedException {
        DNSBroadcastHandler classUnderTest = new DNSBroadcastHandler();
        classUnderTest.startInterfacePoll();
        Thread.sleep(1000);
        assertNotNull(classUnderTest.getJmDNS());
        Thread.sleep(15000);
        assertNull(classUnderTest.getJmDNS());
        classUnderTest = null;
    }

    /**
     * Tests runtime exception. If Jmdns is not null. No new instance of jMDNS should be created.
     *
     * @throws InterruptedException
     *             If interrupted
     */
    @Test
    public void testStartPollingTimerRuntimeException() throws InterruptedException {
        DNSBroadcastHandler classUnderTest = new DNSBroadcastHandler();
        classUnderTest.setJmDNS(stub.getJmDNS());
        classUnderTest.startInterfacePoll();
        Thread.sleep(1000);
        assertEquals(stub.getJmDNS(), classUnderTest.getJmDNS());
    }
}
