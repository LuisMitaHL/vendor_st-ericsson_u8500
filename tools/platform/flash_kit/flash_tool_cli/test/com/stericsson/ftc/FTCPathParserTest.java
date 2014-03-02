/**
 * 
 */
package com.stericsson.ftc;

import static org.junit.Assert.*;

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
public class FTCPathParserTest {

    private static final String DEVICE = "PC";

    private static final String ME = "COM1";

    private static final String PATH = "/flash0/cabs0:/afs";

    private FTCApplicationContext context;

    private static int port = 8088;

    private static final String HOST = "localhost";

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
        context = new FTCApplicationContext(HOST, port);
        context.setWorkingDirectory(new FTCPath(DEVICE, PATH));
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
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseEmpty() {
        FTCPath path = FTCPathParser.parse(context, "");
        assertEquals(DEVICE, path.getPrefix());
        assertEquals(PATH, path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseAbsolute() {
        FTCPath path = FTCPathParser.parse(context, "PC-/dirA/dirB");
        assertEquals("PC", path.getPrefix());
        assertEquals("/dirA/dirB", path.getSuffix());

        path = FTCPathParser.parse(context, "PC-///dirA\\\\/dirB/");
        assertEquals("PC", path.getPrefix());
        assertEquals("/dirA/dirB", path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseAbsoluteVolumeRoot() {
        FTCPath path = FTCPathParser.parse(context, "PC-C:\\");
        assertEquals("PC", path.getPrefix());
        assertEquals("C:/", path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseAbsoluteCurrentParentFolder() {
        FTCPath path = FTCPathParser.parse(context, "PC-/dirA/dirB/./dirC/dirD/dirE/.././.././..");
        assertEquals("PC", path.getPrefix());
        assertEquals("/dirA/dirB", path.getSuffix());

        path = FTCPathParser.parse(context, "PC-C:/dirA/dirB/../../../..");
        assertEquals("PC", path.getPrefix());
        assertEquals("C:/", path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseRelative() {
        FTCPath path = FTCPathParser.parse(context, "xxx");
        assertEquals(DEVICE, path.getPrefix());
        assertEquals(PATH + "/xxx", path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseRelativeFolderUp() {
        context.setWorkingDirectory(new FTCPath(DEVICE, PATH + "/dirX"));
        FTCPath path = FTCPathParser.parse(context, "..");
        assertEquals(DEVICE, path.getPrefix());
        assertEquals(PATH, path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseME() {
        context.setWorkingDirectory(new FTCPath(ME, PATH));
        localServer.addDummyCommunication("OK");
        FTCPath path = FTCPathParser.parse(context, "");
        assertEquals(ME, path.getPrefix());
        assertEquals(PATH, path.getSuffix());
    }

    /**
     * Test method for
     * {@link com.stericsson.ftc.FTCPath#parse(com.stericsson.ftc.FTCApplicationContext, java.lang.String)}
     * .
     */
    @Test
    public void testParseNoExistME() {
        context.setWorkingDirectory(new FTCPath(ME, PATH));
        localServer.addDummyCommunication(ServerResponse.ERROR);//NODEVICE equipment doesn't exist
        localServer.addDummyCommunication("");// COM1 equipment exist
        FTCPath path = FTCPathParser.parse(context, "NODEVICE-/flash0/device:/ccc");
        assertEquals(ME, path.getPrefix());
        assertEquals(PATH+"/NODEVICE-/flash0/device:/ccc", path.getSuffix());
    }
}
