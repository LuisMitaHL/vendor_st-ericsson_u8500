package com.stericsson.sdk.signing.cli.a2;

import static com.stericsson.sdk.ResourcePathManager.getResourceFilePath;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

import org.apache.commons.cli.ParseException;
import org.junit.Test;

import com.stericsson.sdk.signing.DummySignerFactory;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.CommandLineProxy;
import com.stericsson.sdk.signing.cli.ICommandLineProxy;
import com.stericsson.sdk.signing.cli.ISignerFactory;
import com.stericsson.sdk.signing.cli.Sign;
import com.stericsson.sdk.signing.cli.SignerFactory;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomlju
 */
public class A2SignTest {

    private static final String SIGN_PACKAGE_ALIAS = "FAKE_PACKAGE";

    private static final String EMPTY_FILE_FILENAME = "test_files/empty.bin";

    private static final String DIRECTORY_FILE_FILENAME = "test_files";

    private static final String NON_EXISITNG_FILE_FILENAME = "test_files/fakefile.fake";

    private static final String UNSIGNED_LOADER_FILENAME = "test_files/dummy.bin";

    private static final String UNSIGNED_ACC_PRE_LOADER_FILENAME = "test_files/dummy.bin";

    private static final String SIGNED_LOADER_FILENAME = "dummy.ldr";

    private static final String SIGNED_ACC_PRE_LOADER_FILENAME = "dummy.ldr";

    private static final String UNSIGNED_TAR_FILENAME = "test_files/dummy.tar";

    private static final String SIGNED_TAR_FILENAME = "dummy.ssw";

    private static final String UNSIGNED_ELF_FILENAME = "test_files/dummy.elf";

    private static final String UNSIGNED_ELF_WITH_SECTIONS_FILENAME = "test_files/dummy.elf";

    private static final String SIGNED_ELF_FILENAME = "dummy.ssw";

    private static final String SIGNED_ELF_WITH_SECTIONS_FILENAME = "dummy.ssw";

    private static final String UNSIGNED_SRECORD_FILENAME = "test_files/dummy.a01";

