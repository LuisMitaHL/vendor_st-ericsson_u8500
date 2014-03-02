/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.Vector;

import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.BashCreator;
import com.stericsson.RefMan.Exception.NotADirectoryException;
import com.stericsson.RefMan.Exception.NotAUserSpaceTocException;
import com.stericsson.RefMan.Exception.StdAPIFoundException;
import com.stericsson.RefMan.Toc.DocumentFactory;
import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.ElementFactory;
import com.stericsson.RefMan.Toc.TopicElement;

/**
 * A class that have methods for creating individual doxygen configuration
 * files, tocs and starting doxygen as well as take care of the input / error
 * streams from doxygen.
 *
 * @author Thomas Palmqvist
 *
 */
public class DoxygenRunner extends Thread {

    /** The logger */
    private final static Logger logger = LoggerFactory
            .getLogger(DoxygenRunner.class);

    /**
     * The root <code>Element</code> of the toc file.
     */
    protected Element tocElement;

    /**
     * The <code>File</code> representing the toc-locations.xml file.
     */
    protected File tocFile;

    /**
     * The counter that is used to differentiate the different modules that will
     * be included in the manual.
     */
    protected int cnt;

    /**
     * The path to the folder where the <code>HTML</code> documentation will be
     * placed.
     */
    protected File htmlDir;

    /**
     * The path to the folder where the temporary files will be placed these
     * files will also be useful for debugging if something goes wrong.
     */
    protected File tempDir;

    /**
     * The path to the folder where the temporary files will be placed these
     * files will also be useful for debugging if something goes wrong.
     */
    protected File refManDir;

    /**
     * The constructor for <code>DoxygenRunner</code>.
     *
     * @param tocElm
     *            The root <code>Element</code> of the toc file.
     * @param tocFile
     *            The <code>File</code> representing the toc-locations.xml file.
     * @param htmlDir
     *            The path to the folder where the <code>HTML</code>
     *            documentation will be placed.
     * @param tempDir
     *            The path to the folder where the temporary files will be
     *            placed these files will also be useful for debugging if
     *            something goes wrong.
     * @param stelpDir
     *            The path to the folder for the Linux Kernel.
     * @param cnt
     *            The counter that is used to differentiate the different
     *            modules that will be included in the manual.
     */
    public DoxygenRunner(Element tocElm, File tocFile, File htmlDir,
            File tempDir, int cnt) {
        this.tocElement = tocElm;
        this.tocFile = tocFile;
        this.htmlDir = htmlDir;
        this.tempDir = tempDir;
        this.refManDir = tempDir.getParentFile();
        this.cnt = cnt;

    }

