package com.stericsson.sdk.signing.cops;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * @author xtomlju
 */
public class COPSSignedDataTest extends TestCase{

    private static final String SIGNED_DATA_FILENAME = "/a2/signed_software.ssw";

    /**
     * 
     */
    @Test
    public void testCOPSSignedDataReadableByteChannel() {

        FileChannel fileChannel = null;
        FileInputStream fis = null;
        COPSSignedData signedData = null;

        try {
            fis = new FileInputStream(Activator.getResourcesPath() + SIGNED_DATA_FILENAME);
            fileChannel = fis.getChannel();
            signedData = new COPSSignedData(fileChannel);

            assertTrue(signedData != null);
            assertTrue(signedData.getControlFields() != null);
            assertTrue(signedData.getCertificateList() != null);
            assertTrue(signedData.getHashList() != null);
            assertTrue(signedData.getSignature() != null);
            assertTrue(signedData.getPadding() != null);

        } catch (IOException e) {
            fail(e.getMessage());
        } finally {
            if (fileChannel != null) {
                try {
                    fileChannel.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
