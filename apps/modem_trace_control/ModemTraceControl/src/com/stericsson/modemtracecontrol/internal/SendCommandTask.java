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

package com.stericsson.modemtracecontrol.internal;

import com.stericsson.modemtracecontrol.test.CommandServerHelper;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.TextView;

public class SendCommandTask extends AsyncTask<String, Void, Integer> {

    private Context mContext;

    private ProgressDialog mProgress = null;

    /*
     * MLR command interface.
     */
    private CommandSender mCommandSender;

    /*
     * MLR command server mock.
     */
    private CommandServerHelper mCommandServer;

    /**
     * Constructor.
     * @param context The context we live in.
     */
    public SendCommandTask(Context context) {
        mContext = context;
        mCommandSender = new CommandSender();

        /*
         * Create mock server if we run in the emulator.
         */
        if (Utility.DEBUG_PROGRAM) {
            mCommandServer = new CommandServerHelper();
        }
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        mProgress = ProgressDialog.show(mContext, "",
                "Waiting for MLR response...");
    }

    @Override
    protected Integer doInBackground(String... commands) {
        int rc = Utility.FAILURE;

        /*
         * Start mock server if we run in the emulator.
         */
        if (Utility.DEBUG_PROGRAM) {
            mCommandServer.startServer();
        }

        Log.i(Utility.APP_NAME, "Connecting to MLR...");
        if (mCommandSender.Connect() == Utility.SUCCESS) {
            rc = mCommandSender.SendCommand(commands[0]);
        }

        mCommandSender.Disconnect();

        /*
         * Stop mock server if we run in the emulator.
         */
        if (Utility.DEBUG_PROGRAM) {
            mCommandServer.stopServer();
        }

        return rc;
    }

    @Override
    protected void onPostExecute(Integer rc) {
        super.onPostExecute(rc);
        if (mProgress != null && mProgress.isShowing()) {
            try {
                mProgress.dismiss();
            } catch (Exception e) {}
        }

        /*
         * Check result of the operation.
         */
        if (rc == Utility.SUCCESS) {
            String response = mCommandSender.getCommandResponse();
            if (response.equals(Utility.COMMAND_RESP_KO)) {
                TextView atext = new TextView(mContext);
                atext.setText(response);
                atext.setTextSize(14);
                final AlertDialog adlg =
                    new AlertDialog.Builder(
                            mContext).setPositiveButton(
                                    android.R.string.ok, null).setTitle(
                                            "MLR Response").setView(atext).create();
                adlg.show();
            }
        }
    }
}
