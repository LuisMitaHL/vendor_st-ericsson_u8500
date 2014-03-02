package com.stericsson.sdk.equipment.image;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.LinkedHashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.image.ITableOfContentsItem;

/**
 * The class represents an item in the table of contents of a binary image
 * 
 * @author xolabju
 * 
 */
public class TableOfContentsItem extends AbstractByteSequence implements ITableOfContentsItem {

    static Logger logger = Logger.getLogger(TableOfContentsItem.class.getName());

    /**
     * Constructor
     */
    public TableOfContentsItem() {
        fields = new LinkedHashMap<Integer, ByteBuffer>();

        fields.put(FIELD_START, allocateField(SIZE_START));
        fields.put(FIELD_SIZE, allocateField(SIZE_SIZE));
        fields.put(FIELD_FLAGS, allocateField(SIZE_FLAGS));
        fields.put(FIELD_ENTRY_POINT, allocateField(SIZE_ENTRY_POINT));
        fields.put(FIELD_LOAD_ADDRESS, allocateField(SIZE_LOAD_ADDRESS));
        fields.put(FIELD_FILENAME, allocateField(SIZE_FILENAME));
    }

    /**
     * 
     * @return the start address (4 bytes)
     */
    public int getStartAddress() {
        return fields.get(FIELD_START).getInt(0);
    }

    /**
     * 
     * @return the sub image size (4 bytes)
     */
    public int getSize() {
        return fields.get(FIELD_SIZE).getInt(0);
    }

    /**
     * 
     * @return the load address (4 bytes)
     */
    public int getLoadAddress() {
        return fields.get(FIELD_LOAD_ADDRESS).getInt(0);
    }

    /**
     * @return the sub image file name (12 bytes including the zero ['\0'] terminator)
     */
    public byte[] getFileName() {
        byte[] fileName = new byte[SIZE_FILENAME];
        fields.get(FIELD_FILENAME).rewind();
        fields.get(FIELD_FILENAME).get(fileName, 0, SIZE_FILENAME);
        return fileName;
    }

    /**
     * 
     * @return the file name as a String without null terminators
     */
    public String getFileNameString() {
        String s = "";
        try {
            s = new String(getFileName(), "UTF-8");
        } catch (UnsupportedEncodingException e) {
            logger.error(e.getMessage());
        }
        int nullPos = s.indexOf("\0");

        if (s.length() > 0 && nullPos != -1) {
            s = s.substring(0, nullPos);
        }
        return s;
    }

    /**
     * 
     * @return a TOCItem with every byte == 0xFF
     */
    protected static TableOfContentsItem createFilledTOCItem() {
        TableOfContentsItem item = new TableOfContentsItem();
        for (ByteBuffer b : item.fields.values()) {
            Arrays.fill(b.array(), (byte) 0xFF);
        }

        return item;
    }
}
