package com.stericsson.sdk.loader.communication.test;

import static org.junit.Assert.assertArrayEquals;

import java.io.File;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.loader.communication.parser.BulkProtocolParser;
import com.stericsson.sdk.loader.communication.parser.CommandProtocolParser;
import com.stericsson.sdk.loader.communication.parser.LCCommandParser;
import com.stericsson.sdk.loader.communication.parser.LCGDFSParser;
import com.stericsson.sdk.loader.communication.parser.ParserAbstractFactory;

/**
 * @author xdancho
 */
public class ParserTest extends TestCase {

    // session 17, no payload, state command, cmd:9, grp:3

    byte[] command = new byte[] {
        (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x04,

        (byte) 0x4E,

        (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,

        (byte) 0x5D,

        (byte) 0x44, (byte) 0x00, (byte) 0x09, (byte) 0x03};

    // session 7, state ACK, cmd:3, grp:2
    byte[] commandACK = new byte[] {
        (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x04,

        (byte) 0x1C, // EXT HDR CRC

        (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,

        (byte) 0xF, // HDR CRC
        (byte) 0x1D, (byte) 0x00, (byte) 0x03, (byte) 0x02}; // EXT HDR

    // session 17, state GR, cmd:9, grp:3 rawData:
    // AABCEEEE01044C00000000000000005F46000903
    byte[] commandGR = new byte[] {
        (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x04, (byte) 0x4C, // EXT
        // HDR
        // CRC

        (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,

        (byte) 0x5F, // HDR CRC

        (byte) 0x46, (byte) 0x00, (byte) 0x09, (byte) 0x03}; // EXT HDR

    // session 7, state GRACK, payload 3, cmd:9, grp:3 rawData:
    // AABCEEEE01041503000000DD000000D81F000903AABBCC
    byte[] commandGRACK =
        new byte[] {
            (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x04,

            (byte) 0x15, // EXT HDR CRC
            (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xDD, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0xD8, // HDR CRC
            (byte) 0x1F, (byte) 0x00, (byte) 0x09, (byte) 0x03, (byte) 0xAA, (byte) 0xBB, (byte) 0xCC};

    byte[] pay = new byte[] {
        (byte) 0xAA, (byte) 0xBB, (byte) 0xCC};

    /**
     * 
     */
    @Test
    public void testGetBytesFromCMDParser() {

        // command:
        // int commandCRC =
        // LCCommandParser.calculateHeaderChecksum(new byte[] {
        // (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x00, (byte) 0x04, (byte)
        // 0x4E, (byte) 0x00,
        // (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte)
        // 0x00}); // 0x5C
        //
        // int testCRC =
        // LCCommandParser.calculateHeaderChecksum(new byte[] {
        // (byte) 0xAA, (byte) 0xBC, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x04, (byte)
        // 0x1E, (byte) 0x00,
        // (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte)
        // 0x00});// 0D

        // session 17, no payload, state command, cmd:9, grp:3
        CommandProtocolParser parser =
            (CommandProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_COMMAND);

        parser.setCommand((byte) 0x09);
        parser.setGroup((byte) 0x03);
        parser.setState(CommandProtocolParser.STATE_COMMAND_SENT);
        parser.setSessionNumber(17);

        assertArrayEquals(command, parser.getBytes());

        // session 7, state ACK, cmd:3, grp:2
        parser = (CommandProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_COMMAND);

        parser.setCommand((byte) 0x03);
        parser.setGroup((byte) 0x02);
        parser.setState(CommandProtocolParser.STATE_COMMAND_ACK);
        parser.setSessionNumber(7);

        assertArrayEquals(commandACK, parser.getBytes());

        // session 17, state GR, cmd:9, grp:3
        parser = (CommandProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_COMMAND);

        parser.setCommand((byte) 0x09);
        parser.setGroup((byte) 0x03);
        parser.setState(CommandProtocolParser.STATE_GR_RECEIVED);
        parser.setSessionNumber(17);

        assertArrayEquals(commandGR, parser.getBytes());

        // session 7, state GRACK, payload 3, cmd:9, grp:3
        parser = (CommandProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_COMMAND);

        parser.setCommand((byte) 0x09);
        parser.setGroup((byte) 0x03);
        parser.setpayload(pay);
        parser.setState(CommandProtocolParser.STATE_GR_ACK);
        parser.setSessionNumber(7);

        assertArrayEquals(commandGRACK, parser.getBytes());
    }

    /**
     * 
     */
    @Test
    public void testGetCMDFromBytesCMDParser() {

        // Parse command: no payload, state command, cmd:9, grp:3

        LCCommandParser parser = ParserAbstractFactory.getParser(command);
        parser.parse(command);

        if (parser instanceof CommandProtocolParser) {
            CommandProtocolParser cmdParser = (CommandProtocolParser) parser;
            assertEquals(0, cmdParser.getDataSize());
            assertEquals(LCCommandParser.PROTOCOL_COMMAND, cmdParser.getProtocol());
            assertEquals((byte) 0x09, cmdParser.getCommand());
            assertEquals((byte) 0x03, cmdParser.getGroup());
            assertEquals(CommandProtocolParser.STATE_COMMAND_SENT, cmdParser.getState());
            assertEquals((byte) 0x11, cmdParser.getSessionNumber());
        } else {
            fail("should be instanceof: CommandProtocolParser");
        }

        // parse commandACK: session 7, state ACK, cmd:3, grp:2

        parser = ParserAbstractFactory.getParser(commandACK);
        parser.parse(commandACK);

        if (parser instanceof CommandProtocolParser) {
            CommandProtocolParser cmdParser = (CommandProtocolParser) parser;
            assertEquals(0, cmdParser.getDataSize());
            assertEquals(LCCommandParser.PROTOCOL_COMMAND, cmdParser.getProtocol());
            assertEquals((byte) 0x03, cmdParser.getCommand());
            assertEquals((byte) 0x02, cmdParser.getGroup());
            assertEquals(CommandProtocolParser.STATE_COMMAND_ACK, cmdParser.getState());
            assertEquals((byte) 0x07, cmdParser.getSessionNumber());
        } else {
            fail("should be instanceof: CommandProtocolParser");
        }

        // parse commandGR: session 17, state GR, cmd:9, grp:3

        parser = ParserAbstractFactory.getParser(commandGR);
        parser.parse(commandGR);

        if (parser instanceof CommandProtocolParser) {
            CommandProtocolParser cmdParser = (CommandProtocolParser) parser;
            assertEquals(0, cmdParser.getDataSize());
            assertEquals(LCCommandParser.PROTOCOL_COMMAND, cmdParser.getProtocol());
            assertEquals((byte) 0x09, cmdParser.getCommand());
            assertEquals((byte) 0x03, cmdParser.getGroup());
            assertEquals(CommandProtocolParser.STATE_GR_RECEIVED, cmdParser.getState());
            assertEquals((byte) 0x11, cmdParser.getSessionNumber());
        } else {
            fail("should be instanceof: CommandProtocolParser");
        }

        // parse commandGRACK: session 7, state GRACK, payload 3, cmd:9, grp:3

        parser = ParserAbstractFactory.getParser(commandGRACK);
        parser.parse(commandGRACK);

        if (parser instanceof CommandProtocolParser) {
            CommandProtocolParser cmdParser = (CommandProtocolParser) parser;
            assertEquals(pay.length, cmdParser.getDataSize());
            assertArrayEquals(pay, cmdParser.getPayload());
            assertEquals(LCCommandParser.PROTOCOL_COMMAND, cmdParser.getProtocol());
            assertEquals((byte) 0x09, cmdParser.getCommand());
            assertEquals((byte) 0x03, cmdParser.getGroup());
            assertEquals(CommandProtocolParser.STATE_GR_ACK, cmdParser.getState());
            assertEquals((byte) 0x07, cmdParser.getSessionNumber());
        } else {
            fail("should be instanceof: CommandProtocolParser");
        }

    }

    // read packet , session 1, extField 0, chunk size 1346, offset 8189, length
    // 8072
    byte[] readPacket =
        new byte[] {

            (byte) 0xAA, (byte) 0xBD, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x14, (byte) 0x32, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x30,
            (byte) 0x01, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x42, (byte) 0x05, (byte) 0x00, (byte) 0x00,
            (byte) 0xFD, (byte) 0x1F, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x88, (byte) 0x1F, (byte) 0x00, (byte) 0x00

        };

    // read packet, session 1, extField 6, chunk size 1346, offset 8189, length
    // 8072, payload length 6
    // rawData:
    // AABDEEEE0114340600000001000000310100010642050000FD1F000000000000881F0000000102030405
    byte[] readPacketWithAcks =
        new byte[] {
            (byte) 0xAA, (byte) 0xBD, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x14, (byte) 0x34, (byte) 0x06,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01,
            (byte) 0x00,
            (byte) 0x00,
            (byte) 0x00, // PAYLOAD CHECKSUM
            (byte) 0x31,

            (byte) 0x01, (byte) 0x00, (byte) 0x01, (byte) 0x06, (byte) 0x42, (byte) 0x05, (byte) 0x00, (byte) 0x00,
            (byte) 0xFD, (byte) 0x1F, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x88, (byte) 0x1F, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x02, (byte) 0x03,
            (byte) 0x04, (byte) 0x05 // payload

        };

    byte[] readPayload = new byte[] {
        (byte) 0x00, (byte) 0x01, (byte) 0x02, (byte) 0x03, (byte) 0x04, (byte) 0x05};

    // write packet, session 1, extField 0, chunk size 8, offset 0, length 46
    byte[] writePacket =
        new byte[] {
            (byte) 0xAA, (byte) 0xBD, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x14, (byte) 0x24, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x26,
            (byte) 0x01, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x08, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x2E, (byte) 0x00, (byte) 0x00, (byte) 0x00};

    // data packet, session 1, extField 0, chunk size 8, offset 0, length 46,
    // payload length 8
    byte[] dataPacket =
        new byte[] {
            (byte) 0xAA, (byte) 0xBD, (byte) 0xEE, (byte) 0xEE, (byte) 0x01, (byte) 0x14, (byte) 0x25, (byte) 0x08,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0x00,

            (byte) 0x2A, (byte) 0x01, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x08, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x2E, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02,
            (byte) 0x00, (byte) 0x04, (byte) 0x00, (byte) 0x31, (byte) 0x32 // payload
        };

    byte[] bulkData = new byte[] {
        (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x04, (byte) 0x00, (byte) 0x31, (byte) 0x32};

    /**
     * 
     */
    @Test
    public void testGetBytesFromBULKParser() {

        // read packet , session 1, extField 0, chunk size 1346, offset 8189,
        // length 8072
        BulkProtocolParser parser = (BulkProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_BULK);
        parser.setPacketType((byte) BulkProtocolParser.PACKET_TYPE_READ);
        parser.setSessionNumber(1);
        parser.setChunkExt((byte) 0);
        parser.setChunkSize(1346);
        parser.setOffset(8189);
        parser.setLength(8072);
        assertArrayEquals(readPacket, parser.getBytes());

        // read packet, session 1, extField 6, chunk size 1346, offset 8189,
        // length 8072, payload length 6
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_BULK);
        parser.setPacketType((byte) BulkProtocolParser.PACKET_TYPE_READ);
        parser.setSessionNumber(1);
        parser.setChunkExt((byte) 6);
        parser.setChunkSize(1346);
        parser.setOffset(8189);
        parser.setLength(8072);
        parser.setpayload(readPayload);
        assertArrayEquals(readPacketWithAcks, parser.getBytes());

        // write packet, session 1, extField 0, chunk size 8, offset 0, length
        // 46
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_BULK);
        parser.setPacketType((byte) BulkProtocolParser.PACKET_TYPE_WRITE_REQUEST);
        parser.setSessionNumber(1);
        parser.setChunkExt((byte) 0);
        parser.setChunkSize(8);
        parser.setOffset(0);
        parser.setLength(46);
        assertArrayEquals(writePacket, parser.getBytes());

        // data packet, session 1, extField 0, chunk size 8, offset 0, length
        // 46, payload length 8
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(LCCommandParser.PROTOCOL_BULK);
        parser.setPacketType((byte) BulkProtocolParser.PACKET_TYPE_DATA);
        parser.setSessionNumber(1);
        parser.setChunkExt((byte) 0);
        parser.setChunkSize(8);
        parser.setOffset(0);
        parser.setLength(46);
        parser.setpayload(bulkData);
        assertArrayEquals(dataPacket, parser.getBytes());

    }

    /**
     * 
     */
    @Test
    public void testGetBULKFromBytesBULKParser() {

        // read packet , session 1, extField 0, chunk size 1346, offset 8189,
        // length 8072
        BulkProtocolParser parser = (BulkProtocolParser) ParserAbstractFactory.getParser(readPacket);
        parser.parse(readPacket);
        assertEquals(0, parser.getChunkExt());
        assertEquals(8189, parser.getOffset());
        assertEquals(1346, parser.getChunkSize());
        assertEquals(0, parser.getDataSize());
        assertEquals(8072, parser.getLength());
        assertEquals(BulkProtocolParser.PACKET_TYPE_READ, parser.getPacketType());
        assertNull(parser.getPayload());
        assertEquals(LCCommandParser.PROTOCOL_BULK, parser.getProtocol());
        assertEquals(1, parser.getSessionNumber());

        // read packet, session 1, extField 6, chunk size 1346, offset 8189,
        // length 8072, payload length 6
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(readPacketWithAcks);
        parser.parse(readPacketWithAcks);
        assertEquals(6, parser.getChunkExt());
        assertEquals(8189, parser.getOffset());
        assertEquals(1346, parser.getChunkSize());
        assertEquals(6, parser.getDataSize());
        assertEquals(8072, parser.getLength());
        assertArrayEquals(readPayload, parser.getPayload());
        assertEquals(BulkProtocolParser.PACKET_TYPE_READ, parser.getPacketType());
        assertEquals(LCCommandParser.PROTOCOL_BULK, parser.getProtocol());
        assertEquals(1, parser.getSessionNumber());

        // write packet, session 1, extField 0, chunk size 8, offset 0, length
        // 46
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(writePacket);
        parser.parse(writePacket);
        assertEquals(0, parser.getChunkExt());
        assertEquals(0, parser.getOffset());
        assertEquals(8, parser.getChunkSize());
        assertEquals(0, parser.getDataSize());
        assertEquals(46, parser.getLength());
        assertEquals(BulkProtocolParser.PACKET_TYPE_WRITE_REQUEST, parser.getPacketType());
        assertNull(parser.getPayload());
        assertEquals(LCCommandParser.PROTOCOL_BULK, parser.getProtocol());
        assertEquals(1, parser.getSessionNumber());

        // data packet, session 1, extField 0, chunk size 8, offset 0, length
        // 46, payload length 8
        parser = (BulkProtocolParser) ParserAbstractFactory.getParser(dataPacket);
        parser.parse(dataPacket);
        assertEquals(0, parser.getChunkExt());
        assertEquals(0, parser.getOffset());
        assertEquals(8, parser.getChunkSize());
        assertEquals(8, parser.getDataSize());
        assertEquals(46, parser.getLength());
        assertEquals(BulkProtocolParser.PACKET_TYPE_DATA, parser.getPacketType());
        assertArrayEquals(bulkData, parser.getPayload());
        assertEquals(LCCommandParser.PROTOCOL_BULK, parser.getProtocol());
        assertEquals(1, parser.getSessionNumber());

    }

    /**
     * 
     */
    public void testGDFParser() {

        File input = new File(Activator.getResourcesPath() + "/gdfsRead.gdf");
        File output = LCGDFSParser.convertToBinary(input);
        File tempGDF = new File(Activator.getResourcesPath() + "/gdfsRead2.gdf");
        LCGDFSParser.convertToGDF(output, tempGDF);
        // assertEquals(input.length(), tempGDF.length());
    }

    /**
     * 
     */
    @Test
    public void testGDFParserWithInvalidInput() {
        File input = new File(Activator.getResourcesPath());
        File output = LCGDFSParser.convertToBinary(input);
        assertNull(output);
    }

}
