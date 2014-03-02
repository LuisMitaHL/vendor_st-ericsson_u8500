package com.stericsson.sdk.assembling.internal.u8500;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.zip.CRC32;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.flasharchive.FlashArchiveManifestWriter;
import com.stericsson.sdk.assembling.utilities.StreamCloser;
import com.stericsson.sdk.common.VersionParser;
import com.stericsson.sdk.common.image.ITableOfContentsItem;

/**
 * Helper class for U8500 assembly
 * 
 * @author xolabju
 * 
 */
public final class U8500AssemblerHelper {

    /** Buffer size used in file operations */

    private static final String DEFAULT_TARGET = "/flash0";

    private static final String VERSION_NAME = "version.txt";

    private static int bufferSize = 8 * 1024;

    private static int alignmentSize = 512;

    private U8500AssemblerHelper() {
    }

    /**
     * Sets size of the input buffer.
     * 
     * @param size
     *            Size of the input buffer.
     */
    public static void setBufferSize(int size) {
        bufferSize = size;
    }

    /**
     * Returns size of the input buffer.
     * 
     * @return Size of the input buffer.
     */
    public static int getBufferSize() {
        return bufferSize;
    }

    /**
     * Sets size of the archive alignment.
     * 
     * @param size
     *            Size of the archive alignment.
     */
    public static void setAlignmentSize(int size) {
        alignmentSize = size;
    }

    /**
     * Returns size of the archive alignment.
     * 
     * @return Size of the archive alignment.
     */
    public static int getAlignmentSize() {
        return alignmentSize;
    }

    private static int getPaddingSize(File archiveFile, ZipOutputStream zipOutput, ZipEntry zipEntry)
        throws IOException {
        zipOutput.flush();
        long archiveFileLength = archiveFile.length();

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ZipOutputStream zos = new ZipOutputStream(baos);
        CRC32 crc = new CRC32();

        ZipEntry ze = new ZipEntry(zipEntry);
        ze.setSize(0);
        ze.setCompressedSize(0);
        ze.setCrc(crc.getValue());

        zos.putNextEntry(ze);
        zos.closeEntry();
        archiveFileLength += baos.size();
        zos.close();

        int size = (int) (archiveFileLength % alignmentSize);
        if (size != 0) {
            size = alignmentSize - size;
        }

        return size;
    }

    private static byte[] getPaddingSizeByteArray(int paddingSize) {
        return new byte[] {
            (byte) (paddingSize & 0xFF), (byte) ((paddingSize >>> 8) & 0xFF)};
    }

    /**
     * Writes an flash archive entry as a ZIP archive entry in the specified ZIP output stream.
     * 
     * @param archiveFile
     *            Output archive file
     * @param output
     *            ZIP output stream to write entry to.
     * @param entry
     *            Entry to write to the ZIP output stream.
     * @param filename
     *            the file name
     * @throws AssemblerException
     *             In the case of an assembler related error.
     */
    public static void writeEntry(File archiveFile, ZipOutputStream output, U8500ConfigurationEntry entry,
        String filename) throws AssemblerException {

        File file = new File(filename);
        FileInputStream input = null;

        byte[] buffer = new byte[bufferSize];

        try {

            CRC32 crc = calculateCRC(file);

            ZipEntry zipEntry = new ZipEntry(entry.getName());
            zipEntry.setMethod(ZipEntry.STORED);
            zipEntry.setCrc(crc.getValue());
            zipEntry.setSize(file.length());
            zipEntry.setCompressedSize(file.length());
            zipEntry.setComment(null);
            // Padding data in extra field (local header) for archive alignment
            int paddingSize = getPaddingSize(archiveFile, output, zipEntry);
            zipEntry.setExtra(new byte[paddingSize]);

            output.putNextEntry(zipEntry);

            input = new FileInputStream(file);
            int read = 0;
            while ((read = input.read(buffer, 0, bufferSize)) != -1) {
                output.write(buffer, 0, read);
            }

            output.closeEntry();
            // Padding data size as two byte little-endian value in extra field (file header)
            zipEntry.setExtra(getPaddingSizeByteArray(paddingSize));
        } catch (IOException ioe) {
            throw new AssemblerException(ioe);
        } finally {
            StreamCloser.close(input);
        }
    }

