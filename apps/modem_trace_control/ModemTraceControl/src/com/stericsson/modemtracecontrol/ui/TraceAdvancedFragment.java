/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.stericsson.modemtracecontrol.ui;

import java.util.regex.Pattern;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.CommandSender;
import com.stericsson.modemtracecontrol.internal.Utility;
import com.stericsson.modemtracecontrol.test.CommandServerHelper;

/**
 * Fragment for advanced trace features.
 */
public class TraceAdvancedFragment extends Fragment {

    private EditText cmdText;

    private Button sendBtn;

    private TextView statusView;

    // MLR command interface.
    private CommandSender commandSender;

    // MLR command server mock.
    private CommandServerHelper commandServer;

    private static Context mContext;

    ProgressDialog mProgressDialog;

    private static int mReturnCode;

    private static Boolean mDoRefresh = false;

    /**
     * Creates and returns a new instance of this class.
     */
    public static TraceAdvancedFragment newInstance(String title, Context context) {
        TraceAdvancedFragment fragment = new TraceAdvancedFragment();
        Bundle bundle = new Bundle();
        bundle.putString("title", title);
        fragment.setArguments(bundle);
        mContext = context;
        return fragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);

        View view = inflater.inflate(R.layout.advanced, container, false);
        if (mContext == null) {
            Log.w(Utility.APP_NAME, "Context is null, trying to recover");
            mContext = inflater.getContext();
        }

        cmdText = (EditText) view.findViewById(R.id.editTextSendCmd);
        cmdText.addTextChangedListener(new OnCmdTextChangedListener());
        sendBtn = (Button) view.findViewById(R.id.buttonSendCmd);
        sendBtn.setOnClickListener(new OnSendCmdClickListener());
        sendBtn.setEnabled(false);
        statusView = (TextView) view.findViewById(R.id.labelSendCmdStatus);
        statusView.setText("");

        if (Utility.DEBUG_PROGRAM) {
            commandServer = new CommandServerHelper();
        }

        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (commandSender != null) {
            commandSender.Disconnect();
        }
        if (Utility.DEBUG_PROGRAM) {
            commandServer.stopServer();
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (commandSender != null) {
            commandSender.Disconnect();
        }
        if (Utility.DEBUG_PROGRAM) {
            commandServer.stopServer();
        }
    }

    /**
     * Get the state to refresh main page
     *
     * @return true/false
     */
    public static Boolean getDoRefresh() {
        return mDoRefresh;
    }

    /**
     * Set the state to refresh main page
     *
     * @param doRefresh
     *            true for refresh main page, false not to refresh main page
     */
    public static void setDoRefresh(Boolean doRefresh) {
        mDoRefresh = doRefresh;
    }

    private class OnCmdTextChangedListener implements TextWatcher {

        @Override
        public void onTextChanged(CharSequence arg0, int arg1, int arg2,
                int arg3) {
            statusView.setText("");
            if (cmdText.getText().toString().equals("")) {
                sendBtn.setEnabled(false);
            } else {
                sendBtn.setEnabled(true);
            }
        }

        @Override
        public void afterTextChanged(Editable arg0) {
        }

        @Override
        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
                int arg3) {
        }
    }

    /**
     * Listener class for handling "Send" button clicks.
     */
    private class OnSendCmdClickListener implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            if (Utility.DEBUG_PROGRAM) {
                commandServer.startServer();
            }

            commandSender = new CommandSender();
            Log.i(Utility.APP_NAME, "Connecting to MLR...");
            if (commandSender.Connect() == Utility.SUCCESS) {

                // Create a progress bar during sending command
                mProgressDialog = ProgressDialog.show(mContext, "", "Waiting for MLR response...");

                // Perform the actual operation in a different thread,
                // When task is done notify the progress dialog to close
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        int returnCode;
                        String cmd = cmdText.getText().toString();

                        // Need to inform MainPage after stopping the trace
                        mDoRefresh = Pattern.matches(Utility.TRACE_STOP_COMMAND, cmd);

                        // Send command to MLR
                        returnCode = commandSender.SendCommand(cmd);
                        handlerSendCommand.sendEmptyMessage(returnCode);
                    }
                }).start();
            } else {
                Log.e(Utility.APP_NAME, "Failed to connect to MLR");
                setStatus("Failed to connect to MLR", R.color.STE_KO);
            }

            if (Utility.DEBUG_PROGRAM) {
                commandServer.stopServer();
            }
        }
    }

    // Handler that receives messages from send command thread
    // and updates the progress
    final Handler handlerSendCommand = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            mReturnCode = msg.what;
            if (mProgressDialog != null && mProgressDialog.isShowing()) {
                try {
                    mProgressDialog.dismiss();
                } catch (Exception e) {
                    Log.w(Utility.APP_NAME, "Advanced page : Exception when dismissing dialog");
                }
            }

            if (mReturnCode == Utility.SUCCESS) {
                String Response = commandSender.getCommandResponse();
                if (Response.equals("OK")) {
                    setStatus("OK", R.color.STE_OK);
                } else {
                    // Show an alert dialog if response is other than
                    // OK message
                    TextView alertTextView = new TextView(mContext);
                    alertTextView.setText(Response);
                    alertTextView.setTextSize(14);
                    final AlertDialog alertDialog =
                        new AlertDialog.Builder(mContext).setPositiveButton(android.R.string.ok, null)
                            .setTitle("MLR Response").setView(alertTextView).create();

                    alertDialog.show();
                }

            } else {
                setStatus("NOT OK", R.color.STE_KO);
            }
            commandSender.Disconnect();
            commandSender = null;
        }
    };

    private void setStatus(String s, int color) {
        statusView.setText(s);
        statusView.setTextColor(color);
    }
}