    private static final String SIGNED_SRECORD_FILENAME = "dummy.ssw";

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
                return new ISignerFacade() {

                    public void sign(ISignerSettings settings, ISignerListener listener, boolean local)
                        throws SignerException {

                    }

                    public ISignPackageInformation[] getSignPackages(ISignerServiceListener listener, boolean local, 
                            boolean common, ISignerSettings signerSettings) throws SignerException {
                        return null;
                    }

                    public ISignPackage getSignPackage(String packageName, ISignerServiceListener listener,
                        boolean local, boolean common, ISignerSettings signerSettings) throws SignerServiceException {
                        return null;
                    }
                };
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
                // TODO Auto-generated method stub
                return null;
            }

            public ISignerFacade createSignerFacade(String type) {
                return new ISignerFacade() {

                    public void sign(ISignerSettings settings, ISignerListener listener, boolean local)
                        throws SignerException {

                    }

                    public ISignPackageInformation[] getSignPackages(ISignerServiceListener listener, boolean local, 
                            boolean common, ISignerSettings signerSettings) throws SignerException {
                        return null;
                    }

                    public ISignPackage getSignPackage(String packageName, ISignerServiceListener listener,
                        boolean local, boolean common, ISignerSettings signerSettings) throws SignerServiceException {
                        return null;
                    }
                };
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

    /**
     * @throws Throwable
     *             d
     * 
     */
    @Test
    public void testValidateInputNonExistingFile() throws Throwable {

        File unsignedFile = new File(NON_EXISITNG_FILE_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-t", "app", "-m", "3rdSBC",
                "--interactive=static", "--hdr-sec=verifyOnly", "--addr-format=pages", "--app-sec=on",
                "--sw-version=4", unsignedFile.getAbsolutePath(), "output"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new SignerFactory(), arguments, null), 1);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * @throws Throwable
     *             exception
     * 
     */
    @Test
    public void testValidateInputEmptyFile() throws Throwable {

        File unsignedFile = new File(getResourceFilePath(EMPTY_FILE_FILENAME));
        String[] arguments =
            new String[] {
                "a2", "-v", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-t", "app", "-m", "3rdSBC",
                "--interactive=static", "--hdr-sec=verifyOnly", "--addr-format=pages", "--app-sec=on",
                "--sw-version=4", unsignedFile.getAbsolutePath(), "output"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new SignerFactory(), arguments, null), 1);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * @throws Throwable
     *             exception
     * 
     */
    @Test
    public void testValidateInputFolderFile() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(DIRECTORY_FILE_FILENAME));
        String[] arguments =
            new String[] {
                "a2", "-v", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-t", "app", "-m", "3rdSBC",
                "--interactive=static", "--hdr-sec=verifyOnly", "--addr-format=4B", "--app-sec=on", "--sw-version=4",
                unsignedFile.getAbsolutePath(), "output"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new SignerFactory(), arguments, null), 1);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignLoader() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LOADER_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignECLoaderOptions() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LOADER_FILENAME);
        String[] macModes = new String[] {
            "Config", "2ndSBC", "3rdSBC", "4thSBC", "SW", "Licence", "Mac7", "Mac8"};

        String[] pTypes = new String[] {
            "nand", "nor", "physical"};

        String[] hdrSecs = new String[] {
            "asIs=0x0", "replaceSign=0x0", "verifyOnly"};

        String[] interactives = new String[] {
            "static", "interactive"};

        String[] appSecs = new String[] {
            "on", "off", "dataLoading", "PAFdependent"};

        String[] addrFormats = new String[] {
            "pages", "4B"};

        for (String macMode : macModes) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", macMode,
                "-sw-version", "0"});
        }
        for (String pType : pTypes) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", pType, "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String hdrSec : hdrSecs) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", hdrSec, "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String interactive : interactives) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                interactive, unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String appSec : appSecs) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", appSec,
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String addrFormat : addrFormats) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", addrFormat, "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignECAccPreLoaderOptions() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ACC_PRE_LOADER_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ACC_PRE_LOADER_FILENAME);
        String[] macModes = new String[] {
            "Config", "2ndSBC", "3rdSBC", "4thSBC", "SW", "Licence", "Mac7", "Mac8"};

        String[] pTypes = new String[] {
            "nand", "nor", "physical"};

        String[] hdrSecs = new String[] {
            "asIs=0x0", "replaceSign=0x0", "verifyOnly"};

        String[] interactives = new String[] {
            "static", "interactive"};

        String[] appSecs = new String[] {
            "on", "off", "dataLoading", "PAFdependent"};

        String[] addrFormats = new String[] {
            "pages", "4B"};

        for (String macMode : macModes) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", macMode,
                "-sw-version", "0"});
        }
        for (String pType : pTypes) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", pType, "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String hdrSec : hdrSecs) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", hdrSec, "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String interactive : interactives) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                interactive, unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String appSec : appSecs) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", appSec,
                "-addr-format", "4B", "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

        for (String addrFormat : addrFormats) {
            testSigning(unsignedFile, signedFile, new String[] {
                "a2", "-e", "-c", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "loader", "-interactive",
                "static", unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on",
                "-addr-format", addrFormat, "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config",
                "-sw-version", "0"});
        }

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignSRecordImage() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_SRECORD_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_SRECORD_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "image", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "asIs=0x0", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignTARFile() throws Throwable {

        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TAR_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TAR_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "tar", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "verifyOnly", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsImage() throws Throwable {

        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "image", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "asIs=0x0", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsELF() throws Throwable {

        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "elf", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "asIs=0x0", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignELFFileAsELFWithSections() throws Throwable {

        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ELF_WITH_SECTIONS_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ELF_WITH_SECTIONS_FILENAME);
        String[] arguments =
            new String[] {
                "a2", "-v", "-t", "app", "-p", SIGN_PACKAGE_ALIAS, "-s", "elf", "-interactive", "static",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath(), "-app-sec", "on", "-addr-format", "4B",
                "-hdr-sec", "asIs=0x0", "-ptype", "nand", "-short-mac", "-m", "Config", "-sw-version", "0"};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    private void testSigning(File unsignedFile, File signedFile, String[] arguments) throws Throwable {
        Sign sign = new Sign(null, null);

        assertTrue(unsignedFile.exists());
        assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 0);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testListSignPackages() throws Throwable {

        String[] arguments = new String[] {
            "a2", "-l"};
        Sign sign = new Sign(null, null);

        assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 0);
    }

    /** */
    @Test
    public void testMissingArguments() {
        ICommandLineProxy cLine = new CommandLineProxy() {
            public boolean hasOption(String shortCMD) {
                return true;
            }

            public String[] getArgs() {
                return new String[] {
                    Sign.SIGN_A2};
            }
        };

        try {
            Sign sign = new Sign(null, null);
            sign.validateArguments(cLine, null);
            assertTrue(true);
        } catch (ParseException e) {
            assertTrue(false);
        }
    }

    /** */
    @Test
    public void testMissingArgumentsFails() {
        ICommandLineProxy cLine = new CommandLineProxy() {
            public boolean hasOption(String shortCMD) {
                return false;
            }

            public String[] getArgs() {
                return new String[] {
                    Sign.SIGN_A2};
            }
        };

        try {
            Sign sign = new Sign(null, null);
            sign.validateArguments(cLine, null);
            assertTrue(false);
        } catch (ParseException e) {
            assertTrue(true);
        }
    }

}
