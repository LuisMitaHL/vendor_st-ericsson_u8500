package com.stericsson.sdk.common.log4j.test;

import org.apache.log4j.Level;
import org.junit.Test;

import com.stericsson.sdk.common.log4j.LogLevel;

import junit.framework.TestCase;

/**
 * @author xtomlju
 */
public class LogLevelTest extends TestCase {

    /**
     * test for: toLevel(int) toLevel(String) toLevel(String, Level)
     * 
     */
    @Test
    public void testToLevel() {

        assertEquals(LogLevel.SYSLOG, LogLevel.toLevel(LogLevel.SYSLOG_INT));
        assertEquals(Level.DEBUG, LogLevel.toLevel(LogLevel.DEBUG_INT));
        assertEquals(LogLevel.SYSLOG, LogLevel.toLevel(LogLevel.SYSLOG.toString()));
        assertEquals(LogLevel.SYSLOG, LogLevel.toLevel(null, LogLevel.SYSLOG));
        assertEquals(LogLevel.ERROR, LogLevel.toLevel(LogLevel.ERROR.toString(), LogLevel.SYSLOG));
        assertEquals(LogLevel.SYSLOG, LogLevel.toLevel(LogLevel.SYSLOG.toString(), LogLevel.SYSLOG));

    }

}
