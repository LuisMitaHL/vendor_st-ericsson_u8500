/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.io.IOException;

import android.net.LocalSocket;
import android.util.Log;

/**
 * Writes responses to an AT command to an output stream.
 */
public class ResponseWriter {

    // TODO: Could add another type for messages from status updates that have
    // been requested by AT commands - NOT certain that these must be
    // implemented. Don't know how to set up such a subscription.

    /**
     * The socket used to send responses with.
     */
    private LocalSocket socket;

    /**
     * Constructor for a ResponseWriter.
     *
     * @param socket the output stream to write to
     */
    public ResponseWriter(LocalSocket socket) {
        this.socket = socket;

    }

    /**
     * Writes a reply over a socket.
     *
     * @param reply the CommandResponse
     * @return true if successful and false otherwise then the connection is
     *         down
     */
    public boolean write(AtCommandResponse reply) {
        try {
            Log.i(AtService.LOG_TAG, "ResponseWriter.write: Header: "
                  + reply.getHeader().toString());
            // writing header
            socket.getOutputStream().write(reply.getHeader().getBytes());

            // write the response text if any
            if (reply.hasResponseText()) {
                Log.i(AtService.LOG_TAG, "ResponseWriter.write: Response:"
                      + reply.getResponseText() + ". The size is "
                      + reply.getResponseText().length());
                socket.getOutputStream().write(reply.getResponseText().getBytes());
            }

            socket.getOutputStream().flush();
            Log.d(AtService.LOG_TAG, "The messages above sent");
            return true;
        } catch (IOException e) {
            // When suppose that this means that the socket is closed
            // down? Right now always
            Log.d(AtService.LOG_TAG, "ResponseWriter.write:"
                  + " Either an IO error or the connection is closed." + " Got message, "
                  + e.getMessage());
        }

        // could not write, the connection is down
        return false;

    }

}
