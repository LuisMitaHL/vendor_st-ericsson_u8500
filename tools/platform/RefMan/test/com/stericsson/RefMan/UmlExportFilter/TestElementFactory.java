/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.xml.sax.SAXException;

/**
 * This class is responsible for testing that a <code>UmlExport</code> can be
 * correctly built from the example files.
 *
 * @author Fredrik Lundström
 *
 */
public class TestElementFactory {

    /**
     * The root element of the tree with correct structure for
     * export-correct-simple.xml
     */
    UmlExport correct_simple_file;

    /**
     * The root element of the tree with a different structure for
     * export-correct-simple.xml
     */
    UmlExport correct_simple_file_difference;

    /**
     * The root element of the tree with correct structure for
     * export-correct-simple-nomandatorystuff.xml
     */
    UmlExport correct_simple_nomandatory_file;

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(TestElementFactory.class);

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        createSimpleFileRepresentation();
        createSimpleFileRepresentation_Different();
        createNomandatorystuffFileRepresentation();
    }

    private void createSimpleFileRepresentation() {
        correct_simple_file = new UmlExport();

        Interface i = new Interface();
        i.setName("Audio Device Manager");
        i.setPackage("Audio");
        i.setGuid("{A4A2DBA0-6111-4660-9037-E207FF7BA532}");
        i.setClassification("Standard");

        Platform p1 = new Platform();
        p1.setName("U5500");
        p1.setInterfaceInternalOnly("yes");
        i.addPlatform(p1);

        Platform p2 = new Platform();
        p2.setName("U8500");
        p2.setInterfaceInternalOnly("no");
        i.addPlatform(p2);

        correct_simple_file.addInterface(i);
    }

    private void createNomandatorystuffFileRepresentation() {
        correct_simple_nomandatory_file = new UmlExport();

        Interface i = new Interface();
        i.setName("Audio Device Manager");

        correct_simple_nomandatory_file.addInterface(i);
    }

    private void createSimpleFileRepresentation_Different() {
        correct_simple_file_difference = new UmlExport();

        Interface i = new Interface();
        i.setName("Audio Device Manager");
        i.setPackage("Audio");
        i.setGuid("{A4A2DBA0-6111-4660-9037-E207FF7BA532}");
        i.setClassification("Standard");

        Platform p1 = new Platform();
        p1.setName("U5500");
        p1.setInterfaceInternalOnly("yes");
        i.addPlatform(p1);

        Platform p2 = new Platform();
        p2.setName("U8500");
        p2.setInterfaceInternalOnly("yes"); // It only differs by this element!
        i.addPlatform(p2);

        correct_simple_file_difference.addInterface(i);
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using the ste-uml-export-correct-simple.xml file.
     *
     * @throws SAXException
     */
    @Test
    public void testGetElementSimple() throws SAXException {

        boolean passed = false;
        UmlExport elm = ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-correct-simple.xml"));

        passed = checkElements(elm, correct_simple_file);
        assertTrue(passed);

        // This is just to check the checkElements function
        passed = checkElements(elm, correct_simple_file_difference);
        assertFalse(passed);
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using the ste-uml-export-correct-pack4.xml file. This is only a
     * minor test, testing the presence of the 6 interface names, that each
     * interface is Proprietary and contains a U5500 platform. The PRCMU shall
     * be internal, the rest shall be public.
     *
     * @throws SAXException
     */
    @Test
    public void testGetElementPack4() throws SAXException {
        String names[] = new String[] { "Keypad Driver API guide",
                "Touch Panel Driver Function guide",
                "SDMMC Host Driver Function guide",
                "TC35892 Controller Driver Functions and API guide",
                "Mentor IP USB driver Function guide",
                "PRCMU Firmware Interface Driver API guide" };

        UmlExport elm = ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-correct-pack4.xml"));

        List<Interface> interfaces = elm.getInterfaces();
        assertTrue(interfaces.size() == names.length);
        for (Interface i : interfaces) {
            String name = i.getName();
            boolean found = false;
            for (int j = 0; j < names.length; j++) {
                if (name.compareTo(names[j]) == 0) {
                    found = true;
                    break;
                }
            }
            assertTrue(found);

            String guid = i.getGuid();
            assertTrue(guid.length() > 0);

            String classification = i.getClassification();
            assertTrue(classification.compareTo("Proprietary") == 0);
            assertTrue(i.isProprietary());
            assertFalse(i.isProprietaryExtension());
            assertFalse(i.isStandard());

            String packageName = i.getPackage();
            assertTrue(packageName.length() > 0);

            List<Platform> platforms = i.getPlatforms();
            assertTrue(platforms.size() == 1);
            Platform p = platforms.get(0);
            assertTrue(p.getName().compareTo("U5500") == 0);
            if (name.compareTo("PRCMU Firmware Interface Driver API guide") == 0) {
                assertTrue(p.getInterfaceInternalOnly().compareTo("yes") == 0);
                assertTrue(p.isInterfaceInternalOnly());
            } else {
                assertTrue(p.getInterfaceInternalOnly().compareTo("no") == 0);
                assertFalse(p.isInterfaceInternalOnly());
            }

        }
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using the ste-uml-export-correct-model-2010-02-22.xml file. This
     * is only a minor test, testing + The presence of 28 interfaces + Each
     * interface must be used by both U5500 and U8500 platforms + All interfaces
     * are public.
     *
     * @throws SAXException
     */
    @Test
    public void testGetElementModel2010_02_22() throws SAXException {

        UmlExport elm = ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-correct-model-2010-02-22.xml"));

        simplecheckforplatforms(elm, 28, new String[] { "U5500", "U8500" });
    }

    /**
     * Check that the export elm contins the specified number of interfaces, and
     * that each interface contains the specified platforms, all being
     * non-internal-only.
     *
     * @param elm
     *            The element.
     * @param expectedInterfaces
     *            The number of expected interfaces.
     * @param expectedPlatforms
     *            The platforms to expect for each interface.
     */
    private void simplecheckforplatforms(UmlExport elm, int expectedInterfaces,
            String expectedPlatforms[]) {
        List<Interface> interfaces = elm.getInterfaces();
        assertTrue(interfaces.size() == expectedInterfaces);
        for (Interface i : interfaces) {

            List<Platform> platforms = i.getPlatforms();
            int found = 0;

            for (Platform p : platforms) {
                String name = p.getName();
                for (String expectedP : expectedPlatforms) {
                    if (name.compareTo(expectedP) == 0) {
                        found++;
                    }
                }
                assertTrue(p.getInterfaceInternalOnly().compareTo("no") == 0);
                assertFalse(p.isInterfaceInternalOnly());
            }
            assertTrue(found == expectedPlatforms.length);

            assertTrue(platforms.size() == expectedPlatforms.length);
        }
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using the ste-uml-export-correct-simple-nomandatorystuff.xml
     * file.
     *
     * @throws SAXException
     */
    @Test
    public void testGetElementNoMandatoryStuff() throws SAXException {

        boolean passed = false;
        UmlExport elm = ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-correct-simple-nomandatorystuff.xml"));

        passed = checkElements(elm, correct_simple_nomandatory_file);
        assertTrue(passed);
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using a file that lacks &lt;platforms&gt; tags.
     *
     * @throws SAXException
     *
     * @throws SAXException
     */
    @Test(expected = SAXException.class)
    public void testGetElementNoPlatforms() throws SAXException {
        ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-notCorrect-noplatforms.xml"));
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using a file that contains no &lt;interface&gt; tags.
     *
     * @throws SAXException
     */
    @Test(expected = SAXException.class)
    public void testGetElementNoInterface() throws SAXException {
        ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-notCorrect-nointerface.xml"));
    }

    /**
     * Compares two UmlExports to verify built and expected results.
     *
     * @param e1
     * @param e2
     * @return
     */

    private boolean checkElements(UmlExport e1, UmlExport e2) {
        boolean passed = true;
        List<Interface> children_e1;
        List<Interface> children_e2;

        if (!e1.toString().equals(e2.toString())) {
            logger.error(e1.toString() + " doesn´t match " + e2.toString());
            passed = false;
        }
        if (passed) {
            children_e1 = e1.getInterfaces();
            children_e2 = e2.getInterfaces();
            if (children_e1.size() != children_e2.size()) {
                passed = false;
            } else {
                for (int i = 0; i < children_e1.size(); i++) {
                    passed = checkInterface(children_e1.get(i), children_e2
                            .get(i));
                }
            }
        }
        return passed;
    }

    /**
     * Compares two Interfaces to verify built and expected results.
     *
     * @param e1
     * @param e2
     * @return
     */

    private boolean checkInterface(Interface interface1, Interface interface2) {
        boolean passed = true;

        if (!interface1.toString().equals(interface2.toString())) {
            logger.error(interface1.toString() + " doesn´t match "
                    + interface2.toString());
            passed = false;
        }
        if (passed) {
            if (interface1.getName().compareTo(interface2.getName()) == 0
                    && interface1.getClassification().compareTo(
                            interface2.getClassification()) == 0
                    && interface1.getGuid().compareTo(interface2.getGuid()) == 0
                    && interface1.getPackage().compareTo(
                            interface2.getPackage()) == 0) {
                List<Platform> pl1 = interface1.getPlatforms();
                List<Platform> pl2 = interface2.getPlatforms();
                // Check that all p1 are in p2
                for (Platform p : pl1) {
                    boolean found = false;

                    for (Platform p2 : pl2) {
                        if (p.equals(p2)) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        continue;
                    } else {
                        passed = false;
                        System.out.println(p.toString()
                                + " not found in interface 2");
                        break;
                    }
                }

                // Check that all p2 are in p1
                for (Platform p : pl2) {
                    boolean found = false;

                    for (Platform p2 : pl1) {
                        if (p.equals(p2)) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        continue;
                    } else {
                        passed = false;
                        System.out.println(p.toString()
                                + " not found in interface 2");
                        break;
                    }
                }
            } else {
                passed = false;
            }
        }
        return passed;
    }

    /**
     * Test method for {@link ElementFactory#getElement(File imTocFile)}.
     * Testing using the ste-uml-export-from-ea-r1a012b1.xml file. This file is
     * exported using EAScript R1A012 Beta 1 from the model on 2010-03-02.
     *
     * @throws SAXException
     */
    @Test
    public void testGetElementEaExportR1A012Beta1() throws SAXException {

        UmlExport elm = ElementFactory
                .getElement(new File(
                        "test//com//stericsson//RefMan//UmlExportFilter//ste-uml-export-from-ea-r1a012b1.xml"));

        simplecheckforplatforms(elm, 28, new String[] { "U5500", "U8500" });
    }

}
