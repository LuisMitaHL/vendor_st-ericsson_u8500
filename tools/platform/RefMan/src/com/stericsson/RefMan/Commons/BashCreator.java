/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */

package com.stericsson.RefMan.Commons;

import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Class used in order to support unit testing on Windows. On Windows, cygwin
 * needs to be installed.
 *
 * @author qfrelun
 *
 */
public class BashCreator {
    /** The logger */
    public final static Logger logger = LoggerFactory
            .getLogger(BashCreator.class);

    /**
     * Gets the path to bash in an OS-independent way.
     *
     * @return a path such as "/bin/bash" or "C:\cygwin\bin\bash.exe"
     */
    public static String getBash() {
        Properties properties = System.getProperties();
        String osName = properties.getProperty("os.name");
        if (osName.contains("Windows")) {
            return "bash.exe";
        } else {
            return "/bin/bash";
        }
    }

    /**
     * Convert a windows-path to a cygwin path. Paths such as "L:\\whereever"
     * are converted into "/cygdrive/l/whereever".
     *
     * @param path
     *            Windows-path
     * @return Cygwin-path
     */
    public static String convertPath(String path) {
        Properties properties = System.getProperties();
        String osName = properties.getProperty("os.name");
        if (osName.contains("Windows")) {
            String converted = path.replaceAll("\\\\", "/");
            if (converted.charAt(1) == ':' && converted.charAt(2) == '/') {
                converted = "/cygdrive/" + converted.charAt(0)
                        + converted.substring(2);
            }

            return converted;
        } else {
            return path;
        }
    }

}
