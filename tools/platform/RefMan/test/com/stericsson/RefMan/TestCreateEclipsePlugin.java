/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;

import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLUnit;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.Namespace;
import org.jdom.input.SAXBuilder;
import org.jdom.output.DOMOutputter;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author emarttr
 *
 */
public class TestCreateEclipsePlugin {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestCreateEclipsePlugin.class);

    /** The output folder */
    private File outDir;

    /** The html outputfolder */
    private File htmlDir;

    /** A file to test the output against. */
    private static final String testFile = "test/com/stericsson/RefMan/pluginTestFile.xml";

    /** The JDom <code>Element</code> representing the plugin.xml file. */
    private Element plugin;

    /** The JDom <code>Element</code> representing the 'platform'.xml file. */
    private Element testPlatform;

    /** The namespace of the xml files. */
    private static final String testPlatFormNameSpace = "http://www.stericsson.com/refman/API_output_Toc.xsd";

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        outDir = new File("test/com/stericsson/RefMan/outDir");
        if (!outDir.exists()) {
            outDir.mkdirs();
        }
        htmlDir = new File(outDir, "html");
        if (!htmlDir.exists()) {
            htmlDir.mkdirs();
        }

        Element an_include_file = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        an_include_file.setAttribute("label", "an_include_file.h");
        an_include_file
                .setAttribute("href", "html/1/an__include__file_8h.html");

        Element another_include_file = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        another_include_file.setAttribute("label", "another_include_file.h");
        another_include_file.setAttribute("href",
                "html/1/another__include__file_8h.html");

        Element testfile1 = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        testfile1.setAttribute("label", "testFile1");
        testfile1.setAttribute("href",
                "html/Multimedia_User space_testFile1.html");
        testfile1.addContent(an_include_file);
        testfile1.addContent(another_include_file);

        Element opengl = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        opengl.setAttribute("label", "Open GL ES");
        opengl.setAttribute("href", "html/opengl/index.html");

        Element userSpace = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        userSpace.setAttribute("label", "User space");
        userSpace.setAttribute("href", "html/Multimedia_User space.html");
        userSpace.addContent(opengl);
        userSpace.addContent(testfile1);

        Element multimedia = new Element("topic", Namespace
                .getNamespace(testPlatFormNameSpace));
        multimedia.setAttribute("label", "Multimedia");
        multimedia.setAttribute("href", "html/Multimedia.html");
        multimedia.addContent(userSpace);

        testPlatform = new Element("toc", Namespace
                .getNamespace(testPlatFormNameSpace));
        testPlatform.setAttribute("label", "testPlatform_testVersion");
        testPlatform.setAttribute("topic", "html/Mainpage.html");
        testPlatform.addContent(multimedia);

        Element toc = new Element("toc");
        toc.setAttribute("category", "ST-Ericsson Reference Manual");
        toc.setAttribute("extradir", "html");
        toc.setAttribute("file", "testPlatform.xml");
        toc.setAttribute("primary", "true");

        Element extension = new Element("extension");
        extension.setAttribute("point", "org.eclipse.help.toc");
        extension.addContent(toc);

        plugin = new Element("plugin");
        plugin.addContent(extension);
    }

    /**
     * @throws java.lang.Exception
     */
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
     * {@link com.stericsson.RefMan.CreateEclipsePlugin#CreateEclipsePlugin()} .
     */
    @Test
    public void testCreateEclipsePlugin() {
        CreateEclipsePlugin createEclipsePlugin = new CreateEclipsePlugin(
                outDir, htmlDir, "testPlatform", "test_manual", "testVersion");
        createEclipsePlugin.createPlugin(testFile);

        File[] outFiles = outDir.listFiles();
        assertTrue(outFiles.length == 4);

        File[] htmlFiles = htmlDir.listFiles();
        assertTrue(htmlFiles.length == 4);

        File manifest = new File(outDir, "META-INF" + File.separator
                + "MANIFEST.MF");
        assertTrue(manifest.exists());

        compareFile(new File(outDir, "testPlatform.xml"), testPlatform);
        // compareFile(new File(outDir, "plugin.xml"), plugin);
    }

    /**
     * Help method which compares an JDOM <code>Element</code> structure to a
     * specified file.
     *
     * @param file
     *            The file to compare against.
     * @param elm
     *            The <code>Element</code> to compare.
     */
    private void compareFile(File file, Element elm) {
        final DOMOutputter outputter = new DOMOutputter();

        Diff diff = null;
        Document correctDoc = null;
        SAXBuilder builder = new SAXBuilder();

        try {
            correctDoc = builder.build(file);
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
                    .output(new Document(elm)));
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
