/*
 * Copyright (c) 2008-2009, Motorola, Inc.
 * Copyright (C) ST-Ericsson SA 2010
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of the Motorola, Inc. nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

package com.stericsson.bluetooth.map;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * Receives and handles: system broadcasts; Intents from other applications; Intents from
 * MAPService; Intents from modules in MAP application layer.
 */
public class BluetoothMapReceiver extends BroadcastReceiver {

    private static final String TAG = "BluetoothMapReceiver";
    private static final boolean V = Constants.VERBOSE;
    private static final boolean D = Constants.DEBUG;

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (V) Log.v(TAG, "Received intent: " + action);

        if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
            if (BluetoothAdapter.STATE_ON == intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                    BluetoothAdapter.ERROR)) {
                if (D) Log.d(TAG, "Received BLUETOOTH_STATE_CHANGED_ACTION, BLUETOOTH_STATE_ON");
                context.startService(new Intent(context, BluetoothMapService.class));
            } else if (BluetoothAdapter.STATE_OFF == intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                    BluetoothAdapter.ERROR)) {
                if (D) Log.d(TAG, "Received BLUETOOTH_STATE_CHANGED_ACTION, BLUETOOTH_STATE_OFF");
                context.stopService(new Intent(context, BluetoothMapService.class));
            }
        }
    }
}
