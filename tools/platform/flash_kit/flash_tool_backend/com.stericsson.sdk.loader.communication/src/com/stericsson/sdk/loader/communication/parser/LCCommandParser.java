package com.stericsson.sdk.loader.communication.parser;

import java.nio.ByteBuffer;

import com.stericsson.sdk.common.LittleEndianByteConverter;

/**
 * 
 * @author xolabju
 * 
 */
public abstract class LCCommandParser {

    /** sync */
    public static final byte SYNC = (byte) 0xAA;

    /** reserved */
    public static final byte[] RESERVED = new byte[] {
        (byte) 0xEE, (byte) 0xEE};

    /** flag no checksum */
    public static final byte CHECKSUM_NONE = (byte) 0x00;

    /** flag checksum is the 32 LSB of the payload field hashed with SHA256 */
    public static final byte CHECKSUM_SHA256 = (byte) 0x01;

    /** Command protocol */
    public static final byte PROTOCOL_COMMAND = (byte) 0xBC;

    /** Bulk protocol */
    public static final byte PROTOCOL_BULK = (byte) 0xBD;

    /** header length */
    public static final int HEADER_LENGTH = 16;

    /** sync offset */
    public static final int SYNC_OFFSET = 0;

    /** Protocol offset */
    public static final int PROT_OFFSET = 1;

    /** Reserved offset */
    public static final int RESERVED_OFFSET = 2;

    /** flags offset */
    public static final int FLAGS_OFFSET = 4;

    /** extended header length offset */
    public static final int EXT_HDR_LENGTH_OFFSET = 5;

    /** extended header checksum offset */
    public static final int EXT_HDR_CHECKSUM_OFFSET = 6;

    /** payload length offset */
    public static final int PAYLOAD_LENGTH_OFFSET = 7;

    /** payload checksum offset */
    public static final int PAYLOAD_CHECKSUM_OFFSET = 11;

    /** header checksum */
    public static final int HDR_CHECKSUM = 15;

    /** payload field length */
    public static final int PAYLOAD_LENGTH = 4;

    /** payload hash field length */
    public static final int PAYLOAD_HASH_LENGTH = 4;

    /** */
    protected int flags = 1;

    /** protocol */
    protected int protocol = 0;

    /** header */
    protected byte[] header = new byte[HEADER_LENGTH];

    /** ext header */
    protected byte[] extHeader = null;

    /** packet payload */
    protected byte[] payload;

    /** command */
    protected byte command = 0;

    /** group */
    protected byte group = 0;

    int session = 0;

    /**
     * get the parser according to the specified rawdata
     * 
     * @param rawData
     *            the data
     * 
     */
    public void parse(byte[] rawData) {
        parseHeader(rawData);
        parseExtendedHeader(rawData);
    }

    /**
     * parse header
     */
    private void parseHeader(byte[] rawData) {

        byte[] payloadField = new byte[PAYLOAD_LENGTH];

        System.arraycopy(rawData, 0, header, 0, EXT_HDR_CHECKSUM_OFFSET);

        System.arraycopy(rawData, EXT_HDR_CHECKSUM_OFFSET + 1, header, EXT_HDR_CHECKSUM_OFFSET + 1, HEADER_LENGTH
            - (EXT_HDR_CHECKSUM_OFFSET + 1));
        protocol = header[PROT_OFFSET];

        System.arraycopy(header, PAYLOAD_LENGTH_OFFSET, payloadField, 0, PAYLOAD_LENGTH);

        int payloadLength = LittleEndianByteConverter.byteArrayToInt(payloadField);


        if (payloadLength != 0) {
            payload = new byte[payloadLength];
        }

    }

    /**
     * decode raw data to extended header
     * 
     * @param eHeader
     *            the extended header
     */
    public abstract void parseExtendedHeader(byte[] eHeader);

    /**
     * Returns the payload data held by the parser.
     * 
     * @return The data in a byte array, with zero bytes if there is no data
     */
    public byte[] getPayload() {
        return payload;
    }

    /**
     * Sets the data part of the packet.
     * 
     * @param packetPayload
     *            The data to be held by the packet.
     */
    public void setpayload(byte[] packetPayload) {
        payload = packetPayload;

    }

    /**
     * Returns the number of bytes held in the data part.
     * 
     * @return The number of data bytes.
     */
    public int getDataSize() {
        if (payload == null) {
            return 0;
        }
        return payload.length;
    }

    /**
     * Creates a serial representation of the complete network packet.
     * 
     * @return A byte array that can be sent on a network.
     */
    public abstract byte[] getBytes();

    /**
     * Retrieves the packet's session number.
     * 
     * @return The session number or 0 if it does not have one.
     */
    public int getSessionNumber() {
        return session;
    }

    /**
     * Sets the packet's session number.
     * 
     * @param number
     *            The number to assign the packet.
     */
    public void setSessionNumber(int number) {
        session = number;
    }

    /**
     * Returns a string representation of the packet
     * 
     * @return a string representation of the packet
     */
    public abstract String toString();

    /**
     * Returns the protocol type of the data.
     * 
     * @return The protocol identifier.
     */
    public int getProtocol() {
        return protocol;

    }

    /**
     * Sets the protocol of the data held by the packet.
     * 
     * @param prot
     *            The protocol type.
     */
    public void setProtocol(int prot) {
        protocol = prot;
    }

    /**
     * calculate header check sum
     * 
     * @param hdr
     *            the header to calculate
     * @return the checksum
     */
    public static byte calculateHeaderChecksum(byte[] hdr) {

        byte checkSum = 0;

        if (hdr.length == 1) {
            return hdr[0];
        }

        for (int i = 0; i < hdr.length - 1; i++) {
            checkSum ^= hdr[i];
        }

        return checkSum;

    }

    private byte[] calculatePayloadChecksum(byte[] pay) {

        if (pay == null) {
            return new byte[4];
        }
        // this is not implemented in the loader yet
        byte checkSum = 0;
        byte[] temp = new byte[PAYLOAD_HASH_LENGTH];
        for (int i = 0; i < pay.length; i++) {
            checkSum ^= pay[i];
        }
        temp[0] = checkSum;
        return temp;

    }

    /**
     * calculate header check sum
     * 
     * @param hdr
     *            the header to calculate
     * @return the checksum
     */
    public static byte calculateExtHeaderChecksum(byte[] hdr) {

        byte checkSum = 0;

        if (hdr.length == 1) {
            return hdr[0];
        }

        for (int i = 0; i < hdr.length; i++) {
            checkSum ^= hdr[i];
        }

        return checkSum;

    }


    /**
     * creates a header
     * 
     * @return a serial representation of a header
     */
    public byte[] getHeaderAsBytes() {

        ByteBuffer bb = ByteBuffer.allocate(HEADER_LENGTH);
        bb.put(SYNC);
        bb.put((byte) protocol);
        bb.put(RESERVED);
        bb.put((byte) flags);
        bb.put((byte) extHeader.length);
        bb.put(calculateExtHeaderChecksum(extHeader));
        bb.put(LittleEndianByteConverter.intToByteArray(getDataSize(), 4));
        bb.put(calculatePayloadChecksum(payload));

        // copy header part except crc
        System.arraycopy(bb.array(), 0, header, 0, HEADER_LENGTH - 1);
        bb.put(calculateHeaderChecksum(header));

        return bb.array();
    }

}
