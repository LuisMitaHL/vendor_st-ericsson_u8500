/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.kernelDoc;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.util.ArrayList;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Commons.IO;

/**
 * @author ethopal
 * 
 */
public class TestCopyHtml {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(TestCopyHtml.class);

    /**
     * The path to the folder for the Linux Kernel.
     */
    private static File kernelDir;

    /**
     * The path to the folder where the <code>HTML</code> documentation will be
     * copied.
     */
    private static File htmlDir;

    /**
     * The path to the folder where the kernel space toc files for the books
     * reside.
     */
    private static File kernelTocDir;

    /**
     * The path to the folder for the Linux Kernel for std copy test.
     */
    private static File kernelDirStd;

    /**
     * The path to the folder where the <code>HTML</code> documentation will be
     * copied for std copy test.
     */
    private static File htmlDirStd;

    /**
     * The path to the folder where the kernel space toc files for the books
     * reside for std copy test.
     */
    private static File kernelTocDirStd;

    /**
     * @throws java.lang.Exception
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        kernelDir = new File("test/com/stericsson/RefMan/kernelDoc/linux_root");
        htmlDir = new File("test/com/stericsson/RefMan/kernelDoc/htmldir");
        kernelTocDir = new File(
                "test/com/stericsson/RefMan/kernelDoc/kernel_tocs");
        kernelDirStd = new File(
                "test/com/stericsson/RefMan/kernelDoc/stdkernelDoc/linux_root");
        htmlDirStd = new File(
                "test/com/stericsson/RefMan/kernelDoc/stdkernelDoc/htmldir");
        kernelTocDirStd = new File(
                "test/com/stericsson/RefMan/kernelDoc/stdkernelDoc/kernel_tocs");
    }

    /**
     * @throws java.lang.Exception
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        IO.deleteDirContent(htmlDir);
        IO.deleteDirContent(htmlDirStd);
    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.kernelDoc.KernelDoc#copyHtml(java.io.File, java.io.File, java.io.File)}
     * .
     */
    @Test
    public final void testCopyHtml() {
        KernelDoc.copyHtml(kernelDir, htmlDir, kernelTocDir);

        ArrayList<String> correctFiles = new ArrayList<String>();
        correctFiles.add("tc35892.html");
        correctFiles.add("stmpe2401.html");
        correctFiles.add("stmpe1601.html");

        ArrayList<Integer> correctSize = new ArrayList<Integer>();
        correctSize.add(33);
        correctSize.add(19);
        correctSize.add(23);

        if (htmlDir.list().length != 6) {
            logger.error("htmlDir should contain 6 files including folders.");
            fail("htmlDir should contain 6 files including folders.");
        }

        for (File file : htmlDir.listFiles()) {
            if (file.isFile()) {
                correctFiles.remove(file.getName());
            } else {
                correctSize.remove((Integer) file.list().length);
            }
        }

        if (correctFiles.size() != 0 || correctSize.size() != 0) {
            logger.error("Copy operation did not copy the right files.");
            fail("Copy operation did not copy the right files.");
        }

        assertTrue(true);

    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.kernelDoc.KernelDoc#copyHtml(java.io.File, java.io.File, java.io.File)}
     * .
     */
    @Test
    public final void testCopyHtmlStd() {
        logger.info("testCopyHtmlStd started");
        KernelDoc.copyHtml(kernelDirStd, htmlDirStd, kernelTocDirStd);

        ArrayList<String> correctFiles = new ArrayList<String>();
        correctFiles.add("tc35892.html");

        ArrayList<Integer> correctSize = new ArrayList<Integer>();
        correctSize.add(23);

        assertTrue(htmlDirStd.exists());
        assertTrue(htmlDirStd.isDirectory());
        if (htmlDirStd.list().length != 2) {
            logger.info("htmldir contains " + htmlDirStd.list());
            logger.error("htmlDir should contain 2 files including folders.");
            fail("htmlDir should contain 2 files including folders.");
        }

        for (File file : htmlDirStd.listFiles()) {
            if (file.isFile()) {
                correctFiles.remove(file.getName());
            } else {
                correctSize.remove((Integer) file.list().length);
            }
        }

        if (correctFiles.size() != 0 || correctSize.size() != 0) {
            logger.error("Copy operation did not copy the right files.");
            fail("Copy operation did not copy the right files.");
        }
    }
}
