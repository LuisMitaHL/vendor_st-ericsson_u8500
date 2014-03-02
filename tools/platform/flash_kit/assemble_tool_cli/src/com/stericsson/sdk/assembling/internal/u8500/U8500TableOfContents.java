package com.stericsson.sdk.assembling.internal.u8500;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Vector;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.utilities.HexUtilities;

/**
 * The class represents the table of contents of software image
 * 
 * @author xolabju
 * 
 */
public class U8500TableOfContents {

    private static final long serialVersionUID = -1087732805976417282L;

    /** */
    public static final int MAX_NUMBER_OF_TOC_ITEMS = 16;

    /**
     * Size of a Table of contents
     */
    public static final int SIZE = MAX_NUMBER_OF_TOC_ITEMS * U8500TableOfContentsItem.SIZE;

    private Vector<U8500TableOfContentsItem> items;

    private int itemCount;

    /**
     * Constructor
     */
    public U8500TableOfContents() {
        items = new Vector<U8500TableOfContentsItem>();
        for (int i = 0; i < MAX_NUMBER_OF_TOC_ITEMS; i++) {
            items.add(U8500TableOfContentsItem.createFilledTOCItem());
        }
        itemCount = 0;
    }

    /**
     * 
     * @return all items in the TOC
     */
    public Vector<U8500TableOfContentsItem> getItems() {
        Vector<U8500TableOfContentsItem> realItems = new Vector<U8500TableOfContentsItem>();
        for (int i = 0; i < itemCount; i++) {
            realItems.add(items.get(i));
        }
        return realItems;
    }

    /**
     * 
     * @return the number of TOC items in the TOC
     */
    public int getItemCount() {
        return itemCount;
    }

    /**
     * @param item
     *            the TOCItem
     * @throws IndexOutOfBoundsException
     *             if the TOC is full
     */
    public void add(U8500TableOfContentsItem item) throws IndexOutOfBoundsException {
        if (getItemCount() >= MAX_NUMBER_OF_TOC_ITEMS) {
            throw new IndexOutOfBoundsException("Max number of TOC items = " + MAX_NUMBER_OF_TOC_ITEMS);
        }
        // always add ISSW items on top
        if (isISSWItem(item)) {
            items.add(0, item);
        } else {
            items.add(itemCount, item);
        }
        items.remove(items.lastElement());
        // remove last one (filled with 0xFF bytes)
        itemCount++;
    }

    /**
     * 
     * @return the table of contents as a byte array
     */
    public byte[] getData() {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.rewind();
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        for (U8500TableOfContentsItem item : items) {
            buffer.put(item.getData());
        }
        return buffer.array();
    }

    private boolean isISSWItem(U8500TableOfContentsItem item) {
        boolean isISSW = false;
        if (item != null) {
            String fileName = item.getFileNameString();
            if (fileName != null && fileName.equals(U8500TableOfContentsItem.FILENAME_ISSW)) {
                isISSW = true;
            }
        }
        return isISSW;
    }

    /**
     * 
     * @param fileName
     *            the TOC item file name
     * @return the toc item, or null if it wasn't found
     */
    public U8500TableOfContentsItem getItem(String fileName) {
        for (U8500TableOfContentsItem item : items) {
            if (item.getFileNameString().equals(fileName)) {
                return item;
            }
        }
        return null;
    }

    private String intToHex(int i) {
        return HexUtilities.toHexString(i, true);
    }

    private String createSpace(String s) {
        StringBuffer buffer = new StringBuffer();
        for (int i = s.length(); i < 14; i++) {
            buffer.append(" ");
        }

        return buffer.toString();
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        StringBuffer buffer = new StringBuffer();
        buffer.append("  Filename   \tStart Address\t    Size     \t    Flags    \t Entry Point \tLoad Address \n");
        buffer.append("-------------\t-------------\t-------------\t-------------\t-------------\t-------------\n");
        for (U8500TableOfContentsItem item : getItems()) {
            buffer.append(item.getFileNameString() + createSpace(item.getFileNameString()) + "\t"
                + intToHex(item.getStartAddress()) + "     \t" + intToHex(item.getSize()) + "     \t"
                + intToHex(item.getFlags()) + "     \t" + intToHex(item.getEntryPoint()) + "     \t"
                + intToHex(item.getLoadAddress()) + "     \t" + "\n");
        }
        return buffer.toString();
    }

    /**
     * Creates a table of contents from a byte array
     * 
     * @param tocData
     *            the TOC data
     * @return a table of contents
     * @throws AssemblerException
     *             if tocData is null or if the size tocData isn't equal to the size of the TOC or
     *             if any assembler related exceptions occur
     */
    public static U8500TableOfContents createTableOfContents(byte[] tocData) throws AssemblerException {
        if (tocData == null || tocData.length != SIZE) {
            throw new AssemblerException("Invalid TOC data");
        }
        U8500TableOfContents toc = new U8500TableOfContents();
        byte[] itemData = new byte[U8500TableOfContentsItem.SIZE];

        ByteBuffer buffer = ByteBuffer.wrap(tocData);
        buffer.rewind();

        for (int i = 0; i < MAX_NUMBER_OF_TOC_ITEMS; i++) {
            U8500TableOfContentsItem item = new U8500TableOfContentsItem();
            buffer.get(itemData);
            try {
                if (!Arrays.equals(itemData, U8500TableOfContentsItem.createFilledTOCItem().getData())) {
                    item.setData(itemData);
                    toc.add(item);
                }
            } catch (Exception e) {
                throw new AssemblerException(e);
            }
        }

        return toc;
    }
}
