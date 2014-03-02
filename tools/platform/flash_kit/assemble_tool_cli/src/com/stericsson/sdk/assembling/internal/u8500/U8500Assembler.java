package com.stericsson.sdk.assembling.internal.u8500;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import java.util.regex.Pattern;

import com.stericsson.sdk.assembling.AbstractAssembler;
import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class U8500Assembler extends AbstractAssembler {

    /** */
    protected List<U8500ConfigurationEntry> entries;

    /** */
    protected List<String> entryFileList;

    /**
     * Assures that all files in the file list has corresponding configuration settings.
     * 
     * @throws AssemblerException
     *             If not all files in file list has corresponding configuration settings.
     */
    protected void matchFileList() throws AssemblerException {
        List<String> toAdd = new ArrayList<String>();
        List<String> toRemove = new ArrayList<String>();
        ListIterator<String> i = entryFileList.listIterator();
        while (i.hasNext()) {
            String entryFilePath = i.next();
            String entryFileName = new File(entryFilePath).getName();

            String symbolicFilename = extractSymbolicFilename(entryFilePath);

            ListIterator<U8500ConfigurationEntry> j = entries.listIterator();
            boolean found = false;
            while (j.hasNext()) {
                U8500ConfigurationEntry next = j.next();

                if (symbolicFilename == null) {
                    if (isWildCardPattern(entryFileName)) {
                        String match = matchPattern(entryFileName, new File(entryFilePath).getParentFile());
                        if (match != null && new File(match).getName().equalsIgnoreCase(next.getSource())) {
                            toAdd.add(match);
                            toRemove.add(entryFilePath);
                            found = true;
                            break;
                        }
                    } else if (entryFileName.equalsIgnoreCase(next.getSource())) {
                        found = true;
                        break;
                    }
                } else if (symbolicFilename.equalsIgnoreCase(next.getSource())) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw new AssemblerException("Entry settings for " + entryFilePath
                    + " could not be found in configuration");
            }
        }
        updateFileList(toRemove, toAdd);
    }

    private String matchPattern(final String pattern, File parentFile) throws AssemblerException {
        if (pattern == null || parentFile == null) {
            return null;
        }
        FileFilter filter = new FileFilter() {

            public boolean accept(File pathname) {
                return pathname.isFile() && matchesWildCard(pattern, pathname.getName());
            }
        };
        File[] list = parentFile.listFiles(filter);
        if (list != null) {
            if (list.length == 1) {
                return list[0].getPath();
            } else if (list.length > 1) {
                throw new AssemblerException("Multiple matches for " + pattern + " in " + parentFile.getAbsolutePath());
            }
        }
        return null;
    }

    private boolean isWildCardPattern(String entryFileName) {
        for (String wildcard : ConfigurationReader.FILELIST_WILDCARDS) {
            if (entryFileName.contains(wildcard)) {
                return true;
            }
        }
        return false;
    }

    private boolean matchesWildCard(String wildCardPattern, String filename) {
        return Pattern.matches(wildCardPattern.replace("*", ".*").replace("?", "."), filename);
    }

    /**
     * 
     * @param filename
     *            TBD
     * @throws AssemblerException
     *             TBD
     * @return TBD
     */
    protected String extractPathToFile(String filename) throws AssemblerException {
        String result = filename;
        int p = filename.indexOf('=');

        if (p > 0) {
            result = filename.substring(p + 1).trim();
        }
        if (isWildCardPattern(result)) {
            String match = matchPattern(new File(result).getName(), new File(result).getParentFile());
            if (match == null) {
                throw new AssemblerException("No match found for " + result + " in " + new File(result).getParentFile());
            }
            result = match;
        }
        return result;
    }

    /**
     * 
     * @param filename
     *            TBD
     * @return TBD
     */
    protected String extractSymbolicFilename(String filename) {
        String result = null;
        int p = filename.indexOf('=');

        if (p > 0) {
            result = filename.substring(0, p).trim();
        }

        return result;
    }

    private void updateFileList(List<String> toRemove, List<String> toAdd) {
        entryFileList.removeAll(toRemove);
        entryFileList.addAll(toAdd);
    }

}
