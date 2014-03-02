/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Commons;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.Writer;
import java.nio.channels.FileChannel;

import javax.swing.text.html.HTMLEditorKit;
import javax.swing.text.html.HTMLEditorKit.ParserCallback;
import javax.swing.text.html.parser.ParserDelegator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author Thomas Palmqvist
 *
 */
public class IO {

    /**
     * The number of characters to use as initial margin for
     * copyFileAndConvert().
     */
    private static final int STRING_BUFFER_MARGIN = 100;

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(IO.class);

    /**
     * The number of kilobytes to read per chunk for copyFileAndConvert()
     *
     */
    static final int MAX_FILE_BUFFER_KILOBYTES = 100;

    /**
     * Deletes the content of directory <code>directory</code> but keeps the
     * directory <code>directory</code>.
     *
     * @param dir
     *            The directory that should be emptied.
     * @throws IOException
     *             if the deletion failed.
     */
    public static void deleteDirContent(File dir) throws IOException {
        deleteDirContent(dir, true);
    }

    /**
     * Recursive help method to {@link #deleteDirContent(File)}.
     *
     * @param file
     *            the file or directory that should be deleted.
     * @param root
     *            <code>true</code> if the <code>file</code> should be kept.
     * @throws IOException
     *             if the deletion failed.
     */
    private static void deleteDirContent(File file, boolean root)
            throws IOException {
        if (file.isDirectory()) {
            for (File c : file.listFiles())
                deleteDirContent(c, false);
        }
        if (!root) {
            if (!file.delete()) {
                throw new IOException("Failed to delete file: " + file);
            } else {
                logger.info("{} was deleted", file.getAbsolutePath());
            }
        }
    }

    /**
     * Copies the directory <code>sourceLocation</code> to
     * <code>destLocation</code>.
     *
     * If the contents of the sourceLocation needs conversion to HTML, such
     * conversion is done, and the destination file will get ".html" appended to
     * its filename.
     *
     * Similarly, if the file to copy is a HTML file, local <a href> links are
     * converted to link to the corresponding .html file.
     *
     * @param sourceLocation
     *            the directory that will be copied.
     * @param destLocation
     *            the directory that will be a copy.
     * @param optionallyConvertToHtml
     *            if true, convert the file to HTML
     * @param convertHtmlLinks
     *            Replace <a href> links, if required during copying.
     * @throws IOException
     *             indicates a failed copy.
     */
    public static void copyDirectory(File sourceLocation, File destLocation,
            boolean optionallyConvertToHtml, boolean convertHtmlLinks)
            throws IOException {

        if (sourceLocation.isDirectory()) {
            if (!destLocation.exists()) {
                if (!destLocation.mkdir()) {
                    throw new IOException(destLocation.getAbsolutePath()
                            + " Could not be created.");
                }
            } else if (!destLocation.isDirectory()) {
                logger.error("{} is not a directory", destLocation);
                throw new IOException(destLocation + " is not a directory");
            }
            String[] children = sourceLocation.list();
            for (int i = 0; i < children.length; i++) {
                copyDirectory(new File(sourceLocation, children[i]), new File(
                        destLocation, children[i]), optionallyConvertToHtml,
                        convertHtmlLinks);
            }
        } else if (optionallyConvertToHtml) {
            copyFileAndOptionallyConvert(sourceLocation, destLocation,
                    convertHtmlLinks);
        } else {
            copyFile(sourceLocation, destLocation);
        }
    }

    /**
     * Copies the file <code>sourceFile</code> to <code>destFile</code>.
     * <p>
     * The method makes use of java.nio.channels.FileChannel.
     *
     * @param sourceFile
     *            the file that will be copied.
     * @param destFile
     *            the file that will be a copy.
     * @throws IOException
     *             indicates a failed copy.
     */
    public static void copyFile(File sourceFile, File destFile)
            throws IOException {
        if (!destFile.exists()) {
            if (!destFile.createNewFile()) {
                throw new IOException(destFile.getAbsolutePath()
                        + " Could not be created.");
            }
        }

        FileChannel source = null;
        FileChannel destination = null;
        try {
            source = new FileInputStream(sourceFile).getChannel();
            destination = new FileOutputStream(destFile).getChannel();
            destination.transferFrom(source, 0, source.size());
            logger.debug(sourceFile + " copied to " + destFile);
        } finally {
            if (source != null) {
                source.close();
            }
            if (destination != null) {
                destination.close();
            }
        }
    }

    /**
     * Copies the file <code>sourceFile</code> to <code>destFile</code> and
     * converts any HTML escape characters to &amp; &lt; or &gt.
     * <p>
     * The destination file will also contain <code>html</code>,
     * <code>head</code> and <code>body</code> tags.
     *
     * @param sourceFile
     *            The file that will be copied.
     * @param destFile
     *            The file that will be a copy.
     * @param title
     *            The title, which is added to the <code>title</code> tag.
     * @throws IOException
     *             indicates a failed copy.
     */
    static void copyFileAndConvertToHtml(File sourceFile, File destFile,
            String title) throws IOException {
        if (!destFile.exists()) {
            if (!destFile.createNewFile()) {
                throw new IOException(destFile.getAbsolutePath()
                        + " Could not be created.");
            }
        }

        FileReader source = null;
        FileWriter destination = null;
        try {
            source = new FileReader(sourceFile);
            destination = new FileWriter(destFile);
            destination.write("<html><head><title>" + title
                    + "</title></head><body><pre>\n");

            transferFromAndEscapeHtml(source, destination);

            destination.write("</pre></body></html>\n");
            logger.debug(sourceFile + " copied and converted to HTML to "
                    + destFile);
        } finally {
            if (source != null) {
                source.close();
            }
            if (destination != null) {
                destination.close();
            }
        }
    }

