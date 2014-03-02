/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.UmlExportFilter;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.util.List;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

import com.stericsson.RefMan.Toc.TocElement;

/**
 * @author qfrelun
 *
 */
public class TestUmlExport {
    /** The logger */
    private final static Logger logger = LoggerFactory
            .getLogger(TestUmlExport.class);

    /** The platform name */
    final String platformName = "U5500";

    /** A test files directory */
    final String testFilesDirName = "test//com//stericsson//RefMan//UmlExportFilter//checkinterfaces-files";

    /** A test files directory */
    final String testFilesRemoveInterfacesDirName = "test//com//stericsson//RefMan//UmlExportFilter//removeinterfaces-files";

    final String[] documentedInModel_ste_uml_export_from_ea_r1a012b1 = {
            "/dev/videoX", "blt_api", "COPS", "TEE Client API", "/dev/random",
            "/dev/crypto", "/dev/tee", "OpenWF", "/dev/display", "OpenGL ES",
            "EGL", "OpenVG", "/dev/fb", "V4L DVB", "OpenMAX IL Core", "ALSA",
            "Audio Device Manager", "OpenMAX IL Component", "V4L Radio",
            "Cell Broadcast Server", "SMS Server", "Positioning",
            "Bluetooth Subsystem (BlueZ)", "BlueZ", "Routing Controller",
            "WLAN supplicant", "/dev/hdmi", "Wi-Fi Direct Controller" };

    final String[] expectedInterfaceNames_tempDir_correct_all_pack_4_5_1 = new String[] {
            "TC35892 Controller Driver Function and API guide",
            "Keypad Driver API guide", "SDMMC Host Driver Function guide",
            "PRCMU Firmware Interface Driver API guide",
            "ST-Ericsson Connectivity Driver", "ST-Ericsson CG2900 FM Driver",
            "STMPE1601 port Expander Driver API guide",
            "STMPE2401 port Expander Driver API guide"

    };

    final String[] expectedExportedInterfaceNames_adm_cbs = new String[] {
            "Audio Device Manager", "Cell Broadcast Server" };

    final String[] expectedUndocumentedInRefMan = { "B3: PRCMU Firmware Interface Driver API guide" };

    final String[] expectedUndocumentedInModel = new String[] { "A4" };

    final String[] expectedHiddenInRefMan = { "A1: Keypad Driver API guide",
            "A2: Touch Panel Driver Function guide" };

