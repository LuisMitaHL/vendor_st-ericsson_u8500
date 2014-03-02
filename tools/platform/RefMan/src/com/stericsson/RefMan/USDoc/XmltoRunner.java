/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.File;
import java.io.IOException;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.BashCreator;
import com.stericsson.RefMan.Commons.IO;
import com.stericsson.RefMan.Exception.IncludeDocNotSpecifiedException;
import com.stericsson.RefMan.TemplateComparator.XMLVerifier;
import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.ElementFactory;
import com.stericsson.RefMan.Toc.Script;
import com.stericsson.RefMan.Toc.StdAPIElement;

/**
 * The XmlToRunner class manages xmlto-related processing.
 *
 * The run method transforms the documentation referenced in the toc-locations
 * file to html, using xmlto. It also copies the files stated as include files,
 * and applies the required post-processing.
 *
 * @author qfrelun
 *
 */
public class XmltoRunner extends Thread {

    /** The logger */
    final Logger logger = LoggerFactory.getLogger(XmltoRunner.class);

    /**
     * The <code>File</code> representing the standard API toc-locations file.
     */
    private final File stdApiTocFile;

    /** The <code>File</code> representing the project root directory. */
    private final File projectRootDir;

    /**
     * The <code>File</code> representing the folder where the <code>HTML</code>
     * documentation will be placed.
     */
    private final File htmlDir;

    /**
     * A <code>StdAPIElement</code> representing the content of the
     * <code>stdApiTocFile</code>.
     */
    private StdAPIElement stdApiElement;

    /**
     * The temporary directory.
     */
    private final File tempDir;

    /**
     * The name of the temporary output directory for xmlto.
     */
    private String tempOutputFolderForXmlTo;

    /**
     * The name of the desired destination directory for the output.
     */
    private String destOutputFolderForXmlTo;

    /**
     * template file for standard API docbook
     */
    final private File docbookXMLTemplate = new File("resources"
            + File.separator + "StdApi_docbook_template.xml");

    /**
     * The counter that is used to differentiate the different modules that will
     * be included in the manual.
     */
    private final int cnt;

    /**
     * Constructor.
     *
     * @param stdApiTocFile
     *            The <code>File</code> representing the standard API
     *            toc-locations file
     * @param projectRootDir
     *            The <code>File</code> representing the project root directory
     * @param htmlDir
     *            The <code>File</code> representing the folder where the
     *            <code>HTML</code> documentation will be placed.
     * @param tempDir
     *            The <code>File</code> representing the temporary directory
     * @param cnt
     *            The counter that is used to differentiate the different
     *            modules that will be included in the manual.
     */
    public XmltoRunner(File stdApiTocFile, File projectRootDir, File htmlDir,
            File tempDir, int cnt) {
        this.stdApiTocFile = stdApiTocFile;
        this.projectRootDir = projectRootDir;
        this.htmlDir = htmlDir;
        this.tempDir = tempDir;
        this.cnt = cnt;
    }

    /**
     * The method transforms the referenced documentation from docbook format to
     * html, by starting xmlto. It also copies the files stated as files to
     * include, to the <code>htmlDir</code>. During copying, the files indicated
     * as files to include, are transformed to HTML using <code>pre</code> tags,
     * if they aren't already HTML or binary. Also, the links in the DocBook
     * <code>ulink</code> are converted to point to the HTML version of the
     * converted files.
     */
    @Override
    public void run() {
        String fileName = stdApiTocFile.getName();
        logger.info("Standard API toc found: {}", stdApiTocFile
                .getAbsolutePath());
        File destFile = new File(tempDir, cnt + "-" + fileName);
        try {
            IO.copyFile(stdApiTocFile, destFile);
        } catch (IOException e) {
            logger.error("Unable to copy the file {}: " + e, stdApiTocFile
                    .getAbsolutePath());
            return;
        }

        stdApiElement = getStdApiElement();
        transformDocbookToHtml();
        runExternalScripts();
        copyIncludeFiles();
        copyDocBookFiles();
    }

    /**
     * Copies the generated output from the temporary output directory to the
     * desired output directory, converting the <a href> links to the correct
     * files.
     */
    private void copyDocBookFiles() {
        logger.info("Copying DocBookFiles files");
        try {
            IO.copyDirectory(new File(tempOutputFolderForXmlTo), new File(
                    destOutputFolderForXmlTo), true, true);
        } catch (IOException e) {
            logger
                    .warn(
                            "Unable to copy the generated HTML files for {} from {} to {}",
                            new Object[]{stdApiElement.getModuleName(),
                                    tempOutputFolderForXmlTo,
                                    destOutputFolderForXmlTo}, e);
        }
    }

    /**
     * Method which generate an internal file structure representing the
     * standard API toc file.
     *
     * @return A <code>StdAPIElement</code> representing the toc file.
     */
    private StdAPIElement getStdApiElement() {
        Element tocElm = ElementFactory.getElement(stdApiTocFile, 0);
        return (StdAPIElement) tocElm.getTopics().get(0).getTopics().get(0)
                .getTopics().get(0);
    }

