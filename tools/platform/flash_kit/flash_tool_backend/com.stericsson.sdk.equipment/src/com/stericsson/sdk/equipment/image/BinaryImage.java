package com.stericsson.sdk.equipment.image;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.LinkedHashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.VersionParser;

/**
 * Represents a binary image with TOC and data
 * 
 * @author xolabju
 * 
 */
public class BinaryImage {

    private static Logger logger = Logger.getLogger(BinaryImage.class.getName());

    private TableOfContents toc;

    private LinkedHashMap<String, byte[]> imageItems;

    /**
     * Constructor
     */
    public BinaryImage() {
        toc = new TableOfContents();
        imageItems = new LinkedHashMap<String, byte[]>();
    }

    /**
     * Adds a new item to the binary image
     * 
     * @param tocItem
     *            the toc item corresponding to the image data
     * @param data
     *            the image data to add
     * @throws Exception
     *             if length in toc item isn't equal to data.length
     */
    public void addImageItem(TableOfContentsItem tocItem, byte[] data) throws Exception {
        if (tocItem.getSize() != data.length) {
            throw new Exception("Length of data differs from the length stated in TOC item.");
        }
        toc.add(tocItem);
        imageItems.put(tocItem.getFileNameString(), data);
    }

    /**
     * 
     * @param tocFileName
     *            the TOC file name
     * @return the image data referred to by the TOC item with the specified name
     */
    public byte[] getImageData(String tocFileName) {
        return imageItems.get(tocFileName);
    }

    /**
     * 
     * @return the table of contents
     */
    public TableOfContents getTOC() {
        return toc;
    }

    /**
     * 
     * Creates a binary image instance from a file. Note that the file must have a table of contents
     * 
     * @param filePath
     *            the file to read
     * @return a binary image instance
     * @throws IOException
     *             on assembler errors
     */
    public static BinaryImage createFromFile(String filePath) throws IOException {
        if (filePath == null) {
            throw new IOException("file path is null");
        }

        BinaryImage image = new BinaryImage();

        FileInputStream fis = null;
        FileChannel fc = null;
        ByteBuffer buffer = null;

        try {

            File file = new File(filePath);

            if (!file.exists()) {
                throw new IOException("Could not find file " + filePath + ".");
            }

            if (file.length() == 0) {
                throw new IOException(filePath + " is 0 bytes.");
            }

            fis = new FileInputStream(file);
            fc = fis.getChannel();

            byte[] tocData = new byte[TableOfContents.SIZE];
            int read = fis.read(tocData);
            if (read < TableOfContents.SIZE) {
                throw new Exception("Failed to read TOC. Tried to read " + TableOfContents.SIZE + ", got " + read);
            }
            TableOfContents toc = TableOfContents.createTableOfContents(tocData);

            for (TableOfContentsItem item : toc.getItems()) {
                buffer = ByteBuffer.allocate(item.getSize());
                buffer.order(ByteOrder.LITTLE_ENDIAN);
                fc.read(buffer, item.getStartAddress());
                image.addImageItem(item, buffer.array());
            }

        } catch (Exception e) {
            throw new IOException(e.getMessage());
        } finally {
            if (fc != null) {
                try {
                    fc.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }

        return image;
    }

    /**
     * @return binary images versions
     */
    public String parseVersions() {
        StringBuffer versionsBuffer = new StringBuffer();
        String version = "";

        version = VersionParser.getInstance().getISSWVersion(imageItems.get(TableOfContentsItem.FILENAME_ISSW), false);
        appendVersion(version, versionsBuffer);

        version =
            VersionParser.getInstance().getXLoaderVersion(imageItems.get(TableOfContentsItem.FILENAME_XLOADER), false);
        appendVersion(version, versionsBuffer);

        version =
            VersionParser.getInstance().getMemInitVersion(imageItems.get(TableOfContentsItem.FILENAME_MEM_INIT), false);
        appendVersion(version, versionsBuffer);

        version =
            VersionParser.getInstance().getPwrMgtVersion(imageItems.get(TableOfContentsItem.FILENAME_PWR_MGT), false);
        appendVersion(version, versionsBuffer);

        version = VersionParser.getInstance().getModemVersion(imageItems.get(TableOfContentsItem.FILENAME_MODEM));
        appendVersion(version, versionsBuffer);

        version = VersionParser.getInstance().getIPLVersion(imageItems.get(TableOfContentsItem.FILENAME_IPL));
        appendVersion(version, versionsBuffer);

        return versionsBuffer.toString();
    }

    private void appendVersion(String version, StringBuffer versionsBuffer) {
        if (!version.equals("")) {
            versionsBuffer.append(version);
            versionsBuffer.append("\n");
            version = "";
        }
    }
}
