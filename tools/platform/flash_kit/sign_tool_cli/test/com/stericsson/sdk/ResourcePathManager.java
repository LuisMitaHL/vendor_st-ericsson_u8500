package com.stericsson.sdk;

import java.io.File;

import junit.framework.Assert;

import org.junit.Test;

/**
 * Helper for fetching correct paths to resource files regarding runtime environment. Especially it's intended to avoid
 * problems with accessing resource files from junit tests run on build server.
 * 
 * @author emicroh
 */
public class ResourcePathManager {

    private static final String BUILD_SERVER_PATH_PREFIX = "sign_tool_cli/";

    /**
     * Return relative path regarding current runtime environment.
     * 
     * @param relativePath Path to resource file which should be translated into correct form.
     * @return Translated relative path in accordance with runtime environment.
     * @throws NullPointerException When file pointed out by given relative path cannot be found or doesn't exist.
     */
    public static String getResourceFilePath(String relativePath) throws NullPointerException {
        if ((new File(relativePath)).exists()) {
            return relativePath;
        } else if ((new File(BUILD_SERVER_PATH_PREFIX + relativePath)).exists()) {
            return BUILD_SERVER_PATH_PREFIX + relativePath;
        } else {
            throw new NullPointerException("Path to resource file '" + relativePath + "' cannot be found!");
        }
    }

    /** */
    @Test
    public void getResourceFilePathTest() {
        Assert.assertNotNull(getResourceFilePath(System.getProperty("user.dir")));
    }
}
