package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignedLoadModule;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500SignedLoadModuleTest extends TestCase {

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String DIRECTORY_FILE_FILENAME = ("/");

    private static final String UNSIGNED_ELF_FILENAME = "/u5500/unsigned_elf.elf";

    private static final String SIGNED_ELF_FILENAME = "signed_elf.elf";

    private GenericSignerSettings settings;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {
        settings =
            new U5500SignerSettings(Activator.getResourcesPath() + UNSIGNED_ELF_FILENAME, SIGNED_ELF_FILENAME);
    }

    /**
     * 
     */
    @Test
    public void testNonExistingFile() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE, NON_EXISITNG_FILE_FILENAME);
            GenericSignedLoadModule elf = new GenericSignedLoadModule(settings);
            elf.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * 
     */
    @Test
    public void testDirAsInputFile() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE, Activator.getResourcesPath()
                + DIRECTORY_FILE_FILENAME);
            GenericSignedLoadModule elf = new GenericSignedLoadModule(settings);
            elf.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * 
     */
    @Test
    public void testCreateArchive() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {

            GenericSignedLoadModule elf = new GenericSignedLoadModule(settings);

            assertEquals(0, elf.getDestinationAddress());
            GenericCertificate cert = new GenericSignedHeader();
            elf.setCertificate(cert);
            assertEquals(cert, elf.getCertificate());
            byte[] b = new byte[] {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            elf.setSignature(b);
            assertTrue(Arrays.equals(b, elf.getSignature()));
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

}
