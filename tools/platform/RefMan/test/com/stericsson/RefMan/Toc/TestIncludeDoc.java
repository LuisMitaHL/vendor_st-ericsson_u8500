/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestIncludeDoc {

    /** A correct <code>IncludeDoc</code> used for testing */
    IncludeDoc correct1;

    /** A correct <code>IncludeDoc</code> used for testing */
    IncludeDoc correct2;

    /** An incorrect <code>IncludeDoc</code> used for testing */
    IncludeDoc incorrect;

    @Before
    public void setUp() throws Exception {
        correct1 = new IncludeDoc("directory", "destination", true);
        correct2 = new IncludeDoc("directory", "destination", true);
        incorrect = new IncludeDoc("file", "destination", false);
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.IncludeDoc#equals(Object)}.
     */
    @Test
    public void testEquals() {
        assertTrue(correct1.equals(correct1));
        assertTrue(correct1.equals(correct2));
        assertTrue(correct2.equals(correct1));
        assertFalse(correct1.equals(incorrect));
    }

    /**
     * Test method for {@link com.stericsson.RefMan.Toc.IncludeDoc#hashCode()}.
     */
    @Test
    public void testHashCode() {
        assertTrue(correct1.hashCode() == correct1.hashCode());
        assertTrue(correct1.hashCode() == correct2.hashCode());
        assertTrue(correct2.hashCode() == correct1.hashCode());
        assertTrue(correct1.hashCode() != incorrect.hashCode());
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Toc.IncludeDoc#isDirectory()}.
     */
    @Test
    public void testIsDirectory() {
        assertTrue(correct1.isDirectory());
        assertFalse(incorrect.isDirectory());
    }
}
