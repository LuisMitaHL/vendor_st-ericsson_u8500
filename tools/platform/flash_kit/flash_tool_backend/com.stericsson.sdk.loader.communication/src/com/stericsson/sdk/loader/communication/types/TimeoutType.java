package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 * structure to hold the complex data type received for timouts
 */
public class TimeoutType extends LoaderCommunicationType {

    int commandAck = 0;

    int bulkReadReq = 0;

    int bulkReadAck = 0;

    int bulkDR = 0;

    /**
     * @param result
     *            TBD
     * @param cmdAck
     *            TBD
     * @param bulkReq
     *            TBD
     * @param bulkAck
     *            TBD
     * @param bulkDr
     *            TBD
     */
    public TimeoutType(int result, int cmdAck, int bulkReq, int bulkAck, int bulkDr) {
        super(result);
        commandAck = cmdAck;
        bulkReadReq = bulkReq;
        bulkReadAck = bulkAck;
        bulkDR = bulkDr;
    }

    /**
     * @return TBD
     */
    public int getCommandAck() {
        return commandAck;
    }

    /**
     * @param ack
     *            TBD
     */
    public void setCommandAck(int ack) {
        commandAck = ack;
    }

    /**
     * @return TBD
     */
    public int getBulkReadReq() {
        return bulkReadReq;
    }

    /**
     * @param readReq
     *            TBD
     */
    public void setBulkReadReq(int readReq) {
        bulkReadReq = readReq;
    }

    /**
     * @return TBD
     */
    public int getBulkReadAck() {
        return bulkReadAck;
    }

    /**
     * @param readAck
     *            TBD
     */
    public void setBulkReadAck(int readAck) {
        bulkReadAck = readAck;
    }

    /**
     * @return TBD
     */
    public int getBulkDR() {
        return bulkDR;
    }

    /**
     * @param dr
     *            TBD
     */
    public void setBulkDR(int dr) {
        bulkDR = dr;
    }

}
