package com.stericsson.sdk.loader.communication.parser;

import java.nio.ByteBuffer;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * 
 * @author xolabju
 * 
 */
public class BulkProtocolParser extends LCCommandParser {

    /** */
    public static final int PACKET_TYPE_STATUS = 0x00;
    /** */
    public static final int PACKET_TYPE_READ = 0x01;
    /** */
    public static final int PACKET_TYPE_DATA = 0x02;
    /** */
    public static final int PACKET_TYPE_WRITE_REQUEST = 0x03;

    private static final int BULK_EXT_HEADER_LENGTH = 20;

    private static final int BULK_EXT_HEADER_OFFSET = 16;

    private static final int SESSION_OFFSET = 0;

    private static final int TYPE_OFFSET = 2;

    private static final int CHUNK_EXT_OFFSET = 3;

    private static final int CHUNK_SIZE_OFFSET = 4;

    private static final int OFFSET_OFFSET = 8;

    private static final int LENGTH_OFFSET = 17;

    private static final int CHUNK_SIZE_SIZE = 4;

    private static final int OFFSET_SIZE = 8;

    private static final int LENGTH_SIZE = 4;

    private static final int SESSION_SIZE = 2;

    private byte packetType = 0;

    private byte chunkExt = 0;

    private int chunkSize = 0;

    private long offset = 0;

    private int length = 0;

    /**
     * bulk protocol parser
     */
    protected BulkProtocolParser() {
        setProtocol(LCCommandParser.PROTOCOL_BULK);
        extHeader = new byte[BULK_EXT_HEADER_LENGTH];
    }

    /**
     * Sets the packet type
     * 
     * @param type
     *            the packet type
     */
    public void setPacketType(byte type) {
        packetType = type;
    }

    /**
     * Gets the packet type
     * 
     * @return the packet type
     */
    public int getPacketType() {
        return packetType;

    }

    /**
     * Sets the chunk size
     * 
     * @param size
     *            the chunk size
     */
    public void setChunkSize(int size) {
        chunkSize = size;
    }

    /**
     * Gets the chunk size
     * 
     * @return the chunk size
     */
    public int getChunkSize() {
        return chunkSize;

    }

    /**
     * set the extra field chunkID/pad/ACK
     * 
     * @param value
     *            the value for this field
     */
    public void setChunkExt(byte value) {
        chunkExt = value;
    }

    /**
     * get the extra field chunkID/pad/ACK
     * 
     * @return the value
     */
    public byte getChunkExt() {
        return chunkExt;
    }

    /**
     * set the length field
     * 
     * @param value
     *            the value
     */
    public void setLength(int value) {
        length = value;
    }

    /**
     * get the value of the length field
     * 
     * @return the value
     */
    public int getLength() {
        return length;
    }

    /**
     * Sets the offset
     * 
     * @param value
     *            the offset
     */
    public void setOffset(long value) {
        offset = value;
    }

    /**
     * Gets the offset
     * 
     * @return the offset
     */
    public long getOffset() {
        return offset;

    }

    /**
     * (non-Javadoc)
     * 
     * @see se.ericsson.eab.sdk.loader.communication.LCCommandParser#parseExtendedHeader(byte[])
     * @param rawData
     *            the packet to parse
     */
    @Override
    public void parseExtendedHeader(byte[] rawData) {
        System.arraycopy(rawData, BULK_EXT_HEADER_OFFSET, extHeader, 0,
                BULK_EXT_HEADER_LENGTH);
        setPacketType(extHeader[TYPE_OFFSET]);
        setChunkExt(extHeader[CHUNK_EXT_OFFSET]);
        session = getSessionFromBytes();
        chunkSize = getChunkSizeFromBytes();
        offset = getOffsetFromBytes();
        length = getLengthFromBytes();

        // if we have payload
        if (payload != null) {
            System.arraycopy(rawData, HEADER_LENGTH + BULK_EXT_HEADER_LENGTH,
                    payload, 0, payload.length);
        }

    }

    /**
     * (non-Javadoc)
     * 
     * @see se.ericsson.eab.sdk.loader.communication.LCCommandParser#toString()
     * @return tbd
     */
    @Override
    public String toString() {
        // TODO Auto-generated method stub
        return null;
    }

    /**
     * (non-Javadoc)
     * 
     * @see se.ericsson.eab.sdk.loader.communication.LCCommandParser#getBytes()
     * @return serial representation of a bulk command
     * 
     */
    @Override
    public byte[] getBytes() {
        ByteBuffer eHeader = ByteBuffer.allocate(BULK_EXT_HEADER_LENGTH);
        eHeader.put(getSessionFromAsBytes());
        eHeader.put(packetType);
        eHeader.put(chunkExt);
        eHeader.put(getChunkSizeAsBytes());
        eHeader.put(getOffsetAsBytes());
        eHeader.put(getLengthAsBytes());

        extHeader = eHeader.array();

        ByteBuffer bb = ByteBuffer.allocate(HEADER_LENGTH
                + BULK_EXT_HEADER_LENGTH + getDataSize());

        bb.put(getHeaderAsBytes());
        bb.put(extHeader);

        if (getDataSize() != 0) {
            bb.put(payload);
        }

        return bb.array();

    }

    private int getSessionFromBytes() {
        byte[] ses = new byte[SESSION_SIZE];
        System.arraycopy(extHeader, SESSION_OFFSET, ses, 0, SESSION_SIZE);
        return LittleEndianByteConverter.byteArrayToInt(ses);
    }

    private long getOffsetFromBytes() {
        byte[] off = new byte[OFFSET_SIZE];
        System.arraycopy(extHeader, OFFSET_OFFSET, off, 0, OFFSET_SIZE);
        return LittleEndianByteConverter.byteArrayToLong(off);
    }

    private int getLengthFromBytes() {
        byte[] len = new byte[LENGTH_SIZE];
        System.arraycopy(extHeader, LENGTH_OFFSET - 1, len, 0, LENGTH_SIZE);
        return LittleEndianByteConverter.byteArrayToInt(len);
    }

    private int getChunkSizeFromBytes() {
        byte[] chunkS = new byte[CHUNK_SIZE_SIZE];
        System.arraycopy(extHeader, CHUNK_SIZE_OFFSET, chunkS, 0,
                CHUNK_SIZE_SIZE);
        return LittleEndianByteConverter.byteArrayToInt(chunkS);

    }

    private byte[] getSessionFromAsBytes() {
        return LittleEndianByteConverter.intToByteArray(session, SESSION_SIZE);
    }

    private byte[] getChunkSizeAsBytes() {
        return LittleEndianByteConverter.intToByteArray(chunkSize,
                CHUNK_SIZE_SIZE);

    }

    private byte[] getOffsetAsBytes() {
        return LittleEndianByteConverter.longTo8Bytes(offset);
    }

    private byte[] getLengthAsBytes() {
        return LittleEndianByteConverter.intToByteArray(length, LENGTH_SIZE);
    }

}
