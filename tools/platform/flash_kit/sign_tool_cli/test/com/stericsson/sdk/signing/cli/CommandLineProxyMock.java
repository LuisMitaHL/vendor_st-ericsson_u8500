package com.stericsson.sdk.signing.cli;

import java.util.HashMap;

import org.apache.commons.cli.CommandLine;

/**
 * 
 * @author mielcluk
 * 
 */
public class CommandLineProxyMock implements ICommandLineProxy {

    /**
     * Constructor
     */
    public CommandLineProxyMock() {
        initValues();
    }

    private void initValues() {

    }

    HashMap<String, String> optionValues = new HashMap<String, String>();

    /**
     * {@inheritDoc}
     */
    public String[] getArgs() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public void setOptionValue(String option, String value) {
        optionValues.put(option, value);
    }

    /**
     * {@inheritDoc}
     */
    public String getOptionValue(String option) {
        return optionValues.get(option);

    }

    /**
     * {@inheritDoc}
     */
    public boolean hasOption(String option) {
        return optionValues.containsKey(option);
    }

    /**
     * {@inheritDoc}
     */
    public void setProxiedObject(CommandLine pCl) {

    }

    /**
     * {@inheritDoc}
     */
    public void removeOption(String option) {
        optionValues.remove(option);
    }

}