    /**
     * Method which start the transformation of the documentation from docbook
     * format into html format. It executes xmlto for the specified file.
     */
    private void transformDocbookToHtml() {
        File outputFolder = new File(htmlDir, stdApiElement.getModuleName());
        outputFolder.mkdirs();
        File tempFolder = new File(tempDir, stdApiElement.getModuleName());
        tempFolder.mkdirs();
        tempOutputFolderForXmlTo = tempFolder.getAbsolutePath();
        destOutputFolderForXmlTo = outputFolder.getAbsolutePath();
        String fullPathToTocFile = stdApiTocFile.getParent() + File.separator
                + stdApiElement.getHref();
        // validate docbook xml file
        if (!validateDocBookFile(fullPathToTocFile)) {
            logger
                    .error("Docbook file: "
                            + fullPathToTocFile
                            + " referenced from "
                            + stdApiTocFile.getAbsolutePath()
                            + " does not follow guidelines. See log above for further details.");
            return;
        }
        String xmlto = "xmlto -m resources/stylesheet.xsl -o "
                + BashCreator.convertPath(tempOutputFolderForXmlTo) + " html "
                + BashCreator.convertPath(fullPathToTocFile);
        String[] cmdArr = {BashCreator.getBash(), "-c", xmlto};
        ProcessBuilder pb = new ProcessBuilder(cmdArr);

        logger.info("Transforming '{}' to html with xmlto", stdApiElement
                .getHref());

        Process proc;
        try {
            proc = pb.start();
            // any error message?
            StreamGobbler errorGobbler = new StreamGobbler(proc
                    .getErrorStream(), "STDERR", "xmlto");

            // any output?
            StreamGobbler outputGobbler = new StreamGobbler(proc
                    .getInputStream(), "STDOUT", "xmlto");

            // kick them off
            errorGobbler.start();
            outputGobbler.start();
            proc.waitFor();
            int exitcode = proc.exitValue();
            if (exitcode != 0) {
                logger.error(
                        "XMLTO transform for '{}' failed with exit code {}",
                        fullPathToTocFile, exitcode);
            } else {
                logger.info(
                        "XMLTO transform for '{}' succeeded with exit code {}",
                        fullPathToTocFile, exitcode);
            }
        } catch (IOException e) {
            logger.warn("Failed to transform '{}'", stdApiElement.getHref(), e);
        } catch (InterruptedException e) {
            logger.error("Unexpected InterruptedException {}", e);
        }
    }

    /**
     * Verifies if the StdApi docbook file provided conforms to a template
     *
     * @param docBookFileName
     *            - path to the file that should be verified against the
     *            template
     * @return true if file conforms to template, false otherwise
     */
    private boolean validateDocBookFile(String docBookFileName) {
        XMLVerifier verifier = new XMLVerifier();
        File docBookFile = new File(docBookFileName);

        if (!docBookFile.isFile()) {
            logger.error("Could not locate file:" + docBookFileName);
            return false;
        }

        return verifier.compareFileToTemplateFile(docbookXMLTemplate,
                docBookFile);
    }

    /**
     * Method which copies the files stated as files to include in the
     * toc-locations file to the <code>htmlDir</code>. The files are converted
     * to HTML, if required.
     */
    private void copyIncludeFiles() {
        logger.info("Copying include files defined in {}", stdApiTocFile
                .getAbsolutePath());
        String stdApiFilePath = stdApiTocFile.getAbsolutePath();
        try {
            stdApiElement.calculatePaths(new File(stdApiFilePath),
                    projectRootDir.getAbsolutePath());
            // Copy the directories in 'includedirectory' tags to the output
            // folder.
            if (stdApiElement.getIncludeDirs() != null) {
                stdApiElement.copyIncludeFilesDirectory(htmlDir);
            }
            // Copy the files in 'includefile' tags to the output folder.
            if (stdApiElement.getIncludeFiles() != null) {
                stdApiElement.copyIncludeFiles(htmlDir);
            }
        } catch (IOException e) {
            logger.error(e.getMessage());
        } catch (IncludeDocNotSpecifiedException e) {
            logger.error("Failed to copy include files for '{}'",
                    stdApiFilePath);
        }
    }

    /**
     * Method which starts the external scripts, specified in the
     * toc-locations.xml file
     */
    private void runExternalScripts() {
        List<Script> scripts = stdApiElement.getExternalScripts();
        for (Script script : scripts) {
            String externalScript = script.getScript();
            String startDir = stdApiTocFile.getParent() + File.separator
                    + script.getStartDir();
            String[] cmdArr = {BashCreator.getBash(), "-c", externalScript};

            ProcessBuilder pb = new ProcessBuilder(cmdArr);
            pb.directory(new File(startDir));
            logger.info("Starting external script from {}", stdApiTocFile
                    .getAbsolutePath());
            Process proc;
            try {
                proc = pb.start();
                // any error message?
                StreamGobbler errorGobbler = new StreamGobbler(proc
                        .getErrorStream(), "STDERR", "External script");

                // any output?
                StreamGobbler outputGobbler = new StreamGobbler(proc
                        .getInputStream(), "STDOUT", "External script");

                // kick them off
                errorGobbler.start();
                outputGobbler.start();
                proc.waitFor();
                int exitcode = proc.exitValue();
                if (exitcode != 0) {
                    logger
                            .error(
                                    "External script '{}' failed to execute with exitcode {}",
                                    externalScript, exitcode);
                } else {
                    logger.info(
                            "External script '{}' succeeded with exit code {}",
                            externalScript, exitcode);
                }
            } catch (IOException e) {
                logger.warn("Failed to execute external script from '{}'",
                        stdApiTocFile.getAbsolutePath(), e);
            } catch (InterruptedException e) {
                logger.error("Unexpected InterruptedException {}", e);
            }
        }
    }
}