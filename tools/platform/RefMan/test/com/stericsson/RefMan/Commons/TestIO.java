/**
 * © Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Commons;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.LineNumberReader;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Arrays;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author ethopal
 *
 */
public class TestIO {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(TestIO.class);

    /**
     * @throws java.lang.Exception
     */
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    /**
     * @throws java.lang.Exception
     */
    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    private File testDir;
    private File sourceDir;
    private File destDir;
    private File sourceFile;
    private File escapeFile;
    private File escapeDestFile;
    private File optdir;
    private File optdirdest;
    private String failureReason;
    private File sourceFileNoConversion;
    private File sourceDirNoConversion;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        if (!new File("test/com/stericsson/RefMan/Commons/test_files/").mkdir()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File("test/com/stericsson/RefMan/Commons/test_files/genfld1")
                .mkdir()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1_no_conversion")
                .mkdir()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1/genfld2")
                .mkdir()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1/file1.html")
                .createNewFile()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/file1.html")
                .createNewFile()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1/file2.html")
                .createNewFile()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }
        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1/genfld2/file3.html")
                .createNewFile()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }

        if (!new File(
                "test/com/stericsson/RefMan/Commons/test_files/htmlescape.txt")
                .createNewFile()) {
            logger.error("Set up failed");
            fail("Set up failed");
        }

        sourceDir = new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1");
        sourceDirNoConversion = new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1_no_conversion");
        destDir = new File(
                "test/com/stericsson/RefMan/Commons/test_files/copygenfld1");
        testDir = new File("test/com/stericsson/RefMan/Commons/test_files");
        escapeFile = new File(
                "test/com/stericsson/RefMan/Commons/test_files/htmlescape.txt");
        escapeDestFile = new File(
                "test/com/stericsson/RefMan/Commons/test_files/htmlescape-dest.txt");

        sourceFile = new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1/file1.html");
        FileWriter fw = new FileWriter(sourceFile);
        fw.write("<html><head></head><body>HELLO</body></html>");
        fw.flush();
        fw.close();

        sourceFileNoConversion = new File(
                "test/com/stericsson/RefMan/Commons/test_files/genfld1_no_conversion/file1.html");
        FileWriter fw2 = new FileWriter(sourceFileNoConversion);
        fw2.write("HELLO");
        fw2.flush();
        fw2.close();

        fw = new FileWriter(escapeFile);
        String leadingZeros = "00000";
        for (int lineNumber = 1; lineNumber <= 1999; lineNumber++) {
            // 1999 lines of 80 characters each is larger than 100kB, hence
            // causing at least one
            String s = Integer.toString(lineNumber);
            String lineNumberAsString = leadingZeros.substring(0, leadingZeros
                    .length()
                    - s.length())
                    + s;
            String lineAsString = new String(
                    "Line "
                            + lineNumberAsString
                            + ": &<> 7890123456789012345678901234567890123456789012345678901234567890\n");
            fw.write(lineAsString);
        }
        fw.flush();
        fw.close();
        assertTrue(escapeFile.length() > IO.MAX_FILE_BUFFER_KILOBYTES * 1024);

        optdir = new File(
                "test/com/stericsson/RefMan/Commons/test_files/optionally_convert/");
        optdir.mkdirs();
        optdirdest = new File(
                "test/com/stericsson/RefMan/Commons/test_files/optionally_convert_dest/");
        optdirdest.mkdirs();

    }

    /**
     * @throws java.lang.Exception
     */
    @After
    public void tearDown() throws Exception {

        try {
            IO.deleteDirContent(testDir);
        } catch (IOException e) {
            logger.error("deleteDirContent faild ", e);
        }
        if (testDir.exists()) {
            testDir.delete();
        }
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Commons.IO#deleteDirContent(java.io.File)}.
     */
    @Test
    public final void testDeleteDirContent() {

        try {
            IO.deleteDirContent(testDir);
        } catch (IOException e) {
            fail();
            logger.error("", e);
        }
        boolean passed = testDir.list().length == 0 ? true : false;
        if (!passed) {
            logger.error("The directory's content should be empty but wasn't.");
        }
        assertTrue("The directory's content should be empty but wasn't.",
                passed);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Commons.IO#copyDirectory(java.io.File, java.io.File, boolean, boolean)}
     *
     */
    @Test
    public final void testCopyDirectory() {
        try {
            IO.copyDirectory(sourceDir, destDir, true, true);
        } catch (IOException e) {
            logger.error("", e);
            fail();
        }
        boolean passed = dirContentSame(sourceDir, destDir);

        if (!passed) {
            logger
                    .error(
                            "The directory's content should be the same but wasn't, reason=",
                            failureReason);
        }
        assertTrue(
                "The directory's content should be the same but wasn't, reason="
                        + failureReason, passed);

        try {
            FileWriter fw = new FileWriter(sourceFile);
            fw.append("<html><head></head><body>22</body></html>");
            fw.flush();
            fw.close();
        } catch (IOException e) {
            logger.error("Could not append to sourceFile", e);
            fail();
        }

        passed = !dirContentSame(sourceDir, destDir);

        if (!passed) {
            logger
                    .error("The directory's content shouldn't be the same but was");
        }
        assertTrue("The directory's content shouldn't be the same but was.",
                passed);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Commons.IO#copyDirectory(java.io.File, java.io.File, boolean, boolean)}
     *
     */
    @Test
    public final void testCopyDirectoryNoConversion() {

        try {
            IO.copyDirectory(sourceDirNoConversion, destDir, false, false);
        } catch (IOException e) {
            logger.error("", e);
            fail();
        }
        boolean passed = dirContentSame(sourceDirNoConversion, destDir);

        if (!passed) {
            logger
                    .error(
                            "The directory's content should be the same but wasn't, reason=",
                            failureReason);
        }
        assertTrue(
                "The directory's content should be the same but wasn't, reason="
                        + failureReason, passed);

        try {
            FileWriter fw = new FileWriter(sourceFileNoConversion);
            fw.append("22");
            fw.flush();
            fw.close();
        } catch (IOException e) {
            logger.error("Could not append to sourceFile", e);
            fail();
        }

        passed = !dirContentSame(sourceDirNoConversion, destDir);

        if (!passed) {
            logger
                    .error("The directory's content shouldn't be the same but was");
        }
        assertTrue("The directory's content shouldn't be the same but was.",
                passed);
    }

    /**
     * Not a complete compare method. Only a very rough one.
     *
     * @param dir1
     *            Directory to be compared
     * @param dir2
     *            Directory to be compared
     * @return <code>true</code> if the directories same.
     */
    private boolean dirContentSame(File dir1, File dir2) {
        if (dir1.isDirectory() && dir2.isDirectory()) {
            File[] files1 = dir1.listFiles();
            File[] files2 = dir2.listFiles();
            Arrays.sort(files1);
            Arrays.sort(files2);
            if (files1.length != files2.length) {
                failureReason = "Directory '" + dir1 + "' has " + files1.length
                        + ", but directory '" + dir2 + "' has " + files2.length
                        + " files.";
                return false;
            }
            for (int i = 0; i < files1.length; i++) {
                if (files1[i].isDirectory()) {
                    if (!dirContentSame(files1[i], files2[i])) {
                        return false;
                    }
                } else {
                    if (!fileContentSame(files1[i], files2[i])) {
                        failureReason = "File '" + files1[i]
                                + "' mismatch against '" + files2[i];
                        return false;
                    }
                }
            }
        } else {
            failureReason = "Directory/File mismatch between '" + dir1
                    + "' and '" + dir2 + "'";
            return false;
        }
        return true;
    }

    /**
     * Not a complete compare method. Only a very rough one.
     *
     * @param f1
     *            File to be compared
     * @param f2
     *            File to be compared
     * @return <code>true</code> if the File are the same.
     */
    private boolean fileContentSame(File f1, File f2) {
        if (f1.isFile() && f2.isFile() && f1.length() == f2.length()) {
            return true;
        } else {
            return false;
        }
    }

    /**
     *Test method for
     * {@link com.stericsson.RefMan.Commons.IO#copyFile(java.io.File, java.io.File)}
     *
     */
    @Test
    public final void testCopyFile() {
        File destFile = null;
        try {
            if (!destDir.mkdir()) {
                logger.error("destDir could not be created");
                fail("destDir could not be created");
            }
            String destFilePath = destDir.getAbsolutePath() + "/"
                    + sourceFile.getName();
            destFile = new File(destFilePath);
            IO.copyFile(sourceFile, destFile);
        } catch (IOException e) {
            logger.error("", e);
            fail();
        }
        if (!fileContentSame(sourceFile, destFile)) {
            logger
                    .error("The directory's content should be the same but wasn't.");
            fail();
        }
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Commons.IO#escapeHtml(String)}
     *
     */
    @Test
    public final void testEscapeHTML() {
        String result = IO.escapeHtml("&<>");
        assertEquals("&amp;&lt;&gt;", result);
        String result2 = IO.escapeHtml("A&B<C>D");
        assertEquals("A&amp;B&lt;C&gt;D", result2);
    }

    /**
     * Test method for
     * {@link com.stericsson.RefMan.Commons.IO#copyFileAndConvertToHtml(File, File, String)}
     *
     * @throws IOException
     */
    @Test
    public final void testCopyFileAndConvert() throws IOException {
        IO.copyFileAndConvertToHtml(escapeFile, escapeDestFile,
                "The title here");

        assertTrue(escapeDestFile.exists());
        assertTrue(escapeDestFile.length() >= 1999 * (4 + 3 + 3)
                + escapeFile.length());
        LineNumberReader lr = new LineNumberReader(new FileReader(
                escapeDestFile));
        String leadingZeros = "00000";
        String readLine = lr.readLine();
        assertEquals(
                "<html><head><title>The title here</title></head><body><pre>",
                readLine);
        for (int lineNumber = 1; lineNumber <= 1999; lineNumber++) {
            readLine = lr.readLine();
            String s = Integer.toString(lineNumber);
            String lineNumberAsString = leadingZeros.substring(0, leadingZeros
                    .length()
                    - s.length())
                    + s;
            String expectedLine = new String(
                    "Line "
                            + lineNumberAsString
                            + ": &amp;&lt;&gt; 7890123456789012345678901234567890123456789012345678901234567890");
            assertEquals(expectedLine, readLine);
        }
        readLine = lr.readLine();
        assertEquals("</pre></body></html>", readLine);
        String nullString = lr.readLine();
        assertNull(nullString);
        try {
            lr.readLine();
        } catch (IOException x) {
            // ok
        }
        lr.close();
    }

    @Test
    public final void testTransferFromAndReplaceAHref() throws IOException {
        StringReader source = new StringReader(
                "<html><head><title>Test title here</title></head>\n"
                        + "<body>\n"
                        + "<!--Comment text here\nwith a line break-->"
                        + "Normal text here<br>with a line break within\n"
                        + "<b>Bold text here</b> and normal text afterwards\n"
                        + "and some <i>italics text</i> as well\n" + "<a href="
                        + '"' + "http://www.stericsson.com" + '"'
                        + ">a link here</a>" + "</body>" + "</html>");
        StringWriter destination = new StringWriter();
        IO.transferFromAndReplaceAHref(source, destination, "internal");
        System.out.println("Destination='" + destination + "'");

        String expected = "<html><head><title>Test title here</title></head>"
                + "<body><!--Comment text here\n"
                + "with a line break-->Normal text here<br>with a line break within "
                + "<b>Bold text here</b> and normal text afterwards and some <i>italics "
                + "text</i> as well <a href=" + '"'
                + "http://www.stericsson.com" + '"' + ">a link here</a>"
                + "</body></html>";
        String result = destination.toString();
        assertEquals(expected, result);

    }

    @Test
    public final void testTransferFromAndReplaceAHrefSimple1()
            throws IOException {
        StringReader source = new StringReader("<html><head></head><body>"
                + "<a href=" + '"' + "http://www.stericsson.com" + '"'
                + " target=" + '"' + "_top" + '"' + ">a link here</a>"
                + "</body></html>");
        StringWriter destination = new StringWriter();
        IO.transferFromAndReplaceAHref(source, destination, "internal");
        System.out.println("Destination='" + destination + "'");

        String expected1 = "<html><head></head><body>" + "<a href=" + '"'
                + "http://www.stericsson.com" + '"' + " target=" + '"' + "_top"
                + '"' + ">a link here</a>" + "</body></html>";

        String expected2 = "<html><head></head><body>" + "<a" + " target="
                + '"' + "_top" + '"' + " href=" + '"'
                + "http://www.stericsson.com" + '"' + ">a link here</a>"
                + "</body></html>";
        String result = destination.toString();
        if (result.equals(expected1) || result.equals(expected2)) {
            // ok; The MAS's may arrive in undefined order.
        } else {
            fail("Didn't return either possible combination!");
        }
    }

    @Test
    public final void testTransferFromAndReplaceAHrefSConverted()
            throws IOException {
        StringReader source = new StringReader("<html><head></head>" + "<body>"
                + "<a href=" + '"' + "converted.txt" + '"' + ">a link here</a>"
                + "</body>" + "</html>");
        StringWriter destination = new StringWriter();
        IO.transferFromAndReplaceAHref(source, destination, "internal");
        System.out.println("Destination='" + destination + "'");

        String expected = "<html><head></head>" + "<body>" + "<a href=" + '"'
                + "converted.txt.html" + '"' + ">a link here</a>" + "</body>"
                + "</html>";
        String result = destination.toString();
        assertEquals(expected, result);
    }

    @Test
    public final void testConvertHRefConvert() {
        HtmlAHrefConverter c = new HtmlAHrefConverter(null);
        String result;

        //

        result = c.convertHref("convert.TXT");
        assertEquals("convert.TXT.html", result);

        result = c.convertHref("convert.txt");
        assertEquals("convert.txt.html", result);

        result = c.convertHref("relative/convert.txt");
        assertEquals("relative/convert.txt.html", result);

        result = c.convertHref("a/relative/convert.txt");
        assertEquals("a/relative/convert.txt.html", result);

        result = c.convertHref("/a/relative/convert.txt");
        assertEquals("/a/relative/convert.txt.html", result);

        result = c.convertHref("/a/relative/convert.TXT");
        assertEquals("/a/relative/convert.TXT.html", result);

        //

        result = c.convertHref("convert");
        assertEquals("convert.html", result);

        result = c.convertHref("relative/convert");
        assertEquals("relative/convert.html", result);

        result = c.convertHref("a/relative/convert");
        assertEquals("a/relative/convert.html", result);

        result = c.convertHref("/a/relative/convert");
        assertEquals("/a/relative/convert.html", result);

    }

    @Test
    public final void testConvertHRefRemoteKeep() {
        HtmlAHrefConverter c = new HtmlAHrefConverter(null);
        String result;

        result = c.convertHref("http://www.stericsson.com/");
        assertEquals("http://www.stericsson.com/", result);

        result = c.convertHref("http://www.stericsson.com/keep.txt");
        assertEquals("http://www.stericsson.com/keep.txt", result);

        result = c.convertHref("http://www.stericsson.com/keep.TXT");
        assertEquals("http://www.stericsson.com/keep.TXT", result);

        result = c.convertHref("http://www.stericsson.com/relative/keep.txt");
        assertEquals("http://www.stericsson.com/relative/keep.txt", result);

        result = c.convertHref("http://www.stericsson.com/a/relative/keep.txt");
        assertEquals("http://www.stericsson.com/a/relative/keep.txt", result);

        result = c.convertHref("http://www.stericsson.com/keep");
        assertEquals("http://www.stericsson.com/keep", result);

        result = c.convertHref("http://www.stericsson.com/a/relative/keep");
        assertEquals("http://www.stericsson.com/a/relative/keep", result);

        result = c.convertHref("http://www.stericsson.com/a/relative/keep/");
        assertEquals("http://www.stericsson.com/a/relative/keep/", result);
    }

    @Test
    public final void testConvertHRefKeep() {
        HtmlAHrefConverter c = new HtmlAHrefConverter(null);
        String result;

        //

        result = c.convertHref("keep.html");
        assertEquals("keep.html", result);

        result = c.convertHref("keep.HTML");
        assertEquals("keep.HTML", result);

        result = c.convertHref("relative/keep.html");
        assertEquals("relative/keep.html", result);

        result = c.convertHref("a/relative/keep.html");
        assertEquals("a/relative/keep.html", result);

        //

        result = c.convertHref("keep.htm");
        assertEquals("keep.htm", result);

        result = c.convertHref("keep.HTM");
        assertEquals("keep.HTM", result);

        result = c.convertHref("relative/keep.htm");
        assertEquals("relative/keep.htm", result);

        result = c.convertHref("a/relative/keep.htm");
        assertEquals("a/relative/keep.htm", result);

        //

        result = c.convertHref("keep.pdf");
        assertEquals("keep.pdf", result);

        result = c.convertHref("keep.PDF");
        assertEquals("keep.PDF", result);

        result = c.convertHref("relative/keep.pdf");
        assertEquals("relative/keep.pdf", result);

        result = c.convertHref("a/relative/keep.pdf");
        assertEquals("a/relative/keep.pdf", result);

        //

        result = c.convertHref("http://www.stericsson.com/keep.pdf");
        assertEquals("http://www.stericsson.com/keep.pdf", result);

        result = c.convertHref("http://www.stericsson.com/relative/keep.pdf");
        assertEquals("http://www.stericsson.com/relative/keep.pdf", result);

        result = c.convertHref("http://www.stericsson.com/a/relative/keep.pdf");
        assertEquals("http://www.stericsson.com/a/relative/keep.pdf", result);
    }

    @Test
    public final void testCopyFileAndOptionallyConvert() throws IOException {
        String fileNamesToSkip[] = new String[] { "whatever.pdf",
                "whatever.htm", "whatever.html", "whatever.doc",

                "whatever.xml", "whatever.tex", "whatever.xsl",

                "whatever.png", "whatever.jpg", "whatever.jpeg",
                "whatever.gif", "whatever.svg",

        };
        String fileNamesToConvert[] = new String[] { "whatever.txt",
                "whateverSomething", "README.whatever", "ChangeLog.whatever",
                "LICENSE.whatever",

                "whatever.c", "whatever.cpp", "whatever.h", "whatever.hpp",
                "whatever.java", "whatever.pl", "whatever.sh", "whatever.c",

                "syncPPP.FAQ" };

        for (String filename : fileNamesToConvert) {
            File f = new File(optdir, filename);
            FileWriter fw = new FileWriter(f);
            fw.write("This should have been converted to HTML\n");
            fw.write("&<>\n");
            fw.close();
        }

        for (String filename : fileNamesToSkip) {
            File f = new File(optdir, filename);
            FileWriter fw = new FileWriter(f);
            fw.write("This should NOT have been converted to HTML\n");
            fw.write("&<>\n");
            fw.close();
        }

        File f = new File(optdir, "convertlinks.html");
        FileWriter fw = new FileWriter(f);
        fw.write("<html><head></head><body>");
        fw.write("<a href=" + '"' + "convertedlink.txt" + '"'
                + ">convertedlink.txt</a>");
        fw.write("</body></html>");
        fw.close();

        File destFile = new File(optdirdest, "convertlinks.html");
        IO.copyFileAndOptionallyConvert(f, destFile, true);

        File convertedLinksFile = new File(optdirdest, "convertlinks.html");

        convertedLinksFile = new File(optdirdest, "convertlinks.html");

        assertTrue(convertedLinksFile.exists());

        LineNumberReader lr = new LineNumberReader(new FileReader(
                convertedLinksFile));
        String readLine = lr.readLine();
        String title = destFile.getName();
        assertEquals("<html><head></head><body><a href=" + '"'
                + "convertedlink.txt.html" + '"'
                + ">convertedlink.txt</a></body></html>", readLine);
        lr.close();

        for (String filename : fileNamesToConvert) {

            destFile = new File(optdirdest, filename);
            f = new File(optdir, filename);

            IO.copyFileAndOptionallyConvert(f, destFile, true);
        }

        for (String filename : fileNamesToSkip) {
            destFile = new File(optdirdest, filename);
            f = new File(optdir, filename);

            IO.copyFileAndOptionallyConvert(f, destFile, true);
        }
        for (String filename : fileNamesToSkip) {
            destFile = new File(optdirdest, filename);

            assertTrue(destFile.exists());
            assertFalse(new File(destFile.getPath() + ".html").exists());
            lr = new LineNumberReader(new FileReader(destFile));
            readLine = lr.readLine();
            if (destFile.getName().endsWith(".html")
                    || destFile.getName().endsWith(".htm")) {
                // Special case for .html and .htm since they are actually
                // parsed and reconverted.
                assertEquals(
                        "<html _implied_="
                                + '"'
                                + "true"
                                + '"'
                                + "><head _implied_="
                                + '"'
                                + "true"
                                + '"'
                                + "></head><body _implied_="
                                + '"'
                                + "true"
                                + '"'
                                + ">This should NOT have been converted to HTML &</body></html>",
                        readLine);

            } else {
                assertEquals("This should NOT have been converted to HTML",
                        readLine);
                readLine = lr.readLine();
                assertEquals("&<>", readLine);
            }

            lr.close();
        }
        for (String filename : fileNamesToConvert) {
            destFile = new File(optdirdest, filename);
            if (destFile.exists()) {
                throw new RuntimeException();
            }

            assertFalse(destFile.exists());
            File convertedFile = new File(destFile.getPath() + ".html");
            assertTrue(convertedFile.exists());

            lr = new LineNumberReader(new FileReader(convertedFile));
            readLine = lr.readLine();
            title = destFile.getName();
            assertEquals("<html><head><title>" + title
                    + "</title></head><body><pre>", readLine);
            readLine = lr.readLine();
            assertEquals("This should have been converted to HTML", readLine);
            readLine = lr.readLine();
            assertEquals("&amp;&lt;&gt;", readLine);

            lr.close();
        }
    }
}
