/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestFindTocLocationsFromFile {

    /**
     * A path to the test 'tocs.txt'.
     */
    final static String toc = "test/com/stericsson/RefMan/USDoc/resources/tocs.txt";

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testFindTocLocationsFiles() {
        File testToc = new File(toc);
        FindTocLocationsFromFile findTocLocationsFromFile = new FindTocLocationsFromFile(
                testToc);
        List<String> tocs = findTocLocationsFromFile.findTocLocationsFiles();

        assertTrue(tocs.size() == 3);
    }
}