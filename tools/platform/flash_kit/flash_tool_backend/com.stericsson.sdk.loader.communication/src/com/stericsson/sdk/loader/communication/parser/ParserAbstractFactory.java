package com.stericsson.sdk.loader.communication.parser;

/**
 * class to get the parser according to what data to parse or
 * by protocol
 * @author xdancho
 *
 */
public final class ParserAbstractFactory {

    private ParserAbstractFactory() {
    }

    /**
     * get the appropriate parser depending on the the data to be parsed
     * 
     * @param rawData
     *            the data to be is to be parsed
     * @return LCCommandParser or null if a parser is not found
     */
    public static LCCommandParser getParser(byte[] rawData) {

        if (rawData[LCCommandParser.PROT_OFFSET] == LCCommandParser.PROTOCOL_BULK) {
            BulkProtocolParser parser = new BulkProtocolParser();
            parser.parse(rawData);
            return parser;
        } else if (rawData[LCCommandParser.PROT_OFFSET] == LCCommandParser.PROTOCOL_COMMAND) {
            CommandProtocolParser parser = new CommandProtocolParser();
            parser.parse(rawData);
            return parser;
        }

        return null;
    }

    /**
     * get the appropriate parser depending on the protocol
     * 
     * @param protocol
     *            the protocol
     * @return LCCommandParser or null if a parser is not found
     */
    public static LCCommandParser getParser(int protocol) {

        switch (protocol) {

        case LCCommandParser.PROTOCOL_BULK:
            return new BulkProtocolParser();
        case LCCommandParser.PROTOCOL_COMMAND:
            return new CommandProtocolParser();
        default:
            return null;
        }

    }

}
