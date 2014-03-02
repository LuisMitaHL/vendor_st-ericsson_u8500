package com.stericsson.sdk.common.memory;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.TreeMap;
import java.util.Map.Entry;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * This class implements virtual memory with random access which can grow if needed. Virtual memory
 * can handle 32bit addresses what means that address range is <0x00000000, 0xFFFFFFFF>. Memory
 * consits of units of defined unit size. If some address has to be written but haven't been
 * allocated yet, new space of unit size is allocated. Setting of too big unit size will waste with
 * memory and too low could cause significant fragmentation and thus low performance.
 * 
 * @author emicroh
 */
public class VirtualMemory {

    /** Size for kilobyte. */
    public static final int KILO_BYTE = 1024;

    /** Size for megabyte. */
    public static final int MEGA_BYTE = KILO_BYTE * KILO_BYTE;

    /** Size for gigabyte. */
    public static final int GIGA_BYTE = MEGA_BYTE * KILO_BYTE;

    /** Size of software payload block. */
    public static final int PAYLOAD_BLOCK_SIZE = 64 * KILO_BYTE;

    /** Definition of undefined address */
    public static final long UNVALID_ADDRESS = -1L;

    /** Default size for memory unit. */
    private static final int DEFAULT_UNIT_SIZE = 256 * KILO_BYTE;

    /** Default gap filler. */
    private static final byte DEFAULT_GAP_FILL = 0x00;

    /** All allocated memory units. */
    private TreeMap<Integer, MemoryUnit> mUnits;

    private int unitSize;

    private byte gap = DEFAULT_GAP_FILL;

    private long blockPointer = 0L;

    /**
     * Will construct instance of VirtualMemory with unit size equals to
     * {@link VirtualMemory#DEFAULT_UNIT_SIZE} with default gap filler.
     */
    public VirtualMemory() {
        mUnits = new TreeMap<Integer, MemoryUnit>();
        unitSize = DEFAULT_UNIT_SIZE;
    }

    /**
     * Will construct instance of VirtualMemory with unit size equals to
     * {@link VirtualMemory#DEFAULT_UNIT_SIZE} with custom gap filler.
     * 
     * @param gapFill
     *            Byte which is used for gap filling.
     */
    public VirtualMemory(byte gapFill) {
        this();
        gap = gapFill;
    }

    /**
     * Will construct instance of VirtualMemory with custom unit size and default gap filler.
     * 
     * @param bSize
     *            Size of memory units handled by this memory.
     */
    public VirtualMemory(int bSize) {
        this();
        unitSize = bSize;
    }

    /**
     * Will construct instance of VirtualMemory with custom unit size and gap filler.
     * 
     * @param bSize
     *            Size of memory units handled by this memory.
     * @param gapFill
     *            Byte which is used for gap filling.
     */
    public VirtualMemory(int bSize, byte gapFill) {
        this(bSize);
        gap = gapFill;
    }

    /**
     * Serves to retrieve remaining unallocated space in this memory. When no bytes were written,
     * total space will be returned.
     * 
     * @return Amount of available space in bytes.
     */
    public long getCapacity() {
        return getHighestAddress() + 1 - getReserved();
    }

    /**
     * @return Total reserved bytes in this memory.
     */
    public long getReserved() {
        long reserved = 0L;
        for (Entry<Integer, MemoryUnit> unit : mUnits.entrySet()) {
            reserved += unit.getValue().getReserved();
        }
        return reserved;
    }

    /**
     * @return Percentage usage of allocated memory in compare with actually reserved space.
     */
    public double usageEfficiency() {
        double reserved = (double) getReserved();
        if (reserved > 0) {
            return reserved / (mUnits.size() * unitSize);
        } else if (mUnits.size() > 0) {
            return 0.0;
        } else {
            return 1.0;
        }
    }

    /**
     * @return First written address in this memory if anything has already been written,
     *         {@link VirtualMemory#UNVALID_ADDRESS} otherwise.
     */
    public long getFirstUsedAddress() {
        if (mUnits.size() > 0) {
            return mUnits.get(mUnits.firstKey()).getFirstUsedAddress();
        } else {
            return UNVALID_ADDRESS;
        }
    }

