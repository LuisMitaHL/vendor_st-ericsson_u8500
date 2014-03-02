package com.stericsson.sdk.assembling;

/**
 * Exception for assembler settings related errors
 * 
 * @author xolabju
 * 
 */
public class AssemblerSettingsException extends AssemblerException {

    private static final long serialVersionUID = -1847418138158640815L;

    /**
     * Constructor
     * 
     * @param message
     *            exception message
     */
    public AssemblerSettingsException(String message) {
        super(message, UNKNOWN_ERROR);
    }

    /**
     * Constructor
     * 
     * @param message
     *            exception message
     * @param value
     *            error code
     */
    public AssemblerSettingsException(String message, int value) {
        super(message, value);
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     * @param value
     *            Error code value
     */
    public AssemblerSettingsException(Throwable cause, int value) {
        super(cause, value);
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     */
    public AssemblerSettingsException(Throwable cause) {
        this(cause, UNKNOWN_ERROR);
    }

}
