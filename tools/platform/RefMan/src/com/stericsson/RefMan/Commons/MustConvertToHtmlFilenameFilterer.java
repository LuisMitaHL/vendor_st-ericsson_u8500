/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons;

import java.io.File;
import java.io.FilenameFilter;
import java.net.MalformedURLException;
import java.util.HashSet;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A FilenameFilter which can be used to detect if a file, or a link that point
 * to local, non-HTML files, needs to converted into to the same file with a
 * ".html" extension.
 *
 * The name is checked against a list of known common extensions, as well as
 * some common filenames.
 *
 * The FilenameFilter.accept() returns true if the link/file shall be converted.
 * The default is that no conversion is done.
 *
 * @author qfrelun
 *
 */
class MustConvertToHtmlFilenameFilterer implements FilenameFilter {
    /** The logger */
    public final static Logger logger = LoggerFactory
            .getLogger(MustConvertToHtmlFilenameFilterer.class);

    /**
     * Special set of filenames that cannot be handled by the generic algorithm,
     * but needs to be converted anyway.
     */
    private HashSet<String> specialFileNameList = new HashSet<String>();

    /**
     * Constructor.
     *
     * @param specialFileNamesToConvert
     *            An optional set of filenames that cannot be handled by the
     *            generic algorithm, but needs to be converted anyway.
     */
    public MustConvertToHtmlFilenameFilterer(
            HashSet<String> specialFileNamesToConvert) {
        specialFileNameList = specialFileNamesToConvert;
    }

    /**
     * Constructor.
     */
    public MustConvertToHtmlFilenameFilterer() {
    }

    /**
     * Returns true if the link/file shall be converted.
     *
     * @param dir
     *            Ignored by this FilenameFilter, can be set to null.
     * @param name
     *            Filename or link to check.
     * @return true if the link/file shall be converted.
     */
    public boolean accept(File dir, String name) {
        return mustConvertFileName(name);
    }

    /**
     * Returns true if the link/file shall be converted. If the link is pointing
     * to common protocols, or is already a .html file, false is always
     * returned. If the file is included in the specialFileNamesToConvert set,
     * true is always returned. Otherwise, true is returned if the filename
     * indicates that it is a common text format. False is always returned if
     * the filename indicates that it is a common binary format or a known text
     * format that shall not be converted. The default is that no conversion is
     * done.
     *
     * @param filePart
     *            The file part of the link or filename.
     * @return true if the link/file shall be converted.
     */
    private boolean mustConvertFileName(String filePart) {
        if (linkContainsProtocol(filePart)) {
            return false;
        }

        if (mustNotConvertAnchorLinkName(filePart)) {
            return false;
        }

        int lastDotPos = filePart.lastIndexOf('.');
        if (lastDotPos >= 0) {
            String extension = filePart.substring(lastDotPos + 1);
            if (mustNotConvertExtension(extension)) {
                return false;
            }

            if (mustConvertExtension(extension)) {
                return true;
            } else {
                String nonExtension = filePart.substring(0, lastDotPos);
                if (mustConvertNonExtension(nonExtension)
                        || mustConvertSpecialFileName(filePart)) {
                    return true;
                } else {
                    logger
                            .info(
                                    "mustConvertFileName: Extension unknown: '{}' for file '{}'",
                                    extension, filePart);
                    return false;
                }
            }
        } else {
            // No extension at all
            return true;
        }
    }

    /**
     * Returns true if the link contains a link anchor.
     *
     * @param link
     *            The link name
     * @return true if the link contains a link anchor.
     */
    private boolean mustNotConvertAnchorLinkName(String link) {
        int htmlLength = ".html".length();
        int htmLength = ".htm".length();

        int hashPos = link.indexOf('#');
        if (hashPos >= htmlLength) {
            String htmlSubString = link.substring(hashPos - htmlLength + 1,
                    hashPos + 1);
            return htmlSubString.equalsIgnoreCase("html#")
                    || htmlSubString.equalsIgnoreCase(".htm#");
        } else if (hashPos >= htmLength) {
            String htmSubString = link.substring(hashPos - htmLength + 1,
                    hashPos + 1);
            return htmSubString.equalsIgnoreCase(".htm");
        } else if (hashPos == 0) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns true if the link contains a common protocol.
     *
     * @param link
     *            The link.
     * @return true if the link contains a common protocol.
     */
    boolean linkContainsProtocol(String link) {
        try {
            java.net.URL url = new java.net.URL(link);
            if (url.getProtocol() != null && url.getProtocol().length() > 0) {
                return true;
            } else {
                return false;
            }
        } catch (MalformedURLException e) {
            return false;
        }

    }

    /**
     * Returns true if the link or filename is included in the
     * specialFileNameList set.
     *
     * @param name
     *            The link or filename.
     * @return true if the link or filename is included in the
     *         specialFileNameList set.
     */
    private boolean mustConvertSpecialFileName(String name) {
        if (specialFileNameList.contains(name)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns true if the link or filename is one of some common extensionless
     * text files.
     *
     * @param nonExtensionName
     *            The link or filename.
     * @return true if the link or filename is one of some common extensionless
     *         text files.
     */
    private boolean mustConvertNonExtension(String nonExtensionName) {
        return nonExtensionName.equals("README")
                || nonExtensionName.equals("ChangeLog")
                || nonExtensionName.equals("LICENSE");
    }

    /**
     * Returns true if the extension of the link or filename is one of the
     * common text formats.
     *
     * @param extension
     *            The extension
     * @return true if the extension of the link or filename is one of the
     *         common text formats.
     */
    private boolean mustConvertExtension(String extension) {
        return extension.equalsIgnoreCase("txt")
                || extension.equalsIgnoreCase("c")
                || extension.equalsIgnoreCase("cpp")
                || extension.equalsIgnoreCase("cc")
                || extension.equalsIgnoreCase("cxx")
                || extension.equalsIgnoreCase("h")
                || extension.equalsIgnoreCase("hpp")
                || extension.equalsIgnoreCase("hh")
                || extension.equalsIgnoreCase("pl")
                || extension.equalsIgnoreCase("sh")
                || extension.equalsIgnoreCase("java");
    }

    /**
     * Returns true if the extension of the link or filename is one of the
     * common binary formats or formats that shall not be converted.
     *
     * @param extension
     *            The extension.
     * @return true if the extension of the link or filename is one of the
     *         common binary formats or formats that shall not be converted.
     */
    private boolean mustNotConvertExtension(String extension) {
        return extension.equalsIgnoreCase("pdf")
                || extension.equalsIgnoreCase("html")
                || extension.equalsIgnoreCase("htm")
                || extension.equalsIgnoreCase("doc") ||

                extension.equalsIgnoreCase("tex")
                || extension.equalsIgnoreCase("xsl")
                || extension.equalsIgnoreCase("xml")
                || extension.equalsIgnoreCase("tmpl") ||

                extension.equalsIgnoreCase("jpg")
                || extension.equalsIgnoreCase("jpeg")
                || extension.equalsIgnoreCase("gif")
                || extension.equalsIgnoreCase("svg")
                || extension.equalsIgnoreCase("png") ||

                extension.equalsIgnoreCase("ps")
                || extension.equalsIgnoreCase("tif")
                || extension.equalsIgnoreCase("tiff") ||

                extension.equals("gitignore");
    }

}