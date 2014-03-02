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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.CommandSender;
import com.stericsson.modemtracecontrol.internal.ConfigFileHandler;
import com.stericsson.modemtracecontrol.internal.TraceFileStatusHandler;
import com.stericsson.modemtracecontrol.internal.Utility;
import com.stericsson.modemtracecontrol.test.CommandServerHelper;

/**
 * UI page for trace_auto.conf handling.
 */
public class TraceAutoConfFragment extends Fragment {

    private EditText mAutoConfTextBoxText;
    private Button mApplyButton;
    private Spinner mFilesSpinner;
    private EditText mPresetDirectoryText;

    private static Context mContext;

    private ConfigFileHandler mConfigFileHandler;
    private String mCurrentlySelectedFilePath;

    private ArrayAdapter<String> mDataAdapter = null;
    private List<String> mDataList;

    private CommandSender mCommandSender = null;
    private CommandServerHelper mCommandServer = null;

    private static Boolean mDoRefresh = false;

    public static TraceAutoConfFragment newInstance(String title, Context context) {

        TraceAutoConfFragment fragment = new TraceAutoConfFragment();
        Bundle bundle = new Bundle();
        bundle.putString("title", title);
        fragment.setArguments(bundle);
        mContext = context;
        return fragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.autoconfsettings, container, false);
        if (mContext == null) {
            Log.w(Utility.APP_NAME, "Context is null, trying to recover");
            mContext = inflater.getContext();
        }

        mAutoConfTextBoxText = (EditText) view.findViewById(R.id.traceAutoConfTextBox);
        mApplyButton = (Button) view.findViewById(R.id.traceAutoConfButton);
        mFilesSpinner = (Spinner) view.findViewById(R.id.spinner1);
        mPresetDirectoryText = (EditText)view.findViewById(R.id.presetFilePathText);

        mConfigFileHandler = new ConfigFileHandler();
        mDataList = mConfigFileHandler.getFileList();
        if (mDataList != null) {
            createFileSpinnerAdapter();
        }

        displayPresetFilesLocation();
        addFileSpinnerListeners();
        String filePath = mConfigFileHandler.getTraceAutoConfFilePathToUse();
        populateTraceAutoConfTextBox(filePath);
        mApplyButton.setOnClickListener(new OnApplyButtonClickListener());