    /**
     * @return Last written address in this memory if anything has already been written,
     *         {@link VirtualMemory#UNVALID_ADDRESS} otherwise.
     */
    public long getLastUsedAddress() {
        if (mUnits.size() > 0) {
            return mUnits.get(mUnits.lastKey()).getLastUsedAddress();
        } else {
            return UNVALID_ADDRESS;
        }
    }

    /**
     * Convenient method for writing single byte on given address.
     * 
     * @param address
     *            Address to be written.
     * @param c
     *            Low 8 bits will be used as byte to write.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public void write(long address, int c) throws MemoryIOException {
        MemoryUnit unit = getAllocatedUnit(getUnitIndex(address));
        unit.write(address, new byte[] {
            (byte) (c & 0xFF)}, 0, 1);
    }

    /**
     * Convenient method for writing entire byte buffer on given address.
     * 
     * @param address
     *            Address to be written.
     * @param buf
     *            Data to be written.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public void write(long address, byte[] buf) throws MemoryIOException {
        write(address, buf, 0, buf.length);
    }

    /**
     * Convenient method for writing given number of bytes from given offset address of given buffer
     * on specified address.
     * 
     * @param address
     *            Address to be written.
     * @param buf
     *            Byte buffer containing data to be written.
     * @param off
     *            Offset in buffer which should be taken as start of data to be written.
     * @param length
     *            Number of bytes to be written from given offset.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public void write(long address, byte[] buf, int off, int length) throws MemoryIOException {
        checkAddress(address);
        checkAddress(address + length);

        int startUnit = getUnitIndex(address);
        int endUnit = getUnitIndex(address + length - 1);

        int writtenBytes = 0;
        long writePointer = address;
        for (int i = startUnit; i <= endUnit; i++) {
            MemoryUnit mu = getAllocatedUnit(i);
            int offset = writtenBytes + off;
            int cnt = Math.min((int) (mu.getLastAddress() - writePointer) + 1, length - writtenBytes);

            mu.write(writePointer, buf, offset, cnt);
            writtenBytes += cnt;
            writePointer += cnt;
        }
    }

    /**
     * Use for freeing/unreserving specified number of bytes from given address in this memory. When
     * all data for allocation unit are freed, unit will be automatically deallocated.
     * 
     * @param address
     *            Address from which data should be unreserved.
     * @param length
     *            Amount of bytes to be unreserved.
     */
    public void free(long address, long length) {
        int startUnit = getUnitIndex(address);
        int endUnit = getUnitIndex(address + length - 1);

        long freed = 0L;
        for (int i = startUnit; i <= endUnit; i++) {
            MemoryUnit mu = mUnits.get(i);
            if (mu != null) {
                int toBeFreed = (int) Math.min(mu.getLastAddress() - (address + freed) + 1, length - freed);
                try {
                    mu.free(address + freed, toBeFreed);
                } catch (MemoryIOException e) {
                    e.printStackTrace(); // should never happen
                }
                freed += toBeFreed;
                if (mu.getReserved() == 0) {
                    mUnits.remove(i);
                }
            } else {
                freed += Math.min(getHighestUnitAddress(i) - address + freed + 1, unitSize);
            }
        }
    }

