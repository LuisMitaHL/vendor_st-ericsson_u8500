package com.stericsson.sdk.common.test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.FileUtils;

/**
 * Test class of FileUtils.
 * 
 * @author xadazim
 * 
 */
public class FileUtilsTest extends TestCase {

    private static final String ENCODING = "UTF-8";

    /**
     * Test for createTmpDir() method
     */
    @Test
    public void testCreateTmpDir() {
        File tmpDir = null;
        try {
            tmpDir = FileUtils.createTmpDir("file_utils_test", "tmpdir");
        } catch (IOException e) {
            fail("createTmpDir thrown exception");
        }

        assertTrue(tmpDir.isDirectory());
        assertTrue(tmpDir.exists());

        tmpDir.delete();
    }

    /**
     * Test for removeFilesInDir() method
     */
    @Test
    public void testRemoveFilesInDir() {
        File tmpDir = null;
        try {
            tmpDir = FileUtils.createTmpDir("file_utils_test", "tmpdir");
        } catch (IOException e) {
            fail("createTmpDir thrown exception");
            e.printStackTrace();
        }

        // try to remove from nonexisting dir
        try {
            FileUtils.removeFilesInDir(new File(tmpDir.getAbsolutePath() + "_nonexisting"));
            fail("Removing files from nonexisting folder should have thrown exception");
        } catch (IOException e) {
            // test successful
            System.out.println("This exception is OK:" + e);
        }

        // try to remove from a directory which has subdirectories
        File subDir = new File(tmpDir, "inner_dir");
        assertTrue(subDir.mkdir());
        try {
            File[] dirContents = tmpDir.listFiles();
            assertNotNull(dirContents);
            assertTrue(dirContents.length == 1);
            FileUtils.removeFilesInDir(tmpDir);
            fail("Removing contents of a directory containg sub directories should have thrown exception");
        } catch (IOException e) {
            // test successful
            System.out.println("This exception is OK:" + e);
        } finally {
            subDir.delete();
        }

        // successful removal
        try {
            File.createTempFile("pref", "suff", tmpDir);
            File.createTempFile("pref", "suff", tmpDir);
            File[] dirContents = tmpDir.listFiles();
            assertNotNull(dirContents);
            assertTrue(dirContents.length == 2);
            FileUtils.removeFilesInDir(tmpDir);
            dirContents = tmpDir.listFiles();
            assertNotNull(dirContents);
            assertTrue(dirContents.length == 0);
            tmpDir.delete();
        } catch (IOException e) {
            fail("Exception thrown during testRemoveFilesInDir()");
            e.printStackTrace();
        }

        tmpDir.delete();
    }

    /**
     * Test of file copying
     */
    @Test
    public void testCopyFile() {
        File tmpDir = null;
        try {
            tmpDir = FileUtils.createTmpDir("file_utils_test", "tmpdir");
        } catch (IOException e) {
            fail("createTmpDir thrown exception");
            e.printStackTrace();
        }

        BufferedReader reader = null;
        OutputStreamWriter writer = null;
        try {
            File in = new File(tmpDir, "in.txt");
            writer = new OutputStreamWriter(new FileOutputStream(in), ENCODING);
            writer.append("Test string");
            writer.close();

            File out = new File(tmpDir, "out.txt");
            FileUtils.copyFile(in, out);
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(out), ENCODING));
            assertTrue(reader.ready());
            assertEquals(reader.readLine(), "Test string");
            assertFalse(reader.ready());
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
            fail();
        } finally {
            try {
                if(reader!=null) {
                    reader.close();
                }
                if(writer!=null) {
                    writer.close();
                }
                FileUtils.removeFilesInDir(tmpDir);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // can't create out file
        try {
            File in = new File(tmpDir, "in.txt");
            File out = new File(new File(tmpDir.getAbsolutePath() + "_nonexisting"), "out.txt");
            assertTrue(in.createNewFile());
            FileUtils.copyFile(in, out);
            fail("Copying to a nonexisting directory should have thrown exception");
        } catch (IOException e) {
            // test successful
            System.out.println("This exception is OK:" + e);
        } finally {
            try {
                FileUtils.removeFilesInDir(tmpDir);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // input file does not exist
        try {
            File in = new File(new File(tmpDir.getAbsolutePath() + "_nonexisting"), "in.txt");
            File out = new File(tmpDir, "out.txt");
            FileUtils.copyFile(in, out);
            fail("Copying from a nonexisting directory should have thrown exception");
        } catch (IOException e) {
            // test successful
            System.out.println("This exception is OK:" + e);
        } finally {
            try {
                FileUtils.removeFilesInDir(tmpDir);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        tmpDir.delete();
    }
}
