package com.stericsson.sdk.cli;

import junit.framework.Assert;

import org.junit.Test;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ServerResponse;
import com.stericsson.sdk.cli.command.BRPPrintFormatter;

/**
 * test for BRPPrintFormatter class
 * 
 * @author xdancho
 * 
 */
public class PrintFormatterTest {

    static BRPPrintFormatter formatter = BRPPrintFormatter.getInstance();

    String cmdGetConnectedEquipments = CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name();

    String cmdDisableAutoSense = CommandName.BACKEND_DISABLE_AUTO_SENSE.name();

    String cmdEnableAutoSense = CommandName.BACKEND_ENABLE_AUTO_SENSE.name();

    String cmdAutoSenseStatus = CommandName.BACKEND_GET_AUTO_SENSE_STATUS.name();

    String cmdGetCoreDumpLocation = CommandName.BACKEND_GET_CORE_DUMP_LOCATION.name();

    String cmdGetAvailableProfiles = CommandName.BACKEND_GET_AVAILABLE_PROFILES.name();

    String cmdGetActiveProfile = CommandName.BACKEND_GET_ACTIVE_PROFILE.name();

    String cmdGetEquipmentProperties = CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name();

    String cmdGetSecurityProperties = CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name();

    String cmdListDevices = CommandName.FLASH_LIST_DEVICES.name();

    static final String PRE =
        (CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS.name() + ";" + ServerResponse.SUCCESS.name() + ";");

    /**
     * test the printouts for a result with multiple values
     */
    @Test
    public void testPrint() {
        formatter.printResult(cmdGetConnectedEquipments, PRE
            + "value1;value2;value3;value4;value5;value6");
        formatter.printResult(cmdGetConnectedEquipments, PRE + "v1;v2;v3;v4;v5;v6");
        formatter.printResult(cmdGetConnectedEquipments, PRE + "v1;v2;v3;v4;v5;v6");

        formatter
            .printResult(
                cmdGetConnectedEquipments,
                PRE
                    + "value2adfadsfadf;value3;valadsfue4;value5;value2;valueadsfadfadfs3;value4;value5;value6;value1;"
                    + "vdsafadfsdfsadfasalue2;value3;value4;value5;value6;value2;valdsdfue3;value4;value5;value6");

        formatter
            .printResult(
                "test",
                "test;"
                    + ServerResponse.SUCCESS.name()
                    + ";"
                    + "valuadfafe2;value3;valudsafdfase4;value5;valuadfadfse2;value3;value4;valadfsadfue5;"
                    + "value6;value1;value2;value3;value4;valdsafdfadfue5;value6;value2;v3;valdfdsafdfue4;value5;value6;value7");

        formatter
            .printResult(
                "test2",
                "test2;"
                    + ServerResponse.SUCCESS.name()
                    + ";"
                    + "v;v;v;v;v;value2adfadsfadf;value3;valadsfue4;value5;value2;valueadsfadfadfs3;value4;"
                    + "value5;value6;value1;vdsafadfsdfsadfasalue2;value3;value4;value5;value6;value2;"
                    + "valdsdfue3;value4;value5;value6");

    }

    /**
     * test print progress
     */
    @Test
    public void testPrintProgress() {
        String progress =
            (cmdGetConnectedEquipments + AbstractCommand.DELIMITER + ServerResponse.PROGRESS.name() + AbstractCommand.DELIMITER);
        formatter.printProgress(cmdGetConnectedEquipments, progress + "50;1234");
        formatter.printProgress(cmdGetConnectedEquipments, progress + "");

        for (int i = 0; i < 24; i++) {
            formatter.printProgress(cmdGetConnectedEquipments, progress + "-1;1234");
        }

        progress =
            (cmdGetAvailableProfiles + AbstractCommand.DELIMITER + ServerResponse.PROGRESS.name() + AbstractCommand.DELIMITER);
        formatter.printProgress(cmdGetAvailableProfiles, progress + "99; ");
    }

    /**
     * test print progress
     */
    @Test
    public void testPrintProgressWithoutKbps() {
        String progress =
            (cmdGetAvailableProfiles + AbstractCommand.DELIMITER + ServerResponse.PROGRESS.name() + AbstractCommand.DELIMITER);
        formatter.printProgress(cmdGetAvailableProfiles, progress + "99; ");
    }

