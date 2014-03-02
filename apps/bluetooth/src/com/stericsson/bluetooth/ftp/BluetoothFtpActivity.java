/*
 * Copyright (C) 2010, ST-Ericsson SA
 * Author: Arek Lichwa <arkadiusz.lichwa@tieto.com> for ST-Ericsson
 * Licence terms: 3-clause BSD
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
 * - Neither the name of the ST-Ericsson SA nor the names of its contributors
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

package com.stericsson.bluetooth.ftp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.Preference;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Button;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.text.InputFilter.LengthFilter;

import com.android.internal.app.AlertActivity;
import com.android.internal.app.AlertController;

import com.stericsson.bluetooth.R;

/** Provider dialog window to allow user authorize incoming FTP connection */
public class BluetoothFtpActivity extends AlertActivity implements DialogInterface.OnClickListener {
    private static final String TAG = "BluetoothFtpActivity";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private View mView = null;

    private TextView mMessageView = null;

    private CheckBox mAlwaysAllowed = null;

    private String mRemoteName = null;

    private boolean mAlwaysAllowedValue = false;

    private static final int DISMISS_TIMEOUT_DIALOG = 0;

    private static final int DISMISS_TIMEOUT_DIALOG_VALUE = 2000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        String action = intent.getAction();

        if (!action.equals(Constants.ACTION_AUTHORIZE_REQUEST)) {
            Log.e(TAG, "This activity may be only started with intent " + Constants.ACTION_AUTHORIZE_REQUEST);
            finish();
            return;
        }

        mRemoteName = intent.getStringExtra(Constants.EXTRA_REMOTE_NAME);

        registerReceiver(mReceiver, new IntentFilter(Constants.ACTION_AUTHORIZE_TIMEOUT));

        showFtpDialog();
    }

    private void showFtpDialog() {
        final AlertController.AlertParams p = mAlertParams;
        p.mIconId = android.R.drawable.ic_dialog_info;
        p.mTitle = getString(R.string.ftp_acceptance_dialog_header);
        p.mView = createView();
        p.mPositiveButtonText = getString(android.R.string.yes);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = getString(android.R.string.no);
        p.mNegativeButtonListener = this;
        setupAlert();
    }

    private View createView() {
        mView = getLayoutInflater().inflate(R.layout.access, null);

        String dialogMessage = getString(R.string.ftp_acceptance_dialog_title,
                        mRemoteName, mRemoteName);
        mMessageView = (TextView)mView.findViewById(R.id.message);
        mMessageView.setText(dialogMessage);

        mAlwaysAllowed = (CheckBox)mView.findViewById(R.id.alwaysallowed);
        mAlwaysAllowed.setChecked(false);
        mAlwaysAllowed.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    mAlwaysAllowedValue = true;
                } else {
                    mAlwaysAllowedValue = false;
                }
            }
        });
        return mView;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    public void onClick(DialogInterface dialog, int which) {
        if (V) Log.v(TAG, "onClick called (which=" + which + ")");

        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                onPositive();
                break;

            case DialogInterface.BUTTON_NEGATIVE:
                onNegative();
                break;
        }
    }

    private void onPositive() {
        sendIntentReply(Constants.ACTION_AUTHORIZE_ALLOWED, true);
        finish();
    }

    private void onNegative() {
        sendIntentReply(Constants.ACTION_AUTHORIZE_DISALLOWED, false);
        finish();
    }

    private void onTimeout() {
        mMessageView.setText(getString(R.string.ftp_acceptance_timeout_message, mRemoteName));
        mAlert.getButton(DialogInterface.BUTTON_NEGATIVE).setVisibility(View.GONE);
        mAlwaysAllowed.setVisibility(View.GONE);
        mAlwaysAllowed.clearFocus();

        mTimeoutHandler.sendMessageDelayed(mTimeoutHandler.obtainMessage(DISMISS_TIMEOUT_DIALOG),
                        DISMISS_TIMEOUT_DIALOG_VALUE);
    }

    private void sendIntentReply(String name, boolean withExtra) {
        Intent intent = new Intent(name);
        if (withExtra) {
            intent.putExtra(Constants.EXTRA_ALWAYS_ALLOWED, mAlwaysAllowedValue);
        }
        sendBroadcast(intent);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (V) Log.v(TAG, "Received intent: " + action);

            if (action.equals(Constants.ACTION_AUTHORIZE_TIMEOUT)) {
                onTimeout();
            }
        }
    };

    private final Handler mTimeoutHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (V) Log.v(TAG, "Received message: " + msg.what);

            switch (msg.what) {
                case DISMISS_TIMEOUT_DIALOG:
                    finish();
                    break;
                default:
                    break;
            }
        }
    };
}
