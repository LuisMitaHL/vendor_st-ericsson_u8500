/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.kernelDoc;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;

import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.xpath.XPath;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.IO;

/**
 * Main class for coping the Linux Kernel documentation to RefMan's folder.
 *
 * @author Thomas Palmqvist
 *
 */
public class KernelDoc {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(KernelDoc.class);

    /**
     * Private constructor to prevent users to instantiate this class.
     */
    private KernelDoc() {
    }

    /**
     * Checks to see that the toc is a proprietary toc, rather than a Standard
     * API toc.
     *
     * @param tocLocDoc
     *            The JDOM Document representing the toc-locations.xml file
     * @return True if the toc is a proprietary toc, False if the toc is a
     *         standard API toc
     */
    private static boolean isProprietaryTocLocation(Document tocLocDoc) {
        String expression = "/x:toc/x:topic/x:topic/x:standardapi/@title";
        XPath xpath;
        try {
            xpath = XPath.newInstance(expression);
        } catch (JDOMException e) {
            return true;
        }

        xpath.addNamespace("x", tocLocDoc.getRootElement().getNamespaceURI());

        String label;
        try {
            label = xpath.valueOf(tocLocDoc);
        } catch (JDOMException e) {
            return true;
        }

        return (label == null || label.length() == 0);
    }

    /**
     * Copies the <code>HTML</code> documentation from <code>kernelDir</code>
     * /Documentation/DocBook to <code>htmlDir</code> for all books that have
     * kernel space tocs residing in <code>kernelTocDir</code>.
     *
     * @param kernelDir
     *            The path to the folder for the Linux Kernel.
     * @param htmlDir
     *            The path to the folder where the <code>HTML</code>
     *            documentation will be copied.
     * @param kernelTocDir
     *            The path to the folder where the kernel space toc files for
     *            the books reside.
     */
    public static void copyHtml(File kernelDir, File htmlDir, File kernelTocDir) {
        File docBookDir = new File(kernelDir.getAbsolutePath()
                + "/Documentation/DocBook");

        File[] tocLocationsFiles = listTocLocationsFiles(kernelTocDir);
        if (tocLocationsFiles == null) {
            logger.info("tocLocationsFiles is null");
            return;
        }
        for (File tocLocation : tocLocationsFiles) {

            try {
                // Build a JDOM document from the tocLocation file
                SAXBuilder sb = new SAXBuilder();
                Document tocLocDoc = sb.build(tocLocation);

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
                //
                // Path to the attribute containing information about if the toc
                // is a User space or Kernel space toc. The x represent the
                // name space of the root element in our case
                // http://www.stericsson.com/refman/API_Toc.xsd.
                String expression = "/x:toc/x:topic/x:topic/@label";
                XPath xpath = XPath.newInstance(expression);

                xpath.addNamespace("x", tocLocDoc.getRootElement()
                        .getNamespaceURI());

                String space = xpath.valueOf(tocLocDoc);

                boolean isProprietaryToc = isProprietaryTocLocation(tocLocDoc);

                if (isProprietaryToc) {
                    logger.info("{} found and was evaluated to a {} toc",
                            tocLocation.getName(), space);
                } else {
                    logger
                            .info(
                                    "{} found and was evaluated to a standardapi toc - ignoring",
                                    tocLocation.getName());
                }

                if (space.equalsIgnoreCase("kernel space") && isProprietaryToc) {
                    // The prefix to -toc-locations.xml is the book name.
                    String book = tocLocation.getName().split(
                            "-toc-locations\\.xml")[0];

                    // Create File objects for the source and destination of the
                    // kernel documentation.
                    File sourceFile = new File(docBookDir, "/" + book + ".html");
                    File destFile = new File(htmlDir, "/" + book + ".html");
                    File sourceFilesDir = new File(docBookDir, "/" + book);
                    File destFilesDir = new File(htmlDir, "/" + book);
                    if (!destFilesDir.exists()) {
                        destFilesDir.mkdirs();
                    }

                    // Copies the documentation
                    IO.copyFile(sourceFile, destFile);
                    IO
                            .copyDirectory(sourceFilesDir, destFilesDir, false,
                                    false);

                    // Log info about what file and directory that was copied.
                    logger.info("File {} copied to {}", sourceFile, destFile);
                    logger.info("Directory {} copied to {}", sourceFilesDir,
                            destFilesDir);
                }

            } catch (JDOMException e) {
                logger
                        .error(
                                "Something went wrong while building the JDOM document or with XPath ",
                                e);
            } catch (IOException e) {
                logger
                        .error(
                                "Something went wrong while building the JDOM document or when copying the files.",
                                e);
            }
        }
    }

    private static File[] listTocLocationsFiles(File kernelTocDir) {
        return kernelTocDir.listFiles(new FilenameFilter() {

            // Implements accept method used to only iterate files that ends
            // with toc-locations.xml
            public boolean accept(File dir, String name) {
                return name.toLowerCase().endsWith("toc-locations.xml");
            }
        });
    }
}
