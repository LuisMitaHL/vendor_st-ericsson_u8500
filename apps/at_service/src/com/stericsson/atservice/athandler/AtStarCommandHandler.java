/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import android.content.Context;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;

/**
 * The AtStarCommandHandler handles command AT*.
 * <p>
 * Possible alternatives are AT* and AT*=?
 */
public class AtStarCommandHandler extends AtCommandHandler {

    /**
     * Creates a AtStarCommandHandler, that handles the AT* command.
     *
     * @param context
     *            the context this handler was created in
     * @param atParser
     *            the AT parser this handler belong to
     */
    public AtStarCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "*";
        argumentProperties = new AtArgumentProperties[] { new AtArgumentProperties(
                    false, null, null, false),
                                                        };
    }

    /**
     * Return a list of all commands supported by AtService.
     */
    public AtCommandResponse handleActionCommand() {
        return new AtCommandResponse(AtCommandResponse.OK, atParser
                                     .allSupportedCommandsAsString());
    }

    /**
     * Returns OK indicating that the command is supported.
     */
    public AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK);
    }

}
