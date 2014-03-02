/**
 * 
 */
package com.stericsson.ftc.command;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

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
import com.stericsson.sdk.brp.ServerResponse;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class CDCommandTest {

    /** */
    private static final String ME = "USB0";

    /** */
    private static final String PATH = "/flash0/cabs0:/afs";

    private static int port = 8088;

    private static final String HOST = "localhost";

    private CDCommand command;

    private ArrayList<String> params;

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
     * @throws Exception
     */
    @Before
    public void setUp() {
        context = new FTCApplicationContext(HOST, port);
        context.setWorkingDirectory(new FTCPath(ME, PATH));
        command = new CDCommand();
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
     * Test method for {@link com.stericsson.ftc.command.CDCommand#execute()}.
     * 
     * @throws Exception
     *             on problems with sending data to backend
     */
    @Test
    public void testExecute() throws Exception {
        localServer.addDummyCommunication("");// me exist
        localServer.addDummyCommunication("");// folder exist
        assertTrue(command.setParameters(params));
        command.execute();
        assertEquals(ME, context.getWorkingDirectory().getPrefix());
        assertEquals(PATH, context.getWorkingDirectory().getSuffix());

        String dir = "af";
        params.add(dir);
        localServer.addDummyCommunication("");// ME exist
        localServer.addDummyCommunication("");// folder exist
        assertTrue(command.setParameters(params));
        command.execute();
        assertEquals(ME, context.getWorkingDirectory().getPrefix());
        assertEquals(PATH + "/" + dir, context.getWorkingDirectory().getSuffix());
    }

    // /**
    // * Test method for {@link com.stericsson.ftc.command.CDCommand#execute()}.
    // *
    // * @throws Exception
    // * on problems with sending data to backend
    // */
    // @Test
    // public void testExecuteNoExist() throws Exception {
    // String dir = "DirA";
    // params.add(dir);
    // assertTrue(command.setParameters(params));
    // localServer.addDummyCommunication("OK");
    // localServer.addDummyCommunication(ServerResponse.ERROR);
    // command.execute();
    // assertEquals(ME, context.getWorkingDirectory().getPrefix());
    // assertEquals(PATH, context.getWorkingDirectory().getSuffix());
    // }
    //
    // /**
    // * Test method for {@link com.stericsson.ftc.command.CDCommand#execute()}.
    // *
    // * @throws Exception
    // * on problems with sending data to backend
    // */
    // @Test
    // public void testExecuteNoDir() throws Exception {
    // File f = File.createTempFile("test", "cli");
    // f.deleteOnExit();
    // params.add(f.getName());
    // context.setWorkingDirectory(new FTCPath("PC", f.getParentFile().getAbsolutePath()));
    // assertTrue(command.setParameters(params));
    // command.execute();
    // assertEquals("PC", context.getWorkingDirectory().getPrefix());
    // assertEquals(f.getParentFile().getAbsolutePath(), context.getWorkingDirectory().getSuffix());
    // }
    //
    // /**
    // * Test method for {@link com.stericsson.ftc.command.CDCommand#execute()}.
    // *
    // * @throws Exception
    // * on problems with sending data to backend
    // */
    // @Test
    // public void testExecutePCdir() throws Exception {
    // File f = File.createTempFile("test", "cli");
    // f.deleteOnExit();
    // f = f.getParentFile();
    // assertNotNull(f);
    // String parent = f.getAbsolutePath();
    // context.setWorkingDirectory(new FTCPath("PC", parent));
    // assertTrue(command.setParameters(params));
    // command.execute();
    // assertEquals("PC", context.getWorkingDirectory().getPrefix());
    // assertEquals(parent.replaceAll("\\\\", "/"), context.getWorkingDirectory().getSuffix());
    // }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CDCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParameters() throws Exception {
        localServer.addDummyCommunication("");// me exist
        localServer.addDummyCommunication("");// folder exist
        assertTrue(command.setParameters(params));
        params.add("xxx");
        localServer.addDummyCommunication("");// me exist
        localServer.addDummyCommunication("");// folder exist
        assertTrue(command.setParameters(params));
        params.add("vvv");
        assertFalse(command.setParameters(params));

        params.clear();
        File f = File.createTempFile("test", "cli");
        f.deleteOnExit();
        params.add("PC-" + f.getParent());
        assertTrue(command.setParameters(params));
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.command.CDCommand#setParameters(java.util.Collection)}.
     * 
     * @throws Exception .
     */
    @Test
    public void testSetParametersNoDir() throws Exception {
        params.add("xxx");
        localServer.addDummyCommunication("");// me exist
        localServer.addDummyCommunication(ServerResponse.ERROR);// folder exist
        assertFalse(command.setParameters(params));
        File f = File.createTempFile("test", "cli");
        f.deleteOnExit();
        params.clear();
        params.add("PC-" + f.getAbsolutePath());
        assertFalse(command.setParameters(params));
    }

    /**
     * Test method for {@link com.stericsson.ftc.command.CDCommand#printHelp()}.
     */
    @Test
    public void testPrintHelp() {
        // only coverage help
        command.printHelp();
    }

}
