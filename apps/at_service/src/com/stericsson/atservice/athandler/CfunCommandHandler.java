/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice.athandler;

import java.util.HashSet;

import android.content.Context;
import android.content.Intent;
import android.content.ActivityNotFoundException;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.RemoteException;
import android.provider.Settings;
import android.util.Log;

import android.os.ServiceManager;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.RILConstants;

import com.stericsson.atservice.AtArgumentProperties;
import com.stericsson.atservice.AtCommandResponse;
import com.stericsson.atservice.AtParser;
import com.stericsson.atservice.AtService;
import com.stericsson.atservice.CmeErrorCodeConstants;

/**
 * The CfunCommandHandler handles command AT+CFUN.
 * <p>
 * Possible alternatives are AT+CFUN=0, AT+CFUN=1, AT+CFUN=4, AT+CFUN=5,
 * AT+CFUN=6, AT+CFUN? and AT+CFUN=?
 */
public class CfunCommandHandler extends AtCommandHandler {

    private DummyHandler mHandler = new DummyHandler();

    private Phone phone;

    /**
     * Creates a CfunCommandHandler, that handles the AT+CFUN command.
     *
     * @param context the context this handler was created in
     * @param atParser the AT parser this handler belong to
     */
    public CfunCommandHandler(Context context, AtParser atParser) {
        super(context, atParser);
        commandName = "+CFUN";
        HashSet<Object> allowedValuesArg = new HashSet<Object>();
        allowedValuesArg.add(new Integer("0"));
        allowedValuesArg.add(new Integer("99"));
        allowedValuesArg.add(new Integer("1"));
        allowedValuesArg.add(new Integer("4"));
        allowedValuesArg.add(new Integer("5"));
        allowedValuesArg.add(new Integer("6"));
        allowedValuesArg.add(new Integer("7"));
        HashSet<Object> allowedValuesArg2 = new HashSet<Object>();
        allowedValuesArg2.add(new Integer("1"));
        allowedValuesArg2.add(new Integer("0"));
        argumentProperties = new AtArgumentProperties[] {
            new AtArgumentProperties(true, new Integer(0), allowedValuesArg, true),
            new AtArgumentProperties(true, new Integer(0), allowedValuesArg2, true),
        };
        try {
            phone = PhoneFactory.getDefaultPhone();
        } catch (RuntimeException rte) {
            Log.e(AtService.LOG_TAG, "CfunCommandHandler: " + rte.toString());
            phone = null;
        }

    }

    // TODO: WLAN and bluetooth on/off depending on airplane mode - document
    // this in some official document
    /**
     * Sets the level of functionality. This method handles phone off (0), radio
     * on (airplane mode off) (1) and radio off (airplane mode on) (4). GSM only
     * (airplane mode off) (5) and WCDMA only (airplane mode off) (6) are
     * handled in at_core.
     */
    public AtCommandResponse handleSetCommand() {
        if (!checkArgumentsValidSetDefault()) {
            Log.e(AtService.LOG_TAG, "Arguments to " + commandName + " is invalid");
            return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                         CmeErrorCodeConstants.INVALID_CHARACTERS_IN_TEXT_STRING);
        }

        Object[] argumentValues = atCommand.getArguments();

        int fun = ((Integer) argumentValues[0]).intValue();
        int rst = ((Integer) argumentValues[1]).intValue();
        AtCommandResponse response;

