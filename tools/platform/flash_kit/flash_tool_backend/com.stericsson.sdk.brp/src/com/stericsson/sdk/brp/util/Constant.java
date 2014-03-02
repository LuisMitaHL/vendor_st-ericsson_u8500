package com.stericsson.sdk.brp.util;

/**
 * @author xxvs0002
 *
 */
public enum Constant {

    /**
     * Error that 64bit LCD or LCM driver doesn't exist for 64bit OS
     */
    No64BitLcdLcmDriver("No 64bit LCD/LCM driver");

    private String value;

    /**
     * The value and the message that should be set to the enumeration.
     * @param valueParam the value to be set
     * @param messageParam the message to be set
     */
    private Constant(String valueParam){
        this.value = valueParam;
    }

    /**
     * @return the value set to the enumeration
     */
    public String getValue() {
        return value;
    }
}
