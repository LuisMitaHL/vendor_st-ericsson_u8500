package com.stericsson.sdk.common.ui.validators;

import java.io.File;

import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.common.ui.behaviors.PathTFBehavior;

/**
 * Instance of this class is used to validate text field containing file paths. Aim is to answer
 * if given path is valid while given path could be either absolute or relative. 
 * 
 * @author xmicroh
 * 
 */
public class PathTFValidator extends AbstractTFValidator {

    private boolean changeOk;

    private boolean directory;

    /**
     * Construct validator based on given parameters.
     * 
     * @param pValueName Name for the validated text field which is used in validator messages.
     * @param pTf Text field containing path.
     * @param isDirectory True when validated path is directory, false in case of validating files.
     */
    public PathTFValidator(String pValueName, Text pTf, boolean isDirectory) {
        super(pValueName, pTf);
        directory = isDirectory;
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
    public void checkValue(String relativePath) {
        changeOk = false;

        File resourceFile = new File(PathTFBehavior.getAbsolutePath(relativePath));
        changeOk = resourceFile.exists() && (directory ? resourceFile.isDirectory() : resourceFile.isFile());

        if (changeOk) {
            notifyValidatorListeneres(null);
        } else {
            notifyValidatorListeneres(getValueName() + ": Cannot find given path '" + relativePath + "'");
        }
    }

}
