package com.stericsson.sdk.common.ui.validators;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.Control;

/**
 * Validator for UI controls.
 * 
 */
public abstract class AbstractValidator implements IValidator {

    private List<IValidatorMessageListener> validatorListeners;

    private Control validatedObject;

    /** Message shown to user if regex doesn't match */
    protected String message = "";

    /**
     * Constructor.
     * 
     * @param pValidatedObject
     *            validated object
     */
    public AbstractValidator(Control pValidatedObject) {
        validatedObject = pValidatedObject;
        validatorListeners = new ArrayList<IValidatorMessageListener>();
    }

    /**
     * {@inheritDoc}
     */
    public final Control getValidatedObject() {
        return validatedObject;
    }

    /**
     * {@inheritDoc}
     */
    public final void addValidatorListener(IValidatorMessageListener l) {
        if (!validatorListeners.add(l)) {
            throw new IllegalStateException("Given listener is already registered");
        }
    }

    /**
     * {@inheritDoc}
     */
    public final void removeValidatorListener(IValidatorMessageListener l) {
        if (!validatorListeners.remove(l)) {
            throw new IllegalStateException("Given listener is not registered");
        }
    }

    /**
     * @param pMessage
     *            Message which listeners will be notified with.
     */
    public final void notifyValidatorListeneres(String pMessage) {
        for (IValidatorMessageListener l : validatorListeners) {
            l.validatorMessage(this, pMessage);
        }
    }

}
