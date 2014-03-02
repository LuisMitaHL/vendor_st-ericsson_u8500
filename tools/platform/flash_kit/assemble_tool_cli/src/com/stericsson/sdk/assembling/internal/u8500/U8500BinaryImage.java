package com.stericsson.sdk.assembling.internal.u8500;

import java.io.File;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.LinkedHashMap;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.internal.BasicFileValidator;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * Represents a binary image with TOC and data
 * 
 * @author xolabju
 * 
 */
public class U8500BinaryImage {

    private U8500TableOfContents toc;

    private LinkedHashMap<String, byte[]> imageItems;

    /**
     * Constructor
     */
    public U8500BinaryImage() {
        toc = new U8500TableOfContents();
        imageItems = new LinkedHashMap<String, byte[]>();
    }

    /**
     * Adds a new item to the binary image
     * 
     * @param tocItem
     *            the toc item corresponding to the image data
     * @param data
     *            the image data to add
     * @throws AssemblerException
     *             if length in toc item isn't equal to data.length
     */
    public void addImageItem(U8500TableOfContentsItem tocItem, byte[] data) throws AssemblerException {
        if (tocItem.getSize() != data.length) {
            throw new AssemblerException("Length of data differs from the length stated in TOC item.");
        }
        toc.add(tocItem);
        imageItems.put(tocItem.getFileNameString(), data);
    }

    private int calculateImageSize() {
        int size = 0;
        for (U8500TableOfContentsItem item : toc.getItems()) {
            int neededSpace = item.getStartAddress() + item.getSize();
            if (neededSpace > size) {
                size = neededSpace;
            }
        }
        return size;
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
    public U8500TableOfContents getTOC() {
        return toc;
    }

    /**
     * 
     * Creates a binary image instance from a file. Note that the file must have a table of contents
     * 
     * @param filePath
     *            the file to read
     * @return a binary image instance
     * @throws AssemblerException
     *             on assembler errors
     */
    public static U8500BinaryImage createFromFile(String filePath) throws AssemblerException {
        if (filePath == null) {
            throw new AssemblerException("file path is null");
        }
        BasicFileValidator.validateInputFile(filePath);
        U8500BinaryImage image = new U8500BinaryImage();

        FileInputStream fis = null;
        FileChannel fc = null;

        try {
            ByteBuffer buffer = null;
            File file = new File(filePath);
            fis = new FileInputStream(file);
            FileChannel channel = fis.getChannel();

            byte[] tocData = new byte[U8500TableOfContents.SIZE];
            int read = fis.read(tocData);
            if (read < U8500TableOfContents.SIZE) {
                throw new Exception("Failed to read TOC. Tried to read " + U8500TableOfContents.SIZE + ", got " + read);
            }
            U8500TableOfContents toc = U8500TableOfContents.createTableOfContents(tocData);

            for (U8500TableOfContentsItem item : toc.getItems()) {
                buffer = ByteBuffer.allocate(item.getSize());
                buffer.order(ByteOrder.LITTLE_ENDIAN);
                channel.read(buffer, item.getStartAddress());
                image.addImageItem(item, buffer.array());
            }

        } catch (Exception e) {
            throw new AssemblerException(e);
        } finally {
            StreamCloser.close(fc, fis);
        }

        return image;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        StringBuffer buffer = new StringBuffer();

        buffer.append("Image size: " + calculateImageSize() + " bytes\n");
        buffer.append("Table of Contents:\n");
        buffer.append(toc.toString());

        return buffer.toString();
    }

    /**
     * 
     * @return the length of the image
     */
    public int getLength() {
        int length = toc.getData().length;
        for (String key : imageItems.keySet()) {
            length += imageItems.get(key).length;
        }
        return length;
    }

    /**
     * 
     * @return the image items
     */
    public LinkedHashMap<String, byte[]> getImageItems() {
        return imageItems;
    }
}
