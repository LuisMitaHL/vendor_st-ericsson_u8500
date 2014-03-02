/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import android.content.Context;
import android.util.Log;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommand;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;
import com.stericsson.atservice.ResponseWriter;

/**
 * Each AtCommandHandler handles a certain AT command.
 */
public abstract class AtCommandHandler {

    /**
     * The <code>AtCommand</code> this handler should try to handle. Its syntax
     * is checked with <code>arguments</code>.
     */
    protected AtCommand atCommand;

    /**
     * The properties of the arguments for the AT command this handler is
     * responsible for. The properties are used to validate the argument values
     * in <code>atCommand</code>.
     */
    protected AtArgumentProperties[] argumentProperties = new AtArgumentProperties[0];

    /**
     * The name of the AT command this handler takes care of.
     */
    protected String commandName;

    /**
     * Value indicating that the command being processed should be canceled if
     * possible. This value should be checked by a method, handling a command
     * that can be canceled, before it does something new.
     */
    protected boolean cancel;

    /**
     * The connection used to write early responses to an AT command.
     */
    protected ResponseWriter responseWriter;

    /**
     * The android context.
     */
    protected Context context;

    /**
     * The AT parser this handler belong to.
     */
    protected AtParser atParser;

    /**
     * Constructor for an <code>AtCommandHandler</code>.
     *
     * @param context
     *            the context this handler was created in
     * @param atParser
     *            the AT parser this handler belong to
     */
    public AtCommandHandler(Context context, AtParser atParser) {
        this.context = context;
        this.atParser = atParser;
    }

    /**
     * Receives an <code>AtCommand</code> which it will try to handle. It is
     * handled in different ways depending on its type. Synchronized to make
     * sure that a certain command handler is only used by one thread at a time.
     *
     * @param atCommand
     *            the <code>AtCommand</code> to handle
     * @param responseWriter
     *            the connection to write responses to
     * @return an <code>AtCommandResult</code>
     */

    public synchronized AtCommandResponse handleCommand(AtCommand atCommand,
            ResponseWriter responseWriter) {
        // new command so set cancel to false
        cancel = false;
        this.atCommand = atCommand;
        this.responseWriter = responseWriter;
        AtCommandResponse result;
        int type = atCommand.getType();

        switch (type) {
        case AtCommand.TYPE_ACTION:
            Log.d(AtService.LOG_TAG, "AtCommandHandler: command type action");
            result = handleActionCommand();
            break;
        case AtCommand.TYPE_READ:
            Log.d(AtService.LOG_TAG, "AtCommandHandler: command type read");
            result = handleReadCommand();
            break;
        case AtCommand.TYPE_SET:
            Log.d(AtService.LOG_TAG, "AtCommandHandler: command type set");
            result = handleSetCommand();
            break;
        case AtCommand.TYPE_TEST:
            Log.d(AtService.LOG_TAG, "AtCommandHandler: command type test");
            result = handleTestCommand();
            break;
        default:
            Log.d(AtService.LOG_TAG, "AtCommandHandler: command type unknown");
            result = new AtCommandResponse(AtCommandResponse.ERROR);
        }

        return result;
    }

    // For testing purposes
    /**
     * Returns the command presently being handled.
     *
     * @return the command or null
     */

    public synchronized AtCommand getCommand() {
        return atCommand;
    }

    // TODO: Must not forget to "validate" the type of the commands - they
    // should not have all types - validate by not overriding the method

    /**
     * Handle Actions command.
     *
     * @return The result of this command.
     */
    protected AtCommandResponse handleActionCommand() {
        return new AtCommandResponse(AtCommandResponse.ERROR);
    }

    /**
     * Handle Read command.
     *
     * @return The result of this command.
     */
    protected AtCommandResponse handleReadCommand() {
        return new AtCommandResponse(AtCommandResponse.ERROR);
    }

    /**
     * Handle Set command.
     *
     * @return The result of this command.
     */
    protected AtCommandResponse handleSetCommand() {
        return new AtCommandResponse(AtCommandResponse.ERROR);
    }

    /**
     * Handle Test command.
     *
     * @return The result of this command.
     */
    protected AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.ERROR);
    }

    /**
     * Used to validate the arguments in <code>atCommand</code>. The
     * <code>arguments</code> attribute are used to validate them.
     * <p>
     * Two cases, AT commands with and without optional arguments.
     * <p>
     * Case 1: ATH[<n>], AT+CKPD=<keys>[,<time>[,<pause>]]
     * <p>
     * Case 2: AT+CR=<mode>, AT+CMGC=<length><CR><pdu><ctrl-Z/ESC>
     */
    protected boolean checkArgumentsValidSetDefault() {
        Object[] argumentValues = atCommand.getArguments();

        if (argumentValues.length > argumentProperties.length) {
            return false;
        }

        for (int i = 0; i < argumentValues.length; i++) {
            if (!argumentProperties[i].isValid(argumentValues[i])) {
                return false; // must not have any invalid arguments
            }
        }

        // set default values for AtCommand
        atCommand.setDefaultArguments(argumentProperties);
        return true;
    }

    /**
     * Sets the cancel value of this command handler. If the command presently
     * being processed can be canceled it will be.
     */
    public void cancel() {
        this.cancel = true;
    }

}
