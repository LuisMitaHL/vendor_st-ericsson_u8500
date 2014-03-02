package com.stericsson.sdk.signing.cli;

import static com.stericsson.sdk.ResourcePathManager.getResourceFilePath;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

import org.apache.commons.cli.ParseException;
import org.apache.log4j.Logger;
import org.junit.Test;

import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.SoftwareAlreadySignedException;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * 
 * @author mielcluk
 * 
 */
public class SignTest {

    private static final String SIGN_PACKAGE_U5500_ROOT = "FAKE_PKG";

    private static final String EMPTY_FILE_FILENAME = "test_files/empty.bin";

    /**
     * 
     */
    @Test
    public void testExecute1NoArguments() {

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();

        try {
            Sign sign = new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8"));
            sign.execute(new String[] {});
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        String helpText = byteArrayOutputStream.toString();
        byteArrayOutputStream.reset();

        String line = helpText.substring(0, helpText.indexOf("\n\n"));
        helpText = helpText.substring(helpText.indexOf("\n\n") + 2);

        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("No arguments given, possible usage is:"));

        assertTrue("Help does not contain info about sign-tool-a2", helpText.contains("usage: sign-tool-a2"));
        assertTrue("Help does not contain info about sign-tool-u8500 ", helpText.contains("usage: sign-tool-u8500"));
        assertTrue("Help does not contain info about sign-tool-u5500", helpText.contains("usage: sign-tool-u5500"));

    }

    /**
     * 
     */
    @Test
    public void testExecute2NoArguments() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments) throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return null;
            }

            public ISignerFacade createSignerFacade(String type) {
                return null;
            }
        };

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();

        try {
            Sign sign = new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8"));
            sign.execute(factory, new String[] {}, null);
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }

        String helpText = byteArrayOutputStream.toString();
        byteArrayOutputStream.reset();

        String line = helpText.substring(0, helpText.indexOf("\n\n"));
        helpText = helpText.substring(helpText.indexOf("\n\n") + 2);

        assertTrue("Help line doesn't match our intended format, it was: \'" + line + "'", line
            .matches("No arguments given, possible usage is:"));
        assertTrue("Help does not contain info about sign-tool-a2", helpText.contains("usage: sign-tool-a2"));
        assertTrue("Help does not contain info about sign-tool-u8500 ", helpText.contains("usage: sign-tool-u8500"));
        assertTrue("Help does not contain info about sign-tool-u5500", helpText.contains("usage: sign-tool-u5500"));

    }

    /**
     * 
     */
    @Test
    public void testExecuteWithoutSignerSettings() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments) throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return null;
            }

            public ISignerFacade createSignerFacade(String type) {
                return new ISignerFacade() {

                    public ISignPackage getSignPackage(String pPackageName, ISignerServiceListener pListener,
                        boolean pLocal, boolean common, ISignerSettings signerSettings) throws SignerServiceException {
                        return null;
                    }

                    public ISignPackageInformation[] getSignPackages(ISignerServiceListener pListener, boolean pLocal, 
                            boolean common, ISignerSettings signerSettings) throws SignerException {
                        return null;
                    }

                    public void sign(ISignerSettings pSettings, ISignerListener pListener, boolean pLocal)
                        throws SignerException {
                    }
                };
            }
        };

        String path = new File(getResourceFilePath(EMPTY_FILE_FILENAME)).getAbsolutePath();
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        try {
            Sign sign = new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8"));
            try {
                assertEquals(1, sign.execute(factory, new String[] {
                    "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", path, "output"}, null));
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testExecuteWithoutSignerFacade() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments) throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return new ISignerSettings() {

                    public Object getSignerSetting(String pKey) {
                        return null;
                    }

                    public void setFrom(ISignerSettings pSettings) throws SignerSettingsException {
                    }

                    public void setSignerSetting(String pKey, Object pValue) throws SignerSettingsException {
                    }
                };
            }

            public ISignerFacade createSignerFacade(String type) {
                return null;
            }
        };

        String path = new File(getResourceFilePath(EMPTY_FILE_FILENAME)).getAbsolutePath();
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        try {
            Sign sign = new Sign(null, new PrintStream(byteArrayOutputStream, false, "UTF-8"));
            try {
                assertEquals(1, sign.execute(factory, new String[] {
                    "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", path, "output"}, null));
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());

        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testExecuteWithSignedElfFile() {

        ISignerFactory factory = new ISignerFactory() {

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return new ISignerSettings() {

                    public void setSignerSetting(String key, Object value) throws SignerSettingsException {
                    }

                    public void setFrom(ISignerSettings settings) throws SignerSettingsException {
                    }

                    public Object getSignerSetting(String key) {
                        return null;
                    }
                };
            }

            public ISignerFacade createSignerFacade(String type) {
                return new ISignerFacade() {

                    public ISignPackage getSignPackage(String pPackageName, ISignerServiceListener pListener,
                        boolean pLocal, boolean common, ISignerSettings signerSettings) throws SignerServiceException {
                        return null;
                    }

                    public ISignPackageInformation[] getSignPackages(ISignerServiceListener pListener, boolean pLocal, 
                            boolean common, ISignerSettings signerSettings) throws SignerException {
                        return null;
                    }

                    public void sign(ISignerSettings pSettings, ISignerListener pListener, boolean pLocal)
                        throws SignerException {
                        throw new SoftwareAlreadySignedException("software already signed");
                    }
                };
            }
        };

        Logger logger = Logger.getLogger(Sign.class.getName());
        String path = new File(getResourceFilePath(EMPTY_FILE_FILENAME)).getAbsolutePath();
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        try {
            Sign sign = new Sign(logger, new PrintStream(byteArrayOutputStream, false, "UTF-8"));
            try {
                assertEquals(0, sign.execute(factory, new String[] {
                    "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "elf", path, "output"}, logger));
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testExecuteWithException() {

        ISignerFactory factory = new ISignerFactory() {

            @SuppressWarnings("unused")
            ISignerSettings createSignerSettings(ISignPackage signPackage, Object[] arguments) throws SignerException {
                return null;
            }

            public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
                throws SignerException, NullPointerException, ParseException {
                return new ISignerSettings() {

                    public Object getSignerSetting(String pKey) {
                        return null;
                    }

                    public void setFrom(ISignerSettings pSettings) throws SignerSettingsException {
                    }

                    public void setSignerSetting(String pKey, Object pValue) throws SignerSettingsException {
                    }
                };
            }

            public ISignerFacade createSignerFacade(String type) {
                return null;
            }
        };

        try {
            Logger logger = Logger.getLogger(Sign.class.getName());
            Sign sign = new Sign(logger, null);
            try {
                assertEquals(1, sign.execute(factory, new String[] {
                    "u5500"}, null));
            } catch (Exception e) {
                System.out.println(e.getMessage());
            }
        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testSignerMessage() {
        try {
            Logger logger = Logger.getLogger(Sign.class.getName());
            PrintStream ps = new PrintStream(System.out, false, "UTF-8");
            Sign sign = new Sign(logger, ps);
            sign.signerMessage(null, "Test message.");
        } catch (UnsupportedEncodingException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetErrorLogFilePath() {
        try {
            System.out.println(Sign.getErrorLogFilePath());
        } catch (SecurityException e) {
            System.out.println(e.getMessage());
        }
    }

}
