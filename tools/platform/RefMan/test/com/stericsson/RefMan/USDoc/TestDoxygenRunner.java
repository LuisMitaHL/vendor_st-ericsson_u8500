/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;
import java.util.Vector;

import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLUnit;
import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.DOMBuilder;
import org.jdom.input.SAXBuilder;
import org.jdom.output.DOMOutputter;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Exception.NotADirectoryException;
import com.stericsson.RefMan.Exception.NotAUserSpaceTocException;
import com.stericsson.RefMan.Exception.StdAPIFoundException;
import com.stericsson.RefMan.Toc.DocumentFactory;
import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.ElementComparator;
import com.stericsson.RefMan.Toc.ElementFactory;
import com.stericsson.RefMan.Toc.TocElement;
import com.stericsson.RefMan.Toc.TopicElement;

public class TestDoxygenRunner {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestDoxygenRunner.class);

    /** Path to a correct toc file */
    private final static String tocFileCorrect = "test/com/stericsson/RefMan/USDoc/resources/testFileCorrect.xml";

    /** Path to a kernel space toc file */
    private final static String tocFileKernelSpace = "test/com/stericsson/RefMan/USDoc/resources/testFileKernelSpace.xml";

    /** The path to the html directory */
    private final static String htmlDir = "test/com/stericsson/RefMan/USDoc/htmlDir";

    /** The path to the temp directory */
    private final static String tempDir = "test/com/stericsson/RefMan/USDoc/tempDir";

    /** The path to the test h files directory */
    private final static String testhFilesDirPath = "test/com/stericsson/RefMan/USDoc/resources/test_h_files";

    /** Path to a correct doxy.cfg file */
    private final static String correctDoxyCfg = "test/com/stericsson/RefMan/USDoc/correctDoxy.cfg";

    /** The root element representing a correct structure of a toc file. */
    private Element testFile1Root;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        if (!new File("test/com/stericsson/RefMan/USDoc/tempDir").exists()) {
            if (!new File("test/com/stericsson/RefMan/USDoc/tempDir").mkdir()) {
                logger.error("Set up failed");
                fail("Set up failed");
            }
        }
        Element topic = new TopicElement("testFile1", null,
                new Vector<Element>());

        Element hfile = new TopicElement("hello.h", "html" + File.separator
                + "0" + File.separator + "hello_8h.html", new Vector<Element>());
        topic.addTopic(hfile);
        hfile = new TopicElement("Copy of hello.h", "html" + File.separator
                + "0" + File.separator + "_copy_01of_01hello_8h.html",
                new Vector<Element>());
        topic.addTopic(hfile);
        hfile = new TopicElement("Copy_of hello.h", "html" + File.separator
                + "0" + File.separator + "_copy__of_01hello_8h.html",
                new Vector<Element>());
        topic.addTopic(hfile);
        hfile = new TopicElement("Copy.of.hello.h", "html" + File.separator
                + "0" + File.separator + "_copy_8of_8hello_8h.html",
                new Vector<Element>());
        topic.addTopic(hfile);
        hfile = new TopicElement("CoPY_Of hello2.h", "html" + File.separator
                + "0" + File.separator + "_co_p_y___of_01hello2_8h.html",
                new Vector<Element>());
        topic.addTopic(hfile);

        Vector<Element> topics = new Vector<Element>();
        topics.add(topic);
        Element userSpace = new TopicElement("User space", null, topics);
        Vector<Element> usVector = new Vector<Element>();
        usVector.add(userSpace);
        Element area = new TopicElement("Multimedia", null, usVector);
        Vector<Element> areaVector = new Vector<Element>();
        areaVector.add(area);
        testFile1Root = new TocElement("API", null, areaVector);
        ElementFactory.sort(testFile1Root);
    }

    @After
    public void tearDown() throws Exception {
        deleteDirs(new File(tempDir));
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
     * {@link com.stericsson.RefMan.USDoc.DoxygenRunner#createToc(Element, File)}
     * .
     */
    @Test
    public void testCreateToc() {
        boolean passed = true;
        File generatedFile = null;

        File tocFile = new File(tocFileCorrect);
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        DoxygenRunner doxygenRunner = new DoxygenRunner(tocElm, tocFile,
                new File(htmlDir), new File(tempDir), 0);

        try {
            generatedFile = doxygenRunner.createToc(tocElm, tocFile);
        } catch (NotAUserSpaceTocException e) {
            logger.error("Not a user space toc file ", e);
            fail();
        } catch (IOException e) {
            logger.error("", e);
            fail(e.toString());
        } catch (StdAPIFoundException e) {
            logger.info("Found a standard API toc file");
        }

        if (generatedFile == null || !generatedFile.exists()) {
            fail("Could not find the file that should have been generated");
            logger
                    .error("Could not find the file that should have been generated");
        }
        Diff diff = null;
        Document generatedDoc = null;
        SAXBuilder builder = new SAXBuilder();
        final DOMOutputter outputter = new DOMOutputter();

        try {
            generatedDoc = builder.build(generatedFile);
        } catch (JDOMException e) {
            logger.error("Failed to build document" + e.getMessage());
        } catch (IOException e) {
            logger.error("Failed to build document" + e.getMessage());
        }
        org.w3c.dom.Document generatedW3cDoc = null;
        try {
            generatedW3cDoc = outputter.output(generatedDoc);
        } catch (JDOMException e) {
            logger.error(e.getMessage());
        }
        XMLUnit.setIgnoreWhitespace(true);
        try {
            org.w3c.dom.Document correctDoc = new DOMOutputter()
                    .output(new DocumentFactory().getXmlToc(testFile1Root,
                            "API"));
            XMLOutputter xo = new XMLOutputter(Format.getPrettyFormat());
            FileWriter fw = new FileWriter(new File(generatedFile.getParent()
                    + "/method"));
            xo.output(generatedDoc, fw);
            fw.close();
            XMLOutputter xo2 = new XMLOutputter(Format.getPrettyFormat());
            FileWriter fw1 = new FileWriter(new File(generatedFile.getParent()
                    + "/field"));
            xo2.output(new DOMBuilder().build(correctDoc), fw1);
            fw1.close();
            diff = new Diff(generatedW3cDoc, correctDoc);
            passed = diff.similar();
            logger.info(diff.toString());
        } catch (JDOMException e) {
            logger.error("Failed to create a diff ", e);
        } catch (Exception e) {
            logger.error("Failed to create a diff ", e);
        }
        assertTrue(passed);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.USDoc.DoxygenRunner#createToc(Element, File)}
     * with a kernel space toc.
     *
     * @throws NotAUserSpaceTocException
     * @throws IOException
     */
    @Test(expected = NotAUserSpaceTocException.class)
    public void testCreateKernelSpaceToc() throws NotAUserSpaceTocException {
        File tocFile = new File(tocFileKernelSpace);
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        DoxygenRunner doxygenRunner = new DoxygenRunner(tocElm, tocFile,
                new File(htmlDir), new File(tempDir), 0);

        try {
            doxygenRunner.createToc(tocElm, tocFile);
        } catch (IOException e) {
            logger.error("Can not find the file: " + tocFileKernelSpace
                    + e.getMessage());
        } catch (StdAPIFoundException e) {
            logger.info("Found a standard API toc file");
        }
    }

    /**
     * Test method for {@link DoxygenRunner#listFilesFromDoxygen(File)}
     */
    @Test
    public void testListFilesFromDoxygen() {
        Element correctElm = new TopicElement();

        correctElm.addTopic(new TopicElement("Copy of hello.h",
                "_copy_01of_01hello_8h.html", new Vector<Element>()));
        correctElm.addTopic(new TopicElement("Copy_of hello.h",
                "_copy__of_01hello_8h.html", new Vector<Element>()));
        correctElm.addTopic(new TopicElement("CoPY_Of hello2.h",
                "_co_p_y___of_01hello2_8h.html", new Vector<Element>()));
        correctElm.addTopic(new TopicElement("Copy.of.hello.h",
                "_copy_8of_8hello_8h.html", new Vector<Element>()));
        correctElm.addTopic(new TopicElement("hello.h", "hello_8h.html",
                new Vector<Element>()));

        File tocFile = new File(tocFileCorrect);
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        DoxygenRunner doxygenRunnerAbstract = new DoxygenRunner(tocElm,
                tocFile, new File(htmlDir), new File(tempDir), 0);

        File testhFilesDir = new File(testhFilesDirPath);

        ArrayList<Element> testElms = null;
        try {
            testElms = new ArrayList<Element>(doxygenRunnerAbstract
                    .listFilesFromDoxygen(testhFilesDir));
        } catch (NotADirectoryException e) {
            logger.error("testhFilesDir wasn't a directory", e);
            fail();
        }
        Element testElm = new TopicElement();
        for (Element elm : testElms) {
            testElm.addTopic(elm);
        }

        Collections.sort(testElm.getTopics(), new ElementComparator());
        Collections.sort(correctElm.getTopics(), new ElementComparator());
        int i = 0;
        boolean passed = true;
        ArrayList<Element> test = new ArrayList<Element>(testElm.getTopics());
        ArrayList<Element> correct = new ArrayList<Element>(correctElm
                .getTopics());
        for (i = 0; i < 5; i++) {
            logger.debug("testElms {}", test.get(i));
            logger.debug("testElms {}", correct.get(i));
            if (!test.equals(correct)) {
                passed = false;
            }
        }
        if (i != 5) {
            passed = false;
        }

        if (!passed) {
            logger.error("The correct and test lists content didn't match");
        }
        assertTrue("The correct and test lists content didn't match", passed);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.USDoc.DoxygenRunner#createDoxyCfg(File)} .
     */
    @Test
    public void testCreateDoxyCfg() {
        boolean passed = true;
        File tocFile = new File(tocFileCorrect);
        Element tocElm = ElementFactory.getElement(tocFile, 0);
        DoxygenRunner doxygenRunner = new DoxygenRunner(tocElm, tocFile,
                new File(htmlDir), new File(tempDir), 0);

        File doxycfg = doxygenRunner.createDoxyCfg(new File(tocFileCorrect));
        Scanner scan = null;
        Scanner correctScan = null;
        try {
            scan = new Scanner(doxycfg);
            correctScan = new Scanner(new File(correctDoxyCfg));
        } catch (FileNotFoundException e) {
            logger
                    .error("The base doxygen configuration file wasn't found.",
                            e);
        }
        String line;
        String correctLine;
        while (scan.hasNext() && correctScan.hasNext()) {
            line = scan.nextLine();
            correctLine = correctScan.nextLine();
            fromInput: if (!(correctLine.equals(line)
                    || !(correctLine.startsWith("HTML_OUTPUT") && line
                            .startsWith("HTML_OUTPUT")) || !(correctLine
                    .startsWith("HTML_STYLESHEET") && line
                    .startsWith("HTML_STYLESHEET")))) {
                if (line.startsWith("INPUT")) {
                    String newLine = "INPUT = ";
                    int lineLength = line.length();
                    int index = lineLength - 57;
                    newLine += line.substring(index);
                    if (!newLine.equals(correctLine)) {
                        passed = false;
                        logger
                                .error("The produced cfgDoxy-file doesn´t match the correct one on INPUT = ...");
                    }
                    break fromInput;
                }
                passed = false;
                logger
                        .error("The produced cfgDoxy-file doesn´t match the correct one");
            }
        }
        if (scan.hasNext() || correctScan.hasNext()) {
            passed = false;
            logger
                    .error("The produced cfgDoxy-file doesn´t match the correct one,"
                            + "the number of lines does not match");
        }
        scan.close();
        correctScan.close();
        assertTrue(passed);
    }

    /**
     * Test method for {@link com.stericsson.RefMan.USDoc.DoxygenRunner#run()}.
     */
    @Test
    public void testRun() {
        boolean passed = true;
        File tocFile = new File(tocFileCorrect);
        Element tocElm1 = ElementFactory.getElement(tocFile, 0);
        Element tocElm2 = ElementFactory.getElement(tocFile, 0);
        Element tocElm3 = ElementFactory.getElement(tocFile, 0);
        DoxygenRunner doxygenRunner1 = new DoxygenRunner(tocElm1, tocFile,
                new File(htmlDir), new File(tempDir), 0);
        DoxygenRunner doxygenRunner2 = new DoxygenRunner(tocElm2, tocFile,
                new File(htmlDir), new File(tempDir), 1);
        DoxygenRunner doxygenRunner3 = new DoxygenRunner(tocElm3, tocFile,
                new File(htmlDir), new File(tempDir), 2);

        logger.error("Hej");
        doxygenRunner1.start();
        doxygenRunner2.start();
        doxygenRunner3.start();

        logger.error("Hej2");
        try {
            doxygenRunner1.join();
            doxygenRunner2.join();
            doxygenRunner3.join();
        } catch (InterruptedException e) {
            logger.error(e.getMessage());
        }

        logger.error("Hej3");
        for (int i = 0; i < 3; i++) {
            if (!new File(tempDir + "/" + i + "-toc-locations.xml").exists()) {
                logger.error(tempDir + "/" + i
                        + "-toc-locations.xml can´t be found.");
                assertTrue(false);
                passed = false;
            }
            if (!new File(tempDir + "/" + i + "-doxy.cfg").exists()) {
                logger.error(tempDir + "/" + i + "-doxy.cfg can´t be found.");
                assertTrue(false);
                passed = false;
            }
        }
        assertTrue(passed);
    }
}
