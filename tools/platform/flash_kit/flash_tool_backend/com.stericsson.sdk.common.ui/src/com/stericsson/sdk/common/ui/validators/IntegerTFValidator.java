package com.stericsson.sdk.common.ui.validators;

import org.eclipse.swt.widgets.Text;

/**
 * @author xmicroh
 * 
 */
public class IntegerTFValidator extends AbstractTFValidator {

    private int min;

    private int max;

    private boolean changeOk;

    /**
     * @param pValueName Name for the text field.
     * @param pTf Text field for validation.
     * @param pMin Minimum value which given input should be equal or greater than.
     * @param pMax Maximum value which given input should be equal or less than.
     */
    public IntegerTFValidator(String pValueName, Text pTf, int pMin, int pMax) {
        super(pValueName, pTf);
        min = pMin;
        max = pMax;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void checkValue(String str) {
        changeOk = false;
        try {
            int parsedInt = Integer.parseInt(str);
            if ((parsedInt >= min) && (parsedInt <= max)) {
                changeOk = true;
                notifyValidatorListeneres(null);
            } else {
                notifyValidatorListeneres(getValueName() + ": Given number has to be from range <" + min + "," + max + ">");
            }
        } catch (NumberFormatException nfe) {
            notifyValidatorListeneres(getValueName() + ": Given input '" + str + "' has to be number from range <" + min
                + "," + max + ">");
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueOk() {
        return changeOk;
    }

}
