/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestUserSpaceDocumentation {

    /** The path to the html directory */
    private final static File htmlDir = new File(
            "test/com/stericsson/RefMan/USDoc/htmlDir");

    /** The path to the stelp directory */
    private final static File stelpDir = new File(
            "test/com/stericsson/RefMan/USDoc");

    /** The path to the temp directory */
    private final static File tempDir = new File(
            "test/com/stericsson/RefMan/USDoc/tempDir");

    /** The path to the tocs.txt input file. */
    private final static String tocsList = "test/com/stericsson/RefMan/USDoc/resources/tocs.txt";

    @Before
    public void setUp() throws Exception {
        htmlDir.mkdirs();
        tempDir.mkdirs();
    }

    @After
    public void tearDown() throws Exception {
        deleteDirs(tempDir);
        deleteDirs(htmlDir);
    }

    /**
     * Help method to remove temporary files and folders
     * */
    private void deleteDirs(File file) {
        if (file.isDirectory()) {
            File[] children = file.listFiles();
            for (int i = 0; i < children.length; i++) {
                deleteDirs(children[i]);
            }
        }
        file.delete();
    }

    /**
     * Test method for {@link
     * com.stericsson.RefMan.USDoc.UserSpaceDocumentation#
     * generateUserSpaceDocumentation(List<FindTocLocationsFiles>)}
     */
    @Test
    public void testGenerateUserSpaceDocumentation() {
        UserSpaceDocumentation userSpaceDocumentation = new UserSpaceDocumentation(
                stelpDir, htmlDir, tempDir, new File(tocsList));
        FindTocLocationsFromFile findTocLocationsFromFile1 = new FindTocLocationsFromFile(
                new File(tocsList));
        FindTocLocationsFromFile findTocLocationsFromFile2 = new FindTocLocationsFromFile(
                new File(tocsList));
        ArrayList<FindTocLocationsFiles> list = new ArrayList<FindTocLocationsFiles>();
        list.add(findTocLocationsFromFile1);
        list.add(findTocLocationsFromFile2);

        userSpaceDocumentation.generateUserSpaceDocumentation(list);

        Scanner scan;
        try {
            scan = new Scanner(new File(tocsList));
            int i = 0;
            while (scan.hasNextLine()) {
                scan.nextLine();
                i++;
            }
            assertTrue(i == 3);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
}