    /**
     * Creates a CRC32 checksum on a specified file.
     * 
     * @param file
     *            File instance to create a CRC32 checksum on.
     * @return A CRC32 instance representing the checksum.
     * @throws IOException
     *             In the case of an I/O related error.
     */
    private static CRC32 calculateCRC(File file) throws IOException {
        byte[] buffer = new byte[bufferSize];
        CRC32 crc = new CRC32();
        FileInputStream input = null;

        try {
            input = new FileInputStream(file);
            int read = 0;
            while ((read = input.read(buffer, 0, bufferSize)) != -1) {
                crc.update(buffer, 0, read);
            }
        } finally {
            StreamCloser.close(input);
        }
        return crc;
    }

    /**
     * Writes the manifest as an entry to the specified ZIP output stream.
     * 
     * @param archiveFile
     *            Output archive file
     * @param output
     *            ZIP output stream to write the manifest entry to.
     * @param entries
     *            the flash archive entries
     * @param entryFileList
     *            the file list
     * @throws AssemblerException
     *             In the case of a assembler related error.
     * @return the written bytes
     */
    public static byte[] writeManifest(File archiveFile, ZipOutputStream output, List<U8500ConfigurationEntry> entries,
        List<String> entryFileList) throws AssemblerException {
        ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
        FlashArchiveManifestWriter.write(entries, entryFileList, byteOutput);

        try {
            CRC32 crc = new CRC32();
            crc.update(byteOutput.toByteArray());

            ZipEntry zipEntry = new ZipEntry(FlashArchiveManifestWriter.DEFAULT_MANIFEST_NAME);
            zipEntry.setCrc(crc.getValue());
            zipEntry.setMethod(ZipEntry.STORED);
            zipEntry.setSize(byteOutput.size());
            zipEntry.setCompressedSize(byteOutput.size());
            zipEntry.setComment(null);
            // Padding data in extra field (local header) for archive alignment
            int paddingSize = getPaddingSize(archiveFile, output, zipEntry);
            zipEntry.setExtra(new byte[paddingSize]);

            output.putNextEntry(zipEntry);

            byteOutput.writeTo(output);
            output.closeEntry();
            // Padding data size as two byte little-endian value in extra field (file header)
            zipEntry.setExtra(getPaddingSizeByteArray(paddingSize));
        } catch (IOException ioe) {
            throw new AssemblerException(ioe);
        }
        return byteOutput.toByteArray();
    }

    /**
     * Creates a temp file in the user's home directory
     * 
     * @param data
     *            the data to write
     * @param fileName
     *            the file name to use
     * @return the created file
     * @throws AssemblerException
     *             if any errors occur
     */
    public static File createTempFile(byte[] data, String fileName) throws AssemblerException {
        FileOutputStream fout = null;
        File file = null;
        try {
            String path = System.getProperty("user.home") + File.separator + fileName;
            file = new File(path);
            file.deleteOnExit();
            fout = new FileOutputStream(file);
            fout.write(data);
        } catch (Exception e) {
            throw new AssemblerException("Failed to write " + file.getAbsolutePath() + ": " + e.getMessage());
        } finally {
            StreamCloser.close(fout);
        }
        return file;
    }

    /**
     * Reads bytes from a file
     * 
     * @param filePath
     *            the file to read
     * @return the bytes read from the file
     * @throws AssemblerException
     *             if any errors occur
     */
    public static byte[] readFromFile(String filePath) throws AssemblerException {
        if (filePath == null) {
            throw new AssemblerException("file path is null");
        }
        FileInputStream fis = null;
        FileChannel fc = null;

        try {
            File file = new File(filePath);

            fis = new FileInputStream(file);
            fc = fis.getChannel();

            ByteBuffer buffer = ByteBuffer.allocate((int) file.length());
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            buffer.rewind();

            fc.read(buffer);
            buffer.rewind();

            return buffer.array();

        } catch (Exception e) {
            throw new AssemblerException(e);
        } finally {
            StreamCloser.close(fc, fis);
        }
    }

    /**
     * @param output
     *            the output stream to use
     * @param image
     *            the image to write
     * @throws AssemblerException
     *             if any assembler related errors occur
     */
    public static void writeImage(FileOutputStream output, U8500BinaryImage image) throws AssemblerException {
        LinkedHashMap<String, byte[]> imageItems = image.getImageItems();
        try {
            if (imageItems == null) {
                throw new AssemblerException("No image items to assemble");
            }
            output.write(image.getTOC().getData());
            for (String key : imageItems.keySet()) {
                output.write(imageItems.get(key));
            }

        } catch (IOException e) {
            throw new AssemblerException(e);
        }

    }

