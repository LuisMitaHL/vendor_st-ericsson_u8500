/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * The Broadcast receiver will start the AtService on boot,
 * and if needed whenever the user unlocks the device.
 *
 */
public class OnBootBroadcastReceiver extends BroadcastReceiver {

    /**
     * Receives the ACTION_BOOT_COMPLETED and ACTION_USER_PRESENT events.
     * Creates an intent and starts the AtService service.
     *
     * @param context
     *            The running context
     * @param intent
     *            The intent that was broadcasted
     */
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(AtService.LOG_TAG, "OnBootBroadcastReceiver: Entering");

        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {

            Log.i(AtService.LOG_TAG, "BOOT_COMPLETED received");
            context.startService(new Intent(context, AtService.class)
                                 .putExtra(AtService.BOOT_COMPLETED_STR, true));

        } else if (intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {

            Log.i(AtService.LOG_TAG, "USER_PRESENT received");
            context.startService(new Intent(context, AtService.class)
                                 .putExtra(AtService.USER_PRESENT_STR, true));

        }

        Log.i(AtService.LOG_TAG, "OnBootBroadcastReceiver: Leaving");
    }
}
