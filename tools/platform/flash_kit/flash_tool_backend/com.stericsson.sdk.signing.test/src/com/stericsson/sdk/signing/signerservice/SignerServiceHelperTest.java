package com.stericsson.sdk.signing.signerservice;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Random;

import junit.framework.TestCase;

import org.apache.log4j.Logger;
import org.junit.Assert;
import org.junit.Test;

/**
 * @author emicroh
 * 
 */
public class SignerServiceHelperTest extends TestCase {

    private static final Logger log = Logger.getLogger(SignerServiceHelperTest.class.getName());

    /**
     * @throws SignerServiceException
     *             {@link SignerServiceHelper#getSecret(Logger, String)}
     * @throws IOException
     *             When IO operation fails.
     */
    @Test
    public void testGetSecret() throws SignerServiceException, IOException {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("tmp", ".sec");
            byte[] secret = new byte[64];
            Random r = new Random(System.currentTimeMillis());
            r.nextBytes(secret);

            fos = new FileOutputStream(tempFile);
            fos.write(secret);
            fos.close();
            fos = null;

            byte[] fetchedSecret = SignerServiceHelper.getSecret(log, tempFile.getAbsolutePath(),(String) null);
            assertTrue("Fetched secrets doesn't match expected one!", Arrays.equals(secret, fetchedSecret));
        } finally {
            if (tempFile != null) {
                tempFile.delete();
            }
            if (fos != null) {
                fos.close();
            }
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testSecretPathLinuxFix() throws Throwable {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("tmp", ".sec");
            byte[] secret = new byte[64];
            Random r = new Random(System.currentTimeMillis());
            r.nextBytes(secret);

            fos = new FileOutputStream(tempFile);
            fos.write(secret);
            fos.close();
            fos = null;

            System.setProperty("SIGNPACKAGEROOT", tempFile.getParent());
            byte[] fetchedSecret = SignerServiceHelper.getSecret(log, "\\\\server\\epa\\" + tempFile.getName(),(String) null);
            assertTrue(Arrays.equals(secret, fetchedSecret));
        } finally {
            System.getProperties().remove("SIGNPACKAGEROOT");
            if (tempFile != null) {
                tempFile.delete();
            }
            if (fos != null) {
                fos.close();
            }
            assertNull(System.getProperty("SIGNPACKAGEROOT"));
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testFakeSecretPath() throws Throwable {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("tmp", ".sec");
            byte[] secret = new byte[64];
            Random r = new Random(System.currentTimeMillis());
            r.nextBytes(secret);

            fos = new FileOutputStream(tempFile);
            fos.write(secret);
            fos.close();
            fos = null;

            byte[] fetchedSecret = SignerServiceHelper.getSecret(log, "fakePath",(String) null);
            Assert.assertArrayEquals("Fetched secrets doesn't match expected one!", secret, fetchedSecret);
            fail("should not get here");
        } catch (SignerServiceException sse) {
            sse.getMessage();
        } finally {
            if (tempFile != null) {
                tempFile.delete();
            }
            if (fos != null) {
                fos.close();
            }
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testUnavailableSecretPath() throws Throwable {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("tmp", ".sec");
            byte[] secret = new byte[64];
            Random r = new Random(System.currentTimeMillis());
            r.nextBytes(secret);

            fos = new FileOutputStream(tempFile);
            fos.write(secret);
            fos.close();
            fos = null;

            System.setProperty("SIGNPACKAGEROOT", tempFile.getParent());
            byte[] fetchedSecret = SignerServiceHelper.getSecret(log, "\\\\server\\epa\\fakeFile.sec",(String) null);
            assertTrue(Arrays.equals(secret, fetchedSecret));
            fail("should not get here");
        } catch (SignerServiceException sse) {
            sse.getMessage();
        } finally {
            System.getProperties().remove("SIGNPACKAGEROOT");
            if (tempFile != null) {
                tempFile.delete();
            }
            if (fos != null) {
                fos.close();
            }
            assertNull(System.getProperty("SIGNPACKAGEROOT"));
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testTranslatePathSignPackagesRootNull() throws Throwable {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("tmp", ".sec");
            byte[] secret = new byte[64];
            Random r = new Random(System.currentTimeMillis());
            r.nextBytes(secret);

            fos = new FileOutputStream(tempFile);
            fos.write(secret);
            fos.close();
            fos = null;

            byte[] fetchedSecret = SignerServiceHelper.getSecret(log, "\\\\server\\epa\\fakeFile.sec", null);
            assertTrue(Arrays.equals(secret, fetchedSecret));
            fail("should not get here");
        } catch (SignerServiceException sse) {
            sse.getMessage();
        } finally {
            System.getProperties().remove("SIGNPACKAGEROOT");
            if (tempFile != null) {
                tempFile.delete();
            }
            if (fos != null) {
                fos.close();
            }
            assertNull(System.getProperty("SIGNPACKAGEROOT"));
        }
    }
}
