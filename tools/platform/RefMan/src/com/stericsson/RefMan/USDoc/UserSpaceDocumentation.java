/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Only contains one method {@link
 * #generateUserSpaceDocumentation(List<FindTocLocationsFiles> findMethods)}
 *
 * @author Thomas Palmqvist
 *
 */
public class UserSpaceDocumentation extends Thread {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(UserSpaceDocumentation.class);

    /** The project root directory */
    private final File projectRootDir;

    /** The html directory */
    private final File htmlDir;

    /** The temp directory */
    private final File tempDir;

    /**
     * The file which lists all toc files.
     * <p>
     * Will look something like this.
     * <p>
     * path1/toc-locations.xml
     * <p>
     * path2/toc-locations.xml
     * <p>
     * path3/toc-locations.xml
     * <p>
     * path4/toc-locations.xml
     * <p>
     */
    private File tocsList = null;

    /** A <code>Vector</code> containing the created threads. */
    private static Vector<Thread> threads = new Vector<Thread>();

    /**
     * The {@link #generateUserSpaceDocumentation(List<FindTocLocationsFiles>
     * findMethods)} method uses appropriate methods to find the
     * toc-locations.xml files and starts to generate documentation.
     *
     * @param projectRootDir
     *            The path to the folder for the project root.
     * @param htmlDir
     *            The path to the folder where the <code>HTML</code>
     *            documentation will be placed.
     * @param tempDir
     *            The path to the folder where the temporary files will be
     *            placed these files will also be useful for debugging if
     *            something goes wrong.
     */
    public UserSpaceDocumentation(File projectRootDir, File htmlDir,
            File tempDir, File tocsList) {
        this.projectRootDir = projectRootDir;
        this.htmlDir = htmlDir;
        this.tempDir = tempDir;
        this.tocsList = tocsList;
    }

    /**
     * A method which finds all toc-locations.xml files and starts to generate
     * the documentation as specified in them.
     *
     * @param findMethods
     *            A <code>List</code> containing references to different methods
     *            to find the toc-locations.xml files.
     */
    public void generateUserSpaceDocumentation(
            List<FindTocLocationsFiles> findMethods) {
        List<String> tocFiles = findTocLocationsFiles(findMethods);
        handleTocsLocationsFiles(tocFiles);

        try {
            if (tocsList.exists()) {
                tocsList.delete();
            }
            tocsList.createNewFile();
            FileWriter fw = new FileWriter(tocsList);
            for (String tocFile : tocFiles) {
                fw.append(tocFile);
                fw.append("\r\n");
            }
            fw.flush();
            fw.close();
        } catch (IOException e) {
            logger.error("Failed to write to '{}'", tocsList.getAbsolutePath());
        }
    }

    /**
     * The method uses an appropriate way to find the toc-locations.xml files.
     *
     * @return A <code>List</code> of <code>String</code>'s representing the the
     *         absolute paths to the toc.locations.xml files.
     */
    private List<String> findTocLocationsFiles(
            List<FindTocLocationsFiles> findMethods) {
        ArrayList<String> tocFiles = new ArrayList<String>();
        // Iterate through the list of different methods to find the
        // toc-locations.xml files.
        for (FindTocLocationsFiles method : findMethods) {
            List<String> tocs = method.findTocLocationsFiles();
            for (String tocFile : tocs) {
                // A toc-locations.xml file should only be used once. If it has
                // been found using another method, it should not be added to
                // the list of paths.
                if (!tocFiles.contains(tocFile)) {
                    tocFiles.add(tocFile);
                }
            }
        }
        return tocFiles;
    }

    /**
     * A method which starts the generation of the documentation from the
     * toc-locations.xml files.
     *
     * @param tocFiles
     *            A <code>List</code> of <code>String</code>'s representing the
     *            absolute paths to the toc-locations.xml files.
     */
    private void handleTocsLocationsFiles(List<String> tocFiles) {
        int cnt = 0;
        for (String tocFile : tocFiles) {
            TransformDocumentation transformDocumentation = new TransformDocumentation(
                    htmlDir, tempDir, projectRootDir);
            try {
                // References to the threads needs to be saved, otherwise we can
                // not check if they are still alive.
                threads.add(transformDocumentation.transformDocumentation(
                        new File(tocFile), cnt));
            } catch (IOException e) {
                logger.error("Failed to handle toc-locations file '{}'",
                        tocFile, e);
            }
            cnt++;
        }
        // Wait for all threads to finish their job.
        logger.debug("Waiting for all threads to finish");
        for (Thread thread : threads) {
            try {
                thread.join(600000);
            } catch (InterruptedException e) {
                logger.error("Manual generation timed out after 10 minutes."
                        + " The reference manual may not be complete");
            }
        }
    }
}