    /**
     * Convenient method for reading single byte from memory on given address.
     * 
     * @param address
     *            Address to be read.
     * @return Read byte.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public int read(long address) throws MemoryIOException {
        checkAddress(address);
        MemoryUnit unit = mUnits.get(getUnitIndex(address));
        if (unit != null) {
            byte[] buf = new byte[1];
            unit.read(address, buf, 0, buf.length);
            return buf[0] & 0xFF;
        } else {
            return gap & 0xFF;
        }
    }

    /**
     * Convenient method for reading memory from given address into buffer. If unwritten address is
     * reached gap will be used instead and stored in appropriate position in buffer. This means
     * that if reading is successful return value will be equals to buffer length because all its
     * bytes will be filled.
     * 
     * @param address
     *            Address to read from.
     * @param buf
     *            Buffer to read into.
     * @return Number of read bytes which is equals to buffer length if reading is successful.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public int read(long address, byte[] buf) throws MemoryIOException {
        return read(address, buf, 0, buf.length);
    }

    /**
     * Convenient method for reading given number of bytes from given address in memory into buffer
     * on given offset. If unwritten address is reached gap will be used instead and stored in
     * appropriate position in buffer. This means that if reading is successful return value will be
     * equals to buffer length because all its bytes will be filled.
     * 
     * @param address
     *            Address to read from.
     * @param buf
     *            Buffer to read into.
     * @param off
     *            Offset pointing to the buffer which is taken as start position to store read data.
     * @param length
     *            Number of bytes to be read from memory.
     * @return Number of read bytes which is equals to buffer length if reading is successful.
     * 
     * @throws MemoryIOException
     *             When any I/O operation fails.
     */
    public int read(long address, byte[] buf, int off, int length) throws MemoryIOException {
        checkAddress(address);
        checkAddress(address + length);

        int startUnit = getUnitIndex(address);
        int endUnit = getUnitIndex(address + length);

        long readPointer = address;
        int readBytes = 0;
        for (int i = startUnit; i <= endUnit; i++) {
            int offset = readBytes + off;
            int cnt = (int) Math.min(getHighestUnitAddress(i) - readPointer + 1, length - readBytes);

            MemoryUnit unit = mUnits.get(i);
            if (unit != null) {
                unit.read(readPointer, buf, offset, cnt);
            } else {
                Arrays.fill(buf, offset, offset + cnt, gap);
            }

            readBytes += cnt;
            readPointer += cnt;
        }

        return readBytes;
    }

    /**
     * Serves to find out if byte on given address has already been written.
     * 
     * @param address
     *            Address to check.
     * @return True if given address has already been written, false otherwise.
     */
    public boolean isReserved(long address) {
        MemoryUnit unit = mUnits.get(getUnitIndex(address));
        if (unit == null) {
            return false;
        } else {
            return unit.isReserved(address);
        }
    }

    /**
     * Will reset internal pointer for reading software payload blocks.
     */
    public void reset() {
        blockPointer = 0L;
    }

    /**
     * @return True if there are some unread sw payload blocks, false otherwise.
     */
    public boolean hasMorePayloadBlocks() {
        return (getReserved() > 0) && (blockPointer < getLastUsedAddress());
    }

    /**
     * @return Next software payload block if exists, null otherwise.
     */
    public IPayloadBlock getNextPayloadBlock() {
        if (blockPointer < getFirstUsedAddress()) {
            blockPointer = getFirstUsedAddress();
        } else if (blockPointer >= getLastUsedAddress()) {
            return null;
        }

        boolean reserved = isReserved(blockPointer);

        long areaStart = blockPointer;
        long areaEnd = getFirstFrom(areaStart, !reserved);

        long areaLength;

        if (areaEnd == getLastUsedAddress()) {
            areaLength = getLastUsedAddress() - areaStart;
        } else if (areaEnd > getLastUsedAddress()) {
            areaLength = getLastUsedAddress() - areaStart + 1;
        } else {
            areaLength = areaEnd - areaStart;
        }

        long blockEnd = areaStart;

        for (long i = 0; i < areaLength; i++) {
            blockEnd++;
            if ((blockEnd % PAYLOAD_BLOCK_SIZE == 0) || (i >= PAYLOAD_BLOCK_SIZE)) {
                break;
            }
        }

        blockPointer = blockEnd;
        return new PayloadBlock(this, areaStart, blockEnd - 1, reserved);
    }

    /**
     * @return Size of entire software image payload.
     */
    public long getPayloadSize() {
        return getLastUsedAddress() - getFirstUsedAddress() + 1;
    }

    /**
     * Serves to get access to written data via input stream. Stream will return bytes from the
     * first written address till last written address. Reading of unwritten addresses between them
     * will return gap filler.
     * 
     * @return Input data stream.
     */
    public InputStream getInputStream() {
        return new MemoryInputStream(this);
    }

    /**
     * @return Textual representation of this memory address range.
     */
    public String getAddressRange() {
        return "(" + HexUtilities.toHexString(0, true) + "," + HexUtilities.toHexString(getHighestAddress(), false)
            + ")";
    }

    /**
     * @return Size for internal memory unit which is the smallest space for allocation.
     */
    public int getUnitSize() {
        return unitSize;
    }

    /**
     * @return Byte used for filling gaps.
     */
    public byte getGap() {
        return gap;
    }

    /**
     * Free entire memory.
     */
    public void free() {
        reset();
        mUnits.clear();
    }

