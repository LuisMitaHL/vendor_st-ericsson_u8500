package com.stericsson.sdk.equipment.io.port;

/**
 * @author xtomlju
 */
public interface IPortDataListener {

    /**
     * @param port
     *            TBD
     * @param buffer
     *            TBD
     * @param offset
     *            TBD
     * @param count
     *            TBD
     */
    void bytesRead(IPort port, byte[] buffer, int offset, int count);

    /**
     * @param port
     *            TBD
     * @param buffer
     *            TBD
     * @param offset
     *            TBD
     * @param count
     *            TBD
     */
    void bytesWritten(IPort port, byte[] buffer, int offset, int count);
}
