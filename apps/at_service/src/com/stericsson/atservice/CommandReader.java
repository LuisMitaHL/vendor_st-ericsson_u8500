/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.util.Log;

/**
 * Class used to read AT commands.
 */
public class CommandReader extends Thread {

    /**
     * The buffer used to read with.
     */
    private BufferedReader reader = null;

    /**
     * The monitor to put the AT commands in.
     */
    private CommandMonitor commandMonitor;

    /**
     * Value indicating if the command reader is closed.
     */
    private boolean isClosed;

    /**
     * Constructor for a CommandReader.
     *
     * @param monitor
     *            the monitor to put commands in
     * @param input
     *            the input stream to read commands from
     */
    public CommandReader(CommandMonitor monitor, InputStream input) {
        commandMonitor = monitor;
        reader = new BufferedReader(new InputStreamReader(input), 100);
        isClosed = false;

    }

    /**
     * Read commands and adds them to a monitor class.
     */
    public void run() {
        while (!isClosed) {
            // reads a command
            String command = readCommand();
            // puts command in monitor
            commandMonitor.addCommand(command);
        }
    }

    /**
     * Reads a command. Waits until a command has been read. A command is ended
     * by a \n. Note: Using \r instead does not always work. Probably a bug in
     * under laying implementation.
     *
     * @return the command
     */
    protected String readCommand() {
        try {
            String command = reader.readLine();
            // If readLine returns null, the end of the stream is reached -can
            // more command be read later? Suppose it can't.
            Log.d(AtService.LOG_TAG,
                  "CommandReader.readCommand: Read command: " + command);

            if (command == null) { // Could add || equals("close") to allow
                // interpret null as as close down signal
                // close down the communication, set command to close
                Log
                .d(AtService.LOG_TAG,
                   "CommandReader.readCommand: Command null end of stream, closing communication");
                isClosed = true;
                command = CommandMonitor.CLOSE_COMMAND;
            }

            return command;
        } catch (IOException e) {
            // close down the communication, send command close
            isClosed = true;
            Log.d(AtService.LOG_TAG, "CommandReader.readCommand:"
                  + e.getMessage() + ". Closing communication");
            return CommandMonitor.CLOSE_COMMAND;

        }
    }

}
