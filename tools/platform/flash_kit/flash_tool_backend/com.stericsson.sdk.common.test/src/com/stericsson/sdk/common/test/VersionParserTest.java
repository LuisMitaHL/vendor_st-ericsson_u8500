package com.stericsson.sdk.common.test;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.zip.ZipFile;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.common.VersionParser;
import com.stericsson.sdk.common.image.ITableOfContentsItem;

/**
 * @author xtomzap
 * 
 */
public class VersionParserTest extends TestCase {

    /***/
    public static final String TEST_ZIP = "/test_flasharchive.zip";

    /***/
    @Test
    public void testParser() {
        HashMap<String, String> fileNames = new HashMap<String, String>();
        fileNames.put(ITableOfContentsItem.FILENAME_XLOADER, "boot_image_x-loader.bin");
        fileNames.put(ITableOfContentsItem.FILENAME_ISSW, "boot_image_issw.bin");
        fileNames.put(ITableOfContentsItem.FILENAME_MEM_INIT, "mem_init.bin");
        fileNames.put(ITableOfContentsItem.FILENAME_PWR_MGT, "power_management.bin");
        fileNames.put(ITableOfContentsItem.FILENAME_MODEM, "modem.bin");
        fileNames.put(ITableOfContentsItem.FILENAME_IPL, "ipl.bin");

        File versions = new File("versions.txt");
        versions.deleteOnExit();

        File testFile = new File(Activator.getResourcesPath() + TEST_ZIP);
        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(testFile);
            VersionParser.getInstance().parseVersion(fileNames, zipFile, versions.getName());
        } catch (Exception e) {
            assertTrue(false);
        } finally {
            if (zipFile != null) {
                try {
                    zipFile.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /***/
    @Test
    public void testParserEmptyFileNames() {
        HashMap<String, String> fileNames = new HashMap<String, String>();

        File versions = new File("versions.txt");
        versions.deleteOnExit();

        File testFile = new File(Activator.getResourcesPath() + TEST_ZIP);
        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(testFile);
            VersionParser.getInstance().parseVersion(fileNames, zipFile, versions.getName());
        } catch (Exception e) {
            assertTrue(false);
        } finally {
            if (zipFile != null) {
                try {
                    zipFile.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /** */
    @Test
    public void testNullVersionPath() {
        try {
            assertNull(VersionParser.getInstance().parseVersion(null, null, null));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /** */
    @Test
    public void testGetXLoaderVersion() {
        assertEquals(VersionParser.getInstance().getXLoaderVersion(null, true), "");
        assertEquals(VersionParser.getInstance().getXLoaderVersion(new byte[] {}, true), "");
    }

    /** */
    @Test
    public void testGetISSWVersionNull() {
        assertEquals(VersionParser.getInstance().getISSWVersion(null, true), "");
        assertEquals(VersionParser.getInstance().getISSWVersion(new byte[] {}, true), "");
    }

    /** */
    @Test
    public void testGetMemInitVersionNull() {
        assertEquals(VersionParser.getInstance().getMemInitVersion(null, true), "");
        assertEquals(VersionParser.getInstance().getMemInitVersion(new byte[] {}, true), "");
    }

    /** */
    @Test
    public void testGetPwrMgtVersionNull() {
        assertEquals(VersionParser.getInstance().getPwrMgtVersion(null, true), "");
        assertEquals(VersionParser.getInstance().getPwrMgtVersion(new byte[] {}, true), "");
    }

    /***/
    @Test
    public void testGetModemVersionNull() {
        assertEquals(VersionParser.getInstance().getModemVersion(null), "");
        assertEquals(VersionParser.getInstance().getModemVersion(new byte[] {}), "");
    }

    /***/
    @Test
    public void testGetIPLVersion() {
        assertEquals(VersionParser.getInstance().getIPLVersion(null), "");
        assertEquals(VersionParser.getInstance().getIPLVersion(new byte[] {}), "");
    }
}
