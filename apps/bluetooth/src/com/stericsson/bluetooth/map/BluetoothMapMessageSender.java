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

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.telephony.*;
import android.text.format.Time;
import android.util.Log;

import com.stericsson.bluetooth.map.BluetoothMapMessageNotificationClient.BluetoothMapEventData;

import java.util.ArrayList;

/* This class provides access to the Sms/Mms ContentProviders */
public class BluetoothMapMessageSender {

    private static final String TAG = "BluetoothMapMessageSender";

    /* This static flags shall be updated in Intent receivers specified in BluetoothMapService class */
    private static boolean mLastMessageSent;
    private static int SmsManagerErrorCode = Activity.RESULT_OK;

    private BluetoothMapMessageAccessor mMessageAccessor;
    private Context mContext;

    /* Session handler for SDP discovery */
    private HandlerThread mHandlerThread;
    private MessageSender mSessionHandler;

    private static final int SEND_SMS_TO_NETWORK = 0x01;

    /* Data for message sender */
    public StringBuilder bMessage = null;
    private int retry;
    private int charset;
    private int transparent;
    private int folderPath;
    private String msg_handle;

    BluetoothMapMessageSender(BluetoothMapMessageAccessor msgAcccessor, Context context) {
        mMessageAccessor = msgAcccessor;
        mContext = context;
        mLastMessageSent = true;

        bMessage = new StringBuilder();
        if (mHandlerThread == null) {
            mHandlerThread = new HandlerThread("BtMap MessageSender",
                    Process.THREAD_PRIORITY_BACKGROUND);
            mHandlerThread.start();
            mSessionHandler = new MessageSender(mHandlerThread.getLooper());
        }
    }

    /* Message Sending thread */

