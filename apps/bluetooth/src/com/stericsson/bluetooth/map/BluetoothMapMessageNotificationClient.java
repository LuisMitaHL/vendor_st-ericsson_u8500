/*
 * Copyright (C) 2010, ST-Ericsson SA
 * Author: Pradeep Kumar <pradeep-mmc.kumar@stericsson.com> for ST-Ericsson
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
package com.stericsson.bluetooth.map;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.database.Cursor;
import android.database.CursorJoiner;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.ParcelUuid;
import android.util.Log;
import android.os.Process;

public class BluetoothMapMessageNotificationClient {

    private final String TAG = "BluetoothMapMessageNotificationClient";

    /* Constants that indicate the current connection state */
    public static final int STATE_NONE = 0;       /* we're doing nothing */
    public static final int STATE_CONNECTING = 1; /* now initiating an outgoing connection */
    public static final int STATE_CONNECTED = 2;  /* now connected to a remote device */

    /* Unique UUID for MsgNofification Server */
    private static final UUID MNS_UUID =
        UUID.fromString("00001133-0000-1000-8000-00805F9B34FB");

    public static final ParcelUuid MnsParcelUUID =
        ParcelUuid.fromString("00001133-0000-1000-8000-00805F9B34FB");

    private static final boolean V = Constants.VERBOSE;

    private Context mContext;

    private BluetoothAdapter mAdapter;

    private int mState;

    private ConnectThread mConnectThread;

    private DisconnectThread mDisconnectThread;

    private BluetoothDevice mBtDevice;

    private BluetoothMapObexClientSession mSession;

    private BluetoothMapRfcommTransport mThisTransport;

    private BluetoothSocket mmConnectSocket;

    /* Session handler for SDP discovery */
    private HandlerThread mHandlerThread;

    private EventHandler mSessionHandler;

    public static final int MNS_RFCOMM_ERROR = 10;

    public static final int MNS_RFCOMM_CONNECTED = 11;

    public static final int MNS_SDP_RESULT = 12;
    public static final int MNS_SEND_EVENT = 13;
    public static final int MNS_DISCONNECT = 14;

    public static int flag_cursor_A;
    public static int flag_cursor_B;
    public static int flag_cursor_C;
    public static int flag_cursor_D;
    public static int flag_cursor_E;
    public static int flag_cursor_F;
    public static int flag_cursor_G;
    public static int flag_cursor_H;
    public static int flag_cursor_I;
    public static int flag_cursor_J;
    public static int flag_cursor_K;
    public static int flag_cursor_L;
    public static int flag_cursor_M;
    public static int flag_cursor_N;
    public static int flag_cursor_O;
    public static int flag_cursor_P;
    public static int flag_cursor_Q;
    public static int flag_cursor_R;
    public static int flag_cursor_S;
    public static int flag_cursor_T;
    public static int flag_cursor_U;
    public static int flag_cursor_V;
    public static int flag_cursor_W;
    public static int flag_cursor_X;

    private static boolean SmsNotificationState = false;
    public static boolean EmailNotificationState = false;
    public static boolean updatesRegistered = false;

    /* Enhancements for sendEvent Functions,
     * Incrementing the kinds of supported event
     * notification that can be sent from this MAP
     * server implementation */
    /* ContentResolvers and ContentObserver for SMS */
    private SmsContentObserverClass smsContentObserver = new SmsContentObserverClass();
    private InboxContentObserverClass inboxContentObserver = new InboxContentObserverClass();
    private SentContentObserverClass sentContentObserver = new SentContentObserverClass();
    private DraftContentObserverClass draftContentObserver = new DraftContentObserverClass();
    private OutboxContentObserverClass outboxContentObserver = new OutboxContentObserverClass();
    private FailedContentObserverClass failedContentObserver = new FailedContentObserverClass();
    private QueuedContentObserverClass queuedContentObserver = new QueuedContentObserverClass();

    private EmailContentObserverClass emailContentObserver = new EmailContentObserverClass();
    private EmailInboxContentObserverClass emailInboxContentObserver = new EmailInboxContentObserverClass();
    private EmailSentContentObserverClass emailSentContentObserver = new EmailSentContentObserverClass();
    private EmailDraftContentObserverClass emailDraftContentObserver = new EmailDraftContentObserverClass();
    private EmailOutboxContentObserverClass emailOutboxContentObserver = new EmailOutboxContentObserverClass();

    private Cursor crSmsA = null;
    private Cursor crSmsB = null;
    private Cursor crSmsInboxA = null;
    private Cursor crSmsInboxB = null;
    private Cursor crSmsSentA = null;
    private Cursor crSmsSentB = null;
    private Cursor crSmsDraftA = null;
    private Cursor crSmsDraftB = null;
    private Cursor crSmsOutboxA = null;
    private Cursor crSmsOutboxB = null;
    private Cursor crSmsFailedA = null;
    private Cursor crSmsFailedB = null;
    private Cursor crSmsQueuedA = null;
    private Cursor crSmsQueuedB = null;

    private Cursor crEmailA = null;
    private Cursor crEmailB = null;
    private Cursor crEmailOutboxA = null;
    private Cursor crEmailOutboxB = null;
    private Cursor crEmailDraftA = null;
    private Cursor crEmailDraftB = null;
    private Cursor crEmailInboxA = null;
    private Cursor crEmailInboxB = null;
    private Cursor crEmailSentA = null;
    private Cursor crEmailSentB = null;

    private static final int CR_SMS_A = 1;
    private static final int CR_SMS_B = 2;
    private static int currentCRSms = CR_SMS_A;
    private static final int CR_SMS_INBOX_A = 1;
    private static final int CR_SMS_INBOX_B = 2;
    private static int currentCRSmsInbox = CR_SMS_INBOX_A;
    private static final int CR_SMS_SENT_A = 1;
    private static final int CR_SMS_SENT_B = 2;
    private static int currentCRSmsSent = CR_SMS_SENT_A;
    private static final int CR_SMS_DRAFT_A = 1;
    private static final int CR_SMS_DRAFT_B = 2;
    private static int currentCRSmsDraft = CR_SMS_DRAFT_A;
    private static final int CR_SMS_OUTBOX_A = 1;
    private static final int CR_SMS_OUTBOX_B = 2;
    private static int currentCRSmsOutbox = CR_SMS_OUTBOX_A;
    private static final int CR_SMS_FAILED_A = 1;
    private static final int CR_SMS_FAILED_B = 2;
    private static int currentCRSmsFailed = CR_SMS_FAILED_A;
    private static final int CR_SMS_QUEUED_A = 1;
    private static final int CR_SMS_QUEUED_B = 2;
    private static int currentCRSmsQueued = CR_SMS_QUEUED_A;

    private static final int CR_EMAIL_A = 1;
    private static final int CR_EMAIL_B = 2;
    private static int currentCREmail = CR_EMAIL_A;
    private static final int CR_EMAIL_OUTBOX_A = 1;
    private static final int CR_EMAIL_OUTBOX_B = 2;
    private static int currentCREmailOutbox = CR_EMAIL_OUTBOX_A;
    private static final int CR_EMAIL_DRAFT_A = 1;
    private static final int CR_EMAIL_DRAFT_B = 2;
    private static int currentCREmailDraft = CR_EMAIL_DRAFT_A;
    private static final int CR_EMAIL_INBOX_A = 1;
    private static final int CR_EMAIL_INBOX_B = 2;
    private static int currentCREmailInbox = CR_EMAIL_INBOX_A;
    private static final int CR_EMAIL_SENT_A = 1;
    private static final int CR_EMAIL_SENT_B = 2;
    private static int currentCREmailSent = CR_EMAIL_SENT_A;

    private static final int MSG_CP_QUEUED_TYPE = 6;

    private static final int MSG_CP_FAILED_TYPE = 5;

    List <String> DeliveryStatusAwaitingMsgHandle = null;

    public BluetoothMapMessageNotificationClient(Context context) {
        Log.i(TAG, "BluetoothMapMessageNotificationClient constructor called");
        mContext = context;
        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mState = STATE_NONE;
        mBtDevice = null;
        mmConnectSocket = null;

       if (mHandlerThread == null) {
            if (V) Log.v(TAG, "Create handler thread for BluetoothMapMessageNotificationClient");
            mHandlerThread = new HandlerThread("BtMap Notification Handler",
                    Process.THREAD_PRIORITY_BACKGROUND);
            mHandlerThread.start();
            mSessionHandler = new EventHandler(mHandlerThread.getLooper());
        }

        DeliveryStatusAwaitingMsgHandle = new ArrayList<String>();
    }

    public static void turnOnMASInstanceNotification(int MasInstance) {
        if (MasInstance == Constants.SMS_MASINSTANCE) {
            turnOnSmsNotification();
        } else if (MasInstance == Constants.EMAIL_MASINSTANCE) {
            turnOnEmailNotification();
        }
    }

    public static void turnOffMASInstanceNotification(int MasInstance) {
        if (MasInstance == Constants.SMS_MASINSTANCE) {
            turnOffSmsNotification();
        } else if (MasInstance == Constants.EMAIL_MASINSTANCE) {
            turnOffEmailNotification();
        }
    }

    private static void turnOnSmsNotification() {
        SmsNotificationState = true;
    }

    private static void turnOffSmsNotification() {
        SmsNotificationState = false;
    }

    private static void turnOnEmailNotification() {
        EmailNotificationState = true;
    }

    private static void turnOffEmailNotification() {
        EmailNotificationState = false;
    }

    public static boolean areAllNotificationsTurnedOff() {
        if ((SmsNotificationState == false) && (EmailNotificationState == false))
            return true;
        else
            return false;
    }

    public static boolean isAnyNotificationsTurnedOn() {
        if ((SmsNotificationState == true) || (EmailNotificationState == true))
            return true;
        else
            return false;
    }

    public static boolean getMasInstanceNotificationState(int MasInstance) {
        if (MasInstance == Constants.EMAIL_MASINSTANCE) {
            return EmailNotificationState;
        } else if (MasInstance == Constants.SMS_MASINSTANCE) {
            return SmsNotificationState;
        } else {
            throw new IllegalArgumentException("Unknow MasInstance");
        }
    }

    /*
     * Receives events from mConnectThread & mSession back in the main thread.
     */
    private class EventHandler extends Handler {
        public EventHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MNS_SDP_RESULT:
                if (msg.arg1 > 0) {
                    /* Start the thread to connect with the given device */
                    mConnectThread = new ConnectThread(mBtDevice, msg.arg1);
                    mConnectThread.start();
                } else {
                    /* SDP query fail case */
                    Log.e(TAG, "SDP query failed!");
                }
                break;

            case MNS_SEND_EVENT:
                /* Send the event from here */
                if (isAnyNotificationsTurnedOn() == true)
                    sendDataThreadEmulation(msg.arg1, (String)msg.obj);
                break;

            case MNS_DISCONNECT:
                synchronisedDisconnect();
                break;
            }
        }
    }

    public void connectMnsChannel(String address) {
        BluetoothDevice device = mAdapter.getRemoteDevice(address);

        /* Set mBtDevice to current device */
        mBtDevice = device;

        /* Cancel any thread attempting to make a connection */
        if (mState == STATE_CONNECTING) {
            if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}
        }

        /* Do not launch SDP discovery explicitly do the rfcomm directly by
         * specifying the UUID to connect to */
        /* Start the thread to connect with the given device */
        Log.i(TAG, "Creating ConnectThread");
        mConnectThread = new ConnectThread(mBtDevice, 0);
        mConnectThread.start();

        setState(STATE_CONNECTING);
    }

    private synchronized void setState(int state) {
        Log.i(TAG, "setState() " + mState + " -> " + state);
        mState = state;
    }

    /**
     * This thread runs while attempting to make an outgoing connection
     * with a device. It runs straight through; the connection either
     * succeeds or fails.
     */
    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private String mSocketType;

        public ConnectThread(BluetoothDevice device, int channel) {
            BluetoothSocket tmp = null;

            /* Get a BluetoothSocket for a connection with the */
            /* given BluetoothDevice */
            try {
                tmp = device.createRfcommSocketToServiceRecord(MNS_UUID);

            } catch (IOException e) {
                Log.e(TAG, "Socket Type: " + mSocketType + "create() failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            Log.i(TAG, "BEGIN mConnectThread SocketType:");
            setName("ConnectThread" + mSocketType);

            /* Reset the Obex session */
            mSession = null;

            /* Make a connection to the BluetoothSocket */
            try {
                /* This is a blocking call and will only return on a */
                /* successful connection or an exception */
                mmSocket.connect();
                mmConnectSocket = mmSocket;
            } catch (IOException e) {
                /* Close the socket */
                try {
                    mmSocket.close();
                } catch (IOException e2) {
                    Log.i(TAG, "unable to close() " + mSocketType +
                            " socket during connection failure", e2);
                }
                connectionFailed();

                /* Since there's a failure set mSession and
                 * mmConnectSocket as null */
                mSession = null;
                mmConnectSocket = null;
                return;
            }

            /* start the obex channel */
            Log.i(TAG, "Starting Obex connection");
            mThisTransport = new BluetoothMapRfcommTransport(mmSocket);

            try {
                /* Hold the connection for a while to allow for Sending
                NotificationRegistration response to be sent by MAS Server, before
                which if OBEX_CONNECT is sent for Notification channel could cause deadlock
                at the MAS client, which wouldn't accept the OBEX_CONNECT on notification
                channel before OBEX response to the SetNotificationRegistration is sent
                and might reach an unpredictable state */
                Thread.sleep(50);
            } catch (InterruptedException e) {
                Log.e(TAG, "InterruptedException ConnectThread trying to sleep" + e);
            }

            /* Start the obex channel */
            startObexSession(mThisTransport);

            /* Register content observers, it should be registered after the
             * OBEX connection has been established. */
            RegisterMessageChangeObservers();
        }

        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "close() of connect " + mSocketType + " socket failed", e);
            }
        }
    }

    private void startObexSession(BluetoothMapRfcommTransport transport) {
        if (V) Log.v(TAG, "Create Client session with transport");
        mSession = new BluetoothMapObexClientSession(mContext, mThisTransport);
        Log.i(TAG, "BluetoothMapObexClientSession creation");

        if (V) Log.v(TAG, "Start Obex Connection");
        mSession.connect();
    }

    private void connectionFailed() {
        Log.i(TAG, "Connection FAILED");
    }

    public void diconnectMnsChannel() {
        if (V) Log.v(TAG, "Disconnect Message Notification Channel");
        mSessionHandler.obtainMessage(MNS_DISCONNECT, -1, -1,
                -1).sendToTarget();
    }

    /**
     * This thread runs while attempting to disconnect an obex session
     * it runs straight through either succeeding or failing to disconnect.
     */
    private class DisconnectThread extends Thread {
        public void run() {
            setName("DisconnectThread");

            synchronisedDisconnect();
        }
    }

    private void synchronisedDisconnect() {

        try {
            /* Sleep for a while before disconnect, to allow the return of previous OBEX packet to be sent */
            Thread.sleep(50);
        } catch (InterruptedException e) {
            Log.e(TAG, "InterruptedException synchronisedDisconnect trying to sleep" + e);
        }

        if (mSession != null) {
            /* De-register Message change observers first so that there
             * are no interferences from the changing content with SMS or
             * EMAIL ContentProviders. And the connection can be peacefuly disconnected */
            deregisterMessageChangeObservers();

            /* Disconnect obex session */
            mSession.disconnect();
            mSession = null;
        } else {
            Log.i(TAG, "DisconnectThread mSession(Obex Session) not existent");
        }

        /* Now disconnect the rfcomm channel as well */
        if (mmConnectSocket != null) {
            try {
                mmConnectSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Socket close threw IOException");
            }
        } else {
            Log.i(TAG, "DisconnectThread mmConnectSocket (Rfcomm Transport) not existent");
        }
        mmConnectSocket = null;
    }

    public void stop() {

        /* Deregister the updates for safety */
           deregisterMessageChangeObservers();

        /* Stop connect thread and dereference it */
        if (mConnectThread != null) {
            try {
                mConnectThread.interrupt();
                if (V) Log.v(TAG, "waiting for connect thread to terminate");
                mConnectThread.join();
            } catch (InterruptedException e) {
                if (V) Log.v(TAG, "Interrupted waiting for connect thread to join");
            }
            mConnectThread = null;
        }

        /* Stop mDisconnectThread and dereference it */
        if (mDisconnectThread != null) {
            try {
                mDisconnectThread.interrupt();
                if (V) Log.v(TAG, "waiting for mDisconnectThread to terminate");
                mDisconnectThread.join();
            } catch (InterruptedException e) {
                if (V) Log.v(TAG, "Interrupted waiting for mDisconnectThread to join");
            }
            mDisconnectThread = null;
        }

        /* Stop mHandlerThread and dereference it */
        if (mHandlerThread != null) {
            mHandlerThread.getLooper().quit();
            mHandlerThread.interrupt();
            mHandlerThread = null;
        }
    }

    public void sendEvent(int eventType, BluetoothMapEventData eventData, int MasInstance) {

        Log.i(TAG, "Entering sendEvent()");
        if (!BluetoothMapApplicationParameters.delNotification) {

            BluetoothMapApplicationParameters.delNotification = true;
            return;
        }
        String eventReportObj = null;

        Log.i(TAG, eventData.msg_type + ":Notification " + getNotificationType(eventType)+ " MAS:" + MasInstance);

        /* Add similar if conditions to block other notifications */
        if (eventType == Constants.MAP_EVENT_TYPE_MSG_SHIFT) {
            Log.i(TAG, "sendEvent : shall take no action for MAP_EVENT_TYPE_MSG_SHIFT/MAP_EVENT_TYPE_MSG_DELETED");
            return;
        }

        if (getMasInstanceNotificationState(MasInstance) == true) {
            /* Create Map Event report object from the provided data */
            eventReportObj = createEventReportObjectd(eventType,eventData);

            /* Send this event report to the message queue of mSessionHandler
             * this will ensure the proper enqueuing and de-queueing of the event report
             * and will prevent the over-crowding of OBEX transport for sending of
             * event report, the android Handler handles the continous stream of
             * MessageNotification in this way */
            if (eventReportObj != null) {
            mSessionHandler.obtainMessage(MNS_SEND_EVENT, MasInstance, -1,
                    eventReportObj).sendToTarget();
        }
        } else {
            Log.i(TAG, "Registration disabled for this MAS Instance");
        }
    }

    private String getNotificationType(int eventType) {
        String ret = null;
        switch (eventType) {
        case Constants.MAP_EVENT_TYPE_NEW_MSG:
            ret = "MAP_EVENT_TYPE_NEW_MSG";
        case Constants.MAP_EVENT_TYPE_MSG_DELETED:
            ret = "MAP_EVENT_TYPE_MSG_DELETED";
            break;
        case Constants.MAP_EVENT_TYPE_SENT_SUCCESS:
            ret = "MAP_EVENT_TYPE_SENT_SUCCESS";
            break;
        case Constants.MAP_EVENT_TYPE_SENT_FAILED:
            ret = "MAP_EVENT_TYPE_SENT_FAILED";
            break;
        case Constants.MAP_EVENT_TYPE_DELIVERY_SUCCESS:
            ret = "MAP_EVENT_TYPE_DELIVERY_SUCCESS";
            break;
        case Constants.MAP_EVENT_TYPE_DELIVERY_FAILED:
            ret = "MAP_EVENT_TYPE_DELIVERY_FAILED";
            break;
        case Constants.MAP_EVENT_TYPE_MEMORY_FULL:
            ret = "MAP_EVENT_TYPE_MEMORY_FULL";
            break;
        case Constants.MAP_EVENT_TYPE_MEMORY_AVAIL:
            ret = "MAP_EVENT_TYPE_MEMORY_AVAIL";
            break;
        case Constants.MAP_EVENT_TYPE_MSG_SHIFT:
            ret = "MAP_EVENT_TYPE_MSG_SHIFT";
            break;

        default:
            ret = "Unknown Type";
        }
        return ret;
    }

    private String createEventReportObjectd(int eventType, BluetoothMapEventData eventData) {
        String outString = "";
        boolean memory_related = false;

        outString += "<?xml version=\"1.0\"?>\n";

        outString += "<MAP-event-report version=\"1.0\">";
        outString += "<event ";

        if ((eventType == Constants.MAP_EVENT_TYPE_MEMORY_AVAIL) ||
                (eventType == Constants.MAP_EVENT_TYPE_MEMORY_FULL)) {
            memory_related = true;
        }

        switch (eventType) {
        case Constants.MAP_EVENT_TYPE_NEW_MSG:
            outString += "type=";
            outString += "\"NewMessage\"";
            break;
        case Constants.MAP_EVENT_TYPE_MSG_DELETED:
            outString += "type=";
            outString += "\"MessageDeleted\"";
            break;
        case Constants.MAP_EVENT_TYPE_SENT_SUCCESS:
            outString += "type=";
            outString += "\"SendingSuccess\"";
            break;
        case Constants.MAP_EVENT_TYPE_SENT_FAILED:
            outString += "type=";
            outString += "\"SendingFailure\"";
            break;
        case Constants.MAP_EVENT_TYPE_DELIVERY_FAILED:
            outString += "type=";
            outString += "\"DeliveryFailure\"";
            break;
        case Constants.MAP_EVENT_TYPE_DELIVERY_SUCCESS:
            outString += "type=";
            outString += "\"DeliverySuccess\"";
            break;
        case Constants.MAP_EVENT_TYPE_MEMORY_AVAIL:
            outString += "type=";
            outString += "\"DeliverySuccess\"";
            memory_related = true;
            break;
        case Constants.MAP_EVENT_TYPE_MEMORY_FULL:
            outString += "type=";
            outString += "\"MemoryFull\"";
            memory_related = true;
            break;
        case Constants.MAP_EVENT_TYPE_MSG_SHIFT:
            outString += "type=";
            outString += "\"MessageShift\"";
            break;

        default :
            outString += "type=";
            outString += "\"Unknown\"";
        }

        /* include these fields as well for non-memory related events */
        if (!memory_related) {

            outString += " handle = ";
            outString += "\"" + eventData.handle + "\" ";

            outString += " folder = ";
            outString += "\"" + eventData.folder.toUpperCase()+ "\" ";

            if (eventType == Constants.MAP_EVENT_TYPE_MSG_SHIFT) {
                outString += " old_folder = ";
                outString += "\"" + eventData.old_folder + "\" ";
            }
            outString += " msg_type = ";
            outString += "\"" + eventData.msg_type + "\" ";

        }

        outString += "/>";
        outString += "</MAP-event-report>\n";
        return outString;
    }

    private void sendDataThreadEmulation(int instanceId, String object) {
        String EventReportObject;
        int MasInstance;

        Log.i(TAG, "sendDataThreadEmulation entering:");

        EventReportObject = object;
        MasInstance = instanceId;
        if (mSession != null) {

            /* For Email notifications, send the notification to allow
             * the Email application to stabilize and retrieve the data
             * correctly on subsequent GetMessage requests from MCE */
            if (MasInstance == Constants.EMAIL_MASINSTANCE) {

                try {
                    Thread.sleep(6000);
                } catch (InterruptedException e) {
                    Log.e(TAG, "InterruptedException " + e);
                }
            }
            try {
                if (mSession.sendData(MasInstance, EventReportObject) == true) {
                    /* Event Report Object Send successfully */
                    Log.i(TAG, "sendDataThreadEmulation : Data sent successfully on MAP Notification Channel");
                } else {
                    /* Event Report Object Send failed */
                Log.i(TAG, "sendDataThreadEmulation : Data Sending failed on MAP Notification Channel");
                }
            } catch (Exception e) {
                Log.e(TAG,"Exception " + e.toString());
            }
        } else {
            Log.i(TAG, "sendDataThreadEmulation : No Obex Session (Notification Channel) to send data");
        }
    }

    public class BluetoothMapEventData {
        String handle;
        String folder;
        String old_folder;
        String msg_type;
    };

    private class SmsContentObserverClass extends ContentObserver {

        public SmsContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            /* Synchronize this? */
            if (currentCRSms == CR_SMS_A) {
                currentItemCount = crSmsA.getCount();
                //crSmsB.requery();
                flag_cursor_A = 1;
                newItemCount = replaceRequeryA();
            } else {
                currentItemCount = crSmsB.getCount();
                //crSmsA.requery();
                flag_cursor_B = 2;
                newItemCount = replaceRequeryA();
            }

            Log.d(TAG, "SMS current " + currentItemCount + " new "
                    + newItemCount);

            if (newItemCount > currentItemCount) {
                crSmsA.moveToFirst();
                crSmsB.moveToFirst();
                CursorJoiner joiner = new CursorJoiner(crSmsA,
                        new String[] { "_id" }, crSmsB, new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSms == CR_SMS_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                            Log.d(TAG, " SMS ADDED TO INBOX ");
                            String body1 = crSmsA.getString(crSmsA
                                    .getColumnIndex("body"));
                            String id1 = crSmsA.getString(crSmsA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " ADDED SMS ID " + id1 + " BODY "
                                    + body1);
                            String folder = getMAPFolder(crSmsA.getInt(crSmsA
                                    .getColumnIndex("type")));
                            if ( (folder != null ) && (folder.compareToIgnoreCase("inbox") == 0)) {
                                BluetoothMapEventData eventData = createEventData(id1,
                                        "TELECOM/MSG/" + folder, "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_NEW_MSG, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                Log.d(TAG, " ADDED TO UNKNOWN FOLDER");
                            }
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSms == CR_SMS_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                            Log.d(TAG, " SMS ADDED ");
                            String body1 = crSmsB.getString(crSmsB
                                    .getColumnIndex("body"));
                            String id1 = crSmsB.getString(crSmsB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " ADDED SMS ID " + id1 + " BODY "
                                    + body1);
                            String folder = getMAPFolder(crSmsB.getInt(crSmsB
                                    .getColumnIndex("type")));
                            if ( (folder != null ) && (folder.compareToIgnoreCase("inbox") == 0)) {
                                BluetoothMapEventData eventData = createEventData(id1,
                                        "TELECOM/MSG/" + folder, "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_NEW_MSG, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                Log.d(TAG, " ADDED TO UNKNOWN FOLDER");
                            }
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSms == CR_SMS_A) {
                currentCRSms = CR_SMS_B;
            } else {
                currentCRSms = CR_SMS_A;
            }
        }
    }

    private int replaceRequeryA() {
            Cursor c1 = null;
            int num = 0;
            try {
                c1 = mContext.getContentResolver().query(Constants.SMSURI, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_A == 1) {
                try {
                    crSmsB.close();
                    crSmsB =  mContext.getContentResolver().query(Constants.SMSURI, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_A = 0;
            }
            if (flag_cursor_B == 2 ) {
                try {
                    crSmsA.close();
                    crSmsA =  mContext.getContentResolver().query(Constants.SMSURI, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_B = 0;
            }
            return num;
    }
    /**
     * This class listens for changes in Sms Content Provider's inbox table
     * It acts, only when a entry gets removed from the table
     */
    private class InboxContentObserverClass extends ContentObserver {

        public InboxContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            if (currentCRSmsInbox == CR_SMS_INBOX_A) {
                currentItemCount = crSmsInboxA.getCount();
                //crSmsInboxB.requery();
                flag_cursor_C = 1;
                newItemCount = replaceRequeryInboxSms();
            } else {
                currentItemCount = crSmsInboxB.getCount();
                //crSmsInboxA.requery();
                flag_cursor_D = 2;
                newItemCount = replaceRequeryInboxSms();
            }

            Log.d(TAG, "SMS INBOX current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsInboxA.moveToFirst();
                crSmsInboxB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsInboxA,
                        new String[] { "_id" }, crSmsInboxB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsInbox == CR_SMS_INBOX_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM INBOX ");
                            String body = crSmsInboxA.getString(crSmsInboxA
                                    .getColumnIndex("body"));
                            String id = crSmsInboxA.getString(crSmsInboxA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/INBOX", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);

                            } else {
                                Log.d(TAG, "Shouldn't reach here as you cannot " +
                                "move msg from INBOX to any other folder");
                            }
                        } else {
                            /* implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsInbox == CR_SMS_INBOX_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM INBOX ");
                            String body = crSmsInboxB.getString(crSmsInboxB
                                    .getColumnIndex("body"));
                            String id = crSmsInboxB.getString(crSmsInboxB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/INBOX", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                Log.d(TAG,"Shouldn't reach here as you cannot " +
                                "move msg from INBOX to any other folder");
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSmsInbox == CR_SMS_INBOX_A) {
                currentCRSmsInbox = CR_SMS_INBOX_B;
            } else {
                currentCRSmsInbox = CR_SMS_INBOX_A;
            }
        }
    }

    private int replaceRequeryInboxSms() {
            Cursor c1 = null;
            int num = 0;
            Uri smsInboxUri = Uri.parse(Constants.INBOX_URI);
            try {
                c1 = mContext.getContentResolver().query(smsInboxUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_C == 1) {
                try {
                    crSmsInboxB.close();
                    crSmsInboxB =  mContext.getContentResolver().query(smsInboxUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_C = 0;
            }
            if (flag_cursor_D == 2 ) {
                try {
                    crSmsInboxA.close();
                    crSmsInboxA =  mContext.getContentResolver().query(smsInboxUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_D = 0;
            }
            return num;
    }
    /**
     * This class listens for changes in Sms Content Provider's SENT table
     * It acts, only when a entry gets removed from the table
     */
    private class SentContentObserverClass extends ContentObserver {

        public SentContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            if (currentCRSmsSent == CR_SMS_SENT_A) {
                currentItemCount = crSmsSentA.getCount();
                //crSmsSentB.requery();
                flag_cursor_E = 1;
                newItemCount = replaceRequerySent();
            } else {
                currentItemCount = crSmsSentB.getCount();
                //crSmsSentA.requery();
                flag_cursor_F = 2;
                newItemCount = replaceRequerySent();
            }

            Log.d(TAG, "SMS SENT current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsSentA.moveToFirst();
                crSmsSentB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsSentA,
                        new String[] { "_id" }, crSmsSentB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;

                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsSent == CR_SMS_SENT_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM SENT ");
                            String body = crSmsSentA.getString(crSmsSentA
                                    .getColumnIndex("body"));
                            String id = crSmsSentA.getString(crSmsSentA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);

                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/SENT", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                            } else if ((msgType == 4) || (msgType == 5) || (msgType == 6)) {
                                Log.d(TAG,"Shouldn't reach here as you cannot " +
                                "move msg from SENT to any other folder");
                                Log.i(TAG, "Message Moved from SENT folder to OUTBOX");

                                /* Send the  delivery failure notification */
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/SENT", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_DELIVERY_FAILED, eventData, Constants.SMS_MASINSTANCE);
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsSent == CR_SMS_SENT_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM SENT ");
                            String body = crSmsSentB.getString(crSmsSentB
                                    .getColumnIndex("body"));
                            String id = crSmsSentB.getString(crSmsSentB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/SENT", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                                } else {
                                    Log.d(TAG, "Shouldn't reach here as " +
                                            "you cannot move msg from SENT to " +
                                    "any other folder");
                                }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            } else if (currentItemCount == newItemCount) {
                /* the Count is same for the messages but, some field might have has changed in one of the rows, find that row and read the delivery status from there */
                SendSmsDeliveryNotification();
            }
            if (currentCRSmsSent == CR_SMS_SENT_A) {
                currentCRSmsSent = CR_SMS_SENT_B;
            } else {
                currentCRSmsSent = CR_SMS_SENT_A;
            }
        }
    }
   private int replaceRequerySent() {
            Cursor c1 = null;
            int num = 0;
            Uri smsSentUri = Uri.parse(Constants.SENT_URI);
            try {
                c1 = mContext.getContentResolver().query(smsSentUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_E == 1) {
                try {
                    crSmsSentB.close();
                    crSmsSentB =  mContext.getContentResolver().query(smsSentUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_E = 0;
            }
            if (flag_cursor_F == 2) {
                try {
                    crSmsSentA.close();
                    crSmsSentA =  mContext.getContentResolver().query(smsSentUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_F = 0;
            }
            return num;
    }
    /**
     * This class listens for changes in Sms Content Provider's DRAFT table
     * It acts, only when a entry gets removed from the table
     */
    private class DraftContentObserverClass extends ContentObserver {

        public DraftContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            if (currentCRSmsDraft == CR_SMS_DRAFT_A) {
                currentItemCount = crSmsDraftA.getCount();
                //crSmsDraftB.requery();
                flag_cursor_G = 1;
                newItemCount = replaceRequeryDraft();
            } else {
                currentItemCount = crSmsDraftB.getCount();
                //crSmsDraftA.requery();
                flag_cursor_H = 2;
                newItemCount = replaceRequeryDraft();
            }

            Log.d(TAG, "SMS DRAFT current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsDraftA.moveToFirst();
                crSmsDraftB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsDraftA,
                        new String[] { "_id" }, crSmsDraftB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsDraft == CR_SMS_DRAFT_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM DRAFT ");
                            String body = crSmsDraftA.getString(crSmsDraftA
                                    .getColumnIndex("body"));
                            String id = crSmsDraftA.getString(crSmsDraftA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);

                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/DRAFT", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                String newFolder = getMAPFolder(msgType);
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/" + newFolder, "TELECOM/MSG/DRAFT", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                if ( newFolder.equalsIgnoreCase("sent")) {
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG/" + newFolder, "", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                    DeliveryStatusAwaitingMsgHandle.add(id);

                                    Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsDraft == CR_SMS_DRAFT_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM DRAFT ");
                            String body = crSmsDraftB.getString(crSmsDraftB
                                    .getColumnIndex("body"));
                            String id = crSmsDraftB.getString(crSmsDraftB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/DRAFT", "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                String newFolder = getMAPFolder(msgType);
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/" + newFolder, "TELECOM/MSG/DRAFT", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);
                                if ( newFolder.equalsIgnoreCase("sent")) {
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG/" + newFolder, "", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                    DeliveryStatusAwaitingMsgHandle.add(id);
                                    Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSmsDraft == CR_SMS_DRAFT_A) {
                currentCRSmsDraft = CR_SMS_DRAFT_B;
            } else {
                currentCRSmsDraft = CR_SMS_DRAFT_A;
            }
        }
    }
   private int replaceRequeryDraft() {
            Cursor c1 = null;
            int num = 0;
            Uri smsDraftUri = Uri.parse(Constants.DRAFT_URI);
            try {
                c1 = mContext.getContentResolver().query(smsDraftUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_G == 1) {
                try {
                    crSmsDraftB.close();
                    crSmsDraftB =  mContext.getContentResolver().query(smsDraftUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_G = 0;
            }
            if (flag_cursor_H == 2) {
                try {
                    crSmsDraftA.close();
                    crSmsDraftA =  mContext.getContentResolver().query(smsDraftUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_H = 0;
            }
            return num;
    }
    /**
     * This class listens for changes in Sms Content Provider's OUTBOX table
     * It acts only when a entry gets removed from the table
     */
    private class OutboxContentObserverClass extends ContentObserver {

        public OutboxContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            /* Check SMS OUTBOX for changes */

            if (currentCRSmsOutbox == CR_SMS_OUTBOX_A) {
                currentItemCount = crSmsOutboxA.getCount();
                //crSmsOutboxB.requery();
                flag_cursor_I = 1;
                newItemCount = replaceRequeryOutbox();
            } else {
                currentItemCount = crSmsOutboxB.getCount();
                //crSmsOutboxA.requery();
                flag_cursor_J = 2;
                newItemCount = replaceRequeryOutbox();
            }

            Log.d(TAG, "SMS OUTBOX current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsOutboxA.moveToFirst();
                crSmsOutboxB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsOutboxA,
                        new String[] { "_id" }, crSmsOutboxB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsOutbox == CR_SMS_OUTBOX_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM OUTBOX ");
                            String body = crSmsOutboxA.getString(crSmsOutboxA
                                    .getColumnIndex("body"));
                            String id = crSmsOutboxA.getString(crSmsOutboxA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);
                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                                if ( (msgType == MSG_CP_QUEUED_TYPE) ||
                                        (msgType == MSG_CP_FAILED_TYPE)) {
                                    /* Message moved from outbox to queue or failed folder */
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_SENT_FAILED, eventData1, Constants.SMS_MASINSTANCE);

                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsOutbox == CR_SMS_OUTBOX_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM OUTBOX ");
                            String body = crSmsOutboxB.getString(crSmsOutboxB
                                    .getColumnIndex("body"));
                            String id = crSmsOutboxB.getString(crSmsOutboxB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);

                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                            }
                            if ( (msgType == MSG_CP_QUEUED_TYPE) ||
                                    (msgType == MSG_CP_FAILED_TYPE)) {
                                /* Message moved from outbox to queue or failed folder */
                                BluetoothMapEventData eventData2 = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_SENT_FAILED, eventData2, Constants.SMS_MASINSTANCE);
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSmsOutbox == CR_SMS_OUTBOX_A) {
                currentCRSmsOutbox = CR_SMS_OUTBOX_B;
            } else {
                currentCRSmsOutbox = CR_SMS_OUTBOX_A;
            }
        }
    }
   private int replaceRequeryOutbox() {
            Cursor c1 = null;
            int num = 0;
            Uri smsOutboxUri = Uri.parse(Constants.OUTBOX_URI);
            try {
                c1 = mContext.getContentResolver().query(smsOutboxUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_I == 1) {
                try {
                    crSmsOutboxB.close();
                    crSmsOutboxB =  mContext.getContentResolver().query(smsOutboxUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_I = 0;
            }
            if (flag_cursor_J == 2) {
                try {
                    crSmsOutboxA.close();
                    crSmsOutboxA =  mContext.getContentResolver().query(smsOutboxUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_J = 0;
            }
            return num;
    }
    /**
     * This class listens for changes in Sms Content Provider's FAILED table
     * It acts only when a entry gets removed from the table
     */
    private class FailedContentObserverClass extends ContentObserver {

        public FailedContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            /* Mms doesn't have FAILED type */

            if (currentCRSmsFailed == CR_SMS_FAILED_A) {
                currentItemCount = crSmsFailedA.getCount();
                //crSmsFailedB.requery();
                flag_cursor_K = 1;
                newItemCount = replaceRequeryFailed();
            } else {
                currentItemCount = crSmsFailedB.getCount();
                //crSmsFailedA.requery();
                flag_cursor_L = 2;
                newItemCount = replaceRequeryFailed();
            }

            Log.d(TAG, "SMS FAILED current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsFailedA.moveToFirst();
                crSmsFailedB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsFailedA,
                        new String[] { "_id" }, crSmsFailedB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsFailed == CR_SMS_FAILED_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM FAILED ");
                            String body = crSmsFailedA.getString(crSmsFailedA
                                    .getColumnIndex("body"));
                            String id = crSmsFailedA.getString(crSmsFailedA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData, Constants.SMS_MASINSTANCE);

                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsFailed == CR_SMS_FAILED_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM FAILED ");
                            String body = crSmsFailedB.getString(crSmsFailedB
                                    .getColumnIndex("body"));
                            String id = crSmsFailedB.getString(crSmsFailedB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.SMS_MASINSTANCE);

                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData1, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSmsFailed == CR_SMS_FAILED_A) {
                currentCRSmsFailed = CR_SMS_FAILED_B;
            } else {
                currentCRSmsFailed = CR_SMS_FAILED_A;
            }
        }
    }
  private int replaceRequeryFailed() {
            Cursor c1 = null;
            int num = 0;
            Uri smsFailedUri = Uri.parse(Constants.FAILED_URI);
            try {
                c1 = mContext.getContentResolver().query(smsFailedUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_K == 1) {
                try {
                    crSmsFailedB.close();
                    crSmsFailedB =  mContext.getContentResolver().query(smsFailedUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_K = 0;
            }
            if (flag_cursor_L == 2) {
                try {
                    crSmsFailedA.close();
                    crSmsFailedA =  mContext.getContentResolver().query(smsFailedUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_L = 0;
            }
            return num;
    }

    /**
     * This class listens for changes in Sms Content Provider's QUEUED table
     * It acts only when a entry gets removed from the table
     */
    private class QueuedContentObserverClass extends ContentObserver {

        public QueuedContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            /* Mms doesn't have QUEUED type */

            if (currentCRSmsQueued == CR_SMS_QUEUED_A) {
                currentItemCount = crSmsQueuedA.getCount();
                //crSmsQueuedB.requery();
                flag_cursor_M = 1;
                newItemCount = replaceRequeryQueued();
            } else {
                currentItemCount = crSmsQueuedB.getCount();
                //crSmsQueuedA.requery();
                flag_cursor_N = 2;
                newItemCount = replaceRequeryQueued();
            }

            Log.d(TAG, "SMS QUEUED current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crSmsQueuedA.moveToFirst();
                crSmsQueuedB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crSmsQueuedA,
                        new String[] { "_id" }, crSmsQueuedB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCRSmsQueued == CR_SMS_QUEUED_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM QUEUED ");
                            String body = crSmsQueuedA.getString(crSmsQueuedA
                                    .getColumnIndex("body"));
                            String id = crSmsQueuedA.getString(crSmsQueuedA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCRSmsQueued == CR_SMS_QUEUED_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " SMS DELETED FROM QUEUED ");
                            String body = crSmsQueuedB.getString(crSmsQueuedB
                                    .getColumnIndex("body"));
                            String id = crSmsQueuedB.getString(crSmsQueuedB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED SMS ID " + id + " BODY "
                                    + body);
                            int msgType = getMessageType(id);
                            if (msgType == -1) {
                                BluetoothMapEventData eventData = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "SMS_GSM");
                            } else {
                                String newFolder = getMAPFolder(msgType);
                                if ((newFolder != null)
                                        && (!newFolder
                                                .equalsIgnoreCase("outbox"))) {
                                    /* The message has moved on MAP virtual folder representation. */
                                    BluetoothMapEventData eventData = createEventData(id,
                                            "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "SMS_GSM");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData, Constants.SMS_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "SMS_GSM");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                                        DeliveryStatusAwaitingMsgHandle.add(id);
                                        Log.i(TAG, "DeliveryStatusAwaitingMsgHandle size " + DeliveryStatusAwaitingMsgHandle.size());
                                    }
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCRSmsQueued == CR_SMS_QUEUED_A) {
                currentCRSmsQueued = CR_SMS_QUEUED_B;
            } else {
                currentCRSmsQueued = CR_SMS_QUEUED_A;
            }
        }
    }
      private int replaceRequeryQueued() {
            Cursor c1 = null;
            int num = 0;
            Uri smsQueuedUri = Uri.parse(Constants.QUEUED_URI);
            try {
                c1 = mContext.getContentResolver().query(smsQueuedUri, new String[] { "_id", "body", "type" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_M == 1) {
                try {
                    crSmsQueuedB.close();
                    crSmsQueuedB =  mContext.getContentResolver().query(smsQueuedUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_M = 0;
            }
            if (flag_cursor_N == 2) {
                try {
                    crSmsQueuedA.close();
                    crSmsQueuedA =  mContext.getContentResolver().query(smsQueuedUri, new String[] { "_id", "body", "type" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_N = 0;
            }
            return num;
    }


    private void SendSmsDeliveryNotification() {
        Cursor cr = null;
        /* For each of the pending handles, check if the delivery status failed of
         * succeeds and send delivery notification, for pending status do not send
         * any delivery notification
         *
         * In this case it seems that there would only be one handle for which
         * the delivery notification would be received */
        if ((DeliveryStatusAwaitingMsgHandle == null) || (DeliveryStatusAwaitingMsgHandle.size() == 0)) {
            Log.i(TAG, "SendSmsDeliveryNotification returning empty list");
            return;
        }
        for (int i = 0; i < DeliveryStatusAwaitingMsgHandle.size(); i++) {
            Uri uri = Uri.parse(Constants.SMS_URI);
            String whereClause = " _id = " + DeliveryStatusAwaitingMsgHandle.get(i);

            Log.d(TAG, "whereClause is : " + DeliveryStatusAwaitingMsgHandle.get(i));
            try {
                int statusIndex = -1;
                String DeliverySuccess = "0";
                String DeliveryFail = "64";
                /* Read its delivery report value, and send notification if OK or FAIL */
                cr = mContext.getContentResolver().query(uri, null, whereClause, null,
                        null);

                if ((cr != null) && (cr.moveToFirst())) {
                    statusIndex = cr.getColumnIndex("status");

                    if (cr.getString(statusIndex).compareTo(DeliverySuccess) == 0) {
                        BluetoothMapEventData eventData1 = createEventData(DeliveryStatusAwaitingMsgHandle.get(i),
                                "TELECOM/MSG/SENT" , "", "SMS_GSM");
                        sendEvent(Constants.MAP_EVENT_TYPE_DELIVERY_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                        DeliveryStatusAwaitingMsgHandle.remove(i);
                        break;
                    } else if (cr.getString(statusIndex).compareTo(DeliveryFail) == 0) {
                        BluetoothMapEventData eventData1 = createEventData(DeliveryStatusAwaitingMsgHandle.get(i),
                                "TELECOM/MSG/SENT" , "", "SMS_GSM");
                        sendEvent(Constants.MAP_EVENT_TYPE_DELIVERY_FAILED, eventData1, Constants.SMS_MASINSTANCE);
                        DeliveryStatusAwaitingMsgHandle.remove(i);
                        break;
                    }
                } else {
                    Log.i(TAG, "SendSmsDeliveryNotification Cursor Found Null");
                }
            } catch (Exception e) {
                Log.e(TAG, "Exception " + e);
            } finally {
                cr.close();
            }
        }
    }

    /**
     * This class listens for changes in Email Content Provider
     * It acts, only when a new entry gets added to database
     */
    private class EmailContentObserverClass extends ContentObserver {

        public EmailContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;
            EmailUtils eu = new EmailUtils();
            String containingFolder = null;

            /* Synchronize this? */
            if (currentCREmail == CR_EMAIL_A) {
                currentItemCount = crEmailA.getCount();
                //crEmailB.requery();
                flag_cursor_O = 1;
                newItemCount = replaceRequeryEmail();
            } else {
                currentItemCount = crEmailB.getCount();
                //crEmailA.requery();
                flag_cursor_P = 2;
                newItemCount = replaceRequeryEmail();
            }

            Log.d(TAG, "Email current " + currentItemCount + " new "
                    + newItemCount);

            if (newItemCount > currentItemCount) {
                crEmailA.moveToFirst();
                crEmailB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crEmailA,
                        new String[] { "_id" }, crEmailB, new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCREmail == CR_EMAIL_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                            Log.d(TAG, " EMAIL ADDED TO INBOX ");
                            String id1 = crEmailA.getString(crEmailA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " ADDED EMAIL ID " + id1);
                            Cursor cr1 = null;
                            int folderId;
                            Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                            String whereClause = " _id = " + id1;
                            try {
                                cr1 = mContext.getContentResolver().query(uri1, null, whereClause, null,
                                        null);
                                if ( cr1.moveToFirst()) {
                                    do {
                                        for (int i=0;i<cr1.getColumnCount();i++) {
                                            Log.d(TAG, " Column Name: "+ cr1.getColumnName(i) + " Value: " + cr1.getString(i));
                                        }
                                    } while ( cr1.moveToNext());
                                }

                                if (cr1.getCount() > 0) {
                                    cr1.moveToFirst();
                                    folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                    containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                }

                                if ( ( containingFolder != null ) && (containingFolder.compareToIgnoreCase("inbox") == 0)) {
                                    Log.d(TAG, " containingFolder:: "+containingFolder);
                                    id1 = Integer.toString(Integer.valueOf(id1)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);

                                    BluetoothMapEventData eventData1 = createEventData(id1,
                                            "TELECOM/MSG/" + containingFolder , "", "EMAIL");
                                    sendEvent(Constants.MAP_EVENT_TYPE_NEW_MSG, eventData1, Constants.EMAIL_MASINSTANCE);

                                    /*
                                     * instead of sending new message notification from here, queue it
                                     * and dequeue when other content observer notifications are received
                                     * NewEmailNotificationHandle.add(id1);
                                     */
                                } else {
                                    Log.d(TAG, " ADDED TO UNKNOWN FOLDER");
                                }
                            } catch (Exception e) {
                                Log.e(TAG, "Exception " + e);
                            } finally {
                                cr1.close();
                            }
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCREmail == CR_EMAIL_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                            Log.d(TAG, " EMAIL ADDED ");
                            String id1 = crEmailB.getString(crEmailB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " ADDED EMAIL ID " + id1);
                            Cursor cr1 = null;
                            int folderId;
                            Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                            String whereClause = " _id = " + id1;
                            try {
                                cr1 = mContext.getContentResolver().query(uri1, null, whereClause, null,
                                        null);

                                if ( cr1.moveToFirst()) {
                                    do {
                                        for (int i=0;i<cr1.getColumnCount();i++) {
                                            Log.d(TAG, " Column Name: "+ cr1.getColumnName(i) +
                                                    " Value: " + cr1.getString(i));
                                        }
                                    } while ( cr1.moveToNext());
                                }

                                if (cr1.getCount() > 0) {
                                    cr1.moveToFirst();
                                    folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                    containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                }
                                if ( ( containingFolder != null ) && (containingFolder.compareToIgnoreCase("inbox") == 0)) {
                                    Log.d(TAG, " containingFolder:: "+containingFolder);
                                    id1 = Integer.toString(Integer.valueOf(id1)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);

                                    BluetoothMapEventData eventData1 = createEventData(id1,
                                            "TELECOM/MSG/" + containingFolder , "", "EMAIL");
                                    sendEvent(Constants.MAP_EVENT_TYPE_NEW_MSG, eventData1, Constants.EMAIL_MASINSTANCE);

                                    /**
                                     * instead of sending new message notification from here, queue it
                                     * and dequeue when other content observer notifications are received
                                     * NewEmailNotificationHandle.add(id1);
                                     */
                                } else {
                                    Log.d(TAG, " ADDED TO UNKNOWN FOLDER");
                                }
                            } catch (Exception e) {
                                Log.e(TAG, "Exception " + e);
                            } finally {
                                cr1.close();
                            }
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }

            if (currentCREmail == CR_EMAIL_A) {
                currentCREmail = CR_EMAIL_B;
            } else {
                currentCREmail = CR_EMAIL_A;
            }
        }
    }
    private int replaceRequeryEmail() {
            Cursor c1 = null;
            int num = 0;
            Uri EmailUriNoti = Uri.parse(Constants.EMAIL_URI);
            try {
                c1 = mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, null, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_O == 1) {
                try {
                    crEmailB.close();
                    crEmailB =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_O = 0;
            }
            if (flag_cursor_P == 2) {
                try {
                    crEmailA.close();
                    crEmailA =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, null, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_P = 0;
            }
            return num;
    }

    /**
     * This class listens for changes in Email Content Provider's inbox table
     * It acts, only when a entry gets removed from the table
     */
    private class EmailInboxContentObserverClass extends ContentObserver {

        public EmailInboxContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;

            try {
                if (currentCREmailInbox == CR_EMAIL_INBOX_A) {
                    currentItemCount = crEmailInboxA.getCount();
                    //crEmailInboxB.requery();
                    flag_cursor_Q = 1;
                    newItemCount = replaceRequeryEmailInbox();
                } else {
                    currentItemCount = crEmailInboxB.getCount();
                    //crEmailInboxA.requery();
                    flag_cursor_R = 2;
                    newItemCount = replaceRequeryEmailInbox();
                }
            } catch (Exception e) {
                Log.e(TAG,e.toString());
            }
            Log.d(TAG, "EMAIL INBOX current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crEmailInboxA.moveToFirst();
                crEmailInboxB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crEmailInboxA,
                        new String[] { "_id" }, crEmailInboxB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCREmailInbox == CR_EMAIL_INBOX_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM INBOX ");
                            String id = crEmailInboxA.getString(crEmailInboxA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);

                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/INBOX" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Log.d(TAG, "Shouldn't reach here as you cannot "
                                        + "move msg from INBOX to any other folder");
                            }

                        } else {
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCREmailInbox == CR_EMAIL_INBOX_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM INBOX ");
                            String id = crEmailInboxB.getString(crEmailInboxB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/INBOX" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Log.d(TAG, "Shouldn't reach here as you cannot "
                                        + "move msg from INBOX to any other folder");
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }

            if (currentCREmailInbox == CR_EMAIL_INBOX_A) {
                currentCREmailInbox = CR_EMAIL_INBOX_B;
            } else {
                currentCREmailInbox = CR_EMAIL_INBOX_A;
            }
        }
    }
      private int replaceRequeryEmailInbox() {
            Cursor c1 = null;
            int num = 0;
            EmailUtils eu = new EmailUtils();
            String emailInboxCondition = eu.getWhereIsQueryForTypeEmail("inbox", mContext);
            Uri EmailUriNoti = Uri.parse(Constants.EMAIL_URI);
            try {
                c1 = mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailInboxCondition, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            }  finally {
                c1.close();
            }
            if (flag_cursor_Q == 1) {
                try {
                    crEmailInboxB.close();
                    crEmailInboxB =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailInboxCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_Q = 0;
            }
            if (flag_cursor_R == 2) {
                try {
                    crEmailInboxA.close();
                    crEmailInboxA =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailInboxCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_R = 0;
            }
            return num;
    }

    /**
     * This class listens for changes in Email Content Provider's SENT table
     * It acts, only when a entry gets removed from the table
     */
    private class EmailSentContentObserverClass extends ContentObserver {

        public EmailSentContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;
            try {
                if (currentCREmailSent == CR_EMAIL_SENT_A) {
                    currentItemCount = crEmailSentA.getCount();
                    //crEmailSentB.requery();
                    flag_cursor_S = 1;
                    newItemCount = replaceRequeryEmailSent();
                } else {
                    currentItemCount = crEmailSentB.getCount();
                    //crEmailSentA.requery();
                    flag_cursor_T = 2;
                    newItemCount = replaceRequeryEmailSent();
                }
            } catch (Exception e) {
                Log.e(TAG,e.toString());
            }

            Log.d(TAG, "EMAIL SENT current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crEmailSentA.moveToFirst();
                crEmailSentB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crEmailSentA,
                        new String[] { "_id" }, crEmailSentB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCREmailSent == CR_EMAIL_SENT_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM SENT ");
                            String id = crEmailSentA.getString(crEmailSentA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);

                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/SENT" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Log.d(TAG,"Shouldn't reach here as you cannot " +
                                "move msg from SENT to any other folder");
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCREmailSent == CR_EMAIL_SENT_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM SENT ");
                            String id = crEmailSentB.getString(crEmailSentB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);

                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/SENT" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Log.d(TAG, "Shouldn't reach here as " +
                                        "you cannot move msg from SENT to " +
                                "any other folder");
                            }
                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCREmailSent == CR_EMAIL_SENT_A) {
                currentCREmailSent = CR_EMAIL_SENT_B;
            } else {
                currentCREmailSent = CR_EMAIL_SENT_A;
            }
        }
    }
  private int replaceRequeryEmailSent() {
            Cursor c1 = null;
            int num = 0;
            EmailUtils eu = new EmailUtils();
            String emailSentCondition = eu.getWhereIsQueryForTypeEmail("sent", mContext);
            Uri EmailUriNoti = Uri.parse(Constants.EMAIL_URI);
            try {
                c1 = mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailSentCondition, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_S == 1) {
                try {
                    crEmailSentB.close();
                    crEmailSentB =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailSentCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_S = 0;
            }
            if (flag_cursor_T == 2) {
                try {
                    crEmailSentA.close();
                    crEmailSentA =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailSentCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_T = 0;
            }
            return num;
    }

    /**
     * This class listens for changes in Email Content Provider's DRAFT table
     * It acts, only when a entry gets removed from the table
     */
    private class EmailDraftContentObserverClass extends ContentObserver {

        public EmailDraftContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;
            try {
                if (currentCREmailDraft == CR_EMAIL_DRAFT_A) {
                    currentItemCount = crEmailDraftA.getCount();
                    //crEmailDraftB.requery();
                    flag_cursor_U = 1;
                    newItemCount = replaceRequeryEmailDraft();
                } else {
                    currentItemCount = crEmailDraftB.getCount();
                    //crEmailDraftA.requery();
                    flag_cursor_V = 2;
                    newItemCount = replaceRequeryEmailDraft();
                }
            } catch (Exception e) {
                Log.e(TAG,e.toString());
            }
            Log.d(TAG, "EMAIL DRAFT current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crEmailDraftA.moveToFirst();
                crEmailDraftB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crEmailDraftA,
                        new String[] { "_id" }, crEmailDraftB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCREmailDraft == CR_EMAIL_DRAFT_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM DRAFT ");
                            String id = crEmailDraftA.getString(crEmailDraftA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);

                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag != 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/DRAFT" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Cursor cr1 = null;
                                int folderId;
                                String containingFolder = null;
                                EmailUtils eu = new EmailUtils();
                                Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                                String whereClause = " _id = " + id;
                                try {
                                    cr1 = mContext.getContentResolver().query(uri1, null, whereClause, null,
                                            null);

                                    if (cr1.getCount() > 0) {
                                        cr1.moveToFirst();
                                        folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                        containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                    }

                                    String newFolder = containingFolder;
                                    id = Integer.toString(Integer.valueOf(id)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG" + newFolder, "TELECOM/MSG/DRAFT", "EMAIL");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData1, Constants.EMAIL_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "EMAIL");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData, Constants.EMAIL_MASINSTANCE);

                                    }
                                } catch (Exception e) {
                                    Log.e(TAG,e.toString());
                                } finally {
                                    if (cr1 != null)
                                        cr1.close();
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCREmailDraft == CR_EMAIL_DRAFT_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM DRAFT ");
                            String id = crEmailDraftB.getString(crEmailDraftB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag != 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/DRAFT" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Cursor cr1 = null;
                                int folderId;
                                String containingFolder = null;
                                EmailUtils eu = new EmailUtils();
                                Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                                String whereClause = " _id = " + id;
                                try {
                                    cr1 = mContext.getContentResolver().query(uri1, null, whereClause, null,
                                            null);

                                    if (cr1.getCount() > 0) {
                                        cr1.moveToFirst();
                                        folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                        containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                    }

                                    String newFolder = containingFolder;
                                    id = Integer.toString(Integer.valueOf(id)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                    BluetoothMapEventData eventData1 = createEventData(id,
                                            "TELECOM/MSG" + newFolder, "", "EMAIL");
                                    sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData1, Constants.EMAIL_MASINSTANCE);

                                    if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "EMAIL");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData, Constants.EMAIL_MASINSTANCE);

                                    }
                                } catch (Exception e) {
                                    Log.e(TAG,e.toString());
                                } finally {
                                    if (cr1 != null)
                                        cr1.close();
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCREmailDraft == CR_EMAIL_DRAFT_A) {
                currentCREmailDraft = CR_EMAIL_DRAFT_B;
            } else {
                currentCREmailDraft = CR_EMAIL_DRAFT_A;
            }
        }
    }
  private int replaceRequeryEmailDraft() {
            Cursor c1 = null;
            int num = 0;
            EmailUtils eu = new EmailUtils();
            String emailDraftCondition = eu.getWhereIsQueryForTypeEmail("drafts", mContext);
            Uri EmailUriNoti = Uri.parse(Constants.EMAIL_URI);
            try {
                c1 = mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailDraftCondition, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_U == 1) {
                try {
                    crEmailDraftB.close();
                    crEmailDraftB =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailDraftCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_U = 0;
            }
            if (flag_cursor_V == 2) {
                try {
                    crEmailDraftA.close();
                    crEmailDraftA =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailDraftCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_V = 0;
            }
            return num;
    }

    /**
     * This class listens for changes in Sms Content Provider's OUTBOX table
     * It acts only when a entry gets removed from the table
     */
    private class EmailOutboxContentObserverClass extends ContentObserver {

        public EmailOutboxContentObserverClass() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);

            int currentItemCount = 0;
            int newItemCount = 0;
            try {
                if (currentCREmailOutbox == CR_EMAIL_OUTBOX_A) {
                    currentItemCount = crEmailOutboxA.getCount();
                    //crEmailOutboxB.requery();
                    flag_cursor_W = 1;
                    newItemCount = replaceRequeryEmailOutbox();
                } else {
                    currentItemCount = crEmailOutboxB.getCount();
                    //crEmailOutboxA.requery();
                    flag_cursor_X = 2;
                    newItemCount = replaceRequeryEmailOutbox();
                }
            } catch (Exception e) {
                Log.e(TAG,e.toString());
            }
            Log.d(TAG, "EMAIL OUTBOX current " + currentItemCount + " new "
                    + newItemCount);

            if (currentItemCount > newItemCount) {
                crEmailOutboxA.moveToFirst();
                crEmailOutboxB.moveToFirst();

                CursorJoiner joiner = new CursorJoiner(crEmailOutboxA,
                        new String[] { "_id" }, crEmailOutboxB,
                        new String[] { "_id" });

                CursorJoiner.Result joinerResult;
                while (joiner.hasNext()) {
                    joinerResult = joiner.next();
                    switch (joinerResult) {
                    case LEFT:
                        /* handle case where a row in cursor1 is unique */
                        if (currentCREmailOutbox == CR_EMAIL_OUTBOX_A) {
                            /* The new query doesn't have this row; implies it was deleted */
                            String id = crEmailOutboxA.getString(crEmailOutboxA
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Cursor cr1 = null;
                                int folderId;
                                String containingFolder = null;
                                EmailUtils eu = new EmailUtils();
                                Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                                String whereClause = " _id = " + id;
                                try {
                                    cr1 = mContext.getContentResolver().query(uri1, null, whereClause, null,
                                            null);

                                    if (cr1.getCount() > 0) {
                                        cr1.moveToFirst();
                                        folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                        containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                    }

                                    String newFolder = containingFolder;
                                    id = Integer.toString(Integer.valueOf(id)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                    if ((newFolder != null) && (!newFolder.equalsIgnoreCase("outbox"))) {
                                        /* The message has moved on MAP virtual folder representation. */
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "EMAIL");
                                        sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData1, Constants.EMAIL_MASINSTANCE);

                                        if ( newFolder.equalsIgnoreCase("sent")) {
                                            BluetoothMapEventData eventData = createEventData(id,
                                                    "TELECOM/MSG/" + newFolder , "", "EMAIL");
                                            sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData, Constants.EMAIL_MASINSTANCE);
                                        }
                                    }
                                }
                                catch (Exception e) {
                                    Log.e(TAG,e.toString());
                                } finally {
                                    if (cr1 != null)
                                        cr1.close();
                                }
                            }

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case RIGHT:
                        /* handle case where a row in cursor2 is unique */
                        if (currentCREmailOutbox == CR_EMAIL_OUTBOX_B) {
                            /* The new query doesn't have this row; implies it was deleted */
                            Log.d(TAG, " EMAIL DELETED FROM OUTBOX ");
                            String id = crEmailOutboxB.getString(crEmailOutboxB
                                    .getColumnIndex("_id"));
                            Log.d(TAG, " DELETED EMAIL ID " + id);
                            int deletedFlag = getDeletedFlagEmail(id);
                            if (deletedFlag == 1) {
                                id = Integer.toString(Integer.valueOf(id)
                                        + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                BluetoothMapEventData eventData1 = createEventData(id,
                                        "TELECOM/MSG/OUTBOX" , "", "EMAIL");
                                sendEvent(Constants.MAP_EVENT_TYPE_MSG_DELETED, eventData1, Constants.EMAIL_MASINSTANCE);

                            } else {
                                Cursor cr1 = null;
                                int folderId;
                                String containingFolder = null;
                                EmailUtils eu = new EmailUtils();
                                Uri uri1 = Uri.parse(Constants.EMAIL_URI);
                                String whereClause = " _id = " + id;
                                try {
                                    cr1 = mContext.getContentResolver().query(uri1, null, whereClause,
                                             null, null);

                                    if (cr1.getCount() > 0) {
                                        cr1.moveToFirst();
                                        folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                                        containingFolder = eu.getContainingFolderEmail(folderId, mContext);
                                    }

                                    String newFolder = containingFolder;
                                    id = Integer.toString(Integer.valueOf(id)
                                            + BluetoothMapMessageAccessor.EMAIL_HDLR_CONSTANT);
                                    if ((newFolder != null) && (!newFolder.equalsIgnoreCase("outbox")) && (!newFolder.equalsIgnoreCase("sent"))) {
                                        /* The message has moved on MAP virtual folder representation. */
                                        BluetoothMapEventData eventData1 = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "TELECOM/MSG/OUTBOX", "EMAIL");
                                        sendEvent(Constants.MAP_EVENT_TYPE_MSG_SHIFT, eventData1, Constants.EMAIL_MASINSTANCE);

                                    } else  if ( newFolder.equalsIgnoreCase("sent")) {
                                        BluetoothMapEventData eventData = createEventData(id,
                                                "TELECOM/MSG/" + newFolder , "", "EMAIL");
                                        sendEvent(Constants.MAP_EVENT_TYPE_SENT_SUCCESS, eventData, Constants.EMAIL_MASINSTANCE);
                                    }

                                }
                                catch (Exception e) {
                                    Log.e(TAG,e.toString());
                                } finally {
                                    if (cr1 != null)
                                        cr1.close();
                                }}

                        } else {
                            /* The current(old) query doesn't have this row implies it was added */
                        }
                        break;
                    case BOTH:
                        /* handle case where a row with the same key is in both cursors */
                        break;
                    }
                }
            }
            if (currentCREmailOutbox == CR_EMAIL_OUTBOX_A) {
                currentCREmailOutbox = CR_EMAIL_OUTBOX_B;
            } else {
                currentCREmailOutbox = CR_EMAIL_OUTBOX_A;
            }
        }
    }
 private int replaceRequeryEmailOutbox() {
            Cursor c1 = null;
            int num = 0;
            EmailUtils eu = new EmailUtils();
            String emailOutboxCondition = eu.getWhereIsQueryForTypeEmail("outbox", mContext);
            Uri EmailUriNoti = Uri.parse(Constants.EMAIL_URI);
            try {
                c1 = mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailOutboxCondition, null, null);
                if (c1 != null) {
                    num = c1.getCount();
                }
            } finally {
                c1.close();
            }
            if (flag_cursor_W == 1) {
                try {
                    crEmailOutboxB.close();
                    crEmailOutboxB =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailOutboxCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_W = 0;
            }
            if (flag_cursor_X == 2) {
                try {
                    crEmailOutboxA.close();
                    crEmailOutboxA =  mContext.getContentResolver().query(EmailUriNoti, new String[] { "_id", "mailboxkey" }, emailOutboxCondition, null, "_id asc");
                } catch (Exception e) {
                    Log.e(TAG,e.toString());
                }
                flag_cursor_X = 0;
            }
            return num;
    }
    public void RegisterMessageChangeObservers() {
        Log.i(TAG, "Entering RegisterMessageChangeObservers");

        /* This is notification channel for SMS, register sms change observers */
        /* Currently involve NewMessage, MessageDeleted */
        Uri smsUri = Uri.parse(Constants.SMS_URI);
        crSmsA = mContext.getContentResolver().query(smsUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsB = mContext.getContentResolver().query(smsUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsInboxUri = Uri.parse(Constants.INBOX_URI);
        crSmsInboxA = mContext.getContentResolver().query(smsInboxUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsInboxB = mContext.getContentResolver().query(smsInboxUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsSentUri = Uri.parse(Constants.SENT_URI);
        crSmsSentA = mContext.getContentResolver().query(smsSentUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsSentB = mContext.getContentResolver().query(smsSentUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsDraftUri = Uri.parse(Constants.DRAFT_URI);
        crSmsDraftA = mContext.getContentResolver().query(smsDraftUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsDraftB = mContext.getContentResolver().query(smsDraftUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsOutboxUri = Uri.parse(Constants.OUTBOX_URI);
        crSmsOutboxA = mContext.getContentResolver().query(smsOutboxUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsOutboxB = mContext.getContentResolver().query(smsOutboxUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsFailedUri = Uri.parse(Constants.FAILED_URI);
        crSmsFailedA = mContext.getContentResolver().query(smsFailedUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsFailedB = mContext.getContentResolver().query(smsFailedUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsQueuedUri = Uri.parse(Constants.QUEUED_URI);
        crSmsQueuedA = mContext.getContentResolver().query(smsQueuedUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");
        crSmsQueuedB = mContext.getContentResolver().query(smsQueuedUri,
                new String[] { "_id", "body", "type" }, null, null, "_id asc");

        Uri smsObserverUri = Uri.parse(Constants.SMS_MMS);
        mContext.getContentResolver().registerContentObserver(smsObserverUri,
                true, smsContentObserver);

        /* Other content observers associated with SMS */
        Uri smsInboxObserverUri = Uri.parse(Constants.SMS_MMS_INBOX);
        mContext.getContentResolver().registerContentObserver(
                smsInboxObserverUri, true, inboxContentObserver);

        Uri smsSentObserverUri = Uri.parse(Constants.SMS_MMS_SENT);
        mContext.getContentResolver().registerContentObserver(
                smsSentObserverUri, true, sentContentObserver);

        Uri smsDraftObserverUri = Uri.parse(Constants.SMS_MMS_DRAFT);
        mContext.getContentResolver().registerContentObserver(
                smsDraftObserverUri, true, draftContentObserver);

        Uri smsOutboxObserverUri = Uri.parse(Constants.SMS_MMS_OUTBOX);
        mContext.getContentResolver().registerContentObserver(
                smsOutboxObserverUri, true, outboxContentObserver);

        Uri smsFailedObserverUri = Uri.parse(Constants.SMS_MMS_FAILED);
        mContext.getContentResolver().registerContentObserver(
                smsFailedObserverUri, true, failedContentObserver);

        Uri smsQueuedObserverUri = Uri.parse(Constants.SMS_MMS_QUEUED);
        mContext.getContentResolver().registerContentObserver(
                smsQueuedObserverUri, true, queuedContentObserver);

        /*email start */
        Uri emailUri = Uri.parse(Constants.EMAIL_URI);
        crEmailA = mContext.getContentResolver().query(emailUri,
                new String[] { "_id", "mailboxkey" }, null, null, "_id asc");
        crEmailB = mContext.getContentResolver().query(emailUri,
                new String[] { "_id", "mailboxkey" }, null, null, "_id asc");

        EmailUtils eu = new EmailUtils();
        String emailInboxCondition = eu.getWhereIsQueryForTypeEmail("inbox", mContext);
        crEmailInboxA = mContext.getContentResolver().query(emailUri,
                new String[] {  "_id", "mailboxkey"  }, emailInboxCondition, null, "_id asc");
        crEmailInboxB = mContext.getContentResolver().query(emailUri,
                new String[] {  "_id", "mailboxkey" }, emailInboxCondition, null, "_id asc");

        String emailSentCondition = eu.getWhereIsQueryForTypeEmail("sent", mContext);
        crEmailSentA = mContext.getContentResolver().query(emailUri,
                new String[] {"_id", "mailboxkey" }, emailSentCondition, null, "_id asc");
        crEmailSentB = mContext.getContentResolver().query(emailUri,
                new String[] {"_id", "mailboxkey" }, emailSentCondition, null, "_id asc");

        String emailDraftCondition = eu.getWhereIsQueryForTypeEmail("drafts", mContext);
        crEmailDraftA = mContext.getContentResolver().query(emailUri,
                new String[] {"_id", "mailboxkey"}, emailDraftCondition, null, "_id asc");
        crEmailDraftB = mContext.getContentResolver().query(emailUri,
                new String[] {"_id", "mailboxkey" }, emailDraftCondition, null, "_id asc");

        String emailOutboxCondition = eu.getWhereIsQueryForTypeEmail("outbox", mContext);
        crEmailOutboxA = mContext.getContentResolver().query(emailUri,
                new String[] {"_id", "mailboxkey"}, emailOutboxCondition, null, "_id asc");
        crEmailOutboxB = mContext.getContentResolver().query(emailUri,
                new String[] { "_id", "mailboxkey"}, emailOutboxCondition, null, "_id asc");

        Uri emailObserverUri = Uri.parse(Constants.EMAIL_URI);
        mContext.getContentResolver().registerContentObserver(emailObserverUri,
                true, emailContentObserver);

        Uri emailInboxObserverUri = Uri.parse(Constants.EMAIL_URI_NOTIFICATION);
        mContext.getContentResolver().registerContentObserver(
                emailInboxObserverUri, true, emailInboxContentObserver);

        Uri emailSentObserverUri = Uri.parse(Constants.EMAIL_URI_NOTIFICATION);
        mContext.getContentResolver().registerContentObserver(
                emailSentObserverUri, true, emailSentContentObserver);

        Uri emailDraftObserverUri = Uri.parse(Constants.EMAIL_URI_NOTIFICATION);
        mContext.getContentResolver().registerContentObserver(
                emailDraftObserverUri, true, emailDraftContentObserver);

        Uri emailOutboxObserverUri = Uri.parse(Constants.EMAIL_URI_NOTIFICATION);
        mContext.getContentResolver().registerContentObserver(
                emailOutboxObserverUri, true, emailOutboxContentObserver);

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_DEVICE_STORAGE_LOW);
        filter.addAction(Intent.ACTION_DEVICE_STORAGE_OK);
        mContext.registerReceiver(mStorageStatusReceiver, filter);
        /*email end */

        updatesRegistered = true;
    }

    private void deregisterMessageChangeObservers() {
        Log.i(TAG, "Entering deregisterMessageChangeObservers");

        /* This is notification channel for SMS, de-register sms change observers */
        /* Currently involve NewMessage, MessageDeleted */
        if (updatesRegistered == true) {
            Log.d(TAG, "DEREGISTER MNS SMS UPDATES");
            mContext.getContentResolver().unregisterContentObserver(
                    smsContentObserver);
            crSmsA.close();
            crSmsB.close();
            currentCRSms = CR_SMS_A;
            crSmsInboxA.close();
            crSmsInboxB.close();
            currentCRSmsInbox = CR_SMS_INBOX_A;
            crSmsSentA.close();
            crSmsSentB.close();
            currentCRSmsSent = CR_SMS_SENT_A;
            crSmsDraftA.close();
            crSmsDraftB.close();
            currentCRSmsDraft = CR_SMS_DRAFT_A;
            crSmsOutboxA.close();
            crSmsOutboxB.close();
            currentCRSmsOutbox = CR_SMS_OUTBOX_A;
            crSmsFailedA.close();
            crSmsFailedB.close();
            currentCRSmsFailed = CR_SMS_FAILED_A;
            crSmsQueuedA.close();
            crSmsQueuedB.close();
            currentCRSmsQueued = CR_SMS_QUEUED_A;

            Log.d(TAG, "DEREGISTER MNS SMS UPDATES");
            mContext.getContentResolver().unregisterContentObserver(
                    inboxContentObserver);
            mContext.getContentResolver().unregisterContentObserver(
                    sentContentObserver);
            mContext.getContentResolver().unregisterContentObserver(
                    draftContentObserver);
            mContext.getContentResolver().unregisterContentObserver(
                    outboxContentObserver);
            mContext.getContentResolver().unregisterContentObserver(
                    failedContentObserver);
            mContext.getContentResolver().unregisterContentObserver(
                    queuedContentObserver);
            /*email start */
            mContext.getContentResolver().unregisterContentObserver(
                    emailContentObserver);
            crEmailA.close();
            crEmailB.close();
            currentCREmail = CR_EMAIL_A;
            crEmailOutboxA.close();
            crEmailOutboxB.close();
            currentCREmailOutbox = CR_EMAIL_OUTBOX_A;
            crEmailDraftA.close();
            crEmailDraftB.close();
            currentCREmailDraft = CR_EMAIL_DRAFT_A;
            crEmailInboxA.close();
            crEmailInboxB.close();
            currentCREmailInbox = CR_EMAIL_INBOX_A;
            crEmailSentA.close();
            crEmailSentB.close();
            currentCREmailSent = CR_EMAIL_SENT_A;
            /*email end */

            /*deregister the storage change observer as well */
            mContext.unregisterReceiver(mStorageStatusReceiver);
            updatesRegistered = false;
        }
    }

    /**
     * Gets the table type (as in Email Content Provider) for the
     * given id
     */
    private int getDeletedFlagEmail(String id) {
        int deletedFlag = 0;
        Cursor cr = null;
        try {
            cr = mContext.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI + "/" + id),
                    new String[] { "_id", "mailboxKey" }, null, null, null);
            int folderId = -1;
            if (cr.moveToFirst()) {
                folderId = cr.getInt(cr.getColumnIndex("mailboxKey"));
            }
            Cursor cr1 = null;
            try {
                cr1 = mContext.getContentResolver().query(
                        Uri.parse(Constants.EMAIL_URI_MAILBOX),
                        new String[] { "_id", "displayName" }, "_id ="+ folderId, null, null);
                String folderName = null;
                if (cr1.moveToFirst()) {
                    folderName = cr1.getString(cr1.getColumnIndex("displayName"));
                    }
                   if (folderName !=null && (folderName.equalsIgnoreCase("Trash") ||
                      folderName.toUpperCase().contains("TRASH"))) {
                        deletedFlag = 1;
                }} catch (Exception e) {
                    Log.e(TAG, "Exception " + e);
                } finally {
                    if (cr1 != null)
                        cr1.close();
                }
        } catch (Exception e) {
            Log.e(TAG, "Exception " + e);
        } finally {
             if (cr != null)
                 cr.close();
        }
        return deletedFlag;
    }

    /**
     * Get the folder name (MAP representation) based on the
     * folder type value in SMS database
     */
    private String getMAPFolder(int type) {
        String folder = null;
        switch (type) {
        case 1:
            folder = "inbox";
            break;
        case 2:
            folder = "sent";
            break;
        case 3:
            folder = "draft";
            break;
        case 4:
        case 5:
        case 6:
            folder = "outbox";
            break;
        default:
            break;
        }
        return folder;
    }

    public BluetoothMapEventData createEventData(String handle, String folder, String old_folder, String msg_type) {
        BluetoothMapEventData eventData = new BluetoothMapEventData();
        eventData.folder = folder;
        eventData.handle = handle;
        eventData.old_folder = old_folder;
        eventData.msg_type = msg_type;

        return eventData;
    }

    /**
     * Gets the table type (as in Sms Content Provider) for the
     * given id
     */
    private int getMessageType(String id) {
        Cursor cr = null;
        try {
            cr = mContext.getContentResolver().query(
                    Uri.parse(Constants.SMS_URI + id),
                    new String[] { "_id", "type" }, null, null, null);
            if (cr.moveToFirst()) {
                return cr.getInt(cr.getColumnIndex("type"));
            }
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        } finally {
            if (cr != null)
                cr.close();
        }
        return -1;
    }

    private BroadcastReceiver mStorageStatusReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(Intent.ACTION_DEVICE_STORAGE_LOW)) {
                Log.d(TAG, " Memory Full ");
                BluetoothMapEventData eventData1 = createEventData(null, null , null, null);
                sendEvent(Constants.MAP_EVENT_TYPE_MEMORY_FULL, eventData1, Constants.SMS_MASINSTANCE);

            } else if (intent.getAction().equals(Intent.ACTION_DEVICE_STORAGE_OK)) {
                Log.d(TAG, " Memory Available ");
                BluetoothMapEventData eventData1 = createEventData(null, null, null, null);
                sendEvent(Constants.MAP_EVENT_TYPE_MEMORY_AVAIL, eventData1, Constants.SMS_MASINSTANCE);

            }
        }
    };
};
