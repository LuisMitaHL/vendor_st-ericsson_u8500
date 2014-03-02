/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.TemplateComparator;

import static org.junit.Assert.fail;

import java.io.File;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.input.SAXBuilder;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author etompio
 *
 */
public class TestXMLVerifier {

    /** logger */
    final static Logger logger = LoggerFactory.getLogger(TestXMLVerifier.class);

    /**
     * Test method for
     * {@link com.stericsson.RefMan.TemplateComparator.XMLVerifier#compareFileToTemplateFile(java.io.File, java.io.File)}
     * .
     */
    @Test
    public void testCompareFileToTemplateFileNegative() {

        XMLVerifier ver = new XMLVerifier();
        File tmpFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/incorrect_template.xml");
        File srcFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/stdApi-docbook.xml");

        if (ver.compareFileToTemplateFile(tmpFile, srcFile)) {
            fail("Verification of xml file " + srcFile.getAbsolutePath()
                    + " according to template " + tmpFile.getAbsolutePath()
                    + " passes while it should fail");
        }

    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.TemplateComparator.XMLVerifier#compareFileToTemplateFile(java.io.File, java.io.File)}
     * .
     */
    @Test
    public void testCompareFileToTemplateFilePositive() {

        XMLVerifier ver = new XMLVerifier();
        File tmpFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/correct_template.xml");
        File srcFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/stdApi-docbook.xml");

        if (!ver.compareFileToTemplateFile(tmpFile, srcFile)) {
            fail("Verification of xml file " + srcFile.getAbsolutePath()
                    + " according to template " + tmpFile.getAbsolutePath()
                    + " fails while it should pass");
        }

    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.TemplateComparator.XMLVerifier#compareFileToTemplateFile(java.io.File, java.io.File)}
     * .
     */
    @Test
    public void testCompareFileToTemplateFileNegative2() {

        XMLVerifier ver = new XMLVerifier();
        File tmpFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/correct_template.xml");
        File srcFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/stdApi-docbook_withErrors.xml");

        if (ver.compareFileToTemplateFile(tmpFile, srcFile)) {
            fail("Verification of xml file " + srcFile.getAbsolutePath()
                    + " according to template " + tmpFile.getAbsolutePath()
                    + " succeeds while it should fail");
        }

    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.TemplateComparator.XMLVerifier#compareFileToTemplateFile(java.io.File, java.io.File)}
     * . Test verifies the template file used by refman with the docbook file
     * delivered as template for documentation in guidelines.
     */
    @Test
    public void testActualTemplateFilesPositive() {

        XMLVerifier ver = new XMLVerifier();
        File tmpFile = new File("resources" + File.separator
                + "StdApi_docbook_template.xml");
        File srcFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/userspace_std_api_template.xml");

        if (!ver.compareFileToTemplateFile(tmpFile, srcFile)) {
            fail("Verification of recommended xml file "
                    + srcFile.getAbsolutePath()
                    + " according to actual template "
                    + tmpFile.getAbsolutePath() + " fails while it should pass");
        }
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.TemplateComparator.XMLVerifier# compareElementToTemplateFile(File, xmlRootElement)}
     * . Test verifies the template file used by refman with the docbook file
     * delivered as template for documentation in guidelines.
     */
    @Test
    public void testCompareElementToTemplateFile() {
        XMLVerifier ver = new XMLVerifier();

        File templateFile = new File("resources" + File.separator
                + "StdApi_docbook_template.xml");

        File srcFile = new File(
                "test/com/stericsson/RefMan/TemplateComparator/userspace_std_api_template.xml");

        // open template file
        Element srcRoot;
        Document templateXML = null;
        SAXBuilder builder = new SAXBuilder();
        builder
                .setFeature(
                        "http://apache.org/xml/features/nonvalidating/load-external-dtd",
                        false);
        try {
            templateXML = builder.build(srcFile);
            srcRoot = templateXML.getRootElement();
            srcRoot.removeAttribute("id");

            if (!ver.compareElementToTemplateFile(templateFile, srcRoot,
                    srcFile.getAbsolutePath())) {
                fail("Verification of recommended xml file "
                        + srcFile.getAbsolutePath()
                        + " according to actual template "
                        + templateFile.getAbsolutePath()
                        + " fails while it should pass");
            }
        } catch (Exception e) {
            logger.error("Error reading template xml file {}. Error: {}",
                    srcFile.getAbsolutePath(), e.getMessage());
        }
    }
}
