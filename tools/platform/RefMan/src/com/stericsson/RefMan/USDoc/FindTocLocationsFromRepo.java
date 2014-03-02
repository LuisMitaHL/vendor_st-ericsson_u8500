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

public class FindTocLocationsFromRepo implements FindTocLocationsFiles {
    /** root directory for the system, used to locate .repo catalogue */
    private final File projectRootDir;
    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(FindTocLocationsFromRepo.class);

    /**
     * Constructor for <code>FindTocLocationsFromRepo</code>.
     */
    public FindTocLocationsFromRepo(File systemRootDir) {
        projectRootDir = systemRootDir;

    }

    public List<String> findTocLocationsFiles() {

        /** repo manifest file */
        File repoProjectList = new File(projectRootDir, ".repo"
                + File.separator + "project.list");
        List<String> tocFiles = new ArrayList<String>();

        // *parse the repo file and extract toc files*/
        Scanner scan;
        try {
            // *fetch line from project.list
            scan = new Scanner(repoProjectList);
            while (scan.hasNextLine()) {
                String gitPath = scan.nextLine();
                File tocFile = new File(projectRootDir + File.separator
                        + gitPath, "ste-info" + File.separator
                        + "toc-locations.xml");
                File tocFileNotSteInfo = new File(projectRootDir
                        + File.separator + gitPath, "toc-locations.xml");
                if (tocFile.isFile()) {
                    tocFiles.add(tocFile.getAbsolutePath());
                    logger.debug("Found toc file at: "
                            + tocFile.getAbsolutePath());
                } else if (tocFileNotSteInfo.isFile()) {
                    tocFiles.add(tocFileNotSteInfo.getAbsolutePath());
                    logger.debug("Found toc file at: "
                            + tocFileNotSteInfo.getAbsolutePath());
                } else {
                    logger.debug("Git: " + gitPath
                            + " does not contain toc file");
                }
            }
        } catch (FileNotFoundException e) {
            // *if repo file does not exists log error*/
            logger.error("Repo manifest file not found in: "
                    + repoProjectList.getAbsolutePath());
        }
        return tocFiles;
    }
}
