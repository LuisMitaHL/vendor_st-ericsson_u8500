package com.stericsson.sdk.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

/**
 * Utility class to manipulate files and directories.
 * 
 * @author xadazim
 * 
 */
public enum FileUtils {
    /**
     * The only instance.
     */
    INSTANCE;

    /**
     * Creates a temporary directory. Uses File.createTempFile() method to generate unique name.
     * 
     * @param pPrefix
     *            The prefix string to be used in generating the file's name; must be at least three
     *            characters long
     * @param pSuffix
     *            The suffix string to be used in generating the file's name; may be null, in which
     *            case the suffix ".tmp" will be used
     * @return File object representing the newly created directory
     * @throws IOException
     *             In case the tmp directory could not be created
     */
    public static File createTmpDir(String pPrefix, String pSuffix) throws IOException {
        File tmpDir = File.createTempFile(pPrefix, pSuffix);
        tmpDir.delete();
        tmpDir = new File(tmpDir.getPath() + "_dir");
        tmpDir.mkdir();
        return tmpDir;
    }

    /**
     * Removes all files in given directory. If directory contains sub-directories exception is
     * thrown. Recursive file removal is not implemented for security reasons.
     * 
     * @param pDir
     *            directory which contents should be removed
     * @throws IOException
     *             thrown if directory contains sub directories or if some file can not be removed
     */
    public static void removeFilesInDir(File pDir) throws IOException {
        File[] contents = pDir.listFiles();
        if (contents == null) {
            throw new IOException("Could not list contents of directory " + pDir.getAbsolutePath());
        }

        for (File f : contents) {
            if (!f.isFile()) {
                throw new IOException("This method can delete only files "
                    + "(for security reasons recursive removal is not implemented)");
            }

            if (!f.delete()) {
                throw new IOException("File " + f.getAbsolutePath() + " could not be removed");
            }
        }
    }

    /**
     * Copies file on file system.
     * 
     * @param in
     *            input file
     * @param out
     *            output file (will be overwritten if exists)
     * @throws IOException
     *             in case of copy error
     */
    public static void copyFile(File in, File out) throws IOException {
        FileChannel inChannel = null;
        FileInputStream inStream = null;
        FileOutputStream outStream = null;
        FileChannel outChannel = null;
        try {
            inStream = new FileInputStream(in);
            inChannel = inStream.getChannel();
            outStream = new FileOutputStream(out);
            outChannel = outStream.getChannel();
            inChannel.transferTo(0, inChannel.size(), outChannel);
        } finally {
            if (inChannel != null) {
                inChannel.close();
            }
            if (inStream != null) {
                inStream.close();
            }
            if (outChannel != null) {
                outChannel.close();
            }
            if (outStream != null) {
                outStream.close();
            }
        }
    }

}
