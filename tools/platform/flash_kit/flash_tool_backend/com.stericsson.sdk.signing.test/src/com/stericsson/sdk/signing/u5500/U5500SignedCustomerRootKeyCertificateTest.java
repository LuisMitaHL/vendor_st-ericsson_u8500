package com.stericsson.sdk.signing.u5500;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSignedCustomerRootKeyCertificate;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500SignedCustomerRootKeyCertificateTest extends TestCase {

    private static final String NON_EXISITNG_FILE_FILENAME = "fakefile.fake";

    private static final String DIRECTORY_FILE_FILENAME = "/";

    private static final String UNSIGNED_PAYLOAD_FILENAME = "/u5500/unsigned_ProductRootKey.priv.pem";

    private static final String SIGNED_PAYLOAD_FILENAME = "signed_crkc.bin";

    private GenericSignerSettings settings;

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
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE, NON_EXISITNG_FILE_FILENAME);
            GenericSignedCustomerRootKeyCertificate payload = new GenericSignedCustomerRootKeyCertificate(settings);
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
        } catch (Exception e) {
            fail(e.getMessage());
        }
        try {
            settings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE, Activator.getResourcesPath()
                + DIRECTORY_FILE_FILENAME);
            GenericSignedCustomerRootKeyCertificate payload = new GenericSignedCustomerRootKeyCertificate(settings);
            payload.getClass();
            fail("should not get here");
        } catch (SignerException e) {
            e.getMessage();
        }
    }
}
