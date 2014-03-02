package com.stericsson.sdk.common.ui.validators;

import java.io.File;
import java.io.IOException;

import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.signing.util.PEMParser;

/**
 * @author xmicroh
 *
 */
public class PemKeyTFValidator extends AbstractTFValidator {

    private boolean changeOk;

    /**
     * Construct validator for checking if given path points to correct PEM key file.
     * 
     * @param pValueName Name of the text field to check.
     * @param pTf Text field containing path.
     */
    public PemKeyTFValidator(String pValueName, Text pTf) {
        super(pValueName, pTf);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void checkValue(String pathToKeyFile) {
        changeOk = false;

        if (pathToKeyFile.trim().length() > 0) {
            File keyFile = new File(pathToKeyFile);
            if (keyFile.exists() && keyFile.isFile()) {
                try {
                    PEMParser.parseRSAPublicKey(keyFile.getAbsolutePath());
                    changeOk = true;
                    notifyValidatorListeneres(null);
                    return;
                } catch (IOException e) {
                    notifyValidatorListeneres(getValueName() + ": Cannot parse public key from given file '"
                        + keyFile.getAbsolutePath() + "'");
                }
            } 
        }

        if (!changeOk) {
            notifyValidatorListeneres(getValueName() + ": Path '" + pathToKeyFile + "' doesn't point to valid key file.");
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isValueOk() {
        return changeOk;
    }

}
