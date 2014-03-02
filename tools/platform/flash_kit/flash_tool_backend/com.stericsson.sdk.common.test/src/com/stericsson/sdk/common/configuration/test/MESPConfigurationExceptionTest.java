package com.stericsson.sdk.common.configuration.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;

/**
 * @author xtomlju
 */
public class MESPConfigurationExceptionTest extends TestCase{

    private static final String EXCEPTION_MESSAGE = "THIS IS A TEST";

    /**
     * 
     */
    @Test
    public void testMESPConfigurationException() {
        try {
            throw new MESPConfigurationException(EXCEPTION_MESSAGE);
        } catch (MESPConfigurationException e) {
            assertEquals(EXCEPTION_MESSAGE, e.getMessage());
        } catch (Exception e) {
            fail("Wrong exception thrown: " + e.getMessage());
        }
    }

}
