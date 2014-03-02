package com.stericsson.sdk.signing.util;

import java.io.File;

import com.stericsson.sdk.signing.SignerException;


/**
 * @author xtomlju
 */
public final class BasicFileValidator {

    /**
     * @param filename
     *            Filename to validate
     * @throws SignerException
     *             If an signer related error occurred
     */
    public static void validateInputFile(String filename) throws SignerException {
        File inputFile = new File(filename);

        // Check existance

        if (!inputFile.exists()) {
            throw new SignerException("File '" + filename + "' not found");
        }

        // Check that it is not a folder

        if (inputFile.isDirectory()) {
            throw new SignerException("File '" + filename + "' is a directory");
        }

        // Check file size

        if (inputFile.length() == 0) {
            throw new SignerException("File '" + filename + "' does not contain any data");
        }
    }

    private BasicFileValidator() {

    }
}
