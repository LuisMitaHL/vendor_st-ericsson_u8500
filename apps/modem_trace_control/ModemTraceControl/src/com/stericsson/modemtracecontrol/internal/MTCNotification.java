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

package com.stericsson.modemtracecontrol.internal;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import com.stericsson.modemtracecontrol.R;
import com.stericsson.modemtracecontrol.ui.ModemTraceControlActivity;

/**
 * Display notification on notification bar
 */
public class MTCNotification {

    private static final int MTC_NOTIFICATION_ID = 0;

    private NotificationManager mNotificationManager;

    private Notification mNotification;

    private PendingIntent mPendingIntent;

    private static Boolean isNotificationVisible = false;

    private static MTCNotification mMTCNotification;

    /**
     * Constructor
     */
    protected MTCNotification() {
    }

    /**
     * Get the instance of the notification class
     */
    public static MTCNotification getInstance() {

        if (mMTCNotification == null) {
            mMTCNotification = new MTCNotification();
        }
        return mMTCNotification;
    }

    /**
     * Create MTC notification
     *
     * @param context Application context
     * @param notificationMessage Notification message
     * @param isTraceStopped Status of ongoing modem trace
     */
    public void CreateMTCNotification(Context context, String notificationMessage,
            Boolean isTraceStopped) {
        String notificationTitle;
        Intent intent;

        // First time you need to create notification
        if (!isNotificationVisible) {
            mNotificationManager = (NotificationManager) context
                    .getSystemService(Context.NOTIFICATION_SERVICE);
            mNotification = new Notification(R.drawable.ic_mtc_notification, "", 0);

            intent = new Intent(Intent.ACTION_MAIN);
            intent.setClass(context, ModemTraceControlActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            mPendingIntent = PendingIntent.getActivity(context, 0, intent, 0);
            isNotificationVisible = true;
        }

        if (isTraceStopped) {
            notificationTitle = "Modem trace stopped";
            if ((notificationMessage == null) || (notificationMessage.length() == 0)) {
                notificationMessage = "Select to start modem tracing.";
            }
            mNotification.flags &= ~Notification.FLAG_NO_CLEAR;
            mNotification.flags |= Notification.FLAG_AUTO_CANCEL;
        } else {
            notificationTitle = "Modem trace ongoing";
            mNotification.flags &= ~Notification.FLAG_AUTO_CANCEL;
            mNotification.flags |= Notification.FLAG_NO_CLEAR;
        }

        mNotification.tickerText = notificationTitle;
        mNotification.setLatestEventInfo(context, notificationTitle,
                notificationMessage, mPendingIntent);
        mNotificationManager.notify(MTC_NOTIFICATION_ID, mNotification);
    }
}
