package com.stericsson.sdk.common.log4j.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.log4j.LogLevelMatchFilter;

/**
 * @author xtomlju
 */
public class LogLevelMatchFilterTest extends TestCase {

    /** */
    @Test
    public void testSetLevelToMatch() {
        LogLevelMatchFilter filter = new LogLevelMatchFilter();
        filter.setLevelToMatch(null);
        assertEquals(null, filter.getLevelToMatch());

        filter = new LogLevelMatchFilter();
        filter.setLevelToMatch("NULL");
        assertEquals(null, filter.getLevelToMatch());
    }

}
