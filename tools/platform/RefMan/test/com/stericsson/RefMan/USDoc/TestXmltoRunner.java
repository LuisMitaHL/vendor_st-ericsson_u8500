/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.util.Properties;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class TestXmltoRunner {

    /** The Standard API toc file to test with. */
    private final File stdApiTocFile = new File(
            "test/com/stericsson/RefMan/USDoc/stdApi/stdapi-toc-locations.xml");

    /** The stelp directory. */
    private final File stelpDir = new File("test/com/stericsson/RefMan/USDoc");

    /** The html output folder. */
    private final File htmlDir = new File("test/com/stericsson/RefMan/USDoc/htmlOut");

    /** The tempoutput folder. */
    private final File tempDir = new File("test/com/stericsson/RefMan/USDoc/tempOut");

    /** The output folder for the specific Standard API toc file. */
    private final File stdApiOutputFolder = new File(htmlDir, "opengl/docs");

    @Before
    public void setUp() throws Exception {
        if (!htmlDir.exists()) {
            htmlDir.mkdirs();
        }
        if (!tempDir.exists()) {
            tempDir.mkdirs();
        }
    }

    @After
    public void tearDown() throws Exception {
        deleteDirs(htmlDir);
        deleteDirs(tempDir);
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
     * Test method for {@link com.stericsson.RefMan.USDoc.XmltoRunner#run()}
     */
    @Test
    public void testRun() {
        XmltoRunner executeStdApiRunner = new XmltoRunner(stdApiTocFile,
                stelpDir, htmlDir, tempDir, 0);
        executeStdApiRunner.run();

        String[] files = stdApiOutputFolder.list();
        assertTrue(files.length == 1);
        assertTrue(files[0].equalsIgnoreCase("dummy.h.html"));

        // we have problems with the proxy for this test case.
        Properties properties = System.getProperties();
        String osName = properties.getProperty("os.name");
        if (osName.contains("Windows")) {
            fail("This test case cannot be successfylly run on a Windows computer");
        }

    }
}
