package com.stericsson.sdk.common.ui.validators;

import java.util.List;

import org.eclipse.swt.widgets.Text;

/**
 * @author xtomzap
 *
 */
public class UniqueValueTFValidator extends AbstractTFValidator {

    private boolean changeOk;
    private List<String> valueList;

    /**
     * @param pValueName value name
     * @param pValidatedObject validated object
     * @param pValueList list of unique values
     */
    public UniqueValueTFValidator(String pValueName, Text pValidatedObject, List<String> pValueList) {
        super(pValueName, pValidatedObject);
        valueList = pValueList;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void checkValue(String pStr) {
        changeOk = false;

        if (valueList.contains(pStr)) {
            notifyValidatorListeneres(getValueName() + ": has to be unique");
        } else {
            changeOk = true;
            notifyValidatorListeneres(null);
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueOk() {
        return changeOk;
    }

}
