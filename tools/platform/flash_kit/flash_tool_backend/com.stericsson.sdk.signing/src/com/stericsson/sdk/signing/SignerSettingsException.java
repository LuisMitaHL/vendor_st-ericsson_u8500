package com.stericsson.sdk.signing;


/**
 * Exception class for signer settings related errors.
 * 
 * @author xtomlju
 */
public class SignerSettingsException extends SignerException {

    private static final long serialVersionUID = 408643550300503826L;

    private static final int DEFAULT_SIGNER_SETTINGS_ERROR_CODE = SignerException.UNKNOWN_ERROR;

    /**
     * Constructor.
     * 
     * @param message
     *            Exception message
     */
    public SignerSettingsException(String message) {
        this(message, DEFAULT_SIGNER_SETTINGS_ERROR_CODE);
    }

    /**
     * Constructor.
     * 
     * @param message
     *            Exception message
     * @param value
     *            Error code value
     */
    public SignerSettingsException(String message, int value) {
        super(message, value);
    }

    /**
     * Constructor
     * 
     * @param cause
     *            The throwable that caused the exception
     */
    public SignerSettingsException(Throwable cause) {
        super(cause, DEFAULT_SIGNER_SETTINGS_ERROR_CODE);
    }

}
