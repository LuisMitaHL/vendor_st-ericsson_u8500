package com.stericsson.sdk.loader.communication;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * @author xdancho
 */
public interface ILoaderCommunicationListener {

    /** */
    String INSTANCE_POINTER = "instance";

    /**
     * Called when message from the loader communication interface arrives.
     * 
     * @param title
     *            Title of message
     * @param message
     *            Message test
     * @param logOnly
     *            True if message should only be logged
     */
    void loaderCommunicationMessage(String title, String message, boolean logOnly);

    /**
     * Called when loader communication reports progress.
     * 
     * @param totalBytes
     *            Total bytes to be transferred.
     * @param transferredBytes
     *            Bytes transferred.
     */
    void loaderCommunicationProgress(long totalBytes, long transferredBytes);

    /**
     * @param port
     *            TBD
     * @param message
     *            TBD
     */
    void loaderCommunicationError(IPort port, String message);
}
