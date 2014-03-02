package com.stericsson.sdk.assembling.cli;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.assembling.internal.u8500.U8500CommandLineOptions;

/**
 * Factory for creating an instance of command line options depending on which type of assembly that
 * is to be performed (e.g. a2, u8500 ... )
 * 
 * @author xolabju
 * 
 */
public final class CommandLineOptionsFactory {

    private CommandLineOptionsFactory() {
    }

    /**
     * 
     * @param platformType
     *            the platform type towards which the assembly is targeted
     * @return an instance of Options for the specified platform type
     * @throws ParseException
     *             on parsing errors
     */
    public static Options createCommandLineOptions(String platformType) throws ParseException {
        if (Assemble.PLATFORM_U8500.equalsIgnoreCase(platformType)) {
            return new U8500CommandLineOptions();
        } else {
            throw new ParseException("You must specify valid subcommand as first parameter, i.e. "
                + Assemble.PLATFORM_U8500);
        }
    }

}
