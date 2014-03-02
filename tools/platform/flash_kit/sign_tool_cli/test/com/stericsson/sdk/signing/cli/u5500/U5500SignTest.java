package com.stericsson.sdk.signing.cli.u5500;

import static com.stericsson.sdk.ResourcePathManager.getResourceFilePath;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.cli.ParseException;
import org.junit.Test;

import com.stericsson.sdk.ResourcePathManager;
import com.stericsson.sdk.signing.DummySignerFactory;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.CommandLineProxy;
import com.stericsson.sdk.signing.cli.ICommandLineProxy;
import com.stericsson.sdk.signing.cli.ISignerFactory;
import com.stericsson.sdk.signing.cli.Sign;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerServiceListener;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xolabju
 */
public class U5500SignTest {

    private static final String SIGN_PACKAGE_U5500_ROOT = "FAKE_PKG";

    private static final String SIGN_PACKAGE_U5500_AUTH = "xolabju_u5500_auth";

    private static final String SIGN_PACKAGE_U5500_CRK = "crktest";

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String EMPTY_FILE_FILENAME = "test_files/empty.bin";

    private static final String DIRECTORY_FILE_FILENAME = ResourcePathManager.getResourceFilePath("test_files/");

    private static final String UNSIGNED_ISSW_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_ISSW_FILENAME = "signed_issw.bin";

    private static final String UNSIGNED_PRCMU_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_PRCMU_FILENAME = "signed_prcmu.bin";

    private static final String UNSIGNED_MEM_INIT_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_MEM_INIT_FILENAME = "signed_mem_init.bin";

    private static final String UNSIGNED_XLOADER_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_XLOADER_FILENAME = "signed_xloader.bin";

    private static final String UNSIGNED_OSLOADER_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_OSLOADER_FILENAME = "signed_osloader.bin";

    private static final String UNSIGNED_APE_NORMAL_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_APE_NORMAL_FILENAME = "signed_ape_normal.bin";

    private static final String UNSIGNED_LDR_FILENAME = ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_LDR_FILENAME = "signed_ldr.bin";

    private static final String UNSIGNED_MODEM_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_MODEM_FILENAME = "signed_modem.bin";

    private static final String UNSIGNED_FOTA_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_FOTA_FILENAME = "signed_fota.bin";

    private static final String UNSIGNED_DNT_FILENAME = ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_DNT_FILENAME = "signed_dnt.bin";

    private static final String UNSIGNED_IPL_FILENAME = ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_TRUSTED_FILENAME = "signed_trusted.bin";

    private static final String UNSIGNED_TRUSTED_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_FLASH_ARCHIVE_FILENAME = "signed_flash_archive.zip";

    private static final String UNSIGNED_FLASH_ARCHIVE_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String PRESIGNED_FLASH_ARCHIVE_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.zip");

    private static final String SIGNED_IPL_FILENAME = "signed_ipl.bin";

    private static final String UNSIGNED_AUTH_CERT_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_AUTH_CERT_FILENAME = "signed_authcert.bin";

    private static final String UNSIGNED_AUTH_CHALLANGE_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_AUTH_CHALLANGE_FILENAME = "signed_auth_challange.bin";

    private static final String UNSIGNED_CRKC_FILENAME =
        ResourcePathManager.getResourceFilePath("test_files/dummy.bin");

    private static final String SIGNED_CRKC_FILENAME = "signed_crkc.bin";

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

    /**
     * @throws Throwable
     *             exception
     * 
     */
    @Test
    public void testValidateInputNonExistingFile() throws Throwable {

        File unsignedFile = new File(NON_EXISITNG_FILE_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", unsignedFile.getAbsolutePath(),
                "outputfile"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 1);
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
        String[] arguments = new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", unsignedFile.getAbsolutePath(), "output"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 1);
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
        String[] arguments = new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", unsignedFile.getAbsolutePath(), "output"};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 1);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignISSW() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_ISSW_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_ISSW_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "--buffer-size", "8192", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "issw",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath()};
        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * 
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthChallenge() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CHALLANGE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CHALLANGE_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-v", "-p", SIGN_PACKAGE_U5500_AUTH, "-s", "auth_challenge", unsignedFile.getAbsolutePath(),
                signedFile.getAbsolutePath()};
        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     * 
     */
    @Test
    public void testSignGenericSoftwareFormat() throws Throwable {

        File unsignedFile;
        File signedFile;
        List<String[]> testSignArgumentsList = new LinkedList<String[]>();

        unsignedFile = new File(getResourceFilePath(UNSIGNED_PRCMU_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_PRCMU_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "prcmu", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MEM_INIT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MEM_INIT_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "mem_init", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_XLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_XLOADER_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "xloader", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_OSLOADER_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_OSLOADER_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "osloader", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_APE_NORMAL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_APE_NORMAL_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "ape_normal", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_LDR_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_LDR_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "ldr", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_MODEM_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_MODEM_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "modem", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_FOTA_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FOTA_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "fota", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_DNT_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_DNT_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "dnt", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        unsignedFile = new File(getResourceFilePath(UNSIGNED_IPL_FILENAME));
        signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_IPL_FILENAME);
        testSignArgumentsList.add(new String[] {
            "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "ipl", unsignedFile.getAbsolutePath(),
            signedFile.getAbsolutePath()});

        for (String[] testSignArguments : testSignArgumentsList) {
            testSigning(new File(testSignArguments[6]), new File(testSignArguments[7]), testSignArguments);
        }
    }

    /**
     * @throws Throwable
     *             tbd
     * 
     */
    @Test
    public void testSignTrustedApplication() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_TRUSTED_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_TRUSTED_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "trusted", unsignedFile.getAbsolutePath(),
                signedFile.getAbsolutePath()};
        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignAuthCert() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_AUTH_CERT_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_AUTH_CERT_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "auth_cert", unsignedFile.getAbsolutePath(),
                signedFile.getAbsolutePath()};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignFlashArchive() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_FLASH_ARCHIVE_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "--buffer-size", "1M", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "flash_archive",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath()};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testReSignFlashArchive() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(PRESIGNED_FLASH_ARCHIVE_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + "re-" + SIGNED_FLASH_ARCHIVE_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "--buffer-size", "8k", "-v", "-p", SIGN_PACKAGE_U5500_ROOT, "-s", "flash_archive",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath()};
        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignCRKC() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_CRKC_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_CRKC_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-v", "-p", SIGN_PACKAGE_U5500_CRK, "-s", "crkc", unsignedFile.getAbsolutePath(),
                signedFile.getAbsolutePath()};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignCRKCSHA256() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_CRKC_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_CRKC_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-r", "sha-256", "-v", "-p", SIGN_PACKAGE_U5500_CRK, "-s", "crkc",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath()};

        testSigning(unsignedFile, signedFile, arguments);
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSignCRKCInvalidHash() throws Throwable {
        File unsignedFile = new File(getResourceFilePath(UNSIGNED_CRKC_FILENAME));
        File signedFile = new File(unsignedFile.getParent() + File.separator + SIGNED_CRKC_FILENAME);
        String[] arguments =
            new String[] {
                "u5500", "-r", "sha-665", "-v", "-p", SIGN_PACKAGE_U5500_CRK, "-s", "crkc",
                unsignedFile.getAbsolutePath(), signedFile.getAbsolutePath()};
        Sign sign = new Sign(null, null);
        try {
            assertEquals(sign.execute(new DummySignerFactory(), arguments, null), 1);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
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
            "u5500", "-l"};
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
                    Sign.SIGN_U5500};
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
                    Sign.SIGN_U5500};
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
