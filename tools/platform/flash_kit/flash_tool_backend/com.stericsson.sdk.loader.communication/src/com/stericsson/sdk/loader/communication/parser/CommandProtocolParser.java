package com.stericsson.sdk.loader.communication.parser;

import java.nio.ByteBuffer;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.loader.communication.LCCommandMappings;

/**
 * 
 * @author xolabju
 * 
 */
public class CommandProtocolParser extends LCCommandParser {

    /** */
    public static final int STATE_COMMAND_SENT = 0x00;
    /** */
    public static final int STATE_COMMAND_ACK = 0x01;
    /** */
    public static final int STATE_GR_RECEIVED = 0x02;
    /** */
    public static final int STATE_GR_ACK = 0x03;

    /** */
    public static final int SESSION_STATE_OFFSET = 0;

    /** */
    public static final int COMMAND_OFFSET = 2;

    /** */
    public static final int GROUP_OFFSET = 3;

    private int commandState = 0;

    private static final int CMD_EXT_HEADER_OFFSET = 16;

    private static final int CMD_EXT_HEADER_LENGTH = 4;

    /**
     * command protocol parser
     */
    protected CommandProtocolParser() {
        setProtocol(LCCommandParser.PROTOCOL_COMMAND);
        extHeader = new byte[CMD_EXT_HEADER_LENGTH];
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
     * get the command
     * @return
     *      the command
     */
    public byte getCommand() {
        // TODO Auto-generated method stub
        return command;
    }

    /**
     * get the group value
     * @return
     *      the group value
     */
    public byte getGroup() {

        return group;
    }

    /**
     * set command value
     * @param cmd
     *          the command value
     */
    public void setCommand(byte cmd) {
        command = cmd;

    }

    /**
     * set group
     * @param grp
     *      the group value
     */
    public void setGroup(byte grp) {
        group = grp;

    }

    /**
     * Sets the state identifier
     * 
     * @param state
     *            the state identifier
     */
    public void setState(int state) {
        commandState = state;
    }

    /**
     * Returns the state identifier
     * 
     * @return the state identifier
     */
    public int getState() {
        return commandState;
    }

    /**
     * Returns a brief description of the command
     * 
     * @return a brief description of the command
     */
    public  String getCommandDescription() {

        return LCCommandMappings.getDescription(group, command);

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

        System.arraycopy(rawData, CMD_EXT_HEADER_OFFSET, extHeader, 0,
                CMD_EXT_HEADER_LENGTH);
        session = getSessionFromByte(extHeader);
        commandState = getStateFromByte(extHeader);
        setGroup(extHeader[GROUP_OFFSET]);
        setCommand(extHeader[COMMAND_OFFSET]);

        // if we have payload
        if (payload != null) {
            System.arraycopy(rawData, HEADER_LENGTH + CMD_EXT_HEADER_LENGTH,
                    payload, 0, payload.length);
        }
    }

    private int getValueFromSessionState(byte[] extHeaderIn) {

        byte[] sessionState = new byte[2];
        System.arraycopy(extHeaderIn, SESSION_STATE_OFFSET, sessionState, 0,
                sessionState.length);

        return LittleEndianByteConverter.byteArrayToInt(sessionState);
    }

    private int getSessionFromByte(byte[] sessionState) {

        return (getValueFromSessionState(sessionState) >> 2);

    }

    private int getStateFromByte(byte[] sessionState) {
        return (getValueFromSessionState(sessionState) & 0x0003);
    }

    private byte[] getSessionStateAsBytes() {

        // int to byte[2]
        int value = session << 2;
        value += commandState;
        return LittleEndianByteConverter.intToByteArray(value, 2);

    }

    /**
     * (non-Javadoc)
     * 
     * @see se.ericsson.eab.sdk.loader.communication.LCCommandParser#getBytes()
     * @return serial representation of a command
     * 
     */
    @Override
    public byte[] getBytes() {

        ByteBuffer eHeader = ByteBuffer.allocate(CMD_EXT_HEADER_LENGTH);
        eHeader.put(getSessionStateAsBytes());
        eHeader.put(getCommand());
        eHeader.put(getGroup());

        extHeader = eHeader.array();

        ByteBuffer bb = ByteBuffer.allocate(HEADER_LENGTH
                + CMD_EXT_HEADER_LENGTH + getDataSize());
        bb.put(getHeaderAsBytes());

        bb.put(extHeader);

        if (getDataSize() != 0) {
            bb.put(payload);
        }
        return bb.array();
    }

}
