/*
 * Copyright (C) ST-Ericsson SA 2010
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

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.support.v4.app.Fragment;
import android.text.Editable;
import android.text.Html;
import android.text.TextWatcher;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.ToggleButton;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.CommandSender;
import com.stericsson.modemtracecontrol.internal.ConfigFileHandler;
import com.stericsson.modemtracecontrol.internal.MTCNotification;
import com.stericsson.modemtracecontrol.internal.ShutdownReceiver;
import com.stericsson.modemtracecontrol.internal.TraceFileStatusHandler;
import com.stericsson.modemtracecontrol.internal.Utility;
import com.stericsson.modemtracecontrol.test.CommandServerHelper;

/**
 * Main UI page - application start page
 */
public class TraceMainFragment extends Fragment {

    private String mTraceDirectoryPath;

    ToggleButton mButtonTraceStart;

    ToggleButton mButtonTrigger;

    EditText mEditTextTraceDirectory;

    EditText mEditTextLogoutput;

    EditText mEditTextTraceStatus;

    ScrollView mScroller;

    ProgressDialog mProgressDialog;

    CheckBox mTriggerOnPoweroffCheckBox;

    // Socket Client to connect to MLR
    CommandSender mCommandSender = null;

    // CommandServer for testing stand-alone
    CommandServerHelper mCommandServer = null;

    // Notification on trace ongoing
    MTCNotification mNotification;

    private static Context mContext;

    private static Boolean mIsUpdateNeeded = true;

    private HandlerTrigger mTraceStatusUpdater;

