/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;

import org.junit.Before;
import org.junit.Test;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Validator.XSDValidator;

/**
 * This class is responsible for testing the ste-uml-export XDS Schema and
 * example files.
 *
 * @author Fredrik Lundström
 */
public class TestUmlExportExampleFilesAndSchema {

    /**
     * The file containing the schema
     */
    private File schema;

    /**
     * The validator used to validate the schema
     */
    private XSDValidator tocValidator;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        schema = new File("resources/ste-uml-export.xsd");
        tocValidator = new XSDValidator(schema);
    }

    /**
     * Test method for the XDS schema for the simple case.
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateCorrectSimple() throws SAXException, IOException {
        assertTrue(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-correct-simple.xml"));
    }

    /**
     * Test method for the XDS schema for the simple case with no mandatory
     * stuff.
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateCorrectSimpleNoMandatoryStuff()
            throws SAXException, IOException {
        assertTrue(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-correct-simple-nomandatorystuff.xml"));
    }

    /**
     * Test method for the XDS schema for the file required for the Pack 4
     * manual.
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateCorrectPack4() throws SAXException, IOException {
        assertTrue(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-correct-pack4.xml"));
    }

    /**
     * Test method for the XDS schema for the file that would have been exported
     * 2010-02-22.
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateCorrectModel20100222() throws SAXException,
            IOException {
        assertTrue(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-correct-model-2010-02-22.xml"));
    }

    /**
     * Test method for ensuring that at least one interface must be exported to
     * the file.
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateNotCorrectNoInteface() throws SAXException,
            IOException {
        assertFalse(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-notCorrect-nointerface.xml"));
    }

    /**
     * Test method for ensuring that platforms are mandatory in schema
     *
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateNotCorrectNoPlatforms() throws SAXException,
            IOException {
        assertFalse(tocValidator
                .validate("test/com/stericsson/RefMan/UmlExportFilter/ste-uml-export-notCorrect-noplatforms.xml"));
    }
}
