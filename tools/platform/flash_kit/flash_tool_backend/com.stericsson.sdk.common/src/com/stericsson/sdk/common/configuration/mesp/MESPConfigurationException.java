package com.stericsson.sdk.common.configuration.mesp;

/**
 * Exception class for MESP configuration exceptions
 * 
 * @author xtomlju
 */
public class MESPConfigurationException extends Exception {

    private static final long serialVersionUID = 6856743825532104113L;

    /**
     * @param message
     *            Exception message
     */
    public MESPConfigurationException(String message) {
        super(message);
    }

}
