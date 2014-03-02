package com.stericsson.sdk.assembling.cli;

import org.apache.commons.cli.CommandLine;

/**
 * Represents list of arguments parsed against a Options descriptor.
 * 
 * It allows querying of a boolean hasOption(String opt), in addition to retrieving the
 * getOptionValue(String opt) for options requiring arguments.
 * 
 * Additionally, any left-over or unrecognized arguments, are available for further processing.
 * 
 * Implemented as with proxy against Apaches CommandLine.
 */
public class CommandLineProxy implements ICommandLineProxy {
    CommandLine commandLine;

    /**
     * Query to see if an option has been set.
     * 
     * @param shortCMD
     *            Short name of the option
     * 
     * @return true if set, false if not
     */
    public boolean hasOption(String shortCMD) {
        return commandLine.hasOption(shortCMD);
    }

    /**
     * Retrieve the argument, if any, of this option.
     * 
     * @param shortCMD
     *            the character name of the option
     * 
     * @return Value of the argument if option is set, and has an argument, otherwise null.
     */
    public String getOptionValue(String shortCMD) {
        return commandLine.getOptionValue(shortCMD);
    }

    /**
     * Retrieve any left-over non-recognized options and arguments
     * 
     * @return remaining items passed in but not parsed as an array
     */
    public String[] getArgs() {
        return commandLine.getArgs();
    }

    /**
     * Use this method to set backend to be used as implementation for this proxy.
     * 
     * @param cl
     *            commandline to be used.
     */
    public void setProxiedObject(CommandLine cl) {
        this.commandLine = cl;
    }
}
