/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestFindTocLocationsFromRepo {

    /**
     * A path to the test 'tocs.txt'.
     */
    final static String root = "test/com/stericsson/RefMan/USDoc/resources/repo";

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * checks if all toc-location files from the supplied test data are found.
     * Test data is constructed so that all toc-locations.xml files that should
     * be found have test_toc_included directory in the path.
     */
    @Test
    public void testFindTocLocationsFiles() {

        File rootDir = new File(root);

        FindTocLocationsFromRepo findTocLocationsFromRepo = new FindTocLocationsFromRepo(
                rootDir);
        List<String> tocs = findTocLocationsFromRepo.findTocLocationsFiles();
        assertTrue(tocs.size() == 2);
        for (int i = 0; i < tocs.size(); i++) {
            if (!tocs.get(i).contains("test_toc_included")) {
                fail("incorrect toc file located: " + tocs.get(i));
            }
        }
    }
}
