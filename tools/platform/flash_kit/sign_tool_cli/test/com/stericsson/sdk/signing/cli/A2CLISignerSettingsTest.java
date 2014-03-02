package com.stericsson.sdk.signing.cli;

import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
import org.junit.Test;

import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;

/**
 * @author emicroh
 * 
 */
public class A2CLISignerSettingsTest {

    /**
     * @throws Throwable
     *             TBD
     */
    @Test(expected = ParseException.class)
    public void errorArgumentTest() throws Throwable {
        parseArguments(new String[] {
            "--errorArgument"});
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void macModeOptionsTest() throws Throwable {
        String[] macModes = new String[] {
            "Config", "2ndSBC", "3rdSBC", "4thSBC", "SW", "License", "Mac7", "Mac8"};
        for (String macMode : macModes) {
            parseArguments(new String[] {
                "--mac-mode", macMode});
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void ptypeOptionsTest() throws Throwable {
        String[] pTypes = new String[] {
            "nand", "nor", "physical"};
        for (String pType : pTypes) {
            parseArguments(new String[] {
                "--ptype", pType});
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void headerSecuritySupportOptionTest() throws Throwable {
        String[] hdrSecs = new String[] {
            "asIs=0xdestAddr", "replaceSign=0xdestAddr", "verifyOnly"};
        for (String hdrSec : hdrSecs) {
            parseArguments(new String[] {
                "--hdr-sec", hdrSec});
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void targetCPUOptionTest() throws Throwable {
        String[] targetCPUs = new String[] {
            "app", "acc"};
        for (String targetCPU : targetCPUs) {
            parseArguments(new String[] {
                "--target", targetCPU});
        }
    }

    private A2CommandLineOptions parseArguments(String[] arguments) throws NullPointerException, ParseException {
        ICommandLineProxy commandLine = new CommandLineProxy();
        CommandLineParser argumentParser = new PosixParser();

        A2CommandLineOptions commandLineOptions = new A2CommandLineOptions();
        commandLine.setProxiedObject(argumentParser.parse(commandLineOptions, arguments));

        return commandLineOptions;
    }
}
