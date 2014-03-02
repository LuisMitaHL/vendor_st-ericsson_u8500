package com.stericsson.sdk.signing.tvp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.WritableByteChannel;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.test.Activator;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;

/**
 * 
 * @author xolabju
 * 
 */
public class SignedTVPLicenseTest extends TestCase {

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String DIRECTORY_FILE_FILENAME = ("/");

    private static final String UNSIGNED_PAYLOAD_FILENAME = "/tvp/unsigned_license.bin";

    private static final String SIGNED_PAYLOAD_FILENAME = "signed_license.bin";

    private U5500SignerSettings settings;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {
        settings =
            new U5500SignerSettings(Activator.getResourcesPath() + UNSIGNED_PAYLOAD_FILENAME, SIGNED_PAYLOAD_FILENAME);
    }

    /**
     * 
     */
    @Test
    public void testNonExistingFile() {
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE,
                NON_EXISITNG_FILE_FILENAME);
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            payload.getClass();
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
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE,
                DIRECTORY_FILE_FILENAME);
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            payload.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * 
     */
    @Test
    public void testCreatePayload() {
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            File file = new File(Activator.getResourcesPath() + UNSIGNED_PAYLOAD_FILENAME);
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            assertEquals(file.length(), payload.getLength());
            assertEquals(0, payload.getDestinationAddress());
            byte[] b = new byte[] {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            payload.setSignature(b);
            assertTrue(Arrays.equals(b, payload.getSignature()));
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetHashValue(){
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            byte[] hashValue = payload.getHashValue(2);
            assertNotNull(hashValue);
        } catch (SignerException e) {
            fail(e.getMessage());
        }
    }

    /**
     * 
     */
    @Test
    public void testGetHashValueIOException(){
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE,
                    NON_EXISITNG_FILE_FILENAME);
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            byte[] hashValue = payload.getHashValue(2);
            assertNotNull(hashValue);
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }

    /**
     * 
     */
    @Test
    public void testWrite(){
        WritableByteChannel channel = null;
        FileOutputStream fis = null;
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            fis = new FileOutputStream(SIGNED_PAYLOAD_FILENAME);
            channel = fis.getChannel();
            byte[] data = new byte[]{1, 2, 3 , 4};
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            payload.setSignature(data);
            payload.write(channel);
        } catch (SignerException e) {
            fail(e.getMessage());
        }catch (IOException ioe) {
            fail(ioe.getMessage());
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    e.getMessage();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.getMessage();
                }
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testWriteNoSignatureContent(){
        WritableByteChannel channel = null;
        FileOutputStream fis = null;
        try {
            setUp();
        } catch (Exception e1) {
            fail(e1.getMessage());
        }
        try {
            fis = new FileOutputStream(SIGNED_PAYLOAD_FILENAME);
            channel = fis.getChannel();
            SignedTVPLicense payload = new SignedTVPLicense(settings);
            payload.write(channel);
        } catch (SignerException e) {
            fail(e.getMessage());
        }catch (IOException ioe) {
            fail(ioe.getMessage());
        }finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    e.getMessage();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.getMessage();
                }
            }
        }
    }
}
