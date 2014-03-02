package com.stericsson.sdk.signing;

/**
 * Exception class for signer exceptions
 * 
 * @author xtomlju
 */
public class SignerException extends Exception {

    /** Error code for unknown error */
    public static final int UNKNOWN_ERROR = 1;

    private static final long serialVersionUID = 7922684889188075169L;

    private int resultValue;

    /**
     * Constructor
     * 
     * @param message
     *            Exception message
     */
    public SignerException(String message) {
        super(message);
        resultValue = UNKNOWN_ERROR;
    }

    /**
     * @param message
     *            Exception message
     * @param value
     *            Error code value
     */
    public SignerException(String message, int value) {
        super(message);
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
    public SignerException(Throwable cause, int value) {
        super(cause);
        resultValue = value;
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     */
    public SignerException(Throwable cause) {
        this(cause, UNKNOWN_ERROR);
    }

    /**
     * @return Result value
     */
    public int getResultValue() {
        return resultValue;
    }

}