        switch (rst) {
        case 1:/*Set <fun> and reboot ME. Only AT+CFUN=99,1 is supported.*/
            switch (fun) {
            case 99:
            case 1: /* Supported for backwards compability - execute as 99 */
                response = rebootPhone();
                break;
            default:
                response = new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                                 CmeErrorCodeConstants.OPERATION_NOT_SUPPORTED);
            }
            break;
        case 0:/*Set <fun> but do not reboot ME. Same as default*/
        default:
            switch (fun) {
            case 99:
            case 1: /* Supported for backwards compability - execute as 99 */
                /* Leave airplane mode if radio is off. */
                turnAirplaneModeOff();
                response = new AtCommandResponse(AtCommandResponse.OK);
                break;
            case 0:
                response = shutDownPhone();
                break;
            case 7:
                response = setPhoneFunctionality(Phone.NT_MODE_WCDMA_PREF);
                break;
            case 4:
                response = turnAirplaneModeOn();
                break;
            case 5:
                response = setPhoneFunctionality(Phone.NT_MODE_GSM_ONLY);
                break;
            case 6:
                response = setPhoneFunctionality(Phone.NT_MODE_WCDMA_ONLY);
                break;
            default:
                response = new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                                 CmeErrorCodeConstants.OPERATION_NOT_SUPPORTED);
            }
        }

        return response;
    }

    /**
     * Set the phone functionality with certain network mode.
     *
     * @param networkMode the network mode to set
     * @return a command response
     */
    private AtCommandResponse setPhoneFunctionality(int networkMode) {
        turnAirplaneModeOff();
        if (setPreferredNetworkType(networkMode)) {
            return new AtCommandResponse(AtCommandResponse.OK);
        } else {
            return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                         CmeErrorCodeConstants.OPERATION_NOT_ALLOWED);
        }

    }

    /**
     * Reboot the phone.
     *
     * @return a command response
     */
    private AtCommandResponse rebootPhone() {
        /* Create a runnable that will perform the reboot */
        Runnable r = new Runnable() {
            @Override
            public void run() {
                PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
                pm.reboot(null);
            }
        };
        /* Run it on a separate thread */
        Thread rebootThread = new Thread(r);
        rebootThread.start();

        return new AtCommandResponse(AtCommandResponse.OK);
    }

    /**
     * Shut down the phone.
     *
     * @return a command response
     */
    private AtCommandResponse shutDownPhone() {
        Intent shutdown = new Intent(Intent.ACTION_REQUEST_SHUTDOWN);
        shutdown.putExtra(Intent.EXTRA_KEY_CONFIRM, false);
        shutdown.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        try {
            context.startActivity(shutdown);
        } catch (ActivityNotFoundException anfe) {
            Log.e(AtService.LOG_TAG, "Activity for shutdown not found!");
            return new AtCommandResponse(AtCommandResponse.ERROR);
        }
        return new AtCommandResponse(AtCommandResponse.OK);
    }

    /**
     * Turn on airplane mode. The radio is turned off and WLAN and bluetooth is
     * unavailable.
     *
     * @return a command response
     */
    private AtCommandResponse turnAirplaneModeOn() {
        if (Settings.System.getInt(context.getContentResolver(), Settings.System.AIRPLANE_MODE_ON,
                                   0) == 0) { // is currently off
            Log.d(AtService.LOG_TAG, "airplane mode off - setting mode on");
            Settings.System.putInt(context.getContentResolver(), Settings.System.AIRPLANE_MODE_ON,
                                   1);
            Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
            intent.putExtra("state", true);
            context.sendBroadcast(intent);
        }

        return new AtCommandResponse(AtCommandResponse.OK);
    }

    /**
     * Turn off airplane mode. The radio as well as WLAN and bluetooth is
     * activated again.
     */
    private void turnAirplaneModeOff() {
        if (Settings.System.getInt(context.getContentResolver(), Settings.System.AIRPLANE_MODE_ON,
                                   0) == 1) {
            Settings.System.putInt(context.getContentResolver(), Settings.System.AIRPLANE_MODE_ON,
                                   0);
            Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
            intent.putExtra("state", false);
            context.sendBroadcast(intent);
        }
    }

    /**
     * Sets the preferred network type.
     *
     * @param type the type to set
     */
    private boolean setPreferredNetworkType(int type) {
        boolean result = false;
        Settings.Secure.putInt(context.getContentResolver(),
                               Settings.Secure.PREFERRED_NETWORK_MODE, type);
        Log.d("DEBUGGING", "setPreferredNetworkType " + type);
        // Set the modem network mode
        try {
            if (phone != null) {
                phone.setPreferredNetworkType(type, mHandler
                                              .obtainMessage(DummyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
                result = true;
            }
        } catch (RuntimeException rte) {
            Log.e(AtService.LOG_TAG, "setPreferredNetworkType: " + rte.toString());
        }
        return result;
    }

    /**
     * Read the current setting.
     */
    protected AtCommandResponse handleReadCommand() {
        boolean inAirplaneMode = Settings.System.getInt(context.getContentResolver(),
                                 Settings.System.AIRPLANE_MODE_ON, 0) == 1;
        int funMode;

        if (inAirplaneMode) {
            funMode = 4;
        } else {
            int networkMode = Settings.Secure.getInt(context.getContentResolver(),
                              Settings.Secure.PREFERRED_NETWORK_MODE, RILConstants.PREFERRED_NETWORK_MODE);

            switch (networkMode) {
            case Phone.NT_MODE_WCDMA_PREF:
                funMode = 7;
                break;
            case Phone.NT_MODE_GSM_ONLY:
                funMode = 5;
                break;
            case Phone.NT_MODE_WCDMA_ONLY:
                funMode = 6;
                break;
            default:
                Log.e(AtService.LOG_TAG, "Network mode is define only for a CDMA phone");
                return new AtCommandResponse(AtCommandResponse.CME_ERROR,
                                             CmeErrorCodeConstants.INVALID_MOBILE_CLASS);
            }
        }

        return new AtCommandResponse(AtCommandResponse.OK, "+CFUN: " + funMode);
    }

    /**
     * Returns lists of supported c fun modes and reset modes.
     * <p>
     * Supports phone off (0), radio on (1), radio off (airplane mode) (4), GSM
     * only (5) and WCDMA only (6).
     * Supports do not reset phone (0), reset phone (1).
     */
    public AtCommandResponse handleTestCommand() {
        return new AtCommandResponse(AtCommandResponse.OK, "+CFUN: (0,1,4,5,6,7,99),(0,1)");
    }

    /**
     * Dummy handler, does nothing but log, is created to be able to create a
     * message by mHandler.obtainMessage(). The message will have a defined
     * handler and this is necessary to be allowed to call
     * phone.setPreferredNetworkType().
     */
    private class DummyHandler extends Handler {

        private static final int MESSAGE_GET_PREFERRED_NETWORK_TYPE = 0;

        private static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 1;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_GET_PREFERRED_NETWORK_TYPE:
                Log
                .d(AtService.LOG_TAG,
                   "In dummy handler, message is of type MESSAGE_GET_PREFERRED_NETWORK_TYPE");
                break;

            case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
                Log
                .d(AtService.LOG_TAG,
                   "In dummy handler, message is of type MESSAGE_SET_PREFERRED_NETWORK_TYPE");
                break;
            }
        }
    }

}