        return view;
    }

    private void displayPresetFilesLocation(){
        mPresetDirectoryText.setText(mConfigFileHandler.getPresetFilesLocation());
    }

    private void populateSpinner() {
        mDataList = mConfigFileHandler.getFileList();
        if (mDataList != null) {
            if (mDataAdapter == null) {
                createFileSpinnerAdapter();
            }
            mDataAdapter.notifyDataSetChanged();
        }
    }

    private void createFileSpinnerAdapter() {
        mDataAdapter = new ArrayAdapter<String>(mContext,
                android.R.layout.simple_spinner_item, mDataList);
        mDataAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mFilesSpinner.setAdapter(mDataAdapter);
    }

    public void addFileSpinnerListeners() {
        mFilesSpinner.setOnItemSelectedListener(new CustomOnItemSelectedListener());
        mFilesSpinner.setOnTouchListener(new OnSpinnerTouchListener());
    }

    public void populateTraceAutoConfTextBox(String filePath) {
        mApplyButton.setEnabled(true);
        File file = new File(filePath);
        mAutoConfTextBoxText.setText(mConfigFileHandler.readLinesFromFile(filePath));
        String text = mConfigFileHandler.readLinesFromFile(filePath);
        mAutoConfTextBoxText.setText(text);
        if(text.equals(Utility.FILE_NOT_FOUND)){
            mApplyButton.setEnabled(false);
        }
        if (!file.canWrite()) {
            mAutoConfTextBoxText.setTextColor(Color.LTGRAY);
            mAutoConfTextBoxText.setEnabled(false);
        } else {
            mAutoConfTextBoxText.setTextColor(Color.BLACK);
            mAutoConfTextBoxText.setEnabled(true);
        }
    }

    /**
     * Listens for and handles button clicks.
     */
    private class OnApplyButtonClickListener implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            if(mCurrentlySelectedFilePath.equals("trace_auto.conf")){
                saveTraceAutoConfFile();
            } else {
                showAlertDialog(mContext.getText(R.string.restart_MLR_notification));
                if (sendAutoConfCommand(true) == false) {
                    showAlertDialog("Failed to change configuration");
                }
            }
        }

        private void saveTraceAutoConfFile(){
            try {
                // First create and save to a local file
                FileOutputStream fOut = mContext.openFileOutput("trace_auto.conf",
                        Context.MODE_WORLD_READABLE);
                OutputStreamWriter osw = new OutputStreamWriter(fOut);

                osw.write(mAutoConfTextBoxText.getText().toString());
                osw.flush();
                osw.close();

                File f = mContext.getFileStreamPath("trace_auto.conf");
                String filename = f.getAbsolutePath();

                mConfigFileHandler.saveTraceAutoConfText(filename);
                showAlertDialog(mContext.getText(R.string.restart_MLR_notification));
                if (sendAutoConfCommand(false) == false) {
                    showAlertDialog("Failed to change configuration");
                }

            } catch (IOException e) {
                mAutoConfTextBoxText.setText(e.getMessage());
                e.printStackTrace();
            }
        }

        /**
         * Initializes connection to the server (MLR).
         *
         * @return SUCCESS on successful connection, FAILURE on error
         */
        private int Initialize() {
            int returnCode;

            if (mCommandSender == null) {
                mCommandSender = new CommandSender();
            }

            // Used for testing on emulator
            if (Utility.DEBUG_PROGRAM) {
                if (mCommandServer == null) {
                    mCommandServer = new CommandServerHelper();
                }
                mCommandServer.startServer();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException ex) {
                    Log.d(Utility.APP_NAME, "Sleep()");
                }
            }

            // Connect to the MLR in the device
            returnCode = mCommandSender.Connect();
            if (returnCode == Utility.FAILURE) {
                Log.e(Utility.APP_NAME,
                        "Failed to initialize client socket during trigger on power off!");
                Deinitialize();
                return returnCode;
            }
            return Utility.SUCCESS;
        }

        /**
         * Deinitializes connection to the server (MLR).
         *
         * @return SUCCESS on successful disconnection, FAILURE on error
         */
        private int Deinitialize() {
            if (mCommandSender != null && mCommandSender.Disconnect() == Utility.FAILURE) {
                Log.d(Utility.APP_NAME,
                        "Failed to disconnect from command socket during trigger on power off.");
                return Utility.FAILURE;
            }
            if (Utility.DEBUG_PROGRAM) {
                if (mCommandServer != null) {
                    mCommandServer.setClientConnected(false);
                    mCommandServer.stopServer();
                }
            }
            return Utility.SUCCESS;
        }

        private boolean sendAutoConfCommand(boolean overwrite) {
            int returnCode;

            // Connect to command socket
            returnCode = Initialize();
            if (returnCode == Utility.FAILURE) {
                return false;
            }

            if (overwrite) {
                // Overwrite trace_auto.conf command
                returnCode = mCommandSender.SendCommand(Utility.TRACE_OVERWRITE_AUTOCONF_COMMAND + mCurrentlySelectedFilePath);
                if (returnCode == Utility.FAILURE) {
                    Deinitialize();
                    return false;
                }
            }

            // Re-parse trace_auto.conf
            returnCode = mCommandSender.SendCommand(Utility.TRACE_REPARSE_AUTOCONF_COMMAND);
            if (returnCode == Utility.FAILURE) {
                Deinitialize();
                return false;
            }

            // Disconnect from command socket
            Deinitialize();

            return true;
        }
    }

    private class CustomOnItemSelectedListener implements OnItemSelectedListener {

        public void onItemSelected(AdapterView<?> parent, View view, int pos,long id) {

            if(pos == 0) {
                String filePath = mConfigFileHandler.getTraceAutoConfFilePathToUse();
                populateTraceAutoConfTextBox(filePath);
                mCurrentlySelectedFilePath = "trace_auto.conf";
            } else {
                String selectedFileName = parent.getItemAtPosition(pos).toString();
                String selectedFileAbsPath = ((mConfigFileHandler.getPresetFilesLocation()).concat("/")).concat(selectedFileName);
                populateTraceAutoConfTextBox(selectedFileAbsPath);
                mCurrentlySelectedFilePath = selectedFileAbsPath;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> arg0) {
        }
    }

    /**
     * Callback listener that handles spinner touch.
     */
    private class OnSpinnerTouchListener implements OnTouchListener {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            String presetFilesDirectory = mPresetDirectoryText.getText().toString();
            mConfigFileHandler.setPresetFilesDirectory(presetFilesDirectory);
            populateSpinner();
            return false;
        }
    }

    /**
     * Alerts the user with the provided message.
     * @param message an alert message to the user.
     */
    public void showAlertDialog(CharSequence message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setMessage(message).setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        AlertDialog alert = builder.create();
        alert.show();
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
}
