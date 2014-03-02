package com.stericsson.sdk.common.ui.validators.newdesign;

import org.eclipse.core.runtime.IStatus;

/**
 * @author xmicroh
 * 
 */
public interface IValidatorMessageListener {

    /**
     * @param src
     *            Validator which fired the event.
     * @param message
     *            String message describing error or null when validation was OK.
     */
    void validatorMessage(IValidator src, IStatus message);

}
