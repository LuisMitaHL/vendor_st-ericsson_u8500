package com.stericsson.sdk.signing.signerservice;

import com.stericsson.sdk.signing.SignerException;

/**
 * Exception class for signer service related errors.
 * 
 * @author xtomlju
 */
public class SignerServiceException extends SignerException {

    private static final long serialVersionUID = -8818966970052207746L;

    private static final int DEFAULT_SIGNER_SERVICE_ERROR_CODE = SignerException.UNKNOWN_ERROR;

    /**
     * @param message
     *            Exception message
     */
    public SignerServiceException(String message) {
        this(message, DEFAULT_SIGNER_SERVICE_ERROR_CODE);
    }

    /**
     * @param message
     *            Exception message
     * @param value
     *            Error code value
     */
    public SignerServiceException(String message, int value) {
        super(message, value);
    }

}
