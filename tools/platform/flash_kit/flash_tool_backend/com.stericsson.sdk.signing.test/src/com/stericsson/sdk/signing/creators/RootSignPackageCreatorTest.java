package com.stericsson.sdk.signing.creators;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.signing.data.RootSignPackageData;
import com.stericsson.sdk.signing.data.SubpackageData;
import com.stericsson.sdk.signing.test.Activator;

/**
 * Class testing functionality of root sign package creator
 * 
 * @author TSIKOR01
 * 
 */
public class RootSignPackageCreatorTest extends TestCase {

    private static final String ALIAS = "Test_package";

    private static final String DESCRIPTION = "PackageForTesting";

    private static final String ISSW_PATH = Activator.getResourcesPath() + "/common_creators/FAKE_CERTIFICATE.bin";

    private static final String PATH = Activator.getResourcesPath() + File.separator;

    private static final String REVISION = "NoRevision";

    private static final String ROOT_PACKAGE_SUFFIX = "_ROOT.pkg";

    private static final String GENERIC_PACKAGE_SUFFIX = "_GENERIC_1.pkg";

    RootSignPackageCreator creator;

    RootSignPackageData testingRootSignPackageData;

    File fakeISSW;

    /**
     * Sets up environment before tests
     * 
     */
    @Before
    public void setUp() {
        creator = new RootSignPackageCreator();
        testingRootSignPackageData = new RootSignPackageData(PATH, ISSW_PATH, ALIAS, REVISION, DESCRIPTION);
    }

    /**
     * Test of create method, after creation test is made if files exist.
     * 
     * @throws Exception
     *             Exception if file was
     */
    @Test
    public void testCreate() throws Exception {
        List<SubpackageData> subpackages = new ArrayList<SubpackageData>();
        SubpackageData subpackageData = new SubpackageData(-1, "", -1);
        assertEquals(-1, subpackageData.getIndex());
        assertEquals("", subpackageData.getPath());
        assertEquals(-1, subpackageData.getSpare());
        subpackageData.setIndex(1);
        subpackageData.setPath(PATH);
        subpackageData.setSpare(0xFFFFFFFF);
        assertEquals(1, subpackageData.getIndex());
        assertEquals(PATH, subpackageData.getPath());
        assertEquals(0xFFFFFFFF, subpackageData.getSpare());
        subpackages.add(subpackageData);

        testingRootSignPackageData.setSubpackages(subpackages);
        creator.create(testingRootSignPackageData);
        Assert.assertTrue(new File(PATH + File.separator + ALIAS + ROOT_PACKAGE_SUFFIX).exists());
    }

    /**
     * 
     */
    @Test
    public void testCreateException() {
        RootSignPackageData rootSignPackageData = null;

        try {
            creator.create(rootSignPackageData);
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
        testingRootSignPackageData.setAlias(ALIAS);
        testingRootSignPackageData.setDescription(DESCRIPTION);
        testingRootSignPackageData.setIsswCertificate(ISSW_PATH);
        testingRootSignPackageData.setPath(PATH);
        testingRootSignPackageData.setRevision(REVISION);
        testingRootSignPackageData.setSubpackages(null);

        Assert.assertEquals(ALIAS, testingRootSignPackageData.getAlias());
        Assert.assertEquals(DESCRIPTION, testingRootSignPackageData.getDescription());
        Assert.assertEquals(ISSW_PATH, testingRootSignPackageData.getIsswCertificate());
        Assert.assertEquals(PATH, testingRootSignPackageData.getPath());
        Assert.assertEquals(REVISION, testingRootSignPackageData.getRevision());
        Assert.assertTrue(testingRootSignPackageData.getSubpackages().isEmpty());
    }

    /**
     * Cleans up test environment after tests execution
     * 
     * @throws Exception
     *             Exception if file cannot be deleted
     */
    @After
    public void tearDown() throws Exception {
        File rootSignPackage = new File(PATH + File.separator + ALIAS + ROOT_PACKAGE_SUFFIX);
        if (rootSignPackage.exists()) {
            rootSignPackage.delete();
        }

        File genericSignPackage = new File(PATH + File.separator + ALIAS + GENERIC_PACKAGE_SUFFIX);
        if (genericSignPackage.exists()) {
            genericSignPackage.delete();
        }
    }
}
