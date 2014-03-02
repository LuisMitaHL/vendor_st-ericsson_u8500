/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import android.util.Log;

/**
 * Class used to process AT commands. It waits for the monitor class to receive
 * commands and then process them.
 */
public class AtProcessor extends Thread {

    /**
     * The AtParser used to parse an AT command.
     */
    private AtParser atParser;

    /**
     * The monitor class receiving AT commands.
     */
    private CommandMonitor commandMonitor;

    /**
     * The connection used to write responses and results to the AT commands.
     */
    private ResponseWriter responseWriter;

    /**
     * The AtHandler used to parse commands, have this reference to be able to
     * close down
     */
    private AtHandler atHandler;

    /**
     * Constructor for an <code>AtProcessor</code>.
     *
     * @param atParser
     *            the AT parser used to parse AT commands
     * @param commandMonitor
     *            the monitor
     * @param responseWriter
     *            the connection for writing responses
     * @param atHandler
     *            the atHandler used to parse commands
     */
    public AtProcessor(AtParser atParser, CommandMonitor commandMonitor,
                       ResponseWriter responseWriter, AtHandler atHandler) {
        this.atParser = atParser;
        this.commandMonitor = commandMonitor;
        this.responseWriter = responseWriter;
        this.atHandler = atHandler;
    }

    /**
     * Wait for commands and get them from the monitor, process them and write
     * the result.
     */
    public void run() {
        boolean connectionOpen = true;
        while (connectionOpen) {
            // get the first command in the monitor
            String command = commandMonitor.getCommand();

            // check if connection is open
            connectionOpen = connectionOpen(command);

            if (connectionOpen) {
                // process the command
                AtCommandResponse result = atParser.process(command,
                                           responseWriter);
                // write result if possible
                connectionOpen = responseWriter.write(result);

            }
        }
        Log.d(AtService.LOG_TAG, "Closing AtProcessor");
        close();

    }

    /**
     * Checks if the connection is open by checking if command is close.
     *
     * @param command
     *            the command to check
     * @return true if the connection is open and false otherwise
     */
    private boolean connectionOpen(String command) {
        // connection is closed if command == close
        return !CommandMonitor.CLOSE_COMMAND.equals(command);
    }

    /**
     * Closes the AtProcessor.
     */
    private void close() {
        atHandler.close();
        atHandler = null;
        atParser.close();
        atParser = null;
        commandMonitor = null;
        responseWriter = null;
    }

}