    /**
     * Crates a toc <code>File</code> which matches the tool's output.
     *
     * @param tocElm
     *            The root <code>Element</code> of the toc file.
     * @param tocFile
     *            The <code>File</code> representing the toc-locations.xml file.
     * @return A new Toc <code>File</code> that has a correct <code>href</code>
     *         that matches the tool's output.
     * @throws IOException
     *             If an I/O error occurrs during copying of the
     *             toc-locations.xml file to the temporary directory.
     * @throws NotAUserSpaceTocException
     *             If the path of the toc <code>File</code> point to a toc
     *             <code>File</code> that is not a user space toc
     *             <code>File</code>.
     * @throws StdAPIFoundException
     *             If the toc indicates that the module is a standardized API
     *             rather than the expected proprietary API.
     */
    protected File createToc(Element tocElm, File tocFile)
            throws NotAUserSpaceTocException, StdAPIFoundException, IOException {

        // An example of a tocLocation file
        // <?xml version="1.0" encoding="utf-8"?>
        // <toc xmlns="http://www.stericsson.com/refman/API_Toc.xsd"
        // label="API">
        // -<topic label="Undefined area">
        // --<topic label="Kernel space">
        // ---<topic label="USB Driver Function guide"
        // ---href="u8500_usb/index.html"/>
        // --</topic>
        // -</topic>
        // </toc>

        // Traverse down to look for the element containing data about if the
        // toc is a user space toc.
        Element spaceElm = tocElm.getTopics().get(0).getTopics().get(0);
        String space = spaceElm.getLabel();
        if (spaceElm.getTopics().size() > 0) {
            Element topicElm = spaceElm.getTopics().get(0);
            String label = topicElm.getType();
            if (label.equalsIgnoreCase("StdAPI")) {
                throw new StdAPIFoundException();
            }
        }
        if (!space.equalsIgnoreCase("User space")) {
            throw new NotAUserSpaceTocException();
        }
        logger.debug("{} found and was evaluated to a {} toc", tocFile
                .getAbsolutePath(), space);

        Element module = spaceElm.getTopics().get(0);
        String includeDir = tocFile.getParent() + File.separator
                + module.getHref();
        logger.debug("getIncludeDir {}", includeDir);
        File fileDir = new File(includeDir);
        ArrayList<Element> hfilesElm = null;

        try {
            hfilesElm = new ArrayList<Element>(listFilesFromDoxygen(fileDir));
            logger.debug("Number of h files {}", hfilesElm.size());
            for (Element hfileElm : hfilesElm) {
                hfileElm.setHref("html" + File.separator + cnt + File.separator
                        + hfileElm.getHref());
            }
            module.addTopics(hfilesElm);
        } catch (NotADirectoryException e) {
            logger
                    .info(
                            "Something unexpected went wrong when adding file elements to toc.",
                            e);
            throw new IOException(e.toString());
        }

        module.setHref(null);

        File newToc;
        try {
            newToc = storeTocLocationsFile(tocElm, cnt, tempDir);
        } catch (IOException e) {
            logger.info(
                    "Unable to copy the file {} to the temporary directory: "
                            + e, tocFile.getAbsolutePath());
            throw e;
        }
        return newToc;

    }

    /**
     * Writes the selected TocElement to a toc-locations.xml file in the
     * temporary directory.
     *
     * @param tocElm
     *            The TocElement representing the file to store.
     * @param fileNumber
     *            A unique number ensuring that namespace collisions don't
     *            occur.
     * @param tempDir
     *            The directory where the toc-locations.xml file shall be
     *            stored.
     * @return The File representing the stored toc-locations.xml file.
     * @throws IOException
     *             If an IO error occurrs.
     */
    protected File storeTocLocationsFile(Element tocElm, int fileNumber,
            File tempDir) throws IOException {
        ElementFactory.sort(tocElm);

        XMLOutputter outputter = new XMLOutputter(Format.getPrettyFormat());

        File newToc = new File(tempDir.getAbsolutePath(), fileNumber
                + "-toc-locations.xml");
        FileWriter fw = new FileWriter(newToc);
        outputter.output(new DocumentFactory().getXmlToc(tocElm, "API"), fw);
        fw.close();
        logger.debug("{} was written to disk.", newToc.getAbsolutePath());
        return newToc;
    }

    /**
     * Scans <code>includePath</code> for h files and makes A
     * <code>List{@literal <Element>}</code> containing
     * <code>JDOM Elements</code> which represents the topics that will be
     * present in the eclipse help toc. Creates the <code>href attribute</code>
     * according to <code>doxygen's</code> rename pattern.
     *
     * @param includePath
     *            The path to the directory that contains the h files.
     * @return A <code>List{@literal <Element>}</code> containing
     *         <code>JDOM Elements</code> which represents the topics that will
     *         be present in the eclipse help toc.
     * @throws NotADirectoryException
     *             If <code>includePath</code> isn't a directory.
     */
    protected List<Element> listFilesFromDoxygen(File includePath)
            throws NotADirectoryException {
        if (includePath == null || !includePath.isDirectory()) {
            throw new NotADirectoryException();
        }

        File[] hFiles = includePath.listFiles(new FilenameFilter() {

            public boolean accept(File dir, String name) {
                return name.endsWith(".h");
            }
        });

        ArrayList<Element> doxygenFiles = new ArrayList<Element>();

        for (File hFile : hFiles) {
            String realName = hFile.getName();
            String renamedName = realName;
            renamedName = renamedName.replaceAll("_", "__");
            renamedName = renamedName.replaceAll(" ", "_01");
            // Positive lookahead
            renamedName = renamedName.replaceAll("\\.(?=.*\\.)", "_8");
            // Negative lookahead
            renamedName = renamedName.replaceAll("\\.h(?!.*\\.)", "_8h.html");
            renamedName = renamedName.replaceAll("(\\p{javaUpperCase})", "_$1")
                    .toLowerCase();

            Element elm = new TopicElement(realName, renamedName,
                    new Vector<Element>());
            doxygenFiles.add(elm);
        }

        return doxygenFiles;
    }

