package com.stericsson.sdk.assembling.internal.u8500;

import java.util.List;
import java.util.ListIterator;
import java.util.Vector;

/**
 * The U8500ConfigurationEntry class is used to hold information about one configuration entry.
 * 
 * @author xtomlju
 */
public class U8500ConfigurationEntry {
    /** Entry name */
    private String name;

    /** Entry source */
    private String source;

    /** Entry type */
    private String type;

    /** Whether to skip unused blocks from file system image entry or not */
    private boolean skipUnusedBlocks = false;

    /** File system image entry block size for unused blocks skipping */
    private String blockSize;

    /** File system image entry block pattern for unused blocks skipping */
    private String blockPattern;

    /** List of entry attributes */
    private List<U8500ConfigurationEntryAttribute> attributes;

    /**
     * @param n
     *            Entry name
     * @param t
     *            Entry type
     */
    public U8500ConfigurationEntry(String n, String t) {
        attributes = new Vector<U8500ConfigurationEntryAttribute>();
        name = n;
        type = t;
    }

    /**
     * @param attribute
     *            Entry attribute
     */
    public void addAttribute(U8500ConfigurationEntryAttribute attribute) {
        attributes.add(attribute);
    }

    /**
     * @param s
     *            Entry source
     */
    public void setSource(String s) {
        source = s;
    }

    /**
     * @return Entry name
     */
    public String getName() {
        return name;
    }

    /**
     * @return Entry source
     */
    public String getSource() {
        return source;
    }

    /**
     * @return Entry type
     */
    public String getType() {
        return type;
    }

    /**
     * Sets skipping of unused blocks from file system image entry.
     * 
     * @param skip
     *            true to skip unused blocks, otherwise false.
     */
    public void setSkipUnusedBlocks(boolean skip) {
        skipUnusedBlocks = skip;
    }

    /**
     * Returns true when skipping of unused blocks from file system image entry is enabled,
     * otherwise false.
     * 
     * @return true when skipping of unused blocks from file system image entry is enabled,
     *         otherwise false.
     */
    public boolean isSkipUnusedBlocks() {
        return skipUnusedBlocks;
    }

    /**
     * Sets file system image entry block size for unused blocks skipping.
     * 
     * @param size
     *            File system image entry block size for unused blocks skipping.
     */
    public void setBlockSize(String size) {
        blockSize = size;
    }

    /**
     * Returns file system image entry block size for unused blocks skipping.
     * 
     * @return File system image entry block size for unused blocks skipping.
     */
    public String getBlockSize() {
        return blockSize;
    }

    /**
     * Sets file system image entry block pattern for unused blocks skipping.
     * 
     * @param pattern
     *            File system image entry block pattern for unused blocks skipping.
     */
    public void setBlockPattern(String pattern) {
        blockPattern = pattern;
    }

    /**
     * Returns file system image entry block pattern for unused blocks skipping.
     * 
     * @return File system image entry block pattern for unused blocks skipping.
     */
    public String getBlockPattern() {
        return blockPattern;
    }

    /**
     * @return Iterator to entry attributes
     */
    public ListIterator<U8500ConfigurationEntryAttribute> getAttributes() {
        return attributes.listIterator();
    }

    /**
     * @return the attribute with the specified name, or null if it doesn't exist
     * @param attrName
     *            the name of the attribute
     */
    public U8500ConfigurationEntryAttribute getAttribute(String attrName) {
        for (U8500ConfigurationEntryAttribute attr : attributes) {
            if (attr.getName().equals(attrName)) {
                return attr;
            }
        }
        return null;
    }

    /**
     * @return the value of the attribute with the specified name, or null if it doesn't exist
     * @param attrName
     *            the name of the attribute
     */
    public String getAttributeValue(String attrName) {
        for (U8500ConfigurationEntryAttribute attr : attributes) {
            if (attr.getName().equals(attrName)) {
                return attr.getValue();
            }
        }
        return null;
    }
}
