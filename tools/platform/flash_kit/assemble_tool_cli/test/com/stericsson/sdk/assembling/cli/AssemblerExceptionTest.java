package com.stericsson.sdk.assembling.cli;

import static org.junit.Assert.*;

import org.junit.Test;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.AssemblerSettingsException;

/**
 * 
 * @author xolabju
 * 
 */
public class AssemblerExceptionTest {

    /**
     * 
     */
    @Test
    public void testAssemblerException() {
        String message = "message";
        Exception cause = new Exception(message);
        int errorCode = 1;

        try {
            throw new AssemblerException(message);
        } catch (AssemblerException ae) {
            assertTrue(ae.getMessage().endsWith(message));
        }
        try {
            throw new AssemblerException(message, errorCode);
        } catch (AssemblerException ae) {
            assertTrue(ae.getMessage().endsWith(message));
            assertEquals(errorCode, ae.getResultValue());
        }
        try {
            throw new AssemblerException(cause, errorCode);
        } catch (AssemblerException ae) {
            assertEquals(cause, ae.getCause());
            assertTrue(ae.getMessage().endsWith(message));
            assertEquals(errorCode, ae.getResultValue());
        }
    }

    /**
     * 
     */
    @Test
    public void testAssemblerSettingsException() {
        String message = "message";
        Exception cause = new Exception(message);
        int errorCode = 1;

        try {
            throw new AssemblerSettingsException(message);
        } catch (AssemblerSettingsException ae) {
            assertTrue(ae.getMessage().endsWith(message));
        }
        try {
            throw new AssemblerSettingsException(message, errorCode);
        } catch (AssemblerSettingsException ae) {
            assertTrue(ae.getMessage().endsWith(message));
            assertEquals(errorCode, ae.getResultValue());
        }
        try {
            throw new AssemblerSettingsException(cause, errorCode);
        } catch (AssemblerSettingsException ae) {
            assertEquals(cause, ae.getCause());
            assertTrue(ae.getMessage().endsWith(message));
            assertEquals(errorCode, ae.getResultValue());
        }
    }

}
