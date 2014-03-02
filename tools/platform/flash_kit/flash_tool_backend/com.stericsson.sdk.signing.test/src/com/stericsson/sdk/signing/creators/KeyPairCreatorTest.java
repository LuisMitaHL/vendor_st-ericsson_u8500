package com.stericsson.sdk.signing.creators;

import java.io.File;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.data.KeyPairData;

/**
 * Test for key pair class
 * 
 * @author TSIKOR01
 * 
 */
public class KeyPairCreatorTest extends TestCase {

    private KeyPairData testPair;

    private KeyPairCreator creator;

    private File privateKey;

    private File publicKey;

    int keyLength = 1024;

    private String privateKeyName = "privateKey.pem";

    private String publicKeyName = "publicKey.pem";

    /**
     * Sets up key pair data class
     * 
     * 
     * @throws Exception
     *             when there is impossible to create key pair on the disk
     */
    @Before
    public void setUp() throws Exception {
        creator = new KeyPairCreator();
        privateKey = new File(privateKeyName);
        publicKey = new File(publicKeyName);
        testPair = new KeyPairData(privateKey.getName(), publicKey.getName(), keyLength);
    }

    /**
     * Test of create method, after creation test is made if files exist.
     * 
     * @throws IOException
     *             Exception when file is not accessible
     * @throws NoSuchAlgorithmException
     *             Exception when RSA algorithm is not available
     */
    @Test
    public void testCreate() throws NoSuchAlgorithmException, IOException {
        creator.create(testPair);

        Assert.assertTrue(privateKey.exists());
        Assert.assertNotNull(publicKey.exists());
    }

    /**
     * 
     */
    @Test
    public void testCreateException() {
        KeyPairData keyPairData = null;

        try {
            creator.create(keyPairData);
            fail("Should not get here.");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * Tests setter injection of used data access objects
     * 
     */
    @Test
    public void testSetterInjection() {
        testPair.setKeySize(keyLength);
        testPair.setPrivateKey(privateKeyName);
        testPair.setPublicKey(publicKeyName);

        Assert.assertEquals(keyLength, testPair.getKeySize());
        Assert.assertEquals(privateKeyName, testPair.getPrivateKey());
        Assert.assertEquals(publicKeyName, testPair.getPublicKey());
    }

    /**
     * Removes created files after test completion if they exist
     * 
     * @throws Exception
     *             if file cannot be deleted
     */
    @After
    public void tearDown() throws Exception {
        if (privateKey.exists()) {
            privateKey.delete();
        }
        if (publicKey.exists()) {
            publicKey.delete();
        }
    }

}
