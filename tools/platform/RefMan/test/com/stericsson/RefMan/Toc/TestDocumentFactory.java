/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.Vector;

import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLUnit;
import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.output.DOMOutputter;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author Thomas Palmqvist
 *
 */
public class TestDocumentFactory {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestDocumentFactory.class);

    /** Path to a correct toc file in xml format. */
    private final static String correctXmlTocFile = "test//com//stericsson//RefMan//Toc//correct.xml";

    /** Path to a correct toc file in html format. */
    private final static String correctHtmlTocFile = "test//com//stericsson//RefMan//Toc//correct_html.html";

    /** Path to a correct standard API toc file. */
    private final static String correctStdApiXmlTocFile = "test//com//stericsson//RefMan//Toc//output-stdapi-correct.xml";

    /** Path to a correct standard API toc file. */
    private final static String correctStdApiHtmlTocFile = "test//com//stericsson//RefMan//Toc//output-stdapi-correct.html";

    /**
     * The root element of the tree with representing the content of correct.xml
     */
    private Element root_element;

    /**
     * The root element of the tree with representing the content of
     * output-stdapi-correct.xml
     */
    private Element std_api_xml_root_element;

    /**
     * The root element of the tree representing the html structure.
     */
    private Element html_root_element;

    /**
     * The root element of the tree representing the html structure for a
     * standard API toc.
     */
    private Element stdApi_html_root_element;

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
        Element b3r3 = new TopicElement("b3r3", null, topics);
        Element p2r2 = new TopicElement("p2r2", null, topics);
        Element k2r2 = new TopicElement("k2r2", null, topics);
        Element k3r3 = new TopicElement("k3r3", null, topics);

        Vector<Element> mmUserSpace_vector = new Vector<Element>();
        mmUserSpace_vector.add(b2r2);
        mmUserSpace_vector.add(b3r3);
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
        root_element = new TocElement("API", null, toc_vector);

        Element stdapiXml = new StdAPIElement("Open GL ES",
                "opengl/index.html", "opengl", "opengl", "",
                new Vector<IncludeDoc>(), new Vector<IncludeDoc>(),
                new Vector<Script>());
        Vector<Element> stdApiXmlUS_vector = new Vector<Element>();
        stdApiXmlUS_vector.add(stdapiXml);
        Element stdApiXmlUS = new TopicElement("User space", null,
                stdApiXmlUS_vector);
        Vector<Element> stdApiXmlMm_vector = new Vector<Element>();
        stdApiXmlMm_vector.add(stdApiXmlUS);
        Element stdApiXmlMm = new TopicElement("Multimedia", null,
                stdApiXmlMm_vector);
        Vector<Element> stdApiXmlAPI = new Vector<Element>();
        stdApiXmlAPI.add(stdApiXmlMm);
        std_api_xml_root_element = new TocElement("API", null, stdApiXmlAPI);

        Vector<Element> htmlUS_vector = new Vector<Element>();
        htmlUS_vector.add(b2r2);
        Element htmlUS = new TopicElement("User space", null, htmlUS_vector);
        Vector<Element> htmlMm_vector = new Vector<Element>();
        htmlMm_vector.add(htmlUS);
        Element htmlMm = new TopicElement("Multimedia", null, htmlMm_vector);
        Vector<Element> htmlAPI = new Vector<Element>();
        htmlAPI.add(htmlMm);
        html_root_element = new TopicElement("API", null, htmlAPI);

        Element stdapiHtml = new TopicElement("Open GL ES",
                "opengl/index.html", new Vector<Element>());
        Vector<Element> stdApiHtmlUS_vector = new Vector<Element>();
        stdApiHtmlUS_vector.add(stdapiHtml);
        Element stdApiHtmlUS = new TocElement("User space", null,
                stdApiHtmlUS_vector);
        Vector<Element> stdApiHtmlMm_vector = new Vector<Element>();
        stdApiHtmlMm_vector.add(stdApiHtmlUS);
        Element stdApiHtmlMm = new TopicElement("Multimedia", null,
                stdApiHtmlMm_vector);
        Vector<Element> stdApiHtmlAPI = new Vector<Element>();
        stdApiHtmlAPI.add(stdApiHtmlMm);
        stdApi_html_root_element = new TocElement("API", null, stdApiHtmlAPI);
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.DocumentFactory#getXmlToc(com.stericsson.RefMan.Toc.Element, String)}
     * .
     */
    @Test
    public void testGetXmlToc() {

        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(correctXmlTocFile);
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
        XMLUnit.setIgnoreWhitespace(true);
        boolean passed = false;
        try {
            diff = new Diff(correctW3cDoc, new DOMOutputter()
                    .output(new DocumentFactory()
                            .getXmlToc(root_element, "API")));
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
     * {@link com.stericsson.RefMan.Toc.DocumentFactory#getXmlToc(com.stericsson.RefMan.Toc.Element, String)}
     * with a standard api toc file.
     */
    @Test
    public void testGetXmlTocWithStdApiTocFile() {
        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(correctStdApiXmlTocFile);
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
        XMLUnit.setIgnoreWhitespace(true);
        boolean passed = false;
        try {
            diff = new Diff(correctW3cDoc, new DOMOutputter()
                    .output(new DocumentFactory().getXmlToc(
                            std_api_xml_root_element, "API")));
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
     * {@link com.stericsson.RefMan.Toc.DocumentFactory#getHtmlToc(com.stericsson.RefMan.Toc.Element)}
     * .
     */
    @Test
    public void testGetHtmlToc() {
        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(correctHtmlTocFile);
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
        XMLUnit.setIgnoreWhitespace(true);
        boolean passed = false;
        try {
            diff = new Diff(correctW3cDoc, new DOMOutputter()
                    .output(new DocumentFactory().getHtmlToc(html_root_element,
                            "API")));
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
     * {@link com.stericsson.RefMan.Toc.DocumentFactory#getHtmlToc(com.stericsson.RefMan.Toc.Element)}
     * with a Standard API toc file.
     */
    @Test
    public void testGetHtmlTocWithStdAPITocFile() {
        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(correctStdApiHtmlTocFile);
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
        XMLUnit.setIgnoreWhitespace(true);
        boolean passed = false;
        try {
            diff = new Diff(correctW3cDoc, new DOMOutputter()
                    .output(new DocumentFactory().getHtmlToc(
                            stdApi_html_root_element, "API")));
            passed = diff.similar();
            logger.info(diff.toString());
        } catch (JDOMException e) {
            logger.error("Failed to create a diff ", e);
        } catch (Exception e) {
            logger.error("Failed to create a diff ", e);
        }
        assertTrue(passed);
    }
}
