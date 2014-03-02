package com.stericsson.sdk.ftp.communication;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * Listens for communication over FTP. Reports communication progress and errors.
 * 
 * @author esrimpa
 * 
 */
public interface IFTPCommunicatorListener {

    /**
     * @param port
     *            FTP port
     * @param message
     *            error message
     */
    void error(IPort port, String message);

    /**
     * Called when ftp communication reports progress.
     * 
     * @param totalBytes
     *            Total bytes to be transferred.
     * @param transferredBytes
     *            Bytes transferred.
     */
    void taskProgress(long totalBytes, long transferredBytes);

}
