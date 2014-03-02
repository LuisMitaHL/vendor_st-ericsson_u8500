package com.stericsson.sdk.signing.cli;

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
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * @author xolabju
 */
public class GenericCommandLineOptionsTest {

    GenericCommandLineOptions.Validator validator = null;

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        validator = new GenericCommandLineOptions.Validator();
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
        assertNotNull(GenericCommandLineOptions.getSupportedSoftwareTypes());

        assertEquals("FORCE", GenericCommandLineOptions.GenericOption.FORCE.name());

        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.ISSW.name()));
        try {
            assertEquals("sw-type", GenericCommandLineOptions.GenericOption.SW_TYPE.getLongCMD());
            assertEquals("s", GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD());
            assertTrue(GenericCommandLineOptions.GenericOption.SW_TYPE.takesArg());
            assertFalse(GenericCommandLineOptions.GenericOption.FORCE.takesArg());
            assertTrue(GenericCommandLineOptions.GenericOption.SW_TYPE.isRequired());
            assertFalse(GenericCommandLineOptions.GenericOption.FORCE.isRequired());
        } catch (NullPointerException e) {
            assertTrue(e.getMessage(), false);
        } catch (ParseException e) {
            assertTrue(e.getMessage(), false);
        }

        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.TRUSTED.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.PRCMU.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.MEM_INIT.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.XLOADER.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.OSLOADER.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.APE_NORMAL.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.LDR.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.MODEM.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.FOTA.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.DNT.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.AUTH_CHALLENGE.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.IPL.name()));
        assertTrue(GenericCommandLineOptions.isSWTypeSupported(GenericSoftwareType.FLASH_ARCHIVE.name()));
        assertFalse(GenericCommandLineOptions.isSWTypeSupported("fakeoption"));

        try {
            GenericCommandLineOptions u5500CommandLineOptions = new GenericCommandLineOptions();
            assertTrue(u5500CommandLineOptions.hasOption("sw-type"));
        } catch (NullPointerException e) {
            assertTrue(e.getMessage(), false);
        } catch (ParseException e) {
            assertTrue(e.getMessage(), false);
        }
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
                    "u5500", "--help"}, null));
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
            .matches("usage: sign-tool-u5500 \\[OPTIONS\\]( [^ ]+)+"));

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
                    "u5500", "-h"}, null));
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
            .matches("usage: sign-tool-u5500 \\[OPTIONS\\]( [^ ]+)+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));

        line = helpText.substring(0, helpText.indexOf("\n") - 1);
        helpText = helpText.substring(helpText.indexOf("\n") + 1);
        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("[ ]*(-.,)?--[a-zA-Z-]{2,}[*]?([ ]?<arg>)?[ ]?.+"));
    }
}
