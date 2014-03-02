package com.stericsson.sdk.signing.cli.a2;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

import org.apache.commons.cli.ParseException;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.ICommandLineProxy;
import com.stericsson.sdk.signing.cli.ISignerFactory;
import com.stericsson.sdk.signing.cli.Sign;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * @author Daniel Ekelund <daniel.xd.ekelund@stericsson.com>
 */
public class A2CommandLineOptionsTest {

    A2CommandLineOptions.Validator validator = null;

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        validator = new A2CommandLineOptions.Validator();
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
            assertTrue("Thrown parse exception for valid input! Input: -a", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown null pointer exception for valid input! Input: -a", false);
        }

        try {
            validator.validShortCMD("a");
        } catch (ParseException e) {
            assertTrue("Thrown parse exception for valid input! Input: a", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown null pointer exception for valid input! Input: a", false);
        }

        try {
            validator.validShortCMD(null);
        } catch (ParseException e) {
            assertTrue("Thrown parse exception for valid input! Input: null", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown null pointer exception for valid input! Input: null", false);
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
            assertTrue(true);
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
            assertTrue("Thrown parse exception for valid input!", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown null pointer exception for valid input!", false);
        }

        try {
            validator.validLongCMD("--force");
        } catch (ParseException e) {
            assertTrue("Thrown parse exception for valid input!", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown null pointer exception for valid input!", false);
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidInputForLongCMD() {
        try {
            validator.validLongCMD("");
            assertTrue("Constructor should throw ParseException when" + " you use an empty string as short command.",
                false);
        } catch (NullPointerException e) {
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (ParseException e) {
            assertTrue(true);
        }

        try {
            validator.validLongCMD(null);
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (ParseException e) {
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (NullPointerException e) {
            assertTrue(true);
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
            assertTrue("Thrown parse exception for valid input!", false);
        } catch (NullPointerException e) {
            assertTrue("Thrown parse exception for valid input!", false);
        }
    }

    /**
     * 
     */
    @Test
    public void testInvalidInputForDescr() {
        try {
            validator.validDescription("");
            assertTrue("Constructor should throw ParseException when" + " you use an empty string as short command.",
                false);
        } catch (NullPointerException e) {
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (ParseException e) {
            assertTrue(true);
        }

        try {
            validator.validDescription(null);
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (ParseException e) {
            assertTrue("Constructor should throw ParseException when" + " you use null as short command.", false);
        } catch (NullPointerException e) {
            assertTrue(true);
        }
    }

    /**
     * 
     */
    @Test
    public void testSupportedValues() {
        assertNotNull(A2CommandLineOptions.getSupportedSoftwareVersion());
        assertNotNull(A2CommandLineOptions.getSupportedMacMode());
        assertNotNull(A2CommandLineOptions.getSupportedPayloadTypes());
        assertNotNull(A2CommandLineOptions.getSupportedHeaderSecurity());
        assertNotNull(A2CommandLineOptions.getSupportedAddressFormat());
        assertNotNull(A2CommandLineOptions.getSupportedApplicationSecurity());
        assertNotNull(A2CommandLineOptions.getSupportedInteractiveLoadBits());
        assertNotNull(A2CommandLineOptions.getSupportedSoftwareTypes());
        assertNotNull(A2CommandLineOptions.getSupportedTargetCPU());
    }

    /**
     * 
     */
    @Test
    public void testDisplayHelp() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            public ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments)
                throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                // TODO Auto-generated method stub
                return null;
            }

            public ISignerFacade createSignerFacade(String type) {
                // TODO Auto-generated method stub
                return null;
            }
        };

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        try {
            assertEquals(0, new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8")).execute(factory,
                new String[] {
                    "a2", "--help"}, null));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }
        String helpText = byteArrayOutputStream.toString();
        byteArrayOutputStream.reset();

        String line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("usage: sign-tool-a2 \\[OPTIONS\\]( [^ ]+)+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));
    }

    /**
     * 
     */
    @Test
    public void testDisplayH() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            public ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments)
                throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return null;
            }

            public ISignerFacade createSignerFacade(String type) {
                // TODO Auto-generated method stub
                return null;
            }
        };

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        try {
            assertEquals(0, new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8")).execute(factory,
                new String[] {
                    "a2", "-h"}, null));
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }
        String helpText = byteArrayOutputStream.toString();
        byteArrayOutputStream.reset();

        String line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("usage: sign-tool-a2 \\[OPTIONS\\]( [^ ]+)+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));
    }


    /***/
    @Test
    public void testIsHeaderSecuritySupported() {
        assertFalse(A2CommandLineOptions.isHeaderSecuritySupported("xx"));
        assertTrue(A2CommandLineOptions.isHeaderSecuritySupported("verifyOnly"));
    }
}
