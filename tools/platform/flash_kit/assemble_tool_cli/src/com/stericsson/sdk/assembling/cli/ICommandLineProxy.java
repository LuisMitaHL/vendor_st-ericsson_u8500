package com.stericsson.sdk.assembling.cli;

import org.apache.commons.cli.CommandLine;


/**
 * @author qdaneke
 *
 * Proxy against Apache Commons CLI CommandLine,
 * makes it possible to mock the implementation.
 */
public interface ICommandLineProxy {

    /**
     * Query to see if an option has been set.
     *
     * @param shortCMD Short name of the option 
     *
     * @return true if set, false if not
     */
    boolean hasOption(String shortCMD);

    /**
     * Retrieve the argument, if any, of this option.
     *
     * @param shortCMD the character name of the option 
     *
     * @return Value of the argument if option is set,
     *         and has an argument, otherwise null.
     */
    String getOptionValue(String shortCMD);

    /**
     * Retrieve any left-over non-recognized options and arguments
     *
     * @return remaining items passed in but not parsed as an array
     */
    String[] getArgs();

    /**
     * Use this method to set backend to be
     * used as implementation for this
     * proxy.
     * 
     * @param cl commandline to be used.
     */
    void setProxiedObject(CommandLine cl);

}
