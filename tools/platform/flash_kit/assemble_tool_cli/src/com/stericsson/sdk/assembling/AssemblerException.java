package com.stericsson.sdk.assembling;

/**
 * Exception class for assembler errors
 * 
 * @author xolabju
 */
public class AssemblerException extends Exception {

    private static final long serialVersionUID = 7536214301807407151L;

    /** Error code for unknown error */
    public static final int UNKNOWN_ERROR = 1;

    private int resultValue;

    /**
     * Constructor
     * 
     * @param message
     *            Exception message
     */
    public AssemblerException(String message) {
        this(message, UNKNOWN_ERROR);
    }

    /**
     * @param message
     *            Exception message
     * @param value
     *            Error code value
     */
    public AssemblerException(String message, int value) {
        super("Assembling failed: " + message);
        resultValue = value;
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     * @param value
     *            Error code value
     */
    public AssemblerException(Throwable cause, int value) {
        super(cause);
        resultValue = value;
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     */
    public AssemblerException(Throwable cause) {
        this(cause, UNKNOWN_ERROR);
    }

    /**
     * @return Result value
     */
    public int getResultValue() {
        return resultValue;
    }

}
