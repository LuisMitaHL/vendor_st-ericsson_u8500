/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;

import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLUnit;
import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.output.DOMOutputter;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class TestCreateHtmlManual {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestCreateHtmlManual.class);

    /** The temporary output folder. */
    private final File outDir = new File("test/com/stericsson/RefMan/tempOut");

    /** The temporary output html folder. */
    private final File htmlDir = new File(outDir, "html");

    /** A <code>String<code> used as platformName. */
    private final String platformName = "testPlatform";

    /** A <code>String<code> used as platformVersion. */
    private final String platformVersion = "testVersion";

    /** A test header */
    private final String header = "testHeader";

    /** A file used when creating a tree view for test purpose. */
    private final String testFile = "test/com/stericsson/RefMan/pluginTestFile.xml";

    /**
     * A file containing the correct output when creating a tree view using
     * <code>testFile</code>.
     */
    private final String correctTreeOutputFile = "test/com/stericsson/RefMan/pluginTestFileHtmlOutput.html";

    @Before
    public void setUp() throws Exception {
        htmlDir.mkdirs();
    }

    @After
    public void tearDown() throws Exception {
        deleteDirs(outDir);
    }

    /**
     * Help method to remove temporary files and folders
     * */
    private void deleteDirs(File file) {
        if (file.isDirectory()) {
            File[] children = file.listFiles();
            for (int i = 0; i < children.length; i++) {
                deleteDirs(children[i]);
            }
        }
        file.delete();
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.CreateHtmlManual#createManual(String)}.
     */
    @Test
    public void testCreateManual() {
        CreateHtmlManual createHtmlManual = new CreateHtmlManual(outDir,
                htmlDir, platformName, header, platformVersion);
        createHtmlManual.createManual(testFile);

        assertTrue(outDir.listFiles().length == 4);

        assertTrue(htmlDir.listFiles().length == 1);

        File[] outDirFiles = outDir.listFiles(new FileFilter() {
            public boolean accept(File f) {
                return f.getName().equalsIgnoreCase("tree.html");
            }
        });

        if (outDirFiles.length == 1) {
            compareFile(outDirFiles[0], new File(correctTreeOutputFile));
        }
    }

    /**
     * Help method which compares two files.
     *
     * @param outputFile
     *            The <code>File</code> to compare.
     * @param correctFile
     *            The <code>File</code> to compare against.
     */
    private void compareFile(File outputFile, File correctFile) {
        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(correctFile);
        } catch (JDOMException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        org.w3c.dom.Document correctW3cDoc = null;
        try {
            correctW3cDoc = outputter.output(correctDoc);
        } catch (JDOMException e) {
            e.printStackTrace();
        }

        Document outputDoc = null;

        try {
            outputDoc = builder.build(outputFile);
        } catch (JDOMException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        org.w3c.dom.Document outputW3cDoc = null;
        try {
            outputW3cDoc = outputter.output(outputDoc);
        } catch (JDOMException e) {
            e.printStackTrace();
        }

        XMLUnit.setIgnoreWhitespace(true);
        boolean passed = false;
        try {
            diff = new Diff(correctW3cDoc, outputW3cDoc);
            passed = diff.similar();
            logger.info(diff.toString());
        } catch (Exception e) {
            logger.error("Failed to create a diff ", e);
        }
        assertTrue(passed);
    }
}
