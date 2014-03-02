/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.util.HashMap;
import java.util.Set;

import android.content.Context;
import android.util.Log;

import com.stericsson.atservice.athandler.AtCommandHandler;
import com.stericsson.atservice.athandler.AtdCommandHandler;
import com.stericsson.atservice.athandler.AtStarCommandHandler;
import com.stericsson.atservice.athandler.AtTestCommandHandler;
import com.stericsson.atservice.athandler.CclkCommandHandler;
import com.stericsson.atservice.athandler.CfunCommandHandler;
import com.stericsson.atservice.athandler.CkpdCommandHandler;
import com.stericsson.atservice.athandler.CtsaCommandHandler;
import com.stericsson.atservice.athandler.CpasCommandHandler;

/**
 * AtParser has an <code>AtCommandHandler</code> for each AT command it can
 * handle. AT commands are parsed by creating instances of
 * <code>AtCommand</code> class and let the appropriate handler take care of it.
 */
public class AtParser {

    /**
     * Contains all the registered AT command handlers. It is not synchronized
     * but as no handlers will be registered after an initialization phase it
     * should not be a problem.
     */
    private HashMap<String, AtCommandHandler> atHandlers;

    /**
     * The command handler used to handle the command being processed at present
     */
    private AtCommandHandler handlerInUse;

    /**
     * Create a new <code>AtParser</code>.
     * <p>
     * No handlers are registered.
     */
    public AtParser() {
        atHandlers = new HashMap<String, AtCommandHandler>();
    }

    /**
     * Register an AT command handler.
     * <p>
     * AT command handlers are later called via:
     * <ul>
     * <li><code>handleCommand()</code>
     * </ul>
     *
     * @param command the command name
     * @param handler the handler to register
     */
    public void register(String command, AtCommandHandler handler) {
        atHandlers.put(command, handler);
    }

    /**
     * Method used to set up handlers for supported commands. Add new handlers
     * and commands to this method when they have been implemented.
     * <p>
     * Supported commands: AT*STETEST, ATD, AT+PAS, AT*,
     * AT+CKPD, AT+CTSA, AT+CFUN, AT+CCLK
     */
    public void registerAllCommands(Context context) {
        atHandlers.put("*STETEST", new AtTestCommandHandler(context, this));
        atHandlers.put("D", new AtdCommandHandler(context, this));
        atHandlers.put("+CPAS", new CpasCommandHandler(context, this));
        atHandlers.put("*", new AtStarCommandHandler(context, this));
        atHandlers.put("+CKPD", new CkpdCommandHandler(context, this));
        atHandlers.put("+CTSA", new CtsaCommandHandler(context, this));
        atHandlers.put("+CFUN", new CfunCommandHandler(context, this));
        atHandlers.put("+CCLK", new CclkCommandHandler(context, this));
    }

    /**
     * Returns an array with all the commands this parser supports.
     *
     * @return an array of commands
     */
    public String[] allSupportedCommands() {
        Set<String> commands = atHandlers.keySet();
        return (String[]) commands.toArray(new String[0]);

    }

    /**
     * Returns the names of all supported commands as a string.
     *
     * @return a string
     */
    public String allSupportedCommandsAsString() {
        String[] commands = allSupportedCommands();
        StringBuffer commandNames = new StringBuffer();
        // Add names with format ATCommand1<CR><LF>ATCommand2....
        for (int i = 0; i < commands.length; i++) {
            if (i != 0) {
                commandNames.append(AtCommandResponse.CRLF);
            }
            commandNames.append(commands[i]);
        }
        return commandNames.toString();
    }

    // TODO: The name of the command is ignored, the presently handled command
    // is canceled if possible
    /**
     * Method used to cancel the processing of an AT command.
     *
     * @param command the command to cancel
     */
    public void cancel(String command) {
        if (handlerInUse != null) {
            // only cancel if a command is being processed
            // the case there a cancel arrives before the command it is supposed
            // to cancel is considered to be so unlikely that it is ignored
            handlerInUse.cancel();
        }
    }

    /**
     * Processes an incoming AT command.
     * <p>
     *
     * @param inputCommand
     * @param responseWriter the connection used to write intermediate responses
     *            to a command if such are requested
     * @return Result object for this command line.
     */
    public AtCommandResponse process(String inputCommand, ResponseWriter responseWriter) {
        String command = clean(inputCommand);
        // testing log
        Log.e(AtService.LOG_TAG, "All supported commands:" + allSupportedCommandsAsString());

        // Handle empty line - no response necessary
        if (command.equals("")) {
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        try {
            AtCommand cmd = new AtCommand(command);
            String commandName = cmd.getName();

            if (atHandlers.containsKey(commandName)) {
                AtCommandHandler handler = atHandlers.get(commandName);
                handlerInUse = handler;
                AtCommandResponse result = handler.handleCommand(cmd, responseWriter);
                handlerInUse = null;
                return result;

            } else {
                // No handler - error
                Log.e(AtService.LOG_TAG, "AtParser.process: command: " + commandName
                      + " could not be processed, no such handler");
                return new AtCommandResponse(AtCommandResponse.ERROR);
            }

        } catch (IllegalArgumentException e) {
            // could not create an AtCommand
            Log.e(AtService.LOG_TAG,
                  "AtParser.process: Faulty command syntax, could not create AtCommand object");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }
    }

    /**
     * Processes an incoming AT command.
     * <p>
     *
     * @param inputCommand
     * @return Result object for this command line.
     */
    public AtCommandResponse process(String inputCommand) {
        return process(inputCommand, null);
    }

    /**
     * Strip input of whitespace and convert to upper case - except sections
     * inside quotes. A faulty syntax makes the method return an empty string.
     */
    static private String clean(String input) {
        StringBuilder out = new StringBuilder(input.length());

        for (int i = 0; i < input.length(); i++) {
            char c = input.charAt(i);
            if (c == '"') { // opening "
                int j = input.indexOf('"', i + 1); // closing "
                if (j == -1) {
                    // has no closing " incorrect syntax return an empty string
                    return "";
                }
                // append the text in the quote unchanged
                out.append(input.substring(i, j + 1));
                i = j;
            } else if (c != ' ') { // only append non whitespace
                out.append(Character.toUpperCase(c));
            }
        }

        return out.toString();
    }

    public void close() {
        // will reuse parser only clear handlerInUse
        handlerInUse = null;
    }

}
