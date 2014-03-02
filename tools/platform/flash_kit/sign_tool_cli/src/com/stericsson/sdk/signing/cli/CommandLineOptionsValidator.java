package com.stericsson.sdk.signing.cli;

import java.util.Collection;

import org.apache.commons.cli.Option;
import org.apache.commons.cli.ParseException;
import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesCommandLineOptions;
import com.stericsson.sdk.signing.cli.m7x00.M7X00CommandLineOptions;

/**
 * @author xtomzap
 * 
 */
public final class CommandLineOptionsValidator {

    private CommandLineOptionsValidator() {

    }

    /**
     * Validates the options from a set of command line options. Use this to log the missing
     * required options.
     * 
     * @param commandLine
     *            a set of commands that has been specified.
     * 
     * @param l
     *            this logger will be used when we want to logg information.
     * 
     * @throws ParseException
     *             if commandLine lack a required option.
     */
    public static void validateArguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        boolean missingReqOptions = false;

        if (Sign.SIGN_A2.equalsIgnoreCase(commandLine.getArgs()[0])) {
            missingReqOptions = missingA2Arguments(commandLine, l);
        } else if (Sign.SIGN_LOAD_MODULES.equalsIgnoreCase(commandLine.getArgs()[0])) {
            missingReqOptions = missingLoadModulesArguments(commandLine, l);
        } else if (Sign.SIGN_M7X00.equalsIgnoreCase(commandLine.getArgs()[0])) {
            M7X00CommandLineOptions options = new M7X00CommandLineOptions();
            missingReqOptions = missingArguments(commandLine, options.getOptions(), l);
        } else {
            GenericCommandLineOptions options = new GenericCommandLineOptions();
            missingReqOptions = missingArguments(commandLine, options.getOptions(), l);
        }

        if (missingReqOptions) {
            throw new ParseException("Missing one or more required parameters.");
        }
    }

    private static boolean missingLoadModulesArguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        boolean missingReqOptions = false;
        for (LoadModulesCommandLineOptions.Option i : LoadModulesCommandLineOptions.Option.values()) {
            if (missingArgument(commandLine, i.getLongCMD(), i.getDescription(), i.isRequired(), l)) {
                missingReqOptions = true;
            }
        }
        return missingReqOptions;
    }

    private static boolean missingA2Arguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        boolean missingReqOptions = false;
        for (A2CommandLineOptions.Option i : A2CommandLineOptions.Option.values()) {
            if (missingArgument(commandLine, i.getLongCMD(), i.getDescription(), i.isRequired(), l)) {
                missingReqOptions = true;
            }
        }
        return missingReqOptions;
    }

    private static boolean missingArguments(ICommandLineProxy commandLine, Collection<Option> options, Logger l)
        throws ParseException {
        boolean missingReqOptions = false;
        for (Option o : options) {
            if (A2CommandLineOptions.Option.PACKAGE.getLongCMD().equalsIgnoreCase(o.getLongOpt()) 
                && commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGN_KEY.getLongCMD())) {
                o.setRequired(false);
            }
            if (missingArgument(commandLine, o.getLongOpt(), o.getDescription(), o.isRequired(), l)) {
                missingReqOptions = true;
            }
        }
        return missingReqOptions;
    }

    private static boolean missingArgument(ICommandLineProxy commandLine, String longName, String description,
        boolean isRequired, Logger l) {
        if (isRequired && !commandLine.hasOption(longName)) {
            if (l != null) {
                l.error("Missing required param --" + longName + ". Description: " + description);
            }

            return true;
        }

        return false;
    }

}
