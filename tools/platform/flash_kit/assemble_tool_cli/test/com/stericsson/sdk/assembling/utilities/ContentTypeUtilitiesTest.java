/**
 *
 */
package com.stericsson.sdk.assembling.utilities;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.stericsson.sdk.assembling.AssemblerException;

/**
 * @author mbodan01
 *
 */
public class ContentTypeUtilitiesTest {
    private String testSubType = "gift";

    private String testWrongSubType = "goblin";

    private String testPrefix = "x-santaclaus";

    private String testWrongPrefix = "santaclaus";

    private String testContent = testPrefix + "/" + testSubType;

    private String testWrongContent = testWrongPrefix + "/" + testWrongSubType;

    private String testDoubleWrongContent = testContent + testContent;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() {
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() {
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#checkContentSubType
     * (java.lang.String, java.lang.String)} .
     */
    @Test
    public void testCheckContentSubType() {
        assertTrue(ContentTypeUtilities.checkContentSubType(testContent, testSubType));
        assertFalse(ContentTypeUtilities.checkContentSubType(testContent, testWrongSubType));
        assertFalse(ContentTypeUtilities.checkContentSubType(testWrongContent, testSubType));
        assertFalse(ContentTypeUtilities.checkContentSubType(testWrongContent, testWrongSubType));
        assertFalse(ContentTypeUtilities.checkContentSubType(testDoubleWrongContent, testSubType));
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#getPrefixOfContentType
     * (java.lang.String)} .
     */
    @Test
    public void testGetPrefixOfContentType() {
        String result = null;

        result = ContentTypeUtilities.getPrefixOfContentType(testContent);
        assertEquals(testPrefix, result);
        result = ContentTypeUtilities.getPrefixOfContentType(testWrongContent);
        assertEquals(null, result);
        result = ContentTypeUtilities.getPrefixOfContentType(testDoubleWrongContent);
        assertEquals(null, result);
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#makeContentType
     * (java.lang.String)} .
     *
     * @throws AssemblerException
     *             unexpected exception
     */
    @Test
    public void testMakeContentType() throws AssemblerException {
        assertEquals(testContent, ContentTypeUtilities.makeContentType(testPrefix, testSubType));
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#makeContentType
     * (java.lang.String)} .
     *
     * @throws AssemblerException
     *             expected exception
     */
    @Test(expected = AssemblerException.class)
    public void testMakeContentTypeWithNullPrefix() throws AssemblerException {
        ContentTypeUtilities.makeContentType(null, testSubType);
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#makeContentType
     * (java.lang.String)} .
     *
     * @throws AssemblerException
     *             expected exception
     */
    @Test(expected = AssemblerException.class)
    public void testMakeContentTypeWithEmptyPrefix() throws AssemblerException {
        ContentTypeUtilities.makeContentType(new String(""), testSubType);
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#makeContentType
     * (java.lang.String)} .
     *
     * @throws AssemblerException
     *             expected exception
     */
    @Test(expected = AssemblerException.class)
    public void testMakeContentTypeWithNullSubType() throws AssemblerException {
        ContentTypeUtilities.makeContentType(testPrefix, null);
    }

    /**
     * Test method for { @link
     * com.stericsson.sdk.assembling.utilities.ContentTypeUtilities#makeContentType
     * (java.lang.String)} .
     *
     * @throws AssemblerException
     *             expected exception
     */
    @Test(expected = AssemblerException.class)
    public void testMakeContentTypeWithEmptySubType() throws AssemblerException {
        ContentTypeUtilities.makeContentType(testPrefix, new String(""));
    }

}