    /**
     * Transfer data from one Reader to a Writer, escaping HTML control
     * characters as they appear.
     *
     * @param source
     *            The source.
     * @param destination
     *            The destination.
     * @throws IOException
     *             If IO errors occurrs.
     */
    private static void transferFromAndEscapeHtml(Reader source,
            Writer destination) throws IOException {
        boolean done = false;
        int maxLength = MAX_FILE_BUFFER_KILOBYTES * 1024;
        char buffer[] = new char[maxLength];
        do {
            int actualLength = source.read(buffer, 0, maxLength);
            if (actualLength <= 0) {
                done = true;
            } else {
                String s = new String(buffer, 0, actualLength);
                String s2 = escapeHtml(s);
                destination.write(s2);
            }
        } while (!done);
    }

    /**
     * Escape HTML-control charactrs. The HTML control characters &, < and > are
     * converted into &amp;, &lt; and &gt.
     *
     * @param s
     *            The string
     * @return A converted string
     */
    static String escapeHtml(String s) {
        StringBuilder sb = new StringBuilder(s.length() + STRING_BUFFER_MARGIN);
        int max = s.length();
        for (int index = 0; index < max; index++) {
            char c = s.charAt(index);
            switch (c) {
                case '&' :
                    sb.append("&amp;");
                    break;
                case '<' :
                    sb.append("&lt;");
                    break;
                case '>' :
                    sb.append("&gt;");
                    break;
                default :
                    sb.append(c);
            }

        }
        String s2 = sb.toString();
        return s2;
    }

    /**
     * Copies the file <code>sourceFile</code> to <code>destFile</code>,
     * assuming the file is a HTML file, and converts any <a href> pointing to
     * local files to links into the same local files with the ".html" ending.
     * <p>
     * This method is using the same criteria as
     * copyFileAndOptionallyConvertToHtml() in order to determinate if the link
     * shall be modified or not.
     *
     * @param sourceFile
     *            the file that will be copied.
     * @param destFile
     *            the file that will be a copy.
     * @throws IOException
     *             indicates a failed copy.
     */
    public static void copyFileAndReplaceAHref(File sourceFile, File destFile)
            throws IOException {
        if (!destFile.exists()) {
            if (!destFile.createNewFile()) {
                throw new IOException(destFile.getAbsolutePath()
                        + " Could not be created.");
            }
        }

        FileReader source = null;
        FileWriter destination = null;
        try {
            source = new FileReader(sourceFile);
            destination = new FileWriter(destFile);

            transferFromAndReplaceAHref(source, destination, sourceFile
                    .getAbsolutePath());
            logger.debug(sourceFile + " copied and modified local links to "
                    + destFile);
        } finally {
            if (source != null) {
                source.close();
            }
            if (destination != null) {
                destination.close();
            }
        }
    }

    /**
     * Transfer data from one from a reader to a writer, assuming it is a html
     * stream, converting <a href> tags as they appears.
     *
     * @param source
     *            The source.
     * @param destination
     *            The destination.
     * @throws IOException
     *             If IO error occurrs.
     */
    static void transferFromAndReplaceAHref(Reader source, Writer destination,
            String filename) throws IOException {
        HTMLEditorKit.Parser parser = new ParserDelegator();

        ParserCallback cb = new HtmlAHrefConverter(destination, filter,
                filename);
        parser.parse(source, cb, true);
    }

    static KernelDocumentationFilenameFilterer filter = new KernelDocumentationFilenameFilterer();

    /**
     * Copies the file <code>sourceFile</code> to <code>destFile</code> and
     * converts the file to HTML, in case the file is a pure text file. In that
     * case, the name is modified to include ".html"
     * <p>
     * If the file to copy is a HTML file, optionally modify <a href> to point
     * to .html files in the same way.
     *
     * @param srcFile
     *            the file that will be copied.
     * @param destFile
     *            the file that will be a copy.
     * @param convertLinks
     *            If true, will convert <a href> links.
     * @throws IOException
     *             indicates a failed copy.
     */
    public static void copyFileAndOptionallyConvert(File srcFile,
            File destFile, boolean convertLinks) throws IOException {
        String srcFileNameLC = srcFile.getName().toLowerCase();
        if (filter.accept(null, srcFile.getName())) {
            File newName = new File(destFile.getPath() + ".html");
            IO.copyFileAndConvertToHtml(srcFile, newName, srcFile.getName());
        } else if (convertLinks
                && (srcFileNameLC.endsWith(".html") || srcFileNameLC
                        .endsWith(".htm"))) {
            IO.copyFileAndReplaceAHref(srcFile, destFile);
        } else {
            IO.copyFile(srcFile, destFile);
        }
    }
}
