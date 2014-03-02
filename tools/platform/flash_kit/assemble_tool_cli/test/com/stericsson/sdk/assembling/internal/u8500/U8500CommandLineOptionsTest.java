package com.stericsson.sdk.assembling.internal.u8500;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.apache.commons.cli.ParseException;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * @author xolabju
 */
public class U8500CommandLineOptionsTest {

    U8500CommandLineOptions.Validator validator = null;

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        validator = new U8500CommandLineOptions.Validator();
    }

    /**
     * @throws Exception
     *             TBD
     */
    @After
    public void tearDown() throws Exception {
        validator = null;
    }

    /**
     * 
     */
    @Test
    public void testValidInputForShortCMD() {
        try {
            validator.validShortCMD("-a");
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input! Input: -a");
        } catch (NullPointerException e) {
            fail("Thrown null pointer exception for valid input! Input: -a");
        }

        try {
            validator.validShortCMD("a");
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input! Input: a");
        } catch (NullPointerException e) {
            fail("Thrown null pointer exception for valid input! Input: a");
        }

        try {
            validator.validShortCMD(null);
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input! Input: null");
        } catch (NullPointerException e) {
            fail("Thrown null pointer exception for valid input! Input: null");
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidInputForShortCMD() {
        try {
            validator.validShortCMD("");
            assertTrue("Constructor should throw ParseException when you use an empty string as short command.", false);
        } catch (ParseException e) {
            assertEquals("Empty Strings not valid as a short command, should be NULL value.", e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testValidInputForLongCMD() {
        try {
            validator.validLongCMD("force");
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input!");
        } catch (NullPointerException e) {
            fail("Thrown null pointer exception for valid input!");
        }

        try {
            validator.validLongCMD("--force");
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input!");
        } catch (NullPointerException e) {
            fail("Thrown null pointer exception for valid input!");
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidInputForLongCMD() {
        try {
            validator.validLongCMD("");
            fail("Constructor should throw ParseException when" + " you use an empty string as long command.");
        } catch (NullPointerException e) {
            fail("Constructor should throw ParseException when" + " you use null as long command.");
        } catch (ParseException e) {
            assertEquals("Empty String not valid as long command", e.getMessage());
        }

        try {
            validator.validLongCMD(null);
            fail("Constructor should throw ParseException when" + " you use null as long command.");
        } catch (ParseException e) {
            fail("Constructor should throw ParseException when" + " you use null as long command.");
        } catch (NullPointerException e) {
            assertEquals("NULL not valid as long command", e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testValidInputForDescr() {
        try {
            validator.validDescription("This is a good description");
        } catch (ParseException e) {
            fail("Thrown parse exception for valid input!");
        } catch (NullPointerException e) {
            fail("Thrown parse exception for valid input!");
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidInputForDescr() {
        try {
            validator.validDescription("");
            fail("Constructor should throw ParseException when" + " you use an empty string as description.");
        } catch (NullPointerException e) {
            fail("Constructor should throw ParseException when" + " you use null as description.");
        } catch (ParseException e) {
            assertTrue(true);
        }

        try {
            validator.validDescription(null);
            fail("Constructor should throw ParseException when" + " you use null as description.");
        } catch (ParseException e) {
            fail("Constructor should throw ParseException when" + " you use null as description.");
        } catch (NullPointerException e) {
            assertEquals("NULL not valid as description", e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSupportedValues() {
        assertNotNull(U8500CommandLineOptions.getSupportedSoftwareTypes());
    }

}
