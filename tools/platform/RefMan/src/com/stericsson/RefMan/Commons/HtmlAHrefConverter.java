/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons;

import java.io.FilenameFilter;
import java.io.IOException;
import java.io.Writer;
import java.util.Enumeration;

import javax.swing.text.MutableAttributeSet;
import javax.swing.text.html.HTML.Tag;
import javax.swing.text.html.HTMLEditorKit.ParserCallback;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Converts tags of type <a href>, by writing the contents of the HTML input to
 * a new Writer.
 * <p>
 * Links that point to local, non-HTML files, are converted into links to the
 * same file with a ".html" extension, using a FilenameFilter that determines if
 * the link shall be converted or not. The FilenameFilter.accept() shall return
 * true if the link shall be converted.
 *
 * @author qfrelun
 *
 */
final class HtmlAHrefConverter extends ParserCallback {
    /** The logger. */
    final static Logger logger = LoggerFactory
            .getLogger(HtmlAHrefConverter.class);

    /**
     * The Writer to write the output to.
     *
     */
    private Writer destination;

    /**
     * The FilenameFilter which determines if the link shall be converted.
     */
    private FilenameFilter filenameFilter;

    /**
     * The name of the file under conversion.
     */
    private String filename;

    /**
     * Default constructor. Uses a MustConvertToHtmlFilenameFilterer as
     * FilenameFilter. Mainly used for tests.
     *
     * @param destination
     *            The Writer to write the output to.
     */
    HtmlAHrefConverter(Writer destination) {
        this.destination = destination;
        filenameFilter = new MustConvertToHtmlFilenameFilterer();
    }

    /**
     * Constructor
     *
     * @param destination
     *            The Writer to write the output to.
     * @param filter
     *            The FilenameFilter which determines if the link shall be
     *            converted. The FilenameFilter.accept() shall return true if
     *            the link shall be converted.
     * @param filename
     *            The name of the file under conversion.
     */

    public HtmlAHrefConverter(Writer destination, FilenameFilter filter,
            String filename) {
        this.destination = destination;
        filenameFilter = filter;
        this.filename = filename;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * javax.swing.text.html.HTMLEditorKit.ParserCallback#handleComment(char[],
     * int)
     */
    @Override
    public void handleComment(char[] text, int pos) {
        logger.trace("handleComment: pos={}, text='{}'", pos, text);
        try {
            destination.write("<!--");
            destination.write(text);
            destination.write("-->");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * javax.swing.text.html.HTMLEditorKit.ParserCallback#handleError(java.lang
     * .String, int)
     */
    @Override
    public void handleError(String error, int pos) {
        logger.debug("handleError: '{}': pos={}, error='{}'", new Object[]{
                filename, pos, error});

        super.handleError(error, pos);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * javax.swing.text.html.HTMLEditorKit.ParserCallback#handleSimpleTag(javax
     * .swing.text.html.HTML.Tag, javax.swing.text.MutableAttributeSet, int)
     */
    @Override
    public void handleSimpleTag(Tag tag, MutableAttributeSet mas, int pos) {
        logger.trace("handleSimpleTag: pos={}, tag='{}'", pos, tag);
        Enumeration<?> attributeNames = mas.getAttributeNames();
        try {
            destination.write('<');
            destination.write(tag.toString());
            for (; attributeNames.hasMoreElements();) {
                Object attributeNameObject = attributeNames.nextElement();
                String attributeName = attributeNameObject.toString();
                Object attributeValueObject = mas
                        .getAttribute(attributeNameObject);
                String attributeValue = attributeValueObject.toString();
                logger.trace("handleSimpleTag: mas='{}'='{}'", attributeName,
                        attributeValue);
                destination.write(' ');
                destination.write(attributeName.toString());
                destination.write('=');
                destination.write('"');
                destination.write(attributeValue);
                destination.write('"');
            }
            destination.write('>');
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * javax.swing.text.html.HTMLEditorKit.ParserCallback#handleText(char[],
     * int)
     */
    @Override
    public void handleText(char[] text, int pos) {
        logger.trace("handleText: pos={}, text='{}'", pos, text);
        try {
            destination.write(text);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see javax.swing.text.html.HTMLEditorKit.ParserCallback#handleEndTag(Tag,
     * int)
     */
    @Override
    public void handleEndTag(Tag tag, int pos) {
        logger.trace("handleEndTag: pos={}, tag='{}'", pos, tag);
        try {
            destination.write("</");
            destination.write(tag.toString());
            destination.write('>');
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * javax.swing.text.html.HTMLEditorKit.ParserCallback#handleStartTag(Tag,
     * MutableAttributeSet, int)
     */
    @Override
    public void handleStartTag(Tag tag, MutableAttributeSet mas, int pos) {
        logger.trace("handleSimpleTag: pos={}, tag='{}'", pos, tag);
        Enumeration<?> attributeNames = mas.getAttributeNames();
        try {
            destination.write('<');
            destination.write(tag.toString());
            for (; attributeNames.hasMoreElements();) {
                Object attributeNameObject = attributeNames.nextElement();
                String attributeName = attributeNameObject.toString();
                Object attributeValueObject = mas
                        .getAttribute(attributeNameObject);
                String attributeValue = attributeValueObject.toString();
                logger.trace("handleSimpleTag: mas='{}'='{}'", attributeName,
                        attributeValue);
                destination.write(' ');
                destination.write(attributeName);
                destination.write('=');
                destination.write('"');
                if (Tag.A == tag
                        && attributeName.compareToIgnoreCase("href") == 0) {
                    attributeValue = convertHref(attributeValue);
                }
                destination.write(attributeValue);
                destination.write('"');
            }
            destination.write('>');
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Converts a link, if the link points to a local file. The link gets .html
     * appended, unless the link point to a remote URL, by checking the
     * FilenameFilter.accept().
     *
     * @param link
     *            The contents of a link
     * @return
     */

    String convertHref(String link) {
        if (filenameFilter instanceof MustConvertToHtmlFilenameFilterer
                && ((MustConvertToHtmlFilenameFilterer) filenameFilter)
                        .linkContainsProtocol(link)) {
            logger.debug("convertHref: URL is remote, ignoring: '{}'", link);
            return link;
        } else if (link.contains("#")) {
            logger.debug(
                    "convertHref: URL points to .html with #, ignoring:_ '{}'",
                    link);
            return link;
        } else {
            logger.debug("convertHref: URL is local, checking: '{}'", link);
            String newValue = link;
            String lastName = getFilePart(link);
            boolean replace = filenameFilter.accept(null, lastName);
            if (replace) {
                newValue = link + ".html";
                logger.debug("convertHref: Replacing URL '{}' with '{}'", link,
                        newValue);
            }

            return newValue;
        }
    }

    /**
     * Returns the last file part of a link
     *
     * @param link
     *            The full link.
     * @return The file part of the link.
     */
    private String getFilePart(String link) {
        int lastSlashPos = link.lastIndexOf('/');
        String lastName = link;
        if (lastSlashPos >= 0) {
            lastName = link.substring(lastSlashPos + 1);
        }
        return lastName;
    }

}