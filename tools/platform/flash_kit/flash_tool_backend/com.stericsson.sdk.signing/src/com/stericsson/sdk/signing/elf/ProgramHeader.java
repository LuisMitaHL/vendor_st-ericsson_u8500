package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

/**
 * @author etomjun
 */
public class ProgramHeader {
    /** */
    public static final int ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE = 32;

    /** */
    public static final int ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE = 56;

    private int type;

    private long offset;

    private long virtualAddress;

    private long physicalAddress;

    private long fileSize;

    private long memorySize;

    private int flags;

    private long align;

    private byte[] data;

    /**
     * @return the data
     */
    public byte[] getData() {
        return data;
    }

    /**
     * @param d
     *            the data to set
     */
    public void setData(byte[] d) {
        data = d;
    }

    /**
     * Constructor
     */
    public ProgramHeader() {
        type = 0;
        offset = 0;
        virtualAddress = 0;
        physicalAddress = 0;
        fileSize = 0;
        memorySize = 0;
        flags = 0;
        align = 0;
    }

    /**
     * @param input
     *            TBD
     * @param header
     *            TBD
     * @throws IOException
     *             TBD
     */
    public void read(ReadableByteChannel input, FileHeader header) throws IOException {
        ByteBuffer buffer = null;
        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        }
        while (buffer.remaining() > 0) {
            input.read(buffer);
        }
        buffer.order(header.getELFByteOrder());
        buffer.rewind();
        if (header.getELFClass() == FileHeader.CLASS_32BIT) {
            type = buffer.getInt();
            offset = buffer.getInt();
            virtualAddress = buffer.getInt();
            physicalAddress = buffer.getInt();
            fileSize = buffer.getInt();
            memorySize = buffer.getInt();
            flags = buffer.getInt();
            align = buffer.getInt();
        } else {
            type = buffer.getInt();
            flags = buffer.getInt();
            offset = buffer.getLong();
            virtualAddress = buffer.getLong();
            physicalAddress = buffer.getLong();
            fileSize = buffer.getLong();
            memorySize = buffer.getLong();
            align = buffer.getLong();
        }
    }

    /**
     * @param output
     *            TBD
     * @param elfClass
     *            TBD
     * @param order
     *            TBD
     * @throws IOException
     *             TBD
     */
    public void write(WritableByteChannel output, int elfClass, ByteOrder order) throws IOException {
        ByteBuffer buffer = null;
        if (elfClass == FileHeader.CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        }
        buffer.order(order);
        if (elfClass == FileHeader.CLASS_32BIT) {
            buffer.putInt(type);
            buffer.putInt((int) offset);
            buffer.putInt((int) virtualAddress);
            buffer.putInt((int) physicalAddress);
            buffer.putInt((int) fileSize);
            buffer.putInt((int) memorySize);
            buffer.putInt(flags);
            buffer.putInt((int) align);
        } else {
            buffer.putInt(type);
            buffer.putInt(flags);
            buffer.putLong(offset);
            buffer.putLong(virtualAddress);
            buffer.putLong(physicalAddress);
            buffer.putLong(fileSize);
            buffer.putLong(memorySize);
            buffer.putLong(align);
        }
        buffer.rewind();
        output.write(buffer);
    }

    /**
     * @param elfClass
     *            TBD
     * @return TBD
     */
    public byte[] getBytes(int elfClass) {
        ByteBuffer buffer = null;
        if (elfClass == FileHeader.CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
        }
        buffer.rewind();
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        if (elfClass == FileHeader.CLASS_32BIT) {
            buffer.putInt(type);
            buffer.putInt((int) offset);
            buffer.putInt((int) virtualAddress);
            buffer.putInt((int) physicalAddress);
            buffer.putInt((int) fileSize);
            buffer.putInt((int) memorySize);
            buffer.putInt(flags);
            buffer.putInt((int) align);
        } else {
            buffer.putInt(type);
            buffer.putInt(flags);
            buffer.putLong(offset);
            buffer.putLong(virtualAddress);
            buffer.putLong(physicalAddress);
            buffer.putLong(fileSize);
            buffer.putLong(memorySize);
            buffer.putLong(align);
        }
        buffer.rewind();
        return buffer.array();
    }

    /**
     * @return the align
     */
    public long getAlign() {
        return align;
    }

    /**
     * @param a
     *            the align to set
     */
    public void setAlign(long a) {
        align = a;
    }

    /**
     * @return the fileSize
     */
    public long getFileSize() {
        return fileSize;
    }

    /**
     * @param s
     *            the fileSize to set
     */
    public void setFileSize(long s) {
        fileSize = s;
    }

    /**
     * @return the flags
     */
    public int getFlags() {
        return flags;
    }

    /**
     * @param f
     *            the flags to set
     */
    public void setFlags(int f) {
        flags = f;
    }

    /**
     * @return the memorySize
     */
    public long getMemorySize() {
        return memorySize;
    }

    /**
     * @param s
     *            the memorySize to set
     */
    public void setMemorySize(long s) {
        memorySize = s;
    }

    /**
     * @return the offset
     */
    public long getOffset() {
        return offset;
    }

    /**
     * @param o
     *            the offset to set
     */
    public void setOffset(long o) {
        offset = o;
    }

    /**
     * @return the physicalAddress
     */
    public long getPhysicalAddress() {
        return physicalAddress;
    }

    /**
     * @param address
     *            the physicalAddress to set
     */
    public void setPhysicalAddress(long address) {
        physicalAddress = address;
    }

    /**
     * @return the type
     */
    public int getType() {
        return type;
    }

    /**
     * @param t
     *            the type to set
     */
    public void setType(int t) {
        type = t;
    }

    /**
     * @return the virtualAddress
     */
    public long getVirtualAddress() {
        return virtualAddress;
    }

    /**
     * @param address
     *            the virtualAddress to set
     */
    public void setVirtualAddress(long address) {
        virtualAddress = address;
    }
}
