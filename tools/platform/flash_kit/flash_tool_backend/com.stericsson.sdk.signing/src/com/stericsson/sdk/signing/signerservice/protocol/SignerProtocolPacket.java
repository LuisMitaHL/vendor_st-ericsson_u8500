package com.stericsson.sdk.signing.signerservice.protocol;

import java.util.zip.CRC32;

/**
 * Represents a communication packet sent between signing client and signing server.
 * 
 * @author xtomlju
 */
public class SignerProtocolPacket {

    /**
     * Command identifier.
     */
    public static final byte COMMAND_ACKNOWLEDGE = 0x07;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATAENCRYPTION_REQUEST = 0x0C;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATAENCRYPTION_RESPONSE = 0x0D;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_HEADERPACKAGE_REQUEST = 0x13;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_HEADERPACKAGE_RESPONSE = 0x14;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_HELLO = 0x09;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_KEYCATALOGUE_REQUEST = 0x0F;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_KEYCATALOGUE_RESPONSE = 0x0E;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_LOGIN_ACCEPT = 0x05;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_LOGIN_REJECT = 0x06;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_LOGIN_REQUEST = 0x01;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_NEG_ACKNOWLEDGE = 0x08;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_RANDOMNUMBER_REQUEST = 0x10;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_RANDOMNUMBER_RESPONSE = 0x11;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SECRET_REQUEST = 0x0A;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SECRET_RESPONSE = 0x0B;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SEND_DOMAIN = 0x04;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SEND_PASSWORD = 0x03;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SEND_USERNAME = 0x02;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_PKCS1_ENCRYPTION_REQUEST = 0x15;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_PKCS1_ENCRYPTION_RESPONSE = 0x16;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SESSIONCLOSED = 0x12;

    /**
     * 
     */
    public static final byte COMMAND_DATA_A2_ENCRYPTION_REQUEST = 0x17;

    /**
     * 
     */
    public static final byte COMMAND_DATA_A2_ENCRYPTION_RESPONSE = 0x18;

    /**
     * 
     */
    public static final byte COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_REQUEST = 0x19;

    /**
     * 
     */
    public static final byte COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_RESPONSE = 0x20;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U8500_ENCRYPTION_REQUEST = 0x23;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U8500_ENCRYPTION_RESPONSE = 0x24;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SUB_PACKAGE_BY_KEY_REQUEST = 0x25;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_SUB_PACKAGE_BY_KEY_RESPONSE = 0x26;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA = 0x27;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_RESPONSE = 0x28;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA1withRSA = 0x29;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA384withRSA = 0x30;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA512withRSA = 0x31;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA1withPSS = 0x32;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withPSS = 0x33;


    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA384withPSS = 0x34;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA512withPSS = 0x35;

    /**
     * Command identifier.
     */
    public static final byte COMMAND_USE_COMMON_FOLDER_REQUEST = 0x36;

    /**
     * 32 bit 101010....used as packet identifier.
     */
    public static final int HEADER_MAGIC = 0xAAAAAAAA;

    /**
     * Magic packet header array.
     */
    public static final byte[] HEADER_MAGIC_ARRAY = new byte[] {
        (byte) 0xAA, (byte) 0xAA, (byte) 0xAA, (byte) 0xAA};

    /**
     * Packet command.
     */
    private byte command;

    /**
     * Packet data.
     */
    private byte[] packetData = null;

    /**
     * Creates a new command packet.
     * 
     * @param cmd
     *            The packet command.
     * @param data
     *            The packet data.
     */
    public SignerProtocolPacket(byte cmd, byte[] data) {
        command = cmd;
        packetData = data;
    }

    /**
     * Returns the data in this packet.
     * 
     * @return The data.
     */
    public byte[] getData() {
        return packetData;
    }

    /**
     * Returns this packet as a byte array.
     * 
     * @return This packet.
     */
    public byte[] toArray() {
        /*
         * Format: MAGIC + command + <LENGTH> + length + CRC
         */
        byte[] abPacket = new byte[4 + 1 + 4 + packetData.length + 4];
        // int count = 0;
        abPacket[0] = (byte) ((HEADER_MAGIC >> 24) & 0xFF);
        abPacket[1] = (byte) ((HEADER_MAGIC >> 16) & 0xFF);
        abPacket[2] = (byte) ((HEADER_MAGIC >> 8) & 0xFF);
        abPacket[3] = (byte) (HEADER_MAGIC & 0xFF);
        abPacket[4] = command;
        abPacket[5] = (byte) ((packetData.length >> 24) & 0xFF);
        abPacket[6] = (byte) ((packetData.length >> 16) & 0xFF);
        abPacket[7] = (byte) ((packetData.length >> 8) & 0xFF);
        abPacket[8] = (byte) (packetData.length & 0xFF);
        System.arraycopy(packetData, 0, abPacket, 9, packetData.length);

        int crcBegin = abPacket.length - 4;
        CRC32 crc32 = new CRC32();
        crc32.update(abPacket, 4, abPacket.length - 8);

        long crc = crc32.getValue();
        abPacket[crcBegin] = (byte) ((crc >> 24) & 0xFF);
        abPacket[crcBegin + 1] = (byte) ((crc >> 16) & 0xFF);
        abPacket[crcBegin + 2] = (byte) ((crc >> 8) & 0xFF);
        abPacket[crcBegin + 3] = (byte) (crc & 0xFF);

        return abPacket;
    }

    /**
     * @return Protocol command
     */
    public byte getCommand() {
        return command;
    }
}
