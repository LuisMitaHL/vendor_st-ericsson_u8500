/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import android.content.Context;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;

/**
 * The CpasCommandHandler handles command AT+CPAS.
 * <p>
 * Possible alternatives are AT+CPAS and AT+CPAS=?
 */
public class CpasCommandHandler extends AtCommandHandler {

    /**
     * The telephony manager.
     */
    private TelephonyManager telephonyManager;

    /**
     * The current state of the phone.
     */
    private int phoneState;

    /**
     * The phone state listener receives phone state updates.
     */
    private PhoneStateListener stateListener = new PhoneStateListener() {

        /**
         * {@inheritDoc}
         */
        public void onCallStateChanged(int state, String incomingNumber) {
            phoneState = state;
        }
    };

    /**
     * Creates a PasCommandHandler, that handles the AT+CPAS command.
     *
     * @param context
     *            the context this handler was created in
     * @param atParser
     *            the AT parser this handler belong to
     */
    public CpasCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "+CPAS";
        argumentProperties = new AtArgumentProperties[] { new AtArgumentProperties(
                    false, null, null, false),
                                                        };
        telephonyManager = (TelephonyManager) context
                           .getSystemService(Context.TELEPHONY_SERVICE);
        phoneState = 0; // a phone will start in state idle
        telephonyManager.listen(stateListener,
                                PhoneStateListener.LISTEN_CALL_STATE);

    }

    /**
     * Returns phone activity status.
     * <p>
     * Supports ready (0), ringing (3) and call in progress (4).
     */
    public AtCommandResponse handleActionCommand() {
        if (phoneState == TelephonyManager.CALL_STATE_RINGING) {
            // ringing (3)
            return new AtCommandResponse(AtCommandResponse.OK, "+CPAS: 3");
        } else if (phoneState == TelephonyManager.CALL_STATE_OFFHOOK) {
            // call in progress (4)
            return new AtCommandResponse(AtCommandResponse.OK, "+CPAS: 4");
        }

        // the only remaining possibility is ready (0)
        return new AtCommandResponse(AtCommandResponse.OK, "+CPAS: 0");
    }

    /**
     * Returns a list of supported status codes.
     * <p>
     * Supports ready (0), ringing (3) and call in progress (4).
     */
    public AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK, "+CPAS: (0,3,4)");
    }

}
