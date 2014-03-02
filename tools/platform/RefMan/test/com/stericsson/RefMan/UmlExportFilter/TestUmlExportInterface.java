/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import static org.junit.Assert.*;

import java.security.InvalidParameterException;
import java.util.List;
import java.util.Vector;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.RefMan.UmlExportFilter.Interface;
import com.stericsson.RefMan.UmlExportFilter.Platform;

/**
 * This class is responsible for testing that a <code>Interface</code> can be
 * correctly configured.
 * 
 * @author Fredrik Lundström
 */
public class TestUmlExportInterface {

    /**
     * The interface used during each test.
     */
    Interface i = null;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        i = new Interface();
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getName()}.
     */
    @Test
    public void testGetName() {
        assertTrue(i.getName().compareTo("") == 0);

        try {
            i.setName(null);
            fail("null parameter not caught");
        } catch (InvalidParameterException e) {
            // ok
            assertTrue(i.getName().compareTo("") == 0);
        }

        i.setName("TestName");
        assertTrue(i.getName().compareTo("TestName") == 0);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getPackage()}.
     */
    @Test
    public void testGetPackage() {
        assertTrue(i.getPackage().compareTo("") == 0);

        try {
            i.setPackage(null);
            fail("null parameter not caught");
        } catch (InvalidParameterException e) {
            // ok
            assertTrue(i.getPackage().compareTo("") == 0);
        }

        i.setPackage("TestPackage");
        assertTrue(i.getPackage().compareTo("TestPackage") == 0);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getGuid()}.
     */
    @Test
    public void testGetGuid() {
        assertTrue(i.getGuid().compareTo("") == 0);

        try {
            i.setGuid(null);
            fail("null parameter not caught");
        } catch (InvalidParameterException e) {
            // ok
            assertTrue(i.getGuid().compareTo("") == 0);
        }

        i.setGuid("TestGuid");
        assertTrue(i.getGuid().compareTo("TestGuid") == 0);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getClassification()}
     * .
     */
    @Test
    public void testGetClassificationInvalid() {
        assertTrue(i.getClassification().compareTo("") == 0);
        assertFalse(i.isProprietary());
        assertFalse(i.isStandard());
        assertFalse(i.isProprietaryExtension());

        try {
            i.setClassification("Invalid");
            fail("Execption not thrown");
        } catch (InvalidParameterException e) {
            // ok.
            assertTrue(i.getClassification().compareTo("") == 0);
        }

        try {
            i.setClassification(null);
            fail("Execption not thrown");
        } catch (InvalidParameterException e) {
            // ok.
            assertTrue(i.getClassification().compareTo("") == 0);
        }
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getClassification()}
     * .
     */
    @Test
    public void testGetClassificationProprietary() {
        try {
            i.setClassification("proprietary");
            fail("Lower case not allowed.");
        } catch (InvalidParameterException e) {
            // ok
        }
        assertTrue(i.getClassification().compareTo("") == 0);

        i.setClassification("Proprietary");
        assertTrue(i.getClassification().compareTo("Proprietary") == 0);
        assertTrue(i.isProprietary());
        assertFalse(i.isStandard());
        assertFalse(i.isProprietaryExtension());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getClassification()}
     * .
     */
    @Test
    public void testGetClassificationStandard() {
        try {
            i.setClassification("standard");
            fail("Lower case not allowed.");
        } catch (InvalidParameterException e) {
            // ok
        }
        assertTrue(i.getClassification().compareTo("") == 0);

        i.setClassification("Standard");
        assertTrue(i.getClassification().compareTo("Standard") == 0);
        assertFalse(i.isProprietary());
        assertTrue(i.isStandard());
        assertFalse(i.isProprietaryExtension());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getClassification()}
     * .
     */
    @Test
    public void testGetClassificationProprietaryExtension() {
        try {
            i.setClassification("proprietaryExtension");
            fail("Lower case not allowed.");
        } catch (InvalidParameterException e) {
            // ok
        }
        assertTrue(i.getClassification().compareTo("") == 0);

        i.setClassification("ProprietaryExtension");
        assertTrue(i.getClassification().compareTo("ProprietaryExtension") == 0);
        assertFalse(i.isProprietary());
        assertFalse(i.isStandard());
        assertTrue(i.isProprietaryExtension());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#getPlatforms}.
     */
    @Test
    public void testGetPlatforms() {
        List<Platform> pl = i.getPlatforms();

        assertTrue(pl.size() == 0);

        // add one...

        Platform p1 = new Platform();
        p1.setName("Platform 1");

        i.addPlatform(p1);
        List<Platform> pl1 = i.getPlatforms();

        assertTrue(pl1.size() == 1);
        assertTrue(pl1.contains(p1));

        // add another one ...
        Platform p2 = new Platform();
        p2.setName("Platform 2");

        i.addPlatform(p2);
        List<Platform> pl2 = i.getPlatforms();

        assertTrue(pl2.size() == 2);
        assertTrue(pl2.contains(p1));
        assertTrue(pl2.contains(p2));

        // And verify that lists are read-only:
        pl2.remove(0);
        assertTrue(pl2.size() == 1);

        List<Platform> pl2b = i.getPlatforms();

        assertTrue(pl2b.size() == 2);
        assertTrue(pl2b.contains(p1));
        assertTrue(pl2b.contains(p2));
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#setPlatforms}.
     */
    @Test
    public void testSetPlatforms() {
        List<Platform> pl = i.getPlatforms();
        assertTrue(pl.size() == 0);

        List<Platform> pl_add = new Vector<Platform>();

        Platform p1 = new Platform();
        p1.setName("Platform 1");
        Platform p2 = new Platform();
        p2.setName("Platform 2");

        pl_add.add(p1);
        pl_add.add(p2);

        i.setPlatforms(pl_add);

        // Verify

        List<Platform> pl2 = i.getPlatforms();

        assertTrue(pl2 != pl_add);
        assertTrue(pl2.size() == 2);
        assertTrue(pl2.contains(p1));
        assertTrue(pl2.contains(p2));

        // And verify that lists are read-only:

        Platform p3 = new Platform();
        p3.setName("Platform 3");
        pl_add.add(p3);
        List<Platform> pl2b = i.getPlatforms();

        assertTrue(pl2b != pl_add);
        assertTrue(pl2b.size() == 2);
        assertTrue(pl2b.contains(p1));
        assertTrue(pl2b.contains(p2));
        assertFalse(pl2b.contains(p3));
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Interface#addPlatforms}.
     */
    @Test
    public void testAddPlatforms() {
        List<Platform> pl = i.getPlatforms();
        assertTrue(pl.size() == 0);

        Platform p1 = new Platform();
        p1.setName("Platform 1");
        Platform p2 = new Platform();
        p2.setName("Platform 2");

        i.addPlatform(p1);
        i.addPlatform(p2);

        List<Platform> pl_add = new Vector<Platform>();
        Platform p3 = new Platform();
        p3.setName("Platform 4");
        Platform p4 = new Platform();
        p4.setName("Platform 4");
        pl_add.add(p3);
        pl_add.add(p4);

        i.addPlatforms(pl_add);
        List<Platform> pl4 = i.getPlatforms();

        assertTrue(pl_add.size() == 2);
        assertTrue(pl4.size() == 4);
        assertTrue(pl4.contains(p1));
        assertTrue(pl4.contains(p2));
        assertTrue(pl4.contains(p3));
        assertTrue(pl4.contains(p4));
    }

}
