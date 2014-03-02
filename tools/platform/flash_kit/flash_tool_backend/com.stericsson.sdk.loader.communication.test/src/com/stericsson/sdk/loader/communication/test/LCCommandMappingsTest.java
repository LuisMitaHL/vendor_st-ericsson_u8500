package com.stericsson.sdk.loader.communication.test;

import java.nio.ByteBuffer;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * test case for LCCommandMappings class
 * 
 * @author xdancho
 * 
 */
public class LCCommandMappingsTest extends TestCase {

    static ByteBuffer bb = null;

    Object[] test;

    String path;

    byte[] src;

    byte[] dest;

    boolean bulkSrc;

    boolean bulkDest;

    byte[] srcLen;

    byte[] destLen;

    /**
     * test to get the descriptions
     */
    @Test
    public void testCommandDescription() {

        for (int i = 1; i < LCCommandMappings.COMMAND_SYSTEM.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_SYSTEM, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_FLASH.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_FLASH, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_FILE_SYSTEM.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_OTP.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_OTP, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_PARAMETER_STORAGE.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_SECURITY.length; i++) {
            assertNotNull(LCCommandMappings.getDescription(LCDefinitions.COMMAND_GROUP_SECURITY, i));
        }

    }

    /**
     * test to get the command strings
     */
    @Test
    public void testCommandString() {
        for (int i = 1; i < LCCommandMappings.COMMAND_SYSTEM.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_SYSTEM, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_FLASH.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_FLASH, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_FILE_SYSTEM.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_OTP.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_OTP, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_PARAMETER_STORAGE.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, i));
        }

        for (int i = 1; i < LCCommandMappings.COMMAND_SECURITY.length; i++) {
            assertNotNull(LCCommandMappings.getCommandString(LCDefinitions.COMMAND_GROUP_SECURITY, i));
        }
    }

    /**
     * test to get methods for system group
     */
    @Test
    public void testSystemMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_LOADER_STARTUP_STATUS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_LOADER_STARTUP_STATUS));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_CHANGE_BAUDRATE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_CHANGE_BAUDRATE));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_REBOOT, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_REBOOT));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_SHUTDOWN, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_SHUTDOWN));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_SUPPORTED_COMMANDS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_SUPPORTED_COMMANDS));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_EXECUTE_SOFTWARE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_EXECUTE_SOFTWARE));

        assertEquals(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SYSTEM, LCDefinitions.COMMAND_SYSTEM_AUTHENTICATE));
    }

    /**
     * test to get methods for flash group
     */
    @Test
    public void testFlashMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_FLASH_PROCESS_FILE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FLASH, LCDefinitions.COMMAND_FLASH_PROCESS_FILE));
        assertEquals(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FLASH, LCDefinitions.COMMAND_FLASH_LIST_DEVICES));
        assertEquals(LCDefinitions.METHOD_NAME_FLASH_DUMP_AREA, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FLASH, LCDefinitions.COMMAND_FLASH_DUMP_AREA));
        assertEquals(LCDefinitions.METHOD_NAME_FLASH_ERASE_AREA, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FLASH, LCDefinitions.COMMAND_FLASH_ERASE_AREA));
        assertEquals(LCDefinitions.METHOD_NAME_FLASH_SET_ENHANCED_AREA, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FLASH, LCDefinitions.COMMAND_FLASH_SET_ENHANCED_AREA));
    }

    /**
     * test to get methods for file system group
     */
    @Test
    public void testFSMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_FS_VOLUME_PROPERTIES, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_VOLUME_PROPERTIES));

        assertEquals(LCDefinitions.METHOD_NAME_FS_FORMAT_VOLUME, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_FORMAT_VOLUME));

        assertEquals(LCDefinitions.METHOD_NAME_FS_LIST_DIRECTORY, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_LIST_DIRECTORY));

        assertEquals(LCDefinitions.METHOD_NAME_FS_MOVE_FILE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_MOVE_FILE));

        assertEquals(LCDefinitions.METHOD_NAME_FS_DELETE_FILE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_DELETE_FILE));

        assertEquals(LCDefinitions.METHOD_NAME_FS_COPY_FILE, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_COPY_FILE));

        assertEquals(LCDefinitions.METHOD_NAME_FS_CREATE_DIRECTORY, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_CREATE_DIRECTORY));

        assertEquals(LCDefinitions.METHOD_NAME_FS_PROPERTIES, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_PROPERTIES));

        assertEquals(LCDefinitions.METHOD_NAME_FS_CHANGE_ACCESS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_FILE_SYSTEM, LCDefinitions.COMMAND_FS_CHANGE_ACCESS));
    }

    /**
     * test to get methods for OTP group
     */
    @Test
    public void testOTPMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_OTP_READ_BITS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_OTP, LCDefinitions.COMMAND_OTP_READ_BITS));

        assertEquals(LCDefinitions.METHOD_NAME_OTP_WRITE_BITS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_OTP, LCDefinitions.COMMAND_OTP_WRITE_BITS));

        assertEquals(LCDefinitions.METHOD_NAME_OTP_WRITE_AND_LOCK_BITS, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_OTP, LCDefinitions.COMMAND_OTP_WRITE_AND_LOCK));

        assertEquals(LCDefinitions.METHOD_NAME_OTP_STORE_SECURE_OBJECT, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_OTP, LCDefinitions.COMMAND_OTP_STORE_SECURE_OBJECT));
    }

    /**
     * test to get methods for parameter storage group
     */
    @Test
    public void testPSMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_UNIT, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, LCDefinitions.COMMAND_PS_READ_GLOBAL_DATA_UNIT));

        assertEquals(LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_UNIT, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, LCDefinitions.COMMAND_PS_WRITE_GLOBAL_DATA_UNIT));

        assertEquals(LCDefinitions.METHOD_NAME_PS_READ_GLOBAL_DATA_SET, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, LCDefinitions.COMMAND_PS_READ_GLOBAL_DATA_SET));

        assertEquals(LCDefinitions.METHOD_NAME_PS_WRITE_GLOBAL_DATA_SET, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, LCDefinitions.COMMAND_PS_WRITE_GLOBAL_DATA_SET));

        assertEquals(LCDefinitions.METHOD_NAME_PS_ERASE_GLOBAL_DATA_SET, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_PARAMETER_STORAGE, LCDefinitions.COMMAND_PS_ERASE_GLOBAL_DATA_SET));
    }

    /**
     * test to get methods for security group
     */
    @Test
    public void testSecurityMethods() {
        assertEquals(LCDefinitions.METHOD_NAME_SECURITY_SET_DOMAIN, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SECURITY, LCDefinitions.COMMAND_SECURITY_SET_DOMAIN));

        assertEquals(LCDefinitions.METHOD_NAME_SECURITY_GET_DOMAIN, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SECURITY, LCDefinitions.COMMAND_SECURITY_GET_DOMAIN));

        assertEquals(LCDefinitions.METHOD_NAME_SECURITY_GET_PROPERTY, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SECURITY, LCDefinitions.COMMAND_SECURITY_GET_PROPERTY));

        assertEquals(LCDefinitions.METHOD_NAME_SECURITY_SET_PROPERTY, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SECURITY, LCDefinitions.COMMAND_SECURITY_SET_PROPERTY));

        assertEquals(LCDefinitions.METHOD_NAME_SECURITY_BIND_PROPERTIES, LCCommandMappings.getMethodString(
            LCDefinitions.COMMAND_GROUP_SECURITY, LCDefinitions.COMMAND_SECURITY_BIND_PROPERTIES));

    }
}
