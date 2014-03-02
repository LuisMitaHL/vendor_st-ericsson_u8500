package com.stericsson.sdk.common.memory;

import static com.stericsson.sdk.common.memory.VirtualMemory.UNVALID_ADDRESS;

import java.util.Arrays;

import com.stericsson.sdk.common.HexUtilities;

class MemoryUnit {

    private int unitIndex;

    private int size;

    private byte[] data;

    private byte[] reservedBytes;

    private int reserved;

    private byte gapFill = 0x00;

    public MemoryUnit(int blockNumber, int unitSize, byte gap) {
        this(blockNumber, unitSize);
        gapFill = gap;
        if (gapFill != 0) { // zero is default value for native array
            Arrays.fill(data, gapFill);
        }
    }

    public MemoryUnit(int blockNumber, int unitSize) {
        unitIndex = blockNumber;
        size = unitSize;
        data = new byte[unitSize];
        reservedBytes = new byte[size / 8 + ((size % 8 > 0) ? 1 : 0)];
    }

    public void write(long address, byte[] buf, int offset, int length) throws MemoryIOException {
        int addr = getRelativeAddress(address);
        reserve(addr, length, false);
        System.arraycopy(buf, offset, data, addr, length);
    }

    public void update(long address, byte[] buf, int offset, int length) throws MemoryIOException {
        int addr = getRelativeAddress(address);
        reserve(addr, length, true);
        System.arraycopy(buf, offset, data, addr, length);
    }

    public void free(long address, int length) throws MemoryIOException {
        int addr = getRelativeAddress(address);
        unreserve(addr, length, true);
        Arrays.fill(data, addr, addr + length, gapFill);
    }

    public int read(long address, byte[] buffer, int offset, int length) throws MemoryIOException {
        int addr = getRelativeAddress(address);
        int bytesToRead = Math.min(getCapacityFrom(addr), length);
        System.arraycopy(data, addr, buffer, offset, bytesToRead);
        return bytesToRead;
    }

    private void reserve(int addr, int length, boolean ignoreReserved) throws MemoryIOException {
        if (addr + length - 1 > getLastAddress()) {
            throw new MemoryIOException("Data to reserve exceeds memory unit adress range: " + getAddressRange());
        }
        for (int i = addr; i < addr + length; i++) {
            try {
                setReservedBit(i, true);
            } catch (MemoryIOException e) {
                if (ignoreReserved) {
                    continue;
                } else {
                    throw e;
                }
            }
        }
    }

    private void unreserve(int addr, int length, boolean ignoreUnreserved) throws MemoryIOException {
        if (addr + length - 1 > getLastAddress()) {
            throw new MemoryIOException("Data to unreserve exceeds memory unit adress range: " + getAddressRange());
        }
        for (int i = addr; i < addr + length; i++) {
            try {
                setReservedBit(i, false);
            } catch (MemoryIOException e) {
                if (ignoreUnreserved) {
                    continue;
                } else {
                    throw e;
                }
            }
        }
    }

    private void setReservedBit(int bytePos, boolean bitFlag) throws MemoryIOException {
        int mask = 1 << (bytePos % 8);
        if (bitFlag) {
            if (isByteReserved(bytePos)) {
                throw new MemoryIOException("Memory reservation error, byte on address "
                    + getAbsoluteHexAddress(bytePos) + " has already been reserved!");
            } else {
                reservedBytes[bytePos / 8] |= mask;
                reserved++;
            }
        } else {
            if (!isByteReserved(bytePos)) {
                throw new MemoryIOException("Memory deallocation error, byte on address "
                    + getAbsoluteHexAddress(bytePos) + " is not allocated!");
            } else {
                reservedBytes[bytePos / 8] = (byte) ~(~reservedBytes[bytePos / 8] | mask);
                reserved--;
                data[bytePos] = 0;
            }
        }
    }

    public boolean isReserved(long address) {
        try {
            return isByteReserved(getRelativeAddress(address));
        } catch (MemoryIOException e) {
            return false;
        }
    }

    private boolean isByteReserved(int bytePos) {
        return (reservedBytes[bytePos / 8] & (1 << (bytePos % 8))) > 0;
    }

    private int getRelativeAddress(long address) throws MemoryIOException {
        long addr = address - getOffsetAddress();
        if ((addr < 0) || (addr > (int) (size - 1))) {
            throw new MemoryIOException("Address " + HexUtilities.toHexString(address)
                + " is out of valid address range " + getAddressRange());
        } else {
            return (int) (address - getOffsetAddress());
        }
    }

    private String getAbsoluteHexAddress(int pos) {
        return HexUtilities.toHexString(getAbsoluteAddress(pos));
    }

    private Long getAbsoluteAddress(int pos) {
        return getOffsetAddress() + pos;
    }

    private int getCapacityFrom(int relAddress) {
        return size - relAddress;
    }

    public long getFirstUsedAddress() {
        if (reserved == 0) {
            return UNVALID_ADDRESS;
        }

        int tmp = 0;
        for (int i = 0; i < reservedBytes.length; i++) {
            if (reservedBytes[i] != 0) {
                tmp = i;
                break;
            }
        }

        for (int i = 0; i < 8; i++) {
            if ((reservedBytes[tmp] & (1 << i)) > 0) {
                return getOffsetAddress() + tmp * 8 + i;
            }
        }

        return UNVALID_ADDRESS;
    }

    public long getLastUsedAddress() {
        if (reserved == 0) {
            return UNVALID_ADDRESS;
        }

        int tmp = 0;
        for (int i = reservedBytes.length - 1; i >= 0; i--) {
            if (reservedBytes[i] != 0) {
                tmp = i;
                break;
            }
        }

        for (int i = 7; i >= 0; i--) {
            if ((reservedBytes[tmp] & (1 << i)) > 0) {
                return getOffsetAddress() + tmp * 8 + i;
            }
        }

        return UNVALID_ADDRESS;
    }

    public long getFirstFrom(long address, boolean isReserved) {
        int relAddress;
        int bytePos;
        int bitPos;
        int lastAddress;

        try {
            lastAddress = getRelativeAddress(getLastAddress());
            relAddress = getRelativeAddress(address);
            bytePos = relAddress / 8;
            bitPos = relAddress % 8;
        } catch (MemoryIOException e) {
            return UNVALID_ADDRESS;
        }

        out: for (int i = bytePos; i < reservedBytes.length; i++) {
            for (int j = bitPos; j < 8; j++) {
                if (relAddress >= lastAddress) {
                    break out;
                } else if (!(((reservedBytes[i] & (1 << j)) > 0) ^ isReserved)) {
                    break out;
                } else {
                    relAddress++;
                }
            }
            bitPos = 0;
        }

        return getOffsetAddress() + relAddress;
    }

    public long getOffsetAddress() {
        return (long) unitIndex * size;
    }

    public long getLastAddress() {
        return getOffsetAddress() + size - 1;
    }

    public String getAddressRange() {
        return "(" + HexUtilities.toHexString(getOffsetAddress()) + "," + HexUtilities.toHexString(getLastAddress())
            + ")";
    }

    public int getUnitIndex() {
        return unitIndex;
    }

    public int getSize() {
        return size;
    }

    public byte[] getData() {
        return data;
    }

    public byte[] getReservedBytes() {
        return reservedBytes;
    }

    public int getReserved() {
        return reserved;
    }

}
