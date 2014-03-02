package com.stericsson.sdk.common.memory;

/**
 * Instance of this class represents SW payload block, provide its offset, data and position info.
 * SW payload block is homogeneous block of data aligned on {@link VirtualMemory#PAYLOAD_BLOCK_SIZE}.
 * Homogenity of blocks is meant in sense of its content which is always either effective data or
 * gaps. Position info is byte array of length 8 where first four bytes represent offset and second
 * four bytes length in LittleEndian.
 * 
 * @author emicroh
 */
public interface IPayloadBlock {

    /**
     * @return Offset address pointing to the start of payload block in memory.
     */
    long getOffset();

    /**
     * Retrieve data for this block from memory and throw appropriate exception when reading memory
     * failed.
     * 
     * @return Data appropriate to this SW payload block.
     * @throws MemoryIOException
     *             When I/O operation failed during data retrieving.
     */
    byte[] getData() throws MemoryIOException;

    /**
     * @return True if this block contains effective data, false means that this blocks is empty and
     *         contains only gaps.
     */
    boolean containsData();

    /**
     * @return Length of this SW payload block which could be at maximum equals to
     *         {@link VirtualMemory#PAYLOAD_BLOCK_SIZE}.
     */
    int getLength();

    /**
     * @return Represents offset and length of this block in 8B byte array where first four bytes
     *         are address and second four bytes length in LittleEndian.
     */
    byte[] getPositionInfo();
}
