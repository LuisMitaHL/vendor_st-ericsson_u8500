/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import java.util.HashSet;

import android.content.Context;
import android.telephony.TelephonyManager;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;

/**
 * A handler to handle the test command AT*STETEST.
 */
public class AtTestCommandHandler extends AtCommandHandler {


    /**
     * Constructor for an <code>AtdCommandHandler</code>.
     *
     * @param context
     *            the context this handler was created in
     * @param atParser
     *            the AT parser this handler belong to
     */
    public AtTestCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "*STETEST";
        HashSet<Object> allowedValuesArg1 = new HashSet<Object>();
        allowedValuesArg1.add(new Integer(1));
        HashSet<Object> allowedValuesArg2 = new HashSet<Object>();
        allowedValuesArg2.add(new Integer(2));
        HashSet<Object> allowedValuesArg3 = new HashSet<Object>();
        allowedValuesArg3.add("\"Three\"");
        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(true, null, allowedValuesArg1, true),
            new AtArgumentProperties(true, new Integer(2), allowedValuesArg2,
                                     true),
            new AtArgumentProperties(false, null, allowedValuesArg3, true),
        };
    }

    protected AtCommandResponse handleReadCommand() {
        AtCommandResponse result = new AtCommandResponse(AtCommandResponse.OK,
                "*STETEST: 1");
        return result;
    }

    protected AtCommandResponse handleSetCommand() {
        if (!checkArgumentsValidSetDefault()) {
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }

        return new AtCommandResponse(AtCommandResponse.OK);
    }

    protected AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK);
    }
}
