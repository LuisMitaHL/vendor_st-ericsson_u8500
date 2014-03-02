/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;

import org.junit.Before;
import org.junit.Test;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Validator.XSDValidator;

/**
 * This Class is responsible for testing the STD API XDS Schema and example
 * files.
 * 
 * @author Fredrik Lundström
 */
public class TestStdApiTocLocations {

    /**
     * The validator used to validate the schema
     */
    private XSDValidator tocValidator;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        tocValidator = new XSDValidator(new File("resources/API_StdToc.xsd"));
    }

    /**
     * Test method for the simple example file.
     * 
     * @throws SAXException
     * @throws IOException
     */
    @Test
    public void testValidateCorrectSimple() throws SAXException, IOException {
        assertTrue(tocValidator
                .validate("test/com/stericsson/RefMan/Toc/stdapi-correct.xml"));
    }

}
