package com.stericsson.sdk.assembling.internal.flasharchive;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.zip.ZipOutputStream;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.u8500.U8500AssemblerHelper;
import com.stericsson.sdk.assembling.internal.u8500.U8500ConfigurationEntry;
import com.stericsson.sdk.assembling.internal.u8500.U8500ConfigurationEntryAttribute;
import com.stericsson.sdk.assembling.utilities.HexUtilities;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * Provides methods for writing minimized file system image entries.
 * 
 * @author pkutac01
 * 
 */
public final class MinimizedJFFSImageEntryWriter {

    private static final String JUNK_AREA_ENTRY_NAME = "FileSystemArea";

    private static final String JUNK_AREA_CONTENT_TYPE = "x-steflash/junked-area";

    private MinimizedJFFSImageEntryWriter() {

    }

    /**
     * Writes a flash archive entry for file system image with unused blocks skipped as a series of
     * ZIP archive entries in the specified ZIP output stream.
     * 
     * @param archiveFile
     *            Output archive file.
     * @param output
     *            ZIP output stream.
     * @param entry
     *            Original configuration entry for input file.
     * @param filename
     *            Input file name.
     * @return List of configuration entries created from original configuration entry by skipping
     *         unused blocks from file system image.
     * @throws AssemblerException
     *             In the case of an assembler related error.
     */
    public static List<U8500ConfigurationEntry> writeEntry(File archiveFile, ZipOutputStream output,
        U8500ConfigurationEntry entry, String filename) throws AssemblerException {

        File file = new File(filename);
        FileInputStream input = null;

        File tempFile = null;
        FileOutputStream tempOutput = null;

        List<U8500ConfigurationEntry> entries = new LinkedList<U8500ConfigurationEntry>();

        int size = HexUtilities.hexStringToInt(entry.getBlockSize());
        byte[] buffer = new byte[size];
        byte[] pattern = HexUtilities.hexStringToByteArray(entry.getBlockPattern());
        byte[] patternBuffer = createPatternBuffer(size, pattern);

        String target = entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_NAME_TARGET);
        if (target == null) {
            throw new AssemblerException("Missing configuration entry attribute: "
                + ConfigurationReader.ATTRIBUTE_NAME_TARGET);
        }
        int index = target.indexOf(',');
        String targetPath = target.substring(0, index).trim();
        long targetOffset = HexUtilities.hexStringToLong(target.substring(index + 1).trim());

        try {
            input = new FileInputStream(file);

            U8500ConfigurationEntry tempEntry = createJunkAreaEntry(targetPath, targetOffset, file.length());
            entries.add(tempEntry);

            boolean first = true;
            boolean empty = true;
            int read = 0;
            while ((read = input.read(buffer, 0, buffer.length)) != -1) {
                if (empty) {
                    if (!Arrays.equals(buffer, patternBuffer)) {
                        // If the previous block was empty and the current one isn't then new
                        // temporary file is created.
                        // Until another empty block arrives, all non-empty blocks will be written
                        // to this temporary file.
                        tempFile = File.createTempFile("temp", null);
                        tempOutput = new FileOutputStream(tempFile);
                        tempOutput.write(buffer, 0, read);

                        tempEntry = createChunkEntry(entry, targetPath, targetOffset, first);

                        entries.add(tempEntry);
                        first = false;
                        empty = false;
                    }
                } else {
                    if (!Arrays.equals(buffer, patternBuffer)) {
                        // Previous block wasn't empty and the current one isn't too.
                        // The current block is written to the temporary file.
                        tempOutput.write(buffer, 0, read);
                    } else {
                        // Previous block wasn't empty but the current one is.
                        // Temporary file is closed and it is added to the archive as a new entry.
                        tempOutput.flush();
                        tempOutput.close();

                        U8500AssemblerHelper.writeEntry(archiveFile, output, tempEntry, tempFile.getAbsolutePath());

                        tempFile.delete();
                        empty = true;
                    }
                }
                targetOffset += read;
            }
            if (!empty) {
                // Last block wasn't empty.
                // Temporary file is closed and it is added to the archive as a new entry.
                tempOutput.flush();
                tempOutput.close();

                U8500AssemblerHelper.writeEntry(archiveFile, output, tempEntry, tempFile.getAbsolutePath());

                tempFile.delete();
                empty = true;
            }
        } catch (IOException ioe) {
            throw new AssemblerException(ioe);
        } finally {
            StreamCloser.close(input);
            StreamCloser.close(tempOutput);
        }

        return entries;
    }

    private static byte[] createPatternBuffer(int size, byte[] pattern) {
        byte[] patternBuffer = new byte[size];

        int count = patternBuffer.length / pattern.length;
        for (int i = 0; i < count; i++) {
            System.arraycopy(pattern, 0, patternBuffer, i * pattern.length, pattern.length);
        }
        count = patternBuffer.length % pattern.length;
        for (int i = 0; i < count; i++) {
            patternBuffer[patternBuffer.length - count + i] = pattern[i];
        }

        return patternBuffer;
    }

    /**
     * Creates junk area configuration (manifest file) entry which represents area that is supposed
     * to be erased.
     * 
     * @param targetPath
     *            Target path to be written in the manifest file for the junk area entry.
     * @param targetOffset
     *            Target offset to be written in the manifest file for the junk area entry.
     * @param size
     *            Size of the junk area to be erased.
     * @return Junk area configuration entry.
     */
    private static U8500ConfigurationEntry createJunkAreaEntry(String targetPath, long targetOffset, long size) {
        U8500ConfigurationEntry junkAreaEntry =
            new U8500ConfigurationEntry(JUNK_AREA_ENTRY_NAME, JUNK_AREA_CONTENT_TYPE);
        junkAreaEntry.setSource(null);

        junkAreaEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET,
            targetPath + ", " + HexUtilities.toHexString((int) targetOffset, true)));
        junkAreaEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE,
            HexUtilities.toHexString((int) size, true)));

        return junkAreaEntry;
    }

    /**
     * Creates chunk configuration (manifest file) entry which represents part of the complete file.
     * Minimized file system image is stored as several chunk archive entries, because some parts of
     * it are skipped and not stored in the archive.
     * 
     * @param originalEntry
     *            Original configuration entry representing complete file.
     * @param targetPath
     *            Target path to be written in the manifest file for the chunk entry.
     * @param targetOffset
     *            Target offset to be written in the manifest file for the chunk entry.
     * @param firstEntry
     *            Only first chunk configuration entry contains all manifest attributes from the
     *            original configuration entry, following chunk configuration entries have only
     *            target path and offset.
     * @return Chunk configuration entry.
     */
    private static U8500ConfigurationEntry createChunkEntry(U8500ConfigurationEntry originalEntry, String targetPath,
        long targetOffset, boolean firstEntry) {
        U8500ConfigurationEntry chunkEntry =
            new U8500ConfigurationEntry(originalEntry.getName() + "__EXTENT__"
                + HexUtilities.toHexString((int) targetOffset, true), originalEntry.getType());
        chunkEntry.setSource(originalEntry.getSource());

        ListIterator<U8500ConfigurationEntryAttribute> iterator = originalEntry.getAttributes();
        while (iterator.hasNext()) {
            U8500ConfigurationEntryAttribute attribute = iterator.next();
            if (attribute.getName().equalsIgnoreCase(ConfigurationReader.ATTRIBUTE_NAME_TARGET)) {
                chunkEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET,
                    targetPath + ", " + HexUtilities.toHexString((int) targetOffset, true)));
            } else if (firstEntry) {
                chunkEntry.addAttribute(attribute);
            }
        }

        return chunkEntry;
    }

}
