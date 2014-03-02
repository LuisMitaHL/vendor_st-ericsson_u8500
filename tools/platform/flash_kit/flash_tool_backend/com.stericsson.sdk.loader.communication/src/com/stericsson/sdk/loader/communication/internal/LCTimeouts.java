package com.stericsson.sdk.loader.communication.internal;

import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Holds the value for timeouts used for Loader Communication
 * 
 * @author xdancho
 * 
 */
public final class LCTimeouts {

    /** */
    public static final long DEFAULT_LC_BULK_DATA_RECEIVED_TIMEOUT = 20000L;

    /** */
    public static final long DEFAULT_LC_BULK_SESSION_END_TIMEOUT = 10000L;

    /** */
    public static final long DEFAULT_LC_BULK_CMD_RECEIVED_TIMEOUT = 20000L;

    /** */
    public static final long DEFAULT_LC_COMMAND_ACK_TIMEOUT = 3000L;

    /** */
    public static final long DEFAULT_LC_COMMAND_GR_TIMEOUT = 20000L;

    /** */
    private static LCTimeouts instance = null;

    private long bulkDataReceived = DEFAULT_LC_BULK_DATA_RECEIVED_TIMEOUT;

    private long bulkSessionEnd = DEFAULT_LC_BULK_SESSION_END_TIMEOUT;

    private long bulkCommandReceived = DEFAULT_LC_BULK_CMD_RECEIVED_TIMEOUT;

    private long commandAck = DEFAULT_LC_COMMAND_ACK_TIMEOUT;

    private long commandGR = DEFAULT_LC_COMMAND_GR_TIMEOUT;

    private LCTimeouts() {
        // TODO Auto-generated constructor stub
    }

    /**
     * gets the only instance of this class
     * 
     * @return the LCTimeouts instance
     */
    public static LCTimeouts getInstance() {

        if (instance == null) {
            instance = new LCTimeouts();
        }

        return instance;

    }

    /**
     * Sets the timeout for LC
     * 
     * @param type
     *            the type of timeout
     * @param value
     *            the timeout value
     */
    public void setTimeouts(String type, long value) {
        if (type.equalsIgnoreCase(LCDefinitions.BULK_DATA_RECEIVED)) {
            bulkDataReceived = value;
        } else if (type.equalsIgnoreCase(LCDefinitions.BULK_SESSION_END)) {
            bulkSessionEnd = value;
        } else if (type.equalsIgnoreCase(LCDefinitions.BULK_CMD_RECEIVED)) {
            bulkCommandReceived = value;
        } else if (type.equalsIgnoreCase(LCDefinitions.COMMAND_ACK)) {
            commandAck = value;
        } else if (type.equalsIgnoreCase(LCDefinitions.COMMAND_GR)) {
            commandGR = value;
        }
    }

    /**
     * get the timeout for LC
     * 
     * @param type
     *            the type of timeout
     * @return the timeout value
     */
    public long getTimeouts(String type) {
        if (type.equalsIgnoreCase(LCDefinitions.BULK_DATA_RECEIVED)) {
            return bulkDataReceived;
        } else if (type.equalsIgnoreCase(LCDefinitions.BULK_SESSION_END)) {
            return bulkSessionEnd;
        } else if (type.equalsIgnoreCase(LCDefinitions.BULK_CMD_RECEIVED)) {
            return bulkCommandReceived;
        } else if (type.equalsIgnoreCase(LCDefinitions.COMMAND_ACK)) {
            return commandAck;
        } else if (type.equalsIgnoreCase(LCDefinitions.COMMAND_GR)) {
            return commandGR;
        } else {
            return 0;
        }
    }
}
