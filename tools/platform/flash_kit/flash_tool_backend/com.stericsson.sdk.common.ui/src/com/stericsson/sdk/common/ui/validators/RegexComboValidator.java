package com.stericsson.sdk.common.ui.validators;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.swt.widgets.Combo;

/**
 * Instance of this class is used to validate combo field based on regular expression. Aim is to
 * check if text in combo field is valid after change or not.
 * 
 * @author xmicroh
 */
public class RegexComboValidator extends AbstractComboValidator {

    private Pattern regex;

    private boolean changeOk;

    /**
     * @param pValueName
     *            Name for the validated text of AbstractComboValidator used in validator messages.
     * @param pCombo
     *            Combo field which is going to be validated.
     * @param pRegex
     *            Regular expression which is validation based on.
     */
    public RegexComboValidator(String pValueName, Combo pCombo, String pRegex) {
        super(pValueName, pCombo);
        regex = Pattern.compile(pRegex);
    }

    /**
     * @param pValueName
     *            Name for the validated text of AbstractComboValidator used in validator messages.
     * @param pCombo
     *            Combo field which is going to be validated.
     * @param pRegex
     *            Regular expression which is validation based on.
     * @param pMessage
     *            message shown to user.
     */
    public RegexComboValidator(String pValueName, Combo pCombo, String pRegex, String pMessage) {
        super(pValueName, pCombo);
        regex = Pattern.compile(pRegex);
        message = pMessage;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueOk() {
        return changeOk;
    }

    /**
     * {@inheritDoc}
     */
    public void checkValue(String str) {
        changeOk = false;
        if (str == null) {
            str = "";
        }
        Matcher matcher = regex.matcher(str);
        if (!matcher.matches()) {
            if (message != null && !message.equalsIgnoreCase("")) {
                notifyValidatorListeneres(getValueName() + message);
            } else {
                notifyValidatorListeneres(getValueName() + ": value must match regular exprestion: " + regex.pattern());
            }
        } else {
            changeOk = true;
            notifyValidatorListeneres(null);
        }
    }

}