    /**
     * 
     * @param entry
     *            the configuration entry
     * @return the target
     * @throws AssemblerException
     *             on errors
     */
    public static String getTarget(U8500ConfigurationEntry entry) throws AssemblerException {
        ListIterator<U8500ConfigurationEntryAttribute> attributes = entry.getAttributes();
        if (attributes != null) {
            while (attributes.hasNext()) {
                U8500ConfigurationEntryAttribute next = attributes.next();
                if (next != null && next.getName().equalsIgnoreCase(ConfigurationReader.ATTRIBUTE_NAME_TARGET)) {
                    String value = next.getValue();
                    if (value == null) {
                        throw new AssemblerException("Failed to read target for " + entry.getName());
                    }
                    if (value.contains(",")) {
                        String[] split = value.split(",");
                        return split[0];
                    } else {
                        return value;
                    }
                }
            }
        }
        return DEFAULT_TARGET;
    }

    static void attachVersioning(String versionName, File archiveFile, ZipOutputStream zipOutput) throws Exception {
        ZipEntry entry;
        InputStream input = null;
        int bytesRead;
        byte[] buffer = new byte[bufferSize];

        try {
            entry = new ZipEntry(VERSION_NAME);
            if (entry == null) {
                throw new Exception("Could not locate entry: " + versionName);
            }
            entry.setMethod(ZipEntry.STORED);

            // count crc
            CRC32 crc = new CRC32();
            InputStream in = new FileInputStream(VERSION_NAME);
            while ((bytesRead = in.read(buffer, 0, bufferSize)) != -1) {
                crc.update(buffer, 0, bytesRead);
            }
            in.close();
            entry.setCrc(crc.getValue());
            entry.setCompressedSize(new File(VERSION_NAME).length());
            // Padding data in extra field (local header) for archive alignment
            int paddingSize = getPaddingSize(archiveFile, zipOutput, entry);
            entry.setExtra(new byte[paddingSize]);
            zipOutput.putNextEntry(entry);

            input = new FileInputStream(VERSION_NAME);
            while ((bytesRead = input.read(buffer, 0, bufferSize)) != -1) {
                zipOutput.write(buffer, 0, bytesRead);
            }

            zipOutput.closeEntry();
            // Padding data size as two byte little-endian value in extra field (file header)
            entry.setExtra(getPaddingSizeByteArray(paddingSize));
        } catch (Exception e) {
            throw new Exception(e.getMessage() + "\n" + "During signing the entry must be located in the"
                + " unsigned flash archive.");
        } finally {
            StreamCloser.close(input);
        }
    }

    static void updateVersionFile(U8500ConfigurationEntry entry, String filePath, String versionFilePath)
        throws IOException {
        FileInputStream fis = null;
        byte[] payloadBytes = null;
        try {
            File file = new File(filePath);
            fis = new FileInputStream(file);
            payloadBytes = new byte[(int) file.length()];
            int read = fis.read(payloadBytes);
            if (read <= 0) {
                return;
            }
        } finally {
            StreamCloser.close(fis);
        }
        String versionInfo = "";
        U8500ConfigurationEntryAttribute versionId = entry.getAttribute(ConfigurationReader.ATTRIBUTE_VERSION_ID);
        if (versionId != null) {
            VersionParser vp = VersionParser.getInstance();
            if (ITableOfContentsItem.FILENAME_XLOADER.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getXLoaderVersion(payloadBytes, false);
            } else if (ITableOfContentsItem.FILENAME_ISSW.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getISSWVersion(payloadBytes, false);
            } else if (ITableOfContentsItem.FILENAME_MEM_INIT.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getMemInitVersion(payloadBytes, false);
            } else if (ITableOfContentsItem.FILENAME_PWR_MGT.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getPwrMgtVersion(payloadBytes, false);
            } else if (ITableOfContentsItem.FILENAME_MODEM.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getModemVersion(payloadBytes);
            } else if (ITableOfContentsItem.FILENAME_IPL.equalsIgnoreCase(versionId.getValue())) {
                versionInfo = vp.getIPLVersion(payloadBytes);
            }
            FileOutputStream out = null;
            try {
                out = new FileOutputStream(versionFilePath, true);
                out.write(versionInfo.getBytes("UTF-8"));
            } finally {
                StreamCloser.close(out);
            }
        }

    }
}
