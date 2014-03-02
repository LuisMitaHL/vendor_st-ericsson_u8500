/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class FindTocLocationsFromFile implements FindTocLocationsFiles {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(FindTocLocationRecursively.class);

    private final File tocsList;

    /**
     * Constructor for <code>FindTocLocationsFromFile</code>.
     *
     * @param tocsList
     *            The file which lists all toc files. Will look something like
     *            this (paths are absolute).
     *            <p>
     *            path1/toc-locations.xml
     *            <p>
     *            path2/toc-locations.xml
     *            <p>
     *            path3/toc-locations.xml
     *            <p>
     *            path4/toc-locations.xml
     *            <p>
     */

    public FindTocLocationsFromFile(File tocsList) {
        this.tocsList = tocsList;
    }

    /**
     * Finds the absolute paths to the toc-locations.xml files from
     * <code>tocsList</code>.
     *
     * @return A <code>List</code> containing <code>String</code>'s representing
     *         the absolute paths to the toc-locations.xml files.
     */
    public List<String> findTocLocationsFiles() {
        Scanner scan;
        ArrayList<String> tocs = new ArrayList<String>();
        try {
            scan = new Scanner(tocsList);
            while (scan.hasNextLine()) {
                tocs.add(scan.nextLine());
            }
        } catch (FileNotFoundException e) {
            logger.error("Could not find file '{}'", tocsList);
        }
        return tocs;
    }
}
