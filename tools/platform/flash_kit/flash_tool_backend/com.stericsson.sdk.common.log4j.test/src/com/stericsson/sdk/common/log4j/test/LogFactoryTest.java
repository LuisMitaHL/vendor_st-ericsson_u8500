package com.stericsson.sdk.common.log4j.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.log4j.LogFactory;

/**
 * @author xtomlju
 */
public class LogFactoryTest extends TestCase {

    /***/
    @Test
    public void testSyslogCreator() {
        try {
            LogFactory.sysLogCreator(2000);
            LogFactory.sysLogCreator(7001);
        } catch (Exception e) {
            fail();
        }

    }

}
