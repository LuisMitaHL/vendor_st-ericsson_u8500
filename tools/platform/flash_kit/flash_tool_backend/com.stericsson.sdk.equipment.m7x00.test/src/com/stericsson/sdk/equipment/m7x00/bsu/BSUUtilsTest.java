package com.stericsson.sdk.equipment.m7x00.bsu;

import java.io.File;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils.BSUResult;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.equipment.m7x00.test.TestFragmentActivator;

/**
 * Test class for BSUUtils.
 * 
 * @author xadazim
 * 
 */
public class BSUUtilsTest extends TestCase {

    private static final String LOAD_MODULE = "test_load_module.elf";

    private static final String ENCODING = "UTF-8";

    /**
     * 
     * {@inheritDoc}
     */
    @Before
    public void setUp() {
        // System.setProperty("os.name", "mac");
    }

    /**
     * Tests getLoadModules method. LoaderCommunicationMock can pretend LCD
     * FS_READ_LOAD_MODULE_MANIFESTS command. Two load module paths are defined in the mock:
     * ":/boot/" and ":/bin/".
     * 
     * @throws Exception
     *             in case of error
     */
    @Test
    public void testGetLoadModules() throws Exception {

        M7x00Equipment equipment = null;
        // get load modules from all paths
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        Collection<LoadModuleDescription> loadModules = BSUUtils.getLoadModules(equipment);
        Map<String, LoadModuleDescription> nameToLoadModule = new HashMap<String, LoadModuleDescription>();
        for (LoadModuleDescription ld : loadModules) {
            nameToLoadModule.put(ld.getModuleName(), ld);
        }

        assertEquals(5, loadModules.size());
        assertTrue(nameToLoadModule.containsKey("crh1090280_ACCESS_DEBUG_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090280_APPLICATION_DEBUG_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090285_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090284_NS_M570_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090290_LIP_DPU_PC_CARD.elf"));

        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getName(),
            "crh1090280_ACCESS_DEBUG_PC_CARD.elf");
        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getVersion(), "1.0.0");
        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getType(), "psu");

        assertEquals(nameToLoadModule.get("crh1090290_LIP_DPU_PC_CARD.elf").getManifest().getType(), "dpu");

        // get load modules from specific path only (:/boot/)
        loadModules = BSUUtils.getLoadModules(equipment, BSUUtils.BSUPath.PSU.getPath());
        nameToLoadModule = new HashMap<String, LoadModuleDescription>();
        for (LoadModuleDescription ld : loadModules) {
            nameToLoadModule.put(ld.getModuleName(), ld);
        }

        assertEquals(4, loadModules.size());
        assertTrue(nameToLoadModule.containsKey("crh1090280_ACCESS_DEBUG_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090280_APPLICATION_DEBUG_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090285_PC_CARD.elf"));
        assertTrue(nameToLoadModule.containsKey("crh1090284_NS_M570_PC_CARD.elf"));

        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getName(),
            "crh1090280_ACCESS_DEBUG_PC_CARD.elf");
        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getVersion(), "1.0.0");
        assertEquals(nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf").getManifest().getType(), "psu");

        LoadModuleDescription descr = nameToLoadModule.get("crh1090280_ACCESS_DEBUG_PC_CARD.elf");
        assertTrue(descr.toString().contains("Name"));
        assertTrue(descr.toString().contains("Manifest"));
        assertNotNull(descr.getManifest());
        assertTrue(descr.getManifest().toString().contains("Name"));
        assertTrue(descr.getManifest().toString().contains("Version"));
        assertNotNull(descr.getManifest().getRaw());
        assertTrue(descr.getManifest().getRaw().length > 0);

        try {
            new LoadModuleManifest(new String("<?xml version=\"1.0\" encoding=\"UTF-8\"?> "
                + "<bsu  id=\"abc\" version=\"1.0.0\"> </bsu>").getBytes(ENCODING));
            fail();
        } catch (Exception e) {
            assertTrue(e.getMessage().contains("parsing error"));
        }

        try {
            new LoadModuleManifest(new String("<?xml version=\"1.0\" encoding=\"UTF-8\"?> "
                + "<bsu  id=\"abc\"> </bsu>").getBytes(ENCODING));
            fail();
        } catch (Exception e) {
            assertTrue(e.getMessage().contains("parsing error"));
        }

        try {
            new LoadModuleManifest(new String("<?xml version=\"1.0\" encoding=\"UTF-8\"?> " + "<bsu> </bsu>")
                .getBytes(ENCODING));
            fail();
        } catch (Exception e) {
            assertTrue(e.getMessage().contains("parsing error"));
        }
    }

    /**
     * Tests checkCompatibility method. LoaderCommunicationMock can pretend LCD
     * FS_READ_LOAD_MODULE_MANIFESTS command. Two load module paths are defined in the mock:
     * ":/boot/" and ":/bin/".
     * 
     * @throws Exception
     *             in case of error
     */
    @Test
    public void testCheckCompatibility() throws Exception {
        M7x00Equipment equipment = null;

        // get load modules from all paths
        equipment = new M7x00Equipment(TestFragmentActivator.getPort(), null);
        File loadModule = new File(TestFragmentActivator.getResourcesPath(), LOAD_MODULE);
        BSUResult result = BSUUtils.checkCompatibility(equipment, loadModule);
        // watch out, the exact outuput from BsuHostManager.exe might change a little
        // when we upgrade to some recent version
        assertTrue(result.getText().contains("File compatibility.txt result: Compatibility check failed."));
        assertEquals(0, result.getExitCode());

        // test with broken input file
        try {
            File brokenLoadModule = new File(TestFragmentActivator.getResourcesPath(), LOAD_MODULE + "non_existing");
            BSUUtils.checkCompatibility(equipment, brokenLoadModule);
            fail("BSUUtils.checkCompatibility should have thrown an exception");
        } catch (Exception e) {
            assertTrue(e.getMessage().contains("could not be read"));
        }
    }

    /**
     * Tests BSUResult class.
     */
    @Test
    public void testBSUResult() {
        BSUResult res = new BSUResult("text", 3);
        assertEquals("text", res.getText());
        assertEquals(3, res.getExitCode());
    }

}
