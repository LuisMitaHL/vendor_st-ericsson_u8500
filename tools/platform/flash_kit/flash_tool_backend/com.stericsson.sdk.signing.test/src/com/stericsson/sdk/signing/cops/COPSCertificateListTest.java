package com.stericsson.sdk.signing.cops;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * @author xtomlju
 */
public class COPSCertificateListTest extends TestCase {

    private static final String CERTIFICATE_LIST_FILENAME = "/a2/CertList.bin";

    private static final int SUBJECT_CID = 19;

    private static final byte[] KEY_USAGE = new byte[] {
        (byte) 0x01, (byte) 0xE3};

    private static final int NUMBER_OF_CERTIFICATES = 1;

    private static final int MODULUS_BIT_LENGTH = 1024;

    byte[] certificateListData;

    /**
     * @throws Exception
     *             TBD
     */
    public void setUp() throws Exception {

        FileChannel channel = null;
        FileInputStream fis = null;
        File certificateListDataFile = new File(Activator.getResourcesPath() + CERTIFICATE_LIST_FILENAME);

        try {
            certificateListData = new byte[(int) certificateListDataFile.length()];
            fis = new FileInputStream(new File(Activator.getResourcesPath() + CERTIFICATE_LIST_FILENAME));
            channel = fis.getChannel();
            channel.read(ByteBuffer.wrap(certificateListData));
        } catch (IOException e) {
            fail(e.getMessage());
        } finally {
            if (channel != null) {
                try {
                    channel.close();
                } catch (IOException e) {
                    fail(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    fail(e.getMessage());
                }
            }
        }
    }

    /**
     * 
     */
    @Test
    public void testGetBytes() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertEquals(certificateList.getBytes(), certificateListData);
    }

    /**
     * 
     */
    @Test
    public void testGetProtectionTypeInt() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertTrue(COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_1 == certificateList
            .getProtectionType(COPSControlFields.HID_BABE0200));
        assertTrue(COPSControlFields.PROT_TYPE_RSA_1024_SIG_WITH_SHA_256 == certificateList
            .getProtectionType(COPSControlFields.HID_BABE0300));
    }

    /**
     * 
     */
    @Test
    public void testGetSubjectCID() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertEquals(certificateList.getSubjectCID(), SUBJECT_CID);
    }

    /**
     * 
     */
    @Test
    public void testGetKeyUsage() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertTrue(Arrays.equals(certificateList.getKeyUsage(), KEY_USAGE));
    }

    /**
     * 
     */
    @Test
    public void testGetNumberOfCertificates() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertEquals(certificateList.getNumberOfCertificates(), NUMBER_OF_CERTIFICATES);
    }

    /**
     * 
     */
    @Test
    public void testGetLength() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertEquals(certificateList.getLength(), certificateListData.length);
    }

    /**
     * 
     */
    @Test
    public void testGetModulusBitLength() {
        try {
            setUp();
        } catch (Exception e) {
            fail(e.getMessage());
        }
        COPSCertificateList certificateList = new COPSCertificateList(certificateListData);
        assertEquals(certificateList.getModulusBitLength(), MODULUS_BIT_LENGTH);
    }

}
