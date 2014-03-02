/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class FindTocLocationRecursively implements FindTocLocationsFiles {

    /** The logger */
    final static Logger logger = LoggerFactory
            .getLogger(FindTocLocationRecursively.class);

    /** The stelp directory */
    private final File stelpDir;

    /**
     * Constructor for <code>FindTocLocationRecursively</code>
     *
     * @param stelpDir
     *            The path to the folder for the STELP.
     */
    public FindTocLocationRecursively(File stelpDir) {
        this.stelpDir = stelpDir;
    }

    /**
     * This methods finds all toc-locations.xml files using the Linux find
     * command. The search is started from the <code>stelpDir</code>.
     *
     * @return A <code>List</code> containing <code>String</code>'s representing
     *         the absolute paths to the toc-locations.xml files.
     */
    public List<String> findTocLocationsFiles() {
        ArrayList<String> tocs = new ArrayList<String>();
        try {
            String[] cmdArr = {
                    "/bin/bash",
                    "-c",
                    "find -L . \\( \\( -iname '*.git' -o -iname 'kernel' -o -iname 'kernel-goldfish' -o -iname 'linux' -o -iname 'linux-goldfish' -o -iname 'out' -o -iname 'RefMan' \\) -prune \\) -o \\( -iname '*toc-locations.xml*' -print \\)"};
            ProcessBuilder pb = new ProcessBuilder(cmdArr);

            pb.directory(stelpDir);

            logger.info("Find starting from {}", stelpDir);
            Process proc = pb.start();

            // any error message?
            StreamGobbler errorGobbler = new StreamGobbler(proc
                    .getErrorStream(), "ERROR", "Find");
            errorGobbler.start();

            InputStreamReader isr = new InputStreamReader(proc.getInputStream());
            BufferedReader br = new BufferedReader(isr);
            String line = null;
            while ((line = br.readLine()) != null) {
                // If the line found by 'find' contains toc-locations.xml,
                // add it to the list of toc files to be returned.
                if (line.toLowerCase().contains("toc-locations.xml")) {
                    String tocFilePath = stelpDir.getAbsolutePath()
                            + line.substring(1);
                    if (!tocs.contains(tocFilePath)) {
                        tocs.add(tocFilePath);
                    }
                }
            }
        } catch (IOException e) {
            logger.error("Failed to find toc-locations files using find", e);
        }
        return tocs;
    }
}
