package com.stericsson.sdk.common.ui.validators;

import org.eclipse.swt.widgets.Text;

/**
 * @author xmicroh
 *
 */
public class LongTFValidator extends AbstractTFValidator {

    private static final int UNDEF_RADIX = -1;

    private long min;

    private long max;

    private boolean changeOk;

    private int radix = UNDEF_RADIX;

    /**
     * @param pValueName Name for the text field.
     * @param pTf Text field for validation.
     * @param pMin Minimum value which given input should be equal or greater than.
     * @param pMax Maximum value which given input should be equal or less than.
     */
    public LongTFValidator(String pValueName, Text pTf, long pMin, long pMax) {
        super(pValueName, pTf);
        min = pMin;
        max = pMax;
    }

    /**
     * @param pValueName Name for the text field.
     * @param pTf Text field for validation.
     * @param pMin Minimum value which given input should be equal or greater than.
     * @param pMax Maximum value which given input should be equal or less than.
     * @param pRadix Radix which number should be specified in.
     */
    public LongTFValidator(String pValueName, Text pTf, long pMin, long pMax, int pRadix) {
        this(pValueName, pTf, pMin, pMax);
        radix  = pRadix;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void checkValue(String str) {
        changeOk = false;
        try {
            long parsedLong = radix != UNDEF_RADIX ? Long.parseLong(str, radix) : Long.parseLong(str);
            if ((parsedLong >= min) && (parsedLong <= max)) {
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
