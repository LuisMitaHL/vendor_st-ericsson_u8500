/**
 * 
 */
package com.stericsson.sdk.common.ui;

/**
 * @author xhelciz
 * 
 */
public class UnrecognizedVisualTypeException extends Exception {

    /**
     * 
     */
    private static final long serialVersionUID = 1942766182365789411L;

    /**
     * This exception should be thrown if the Visual Type, which was requested during UI creation,
     * is not defined
     * 
     * @param message
     *            message of this exception
     */
    public UnrecognizedVisualTypeException(String message) {
        super(message);
    }
}
