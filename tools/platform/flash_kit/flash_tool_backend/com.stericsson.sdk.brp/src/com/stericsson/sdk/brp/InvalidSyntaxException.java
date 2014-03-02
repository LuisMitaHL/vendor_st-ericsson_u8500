package com.stericsson.sdk.brp;

/**
 * Exception that will be thrown when a command is constructed with invalid parameters
 * 
 * @author xolabju
 * 
 */
public class InvalidSyntaxException extends Exception {

    private static final long serialVersionUID = 5612306115878333305L;

    /**
     * Constructor
     * 
     * @param msg
     *            exception message
     */
    public InvalidSyntaxException(String msg) {
        super(msg);
    }
}
