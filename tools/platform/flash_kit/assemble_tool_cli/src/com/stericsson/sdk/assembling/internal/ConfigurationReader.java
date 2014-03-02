package com.stericsson.sdk.assembling.internal;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Vector;

import org.xml.sax.helpers.DefaultHandler;

/**
 * ConfigurationReader is an abstract base class which also extends the
 * org.sax.xml.helpers.DefaultHandler class to provide XML parsing capabilities.
 *
 * The ConfigurationReader also implements a static utility method to parse a file list file into a
 * List data structure.
 *
 * This class also holds information about the XML elements used in the configuration file.
 *
 * @author etomjun
 */
public abstract class ConfigurationReader extends DefaultHandler {
    /** Name space location */
    protected static final String URI_NAME = "TBD";

    /** Element name for a flash archive */
    public static final String ELEMENT_NAME_FLASH_ARCHIVE = "flash_archive";

    /** Element name for a binary image */
    public static final String ELEMENT_NAME_RAM_IMAGE = "ram_image";

    /** Element name for a preflash image */
    public static final String ELEMENT_NAME_PREFLASH_IMAGE = "preflash_image";

    /** Element name for a preflash archive */
    public static final String ELEMENT_NAME_PREFLASH_ARCHIVE = "preflash_archive";

    /** Element name for a source */
    protected static final String SOURCE_ELEMENT_NAME = "source";

    /** Element name for a output */
    protected static final String OUTPUT_ELEMENT_NAME = "output";

    /** Element name for a entry */
    protected static final String ENTRY_ELEMENT_NAME = "entry";

    /** Element name for a profile */
    protected static final String PROFILE_ELEMENT_NAME = "profile";

    /** Element name for a attribute */
    protected static final String ATTRIBUTE_ELEMENT_NAME = "attribute";

    /** Element name for skipping of unused file system image blocks */
    protected static final String SKIP_ELEMENT_NAME = "skip";

    /** Attribute name for a entry type */
    public static final String ENTRY_TYPE_ATTRIBUTE_NAME = "type";

    /** Attribute name for a entry name */
    public static final String ENTRY_NAME_ATTRIBUTE_NAME = "name";

    /** Attribute name for a attribute name */
    public static final String ATTRIBUTE_NAME_ATTRIBUTE_NAME = "name";

    /** Attribute name for a block size */
    public static final String BLOCK_SIZE_ATTRIBUTE_NAME = "block_size";

    /** Attribute name for a block pattern */
    public static final String BLOCK_PATTERN_ATTRIBUTE_NAME = "block_pattern";

    /** */
    public static final String SUBTYPE_BOOTIMAGE = "bootimage";

    /** */
    public static final String SUBTYPE_IMAGE = "image";

    /** Type for preflash image */
    public static final String SUBTYPE_PREFLASH = "preflashimage";

    /** Type for flash archive */
    public static final String SUBTYPE_FLASHARCHIVE = "flasharchive";

    /** Type for gdfs */
    public static final String SUBTYPE_GDFS = "gdfs";

    /** Type for CRKC */
    public static final String SUBTYPE_CRKC = "crkc";

    /**  */
    public static final String SUBTYPE_INIT_AREA = "initilized-area";

    /** */
    public static final String ATTRIBUTE_NAME_TARGET = "Target";

    /** */
    public static final String ATTRIBUTE_SIZE_MODE = "SIZE-MODE";

    /** */
    public static final String ATTRIBUTE_NAME_SIZE = "Size";

    /** */
    public static final String ATTRIBUTE_TOC_ID = "TOC-ID";

    /** */
    public static final String ATTRIBUTE_VERSION_ID = "VERSION-ID";

    /** */
    public static final String ATTRIBUTE_TOC_LOAD_ADDRESS = "TOC-LoadAddress";

    /** */
    public static final String ATTRIBUTE_TOC_ENTRY_POINT = "TOC-EntryPoint";

    /** */
    public static final String ATTRIBUTE_TOC_FLAGS = "TOC-Flags";

    private static final String FILELIST_COMMENT_PREFIX = "#";

    /** Attribute name for a profile name */
    public static final String PROFILE_PATH_ATTRIBUTE_NAME = "path";

    /** */
    public static final String[] FILELIST_WILDCARDS = new String[] {
        "*", "?"};

    /**
     * @param fileListFilename
     *            Path/filename to a file list file.
     * @return A list of strings.
     * @throws IOException
     *             If an I/O error occurred
     */
    public static List<String> getFileListFilenames(final String fileListFilename) throws IOException {
        List<String> result = new Vector<String>();
        BufferedReader lineReader = null;

        try {
            lineReader = new BufferedReader(new InputStreamReader(new FileInputStream(fileListFilename), "UTF-8"));
            String line = null;
            while ((line = lineReader.readLine()) != null) {
                if (!line.startsWith(FILELIST_COMMENT_PREFIX) && !line.trim().equalsIgnoreCase("")) {
                    result.add(line);
                }
            }
        } finally {
            lineReader.close();
        }

        return result;
    }

}
