package com.stericsson.sdk.signing.creators;

import java.io.File;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.data.ISSWCertificateData;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.data.KeyData;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.test.Activator;

/**
 * Test of ISSW creation
 * 
 * @author TSIKOR01
 * 
 */
public class ISSWCertificateCreatorTest extends TestCase {

    ISSWCertificateCreator creator;

    ISSWCertificateData testISSWCertificateData;

    File output;

    private String outputPath = Activator.getResourcesPath() + File.separator + "testCertificate.bin";

    private static final String KEY_PATH = Activator.getResourcesPath() + "/common_creators/FAKE_KEY_pub.pem";

    /**
     * Setup of environment before the test
     * 
     */
    @Before
    public void setUp() {
        creator = new ISSWCertificateCreator();
        output = new File("testISSWCertificate");

        testISSWCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                createTestKeyData(), createTestConstraints());
    }

    private HashMap<GenericSoftwareType, Integer> createTestConstraints() {
        HashMap<GenericSoftwareType, Integer> constrains = new HashMap<GenericSoftwareType, Integer>();
        constrains.put(GenericSoftwareType.XLOADER, 1);
        return constrains;
    }

    private List<Short> createTestMoDelIDs() {
        List<Short> modeIDS = new ArrayList<Short>();
        short id = 0x11;
        short anotherID = 0x12;

        modeIDS.add(id);
        modeIDS.add(anotherID);

        return modeIDS;
    }

    private List<KeyData> createTestKeyData() {
        List<KeyData> keys = new ArrayList<KeyData>();
        keys.add(new KeyData(KEY_PATH, SignatureType.ECDSA));
        return keys;
    }

    /**
     * Tests setter injection of ISSW Certificate data object
     * 
     */
    @Test
    public void testSetterInjectionForISSWCertificateObject() {
        List<KeyData> keyData = createTestKeyData();
        List<Short> modeIDs = createTestMoDelIDs();
        KeyData rootKey = createRootKey();
        HashMap<GenericSoftwareType, Integer> signingConstraints = createTestConstraints();

        testISSWCertificateData.setKey(keyData);
        testISSWCertificateData.setModelID(modeIDs);
        testISSWCertificateData.setPath(outputPath);
        testISSWCertificateData.setRootKey(rootKey);

        testISSWCertificateData.setSigningConstraints(null);
        Assert.assertNotNull(testISSWCertificateData.getSigningConstraints());
        testISSWCertificateData.setSigningConstraints(signingConstraints);

        testISSWCertificateData.setType(ISSWCertificateType.TYPE1);

        Assert.assertEquals(keyData, testISSWCertificateData.getKey());
        Assert.assertEquals(outputPath, testISSWCertificateData.getPath());
        Assert.assertEquals(modeIDs, testISSWCertificateData.getModelID());
        Assert.assertEquals(rootKey, testISSWCertificateData.getRootKey());
        Assert.assertEquals(signingConstraints, testISSWCertificateData.getSigningConstraints());
        Assert.assertEquals(ISSWCertificateType.TYPE1, testISSWCertificateData.getType());

        testISSWCertificateData.setModelID(null);
        Assert.assertNotNull(testISSWCertificateData.getModelID());
    }

    private KeyData createRootKey() {
        return new KeyData(KEY_PATH, SignatureType.DSA);
    }

    /**
     * Tests setter injection of ISSW Certificate data object
     * 
     */
    @Test
    public void testSetterInjectionForKeyObject() {
        KeyData testKeyData = new KeyData("path", SignatureType.DSA);
        String changedPath = "changedPath";

        testKeyData.setPath(changedPath);
        testKeyData.setType(SignatureType.ECDSA);

        Assert.assertEquals(changedPath, testKeyData.getPath());
        Assert.assertEquals(SignatureType.ECDSA, testKeyData.getType());
    }

    /**
     * Tests setter injection of ISSW Certificate data object
     * 
     */
    @Test
    public void testSetterInjectionRootKey() {
        KeyData testRootKeyData = new KeyData("path", SignatureType.DSA);
        String testRootKeyDataPath = "changedPath";

        testRootKeyData.setPath(testRootKeyDataPath);
        testRootKeyData.setType(SignatureType.ECDSA);

        Assert.assertEquals(testRootKeyDataPath, testRootKeyData.getPath());
        Assert.assertEquals(SignatureType.ECDSA, testRootKeyData.getType());
    }

    /**
     * Test of create method, after creation test is made if files exist.
     * 
     * @throws Exception
     *             Exception.
     * @throws NoSuchAlgorithmException
     *             Exception when RSA algorithm is not available
     */
    @Test
    public void testCreate() throws NoSuchAlgorithmException, Exception {
        creator.create(testISSWCertificateData);
        Assert.assertTrue(output.exists());
    }

    /**
     * 
     * @throws Exception
     */
    @Test
    public void testCreateException() {
        ISSWCertificateData isswCertificateData = null;

        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     * @throws Exception
     */
    @Test
    public void testInvalidRootKey() {
        ISSWCertificateData isswCertificateData = null;

        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, null, createTestMoDelIDs(),
                createTestKeyData(), createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     * @throws Exception
     */
    @Test
    public void testInvalidKeys() {
        ISSWCertificateData isswCertificateData = null;

        List<KeyData> keys = new ArrayList<KeyData>();
        KeyData keyData = null;
        keys.add(keyData);
        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                keys, createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     * @throws Exception
     */
    @Test
    public void testInvalidNumberOfKeys() {
        ISSWCertificateData isswCertificateData = null;

        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                null, createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        List<KeyData> keys = new ArrayList<KeyData>();
        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                keys, createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        for (int i = 0; i < 17; i++) {
            keys.add(new KeyData("path" + i, SignatureType.RSASSA_PKCS_V1_5));
        }
        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                keys, createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        for (int i = 0; i < 48; i++) {
            keys.add(new KeyData("path" + i, SignatureType.RSASSA_PKCS_V1_5));
        }
        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE2, createRootKey(), createTestMoDelIDs(),
                keys, createTestConstraints());
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * Test of create method, after creation test is made if files exist with type2 certificate.
     * 
     * @throws Exception
     *             Exception.
     * @throws NoSuchAlgorithmException
     *             Exception when RSA algorithm is not available
     */
    @Test
    public void testCreateType2() throws NoSuchAlgorithmException, Exception {
        testISSWCertificateData.setType(ISSWCertificateType.TYPE2);
        testISSWCertificateData.setReserved(null);
        Assert.assertNotNull(testISSWCertificateData.getReserved());
        byte[] reserved = new byte[] {
            0x13, 0x5, 0x13};
        testISSWCertificateData.setReserved(reserved);
        Assert.assertArrayEquals(reserved, testISSWCertificateData.getReserved());
        creator.create(testISSWCertificateData);
        Assert.assertTrue(output.exists());
    }

    /**
     * 
     */
    @Test
    public void testInvalidReserved() {
        testISSWCertificateData.setType(ISSWCertificateType.TYPE2);
        testISSWCertificateData.setReserved(new byte[] {
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0});
        try {
            creator.create(testISSWCertificateData);
            fail("Should not get here." + testISSWCertificateData.getReserved().length);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * 
     * @throws Exception
     */
    @Test
    public void testInvalidSoftwareType() {
        ISSWCertificateData isswCertificateData = null;

        HashMap<GenericSoftwareType, Integer> signingConstraints = new HashMap<GenericSoftwareType, Integer>();
        signingConstraints.put(GenericSoftwareType.USTA, 1);

        isswCertificateData =
            new ISSWCertificateData(output.getName(), ISSWCertificateType.TYPE1, createRootKey(), createTestMoDelIDs(),
                createTestKeyData(), signingConstraints);
        try {
            creator.create(isswCertificateData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * Cleans up environment after test
     * 
     */
    @After
    public void tearDown() {
        if (output.exists()) {
            output.delete();
        }
    }

}
