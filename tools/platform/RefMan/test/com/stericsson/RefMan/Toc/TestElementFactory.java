/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;
import java.util.Vector;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author emarttr <a
 *         href="mailto:martin.trulsson@stericsson.com?subject=ElementFactory"
 *         >Email</a>
 *
 */
public class TestElementFactory {

    /** The root element of the tree with correct structure for b2r2.xml */
    Element rootOneFile;

    /**
     * The root element of the tree with correct structure for all files in
     * IM_Tocs folder
     */
    Element rootFull;

    /**
     * The root element of the tree with correct structure for
     * stdapi-correct.xml
     */
    Element stdApi;

    /**
     * The root element of the tree with correct structure for standardAPI file
     * with values for required arguments and tags only.
     */
    Element stdApiOnlyRequired;

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestElementFactory.class);

    /**
     * @throws java.lang.Exception
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        Element bar = new TopicElement("1", "bar.html", new Vector<Element>());
        Element foo = new TopicElement("2", "foo.html", new Vector<Element>());
        Element mySection = new TopicElement("3", "mySection.html",
                new Vector<Element>());
        Vector<Element> topics = new Vector<Element>();
        topics.add(bar);
        topics.add(foo);
        topics.add(mySection);
        Element b2r2 = new TopicElement("b2r2", null, topics);
        Vector<Element> b2r2_vector = new Vector<Element>();
        b2r2_vector.add(b2r2);
        Element userSpace = new TopicElement("User space", null, b2r2_vector);
        Vector<Element> userSpace_vector = new Vector<Element>();
        userSpace_vector.add(userSpace);
        Element multimedia = new TopicElement("Multimedia", null,
                userSpace_vector);
        Vector<Element> multimedia_vector = new Vector<Element>();
        multimedia_vector.add(multimedia);
        rootOneFile = new TocElement("API", null, multimedia_vector);

        Element b3r3 = new TopicElement("b3r3", null, topics);
        Element p2r2 = new TopicElement("p2r2", null, topics);
        Element k2r2 = new TopicElement("k2r2", null, topics);
        Element k3r3 = new TopicElement("k3r3", null, topics);

        Vector<IncludeDoc> includeDirs = new Vector<IncludeDoc>();
        includeDirs.add(new IncludeDoc("includedir/", "includes", true));
        Vector<IncludeDoc> includeFiles = new Vector<IncludeDoc>();
        includeFiles.add(new IncludeDoc("es_full_spec.1.1.10.pdf", "docs",
                false));
        Vector<Script> scripts = new Vector<Script>();
        scripts.add(new Script("testScript", "directory/to/start/in"));
        Element stdApiElm = new StdAPIElement("Open GL ES",
                "ste-meta-data/opengl-index.xml", "opengl", "opengl", "opengl",
                includeDirs, includeFiles, scripts);

        Vector<Element> mmUserSpace_vector = new Vector<Element>();
        mmUserSpace_vector.add(b2r2);
        mmUserSpace_vector.add(b3r3);
        mmUserSpace_vector.add(stdApiElm);
        Vector<Element> mmKernelSpace_vector = new Vector<Element>();
        mmKernelSpace_vector.add(k2r2);
        mmKernelSpace_vector.add(k3r3);
        Vector<Element> baseKernelSpace_vector = new Vector<Element>();
        baseKernelSpace_vector.add(p2r2);

        Element baseKernelSpace = new TopicElement("Kernel space", null,
                baseKernelSpace_vector);
        Element mmUserSpace = new TopicElement("User space", null,
                mmUserSpace_vector);
        Element mmKernelSpace = new TopicElement("Kernel space", null,
                mmKernelSpace_vector);

        Vector<Element> base_vector = new Vector<Element>();
        base_vector.add(baseKernelSpace);
        Vector<Element> mm_vector = new Vector<Element>();
        mm_vector.add(mmUserSpace);
        mm_vector.add(mmKernelSpace);

        Element baseUtilities = new TopicElement("Base utilities", null,
                base_vector);
        Element multimedia_full = new TopicElement("Multimedia", null,
                mm_vector);

        Vector<Element> toc_vector = new Vector<Element>();
        toc_vector.add(baseUtilities);
        toc_vector.add(multimedia_full);

        rootFull = new TocElement("API", null, toc_vector);
        rootFull.setHref("");
        setupStdAPI();
    }

    /**
     * Help method to set up correct standard api element structure.
     */
    private void setupStdAPI() {
        Vector<IncludeDoc> includeDirs = new Vector<IncludeDoc>();
        includeDirs.add(new IncludeDoc("includedir/", "includes", true));
        Vector<IncludeDoc> includeFiles = new Vector<IncludeDoc>();
        includeFiles.add(new IncludeDoc("es_full_spec.1.1.10.pdf", "docs",
                false));
        Vector<Script> scripts = new Vector<Script>();
        scripts.add(new Script("testScript", "directory/to/start/in"));
        Element stdApiElm = new StdAPIElement("Open GL ES",
                "ste-meta-data/opengl-index.xml", "opengl", "opengl", "opengl",
                includeDirs, includeFiles, scripts);
        Vector<Element> userSpace_vector = new Vector<Element>();
        userSpace_vector.add(stdApiElm);
        Element userSpace = new TopicElement("User space", null,
                userSpace_vector);
        Vector<Element> mm_vector = new Vector<Element>();
        mm_vector.add(userSpace);
        Element multimedia = new TopicElement("Multimedia", null, mm_vector);
        Vector<Element> toc_vector = new Vector<Element>();
        toc_vector.add(multimedia);
        stdApi = new TocElement("API", null, toc_vector);

        Element stdAPIElmReq = new StdAPIElement("Open GL ES",
                "ste-meta-data/opengl-index.xml", "opengl", null, null,
                new Vector<IncludeDoc>(), new Vector<IncludeDoc>(),
                new Vector<Script>());
        Vector<Element> vUs = new Vector<Element>();
        vUs.add(stdAPIElmReq);
        Element us = new TopicElement("User space", null, vUs);
        Vector<Element> vMm = new Vector<Element>();
        vMm.add(us);
        Element mm = new TopicElement("Multimedia", null, vMm);
        Vector<Element> vToc = new Vector<Element>();
        vToc.add(mm);
        stdApiOnlyRequired = new TocElement("API", null, vToc);
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for {@link ElementFactory#getElementArray(File ImTocFiles)}.
     */
    @Test
    public void testGetElementArray() {
        String inputFilePath = "test//com//stericsson//RefMan//Toc//IM_Tocs";
        boolean passed = true;

        try {
            List<Element> tocs = ElementFactory.getElementArray(new File(
                    inputFilePath));
            if (tocs.size() > 0) {
                passed = checkElements(tocs.get(0), rootOneFile);
            }
        } catch (Exception e) {
            logger.error("Could not get the toc files. ", e.getMessage());
        }

        assertTrue(passed);
    }

    /**
     * Test method for {@link ElementFactory#getElementArray(File ImTocFiles)}.
     *
     * Test getElementArray with two toc files that have the same name.
     */
    @Test
    public void testGetElementArrayDuplicatedModule() {
        String inputFilePath = "test//com//stericsson//RefMan//Toc//DuplicatedModules";

        try {
            InputStream in = new FileInputStream(new File(inputFilePath
                    + "//toc-location2.xml"));
            OutputStream out = new FileOutputStream(new File(inputFilePath
                    + "//toc-location.xml"));

            // Transfer bytes from in to out
            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            in.close();
            out.close();
        } catch (FileNotFoundException e1) {
            logger.info("File could not be found " + e1);
        } catch (IOException e) {
            logger.info("Failed to copy file " + e);
        }

        try {
            List<Element> tocs = ElementFactory.getElementArray(new File(
                    inputFilePath));
            assertTrue(tocs.size() == 1);
        } catch (IOException e) {
            logger.error("Could not get the toc files. ", e.getMessage());
        }
    }

    /**
     * Test method for
     * {@link ElementFactory#getElement(File imTocFile, int depth)} .
     */
    @Test
    public void testGetElement() {

        boolean passed = true;
        Element elm = ElementFactory.getElement(new File(
                "test//com//stericsson//RefMan//Toc//IM_Tocs//b2r2.xml"), 0);

        passed = checkElements(elm, rootOneFile);

        assertTrue(passed);
    }

    /**
     * Test method for
     * {@link ElementFactory#getElement(File imTocFile, int depth)} . Test to
     * handle a toc-location file for a standard api.
     */
    @Test
    public void testGetElementStdAPIElement() {
        Element elm = ElementFactory.getElement(new File(
                "test//com//stericsson//RefMan//Toc//stdapi-correct.xml"), 0);

        assertTrue(checkElements(elm, stdApi));
    }

    /**
     * Test method for
     * {@link ElementFactory#getElement(File imTocFile, int depth)} . Test to
     * handle a toc-location file for a standard api with values for required
     * arguments and tags only.
     */
    @Test
    public void testGetElementStdAPIElementRequiredOnly() {
        Element elm = ElementFactory
                .getElement(
                        new File(
                                "test//com//stericsson//RefMan//Toc//stdapi-required-only.xml"),
                        0);

        assertTrue(checkElements(elm, stdApiOnlyRequired));
    }

    /**
     * Help method that compares two <code>Element's</code> to see if they are equal.
     *
     * @return <code>True</code> if the <code>Element's</code> are equal, otherwise
     *         <code>False</code>.
     */
    private boolean checkElements(Element e1, Element e2) {
        boolean passed = true;
        List<Element> children_e1;
        List<Element> children_e2;

        if (!e1.toString().equals(e2.toString())) {
            logger.error(e1.toString() + " doesn´t match " + e2.toString());
            passed = false;
        }
        if (passed) {
            children_e1 = e1.getTopics();
            children_e2 = e2.getTopics();
            if (children_e1.size() != children_e2.size()) {
                passed = false;
            } else {
                for (int i = 0; i < children_e1.size(); i++) {
                    passed = checkElements(children_e1.get(i), children_e2
                            .get(i));
                    if (!passed) {
                        break;
                    }
                }
            }
        }
        return passed;
    }

    /**
     * Test method for
     * {@link ElementFactory#getAssembledElement(File ImTocFiles)} .
     */
    @Test
    public void testGetAssembledElement() {
        boolean passed = true;
        Element elm = ElementFactory.getAssembledElement(new File(
                "test//com//stericsson//RefMan//Toc//IM_Tocs"));

        passed = checkElements(elm, rootFull);

        assertTrue(passed);
    }

    /**
     * Test method for {@link ElementFactory#sort(Element tocElm)} .
     */
    @Test
    public void testSort() {
        Element bar = new TopicElement("1", "bar.html", new Vector<Element>());
        Element foo = new TopicElement("2", "foo.html", new Vector<Element>());
        Element mySection = new TopicElement("3", "mySection.html",
                new Vector<Element>());
        Vector<Element> topics = new Vector<Element>();
        topics.add(mySection);
        topics.add(bar);
        topics.add(foo);
        Element elm = new TopicElement("name", null, topics);

        List<Element> correct = new Vector<Element>();
        correct.add(bar);
        correct.add(foo);
        correct.add(mySection);

        ElementFactory.sort(elm);
        Vector<Element> sorted = (Vector<Element>) elm.getTopics();
        for (int i = 0; i < sorted.size(); i++) {
            assertTrue(sorted.get(i).equals(correct.get(i)));
        }
    }
}
