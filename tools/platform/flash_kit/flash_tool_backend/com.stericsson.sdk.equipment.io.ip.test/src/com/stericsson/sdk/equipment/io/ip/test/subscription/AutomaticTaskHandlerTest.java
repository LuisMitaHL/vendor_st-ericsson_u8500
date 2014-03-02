/**
 * 
 */
package com.stericsson.sdk.equipment.io.ip.test.subscription;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortProvider;
import com.stericsson.sdk.equipment.io.ip.subscription.AutomaticTaskHandler;
import com.stericsson.sdk.equipment.io.ip.subscription.DumpEvent;

/**
 * @author eolabor
 * 
 */
public class AutomaticTaskHandlerTest extends TestCase {

    private AutomaticTaskHandler mClassUnderTest;

    private FtpPortProvider portprovider;

    private DumpEvent dumpevent;

    /**
     * {@inheritDoc}
     */
    @Before
    public void setUp() throws Exception {
        mClassUnderTest = AutomaticTaskHandler.getInstance();
        portprovider = new FtpPortProvider("PortA");
        FtpPortHandler.getInstance().getIpDevices().put("PortA", portprovider);
        FtpPortHandler.getInstance().setAutoDownload(true);
        dumpevent = new DumpEvent("PortA", "dummyPath");
    }

    /**
     * {@inheritDoc}
     */
    @After
    public void tearDown() throws Exception {
        mClassUnderTest = null;
    }

    /**
     * Test Case
     */
    @Test
    public void testDumpEvent() {
        mClassUnderTest.dumpingMEdetected(dumpevent);
        // Waiting for the task to complete.
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        assertTrue(mClassUnderTest.getResultCode() == 0);
    }

    /**
     * Test Case
     */
    @Test
    public void testDumpEventNoEquipment() {
        FtpPortProvider portproviderDummy = new FtpPortProvider("UnKnown");
        FtpPortHandler.getInstance().getIpDevices().put("UnKnown", portproviderDummy);
        DumpEvent dumpEventDummy = new DumpEvent("UnKnown", "dummyPath");
        mClassUnderTest.dumpingMEdetected(dumpEventDummy);
        // Waiting for the task to complete.
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        assertTrue(mClassUnderTest.getResultCode() == -1);
    }

}
