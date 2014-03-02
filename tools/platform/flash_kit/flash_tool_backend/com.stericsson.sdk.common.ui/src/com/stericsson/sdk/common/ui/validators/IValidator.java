package com.stericsson.sdk.common.ui.validators;

import org.eclipse.swt.widgets.Control;

/**
 * Objects of this type are able to validate input from various UI controls. 
 * 
 * @author xmicroh
 *
 */
public interface IValidator {

    /**
     * @param l Listener to be added.
     */
    void addValidatorListener(IValidatorMessageListener l);

    /**
     * @param l Listener to be removed.
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
}
