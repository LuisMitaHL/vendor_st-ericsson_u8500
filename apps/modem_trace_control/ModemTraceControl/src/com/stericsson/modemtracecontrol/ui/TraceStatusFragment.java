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

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ScrollView;
import android.widget.TextView;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.internal.TraceFileStatusHandler;

/**
 * UI page for displaying ongoing trace status.
 * The ongoing trace is displayed by listing the trace files and their current size ("ls -l").
 */
public class TraceStatusFragment extends Fragment {

    ScrollView mScroller;

    TextView mText;

    TextView mTracePathTextView;

    String traceDirectoryPath;

    public static TraceStatusFragment newInstance(String title) {

        TraceStatusFragment fragment = new TraceStatusFragment();
        Bundle bundle = new Bundle();
        bundle.putString("title", title);
        fragment.setArguments(bundle);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.tracestatus, container, false);
        traceDirectoryPath = TraceFileStatusHandler.getTraceFilePath();

        mText = (TextView) view.findViewById(R.id.statusTextBox);
        mText.setMovementMethod(new ScrollingMovementMethod());
        mScroller = (ScrollView) view.findViewById(R.id.traceStatusScroller);

        mText.setText(new TraceFileStatusHandler().listFourLastChangedFiles(traceDirectoryPath));
        mText.setEnabled(false);
        mTracePathTextView = (TextView) view.findViewById(R.id.pathLabel);
        mTracePathTextView.setText(traceDirectoryPath);

        return view;
    }

    public void updateDisplay() {
        mText.setText(new TraceFileStatusHandler().listFourLastChangedFiles(traceDirectoryPath));
        traceDirectoryPath = TraceFileStatusHandler.getTraceFilePath();
        mTracePathTextView.setText(traceDirectoryPath);
    }

}