    private static final String[] expectedFilteredInterfaceNames = { "Undefined area/Kernel space/A3: SDMMC Host Driver Function guide", };

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#UmlExport()}.
     *
     * @throws SAXException
     */
    @Test
    public void testUmlExport() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-correct-all-pack-4.5-1"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(new File(testFilesDirName, "testExport-adm+cbs.xml"));
        List<com.stericsson.RefMan.Toc.Element> interfacesFromRefMan = API
                .getInterfaces();
        assertEquals(8, interfacesFromRefMan.size());
        checkRefManInterfacesAgainstExpected(interfacesFromRefMan,
                expectedInterfaceNames_tempDir_correct_all_pack_4_5_1);
        List<Interface> interfacesFromExport = umlExport.getInterfaces();
        checkExportedInterfacesAgainstExpected(interfacesFromExport,
                expectedExportedInterfaceNames_adm_cbs);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#checkInterfaces(com.stericsson.RefMan.Toc.TocElement, java.lang.String)}
     * . This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     *
     * @throws SAXException
     */
    @Test
    public void testCheckInterfaces() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName, "tempDir-adm"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(new File(testFilesDirName, "testExport-adm.xml"),
                        API, platformName);
        int faults = umlExport.checkInterfaces();
        assertEquals(0, faults);
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        assertEquals(0, undocumentedInRefMan.size());
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        assertEquals(0, undocumentedModel.size());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#checkInterfaces(com.stericsson.RefMan.Toc.TocElement, java.lang.String)}
     * . This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     *
     * @throws SAXException
     */
    @Test
    public void testCheckInterfaces2() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-adm+cbs"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(
                        new File(testFilesDirName, "testExport-adm+cbs.xml"),
                        API, platformName);
        int faults = umlExport.checkInterfaces();
        assertEquals(0, faults);
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        assertEquals(0, undocumentedInRefMan.size());
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        assertEquals(0, undocumentedModel.size());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#checkInterfaces(com.stericsson.RefMan.Toc.TocElement, java.lang.String)}
     * . This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     *
     * @throws SAXException
     */
    @Test
    public void testCheckInterfacesOneTooManyInRefMan() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-adm+cbs"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(new File(testFilesDirName, "testExport-adm.xml"),
                        API, platformName);
        int faults = umlExport.checkInterfaces();
        assertEquals(1, faults);
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        assertEquals(0, undocumentedInRefMan.size());
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        assertEquals(1, undocumentedModel.size());
        assertTrue(undocumentedModel.get(0).equals("Cell Broadcast Server"));
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#checkInterfaces(com.stericsson.RefMan.Toc.TocElement, java.lang.String)}
     * . This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) works.
     *
     * @throws SAXException
     */
    @Test
    public void testCheckInterfacesOneTooManyInModel() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName, "tempDir-adm"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(
                        new File(testFilesDirName, "testExport-adm+cbs.xml"),
                        API, platformName);
        int faults = umlExport.checkInterfaces();
        assertEquals(1, faults);
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        assertEquals(1, undocumentedInRefMan.size());
        assertTrue(undocumentedInRefMan.get(0).equals("Cell Broadcast Server"));
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        assertEquals(0, undocumentedModel.size());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#checkInterfaces(com.stericsson.RefMan.Toc.TocElement, java.lang.String)}
     * . This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) and
     *
     * @throws SAXException
     */
    @Test
    public void testCheckInterfacesReallifeData() throws SAXException {
        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesDirName,
                        "tempDir-correct-all-pack-4.5-1"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(new File(testFilesDirName,
                        "ste-uml-export-from-ea-r1a012b1.xml"), API,
                        platformName);
        int faults = umlExport.checkInterfaces();
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        checkNamesAgainstExpected(undocumentedInRefMan,
                documentedInModel_ste_uml_export_from_ea_r1a012b1,
                "umlExport.getUndocumentedInRefMan()");
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        checkNamesAgainstExpected(undocumentedModel,
                expectedInterfaceNames_tempDir_correct_all_pack_4_5_1,
                "umlExport.getUndocumentedModel()");
        assertEquals(
                expectedInterfaceNames_tempDir_correct_all_pack_4_5_1.length
                        + documentedInModel_ste_uml_export_from_ea_r1a012b1.length,
                faults);
    }

