package com.stericsson.sdk.signing.cli;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesCommandLineOptions;
import com.stericsson.sdk.signing.cli.m7x00.M7X00CommandLineOptions;

/**
 * Factory for creating an instance of command line options depending on which type of signing that
 * is to be performed (e.g. sign_a2, sign_u8500 ... )
 * 
 * @author xolabju
 * 
 */
public final class CommandLineOptionsFactory {

    private CommandLineOptionsFactory() {
    }

    /**
     * 
     * @param signingType
     *            the type of signing to perform
     * @return an instance of Options for the specified signing type
     * @throws ParseException
     *             on parsing errors
     */
    public static Options createCommandLineOptions(String signingType) throws ParseException {
        if (Sign.SIGN_A2.equalsIgnoreCase(signingType)) {
            return new A2CommandLineOptions();
        } else if (Sign.SIGN_L9540.equalsIgnoreCase(signingType) || Sign.SIGN_U8500.equalsIgnoreCase(signingType) 
            || Sign.SIGN_U5500.equalsIgnoreCase(signingType)) {
            return new GenericCommandLineOptions();
        } else if (Sign.SIGN_M7X00.equalsIgnoreCase(signingType)) {
            return new M7X00CommandLineOptions();
        } else if (Sign.SIGN_LOAD_MODULES.equalsIgnoreCase(signingType)) {
            return new LoadModulesCommandLineOptions();
        } else {
            throw new ParseException("You must specify valid subcommand as first parameter, i.e. " + Sign.SIGN_A2
                + ", " + Sign.SIGN_U5500 + ", " + Sign.SIGN_U8500 + ", " + Sign.SIGN_L9540 + ", " + Sign.SIGN_M7X00 + "or "
                + Sign.SIGN_LOAD_MODULES);
        }
    }

}
