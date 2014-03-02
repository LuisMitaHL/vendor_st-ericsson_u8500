/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons.log4j;


import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.apache.log4j.Level;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Main;

/**
 * @author qfrelun
 *
 */
public class TestLogTracker {
    /** The logger */
    public final static Logger logger = LoggerFactory
            .getLogger(TestLogTracker.class);

    /** The LogTracker under test.
     */
    private LogTracker t;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        t = new LogTracker();
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
        t.close();
        t = null;
    }

    /**
     *
     */
    @Test
    public final void testLoggerTrace1FilteredByConfig() {
        logger.trace("trace message"); // This should not be shown in config by default
        t.assertNotEvent(Level.TRACE);
        t.assertNotEvent(Level.DEBUG);
        t.assertNotEvent(Level.INFO);
        t.assertNotEvent(Level.WARN);
        t.assertNotEvent(Level.ERROR);
        t.assertNotEvent(Level.FATAL);
    }

    /**
     *
     */
    @Test
    public final void testLoggerDebug() {

        logger.debug("debug message");
        t.assertNotEvent(Level.TRACE);
        t.assertEvent(Level.DEBUG);
        t.assertNotEvent(Level.INFO);
        t.assertNotEvent(Level.WARN);
        t.assertNotEvent(Level.ERROR);
        t.assertNotEvent(Level.FATAL);
    }

    /**
     *
     */
    @Test(expected=LogEventWasLoggedException.class)
    public final void testLoggerDebugNegative1() {

        logger.debug("debug message");
        t.assertNotEvent(Level.DEBUG);
    }

    /**
     *
     */
    @Test(expected=LogEventNotLoggedException.class)
    public final void testLoggerDebugNegative2() {

        logger.debug("debug message");
        t.assertEvent(Level.WARN);
    }

    /**
     *
     */
    @Test
    public final void testLoggerClass() {

        logger.debug("debug message");
        t.assertNotEvent(Main.class);
        t.assertEvent(TestLogTracker.class);
    }

    /**
     *
     */
    @Test(expected=LogEventWasLoggedException.class)
    public final void testLoggerClassNegative1() {

        logger.debug("debug message");
        t.assertNotEvent(TestLogTracker.class);
    }

    /**
     *
     */
    @Test(expected=LogEventNotLoggedException.class)
    public final void testLoggerClassNegative2() {

        logger.debug("debug message");
        t.assertEvent(Main.class);
    }


    /**
     *
     */
    @Test
    public final void testLoggerMessage() {

        logger.debug("debug something with a message");
        t.assertNotEvent("This message has not been logged");
        t.assertEvent("debug {} with a message");
    }

    /**
     *
     */
    @Test(expected=LogEventNotLoggedException.class)
    public final void testLoggerMessageNegative1() {

        logger.debug("debug something with a message");
        t.assertEvent("This message has not been logged so it should cause an exception");
    }

    /**
     *
     */
    @Test(expected=LogEventWasLoggedException.class)
    public final void testLoggerMessageNegative2() {

        logger.debug("debug something with a message");
        t.assertNotEvent("debug {} with a message"); // This should cause an exception
    }

    /**
     *
     */
    @Test
    public final void testMatches() {
        assertTrue(t.matches("a small string", "a small string"));
        assertFalse(t.matches("a small string", "mismatch"));

        assertTrue(t.matches("a small 'extra' string", "a small '{}' string"));
        assertTrue(t.matches("a small 'but not so small' string", "a small '{}' string"));
        assertTrue(t.matches("a small extra string", "a small {} string"));
        assertTrue(t.matches("a small but not so small string", "a small {} string"));

        assertFalse(t.matches("a small 'extra' string", "a small '{}' mismatch"));
        assertFalse(t.matches("a small extra string", "a small {} mismatch"));
        assertFalse(t.matches("a small 'but not so small' string", "a small '{}' mismatch"));
        assertFalse(t.matches("a small but not so small string", "a small {} mismatch"));

        assertTrue(t.matches("a small 'extra' string 'again' Wohoo!", "a small '{}' string '{}' Wohoo!"));
        assertTrue(t.matches("a small 'but not so small' string 'once again' Wohoo!", "a small '{}' string '{}' Wohoo!"));
        assertTrue(t.matches("a small extra string again Wohoo!", "a small {} string {} Wohoo!"));
        assertTrue(t.matches("a small but not so small string once again Wohoo!", "a small {} string {} Wohoo!"));

        assertFalse(t.matches("a small 'extra' string 'again' Wohoo!", "a small '{}' string '{}' Mismatch!"));
        assertFalse(t.matches("a small 'but not so small' string 'once again' Wohoo!", "a small '{}' string '{}' Mismatch!"));
        assertFalse(t.matches("a small extra string again Wohoo!", "a small {} string {} Mismatch!"));
        assertFalse(t.matches("a small but not so small string once again Wohoo!", "a small {} string {} Mismatch!"));

        assertFalse(t.matches("a small 'extra' m 'again' Wohoo!", "a small '{}' mismatch '{}' Wohoo!"));
        assertFalse(t.matches("a small 'but not so small' string 'once again' Wohoo!", "a small '{}' mismatch '{}' Wohoo!"));
        assertFalse(t.matches("a small extra string again Wohoo!", "a small {} mismatch {} Wohoo!"));
        assertFalse(t.matches("a small but not so small string once again Wohoo!", "a small {} mismatch {} Wohoo!"));

        assertFalse(t.matches("a small but not so small string with a mismatch", "a small {} string"));
        assertFalse(t.matches("mismatch a small but not so small string", "a small {} string"));
    }
    /**
     *
     */
    @Test
    public final void testLoggerWarnWarnDebugWarn() {
        logger.warn("warn message 1");
        logger.warn("warn message 2");
        logger.debug("debug message");
        logger.warn("warn message 3");

        t.assertNotEvent(Level.TRACE);
        t.assertEvent(Level.DEBUG);
        t.assertNotEvent(Level.INFO);
        t.assertEvent(Level.WARN);
        t.assertNotEvent(Level.ERROR);
        t.assertNotEvent(Level.FATAL);
    }

    /**
     *
     */
    @Test
    public final void testLogger() {
        logger.trace("trace message"); // Not shown
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warn("warning message");
        logger.error("error message");
    }
}
