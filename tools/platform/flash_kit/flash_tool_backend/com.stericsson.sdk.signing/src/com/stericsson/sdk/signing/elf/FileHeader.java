package com.stericsson.sdk.signing.elf;

import java.io.IOException;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * @author etomjun
 */
public class FileHeader {

    /** */
    private static final int ELF_IDENTIFICATION_SIZE = 16;

    /** */
    private static final int MAGIC_0 = 0x7F;

    /** */
    private static final int MAGIC_1 = 0x45;

    /** */
    private static final int MAGIC_2 = 0x4C;

    /** */
    private static final int MAGIC_3 = 0x46;

    /** */
    public static final int CLASS_32BIT = 1;

    /** */
    public static final int CLASS_64BIT = 2;

    /** */
    private static final int ORDER_LSB = 1;

    /** */
    private static final int ORDER_MSB = 2;

    /** */
    private static final int VERSION_CURRENT = 1;

    /** */
    private static final int ELF32_HEADER_SIZE = 36;

    /** */
    private static final int ELF64_HEADER_SIZE = 48;

    /** */
    private int elfClass;

    /** */
    private ByteOrder elfByteOrder;

    /** */
    private int type;

    /** */
    private int machine;

    /** */
    private int version;

    /** */
    private long entry;

    /** */
    private long programHeaderOffset;

    /** */
    private long sectionHeaderOffset;

    /** */
    private int flags;

    /** */
    private int programHeaderNumOfEntries;

    /** */
    private int sectionHeaderNumOfEntries;

    /** */
    private int sectionHeaderStringTableIndex;

    /**
     * Constructor.
     * 
     * @throws ELFException
     *             TBD
     */
    public FileHeader() throws ELFException {
        this(CLASS_32BIT);
    }

    /**
     * @param c
     *            TBD
     * @throws ELFException
     *             TBD
     */
    public FileHeader(final int c) throws ELFException {
        elfClass = c;
        type = 0;
        machine = 0;
        version = 0;
        entry = 0;
        flags = 0;
        programHeaderNumOfEntries = 0;
        sectionHeaderNumOfEntries = 0;
        sectionHeaderStringTableIndex = 0;
        if (elfClass == CLASS_32BIT || elfClass == CLASS_64BIT) {
            programHeaderOffset += getElfHeaderSize();
            sectionHeaderOffset += getElfHeaderSize();
        } else {
            throw new ELFException("Unsupported class");
        }
    }

    /**
     * @param input
     *            TBD
     * @throws IOException
     *             TBD
     * @throws ELFException
     *             TBD
     */
    public void read(final FileChannel input) throws IOException, ELFException {
        if (input == null) {
            throw new IOException("Input channel is null");
        }
        input.position(0);
        ByteBuffer buffer = ByteBuffer.allocate(ELF_IDENTIFICATION_SIZE);
        while (buffer.remaining() > 0) {
            input.read(buffer);
        }
        buffer.rewind();
        matchMagicSequence(buffer);
        elfClass = buffer.get();
        if ((elfClass != CLASS_32BIT) && (elfClass != CLASS_64BIT)) {
            throw new ELFException("Unsupported class");
        }
        int order = buffer.get();
        if (order == ORDER_LSB) {
            elfByteOrder = ByteOrder.LITTLE_ENDIAN;
        } else if (order == ORDER_MSB) {
            elfByteOrder = ByteOrder.BIG_ENDIAN;
        } else {
            throw new ELFException("Unsupported byte order");
        }

        if (buffer.get() != VERSION_CURRENT) {
            throw new ELFException("Unsupported version");
        }

        if (elfClass == CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_HEADER_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_HEADER_SIZE);
        }

        while (buffer.remaining() > 0) {
            input.read(buffer);
        }
        buffer.order(elfByteOrder);
        buffer.rewind();
        type = buffer.getShort();
        machine = buffer.getShort();
        version = buffer.getInt();

        if (elfClass == CLASS_32BIT) {
            entry = buffer.getInt();
            programHeaderOffset = buffer.getInt();
            sectionHeaderOffset = buffer.getInt();
        } else {
            entry = buffer.getLong();
            programHeaderOffset = buffer.getLong();
            sectionHeaderOffset = buffer.getLong();
        }

