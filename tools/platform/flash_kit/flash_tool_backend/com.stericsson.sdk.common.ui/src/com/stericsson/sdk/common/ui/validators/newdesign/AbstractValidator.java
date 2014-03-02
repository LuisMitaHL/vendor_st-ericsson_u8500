package com.stericsson.sdk.common.ui.validators.newdesign;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;

import com.stericsson.sdk.common.ui.decorators.ControlDecorator;

/**
 * Validator for UI controls.
 * 
 */
public abstract class AbstractValidator implements IValidator {

    private final List<IValidatorMessageListener> validatorListeners;

    private final Control validatedObject;

    private int severity = IStatus.ERROR;

    private int decorationPosition = SWT.LEFT | SWT.TOP;

    private ControlDecorator decorator;

    // /**
    // * Constructor.
    // *
    // * @param pValidatedObject
    // * validated object
    // */
    // public AbstractValidator(Control pValidatedObject) {
    // validatedObject = pValidatedObject;
    // validatorListeners = new ArrayList<IValidatorMessageListener>();
    // }

    /**
     * Constructor.
     * 
     * @param pSeverity
     *            Validation severity. See {@link IValidator#setSeverity(int)}
     * @param pValidatedObject
     *            validated object
     */
    public AbstractValidator(int pSeverity, Control pValidatedObject) {
        // this(pValidatedObject);
        validatedObject = pValidatedObject;
        validatorListeners = new ArrayList<IValidatorMessageListener>();
        setSeverity(pSeverity);
        enableDecoration(true, SWT.LEFT);
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
        IStatus msg = pMessage == null ? null : ControlDecorator.getStatusMessage(severity, pMessage);

        if (isDecorationEnabled()) {
            decorator.showMessage(msg);
        }
        for (IValidatorMessageListener l : validatorListeners) {
            l.validatorMessage(this, msg);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void enableDecoration(boolean enable, int position) {
        if (enable) {
            decorationPosition = position;
            if (decorator != null) {
                decorator.dispose();
            }
            decorator =
                new ControlDecorator(getValidatedObject().getParent(), getValidatedObject(), decorationPosition);
        } else if (decorator != null) {
            decorator.dispose();
            decorator = null;
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isDecorationEnabled() {
        return decorator != null;
    }

    /**
     * {@inheritDoc}
     */
    public void setSeverity(int sev) {
        switch (sev) {
            case IStatus.INFO:
                severity = IStatus.INFO;
                break;
            case IStatus.WARNING:
                severity = IStatus.WARNING;
                break;
            case IStatus.ERROR:
                severity = IStatus.ERROR;
                break;
            default:
                severity = IStatus.ERROR;
        }
    }

    /**
     * {@inheritDoc}
     */
    public int getSeverity() {
        return severity;
    }
}
