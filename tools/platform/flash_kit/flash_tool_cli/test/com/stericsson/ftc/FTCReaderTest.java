package com.stericsson.ftc;

import static org.junit.Assert.assertNotNull;

import java.util.Collection;

import junit.framework.Assert;

import org.junit.Test;

/**
 * Tests for Console Reader.
 * 
 * @author TSIKOR01
 * 
 */
public class FTCReaderTest {
    static boolean running = true;

    /**
     * constructor test
     */
    @Test
    public void testFTCReader() {
        assertNotNull(new FTCReader(new String[] {
            "console"}));
        assertNotNull(new FTCReader(new String[] {
            "console", "host", "localhost"}));
        assertNotNull(new FTCReader(new String[] {
            "console", "port", "8088"}));

    }

    /**
     * Test of successful parse
     */
    @Test
    public void testParseSucces() {
        boolean error = false;

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        reader.parse("adsasdasd asdasdas \"asdasd asdads bdfbdfb dfgdfgdf\"");

        Assert.assertEquals(error, false);
    }

    /**
     * Test of successful parse result
     */
    @Test
    public void testParseEquals() {

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        Collection<String> e = reader.parse("commandA arg1 arg2");

        Assert.assertTrue(e.contains("commandA"));
        Assert.assertTrue(e.contains("arg1"));
        Assert.assertTrue(e.contains("arg2"));
    }

    /**
     * Test of successful parse with additional whitespaces
     */
    @Test
    public void testWhiteSpaces() {

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        Collection<String> e = reader.parse("commandA               arg1   arg2");

        Assert.assertTrue(e.contains("commandA"));
        Assert.assertTrue(e.contains("arg1"));
        Assert.assertTrue(e.contains("arg2"));
    }

    /**
     * Test of successful parse with quotation marks
     */
    @Test
    public void testQuotationMarks() {

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        Collection<String> e =
            reader.parse("commandA  \"C:\\Program Files\"   \"C:\\Program Files2\" \"C:\\Program Files3\"");

        Assert.assertTrue(e.contains("commandA"));
        Assert.assertTrue(e.contains("C:\\Program Files"));
        Assert.assertTrue(e.contains("C:\\Program Files2"));
        Assert.assertTrue(e.contains("C:\\Program Files3"));
    }

    /**
     * Test of windows style separator parse result
     */
    @Test
    public void testWindowsSeparator() {

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        Collection<String> e = reader.parse("commandA C:\\work arg2");

        Assert.assertTrue(e.contains("commandA"));
        Assert.assertTrue(e.contains("C:\\work"));
        Assert.assertTrue(e.contains("arg2"));
    }

    /**
     * Test of successful parse result
     */
    @Test
    public void testLinuxSeparator() {

        FTCReader reader = new FTCReader(new String[] {
            "console"});
        Collection<String> e = reader.parse("commandA /etc/work arg2");

        Assert.assertTrue(e.contains("commandA"));
        Assert.assertTrue(e.contains("/etc/work"));
        Assert.assertTrue(e.contains("arg2"));
    }

    /**
     * Test of null length
     */
    @Test
    public void testNullLength() {
        FTCReader reader = new FTCReader(new String[] {
            "console"});

        Assert.assertEquals(reader.parse(null), null);
    }

    /**
     * Test of parse Reading
     */
    @Test
    public void testRead() {
        boolean success = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});

        try {
            success = reader.read("LS " + System.getProperty("user.dir"));
        } catch (Exception e) {
            e.printStackTrace();
        }

        Assert.assertEquals(success, true);
    }

    /**
     * Test of wrong command names
     */
    @Test
    public void testWrongCommandName() {
        boolean exception = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});
        try {
            exception = reader.read("IAMNOTEXISTANTFOLDER");
        } catch (Exception e1) {
            exception = true;
        }
        Assert.assertEquals(exception, true);
    }

    /**
     * Test of wrong parameters exception
     */
    @Test
    public void testWrongParametersException() {
        boolean exception = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});
        try {
            exception = reader.read("CP ONLY_ONE_OPTION");
        } catch (Exception e1) {
            exception = true;
        }
        Assert.assertEquals(exception, true);
    }

    /**
     * Test of exit message
     */
    @Test
    public void testExit() {
        boolean read = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});

        try {
            read = reader.read("EXIT");
        } catch (Exception e) {
            e.printStackTrace();
            read = true;
        }

        Assert.assertEquals(read, false);
    }

    /**
     * Test of null input
     */
    @Test
    public void testNullInput() {
        boolean exception = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});
        try {
            exception = reader.read(null);
        } catch (Exception e1) {
            exception = true;
        }
        Assert.assertEquals(exception, true);
    }

    /**
     * Test of connection exception
     */
    @Test
    public void testConnectionException() {
        boolean exception = false;
        FTCReader reader = new FTCReader(new String[] {
            "console"});
        try {
            exception = reader.read("cp usb0-\\flash0 C:\\work");
        } catch (Exception e1) {
            exception = true;
        }
        Assert.assertEquals(exception, true);

    }

}
