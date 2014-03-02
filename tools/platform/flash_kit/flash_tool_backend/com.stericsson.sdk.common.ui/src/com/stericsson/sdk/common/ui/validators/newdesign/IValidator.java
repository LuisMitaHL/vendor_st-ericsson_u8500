package com.stericsson.sdk.common.ui.validators.newdesign;

import org.eclipse.swt.widgets.Control;

/**
 * Objects of this type are able to validate input from various UI controls.
 * 
 * @author xmicroh
 * 
 */
public interface IValidator {

    /**
     * @param l
     *            Listener to be added.
     */
    void addValidatorListener(IValidatorMessageListener l);

    /**
     * @param l
     *            Listener to be removed.
     */
    void removeValidatorListener(IValidatorMessageListener l);

    /**
     * @return Objects which is validated by the validator.
     */
    Control getValidatedObject();

    /**
     * Switch ON this validator.
     */
    void switchOn();

    /**
     * Switch OFF this validator.
     */
    void switchOff();

    /**
     * @return True if last current value of validated object is valid, false otherwise.
     */
    boolean isValueOk();

    /**
     * Perform validation on demand.
     */
    void checkCurrentValue();

    /**
     * 
     * @param enable
     *            True enable control decoration of validate object, false disable it.
     * @param decorationPosition
     *            For more info see {link ControlDecoration#ControlDecoration(Control, int,
     *            Composite)}.
     */
    void enableDecoration(boolean enable, int decorationPosition);

    /**
     * @return True if validated object is decorated, false otherwise.
     */
    boolean isDecorationEnabled();

    /**
     * 
     * @param severity
     *            Accepted values are IStatus.INFO, IStatus.WARNING and IStatus.ERROR. By default is
     *            set SWT.ERROR.
     */
    void setSeverity(int severity);

    /**
     * @return Severity of validation result.
     */
    int getSeverity();
}
