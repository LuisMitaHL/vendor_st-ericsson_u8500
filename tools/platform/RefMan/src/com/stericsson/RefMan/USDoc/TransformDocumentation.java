/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.File;
import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.ElementFactory;

public class TransformDocumentation {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TransformDocumentation.class);

    /** The project root directory */
    private final File projectRootDir;

    /** The html directory */
    private final File htmlDir;

    /** The temp directory */
    private final File tempDir;

    /**
     * Constructor for <code>TransformDocumentation</code>.
     *
     * @param htmldir
     *            The html directory
     * @param tempDir
     *            The temp directory
     * @param projectRootDir
     *            The project root directory
     */
    public TransformDocumentation(File htmlDir, File tempDir,
            File projectRootDir) {
        this.htmlDir = htmlDir;
        this.tempDir = tempDir;
        this.projectRootDir = projectRootDir;
    }

    /**
     * This method starts the transformation of the documentation, using doxygen
     * for proprietary modules and Xmlto for standard APIs.
     *
     * @param tocFilePath
     *            A file representing the toc file.
     * @param cnt
     *            A number to differentiate between modules
     * @throws IOException
     */
    public Thread transformDocumentation(File tocFile, int cnt)
            throws IOException {
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        // Get the type of the toc file, 'topic' or 'stdapi'.
        String tocType = tocElm.getTopics().get(0).getTopics().get(0)
                .getTopics().get(0).getType();
        Thread thread = null;
        if (tocType.equalsIgnoreCase("topic")) {
            // If the module is a proprietary module, the documentation
            // should be transformed with doxygen.
            thread = startDoxygen(tocElm, tocFile, cnt);
        } else if (tocType.equalsIgnoreCase("stdapi")) {
            // If the module is a standard API, the documentation
            // should be
            // transformed with xmlto instead.
            thread = startXmlto(tocFile, cnt);
        }
        return thread;
    }

    /**
     * A method which starts <code>DoxygenRunner</code> proprietary modules.
     *
     * @param tocElm
     *            An <code>Element</code> representing the toc file.
     * @param toc
     *            A file representing the toc file.
     * @param cnt
     *            A number to differentiate between modules
     */
    private Thread startDoxygen(Element tocElm, File toc, int cnt) {
        DoxygenRunner doxygenRunner = new DoxygenRunner(tocElm, toc, htmlDir,
                tempDir, cnt);
        logger.debug("Start of doxygen from '{}'", toc.getAbsolutePath());
        doxygenRunner.start();
        return doxygenRunner;
    }

    /**
     * A method which starts <code>XmltoRunner</code> for standard APIs.
     *
     * @param toc
     *            The toc file for the module.
     * @throws IOException
     */
    private Thread startXmlto(File toc, int cnt) throws IOException {
        XmltoRunner xmltoRunner = new XmltoRunner(toc, projectRootDir, htmlDir,
                tempDir, cnt);
        logger.debug("Start of xmlto from '{}'", toc.getAbsolutePath());
        xmltoRunner.start();
        return xmltoRunner;
    }
}
