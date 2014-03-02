package com.stericsson.ftc;

import junit.framework.Assert;

import org.junit.Test;

/**
 * Test for FTCCommand Factory
 * 
 * @author TSIKOR01
 * 
 */
public class FTCCommandFactoryTest {
    /**
     * Successful creation of command
     */
    @Test
    public void testCreateCommand() {
        Assert.assertNotNull(FTCCommandFactory.createCommand("LS"));
    }

    /**
     * Unsuccessful creation of command
     */
    @Test
    public void testCreateCommandFail() {
        Assert.assertEquals(FTCCommandFactory.createCommand("Stupid_nonexisting_command"), null);
    }
}