    private long getFirstFrom(long address, boolean reserved) {
        long pointer = address;

        long lastUsed = getLastUsedAddress();

        while (pointer <= lastUsed) {
            MemoryUnit unit = mUnits.get(getUnitIndex(pointer));
            if (unit != null) {
                pointer = unit.getFirstFrom(pointer, reserved);
                if (pointer == UNVALID_ADDRESS) {
                    return pointer;
                } else if ((pointer == unit.getLastAddress()) && (isReserved(pointer) ^ reserved)) {
                    pointer++;
                    continue;
                } else {
                    break;
                }
            } else if (reserved) {// trying to find first reserved so gaps should be skipped
                pointer = getHighestUnitAddress(getUnitIndex(pointer)) + 1;
            } else {
                pointer = Math.max(pointer, (getUnitIndex(pointer) + 1) * PAYLOAD_BLOCK_SIZE);
            }
        }

        /*
         * if (pointer >= lastUsed) { return lastUsed; } else { return pointer; }
         */
        return pointer;
    }

    private void checkAddress(long address) throws MemoryIOException {
        if ((address < 0) || (address > getHighestAddress())) {
            throw new MemoryIOException("Given address " + HexUtilities.toHexString(address)
                + " is out of valid range: " + getAddressRange());
        }
    }

    private int getUnitIndex(long address) {
        return (int) (address / unitSize);
    }

    private MemoryUnit getAllocatedUnit(int unitIndex) {
        if (mUnits.containsKey(unitIndex)) {
            return mUnits.get(unitIndex);
        } else {
            MemoryUnit mu;
            if (gap == DEFAULT_GAP_FILL) {
                mu = new MemoryUnit(unitIndex, unitSize);
            } else {
                mu = new MemoryUnit(unitIndex, unitSize, gap);
            }
            mUnits.put(unitIndex, mu);
            return mu;
        }
    }

    /**
     * @return Highest address of this memory addressing space.
     */
    public long getHighestAddress() {
        return (long) GIGA_BYTE * 4 - 1;
    }

    private long getHighestUnitAddress(int unitIndex) {
        return (long) unitSize * unitIndex + unitSize - 1;
    }

    private static class MemoryInputStream extends InputStream {

        private VirtualMemory virtualMemory;

        private long readPointer;

        public MemoryInputStream(VirtualMemory vm) {
            super();
            virtualMemory = vm;
            readPointer = virtualMemory.getFirstUsedAddress();
        }

        @Override
        public int available() throws IOException {
            int res = (int) (Integer.MAX_VALUE & (virtualMemory.getLastUsedAddress() - readPointer + 1));
            if (res > 0) {
                return res;
            } else {
                return 0;
            }
        }

        @Override
        public int read() throws IOException {
            return virtualMemory.read(readPointer++);
        }

        @Override
        public int read(byte[] b, int off, int len) throws IOException {
            int res = virtualMemory.read(readPointer, b, off, len);
            readPointer += res;
            return res;
        }

        @Override
        public int read(byte[] b) throws IOException {
            return read(b, 0, b.length);
        }

        @Override
        public void reset() throws IOException {
            readPointer = virtualMemory.getFirstUsedAddress();
        }

        @Override
        public boolean markSupported() {
            return false;
        }

    }

    private static class PayloadBlock implements IPayloadBlock {

        private VirtualMemory virtualMemory;

        private long offset;

        private int length;

        private byte[] positionInfo;

        private boolean reserved;

        public PayloadBlock(VirtualMemory vm, long start, long end, boolean containsData) {
            virtualMemory = vm;
            offset = start;
            length = (int) (end - start + 1);
            reserved = containsData;
            positionInfo = new byte[8];
            System.arraycopy(LittleEndianByteConverter.valueToByteArray((int) offset), 0, positionInfo, 0, 4);
            System.arraycopy(LittleEndianByteConverter.valueToByteArray(length), 0, positionInfo, 4, 4);
        }

        public byte[] getData() throws MemoryIOException {
            byte[] buf = new byte[length];
            virtualMemory.read(offset, buf);
            return buf;
        }

        public int getLength() {
            return length;
        }

        public long getOffset() {
            return offset;
        }

        public byte[] getPositionInfo() {
            return positionInfo;
        }

        public boolean containsData() {
            return reserved;
        }
    }
}
