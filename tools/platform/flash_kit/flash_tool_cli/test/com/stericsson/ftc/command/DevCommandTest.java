/**
 * 
 */
package com.stericsson.ftc.command;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

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
public class DevCommandTest {

    private FTCApplicationContext context;

    private DevCommand command;

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
     * @throws java.lang.Exception
     */
    @Before
    public void setUp(){
        context = new FTCApplicationContext(HOST, port);
        context.setWorkingDirectory(new FTCPath(PC, PATH));
        command = new DevCommand();
        command.setContext(context);
        params = new ArrayList<String>();
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown(){
        localServer.test();
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.DevCommand#setParameters(java.util.Collection)}.
     */
    @Test
    public void testSetParametersCount() {
     // only coverage help
        assertTrue(command.setParameters(params));
        params.add("param1");
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for {@link com.stericsson.ftc.command.DevCommand#printHelp()}.
     */
    @Test
    public void testPrintHelp() {
        // only coverage help
        command.printHelp();
    }

}
