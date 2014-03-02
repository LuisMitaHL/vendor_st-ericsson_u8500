package com.stericsson.sdk.equipment.image;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Vector;

/**
 * The class represents the table of contents of software image
 * 
 * @author xolabju
 * 
 */
public class TableOfContents {

    private static final long serialVersionUID = -1087732805976417282L;

    /** */
    public static final int MAX_NUMBER_OF_TOC_ITEMS = 16;

    /**
     * Size of a Table of contents
     */
    public static final int SIZE = MAX_NUMBER_OF_TOC_ITEMS * TableOfContentsItem.SIZE;

    private Vector<TableOfContentsItem> items;

    private int itemCount;

    /**
     * Constructor
     */
    public TableOfContents() {
        items = new Vector<TableOfContentsItem>();
        for (int i = 0; i < MAX_NUMBER_OF_TOC_ITEMS; i++) {
            items.add(TableOfContentsItem.createFilledTOCItem());
        }
        itemCount = 0;
    }

    /**
     * 
     * @return all items in the TOC
     */
    public Vector<TableOfContentsItem> getItems() {
        Vector<TableOfContentsItem> realItems = new Vector<TableOfContentsItem>();
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
    public void add(TableOfContentsItem item) throws IndexOutOfBoundsException {
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
        for (TableOfContentsItem item : items) {
            buffer.put(item.getData());
        }
        return buffer.array();
    }

    private boolean isISSWItem(TableOfContentsItem item) {
        boolean isISSW = false;
        if (item != null) {
            String fileName = item.getFileNameString();
            if (fileName != null && fileName.equals(TableOfContentsItem.FILENAME_ISSW)) {
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
    public TableOfContentsItem getItem(String fileName) {
        for (TableOfContentsItem item : items) {
            if (item.getFileNameString().equals(fileName)) {
                return item;
            }
        }
        return null;
    }

    /**
     * Creates a table of contents from a byte array
     * 
     * @param tocData
     *            the TOC data
     * @return a table of contents
     * @throws Exception
     *             if tocData is null or if the size tocData isn't equal to the size of the TOC or
     *             if any assembler related exceptions occur
     */
    public static TableOfContents createTableOfContents(byte[] tocData) throws Exception {
        if (tocData == null || tocData.length != SIZE) {
            throw new Exception("Invalid TOC data");
        }
        TableOfContents toc = new TableOfContents();
        byte[] itemData = new byte[TableOfContentsItem.SIZE];

        ByteBuffer buffer = ByteBuffer.wrap(tocData);
        buffer.rewind();

        for (int i = 0; i < MAX_NUMBER_OF_TOC_ITEMS; i++) {
            TableOfContentsItem item = new TableOfContentsItem();
            buffer.get(itemData);
            try {
                if (!Arrays.equals(itemData, TableOfContentsItem.createFilledTOCItem().getData())) {
                    item.setData(itemData);
                    toc.add(item);
                }
            } catch (Exception e) {
                throw new Exception(e.getMessage());
            }
        }

        return toc;
    }
}
