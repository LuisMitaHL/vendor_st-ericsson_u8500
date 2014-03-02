package com.stericsson.sdk.common.ui.validators;

/**
 * @author xmicroh
 *
 */
public interface IValidatorMessageListener {

    /**
     * @param src Validator which fired the event.
     * @param message String message describing error or null when validation was OK.
     */
    void validatorMessage(IValidator src, String message);

}
