package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Custom wizard exception.
 * 
 */
public class WizardException extends Exception {

    private static final long serialVersionUID = 1L;

    String mMessage;

    /**
     * Constructor.
     * 
     * @param pMessage
     *            Exception message.
     */
    public WizardException(String pMessage) {
        mMessage = pMessage;
    }
}
