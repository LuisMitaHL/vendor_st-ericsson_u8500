/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import static org.junit.Assert.*;

import java.security.InvalidParameterException;

import org.junit.Before;
import org.junit.Test;

import com.stericsson.RefMan.UmlExportFilter.Platform;

/**
 * This class is responsible for testing that a <code>Platform</code> can be
 * correctly configured and compared.
 * 
 * @author Fredrik Lundström
 * 
 */
public class TestUmlExportPlatform {

    /**
     * The Platform used during each test.
     */
    Platform p = null;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        p = new Platform();
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#getName()}.
     */
    @Test
    public void testGetName() {
        assertTrue(p.getName().compareTo("") == 0);

        p.setName("TestName");
        assertTrue(p.getName().compareTo("TestName") == 0);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#isInterfaceInternalOnly()}
     * .
     */
    @Test(expected = InvalidParameterException.class)
    public void testIsInterfaceInternalOnlyInvalid() {
        assertTrue(p.getInterfaceInternalOnly().compareTo("") == 0);
        assertFalse(p.isInterfaceInternalOnly());

        p.setInterfaceInternalOnly("Invalid");
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#isInterfaceInternalOnly()}
     * .
     */
    @Test(expected = InvalidParameterException.class)
    public void testIsInterfaceInternalOnlyYesUpperCase() {
        p.setInterfaceInternalOnly("Yes");
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#isInterfaceInternalOnly()}
     * .
     */
    @Test
    public void testIsInterfaceInternalOnlyYesLowerCase() {
        p.setInterfaceInternalOnly("yes");
        assertTrue(p.getInterfaceInternalOnly().compareTo("yes") == 0);
        assertTrue(p.isInterfaceInternalOnly());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#isInterfaceInternalOnly()}
     * .
     */
    @Test(expected = InvalidParameterException.class)
    public void testIsInterfaceInternalOnlyNoUpperCase() {
        p.setInterfaceInternalOnly("No");
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#isInterfaceInternalOnly()}
     * .
     */
    @Test
    public void testIsInterfaceInternalOnlyNoLowerCase() {
        p.setInterfaceInternalOnly("no");
        assertTrue(p.getInterfaceInternalOnly().compareTo("no") == 0);
        assertFalse(p.isInterfaceInternalOnly());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.UmlExportFilter.Platform#equals(java.lang.Object)}
     * .
     */
    @Test
    public void testEqualsObject() {
        Platform p2 = new Platform();
        assertTrue(p.equals(p2));

        // Check names (ii not initialized), first one name uninitialized,
        // then equals

        p.setName("A Name");
        assertFalse(p.equals(p2));

        p2.setName("A Name");
        assertTrue(p.equals(p2));

        // Check one initialized

        p.setInterfaceInternalOnly("yes");
        assertFalse(p.equals(p2));

        // Check both initialized, but different

        p2.setInterfaceInternalOnly("no");
        assertFalse(p.equals(p2));

        // Check names (differnet names)

        p2.setName("A Different Name");
        assertFalse(p.equals(p2));
    }

}
