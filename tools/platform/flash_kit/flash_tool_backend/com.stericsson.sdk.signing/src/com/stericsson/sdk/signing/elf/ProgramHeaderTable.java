package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.LinkedList;
import java.util.List;

/**
 * @author etomjun
 */
public final class ProgramHeaderTable {

    /**
     * @param input
     *            TBD
     * @param header
     *            TBD
     * @return TBD
     * @throws IOException
     *             TBD
     */
    public static List<ProgramHeader> create(FileChannel input, FileHeader header) throws IOException {
        input.position(header.getProgramHeaderOffset());
        List<ProgramHeader> entries = new LinkedList<ProgramHeader>();
        for (int i = 0; i < header.getProgramHeaderNumOfEntries(); i++) {
            ProgramHeader entry = new ProgramHeader();
            entry.read(input, header);
            entries.add(entry);
        }
        return entries;
    }

    /**
     * Private constructor
     */
    private ProgramHeaderTable() {

    }

}