    private class MessageSender extends Handler {
        public MessageSender(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case SEND_SMS_TO_NETWORK:
                performPushMessage(bMessage.toString(), retry, charset, transparent, folderPath, msg_handle);
                break;
            }
        }
    }

    private void sendSMS(String phoneNumber, String message, String msgHandle, int transparent) {
        String SENT = "SMS_SENT";
        String DELIVERED = "SMS_DELIVERED";

        Intent SentIntent = new Intent("SMS_SENT");
        Intent Delivered = new Intent("SMS_DELIVERED");

        SentIntent.putExtra("handle", msgHandle);
        Delivered.putExtra("handle", msgHandle);
        SentIntent.putExtra("transparent", transparent);
        Delivered.putExtra("transparent", transparent);

        PendingIntent sentPI = PendingIntent.getBroadcast(mContext, 0,
                SentIntent, PendingIntent.FLAG_UPDATE_CURRENT );
        PendingIntent deliveredPI = PendingIntent.getBroadcast(mContext, 0,
                Delivered, PendingIntent.FLAG_UPDATE_CURRENT );

        /* When the SMS has been sent to Network */
        mContext.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context rContext, Intent mIntent) {

                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    Log.i("SENT Intent Receiver", "RESULT_OK");
                    /* Set the sent successful flag */
                    BluetoothMapMessageSender.mLastMessageSent = true;
                    BluetoothMapMessageSender.SmsManagerErrorCode = Activity.RESULT_OK;
                    break;
                case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                    Log.i("SENT Intent Receiver", "RESULT_ERROR_GENERIC_FAILURE");
                    /* Set the sent successful flag */
                    BluetoothMapMessageSender.mLastMessageSent = false;
                    BluetoothMapMessageSender.SmsManagerErrorCode = SmsManager.RESULT_ERROR_GENERIC_FAILURE;
                    break;
                case SmsManager.RESULT_ERROR_NO_SERVICE:
                    Log.i("SENT Intent Receiver", "RESULT_ERROR_NO_SERVICE");
                    /* Set the sent successful flag to fail */
                    BluetoothMapMessageSender.mLastMessageSent = false;
                    BluetoothMapMessageSender.SmsManagerErrorCode = SmsManager.RESULT_ERROR_NO_SERVICE;
                    break;
                case SmsManager.RESULT_ERROR_NULL_PDU:
                    Log.i("SENT Intent Receiver", "RESULT_ERROR_NULL_PDU");
                    /* Set the sent successful flag to fail */
                    BluetoothMapMessageSender.mLastMessageSent = false;
                    BluetoothMapMessageSender.SmsManagerErrorCode = SmsManager.RESULT_ERROR_NULL_PDU;
                    break;
                case SmsManager.RESULT_ERROR_RADIO_OFF:
                    Log.i("SENT Intent Receiver", "RESULT_ERROR_RADIO_OFF");
                    /* Set the sent successful flag to fail */
                    BluetoothMapMessageSender.mLastMessageSent = false;
                    BluetoothMapMessageSender.SmsManagerErrorCode = SmsManager.RESULT_ERROR_RADIO_OFF;
                    break;
                }
                /* release this semaphore */
                BluetoothMapService.semEvent.release();
                mContext.unregisterReceiver(this);
            }
        }, new IntentFilter(SENT));

        /* when the SMS has been delivered */
        mContext.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context rContext, Intent mIntent) {

                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    Log.d(TAG, "Sms Delivered");

                    String handle = mIntent.getStringExtra("handle");
                    int transparent = mIntent.getIntExtra("transparent", 0);
                    Log.d(TAG, "transparent mode = " + transparent);
                    if (transparent == 0) {
                        Log.d(TAG, "handle " + handle);
                        BluetoothMapEventData eventData1 = BluetoothMapService.mMessageNotifier.createEventData(handle,
                                "TELECOM/MSG/SENT" , "", "SMS_GSM");
                        BluetoothMapService.mMessageNotifier.sendEvent(Constants.MAP_EVENT_TYPE_DELIVERY_SUCCESS, eventData1, Constants.SMS_MASINSTANCE);
                        mMessageAccessor.setDeliveryStatusOK(handle);
                    }
                    break;
                case Activity.RESULT_CANCELED:
                    Log.d(TAG, "Sms NOT Delivered ");

                    /* Ready for DeliveryFailed */
                    String handle1 = mIntent.getStringExtra("handle");
                    int transparent1 = mIntent.getIntExtra("transparent", 0);
                    Log.d(TAG, "transparent mode = " + transparent1);
                    if (transparent1 == 0) {
                        Log.d(TAG, "handle1 " + handle1);
                        BluetoothMapEventData eventData = BluetoothMapService.mMessageNotifier.createEventData(handle1,
                                "TELECOM/MSG/SENT" , "", "SMS_GSM");
                        BluetoothMapService.mMessageNotifier.sendEvent(Constants.MAP_EVENT_TYPE_DELIVERY_FAILED, eventData, Constants.SMS_MASINSTANCE);
                        mMessageAccessor.setDeliveryStatusFailed(handle1);
                    }
                    break;
                }
                mContext.unregisterReceiver(this);
            }
        }, new IntentFilter(DELIVERED));

        SmsManager sms = SmsManager.getDefault();

        ArrayList<String> parts = sms.divideMessage(message);
        int numParts = parts.size();

        ArrayList<PendingIntent> sentIntents = new ArrayList<PendingIntent>();
        ArrayList<PendingIntent> deliveryIntents = new ArrayList<PendingIntent>();

        for (int i = 0; i < numParts; i++) {
            sentIntents.add(sentPI);
            deliveryIntents.add(deliveredPI);
        }

        sms.sendMultipartTextMessage(phoneNumber, null, parts, sentIntents, deliveryIntents);

        try {
            BluetoothMapService.semEvent.acquire();
        } catch (InterruptedException e) {
            Log.e("BluetoothMapMessageSender", "InterruptedException " + e);
        }
    }

    private String emailAddress;
    private String emailText;
    private String emailHandle;
    private String emailSubject;
    private String emailOriginator;
    private String emailOrigName;
    public boolean performPushMessageInSeperateContext(String bMsg,
                                                        int p_retry,int p_charset,int p_transparent,int p_folderPath) {
        retry = p_retry;
        charset = p_charset;
        transparent = p_transparent;
        folderPath = p_folderPath;

        String [] phoneNum = null;
        String msgBody = null;
        boolean success = true;
        int startInd = 0, endIndex = 0;

         /* check for invalid bMsg */
        if ((bMsg == null) || (bMsg.compareTo("") == 0)) {
            Log.i("performPushMessage", "Message invalid or null");
            success = false;
        } else {
            bMessage.delete(0, bMessage.length());
            bMessage.append(bMsg);
            /* Now make the decision based upon the type of message */
            String type = MapUtils.fetchType(bMsg);

            if (type!=null && (type.equalsIgnoreCase("SMS_GSM")) || (type.equalsIgnoreCase("SMS_CDMA"))) {
                /* Start of SMS sending Block */
                Log.i(TAG, "Start of SMS sending Block");

                /* Parse Phone Number and Sms text */
                phoneNum = getRecipientAddressesFrombMsg(bMsg);

                startInd = bMsg.indexOf("BEGIN:MSG\r\n") + "BEGIN:MSG\r\n".length();
                endIndex = bMsg.indexOf("\r\nEND:MSG");
                msgBody = "";
                for (int i= startInd; i < endIndex; i++) {
                    msgBody = msgBody + bMsg.charAt(i);
                }

                /**
                 * Important decision making block
                 * This block shall the sms in the specified
                 * folder if the specified folder is not outbox.
                 * If the specified folder is  outbox, it shall try to send
                 * the sms, and shall save the sms with proper status and in proper
                 * folder depending upon the sending result
                 */
                if (folderPath == Constants.MAP_FOLDER_OUTBOX) {

                    Log.i("performPushMessage", "The extracted sms text is :: " + msgBody);

                    String msgHandle = "";

                    /**
                     * save this message to outbox and then shift to whatever
                     * folder needed depending upon the kind of the
                     * response retrieved in the sending process.
                     */
                    msgHandle = mMessageAccessor.saveSmsToOutboxFolder(phoneNum, msgBody);
                    msg_handle = msgHandle;

                    mSessionHandler.obtainMessage(SEND_SMS_TO_NETWORK, -1, -1, -1).sendToTarget();
                } else {
                    /* Do not try to send the SMS just save to the specified folder */
                    mMessageAccessor.saveSmsToSpecifiedFolder(phoneNum, msgBody, folderPath);
                }
            } else if (type!=null && (type.equalsIgnoreCase("EMAIL"))) {

                Log.i(TAG, "Start of EMAIL sending Block: " + bMsg);
                Cursor cursorMailBox = null;
                String orderby = "_id desc";
                int accountKey = 0;
                try {

                    cursorMailBox = mContext.getContentResolver().query(
                            Uri.parse(Constants.EMAIL_URI_MAILBOX),
                            null, null, null, orderby);
                    if( cursorMailBox.getCount() > 0) {
                        cursorMailBox.moveToFirst();
                        accountKey = cursorMailBox.getInt(cursorMailBox.getColumnIndex("accountKey"));
                    }
                } finally {
                    if(cursorMailBox != null)
                        cursorMailBox.close();
                }
                Cursor cursorMailBoxUpdate = null;
                String box = null;
                String whereClause = "UPPER(serverId) = 'OUTBOX'";
                try {
                    cursorMailBoxUpdate = mContext.getContentResolver().query(
                            Uri.parse(Constants.EMAIL_URI_MAILBOX),
                            null, whereClause, null, null);
                    if (cursorMailBoxUpdate.getCount() <= 0 ) {
                        ContentValues values = new ContentValues();
                        values.put("displayName", "Outbox");
                        values.put("serverId", "Outbox");
                        values.put("parentServerId", "");
                        values.put("parentKey", "-1");
                        values.put("accountKey", accountKey);
                        values.put("syncKey", "");
                        values.put("syncLookback", 0);
                        values.put("syncInterval", 0);
                        values.put("syncTime", 0);
                        values.put("unreadCount", 1);
                        values.put("flagVisible", 1);
                        values.put("delimiter", 47);
                        values.put("type", 4);
                        Uri tempUri = mContext.getContentResolver().insert( Uri.parse(Constants.EMAIL_URI_MAILBOX), values);
                    }
                } finally {
                    if (cursorMailBoxUpdate != null)
                        cursorMailBoxUpdate.close();
                }

                Bmessage bMsgEmail = MapUtils.fromBmessageEmail(bMsg);
                emailAddress = bMsgEmail.getRecipientVcard_email();
                emailText = bMsgEmail.getBody_msg();
                emailSubject = bMsgEmail.getSubject();
                emailOriginator = fetchOriginator();
                emailOrigName = bMsgEmail.getOriginatorVcard_name();

                String tmpPath = "";
                tmpPath = BluetoothMapFolderAccessor.mapGivenFolderToStringWithoutLeadingBar(folderPath);

                if (!tmpPath.equalsIgnoreCase("telecom/msg/sent")) {
                    String splitStrings[] = tmpPath.split("/");

                    int tmp = splitStrings.length;
                    String folderName;

                    folderName = splitStrings[tmp - 1];

                    emailHandle = addToEmailFolder(folderName, emailAddress, emailText, emailSubject,
                            emailOriginator, emailOrigName);

                    mMessageAccessor.setLastMessageHandleCreated(emailHandle);

                    long accountId = getEmailAccountId(emailOriginator);
                    Log.d(TAG, " Account id before Mail service:: "+accountId);

                    Intent emailIn = new Intent();

                    emailIn.setAction("com.android.email.intent.action.MAIL_SERVICE_WAKEUP");
                    emailIn.putExtra("com.android.email.intent.extra.ACCOUNT", accountId);
                    this.mContext.startService(emailIn);

                    Intent intent = new Intent(Constants.ACTION_UPDATE_OUTBOX);
                    long[] mailboxDetail = new long[2];
                    mailboxDetail[0] = getAccountID();
                    mailboxDetail[1] = getMailboxID("'Outbox'");
                    intent.putExtra("android.content.Intent.EXTRA_TEXT", mailboxDetail);
                    mContext.sendBroadcast(intent);
                }
            }
        }
        return success;
    }

    public long getMailboxID(String folder) {
        Uri mailboxtableEmailUri = Uri.parse(Constants.EMAIL_URI_MAILBOX);
        Cursor CursormailboxtableEmail = null;
        String whereClause1 = "displayName = "+ folder;
        CursormailboxtableEmail = mContext.getContentResolver().query(mailboxtableEmailUri, null, whereClause1,null,null);
        if(CursormailboxtableEmail != null){
                CursormailboxtableEmail.moveToFirst();
                int Columnindex = CursormailboxtableEmail.getColumnIndex("_id");
                Log.i(TAG, "Columnindex"+ Columnindex);
                String mailboxID = CursormailboxtableEmail.getString(Columnindex);
                Log.i(TAG, "mailboxID"+mailboxID);
                return Integer.parseInt(mailboxID);
        } else {
            Log.i(TAG,"CursormailboxtableEmail is NULL ");
            return 0;
        }
    }

    public long getAccountID() {
        Uri accounttableEmailUri = Uri.parse(Constants.EMAIL_URI_ACCOUNT);
        Cursor CursoraccounttableEmail = null;
        CursoraccounttableEmail = mContext.getContentResolver().query(accounttableEmailUri, null, null,null,"_id desc");
        if(CursoraccounttableEmail != null){
                CursoraccounttableEmail.moveToFirst();
                int Columnindex = CursoraccounttableEmail.getColumnIndex("_id");
                Log.i(TAG, "Columnindex"+ Columnindex);
                String AccountID = CursoraccounttableEmail.getString(Columnindex);
                Log.i(TAG, "AccountID"+AccountID);
                return Integer.parseInt(AccountID);
        } else {
            Log.i(TAG,"CursoraccounttableEmail is NULL");
            return 0;
        }
    }

    private String fetchOriginator() {

        Cursor cr1 = null;
        String originator = null;
        String whereClause1 = null;
        Log.i(TAG,"whereClause1 is " + whereClause1);
        try {
            cr1 = mContext.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI_ACCOUNT),
                    null, null, null, null);
            if (cr1.getCount() > 0) {
                cr1.moveToFirst();
                originator = cr1.getString(cr1.getColumnIndex("emailAddress"));
            }
        } finally {
            if (cr1 != null) {
                cr1.close();
            }
        }

        Log.i(TAG,"originator is " + originator);
        return originator;

    }
    /**
     * This method performs push message function along with
     * applying the supplied parameters in the PushMessage
     * function.
     *
     * @param bMsg
     * @param retry
     * @param charset
     * @param transparent
     * @return boolean
     */
    public boolean performPushMessage(String bMsg, int retry, int charset, int transparent, int folderPath, String msgHandle) {
        String [] phoneNum = null;
        String msgBody = null;
        boolean success = true;
        int startInd = 0, endIndex = 0;

        /* check for invalid bMsg */
        if ((bMsg == null) || (bMsg.compareTo("") == 0)) {
            Log.i("performPushMessage", "Message invalid or null");
            success = false;
        } else {
            /* Start of SMS sending Block */
            Log.i(TAG, "Start of SMS sending Block");

            /* parse Phone Number and Sms text */
            phoneNum = getRecipientAddressesFrombMsg(bMsg);

            startInd = bMsg.indexOf("BEGIN:MSG\r\n") + "BEGIN:MSG\r\n".length();
            endIndex = bMsg.indexOf("\r\nEND:MSG");
            msgBody = "";
            for (int i= startInd; i < endIndex; i++) {
                msgBody = msgBody + bMsg.charAt(i);
            }

            /* Important decision making block
             * This block shall the sms in the specified
             * folder if the specified folder is not outbox.
             * If the specified folder is  outbox, it shall try to send
             * the sms, and shall save the sms with proper status and in proper
             * folder depending upon the sending result */
            if (folderPath == Constants.MAP_FOLDER_OUTBOX) {

                Log.i("performPushMessage", "The extracted sms text is :: " + msgBody);

                /* Send the text sms to each of the recipient in a loop */
                for (int i = 0; i < phoneNum.length; i ++) {
                    try {
                        sendSMS(phoneNum[i], msgBody, msgHandle, transparent);
                    }
                    catch(Exception e) {
                        Log.e(TAG, "Caught exception " + e);
                    }
                }

                /* Recfactored code to take care of transparent mode and sending status */
                if (mLastMessageSent == false) {
                    /**
                     * If sending failed save the message to the folder
                     *  specified by last set path request, this is outbox by default
                     *  since sending could've been only initiated if the specified folder
                     *  was outbox */
                    switch (SmsManagerErrorCode) {
                    case SmsManager.RESULT_ERROR_NO_SERVICE:
                    case SmsManager.RESULT_ERROR_RADIO_OFF:
                        Log.i("performPushMessage", "Save Sms with Pending state");
                        mMessageAccessor.moveToPendingFolder(msgHandle);
                        break;
                    case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                    case SmsManager.RESULT_ERROR_NULL_PDU:
                        Log.i("performPushMessage", "Save Sms with FAILED state");
                        mMessageAccessor.moveToFailedFolder(msgHandle);
                        break;
                    default:
                        mMessageAccessor.moveToFailedFolder(msgHandle);
                        break;
                    }
                } else if (transparent == Constants.PARAM_TRANSPARENT_OFF) {
                    /* SENT has been successful, save this message to sent folder, since transparent mode is off */
                    Log.i("performPushMessage", "SENT Successful : Save Sms to SENT Folder");
                    mMessageAccessor.moveToSentFolder(msgHandle);
                } else {
                    /**
                     * Sending has been successful, but since transparent mode is
                     * on, delete this message, it was needed to be stored in database
                     * only until it was not sent successfully */
                    mMessageAccessor.deleteSms(msgHandle);
                }
                /* Reset the sent folder flag to false */
                mLastMessageSent = false;
            }
            else {
                /* Do not try to send the SMS just save to the specified folder */
                mMessageAccessor.saveSmsToSpecifiedFolder(phoneNum, msgBody, folderPath);
            }
        }
        return success;
    }

    /**
     * Adds an Email to the Email ContentProvider
     */

    public String addToEmailFolder(String folder, String address, String text, String subject, String OrigEmail, String OrigName) {

        Log.d(TAG, "address " + address);
        Log.d(TAG, "TEXT " + text);

        if (folder.compareToIgnoreCase("draft") == 0) {
            folder = "Drafts";
        }

        Cursor cr = null;
        int folderId = 0;
        long accountId = 0;
        Time timeObj = new Time();
        timeObj.setToNow();

        String whereClause = "UPPER(serverId) LIKE '"+folder.toUpperCase().trim()+"%'";

        try {
            cr = mContext.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI_MAILBOX),
                    null, whereClause, null, null);
            if (cr.getCount() > 0) {
                cr.moveToFirst();
                folderId = cr.getInt(cr.getColumnIndex("_id"));
                if (folder.equalsIgnoreCase("outbox")) {
                    ContentValues values = new ContentValues();
                    values.put("syncInterval", "1");
                    mContext.getContentResolver().update(
                            Uri.parse(Constants.EMAIL_URI_MAILBOX+"/"+folderId), values,null,null);
                }
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }

        accountId = getEmailAccountId(OrigEmail);

        Log.d(TAG, "To address " + address);
        Log.d(TAG, "Text " + text);
        Log.d(TAG, "Originator email address:: " + OrigEmail);
        Log.d(TAG, "Originator email name:: " + OrigName);
        Log.d(TAG, "Time Stamp:: " + timeObj.toMillis(false));
        Log.d(TAG, "Account Key:: " + accountId);
        Log.d(TAG, "Folder Id:: " + folderId);
        Log.d(TAG, "Folder Name:: " + folder);
        String subject1 = subject;
        if (subject == null) {
            subject1 = "";
        }
        Log.d(TAG, "Subject" + subject1);

        ContentValues values = new ContentValues();
        /* Hardcoded values */
        values.put("syncServerId", "1");
        /* end Hardcoded values */
        values.put("syncServerTimeStamp", timeObj.toMillis(false));
        values.put("displayName", OrigName.trim());
        values.put("timeStamp", timeObj.toMillis(false));
        values.put("subject", subject1.trim());
        values.put("flagLoaded", "1");
        values.put("flagFavorite", "0");
        values.put("flagAttachment", "0");
        values.put("flags", "0");

        values.put("accountKey", accountId);
        values.put("fromList", OrigEmail.trim());

        values.put("mailboxKey", folderId);
        values.put("toList", address.trim());
        values.put("flagRead", 0);

        Uri uri = mContext.getContentResolver().insert(
                Uri.parse(Constants.EMAIL_URI), values);

        String str = uri.toString();
        String[] splitStr = str.split("/");

        ContentValues valuesBody = new ContentValues();
        valuesBody.put("messageKey", splitStr[4]);
        valuesBody.put("textContent", text);

        mContext.getContentResolver().insert(
                Uri.parse(Constants.EMAIL_URI_BODY), valuesBody);

        return splitStr[4];

    }

    public long getEmailAccountId(String email) {
        Cursor cr1 = null;
        long accountId = -1;

        try {
            cr1 = mContext.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI_ACCOUNT),
                    null, null, null, null);
            if (cr1.getCount() > 0) {
                cr1.moveToFirst();
                accountId = cr1.getInt(cr1.getColumnIndex("_id"));
            }
        } finally {
            if (cr1 != null) {
                cr1.close();
            }
        }
        return accountId;
    }

    /**
     * This method retrieves all the telephone numbers specified in the
     * bMsg object, considering the possibility of multiple recipients
     * of a single text message.
     *
     * @param bMsg
     * @return
     */
    private String [] getRecipientAddressesFrombMsg(String bMsg) {
        int startInd = 0, endIndex = 0;
        int count = 0, len = bMsg.length();
        String [] phoneNum = null;

        Log.i("getRecipientAddressesFrombMsg", " Entering");
        while (startInd < len) {
            if ((bMsg.indexOf("TEL:", startInd) + 1) < startInd) {
                break;
            }
            startInd = bMsg.indexOf("BEGIN:BENV", startInd) + "BEGIN:BENV".length();
            startInd = bMsg.indexOf("TEL:", startInd) + "TEL:".length();
            if (startInd < len)
                count++;
        }

        phoneNum = new String[count];
        startInd = 0;
        for (int i = 0; i < count; i ++) {
            startInd = bMsg.indexOf("BEGIN:BENV", startInd) + "BEGIN:BENV".length();
            startInd = bMsg.indexOf("TEL:", startInd) + "TEL:".length();
            endIndex = bMsg.indexOf("END:VCARD", startInd);
            phoneNum[i] = "";
            Log.i("getRecipientAddressesFrombMsg", "Indexes " + startInd +" " + endIndex);
            for (int j= startInd; j < endIndex; j++) {

                /* Eliminate + or - symbols in the phone number */
                if ((bMsg.charAt(j) == '+') || (bMsg.charAt(j) == '-')) {
                    phoneNum[i] = phoneNum[i] + bMsg.charAt(j);
                } else if ((bMsg.charAt(j) > '9') || (bMsg.charAt(j) < '0')) {
                    /* End the loop if any other non-numeric character is found */
                    j = endIndex;
                } else {
                    phoneNum[i] = phoneNum[i] + bMsg.charAt(j);
                }
            }
        }

        return phoneNum;
    }

    /**
     * Adds a SMS to the Sms ContentProvider
     */

    public String addToSmsFolder(String folder, String address, String text) {

        int threadId = getThreadId(address);
        Log.d(TAG, "address " + address + " TEXT " + text + " Thread ID "
                + threadId);

        ContentValues values = new ContentValues();
        values.put("thread_id", threadId);
        values.put("body", text);
        values.put("address", address);
        values.put("read", 0);
        values.put("seen", 0);
        /*
         * status none -1 complete 0 pending 64 failed 128
         */
        values.put("status", -1);
        /*
         * outbox 4 queued 6
         */
        values.put("locked", 0);
        values.put("error_code", 0);
        Uri uri = mContext.getContentResolver().insert(
                Uri.parse(Constants.SMS_URI + folder), values);

        String str = uri.toString();
        String[] splitStr = str.split("/");
        Log.d(TAG, " NEW HANDLE " + splitStr[3]);
        return splitStr[3];
    }

    /**
     * Retrieve the conversation thread id
     */
    private int getThreadId(String address) {

        Cursor cr = mContext.getContentResolver().query(
                Uri.parse(Constants.SMS_URI), null,
                "address = '" + address + "'", null, null);
        if (cr.moveToFirst()) {
            int threadId = Integer.valueOf(cr.getString(cr
                    .getColumnIndex("thread_id")));

            return (threadId);
        }
        return 0;
    }
}
