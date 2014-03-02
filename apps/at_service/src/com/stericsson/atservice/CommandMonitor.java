/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.concurrent.ConcurrentLinkedQueue;

import android.util.Log;

/**
 * Monitor class to store incoming AT commands.
 */
public class CommandMonitor {

    /**
     * The special command for cancel that is only used locally in this
     * implementation.
     */
    public static final String CANCEL_COMMAND = "cancel";

    /**
     * The command signaling that the ATProcessor should close down.
     */
    public static final String CLOSE_COMMAND = "close";

    /**
     * The queue of commands.
     */
    private ConcurrentLinkedQueue<String> commands;

    /**
     * The command being executed at present.
     */
    private String executingCommand;

    /**
     * The parser used to parse the command, this reference is needed to be able
     * to cancel the execution of a command.
     */
    private AtParser atParser;

    /**
     * Constructor for a <code>CommandMonitor</code>.
     *
     * @param atParser
     *            the atParser used to parse an AT command
     */
    public CommandMonitor(AtParser atParser) {
        this.atParser = atParser;
        commands = new ConcurrentLinkedQueue<String>();
    }

    /**
     * Adds an AT command and notifies that the queue contains a command.
     *
     * @param command
     *            the command to add
     */

    public synchronized void addCommand(String command) {
        // Check if new command is a cancel command
        if (CANCEL_COMMAND.equalsIgnoreCase(command)
                && executingCommand != null) {
            Log.d(AtService.LOG_TAG,
                  "CommandMonitor.addCommand: Got a cancel");
            atParser.cancel(command);
        } else {
            commands.add(command);
            Log.d(AtService.LOG_TAG,
                  "CommandMonitor.addCommand: Adding AT command:" + command
                  + " to the monitor");
            notifyAll();
        }

    }

    /**
     * Get the first command in the queue or wait until a command arrive.
     *
     * @return the command
     */

    public synchronized String getCommand() {
        // if another command should be collected the old command has finished
        // executing
        executingCommand = null;

        while (commands.isEmpty()) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        executingCommand = commands.remove();
        Log.d(AtService.LOG_TAG, "CommandMoitor.getCommand: Taking AT command:"
              + executingCommand + " from monitor to process it");
        return executingCommand;
    }

}
