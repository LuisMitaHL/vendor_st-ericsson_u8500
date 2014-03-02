package com.stericsson.sdk.common.ui.validators.newdesign;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Text;

/**
 * Instance of this class is used to validate text field based on regular expression. Aim is to
 * check if text in text field is valid after change or not.
 * 
 * @author xmicroh
 */
public class RegexTFValidator extends AbstractTFValidator implements ModifyListener {

    private final Pattern regex;

    private boolean changeOk;

    private final String message;

    /**
     * @param severity
     *            Validation severity. See {@link IValidator#setSeverity(int)}
     * @param pValueName
     *            Name for the validated textAbstractTFValidatorused in validator messages.
     * @param pTf
     *            Text field which is going to be validated.
     * @param pRegex
     *            Regular expression which is validation based on.
     */
    public RegexTFValidator(int severity, String pValueName, Text pTf, String pRegex) {
        super(severity, pValueName, pTf);
        regex = Pattern.compile(pRegex);
        message = null;
    }

    /**
     * @param severity
     *            Validation severity. See {@link IValidator#setSeverity(int)}
     * @param pValueName
     *            Name for the validated textAbstractTFValidatorused in validator messages.
     * @param pTf
     *            Text field which is going to be validated.
     * @param pRegex
     *            Regular expression which is validation based on.
     * @param pMessage
     *            Custom message which is displayed instead of default one.
     */
    public RegexTFValidator(int severity, String pValueName, Text pTf, String pRegex, String pMessage) {
        super(severity, pValueName, pTf);
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
    @Override
    public void checkValue(String str) {
        changeOk = false;
        Matcher matcher = regex.matcher(str);
        if (!matcher.matches()) {
            if (message != null && !message.equalsIgnoreCase("")) {
                notifyValidatorListeneres(getValueName() + ": " + message);
            } else {
                notifyValidatorListeneres(getValueName() + ": value must match regular exprestion: " + regex.pattern());
            }
        } else {
            changeOk = true;
            notifyValidatorListeneres(null);
        }
    }

}
