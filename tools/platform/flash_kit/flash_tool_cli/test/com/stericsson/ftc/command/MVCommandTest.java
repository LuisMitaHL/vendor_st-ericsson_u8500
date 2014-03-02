/**
 * 
 */
package com.stericsson.ftc.command;

import static org.junit.Assert.*;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import com.stericsson.ftc.FTCApplicationContext;
import com.stericsson.ftc.FTCPath;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class MVCommandTest {

    private FTCApplicationContext context;

    private MVCommand command;

    private ArrayList<String> params;

    private static DummyBackend localServer;

    /** */
    private static final String PC = "PC";

    /** */
    private static final String PATH = "/";

    private static int port = 8088;

    private static final String HOST = "localhost";

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
        context = new FTCApplicationContext(HOST, port);
        context.setWorkingDirectory(new FTCPath(PC, PATH));
        command = new MVCommand();
        command.setContext(context);
        params = new ArrayList<String>();
    }

    /**
     * @throws java.lang.Exception .
     */
    @After
    public void tearDown() throws Exception {
        localServer.test();
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.MVCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersCount() throws Exception {
        File f = File.createTempFile("test", "cli");
        f.deleteOnExit();
        assertFalse(command.setParameters(params));
        params.add("COM1-" + f.getAbsolutePath());
        assertFalse(command.setParameters(params));
        params.add("COM1-/flash0:/target");
        localServer.addDummyCommunication("OK");//COM1 is connected equipment
        localServer.addDummyCommunication("OK");//COM1 is connected equipment
        localServer.addDummyCommunication("OK");//path exist on equipment.
        assertTrue(command.setParameters(params));
        params.add("from2");
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.MVCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersMeMe() throws Exception {
        params.add("COM1-/flash0/x");
        params.add("COM1-/flash0/y");
        localServer.addDummyCommunication("OK");//COM1 is connected
        localServer.addDummyCommunication("OK");//COM1 is connected
        localServer.addDummyCommunication("OK");//path exist
        assertTrue(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CPCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersMePc() throws Exception {
        params.add("COM1-/flash0/x");
        params.add("PC-/flash0/y");
        localServer.addDummyCommunication("OK");//COM1 is connected
        localServer.addDummyCommunication("OK");///flash0/x exist
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CPCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersPcMe() throws Exception {
        File f=File.createTempFile("test", "cli");
        f.deleteOnExit();
        params.add("PC-"+f.getAbsolutePath());
        params.add("COM1-/flash0/x");
        localServer.addDummyCommunication("OK");//COM1 is connected
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CPCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersPcPc() throws Exception {
        File f=File.createTempFile("test", "cli");
        f.deleteOnExit();
        params.add("PC-"+f.getAbsolutePath());
        params.add("PC-/flash0/x");
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CPCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersSourceNotExist() throws Exception {
        File f = File.createTempFile("test", "cli");
        f.delete();
        params.add("PC-" + f.getAbsolutePath());
        params.add("to");
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for {@link com.stericsson.ftc.command.MVCommand#printHelp()}.
     */
    @Test
    public void testPrintHelp() {
        // only coverage help
        command.printHelp();
    }

}
