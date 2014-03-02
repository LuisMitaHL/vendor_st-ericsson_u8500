package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * @author etomjun
 */
public class SectionHeader {
    /** */
    public static final int ELF32_SECTION_TABLE_ENTRY_SIZE = 40;

    /** */
    public static final int ELF64_SECTION_TABLE_ENTRY_SIZE = 64;

    /** */
    private int name;

    /** */
    private int type;

    /** */
    private long flags;

    /** */
    private long address;

    /** */
    private long offset;

    /** */
    private long size;

    /** */
    private int link;

    /** */
    private int info;

    /** */
    private long addressAlign;

    /** */
    private long entrySize;

    /** */
    private String stringName;

    /**
     * Constructor
     */
    public SectionHeader() {
        name = 0;
        type = 0;
        flags = 0;
        address = 0;
        offset = 0;
        size = 0;
        link = 0;
        info = 0;
        addressAlign = 0;
        entrySize = 0;
    }

    /**
     * @param input
     *            TBD
     * @param header
     *            TBD
     * @throws IOException
     *             TBD
     */
    public void read(FileChannel input, FileHeader header) throws IOException {
        ByteBuffer buffer = null;
        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_SECTION_TABLE_ENTRY_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_SECTION_TABLE_ENTRY_SIZE);
        }
        input.read(buffer);
        buffer.order(header.getELFByteOrder());
        buffer.rewind();
        name = buffer.getInt();
        type = buffer.getInt();
        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
            flags = buffer.getInt();
            address = buffer.getInt();
            offset = buffer.getInt();
            size = buffer.getInt();
            link = buffer.getInt();
            info = buffer.getInt();
            addressAlign = buffer.getInt();
            entrySize = buffer.getInt();
        } else {
            flags = buffer.getLong();
            address = buffer.getLong();
            offset = buffer.getLong();
            size = buffer.getLong();
            link = buffer.getInt();
            info = buffer.getInt();
            addressAlign = buffer.getLong();
            entrySize = buffer.getLong();
        }
    }

//FIXME - uncomment in a future when tests will be ready and method will be really used
//    /**
//     * @param output
//     *            TBD
//     * @param header
//     *            TBD
//     * @throws IOException
//     *             TBD
//     */
//    public void write(FileChannel output, FileHeader header) throws IOException {
//        ByteBuffer buffer = null;
//        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
//            buffer = ByteBuffer.allocate(ELF32_SECTION_TABLE_ENTRY_SIZE);
//        } else {
//            buffer = ByteBuffer.allocate(ELF64_SECTION_TABLE_ENTRY_SIZE);
//        }
//        buffer.rewind();
//        buffer.order(ByteOrder.LITTLE_ENDIAN);
//        buffer.putInt(name);
//        buffer.putInt(type);
//        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
//            buffer.putInt((int) flags);
//            buffer.putInt((int) address);
//            buffer.putInt((int) offset);
//            buffer.putInt((int) size);
//            buffer.putInt(link);
//            buffer.putInt(info);
//            buffer.putInt((int) addressAlign);
//            buffer.putInt((int) entrySize);
//        } else {
//            buffer.putLong(flags);
//            buffer.putLong(address);
//            buffer.putLong(offset);
//            buffer.putLong(size);
//            buffer.putInt(link);
//            buffer.putInt(info);
//            buffer.putLong(addressAlign);
//            buffer.putLong(entrySize);
//        }
//        buffer.rewind();
//        output.write(buffer);
//    }

    /**
     * @return TBD
     */
    public long getAddress() {
        return address;
    }

    /**
     * @param a
     *            TBD
     */
    public void setAddress(final long a) {
        address = a;
    }

    /**
     * @return TBD
     */
    public long getAddressAlign() {
        return addressAlign;
    }

    /**
     * @param a
     *            TBD
     */
    public void setAddressAlign(final long a) {
        addressAlign = a;
    }

    /**
     * @return TBD
     */
    public long getEntrySize() {
        return entrySize;
    }

    /**
     * @param s
     *            TBD
     */
    public void setEntrySize(final long s) {
        entrySize = s;
    }

    /**
     * @return TBD
     */
    public long getFlags() {
        return flags;
    }

    /**
     * @param f
     *            TBD
     */
    public void setFlags(final long f) {
        flags = f;
    }

    /**
     * @return TBD
     */
    public int getInfo() {
        return info;
    }

    /**
     * @param i
     *            TBD
     */
    public void setInfo(final int i) {
        info = i;
    }

    /**
     * @return TBD
     */
    public int getLink() {
        return link;
    }

    /**
     * @param l
     *            TBD
     */
    public void setLink(final int l) {
        link = l;
    }

    /**
     * @return TBD
     */
    public int getName() {
        return name;
    }

    /**
     * @param n
     *            TBD
     */
    public void setName(final int n) {
        name = n;
    }

    /**
     * @return TBD
     */
    public String getStringName() {
        return stringName;
    }

    /**
     * @param n
     *            TBD
     */
    public void setStringName(String n) {
        stringName = n;
    }

    /**
     * @return TBD
     */
    public long getOffset() {
        return offset;
    }

    /**
     * @param o
     *            TBD
     */
    public void setOffset(final long o) {
        offset = o;
    }

    /**
     * @return TBD
     */
    public long getSize() {
        return size;
    }

    /**
     * @param s
     *            TBD
     */
    public void setSize(final long s) {
        size = s;
    }

    /**
     * @return TBD
     */
    public int getType() {
        return type;
    }

    /**
     * @param t
     *            TBD
     */
    public void setType(final int t) {
        type = t;
    }
}