/**
     * Test method for {@link com.stericsson.RefMan.UmlExportFilter.UmlExport#removeinterfaces().
     * This test method assumes that
     * com.stericsson.RefMan.Toc.ElementFactory#getAssembledElement(File) and
     * com.stericsson.RefMan.Toc.TocElement#removeInterfaces(java.utils.list<java.lang.String>)
     * works.
     *
     * @throws SAXException
    */
    @Test
    public void testRemoveInterfaces() throws SAXException {

        TocElement API = com.stericsson.RefMan.Toc.ElementFactory
                .getAssembledElement(new File(testFilesRemoveInterfacesDirName,
                        "tempDir"));
        UmlExport umlExport = com.stericsson.RefMan.UmlExportFilter.ElementFactory
                .getElement(new File(testFilesRemoveInterfacesDirName,
                        "ste-uml-export.xml"), API, platformName);

        logger.debug("testRemoveInterfaces()");
        int faults = umlExport.checkInterfaces();
        List<String> undocumentedInRefMan = umlExport.getUndocumentedInRefMan();
        checkNamesAgainstExpected(undocumentedInRefMan,
                expectedUndocumentedInRefMan,
                "umlExport.getUndocumentedInRefMan()");
        List<String> undocumentedModel = umlExport.getUndocumentedModel();
        checkNamesAgainstExpected(undocumentedModel,
                expectedUndocumentedInModel, "umlExport.getUndocumentedModel()");

        List<String> hidddenInRefMan = umlExport.getHiddenInRefMan();
        checkNamesAgainstExpected(hidddenInRefMan, expectedHiddenInRefMan,
                "umlExport.getHiddenInRefMan()");
        assertEquals(expectedUndocumentedInModel.length
                + expectedUndocumentedInRefMan.length, faults);

        TocElement filteredInterfaces = umlExport.removeInterfaces();
        List<String> filteredInterfaceNames = filteredInterfaces
                .getInterfaceNames();
        checkNamesAgainstExpected(filteredInterfaceNames,
                expectedFilteredInterfaceNames, "umlExport.removeInterfaces()");
    }

    /**
     * Wrapper method around checkNameAgainstExpectedNames() and
     * verifyAllExpectedNamesFound() for verifying the result of
     * TocElement.getInterfaces().
     *
     * @param interfaces
     *            List of Element as returned by TocElement.getInterfaces()
     * @param expectedInterfaceNames
     *            List of expected String's returned by getLabel() on each
     *            Element.
     */
    private void checkRefManInterfacesAgainstExpected(
            List<com.stericsson.RefMan.Toc.Element> interfaces,
            String[] expectedInterfaceNames) {
        boolean[] expectedInterfaceNamesFound = new boolean[expectedInterfaceNames.length];
        String calleeName = "API.getInterfaces()";
        for (com.stericsson.RefMan.Toc.Element element : interfaces) {
            String name = element.getLabel();
            checkNameAgainstExpectedNames(expectedInterfaceNames,
                    expectedInterfaceNamesFound, name, calleeName);
        }
        verifyAllExpectedNamesFound(expectedInterfaceNames,
                expectedInterfaceNamesFound, calleeName);
    }

    /**
     * Wrapper method around checkNameAgainstExpectedNames() and
     * verifyAllExpectedNamesFound() for verifying the result of
     * umlExport.getInterfaces().
     *
     * @param interfaces
     *            List of Interface as returned by UmlExport.getInterfaces()
     * @param expectedInterfaceNames
     *            List of expected String's returned by getName() on each
     *            Interface.
     */
    private void checkExportedInterfacesAgainstExpected(
            List<Interface> interfaces, String[] expectedInterfaceNames) {
        boolean[] expectedInterfaceNamesFound = new boolean[expectedInterfaceNames.length];
        String calleeName = "umlExport.getInterfaces()";

        for (Interface i : interfaces) {
            String name = i.getName();
            checkNameAgainstExpectedNames(expectedInterfaceNames,
                    expectedInterfaceNamesFound, name, calleeName);
        }
        verifyAllExpectedNamesFound(expectedInterfaceNames,
                expectedInterfaceNamesFound, calleeName);
    }

    /**
     * Wrapper method around checkNameAgainstExpectedNames() and
     * verifyAllExpectedNamesFound() for verifying the result anything returning
     * a List of String's.
     *
     * @param names
     *            List of Strings
     * @param expectedInterfaceNames
     *            List of expected String's
     */
    static public void checkNamesAgainstExpected(List<String> names,
            String[] expectedInterfaceNames, String calleeName) {
        boolean[] expectedInterfaceNamesFound = new boolean[expectedInterfaceNames.length];

        for (String name : names) {
            checkNameAgainstExpectedNames(expectedInterfaceNames,
                    expectedInterfaceNamesFound, name, calleeName);
        }
        verifyAllExpectedNamesFound(expectedInterfaceNames,
                expectedInterfaceNamesFound, calleeName);
    }

    /**
     * Check that the specified name is included in an array of expected names.
     * Sets expectedNamesFound[] for the found name.
     *
     * Call verifyAllExpectedNamesFound() to verify that all names has been
     * found, after repeatedly calling checkNameAgainstExpectedNames().
     *
     * @param expectedNames
     *            Array of expected names
     * @param expectedNamesFound
     *            Array of booleans for each name. True when found.
     * @param name
     *            Name to check
     * @param calleeName
     *            Name of function under test.
     */
    private static void checkNameAgainstExpectedNames(String[] expectedNames,
            boolean[] expectedNamesFound, String name, String calleeName) {
        boolean found = false;
        int index = 0;
        for (String expectedName : expectedNames) {
            if (name.compareTo(expectedName) == 0) {
                assertFalse(calleeName + " returned interface name '" + name
                        + "' twice", expectedNamesFound[index]);
                found = true;
                expectedNamesFound[index] = true;
                break;
            }
            index++;
        }

        assertTrue(calleeName + " returned unexpected interface name '" + name
                + "'", found);
    }

    /**
     * Checks that the array of booleans is true for all names.
     *
     * @param expectedInterfaceNames
     *            Array of expected names.
     * @param expectedInterfaceNamesFound
     *            Array booleans for each name.
     * @param calleeName
     *            Name of function under test.
     */
    private static void verifyAllExpectedNamesFound(
            String[] expectedInterfaceNames,
            boolean[] expectedInterfaceNamesFound, String calleeName) {
        int index = 0;
        for (String expectedName : expectedInterfaceNames) {
            assertTrue(calleeName + " did not return expected interface name '"
                    + expectedName + "'", expectedInterfaceNamesFound[index]);
            index++;
        }
    }

}
