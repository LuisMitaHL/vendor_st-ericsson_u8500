package com.stericsson.sdk.assembling.internal;

import java.io.File;

import com.stericsson.sdk.assembling.AssemblerException;

/**
 * @author xtomlju
 */
public final class BasicFileValidator {

    /**
     * @param filename
     *            Filename to validate
     * @throws AssemblerException
     *             If an signer related error occurred
     */
    public static void validateInputFile(String filename) throws AssemblerException {
        File inputFile = new File(filename);

        // Check existance

        if (!inputFile.exists()) {
            throw new AssemblerException("File '" + filename + "' not found");
        }

        // Check that it is not a folder

        if (inputFile.isDirectory()) {
            throw new AssemblerException("File '" + filename + "' is a directory");
        }

        // Check file size

        if (inputFile.length() == 0) {
            throw new AssemblerException("File '" + filename + "' does not contain any data");
        }
    }

    /**
     * @param filename
     *            Filename to validate
     * @param overWrite
     *            true if overwriting is allowed, else false
     * @throws AssemblerException
     *             If an signer related error occurred
     */
    public static void validateOutputFile(String filename, boolean overWrite) throws AssemblerException {
        File inputFile = new File(filename);

        // Check existance

        if (inputFile.exists() && !overWrite) {
            throw new AssemblerException("File '" + filename + "' already exists");
        }

        // Check that it is not a folder

        if (inputFile.isDirectory()) {
            throw new AssemblerException("File '" + filename + "' is a directory");
        }
    }

    private BasicFileValidator() {

    }
}