        flags = buffer.getInt();
        buffer.getShort(); // Skip header size, known through elfClass
        buffer.getShort(); // Skip program header entry size, known through elfClass
        programHeaderNumOfEntries = buffer.getShort();
        buffer.getShort(); // Skip section header entry size, known through elfClass
        sectionHeaderNumOfEntries = buffer.getShort();
        sectionHeaderStringTableIndex = buffer.getShort();
    }

    private void matchMagicSequence(ByteBuffer buffer) throws ELFException {
        if ((buffer.get() != MAGIC_0) || (buffer.get() != MAGIC_1) || (buffer.get() != MAGIC_2)
            || (buffer.get() != MAGIC_3)) {
            throw new ELFException("Not an ELF file");
        }
    }

    /**
     * @param output
     *            TBD
     * @throws IOException
     *             TBD
     */
    public void write(final FileChannel output) throws IOException {
        ByteBuffer buffer = ByteBuffer.allocate(ELF_IDENTIFICATION_SIZE);
        buffer.rewind();
        buffer.put((byte) MAGIC_0).put((byte) MAGIC_1);
        buffer.put((byte) MAGIC_2).put((byte) MAGIC_3);
        buffer.put((byte) elfClass);
        buffer.put((byte) ORDER_LSB);
        buffer.put((byte) VERSION_CURRENT);
        buffer.rewind();
        output.write(buffer, 0);
        if (elfClass == CLASS_32BIT) {
            buffer = ByteBuffer.allocate(ELF32_HEADER_SIZE);
        } else {
            buffer = ByteBuffer.allocate(ELF64_HEADER_SIZE);
        }
        buffer.rewind();
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putShort((short) type);
        buffer.putShort((short) machine);
        buffer.putInt(version);
        if (elfClass == CLASS_32BIT) {
            buffer.putInt((int) entry);
            buffer.putInt((int) programHeaderOffset);
            buffer.putInt((int) sectionHeaderOffset);
        } else {
            buffer.putLong(entry);
            buffer.putLong(programHeaderOffset);
            buffer.putLong(sectionHeaderOffset);
        }
        buffer.putInt(flags);
        if (elfClass == CLASS_32BIT) {
            buffer.putShort((short) getElfHeaderSize());
            buffer.putShort((short) ProgramHeader.ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
            buffer.putShort((short) programHeaderNumOfEntries);
            buffer.putShort((short) SectionHeader.ELF32_SECTION_TABLE_ENTRY_SIZE);
            buffer.putShort((short) sectionHeaderNumOfEntries);
        } else {
            buffer.putShort((short) getElfHeaderSize());
            buffer.putShort((short) ProgramHeader.ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
            buffer.putShort((short) programHeaderNumOfEntries);
            buffer.putShort((short) SectionHeader.ELF64_SECTION_TABLE_ENTRY_SIZE);
            buffer.putShort((short) sectionHeaderNumOfEntries);
        }
        buffer.putShort((short) sectionHeaderStringTableIndex);
        buffer.rewind();
        output.write(buffer, ELF_IDENTIFICATION_SIZE);
    }

    /**
     * @return TBD
     */
    public byte[] getBytes() {
        ByteBuffer buffer = null;
        if (elfClass == CLASS_32BIT) {
            buffer = ByteBuffer.allocate(getElfHeaderSize());
        } else {
            buffer = ByteBuffer.allocate(getElfHeaderSize());
        }
        buffer.rewind();
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put((byte) MAGIC_0).put((byte) MAGIC_1);
        buffer.put((byte) MAGIC_2).put((byte) MAGIC_3);
        buffer.put((byte) elfClass);
        buffer.put((byte) ORDER_LSB);
        buffer.put((byte) VERSION_CURRENT);
        buffer.position(ELF_IDENTIFICATION_SIZE);
        buffer.putShort((short) type);
        buffer.putShort((short) machine);
        buffer.putInt(version);
        if (elfClass == CLASS_32BIT) {
            buffer.putInt((int) entry);
            buffer.putInt((int) programHeaderOffset);
            buffer.putInt((int) sectionHeaderOffset);
        } else {
            buffer.putLong(entry);
            buffer.putLong(programHeaderOffset);
            buffer.putLong(sectionHeaderOffset);
        }
        buffer.putInt(flags);
        if (elfClass == CLASS_32BIT) {
            buffer.putShort((short) getElfHeaderSize());
            buffer.putShort((short) ProgramHeader.ELF32_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
            buffer.putShort((short) programHeaderNumOfEntries);
            buffer.putShort((short) SectionHeader.ELF32_SECTION_TABLE_ENTRY_SIZE);
            buffer.putShort((short) sectionHeaderNumOfEntries);
        } else {
            buffer.putShort((short) getElfHeaderSize());
            buffer.putShort((short) ProgramHeader.ELF64_PROGRAM_HEADER_TABLE_ENTRY_SIZE);
            buffer.putShort((short) programHeaderNumOfEntries);
            buffer.putShort((short) SectionHeader.ELF64_SECTION_TABLE_ENTRY_SIZE);
            buffer.putShort((short) sectionHeaderNumOfEntries);
        }
        buffer.putShort((short) sectionHeaderStringTableIndex);
        buffer.rewind();
        return buffer.array();
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

    /**
     * @return TBD
     */
    public int getMachine() {
        return machine;
    }

    /**
     * @param m
     *            TBD
     */
    public void setMachine(final int m) {
        machine = m;
    }

    /**
     * @return TBD
     */
    public int getVersion() {
        return version;
    }

    /**
     * @param v
     *            TBD
     */
    public void setVersion(final int v) {
        version = v;
    }

    /**
     * @return TBD
     */
    public long getEntry() {
        return entry;
    }

    /**
     * @param e
     *            TBD
     */
    public void setEntry(final long e) {
        entry = e;
    }

    /**
     * @return TBD
     */
    public long getProgramHeaderOffset() {
        return programHeaderOffset;
    }

    /**
     * @param o
     *            TBD
     */
    public void setProgramHeaderOffset(final long o) {
        programHeaderOffset = o;
    }

    /**
     * @return TBD
     */
    public long getSectionHeaderOffset() {
        return sectionHeaderOffset;
    }

    /**
     * @param o
     *            TBD
     */
    public void setSectionHeaderOffset(final long o) {
        sectionHeaderOffset = o;
    }

    /**
     * @return TBD
     */
    public int getFlags() {
        return flags;
    }

    /**
     * @param f
     *            TBD
     */
    public void setFlags(final int f) {
        flags = f;
    }

    /**
     * @return TBD
     */
    public int getProgramHeaderNumOfEntries() {
        return programHeaderNumOfEntries;
    }

    /**
     * @param n
     *            TBD
     */
    public void setProgramHeaderNumOfEntries(final int n) {
        programHeaderNumOfEntries = n;
    }

    /**
     * @return TBD
     */
    public int getSectionHeaderNumOfEntries() {
        return sectionHeaderNumOfEntries;
    }

    /**
     * @param n
     *            TBD
     */
    public void setSectionHeaderNumOfEntries(final int n) {
        sectionHeaderNumOfEntries = n;
    }

    /**
     * @return TBD
     */
    public int getSectionHeaderStringTableIndex() {
        return sectionHeaderStringTableIndex;
    }

    /**
     * @param i
     *            TBD
     */
    public void setSectionHeaderStringTableIndex(final int i) {
        sectionHeaderStringTableIndex = i;
    }

    /**
     * @return TBD
     */
    public ByteOrder getELFByteOrder() {
        return elfByteOrder;
    }

    /**
     * @param o
     *            TBD
     */
    public void setELFByteOrder(final ByteOrder o) {
        elfByteOrder = o;
    }

    /**
     * @return TBD
     */
    public int getELFClass() {
        return elfClass;
    }

    /**
     * @return TBD
     */
    public int getElfHeaderSize() {
        int result = ELF_IDENTIFICATION_SIZE;
        if (elfClass == CLASS_32BIT) {
            result += ELF32_HEADER_SIZE;
        } else {
            result += ELF64_HEADER_SIZE;
        }
        return result;
    }
}
