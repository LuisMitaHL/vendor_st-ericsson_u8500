/**
 * 
 */
package com.stericsson.ftc;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.stericsson.ftc.command.DummyBackend;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class FTCPathTest {

    private FTCPath pcpath;

    private FTCPath mobilepath;

    private static int port = 8088;

    private static final String HOST = "localhost";

    private FTCApplicationContext context;

    private static DummyBackend localServer;

    /**
     * 
     */
    @BeforeClass
    public static void setUpBeforeClass() {
        localServer = new DummyBackend();
        localServer.startServer();
        port = localServer.getServerPort();
    }

    /**
     * @throws IOException .
     * 
     */
    @AfterClass
    public static void tearDownAfterClass() throws IOException {
        localServer.shutDown();
    }

    /**
     * @throws java.lang.Exception .
     */
    @Before
    public void setUp() throws Exception {
        mobilepath = new FTCPath("COM1", "/flash0/cabs0:/afs");
        pcpath = new FTCPath("PC", "/tmp");
        context = new FTCApplicationContext(HOST, port);
        context.setWorkingDirectory(new FTCPath("COM1", "/flash0/cabs0:/afs"));
    }

    /**
     * @throws java.lang.Exception .
     */
    @After
    public void tearDown() throws Exception {
        localServer.test();
    }

    /**
     * Test method for {@link com.stericsson.ftc.FTCPath#isOnME()}.
     */
    @Test
    public void testIsOnME() {
        assertTrue(mobilepath.isOnME());
        assertFalse(pcpath.isOnME());
    }

    /**
     * Test method for {@link com.stericsson.ftc.FTCPath#toString()}.
     */
    @Test
    public void testToString() {
        assertEquals("COM1-/flash0/cabs0:/afs", mobilepath.toString());
        assertEquals("PC-/tmp", pcpath.toString());
    }

    /**
     * Test method for {@link com.stericsson.ftc.FTCPath#getCommandString()}.
     */
    @Test
    public void testGetCommandString() {
        assertEquals("/flash0/cabs0:/afs", mobilepath.getCommandString());
        assertEquals("PC:/tmp", pcpath.getCommandString());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#exist(com.stericsson.ftc.FTCApplicationContext)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testExist() throws Exception {
        localServer.addDummyCommunication("OK");
        assertTrue(mobilepath.exist(context));
        localServer.addDummyCommunication(ServerResponse.ERROR);
        assertFalse(mobilepath.exist(context));
        assertTrue(pcpath.exist(context));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#exist(com.stericsson.ftc.FTCApplicationContext)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testIsDir() throws Exception {
        localServer.addDummyCommunication("OK");
        assertTrue(mobilepath.isDirectory(context));
        localServer.addDummyCommunication(ServerResponse.ERROR);
        assertFalse(mobilepath.isDirectory(context));
        File.createTempFile("test", "cli");
        assertTrue(pcpath.exist(context));
    }

}
