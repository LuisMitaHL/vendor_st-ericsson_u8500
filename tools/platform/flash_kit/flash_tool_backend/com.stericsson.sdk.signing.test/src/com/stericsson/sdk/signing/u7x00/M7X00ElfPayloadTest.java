package com.stericsson.sdk.signing.u7x00;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignedPayload;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author TSIKOR01
 * 
 */
public class M7X00ElfPayloadTest extends TestCase {

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String DIRECTORY_FILE_FILENAME = "/";

    private static final String UNSIGNED_FILENAME = "/m7x00/unsigned_elf.elf";

    private static final String SIGNED_FILENAME = "signed.bin";

    private static final String XML_ADDRESS = "load_adress";

    private GenericSignerSettings settings;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {
        settings = new M7X00SignerSettings(Activator.getResourcesPath() + UNSIGNED_FILENAME, SIGNED_FILENAME);
    }

    /**
     * Tests nonexisting file
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
            settings.setSignerSetting(ISignerSettings.KEY_LOAD_ADDRESS_XML, XML_ADDRESS);

            GenericSignedPayload payload = new GenericSignedPayload(settings);
            payload.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * Tests exception thrown if input file is directory
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
            settings.setSignerSetting(ISignerSettings.KEY_LOAD_ADDRESS_XML, XML_ADDRESS);

            GenericSignedPayload payload = new GenericSignedPayload(settings);
            payload.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * Tests if methods needed to correct handling of payload are returning correct values Fails if
     * exception of any kind occurs.
     */
    @Test
    public void testCreatePayload() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {
            File file = new File(Activator.getResourcesPath() + UNSIGNED_FILENAME);
            GenericSignedPayload payload = new GenericSignedPayload(settings);
            assertEquals(file.length(), payload.getLength());
            assertEquals(0, payload.getDestinationAddress());
            GenericCertificate cert = new GenericSignedHeader();
            payload.setCertificate(cert);
            assertEquals(cert, payload.getCertificate());
            byte[] b = new byte[] {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            payload.setSignature(b);
            assertTrue(Arrays.equals(b, payload.getSignature()));
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * Tests if there are null values handled correctly. Null values are tested inside of write
     * method, which should throw Exception, because input stream have null value.
     * 
     */
    @Test
    public void testNullValues() {
        FileChannel channel = null;
        FileOutputStream fis = null;

        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {

            fis = new FileOutputStream("signed.bin");
            channel = fis.getChannel();

            GenericSignedPayload payload = new GenericSignedPayload(settings);
            payload.setCertificate(null);
            payload.setSignature(null);
            payload.write(channel);
        } catch (Exception e1) {
            assertTrue(true);
            e1.printStackTrace();
        } finally {
            try {
                if (fis != null) {
                    fis.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

}
