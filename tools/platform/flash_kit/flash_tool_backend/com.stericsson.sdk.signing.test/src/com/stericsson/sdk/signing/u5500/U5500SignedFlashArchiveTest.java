package com.stericsson.sdk.signing.u5500;

import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignedFlashArchive;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500SignedFlashArchiveTest extends TestCase {

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String DIRECTORY_FILE_FILENAME = ("/");

    private static final String UNSIGNED_FLASH_ARCHIVE_FILENAME = "/u5500/unsigned_flasharchive.zip";

    private static final String SIGNED_FLASH_ARCHIVE_FILENAME = "signed_u5500_archive.zip";

    private GenericSignerSettings settings;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {
        settings =
            new U5500SignerSettings(Activator.getResourcesPath() + UNSIGNED_FLASH_ARCHIVE_FILENAME,
                SIGNED_FLASH_ARCHIVE_FILENAME);
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
            GenericSignedFlashArchive archive = new GenericSignedFlashArchive(settings);
            archive.getClass();
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
            GenericSignedFlashArchive archive = new GenericSignedFlashArchive(settings);
            archive.getClass();
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

            GenericSignedFlashArchive archive = new GenericSignedFlashArchive(settings);
            // cannot test the size since a version.txt is created and put into archive when the
            // unsigned archive becames signed
            // File file = new File(UNSIGNED_FLASH_ARCHIVE_FILENAME);
            // assertEquals(file.length(), archive.getLength());
            assertEquals(0, archive.getDestinationAddress());
            GenericCertificate cert = new GenericSignedHeader();
            archive.setCertificate(cert);
            assertEquals(cert, archive.getCertificate());
            byte[] b = new byte[] {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            archive.setSignature(b);
            assertTrue(Arrays.equals(b, archive.getSignature()));
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

}
