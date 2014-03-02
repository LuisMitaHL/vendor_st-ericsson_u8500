package com.stericsson.sdk.common.ui.validators;

import java.io.File;

import org.eclipse.swt.widgets.Text;

/**
 * Class extends AbstractTFValidator and adds validation of output path to which a file is going to
 * be created.
 */
public class OutputPathTFValidator extends AbstractTFValidator {

    private boolean changeOk;

    /**
     * Validates output path. Verifies that parent directory exists. A file is going to be created
     * in this directory
     * 
     * @param pValueName
     *            Name of the text field.
     * @param pValidatedObject
     *            Text field itself.
     */
    public OutputPathTFValidator(String pValueName, Text pValidatedObject) {
        super(pValueName, pValidatedObject);
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
    public void checkValue(String path) {

        changeOk = false;

        if (path != null && !path.equals("")) {
            File file = new File(path);
            if (file != null && file.getParent() != null) {
                if (new File(file.getParent()).exists()) {
                    changeOk = true;
                }
            } else {
                changeOk = false;

            }

        }

        if (changeOk) {
            notifyValidatorListeneres(null);
        } else {
            notifyValidatorListeneres(getValueName() + ": Given path '" + path + "' is invalid");
        }
    }

}
