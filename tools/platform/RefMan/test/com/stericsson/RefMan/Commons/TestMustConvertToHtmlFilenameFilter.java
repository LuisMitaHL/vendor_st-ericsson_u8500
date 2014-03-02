/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Commons;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.FilenameFilter;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Test the TestMustConvertToHtmlFilenameFilter.accept() method also tests the
 * KernelDocumentationFilenameFilterer.accept().
 *
 * @author qfrelun
 *
 */
public class TestMustConvertToHtmlFilenameFilter {
    /** The logger */
    public final static Logger logger = LoggerFactory
            .getLogger(TestMustConvertToHtmlFilenameFilter.class);

    /**
     * Tests the KernelDocumentationFilenameFilterer
     */
    @Test
    public final void testAcceptKernelDocumentationFilenameFilterer() {
        FilenameFilter f = new KernelDocumentationFilenameFilterer();

        assertTrue(f.accept(null, "syncPPP.FAQ"));
    }

    @Test
    public final void testAcceptShallBeConverted() {
        FilenameFilter f = new MustConvertToHtmlFilenameFilterer();

        assertTrue(f.accept(null, "whatever.txt"));
        assertTrue(f.accept(null, "whatever"));
        assertTrue(f.accept(null, "README.whatever"));
        assertTrue(f.accept(null, "ChangeLog.whatever"));
        assertTrue(f.accept(null, "LICENSE.whatever"));

        assertTrue(f.accept(null, "whatever.c"));
        assertTrue(f.accept(null, "whatever.cpp"));
        assertTrue(f.accept(null, "whatever.h"));
        assertTrue(f.accept(null, "whatever.hpp"));
        assertTrue(f.accept(null, "whatever.java"));
        assertTrue(f.accept(null, "whatever.pl"));
        assertTrue(f.accept(null, "whatever.sh"));
        assertTrue(f.accept(null, "whatever.c"));
    }

    @Test
    public final void testAcceptShallNotBeConverted() {
        FilenameFilter f = new MustConvertToHtmlFilenameFilterer();

        assertFalse(f.accept(null, "whatever.pdf"));
        assertFalse(f.accept(null, "whatever.htm"));
        assertFalse(f.accept(null, "whatever.html"));
        assertFalse(f.accept(null, "whatever.doc"));

        assertFalse(f.accept(null, "whatever.xml"));
        assertFalse(f.accept(null, "whatever.tex"));
        assertFalse(f.accept(null, "whatever.xsl"));

        assertFalse(f.accept(null, "whatever.png"));
        assertFalse(f.accept(null, "whatever.jpg"));
        assertFalse(f.accept(null, "whatever.jpeg"));
        assertFalse(f.accept(null, "whatever.gif"));
        assertFalse(f.accept(null, "whatever.svg"));

        assertFalse(f.accept(null, "syncPPP.FAQ"));

        assertFalse(f.accept(null, "whatever.htm#whatever"));
        assertFalse(f.accept(null, "whatever.html#whatever"));

        assertFalse(f.accept(null, "whatever.htm#whatever.txt"));
        assertFalse(f.accept(null, "whatever.html#whatever.txt"));

        assertFalse(f.accept(null, "#whatever"));

        // These are basically incorrect, but at least they give a known
        // behavior
        assertTrue(f.accept(null, "1#whatever"));
        assertTrue(f.accept(null, "12#whatever"));
        assertTrue(f.accept(null, "123#whatever"));
        assertTrue(f.accept(null, "1234#whatever"));
        assertTrue(f.accept(null, "12345#whatever"));
        assertTrue(f.accept(null, "123456#whatever"));

        assertFalse(f.accept(null, "http://whereever.com/123456#whatever"));
        assertFalse(f.accept(null, "http://whereever.com/123456#whatever.txt"));
        assertFalse(f.accept(null, "http://whereever.com/123456#whatever.html"));
        assertFalse(f.accept(null, "http://whereever.com/123456#whatever.htm"));
        assertFalse(f.accept(null, "http://whereever.com/123456.html#whatever"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.html#whatever.txt"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.html#whatever.html"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.html#whatever.htm"));
        assertFalse(f.accept(null, "http://whereever.com/123456.htm#whatever"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.htm#whatever.txt"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.htm#whatever.html"));
        assertFalse(f.accept(null,
                "http://whereever.com/123456.htm#whatever.htm"));

        assertFalse(f.accept(null, "ftp://whereever.com/123456#whatever"));
        assertFalse(f.accept(null, "ftp://whereever.com/123456#whatever.txt"));
        assertFalse(f.accept(null, "ftp://whereever.com/123456#whatever.html"));
        assertFalse(f.accept(null, "ftp://whereever.com/123456#whatever.htm"));
        assertFalse(f.accept(null, "ftp://whereever.com/123456.html#whatever"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.html#whatever.txt"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.html#whatever.html"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.html#whatever.htm"));
        assertFalse(f.accept(null, "ftp://whereever.com/123456.htm#whatever"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.htm#whatever.txt"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.htm#whatever.html"));
        assertFalse(f.accept(null,
                "ftp://whereever.com/123456.htm#whatever.htm"));

        assertFalse(f.accept(null, "mailto:xyzzy"));
        assertFalse(f.accept(null, "mailto:iwai@ww.uni-erlangen.de"));
    }

}
