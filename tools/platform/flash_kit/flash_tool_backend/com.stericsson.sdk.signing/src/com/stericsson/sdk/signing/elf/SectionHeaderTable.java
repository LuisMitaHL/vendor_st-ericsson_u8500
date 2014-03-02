package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

/**
 * @author etomjun
 */
public final class SectionHeaderTable {
    /**
     * This method creates a list of section header table entries from an ELF file.
     * 
     * No checking that input is in ELF format is done.
     * 
     * This method will also associate a string name to each section if string table section is
     * available in ELF input file.
     * 
     * @param input
     *            A FileChannel instance to read data from
     * @param header
     *            An ELF file header instance that specifies where in file to read information.
     * @throws IOException
     *             TBD
     * @return A list of SectionHeaderTableEntry instances representing the section header table.
     */
    public static List<SectionHeader> create(FileChannel input, FileHeader header) throws IOException {
        input.position(header.getSectionHeaderOffset());
        // Create a result set of SectionHeaderTableEntry elements
        List<SectionHeader> entries = new LinkedList<SectionHeader>();
        for (int i = 0; i < header.getSectionHeaderNumOfEntries(); i++) {
            SectionHeader entry = new SectionHeader();
            entry.read(input, header);
            entries.add(entry);
        }

        // Extract section name information
        SectionHeader entry = entries.get(header.getSectionHeaderStringTableIndex());
        input.position(entry.getOffset());
        byte[] stringData = new byte[(int) entry.getSize()];
        ByteBuffer buffer = ByteBuffer.wrap(stringData);
        // Make sure we fill buffer fully from file channel input
        while (buffer.remaining() > 0) {
            input.read(buffer);
        }
        Iterator<SectionHeader> i = entries.iterator();
        while (i.hasNext()) {
            entry = i.next();
            int j = entry.getName();
            int len = 0;
            while (j < stringData.length) {
                if (stringData[j] == 0) {
                    break;
                }
                j++;
                len++;
            }

            if (len > 0) {
                entry.setStringName(new String(stringData, entry.getName(), len, "UTF-8"));
            } else {
                entry.setStringName("");
            }
        }
        return entries;
    }

    /**
     * This utility method will iterate through a list of section header table entries and look for
     * a specified name. If entry was found with name equal (compareTo() == 0) to specified it is
     * added to an internal list. An iterator for this list is returned.
     * 
     * @param entries
     *            A list of section header table entries
     * @param name
     *            The name of section to look for.
     * @return An iterator to a list containing section header table entries with same name as
     *         specified.
     */
    public static Iterator<SectionHeader> getEntriesFromName(List<SectionHeader> entries, String name) {
        return getEntriesFromNames(entries, new String[] {
            name});
    }

    /**
     * @param entries
     *            TBD
     * @param names
     *            TBD
     * @return TBD
     */
    public static Iterator<SectionHeader> getEntriesFromNames(List<SectionHeader> entries, String[] names) {
        List<SectionHeader> filteredEntries = new LinkedList<SectionHeader>();
        Iterator<SectionHeader> i = entries.iterator();
        while (i.hasNext()) {
            SectionHeader entry = i.next();
            for (int j = 0; j < names.length; j++) {
                if (entry.getStringName().compareTo(names[j]) == 0) {
                    filteredEntries.add(entry);
                    break;
                }
            }
        }
        return filteredEntries.iterator();
    }

    /**
     * Constructor
     */
    private SectionHeaderTable() {
    }
}