    public static TraceMainFragment newInstance(String title, Context context) {
        TraceMainFragment fragment = new TraceMainFragment();
        Bundle bundle = new Bundle();
        bundle.putString("TraceMainFragment", title);
        fragment.setArguments(bundle);
        mContext = context;
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroy() {
        if (Utility.DEBUG_PROGRAM) {
            mCommandServer.stopServer();
        }
        mIsUpdateNeeded = true;
        super.onDestroy();
    }


    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mButtonTrigger != null) {
            mButtonTrigger.setChecked(false);
        }
    }

    @Override
    public void onDestroyView() {
        mTraceStatusUpdater.done = true;
        if (Utility.DEBUG_PROGRAM) {
            mCommandServer.stopServer();
        }
        super.onDestroyView();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.trace_main, container, false);
        if (mContext == null) {
            Log.w(Utility.APP_NAME, "Context is null, trying to recover");
            mContext = inflater.getContext();
        }

        // Get all the GUI components
        mNotification = MTCNotification.getInstance();
        mButtonTraceStart = (ToggleButton) view.findViewById(R.id.buttonTrace);
        mButtonTrigger = (ToggleButton) view.findViewById(R.id.buttonTrigger);
        mTriggerOnPoweroffCheckBox = (CheckBox) view.findViewById(R.id.triggerOnPoweroffBox);
        mTriggerOnPoweroffCheckBox
                .setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

                    @Override
                    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                        ShutdownReceiver.setTriggerOnPoweroff(isChecked);
                    }
                });
        mEditTextTraceDirectory = (EditText) view.findViewById(R.id.editTextTraceDir);
        mEditTextTraceDirectory.addTextChangedListener(new TextWatcher() {

            @Override
            public void afterTextChanged(Editable s) {
                String traceDirectory = mEditTextTraceDirectory.getText().toString();
                TraceFileStatusHandler.setTraceFilePath(traceDirectory);
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

        });

        mEditTextLogoutput = (EditText) view.findViewById(R.id.editTextLogOutput);
        mEditTextLogoutput.setMovementMethod(new ScrollingMovementMethod());
        mScroller = (ScrollView) view.findViewById(R.id.scroller);

        mEditTextTraceStatus = (EditText) view.findViewById(R.id.traceStatusText);

        /*
         * Create thread for updating ongoing trace status.
         */
        mTraceStatusUpdater = new HandlerTrigger(handlerTraceStatus, 10);
        mTraceStatusUpdater.start();

        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll()
                    .build();
            StrictMode.setThreadPolicy(policy);
        }

        if (mIsUpdateNeeded) {
            updateMLRStatus();
            mIsUpdateNeeded = false;
        }

        // Set listener for Start Trace Button
        mButtonTraceStart.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // When trace start button is clicked
                if (mButtonTraceStart.isChecked()) {

                    // Start the trace
                    String traceDirectory = TraceFileStatusHandler.getTraceFilePath();
                    if (StartTrace(traceDirectory) == Utility.FAILURE) {
                        mButtonTraceStart.setChecked(false);
                    } else {
                        setTraceStatus();
                    }

                } else {

                    // Create a progress bar during stopping the trace
                    mProgressDialog = ProgressDialog.show(mContext, "", "Stop trace ongoing...");

                    // Perform the actual operation in a different thread,
                    // When task is done notify the progress dialog to close
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            int returnCode;
                            // Stop Trace
                            returnCode = StopTrace();
                            handlerTraceStop.sendEmptyMessage(returnCode);
                        }
                    }).start();
                }
            }
        });

        // Set listener for Trigger Button
        mButtonTrigger.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // When the trigger button is clicked
                if (mButtonTrigger.isPressed()) {

                    // Create a progress bar
                    mProgressDialog = ProgressDialog
                            .show(mContext, "",
                                    "Trigger report ongoing...");

                    // Perform the actual operation in a different thread,
                    // When task is done notify the progress dialog to close
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            int returnCode;
                            returnCode = TriggerReport();
                            handler.sendEmptyMessage(returnCode);
                        }
                    }).start();
                }
            }
        });

        // get the trace directory path and display it in the GUI
        mTraceDirectoryPath = new ConfigFileHandler().getTraceDirectoryPath();
        TraceFileStatusHandler.setTraceFilePath(mTraceDirectoryPath);
        mEditTextTraceDirectory.setText(mTraceDirectoryPath);

        return view;
    }

    // Handler that receives messages from the Trace Trigger thread
    // and updates the progress
    final Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            dismissDialog();
            if (msg.what == Utility.SUCCESS) {
                PrintLogOutput(Utility.MessageType.INFO, "Trigger report completed successfully.");
            } else {
                PrintLogOutput(Utility.MessageType.ERROR, "Trigger report failed.");
            }
            mButtonTrigger.setChecked(false);

        }
    };

    // Handler that receives messages from the stop trace thread
    // and updates the progress
    final Handler handlerTraceStop = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            dismissDialog();
            if (msg.what == Utility.SUCCESS) {
                PrintLogOutput(Utility.MessageType.INFO, "Trace stopped successfully.\n");
            } else {
                PrintLogOutput(Utility.MessageType.ERROR, "Failed to stop the ongoing trace.\n");
            }

        }
    };

    // Handler that receives messages from the trace status updater thread.
    final Handler handlerTraceStatus = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (mButtonTraceStart.isChecked()) {
                setTraceStatus();
            } else {
                mEditTextTraceStatus.setText("");
            }
        }
    };

    /**
     * This method is used for updating Main fragment when user sends a stop
     * trace command from Advanced Page.
     */
    public void updateDisplay() {
        int retunCode;
        if (TraceAdvancedFragment.getDoRefresh() || TraceAutoConfFragment.getDoRefresh()) {

            // Check if MLR is running on the Phone, else disable the
            // buttons
            retunCode = Initialize();
            if (retunCode == Utility.FAILURE) {
                mButtonTraceStart.setEnabled(false);
                mButtonTrigger.setEnabled(false);
                return;
            }

            // Send query command
            retunCode = mCommandSender.SendCommand(Utility.TRACE_SDCARD_LOGGING_COMMAND);
            if (retunCode == Utility.SUCCESS) {
                mNotification.CreateMTCNotification(mContext, "Select to stop modem tracing.", false);
                mButtonTraceStart.setChecked(true);
                ShutdownReceiver.setTraceOngoing(true);
                setTraceStatus();
            } else {
                String notificationMessage = new TraceFileStatusHandler()
                        .getLastModifiedDirectory(mTraceDirectoryPath);
                mNotification.CreateMTCNotification(mContext, notificationMessage, true);
                mButtonTraceStart.setChecked(false);
                ShutdownReceiver.setTraceOngoing(false);
            }

            // Disconnect from command socket
            Deinitialize();

            TraceAdvancedFragment.setDoRefresh(false);
            TraceAutoConfFragment.setDoRefresh(false);
        }
    }

    /**
     * Initializes connection to the server (MLR)
     *
     * @return SUCCESS on successful connect, FAILURE on error
     */
    private int Initialize() {
        int returnCode;

        if (mCommandSender == null) {
            mCommandSender = new CommandSender();
        }

        // TODO: It is used for testing on emulator
        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer == null) {
                mCommandServer = new CommandServerHelper();
            }
            mCommandServer.startServer();
        }

        // Connect to the MLR in the device
        Log.d(Utility.APP_NAME, "Initiate Client connection");
        returnCode = mCommandSender.Connect();
        if (returnCode == Utility.FAILURE) {
            Log.e(Utility.APP_NAME, "Failed to initialize client socket !");
            return returnCode;
        }
        Log.d(Utility.APP_NAME, "Client connected");

        return Utility.SUCCESS;
    }

    /**
     * Deinitializes connection to the server (MLR)
     *
     * @return SUCCESS on successful disconnect, FAILURE on error
     */
    private int Deinitialize() {
        if (mCommandSender != null) {
            Log.d(Utility.APP_NAME, "DeInitialize");
            if (mCommandSender.Disconnect() == Utility.FAILURE) {
                PrintLogOutput(Utility.MessageType.ERROR, "Failed to disconnect from command socket.");
                return Utility.FAILURE;

            }
        }
        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer != null) {
                mCommandServer.stopServer();
            }
        }
        Log.d(Utility.APP_NAME, "Disconnect Client");
        return Utility.SUCCESS;
    }

    /**
     * Starts by checking if MLR is running on the Phone, and disables buttons
     * if not. If MLR is running a check is made to see if SD card logging is
     * ongoing and sets the Trace Start button accordingly.
     */
    private void updateMLRStatus() {
        int rc;

        // Connect to command socket.
        rc = Initialize();
        if (rc == Utility.FAILURE) {
            // Disable buttons.
            mButtonTraceStart.setEnabled(false);
            mButtonTrigger.setEnabled(false);

            PrintLogOutput(
                    Utility.MessageType.ERROR,
                    "Could not connect to Modem Log Relay because it is not responding. Try to restart modem_log_relay process manually."
                            + " If you don't know how to do that, restart the phone and the process should be automatically restarted for you");
            Deinitialize();
            return;
        }

        // Check if SD card logging is ongoing.
        rc = mCommandSender.SendCommand(Utility.TRACE_SDCARD_LOGGING_COMMAND);
        if (rc == Utility.FAILURE) {
            Deinitialize();
            return;
        }

        // Set trace ongoing status.
        PrintLogOutput(Utility.MessageType.INFO, "SDCard tracing is ongoing...");
        mButtonTraceStart.setChecked(true);
        ShutdownReceiver.setTraceOngoing(true);

        // Create a notification.
        mNotification.CreateMTCNotification(mContext, "Select to stop modem tracing.", false);

        // Disconnect from command socket.
        Deinitialize();
    }

    /**
     * Starts modem logging on SDcard
     *
     * @param traceDirectory The path where trace will be stored
     * @return SUCESS on SDcard logging is active else FAILURE
     */
    private int StartTrace(String traceDirectory) {
        int returnCode;

        // Connect to command socket
        returnCode = Initialize();
        if (returnCode == Utility.FAILURE) {
            PrintLogOutput(Utility.MessageType.ERROR, "Failed to connect to command socket.");
            return returnCode;
        }

        // Trace destination command
        returnCode = mCommandSender.SendCommand(Utility.TRACE_DESTINATION_COMMAND + traceDirectory);
        if (returnCode == Utility.FAILURE) {
            PrintLogOutput(Utility.MessageType.ERROR, "Failed to start trace.");
            Deinitialize();
            return returnCode;
        }

        // Trace Start command
        returnCode = mCommandSender.SendCommand(Utility.TRACE_START_COMMAND);
        if (returnCode == Utility.FAILURE) {
            PrintLogOutput(Utility.MessageType.ERROR, "Failed to start trace.");
            Deinitialize();
            return returnCode;
        }

        // Check that trace is now actually ongoing.
        // if not, most likely the SD card is not mounted.
        returnCode = mCommandSender.SendCommand(Utility.TRACE_SDCARD_LOGGING_COMMAND);
        if (returnCode == Utility.FAILURE) {
            AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
            builder.setTitle(this.getText(R.string.error_sd_card_title));
            builder.setMessage(this.getText(R.string.error_sd_card_desc));
            builder.setNeutralButton("OK", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int id) {
                    dialog.cancel();
                    return;
                }
            });
            AlertDialog alert = builder.create();
            alert.show();

            // We need to stop the on going trace also
            // Trace stop command
            returnCode = mCommandSender.SendCommand(Utility.TRACE_STOP_COMMAND);
            if (returnCode == Utility.FAILURE) {
                Log.e(Utility.APP_NAME, "Unable to stop trace");
            }

            // Need to toggle the button too
            mButtonTraceStart.toggle();
            PrintLogOutput(Utility.MessageType.ERROR, getText(R.string.error_sd_card_desc).toString());

        } else {

            PrintLogOutput(Utility.MessageType.INFO, "Trace started successfully.");
            mNotification.CreateMTCNotification(mContext, "Select to stop modem tracing.", false);
            ShutdownReceiver.setTraceOngoing(true);
        }

        // Disconnect from command socket
        Deinitialize();

        return returnCode;
    }

    /**
     * Stops modem logging
     *
     * @return SUCESS on SDcard logging is active else FAILURE
     */
    private int StopTrace() {
        int retunCode;

        // Connect to command socket
        retunCode = Initialize();
        if (retunCode == Utility.FAILURE) {
            return retunCode;
        }

        // Trace stop command
        retunCode = mCommandSender.SendCommand(Utility.TRACE_STOP_COMMAND);
        if (retunCode == Utility.FAILURE) {
            Deinitialize();
            return retunCode;
        }

        String notificationMessage = new TraceFileStatusHandler().getLastModifiedDirectory(mTraceDirectoryPath);
        mNotification.CreateMTCNotification(mContext, notificationMessage, true);
        ShutdownReceiver.setTraceOngoing(false);

        // Disconnect from command socket
        retunCode = Deinitialize();
        if (retunCode == Utility.FAILURE) {
            return retunCode;
        }

        return Utility.SUCCESS;
    }

    /**
     * Triggers a modem trace report
     *
     * @return SUCCESS on SDcard logging is active else FAILURE
     */
    private int TriggerReport() {
        int returnCode;

        // Connect to command socket
        returnCode = Initialize();
        if (returnCode == Utility.FAILURE) {
            return returnCode;
        }

        // Trigger report command
        returnCode = mCommandSender.SendCommand(Utility.TRACE_TRIGGER_COMMAND);
        if (returnCode == Utility.FAILURE) {
            Deinitialize();
            return returnCode;
        }

        // Disconnect from command socket
        returnCode = Deinitialize();
        if (returnCode == Utility.FAILURE) {
            return returnCode;
        }

        return Utility.SUCCESS;
    }

    /**
     * Writes log to "Log Output" text view based on the messageType
     *
     * @param logType Log category
     * @param logText Text to be written
     */
    private void PrintLogOutput(Utility.MessageType logType, String logText) {
        switch (logType) {
            case INFO:
                Log.d(Utility.APP_NAME, logText);
                mEditTextLogoutput.append(Html.fromHtml("<font color='green'>" + logText
                        + "</font><br/>"));
                break;
            case ERROR:
                Log.e(Utility.APP_NAME, logText);
                mEditTextLogoutput.append(Html.fromHtml("<font color='red'>" + logText
                        + "</font><br/>"));
                break;
            default:
                Log.i(Utility.APP_NAME, logText);
                mEditTextLogoutput.append(Html.fromHtml("<font color='green'>" + logText
                        + "</font><br/>"));
                break;
        }

        // Update the scroll bar to show the last line
        mScroller.post(new Runnable() {

            @Override
            public void run() {
                mScroller.scrollTo(0, mEditTextLogoutput.getHeight());

            }
        });
    }

    private void dismissDialog() {
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            try {
                mProgressDialog.dismiss();
            } catch (Exception e) {
                Log.w(Utility.APP_NAME, "Exception when dismissing dialog");
            }
        }
    }

    private void setTraceStatus() {
        mEditTextTraceStatus.setText(
                new TraceFileStatusHandler().listFourLastChangedFiles(
                        TraceFileStatusHandler.getTraceFilePath()));
    }

    /**
     * Thread triggering a handler periodically.
     */
    private class HandlerTrigger extends Thread {

        public volatile boolean done = false;

        /*
         * Handler to be triggered.
         */
        Handler handler;

        /*
         * Update delay in seconds.
         */
        private int delay = 1;

        public HandlerTrigger(Handler handler, int delay) {
            this.handler = handler;
            if (delay > 1) {
                this.delay = delay;
            }
        }

        public void run() {
            while (true) {
                this.handler.sendEmptyMessage(0);
                try {
                    Thread.sleep(this.delay * 1000);
                } catch (InterruptedException e) {}

                if (this.done) {
                    return;
                }
            }
        }
    }
}
