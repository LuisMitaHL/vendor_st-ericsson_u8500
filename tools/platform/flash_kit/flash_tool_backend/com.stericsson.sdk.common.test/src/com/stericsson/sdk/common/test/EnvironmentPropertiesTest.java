package com.stericsson.sdk.common.test;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.EnvironmentProperties;

/**
 * Class testing the functionality of EnvironmentProperties class.
 * 
 * @author xadazim
 * 
 */
public class EnvironmentPropertiesTest extends TestCase {

    /**
     * Tests isWindows() isUnix() and isMac() methods.
     */
    @Test
    public void testSystemDetectionMethods() {
        // One and only one method should return true.
        int howMany = 0;
        if (EnvironmentProperties.isMac()) {
            howMany++;
        }

        if (EnvironmentProperties.isUnix()) {
            howMany++;
        }

        if (EnvironmentProperties.isWindows()) {
            howMany++;
        }
        assertEquals(1, howMany);
    }

}
