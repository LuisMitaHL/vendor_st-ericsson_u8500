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
import com.stericsson.sdk.assembling.utilities.FileStreamUtilities;
import com.stericsson.sdk.assembling.utilities.HexUtilities;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * Provides methods for writing minimized sparse EXT4 file system image entries.
 * 
 * @author pkutac01
 * 
 */
public final class MinimizedSparseEXT4ImageEntryWriter {

    private static final int HEADER_SIZE = 28;

    private static final byte[] HEADER_MAGIC_BYTES = {
        0x3A, (byte) 0xFF, 0x26, (byte) 0xED};

    private static final int CHUNK_HEADER_SIZE = 12;

    private static final int RAW_DATA_CHUNK = 0xCAC1;

    // private static final int FILL_CHUNK = 0xCAC2;

    // private static final int EMPTY_CHUNK = 0xCAC3;

    // private static final int CRC_CHUNK = 0xCAC4;

    private static final String SPARSE_IMAGE_CONTENT_TYPE = "x-steflash/sparse-image";

    private MinimizedSparseEXT4ImageEntryWriter() {

    }

    /**
     * Checks whether the specified file is sparse EXT4 image or not.
     * 
     * @param filePath
     *            Input file path.
     * @return true if the specified file is sparse EXT4 image, otherwise false.
     * @throws AssemblerException
     *             Assembler exception.
     */
    public static boolean isSparseEXT4Image(String filePath) throws AssemblerException {
        File file = new File(filePath);
        if (file.length() < HEADER_SIZE) {
            return false;
        }
        FileInputStream input = null;

        try {
            input = new FileInputStream(file);

            byte[] buffer = new byte[HEADER_MAGIC_BYTES.length];
            int read = input.read(buffer);
            if (read < HEADER_MAGIC_BYTES.length || !Arrays.equals(buffer, HEADER_MAGIC_BYTES)) {
                return false;
            }
        } catch (IOException e) {
            throw new AssemblerException(e);
        } finally {
            StreamCloser.close(input);
        }

        return true;
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
     * @param filePath
     *            Input file path.
     * @return List of configuration entries created from original configuration entry by skipping
     *         unused blocks from file system image.
     * @throws AssemblerException
     *             In the case of an assembler related error.
     */
    public static List<U8500ConfigurationEntry> writeEntry(File archiveFile, ZipOutputStream output,
        U8500ConfigurationEntry entry, String filePath) throws AssemblerException {
        File file = new File(filePath);
        FileInputStream input = null;
        File tempFile = null;
        FileOutputStream tempOutput = null;

        List<U8500ConfigurationEntry> entries = new LinkedList<U8500ConfigurationEntry>();

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

            byte[] buffer = new byte[HEADER_MAGIC_BYTES.length];
            FileStreamUtilities.readByteArrayFully(input, buffer);
            // major version (16-bit unsigned integer)
            long skip = FileStreamUtilities.read16BitUnsignedInteger(input);
            // minor version (16-bit unsigned integer)
            skip = FileStreamUtilities.read16BitUnsignedInteger(input);
            // major revision (16-bit unsigned integer)
            skip = FileStreamUtilities.read16BitUnsignedInteger(input);
            // minor revision (16-bit unsigned integer)
            skip = FileStreamUtilities.read16BitUnsignedInteger(input);
            // block size (32-bit unsigned integer)
            long blockSize = FileStreamUtilities.read32BitUnsignedInteger(input);
            // block count (32-bit unsigned integer)
            skip = FileStreamUtilities.read32BitUnsignedInteger(input);
            // chunk count (32-bit unsigned integer)
            long chunkCount = FileStreamUtilities.read32BitUnsignedInteger(input);
            // CRC (32-bit unsigned integer)
            skip = FileStreamUtilities.read32BitUnsignedInteger(input);

            U8500ConfigurationEntry tempEntry = null;

            boolean first = true;
            boolean empty = true;
            for (int i = 0; i < chunkCount; i++) {
                // chunk type (16-bit unsigned integer)
                int chunkType = FileStreamUtilities.read16BitUnsignedInteger(input);
                // reserved (16-bit unsigned integer)
                skip = FileStreamUtilities.read16BitUnsignedInteger(input);
                // chunk block count (32-bit unsigned integer)
                long chunkBlockCount = FileStreamUtilities.read32BitUnsignedInteger(input);
                // chunk size (32-bit unsigned integer)
                long chunkSize = FileStreamUtilities.read32BitUnsignedInteger(input);
                long chunkDataSize = chunkSize - CHUNK_HEADER_SIZE;

                switch (chunkType) {
                    case RAW_DATA_CHUNK:
                        buffer = new byte[(int) chunkDataSize];
                        FileStreamUtilities.readByteArrayFully(input, buffer);

                        if (empty) {
                            // If the previous block was empty and the current one isn't then new
                            // temporary file is created.
                            // Until another empty block arrives, all non-empty blocks will be
                            // written to this temporary file.
                            tempFile = File.createTempFile("temp", null);
                            tempOutput = new FileOutputStream(tempFile);
                            tempOutput.write(buffer);

                            tempEntry = createChunkEntry(entry, targetPath, targetOffset, first);

                            entries.add(tempEntry);
                            first = false;
                            empty = false;
                        } else {
                            // Previous block wasn't empty and the current one isn't too.
                            // The current block is written to the temporary file.
                            tempOutput.write(buffer);
                        }
                        break;
                    default:
                        skip = chunkDataSize;
                        FileStreamUtilities.skip(input, skip);

                        if (!empty) {
                            // Previous block wasn't empty but the current one is.
                            // Temporary file is closed and it is added to the archive as a new
                            // entry.
                            tempOutput.flush();
                            tempOutput.close();

                            U8500AssemblerHelper.writeEntry(archiveFile, output, tempEntry, tempFile.getAbsolutePath());

                            tempFile.delete();
                            empty = true;
                        }
                        break;
                }

                targetOffset += chunkBlockCount * blockSize;
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
        } catch (IOException e) {
            throw new AssemblerException(e);
        } finally {
            StreamCloser.close(input);
            StreamCloser.close(tempOutput);
        }

        return entries;
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
                + HexUtilities.toHexString((int) targetOffset, true), SPARSE_IMAGE_CONTENT_TYPE);
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
