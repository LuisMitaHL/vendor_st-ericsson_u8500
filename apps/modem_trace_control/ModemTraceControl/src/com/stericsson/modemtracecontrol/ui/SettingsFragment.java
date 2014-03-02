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

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.SendCommandTask;
import com.stericsson.modemtracecontrol.internal.Utility;

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;


/**
 * Fragment for various trace settings.
 */
public class SettingsFragment extends Fragment {

    private static Context mContext;

    /*
     * UI views.
     */
    private Spinner mRouteSpinner;
    private Spinner mFileActionSpinner;
    private Button mSetTraceRouteButton;
    private Button mSetRolFileButton;
    private EditText mMediumBufSizeText;
    private EditText mLongBufSizeText;

    /*
     * Current selection for trace route.
     */
    private String mTraceRoute;

    /*
     * Current selection for circular trace files.
     */
    private String mRolFileAction;

    /**
     * Creates and returns a new instance of this class.
     */
    public static SettingsFragment newInstance(String title, Context context) {
        SettingsFragment fragment = new SettingsFragment();
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

        View view = inflater.inflate(R.layout.settings, container, false);
        if (mContext == null) {
            Log.w(Utility.APP_NAME, "Context is null, trying to recover");
            mContext = inflater.getContext();
        }

        /*
         * Setup the views we need to manage.
         */
        mRouteSpinner = (Spinner) view.findViewById(R.id.spinnerSettingsRoute);
        mFileActionSpinner = (Spinner) view.findViewById(R.id.spinnerSettingsFileAction);
        mSetTraceRouteButton = (Button) view.findViewById(R.id.buttonSettingsRoute);
        mSetRolFileButton = (Button) view.findViewById(R.id.buttonSettingsFiles);
        mMediumBufSizeText = (EditText) view.findViewById(R.id.textSettingsMediumBufSize);
        mLongBufSizeText = (EditText) view.findViewById(R.id.textSettingsLongBufSize);

        /*
         * Create a backing adapter for the trace route spinner from a list of
         * the available trace routes. The list is defined in string.xml.
         */
        ArrayAdapter<CharSequence> routeSpinnerAdapter =
                ArrayAdapter.createFromResource(mContext, R.array.trace_route,
                        android.R.layout.simple_spinner_item);
        routeSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mRouteSpinner.setAdapter(routeSpinnerAdapter);

        /*
         * Create a backing adapter for the file action spinner from a list of
         * the available actions. The list is defined in string.xml.
         */
        ArrayAdapter<CharSequence> actionSpinnerAdapter =
                ArrayAdapter.createFromResource(mContext, R.array.rol_file_action,
                        android.R.layout.simple_spinner_item);
        actionSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mFileActionSpinner.setAdapter(actionSpinnerAdapter);

        /*
         * Create a listener that is triggered when Android detects the user
         * has selected one of our spinner items.
         */
        OnItemSelectedListener spinnerListener = new OnSpinnerItemSelectedListener();
        mFileActionSpinner.setOnItemSelectedListener(spinnerListener);
        mRouteSpinner.setOnItemSelectedListener(spinnerListener);

        /*
         * Create a listener that is triggered when Android detects the user
         * has clicked one of our buttons.
         */
        OnClickListener buttonListener = new OnButtonClickListener();
        mSetTraceRouteButton.setOnClickListener(buttonListener);
        mSetRolFileButton.setOnClickListener(buttonListener);

        return view;
    }

    /**
     *  Callback listener that handles spinner selections.
     */
    private class OnSpinnerItemSelectedListener implements OnItemSelectedListener {

        /**
         * When the user selects an item in the spinner, this method is invoked
         * by the callback chain. Android calls the item selected listener for
         * the spinner, which invokes the onItemSelected method.
         */
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int pos,
                long row) {
            switch (view.getId()) {
            case R.id.spinnerSettingsRoute:
                mTraceRoute = parent.getItemAtPosition(pos).toString();
                break;

            case R.id.spinnerSettingsFileAction:
                mRolFileAction = parent.getItemAtPosition(pos).toString();
                break;
            }

        }

        @Override
        public void onNothingSelected(AdapterView<?> arg0) {
        }
    }

    /**
     * Callback listener that handles button clicks.
     */
    private class OnButtonClickListener implements OnClickListener {

        @Override
        public void onClick(View view) {
            switch (view.getId()) {
            case R.id.buttonSettingsRoute:
                new SendCommandTask(mContext).execute(
                        Utility.TRACE_ROUTE_PRE_COMMAND + mTraceRoute);
                break;
            case R.id.buttonSettingsFiles:
                sendRolFileCommand();
                break;
            }
        }

        private void sendRolFileCommand() {
            boolean s1 = mMediumBufSizeText.getText().toString().equals("");
            boolean s2 = mLongBufSizeText.getText().toString().equals("");
            if (s1 ^ s2) {
                alert("Buffer size", "Make sure that both values are set, or none");
            } else {
                String mediumBufSize = mMediumBufSizeText.getText().toString();
                String longBufSize = mLongBufSizeText.getText().toString();
                new SendCommandTask(mContext).execute(
                        Utility.TRACE_ROLFILE_PRE_COMMAND + mRolFileAction +
                        " " + mediumBufSize + " " + longBufSize);
            }
        }

        private void alert(String title, String msg) {
            TextView atext = new TextView(mContext);
            atext.setText(msg);
            atext.setTextSize(14);
            final AlertDialog adlg =
                new AlertDialog.Builder(
                        mContext).setPositiveButton(
                                android.R.string.ok, null).setTitle(
                                        title).setView(atext).create();
            adlg.show();
        }
    }
}