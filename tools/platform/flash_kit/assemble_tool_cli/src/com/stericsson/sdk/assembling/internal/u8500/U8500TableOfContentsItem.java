package com.stericsson.sdk.assembling.internal.u8500;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.logging.Logger;

import com.stericsson.sdk.assembling.utilities.AbstractByteSequence;
import com.stericsson.sdk.assembling.utilities.HexUtilities;

/**
 * The class represents an item in the table of contents of a binary image
 * 
 * @author xolabju
 * 
 */
public class U8500TableOfContentsItem extends AbstractByteSequence {

    /**
     * Total size of a single TOC item
     */
    public static final int SIZE = 32;

    private static final int SIZE_START = 4;

    private static final int SIZE_SIZE = 4;

    private static final int SIZE_FLAGS = 4;

    private static final int SIZE_ENTRY_POINT = 4;

    private static final int SIZE_LOAD_ADDRESS = 4;

    /** The size of the filename field of the item */
    public static final int SIZE_FILENAME = 12;

    private static final int FIELD_START = 1;

    private static final int FIELD_SIZE = 2;

    private static final int FIELD_FLAGS = 3;

    private static final int FIELD_ENTRY_POINT = 4;

    private static final int FIELD_LOAD_ADDRESS = 5;

    private static final int FIELD_FILENAME = 6;

    // TOC item file names.
    /** */
    public static final String FILENAME_ISSW = "ISSW";

    /** */
    public static final String FILENAME_XLOADER = "X-LOADER";

    /** */
    public static final String FILENAME_MEM_INIT = "MEM_INIT";

    /** */
    public static final String FILENAME_NORMAL = "NORMAL";

    /** Power Management / XP70 */
    public static final String FILENAME_PWR_MGT = "PWR_MGT";

    /** */
    public static final String FILENAME_IPL = "IPL";

    /** */
    public static final String FILENAME_MODEM = "MODEM";

    /** */
    public static final String FILENAME_ADL = "ADL";

    /** */
    public static final String FILENAME_PRODUCTION = "PRODUCTION";

    /** */
    public static final String FILENAME_CRKC = "CRKC";

    static Logger logger = Logger.getLogger(U8500TableOfContentsItem.class.getName());

    /**
     * Constructor
     */
    public U8500TableOfContentsItem() {
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
     * @param address
     *            the start address (4 bytes)
     */
    public void setStartAddress(int address) {
        fields.get(FIELD_START).putInt(0, address);
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
     * @param size
     *            the sub image size (4 bytes)
     */
    public void setSize(int size) {
        fields.get(FIELD_SIZE).putInt(0, size);
    }

    /**
     * 
     * @return the load address (4 bytes)
     */
    public int getLoadAddress() {
        return fields.get(FIELD_LOAD_ADDRESS).getInt(0);
    }

    /**
     * 
     * @param address
     *            the load address (4 bytes)
     */
    public void setLoadAddress(long address) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_LOAD_ADDRESS).putInt(0, (int) (address & 0xFFFFFFFF));
    }

    /**
     * 
     * @return the entry point (4 bytes)
     */
    public int getEntryPoint() {
        return fields.get(FIELD_ENTRY_POINT).getInt(0);
    }

    /**
     * 
     * @param ep
     *            the entry point (4 bytes)
     */
    public void setEntryPoint(long ep) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_ENTRY_POINT).putInt(0, (int) (ep & 0xFFFFFFFF));
    }

    /**
     * @param fileName
     *            the sub image file name (12 bytes including the zero ['\0'] terminator)
     * 
     */
    public void setFileName(byte[] fileName) {
        fields.get(FIELD_FILENAME).rewind();
        fields.get(FIELD_FILENAME).put(fileName, 0, fileName.length);
    }

    /**
     * @param fileName
     *            the sub image file name (max 11 characters)
     * 
     */
    public void setFileName(String fileName) {
        try {
            setFileName(fileName.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            System.err.print("\nException technical details:\n");
            e.printStackTrace();
            logger.severe(e.getMessage());
        }
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
     * @return the flags (4 bytes)
     */
    public int getFlags() {
        return fields.get(FIELD_FLAGS).getInt(0);
    }

    /**
     * 
     * @param flags
     *            the flags (4 bytes)
     */
    public void setFlags(long flags) {
        // workaround to be able to use the int as if it were unsigned
        fields.get(FIELD_FLAGS).putInt(0, (int) (flags & 0xFFFFFFFF));
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
            System.err.print("\nException technical details:\n");
            e.printStackTrace();
            logger.severe(e.getMessage());
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
    protected static U8500TableOfContentsItem createFilledTOCItem() {
        U8500TableOfContentsItem item = new U8500TableOfContentsItem();
        for (ByteBuffer b : item.fields.values()) {
            Arrays.fill(b.array(), (byte) 0xFF);
        }

        return item;
    }

    /**
     * @return string representation
     */
    @Override
    public String toString() {
        StringBuffer b = new StringBuffer();
        b.append("TOC Item: ");
        b.append("\nStart Address: ");
        b.append(HexUtilities.toHexString(getStartAddress()));
        b.append("\nSize:          ");
        b.append(getSize() + " Bytes");
        b.append("\nLoad Address:  ");
        b.append(HexUtilities.toHexString(getLoadAddress()));
        b.append("\nFileName:      ");
        b.append(getFileNameString());
        return b.toString();
    }
}
