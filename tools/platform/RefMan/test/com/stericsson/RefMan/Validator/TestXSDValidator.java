/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Validator;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 * This Class is responsible for testing the XSDValidator class.
 * 
 * @author Thomas Palmqvist
 */
public class TestXSDValidator {

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
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Validator.XSDValidator#validate(java.lang.String)}
     * .
     */
    @Test
    public void testValidateCorrect() {
        try {
            // The schema the xml file should conform to.
            File schema = new File("resources/API_Toc.xsd");

            XSDValidator APITocValidator = new XSDValidator(schema);

            assertTrue(APITocValidator
                    .validate("test/com/stericsson/RefMan/Validator/correct.xml"));
            assertFalse(APITocValidator
                    .validate("test/com/stericsson/RefMan/Validator/notCorrect.xml"));
        } catch (Exception e) {
            e.printStackTrace();
            fail();
        }
    }

}