    /**
     * test print nak
     */
    @Test
    public void testPrintNak() {
        formatter.printNak(cmdGetConnectedEquipments);

    }

    /**
     * test print error
     */
    @Test
    public void testPrintError() {
        String error =
            (cmdGetConnectedEquipments + AbstractCommand.DELIMITER + ServerResponse.ERROR.name() + AbstractCommand.DELIMITER);
        formatter.printError(cmdGetConnectedEquipments, error + AbstractCommand.DELIMITER
            + "Error message");
        formatter.printError(cmdGetConnectedEquipments, error);
    }

    /**
     * test print get Connected equipments
     */
    @Test
    public void testPrintGetConnectedEquipments() {
        String result =
            (cmdGetConnectedEquipments + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name());
        formatter.printResult(cmdGetConnectedEquipments, result + ";DB1234;USB1;DB4567;COM2");
        formatter.printResult(cmdGetConnectedEquipments, result);
    }

    /**
     * test print auto sense on
     */
    @Test
    public void testPrintAutoSenseOn() {
        String result =
            (cmdEnableAutoSense + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name());
        formatter.printResult(cmdEnableAutoSense, result);
    }

    /**
     * test print auto sense off
     */
    @Test
    public void testPrintAutoSenseOff() {
        String result =
            (cmdEnableAutoSense + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name());
        formatter.printResult(cmdDisableAutoSense, result);
    }

    /**
     * test print get autosense status
     */
    @Test
    public void testPrintGetAutoSenseStatus() {
        String result =
            (cmdAutoSenseStatus + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name());
        formatter.printResult(cmdAutoSenseStatus, result + ";Auto sense ON");
    }

    /**
     * test print get autosense status
     */
    @Test
    public void testPrintGetCoreDumpLocation() {
        String result =
            (cmdGetCoreDumpLocation + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name());
        formatter.printResult(cmdGetCoreDumpLocation, result);
    }

    /**
     * test print get available profiles
     */
    @Test
    public void testPrintGetAvailableProfiles() {
        String result =
            (cmdGetAvailableProfiles + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name()
                + AbstractCommand.DELIMITER + "profile1;profile2;profile3");
        formatter.printResult(cmdGetAvailableProfiles, result);
    }

    /**
     * test print get active profile
     */
    @Test
    public void testPrintGetActiveProfile() {
        String result =
            (cmdGetActiveProfile + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name()
                + AbstractCommand.DELIMITER + "active_profile_alias");
        formatter.printResult(cmdGetActiveProfile, result);
    }

    /**
     * test print list devices
     */
    @Test
    public void testListDevices() {
        String result =
            (cmdListDevices + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name()
                + AbstractCommand.DELIMITER + "device_list");
        formatter.printResult(cmdListDevices, result);
    }

    /**
     * test print GetSecurityProperties
     */
    @Test
    public void testGetSecurityProperties() {
        String result =
            (cmdGetSecurityProperties + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name()
                + AbstractCommand.DELIMITER + "security_properties");
        formatter.printResult(cmdGetSecurityProperties, result);
    }

    /**
     * test print GetSecurityProperties
     */
    @Test
    public void testGetEquipmentProperties() {
        String result =
            (cmdGetEquipmentProperties + AbstractCommand.DELIMITER + ServerResponse.SUCCESS.name()
                + AbstractCommand.DELIMITER + "equipment_properties");
        formatter.printResult(cmdGetEquipmentProperties, result);
    }

    /**
     * test print getResponseData
     */
    @Test
    public void testGetResponseData() {
        String completeResult =
            cmdListDevices + AbstractCommand.DELIMITER + ServerResponse.ERROR.name();

        Assert.assertEquals("No description available", formatter.getResponseData(completeResult,
            ServerResponse.ERROR));

        completeResult = cmdListDevices + AbstractCommand.DELIMITER + ServerResponse.NAK.name();

        Assert.assertEquals("No description available", formatter.getResponseData(completeResult,
            ServerResponse.NAK));

        completeResult =
            cmdListDevices + AbstractCommand.DELIMITER + ServerResponse.PROGRESS.name();

        Assert.assertEquals("", formatter.getResponseData(completeResult, ServerResponse.PROGRESS));
    }
}
