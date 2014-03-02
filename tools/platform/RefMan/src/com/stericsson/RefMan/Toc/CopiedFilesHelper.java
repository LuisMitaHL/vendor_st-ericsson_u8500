/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.util.HashSet;

public class CopiedFilesHelper {

    /** A variable representing a CopiedFilesHelper object. */
    private static CopiedFilesHelper copiedFilesHelper;

    /**
     * A <code>HashSet</code> that is used to remember which files has been
     * copied. To handle files with the same name.
     */
    private final HashSet<String> copiedFiles;

    /**
     * A private Constructor prevents any other class from instantiating.
     */
    private CopiedFilesHelper() {
        copiedFiles = new HashSet<String>();
    }

    /**
     * A method for getting a the reference to the CopiedFilesHelper object.
     */
    public static synchronized CopiedFilesHelper getCopiedFilesHelper() {
        if (copiedFilesHelper == null) {
            copiedFilesHelper = new CopiedFilesHelper();
        }
        return copiedFilesHelper;
    }

    /**
     * A method which adds the name of the file to the set of files that has
     * been copied.
     *
     * The file path should be the absolute path of a <code>File</code>.
     *
     * @param filePath
     *            The path of the file that has been copied.
     */
    public void addFileName(String filePath) {
        copiedFiles.add(filePath);
    }

    /**
     * A method that checks if the given file name already exists in the set of
     * files that already has been copied.
     *
     * The file path should be the absolute path of a <code>File</code>.
     *
     * @param filePath
     *            The path of the file that is to be checked if it already has
     *            been copied.
     * @return @return {@code true} if the file already has been copied,
     *         otherwise <code>False</code>.
     */
    public boolean fileAlreadyCopied(String filePath) {
        return copiedFiles.contains(filePath);
    }

    /**
     * Override the Object clone method to prevent cloning.
     */
    @Override
    public Object clone() throws CloneNotSupportedException {
        throw new CloneNotSupportedException();
    }
}