    /**
     * Calculates the path to the directory where the files to be run thru
     * <code>doxygen</code> resides.
     *
     * @param tocDoc
     *            The toc <code>Document</code>. From on of the href attributes
     *            in this <code>Document</code> the relative path is taken.
     * @param tocFile
     *            The toc <code>File</code>. From the directory the toc
     *            <code>File</code> lays in, and the relative path, the
     *            directory where the files to be run thru <code>doxygen</code>
     *            resides can be determined.
     * @return The path to the directory where the files to be run thru
     *         <code>doxygen</code> resides.
     */
    private String getIncludeDir(Document tocDoc, File tocFile) {
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        // Looks for the relative path to the include files to be run in
        // doxygen.
        Element moduleElm = tocElm.getTopics().get(0).getTopics().get(0)
                .getTopics().get(0);
        String releativePath = moduleElm.getHref();

        logger.debug("releativePath {}", releativePath);

        String includeDir = tocFile.getParent() + "/" + releativePath;
        logger.debug("includeDir {}", includeDir);

        return includeDir;
    }

    /**
     * Takes a predefined doxygen configuration <code>File</code> copies it and
     * append information about INPUT and OUTPUT directory from a toc file in
     * <code>tocFileDir</code>.
     * <p>
     * If information about INPUT and OUTPUT directories already exists it will
     * be overwritten by this method.
     *
     * @param tocFile
     *            The toc <code>File</code>.
     * @return A configuration <code>File</code> that <code>doxygen</code> will
     *         use while generating documentation for the directory that
     *         contains the toc <code>File</code>.
     */
    protected File createDoxyCfg(File tocFile) {

        // Build a JDOM document from the toc file
        SAXBuilder sb = new SAXBuilder();
        Document tocDoc = null;
        try {
            tocDoc = sb.build(tocFile);
        } catch (JDOMException e) {
            logger.error("Something went wrong while building a document for"
                    + tocFile.getAbsolutePath(), e);
        } catch (IOException e) {
            logger.error("Something went wrong while building a document for"
                    + tocFile.getAbsolutePath(), e);
        }

        String includeDir = getIncludeDir(tocDoc, tocFile);

        String cssFile = "HTML_STYLESHEET = " + refManDir.getAbsolutePath()
                + File.separator + "resources" + File.separator + "style.css"
                + "\n";
        String htmlOutput = "HTML_OUTPUT = " + htmlDir.getAbsolutePath()
                + File.separator + cnt + File.separator + "\n";
        String input = "INPUT = " + includeDir + File.separator + "\n";

        // Builds a new doxygen configuration file for the module with
        // customized input and output folders.
        StringBuilder sbNewDoxyCfg = new StringBuilder();
        boolean outputAdded = false;
        boolean inputAdded = false;

        File doxycfg = new File(refManDir.getAbsolutePath()
                + "/resources/doxy.cfg");

        Scanner scan = null;
        try {
            scan = new Scanner(doxycfg);
        } catch (FileNotFoundException e) {
            logger
                    .error("The base doxygen configuration file wasn't found.",
                            e);
            return null;
        }

        while (scan.hasNextLine()) {
            String doxyLine = scan.nextLine();
            if (doxyLine.matches(".*HTML_OUTPUT\\s*=.*")) {
                sbNewDoxyCfg.append(htmlOutput);
                outputAdded = true;
            } else if (doxyLine.matches(".*INPUT\\s*=.*")) {
                sbNewDoxyCfg.append(input);
                inputAdded = true;
            } else {
                sbNewDoxyCfg.append(doxyLine + "\n");
            }
        }
        sbNewDoxyCfg.append(cssFile);
        if (!inputAdded) {
            sbNewDoxyCfg.append(input);
            inputAdded = true;
        }
        if (!outputAdded) {
            sbNewDoxyCfg.append(htmlOutput);
            outputAdded = true;
        }

        // Saves the file to disk
        File newDoxyCfg = new File(tempDir.getAbsolutePath(), cnt + "-doxy.cfg");
        BufferedWriter output = null;
        try {
            output = new BufferedWriter(new FileWriter(newDoxyCfg));
            output.write(sbNewDoxyCfg.toString());
            output.flush();
        } catch (IOException e) {
            logger
                    .error(
                            "Something went wrong while creating a doxygen configuration file.",
                            e);
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException e) {
                    logger
                            .error(
                                    "Could not close buffered output steam for newDoxyCfg/sbNewDoxyCfg",
                                    e);
                }
            }
        }

        return newDoxyCfg;
    }

    /**
     * Starts doxygen with the configuration file specified by
     * <code>newDoxyCfg</code>.
     * <p>
     * Uses <code>StreamGobblers</code> for <code>InputStream</code> and
     * <code>ErrorStream</code>.
     *
     * @param doxyCfg
     *            The configuration <code>file</code> that <code>doxygen</code>
     *            will use.
     * @throws IOException
     */
    protected void startDoxygen(File doxyCfg) throws IOException {

        String[] cmdArr = {BashCreator.getBash(), "-c",
                "doxygen " + doxyCfg.getAbsolutePath()};

        ProcessBuilder pb = new ProcessBuilder(cmdArr);

        pb.directory(refManDir);
        Process proc = pb.start();

        // any error message?
        StreamGobbler errorGobbler = new StreamGobbler(proc.getErrorStream(),
                "STDERR", "DOXYGEN");

        // any output?
        StreamGobbler outputGobbler = new StreamGobbler(proc.getInputStream(),
                "STDOUT", "DOXYGEN");

        // kick them off
        errorGobbler.start();
        outputGobbler.start();

        try {
            proc.waitFor();
            int exitcode = proc.exitValue();
            if (exitcode != 0) {
                logger.error(
                        "DOXYGEN transform for '{}' failed with exit code {}",
                        tocFile.getAbsolutePath(), exitcode);
            } else {
                logger
                        .info(
                                "DOXYGEN transform for '{}' succeeded with exit code {}",
                                tocFile.getAbsolutePath(), exitcode);
            }
        } catch (InterruptedException e) {
            logger.error("Unexpected InterruptedException {}", e);
        }
    }

    /**
     * The run method which starts the Doxygen transformation.
     */
    @Override
    public void run() {
        boolean userSpaceToc = false;
        @SuppressWarnings("unused")
        File toc = null;
        String tocFilePath = tocFile.getAbsolutePath();
        try {
            logger.debug("tocFilePath is {}", tocFilePath);
            // Create a new toc file in tempOut
            toc = createToc(tocElement, tocFile);
            userSpaceToc = true;
            logger.debug("User Space toc found: '{}'", tocFilePath);
        } catch (NotAUserSpaceTocException e) {
            logger
                    .error(
                            "'{}' is not a user-space proprietary api toc file. It should not be transformed with Doxygen.",
                            tocFilePath);
            return;
        } catch (StdAPIFoundException e) {
            logger
                    .error(
                            "'{}' is a standard api toc file. It should not be transformed with Doxygen.",
                            tocFilePath);
            return;
        } catch (IOException e) {
            logger.error("Unexpected I/O error when creating toc: "
                    + tocFilePath, e);
            return;
        }

        if (userSpaceToc) {
            // Create a doxygen cfg file
            File newDoxyCfg = createDoxyCfg(new File(tocFilePath));

            try {
                // start the Doxygen transformation
                startDoxygen(newDoxyCfg);
            } catch (IOException e) {
                logger.error(
                        "Something involving start of doxygen went wrong: ", e);
            }
        }
    }
}
