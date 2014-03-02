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

import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager.WakeLock;
import android.os.PowerManager;
import android.util.Log;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import javax.obex.ApplicationParameter;
import javax.obex.HeaderSet;
import javax.obex.ObexTransport;
import javax.obex.Operation;
import javax.obex.ResponseCodes;
import javax.obex.ServerRequestHandler;
import javax.obex.ServerSession;

/**
 * This class runs as an OBEX server
 */
public class BluetoothMapObexServerSession extends ServerRequestHandler {

    private static final String TAG = "BluetoothMapObexServerSession";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private ObexTransport mTransport;

    private Context mContext;

    private Handler mCallback = null;

    private ServerSession mSession;

    private WakeLock mWakeLock;

    private WakeLock mPartialWakeLock;

    private String mPathString = Constants.MAP_ROOT_FOLDER_DIR;

    private String mCurrentPath = Constants.MAP_ROOT_FOLDER_DIR;

    /**
     * mCurrentPathState contains current path in terms of constant
     * discrete paths defined as int values. Rather than having to
     * maintain String format paths for comparison purposes.
     */
    private int mCurrentPathState;

    /**
     * Map Obex Session's Message Folder Accessor
     */
    private BluetoothMapFolderAccessor mFolderAccessor;

    /**
     * Map Obex Session's Message Accessor
     */
    public BluetoothMapMessageAccessor mMessageAccessor;

    /**
     * MAP MessageSender Instance
     */
    private BluetoothMapMessageSender mMessageSender;

    private static final boolean PARSE_OK = true;

    private static final boolean PARSE_NOK = false;

    private static final String MSGLIST_FILENAME = "msglist";

    private int localMasInstance;

    public BluetoothMapObexServerSession(Context context, ObexTransport transport, int masInstance, Handler callback) {
        mContext = context;
        mTransport = transport;
        PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP
                | PowerManager.ON_AFTER_RELEASE, TAG);
        mPartialWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
        mCurrentPathState = Constants.MAP_FOLDER_ROOT;
        mPathString = Constants.MAP_ROOT_FOLDER_DIR;
        mFolderAccessor = new BluetoothMapFolderAccessor(context);
        mMessageAccessor = new BluetoothMapMessageAccessor(context, this, masInstance);
        mMessageSender = new BluetoothMapMessageSender(mMessageAccessor, context);
        localMasInstance = masInstance;
        mCallback = callback;
        Log.i(TAG, "BluetoothMapObexServerSession -> EMAIL=1, SMS=0 Local Instance" + masInstance);
    }

    public void unblock() {

    }

    /**
     * Called when connection is accepted from remote, to retrieve the first Header then wait for
     * user confirmation
     */
    public void preStart() {
        if (D) Log.d(TAG, "connection accepted from remote");
        mWakeLock.acquire();
        try {
            if (D) Log.d(TAG, "Create ServerSession with transport " + mTransport.toString());
            mSession = new ServerSession(mTransport, this, null);
        } catch (IOException e) {
            Log.e(TAG, "Create server session error" + e);
        }
    }

    /**
     * Start MapObexServerSession
     */
    public void start(Handler handler) {
        if (D) Log.d(TAG, "Start!");
    }

    /**
     * Called from BluetoothMapService to close the connection
     */
    public void stop() {
        if (D) Log.d(TAG, "Stop!");
        if (mSession != null) {
            try {
                mSession.close();
            } catch (Exception e) {
                Log.e(TAG, "close mSession error" + e);
            }
        }
        if (mTransport != null) {
            try {
                mTransport.close();
            } catch (Exception e) {
                Log.e(TAG, "close mTransport error" + e);
            }
        }
        mCallback = null;
        mSession = null;
    }

    /**
     * Converts the folder paths present from enumerable
     * defined path state to the String format.
     *
     * @return String
     */
    private String mapCurrentFolderToString() {
        String returnString;

        switch (this.mCurrentPathState) {
        case Constants.MAP_FOLDER_ROOT:
            /* Use empty string for root */
            returnString = "";
            break;
        case Constants.MAP_FOLDER_TELECOM:
            returnString = "/telecom";
            break;
        case Constants.MAP_FOLDER_MSG:
            returnString = "/telecom/msg";
            break;
        case Constants.MAP_FOLDER_INBOX:
            returnString = "/telecom/msg/inbox";
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            returnString = "/telecom/msg/outbox";
            break;
        case Constants.MAP_FOLDER_SENT:
            returnString = "/telecom/msg/sent";
            break;
        case Constants.MAP_FOLDER_DELETED:
            returnString = "/telecom/msg/deleted";
            break;
        case Constants.MAP_FOLDER_DRAFT:
            returnString = "/telecom/msg/draft";
            break;
        default:
            returnString = "";
        }

        return returnString;
    }

    /**
     * This method facilitate changing the state of the current
     * folder to a folder location one level up. It prohibits going
     * one level up for a path already indicating the root folder.
     *
     * @param paramCurrentPathState
     * @return int - New folder path state
     */
    private int changeFolderOneLevelUp(int paramCurrentPathState) {
        int returnPathState;

        switch (paramCurrentPathState) {
        case Constants.MAP_FOLDER_TELECOM:
        case Constants.MAP_FOLDER_ROOT:
            returnPathState = Constants.MAP_FOLDER_ROOT;
            break;
        case Constants.MAP_FOLDER_INBOX:
        case Constants.MAP_FOLDER_OUTBOX:
        case Constants.MAP_FOLDER_SENT:
        case Constants.MAP_FOLDER_DELETED:
        case Constants.MAP_FOLDER_DRAFT:
            returnPathState = Constants.MAP_FOLDER_MSG;
            break;
        case Constants.MAP_FOLDER_MSG:
            returnPathState = Constants.MAP_FOLDER_TELECOM;
            break;
        default:
            returnPathState = Constants.MAP_INVALID_PATH_REQ;
            break;
        }

        return returnPathState;
    }

    /**
     * This method parses the application parameter header for the
     * Get folder request.
     * @param appliParams
     * @param appParamValues
     */
    private void parseFolderListingApplicationParameters(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0;
        int lowerByte, higherByte;

        if (appliParams != null) {
            Log.i(TAG, "AppliParams length" + appliParams.length);

            while(i < appliParams.length) {
                switch (appliParams[i]) {
                case Constants.APPLI_PARAM_ID_MAX_LIST_COUNT:
                {
                    higherByte = appliParams[i + 2] & 0xff;
                    lowerByte = appliParams[i + 3] & 0xff;
                    appParamValues.maxListCount = higherByte * 256 + lowerByte;
                    i +=4;
                    break;
                }
                case Constants.APPLI_PARAM_ID_LIST_START_OFFSET:
                {
                    higherByte = appliParams[i + 2] & 0xff;
                    lowerByte = appliParams[i + 3] & 0xff;
                    appParamValues.listStartOffset = higherByte * 256 + lowerByte;
                    i +=4;
                    break;
                }
                }
            }
        }
    }

    private void dumpByteaArray(String header, byte[] array) {
        Log.i(TAG, "Header" + header + " Dumping");
        for (int i = 0; i < array.length;  i++) {
            Log.i(header, i+ "th byte : " + array[i]);
        }
    }

    /**
     * This method parses the Get Message Listing application parameter header
     * @param appliParams
     * @param appParamValues
     */
    private void parseGetMessageListingApplicationParams(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0;
        int lowerByte = 0, higherByte = 0, lengthByte;
        char [] charSeqParameter;

        if (appliParams == null ) {
            Log.i(TAG, "AppliParams field : Non Existent");
            return;
        }

        Log.i(TAG, "AppliParams length" + appliParams.length);

        dumpByteaArray("Application Parameter", appliParams);

        while(i < appliParams.length) {
            /* Initialize each iteration */
            lowerByte = 0;
            higherByte = 0;
            Log.d(TAG, "Value of i is " + i);
            Log.d(TAG, "Application Params byte is " + appliParams[i]);

            switch (appliParams[i]) {
            case Constants.APPLI_PARAM_ID_MAX_LIST_COUNT:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == 0x02) {
                    higherByte = appliParams[i] & 0xff;
                    lowerByte = appliParams[i + 1] & 0xff;
                }
                appParamValues.maxListCount = higherByte * 256 + lowerByte;
                Log.i(TAG, "APPLI_PARAM_ID_MAX_LIST_COUNT = " + appParamValues.maxListCount);
                i += Constants.MAX_LIST_COUNT_FIELD_LENGTH;
                break;

            case Constants.APPLI_PARAM_ID_LIST_START_OFFSET:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == 0x02) {
                    higherByte = appliParams[i] & 0xff;
                    lowerByte = appliParams[i + 1] & 0xff;
                }
                appParamValues.listStartOffset = higherByte * 256 + lowerByte;
                Log.i(TAG, "APPLI_PARAM_ID_LIST_START_OFFSET = " + appParamValues.listStartOffset);
                i += Constants.LIST_START_OFFSET_FIELD_LENGTH;
                break;

            case Constants.APPLI_PARAM_ID_SUBJECT_LEGTH:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == Constants.APP_ID_SUBJECT_LENGTH_FIELD_LENGTH) {
                    lowerByte = appliParams[i] & 0xff;
                }
                appParamValues.subjectLength = lowerByte;
                Log.i(TAG, "APPLI_PARAM_ID_SUBJECT_LEGTH = " + appParamValues.subjectLength);
                i += Constants.APP_ID_SUBJECT_LENGTH_FIELD_LENGTH;
                break;

            case Constants.APPLI_PARAM_ID_PARAMETER_MASK:
                i ++;
                lengthByte = appliParams[i];
                i ++;

                /* Ignore the bits 16-31 as they are reserved for future use */
                if (lengthByte == Constants.PARAMETER_MASK_FIELD_LENGTH) {
                    higherByte = appliParams[i + 2] & 0xff;
                    lowerByte = appliParams[i + 3] & 0xff;
                }
                appParamValues.parameterMask = higherByte * 256 + lowerByte;
                Log.i(TAG, "APPLI_PARAM_ID_PARAMETER_MASK = " + appParamValues.parameterMask);
                i += Constants.PARAMETER_MASK_FIELD_LENGTH;
                break;

            case Constants.APPLI_PARAM_ID_FILTER_MSG_TYPE:
                i ++;
                lengthByte = appliParams[i];
                i ++;

                if (lengthByte == Constants.FILTER_MESSAGE_TYPE_FIELD_LENGTH) {
                    lowerByte = appliParams[i] & 0xff;
                }
                appParamValues.filterMessageType = lowerByte;
                Log.i(TAG, "APPLI_PARAM_ID_FILTER_MSG_TYPE = " + appParamValues.filterMessageType);
                i += Constants.FILTER_MESSAGE_TYPE_FIELD_LENGTH;
                break;

            case Constants.APPLI_PARAM_ID_FILTER_PERIOD_BEGIN:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte !=0 ) {
                    /* Read the entire string character by character */
                    charSeqParameter = new char[lengthByte];

                    for (int j = 0,k=i; j < lengthByte; j++,k++) {
                        charSeqParameter[j] = (char)appliParams[k];
                    }

                    appParamValues.filterPeriodBegin = new String(charSeqParameter);

                    Log.i(TAG, "APPLI_PARAM_ID_FILTER_PERIOD_BEGIN = " + appParamValues.filterPeriodBegin);
                    i += lengthByte;
                }
                break;

            case Constants.APPLI_PARAM_ID_FILTER_PERIOD_END:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte !=0 ) {
                    /* Read the entire string character by character */
                    charSeqParameter = new char[lengthByte];
                    for (int j = 0,k=i ; j < lengthByte; j++,k++) {
                        charSeqParameter[j] = (char)appliParams[k];
                    }

                    appParamValues.filterPeriodEnd  = new String(charSeqParameter);
                    Log.i(TAG, "APPLI_PARAM_ID_FILTER_PERIOD_END = " + appParamValues.filterPeriodEnd);
                    i += lengthByte;
                }
                break;

            case Constants.APPLI_PARAM_ID_FILTER_READ_STATUS:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == Constants.FILTER_READ_STATUS_FIELD_LENGTH) {
                    appParamValues.filterReadStatus = appliParams[i];
                }
                i += Constants.FILTER_READ_STATUS_FIELD_LENGTH;
                Log.i(TAG, "APPLI_PARAM_ID_FILTER_READ_STATUS = " + appParamValues.filterReadStatus);
                break;

            case Constants.APPLI_PARAM_ID_FILETER_RECIPIENT:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte !=0 ) {
                    /* Read the entire string character by character */
                    charSeqParameter = new char[lengthByte];
                    for (int j = 0,k=i; j < lengthByte ; j++,k++) {

                        charSeqParameter[j] = (char)appliParams[k];
                    }

                    appParamValues.filterRecipient = new String(charSeqParameter);

                    Log.i(TAG, "APPLI_PARAM_ID_FILETER_RECIPIENT = " + appParamValues.filterRecipient);
                    i += lengthByte;
                }
                break;

            case Constants.APPLI_PARAM_ID_FILTER_ORIGINATOR:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte !=0 ) {
                    /* Read the entire string character by character */
                    charSeqParameter = new char[lengthByte];
                    for (int j = 0,k=i; j < lengthByte; j++,k++) {

                        charSeqParameter[j] = (char)appliParams[k];
                    }

                    appParamValues.filterOriginator = new String(charSeqParameter);
                    Log.i(TAG, "APPLI_PARAM_ID_FILTER_ORIGINATOR = " + appParamValues.filterOriginator);
                    i += lengthByte;
                }
                break;

            case Constants.APPLI_PARAM_ID_FILTER_PRIORITY:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == Constants.FILTER_PRIORITY_FIELD_LENGTH) {
                    appParamValues.filterPriority = appliParams[i];
                    Log.i(TAG, "APPLI_PARAM_ID_FILTER_PRIORITY = " + appParamValues.filterPriority);
                }
                i += Constants.FILTER_PRIORITY_FIELD_LENGTH;
                break;
            default :
                Log.i(TAG, "parseGetMessageListingApplicationParams : Entering default part");
                i ++;
                lengthByte = appliParams[i];
                i += lengthByte;
            }
        }
    }

    /**
     * This method parses the Get Message application parameter header
     * @param appliParams
     * @param appParamValues
     */
    private boolean parseGetMessageApplicationParams(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0, lengthByte = 0;

        if (appliParams != null) {
            Log.i(TAG, "parseApplicationParameters" + appliParams);

            for (int j = 0; j < appliParams.length; j ++) {
                Log.i(TAG, "appliParams[" + j + "]" + appliParams[j]);
            }

            while(i < appliParams.length) {
                /* Initialize each iteration */
                switch (appliParams[i]) {
                case Constants.APPLI_PARAM_ID_ATTACHMENT:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.ATTACHMENT_FIELD_LENGTH) {
                        appParamValues.attachment = appliParams[i];
                    }
                    i += Constants.ATTACHMENT_FIELD_LENGTH;
                    break;

                case Constants.APPLI_PARAM_ID_CHARSET:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.CHARSET_FIELD_LENGTH) {
                        appParamValues.charSet = appliParams[i];
                    }
                    i += Constants.CHARSET_FIELD_LENGTH;
                    break;

                case Constants.APPLI_PARAM_ID_FRACTION_REQUEST:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.FRACTION_REQUEST_FIELD_LENGTH) {
                        appParamValues.fractionRequest = appliParams[i];
                        appParamValues.isFractionRequestPresent = true;
                    }
                    i += Constants.FRACTION_REQUEST_FIELD_LENGTH;
                    break;
                default :
                    Log.i(TAG, "parseGetMessageApplicationParams : Entering default part");
                    i ++;
                    lengthByte = appliParams[i];
                    i += lengthByte;
                }
            }
        } else {
            Log.e(TAG, "Application parameters not specified");
            return PARSE_NOK;
        }

        return PARSE_OK;
    }

    /**
     * This method parses the Push Message application parameter header
     * @param appliParams
     * @param appParamValues
     */
    private boolean parsePushMessageApplicationParams(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0, lengthByte = 0;
        boolean retVal = PARSE_OK;

        if (appliParams != null) {
            Log.i(TAG, "parseApplicationParameters" + appliParams);

            for (int j = 0; j < appliParams.length; j ++) {
                Log.i(TAG, "appliParams[" + j + "]" + appliParams[j]);
            }

            while(i < appliParams.length) {
                /* Initialize each iteration */
                switch (appliParams[i]) {
                case Constants.APPLI_PARAM_ID_TRANSPARENT:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.TRANSPARENT_FIELD_LENGTH) {
                        appParamValues.transparent = appliParams[i] & 0xff;
                    }
                    i += Constants.TRANSPARENT_FIELD_LENGTH;
                    break;

                case Constants.APPLI_PARAM_ID_CHARSET:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.CHARSET_FIELD_LENGTH) {
                        appParamValues.charSet = appliParams[i] & 0xff;
                        appParamValues.isCharSetPresent = true;
                    }
                    i += Constants.CHARSET_FIELD_LENGTH;
                    break;

                case Constants.APPLI_PARAM_ID_RETRY:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.RETRY_FIELD_LENGTH) {
                        appParamValues.retry = appliParams[i] & 0xff;
                    }
                    i += Constants.RETRY_FIELD_LENGTH;
                    break;
                default :
                    Log.i(TAG, "parsePushMessageApplicationParams : Entering default part");
                    i ++;
                    lengthByte = appliParams[i];
                    i += lengthByte;
                }
            }
        } else {
            Log.e(TAG, "Application parameters not specified");
            return PARSE_NOK;
        }
        if (appParamValues.isCharSetPresent == false) {
            retVal = false;
        }
        return retVal;
    }

    /**
     * This method parses the setMessageStatus application parameter header
     *
     * @param appliParams
     * @param appParamValues
     * @return true if parse has been okay
     *            false if parse has not succeeded.
     */
    private boolean parsesetMessageStatusApplicationParams(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0, lengthByte = 0;
        boolean isStatusValPresent = false, isStatusIndPresent = false;

         if (appliParams != null) {
             Log.i(TAG, "parseApplicationParameters" + appliParams);

            while(i < appliParams.length) {
                /* Initialize each iteration */
                switch (appliParams[i]) {
                case Constants.APPLI_PARAM_ID_STATUS_INDICATOR:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.FIELD_LENGTH_ONE_BYTE) {
                        appParamValues.statusIndicator = appliParams[i] & 0xff;
                    }
                    i += Constants.FIELD_LENGTH_ONE_BYTE;
                    isStatusValPresent = true;
                    if (appParamValues.statusIndicator == 1)
                        BluetoothMapApplicationParameters.delNotification = false;
                    break;

                case Constants.APPLI_PARAM_ID_STATUS_VALUE:
                    i ++;
                    lengthByte = appliParams[i];
                    i ++;
                    if (lengthByte == Constants.FIELD_LENGTH_ONE_BYTE) {
                        appParamValues.statusValue = appliParams[i] & 0xff;
                    }
                    i += Constants.FIELD_LENGTH_ONE_BYTE;
                    isStatusIndPresent  = true;
                    break;
                default :
                    Log.i(TAG, "parsesetMessageStatusApplicationParams : Entering default part");
                    i ++;
                    lengthByte = appliParams[i];
                    i += lengthByte;
                }
            }
        } else {
            Log.e(TAG, "Application parameters not specified");
            return PARSE_NOK;
        }

        return (isStatusIndPresent && isStatusValPresent);
    }

    private boolean parseSetNotificationRegApplicationParams(byte [] appliParams, BluetoothMapApplicationParameters appParamValues) {
        int i = 0, lengthByte = 0;
        boolean isStatusValPresent = false;

        if (appliParams != null) {
            Log.i(TAG, "parseApplicationParameters" + appliParams);

            /* Initialize each iteration */
            switch (appliParams[i]) {
            case Constants.APPLI_PARAM_ID_NOTIFICATION_STATUS:
                i ++;
                lengthByte = appliParams[i];
                i ++;
                if (lengthByte == Constants.FIELD_LENGTH_ONE_BYTE) {
                    appParamValues.notificationStatus = appliParams[i] & 0xff;
                }
                i += Constants.FIELD_LENGTH_ONE_BYTE;
                isStatusValPresent = true;
                break;
            }
        } else {
            Log.i(TAG, "Application parameters not specified");
            return PARSE_NOK;
        }

        return isStatusValPresent;
    }

    /**
     * This method reads message body from input stream
     * contained in the body/end_of_body headers.
     * @param op
     * @return String msgBody
     */
    String receivebMessageBody(Operation op) {
        String msgBody = null;
        int responseCode = ResponseCodes.OBEX_HTTP_OK;
        InputStream iStream = null;
        Long length = (long) 0;
        HeaderSet request = null;

        try {
            iStream = op.openInputStream();
            request = op.getReceivedHeader();
            length = (Long) request.getHeader(HeaderSet.LENGTH);
        } catch (IOException e1) {
            Log.e(TAG, "receivebMessageBody: Error when openInputStream in receiving file");
            responseCode = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        if (responseCode == ResponseCodes.OBEX_HTTP_OK) {
            if (D) Log.d(TAG, "receivebMessageBody: allocating length: " + 0x10000);

            int outputBufferSize = op.getMaxPacketSize();
            byte[] b = new byte[outputBufferSize];
            int readLength = 0;
            int position = 0;

            try {
                while (position != length.intValue()) {

                    readLength = iStream.read(b);

                    if (readLength == -1) {
                        Log.d(TAG, "receivebMessageBody: Receive file reached stream end at position" + position);
                        break;
                    }

                    /* Write the byte stream to String */
                    msgBody = new String(b);

                    Log.d(TAG, "receivebMessageBody: Message body received is " + msgBody);
                }

            } catch (IOException e1) {
                Log.e(TAG, "receivebMessageBody: Error when receiving file");
                responseCode = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            } finally {
                try {
                    if (iStream != null) {
                        iStream.close();
                    }
                } catch (IOException e1) {
                    Log.e(TAG, "receivebMessageBody: Error when closing stream after send");
                    responseCode = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                }
            }
        }

        return msgBody;
    }

    /**
     * Creates and sends Get Folder Listing Response
     * @param op
     * @param paramValue
     * @return
     */
    private int createSendGetFolderResponse(Operation op, BluetoothMapApplicationParameters paramValue) {
        if (D) Log.d(TAG, "createSendGetFolderResponse called");

        byte[] bytesInFile = null;
        StringBuffer strBuffer = new StringBuffer();
        OutputStream outStream = null;
        ApplicationParameter ap = new ApplicationParameter();
        String localCurrentPath;

        try {

            if ((paramValue.maxListCount > 0xFFFF) || (paramValue.listStartOffset > 0xFFFF)) {
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            HeaderSet response = new HeaderSet();
            byte [] folderListingSize = new byte[2];
            int numFolders = 0;

            /* Based upon the current folder, send path to the
             * mFolderAccessor.createFolderListing() method */
            localCurrentPath = mapCurrentFolderToString();

            strBuffer.append(mFolderAccessor.createFolderListing(localCurrentPath, localMasInstance, paramValue));

            if (strBuffer.toString().compareToIgnoreCase("null") == 0) {
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }

            bytesInFile = strBuffer.toString().getBytes();

            numFolders = mFolderAccessor.getFolderListSize();
            folderListingSize[0] = (byte)((numFolders / 256) & 0xff);/* HIGH VALUE */
            folderListingSize[1] = (byte)((numFolders % 256) & 0xff);/* LOW VALUE */
            Log.i(TAG, "folderListingSize[0] " + folderListingSize[0]);
            Log.i(TAG, "folderListingSize[1] " + folderListingSize[1]);

            /* Add application parameter folderListingSize to HeaderSet */
            ap.addAPPHeader(Constants.APPLI_PARAM_ID_FOLDER_LISTING_SIZE,
                    Constants.FOLDER_LISTING_SIZE_FIELD_LENGTH, folderListingSize);

            response.setHeader(HeaderSet.APPLICATION_PARAMETER, ap.getAPPparam());
            op.sendHeaders(response);
            if (paramValue.maxListCount != 0) {
                try {
                    outStream = op.openDataOutputStream();
                    outStream.write(bytesInFile);
                    if (V) Log.v(TAG, "data sent successfully to the remote client");
                    outStream.flush();
                } catch (IOException e) {
                    Log.e(TAG, "exception in sending xml to client " + e.toString());
                    return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                } finally {
                    try {
                        outStream.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Exception in closing outStream" + e.toString());
                        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
                    }
                }
            }
        } catch (IOException e) {
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        return ResponseCodes.OBEX_HTTP_OK;
    }

    /**
     * Creates and sends Get Message Listing Response
     * @param op
     * @param paramValue
     * @return
     */
    private int createSendGetMessageListingResponse(Operation op, BluetoothMapApplicationParameters paramValue) {
        if (D) Log.d(TAG, "createSendGetMessageListingResponse called");

        StringBuffer strBuffer = new StringBuffer();
        ApplicationParameter ap = new ApplicationParameter();

        String appParam = "Off";
        byte[] newMessagePresent = new byte[1];
        byte [] messageListingSize = new byte[2];
        int numMessageListingSize = 0;
        try {
            if ((paramValue.maxListCount > 0xFFFF) || (paramValue.listStartOffset > 0xFFFF)) {
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            HeaderSet response = new HeaderSet();
            String newPath = null, folderName;
            int mCurrentPathForOperation;
            /* Get the name of the folder if specified */
            newPath = mapCurrentFolderToString();
            folderName = (String )op.getReceivedHeader().getHeader(HeaderSet.NAME);

            if ((folderName != null) && (folderName.isEmpty() == false)) {
                mCurrentPathForOperation = mFolderAccessor.getCurrentPathStateFromFolderName(folderName);
            } else {
                mCurrentPathForOperation = mFolderAccessor.getCurrentPathStateFromString(newPath);
            }
            mMessageAccessor.resetNewMessagePresence();
            mMessageAccessor.resetMessageListingSize();
            if (paramValue.maxListCount != 0) {
                mMessageAccessor.resetGenericMessageList();
                mMessageAccessor.createMessageListingResponse(mCurrentPathForOperation,
                        paramValue);
                strBuffer.append(mMessageAccessor.getLastGeneratedMessageList());
                mMessageAccessor.resetGenericMessageList();
                Log.i(TAG, "MessageListing is " + strBuffer);
            } else {
                /* paramValue.maxListCount is zero so just calculate the message listing
                 * size and return it without any body */
                if (localMasInstance == Constants.SMS_MASINSTANCE)
                    mMessageAccessor.calculateMessageListingSize(paramValue, mCurrentPathForOperation);
                else if (localMasInstance == Constants.EMAIL_MASINSTANCE)
                    mMessageAccessor.calculateEmailListingSize(paramValue, mCurrentPathForOperation);
            }

            /* Add application parameters to HeaderSet */
            if (mMessageAccessor.isNewMessagePresent() == true) {
                newMessagePresent[0] = 0x01;
                appParam = "On";
            } else {
                appParam = "Off";
                newMessagePresent[0] = 0x00;
            }
            ap.addAPPHeader(Constants.APPLI_PARAM_ID_NEW_MSG,
                    (byte)1, newMessagePresent);

            /* add the app param Message Listing Size */
            numMessageListingSize = mMessageAccessor.getMessageListingSize();
            Log.i(TAG, "getMessageListingSize returned : " + numMessageListingSize);

            if (paramValue.maxListCount != 0) {
                /* Add the app param MSETime */
                appParam = mMessageAccessor.getLocalTimeBasisOfMSE();
                Log.i(TAG, "getLocalTimeBasisOfMSE returned : " + appParam);

                ap.addAPPHeader(Constants.APPLI_PARAM_ID_MSET_TIME,
                        (byte)(appParam.getBytes()).length, appParam.getBytes());
            }

            messageListingSize[0] = (byte)((numMessageListingSize / 256) & 0xff);/* HIGH VALUE */
            messageListingSize[1] = (byte)((numMessageListingSize % 256) & 0xff);/* LOW VALUE */
            ap.addAPPHeader(Constants.APPLI_PARAM_ID_MSG_LISTING_SIZE,
                    Constants.MESSAGE_LISTING_SIZE_FIELD_LENGTH, messageListingSize);
            try {
                response.setHeader(HeaderSet.APPLICATION_PARAMETER, ap.getAPPparam());
                op.sendHeaders(response);

                if (paramValue.maxListCount != 0) {
                    OutputStream outStream = null;

                    /* Send Body packets synchronously instead */
                    outStream = op.openDataOutputStream();
                    outStream.write(strBuffer.toString().getBytes());

                    /* Dump message listing retrieved */
                    if (D) {
                        createFileFromString(strBuffer, MSGLIST_FILENAME);
                    }

                    /* Free this strBuffer */
                    strBuffer.delete(0, strBuffer.length());

                    if (V) Log.v(TAG, "data sent successfully to the remote client");
                }
            } catch (IOException e) {
                Log.e(TAG, e.toString());
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }
        }
        catch (IOException e) {
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            /* Free this strBuffer */
            strBuffer.delete(0, strBuffer.length());
        }
        return ResponseCodes.OBEX_HTTP_OK;
    }

    private File createFileFromString(StringBuffer InBuffer, String fileName) {
        File fileToRead = null;
        FileOutputStream OutFile = null;
        boolean fileCreated = false;

        if ((fileName != null) && (fileName.compareTo("") != 0) && (InBuffer != null)) {
            try {
                OutFile = mContext.openFileOutput(fileName, Context.MODE_PRIVATE);
                fileCreated = true;
            } catch (FileNotFoundException e) {
                Log.e(TAG, e.toString());
            }

            /* Write the string buffer */
            try {
                OutFile.write(InBuffer.toString().getBytes());
            } catch (IOException e) {
                Log.e(TAG, e.toString());
            } finally {
                try {
                    OutFile.close();
                } catch (IOException e) {
                    Log.e(TAG, e.toString());
                }
            }
            if (fileCreated == true) {
                fileToRead = new File(mContext.getFilesDir() + "/" + fileName);
            }
        } else {
            Log.i(TAG, "Bad/No fileName specified or InputBuffer is NULL!!");
        }
        Log.i(TAG, "Whether fileCreated : " + fileCreated);
        return fileToRead;
    }

    /**
     * Creates and sends Get Message Response
     * @param op
     * @param paramValue
     * @return
     */
    private int createSendGetMessageResponse(Operation op, BluetoothMapApplicationParameters paramValue) {
        if (D) Log.d(TAG, "createSendGetMessageResponse called");

        ApplicationParameter ap = new ApplicationParameter();
        String msgHandle = null;

        try {
            if (mCurrentPath == null) {
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }

            if (((paramValue.attachment < 0x00) || (paramValue.attachment > 0x01)) ||
                    ((paramValue.charSet < 0x00) || (paramValue.charSet > 0x01))) {
                Log.e(TAG, "Bad application parameters");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            Log.i(TAG, "attachment = " + paramValue.attachment);
            Log.i(TAG, "isFractionRequestPresent = " + paramValue.isFractionRequestPresent);

            /* Create bMessage response */
            HeaderSet response = new HeaderSet();
            HeaderSet request = op.getReceivedHeader();
            File fileinfo = null;

            /* Retrieve the msgHandle for the message to retrieve */
            msgHandle = (String) request.getHeader(HeaderSet.NAME);
            if (BluetoothMapMessageAccessor.isMessageHandleValid(msgHandle) == false) {
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            /* Call the method GetMessageResponse */
                fileinfo = mMessageAccessor.createGetMessageResponse(msgHandle,
                        mCurrentPathState,
                    (paramValue.charSet == 0)?true:false);

            /**
             * Include fractionDeliver and keep the fractionDeliver value to
             * "last" by default since complete handling of FractionDelivery feature is
             * not supported
             */
            if (paramValue.isFractionRequestPresent == true) {
                byte[] fractionValueLast = new byte[1];
                fractionValueLast[0] = 0x01;
                ap.addAPPHeader(Constants.APPLI_PARAM_ID_FRACTION_DELIVER,
                        Constants.FRACTION_DELIVER_FIELD_LENGTH, fractionValueLast);

                response.setHeader(HeaderSet.APPLICATION_PARAMETER, ap.getAPPparam());
                op.sendHeaders(response);
            }

            if (paramValue.maxListCount != 0) {
                /* Send the OBEX body using the File */
                if (fileinfo != null) {
                    sendBody(op, fileinfo);
                } else {
                    Log.i(TAG, "File null found for MessageListing");
                    return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
                }
            }

        } catch (IOException e) {
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        return ResponseCodes.OBEX_HTTP_OK;
    }

    private final int sendBody(Operation op, File fileinfo) {

        Log.e(TAG, "sendFile = " + fileinfo.getName());
        int position = 0;
        int readLength = 0;
        int outputBufferSize = op.getMaxPacketSize();
        FileInputStream fileInputStream;
        OutputStream outputStream = null;
        BufferedInputStream bis;
        long timestamp = 0;

        if (D) Log.d(TAG, "Send Body: Enter");
        try {
            byte[] buffer = new byte[outputBufferSize];
            fileInputStream = new FileInputStream(fileinfo);
            outputStream = op.openOutputStream();
            bis = new BufferedInputStream(fileInputStream);
            while ((position != fileinfo.length())) {
                if (V) {
                    timestamp = System.currentTimeMillis();
                }
                readLength = bis.read(buffer, 0, outputBufferSize);
                outputStream.write(buffer, 0, readLength);
                position += readLength;
                if (V) {
                    Log.e(TAG, "Sending file position = " + position
                            + " readLength " + readLength + " bytes took "
                            + (System.currentTimeMillis() - timestamp) + " ms");
                }
            }

            /* Close the inputByteStream */
            bis.close();

        } catch (IOException e) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            /* Close the output stream */
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, "IOException while closing the output stream");
                }
            }
        }

        if (position == fileinfo.length()) {
            if (D) Log.d(TAG, "SendBody : Exit: OK");
            return ResponseCodes.OBEX_HTTP_OK;
        } else {
            if (D) Log.d(TAG, "SendBody : Exit: CONTINUE");
            return ResponseCodes.OBEX_HTTP_CONTINUE;
        }
    }

    /**
     * Creates and sends Push Message Response
     * @param op
     * @param paramValue
     * @return
     */
    private int createSendPushMessageResponse(Operation op, BluetoothMapApplicationParameters paramValue) {
        if (D) Log.d(TAG, "createSendPushMessageResponse called");

        String msgHandle = null, bMessageBody = null, folderName = null;

        try {
            if (mCurrentPath == null) {
                return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
            }

            if (((paramValue.transparent < 0x00) || (paramValue.transparent > 0x01)) ||
                    ((paramValue.charSet < 0x00) || (paramValue.charSet > 0x01)) ||
                    ((paramValue.retry < 0x00) || (paramValue.retry > 0x01))) {
                Log.e(TAG, "Bad application parameters");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            if (paramValue.isCharSetPresent == false) {
                /* Charset parameter not present */
                Log.e(TAG, "createSendPushMessageResponse : Charset parameter not present");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            /* Create response */
            HeaderSet response = new HeaderSet();
            HeaderSet request = op.getReceivedHeader();
            int mCurrentPathForOperation = mCurrentPathState;
            int param_retry, charset, transparent;
            /* Retrieve the folder name to push message to */
            folderName = (String )request.getHeader(HeaderSet.NAME);

            mCurrentPathForOperation = mCurrentPathState;
            if ((folderName != null) && (folderName.isEmpty() == false)) {
                mCurrentPathForOperation = mFolderAccessor.getCurrentPathStateFromFolderName(folderName);
            }

            /**
             * If the current path is one of telecom, msg or root, just reset it for operation
             * to the draft, considered a default folder for storing the pushed messages
             * for later editing or viewing */
            if ((mCurrentPathForOperation == Constants.MAP_FOLDER_ROOT) ||
                    (mCurrentPathForOperation == Constants.MAP_FOLDER_TELECOM) ||
                    (mCurrentPathForOperation == Constants.MAP_FOLDER_MSG)) {
                mCurrentPathForOperation = Constants.MAP_FOLDER_DRAFT;
            } else if (mCurrentPathForOperation == Constants.MAP_FOLDER_INVALID) {
                return ResponseCodes.OBEX_HTTP_FORBIDDEN;
            }

            /* Read bMessage body from InputStream */
            InputStream iStream = null;
            iStream = op.openInputStream();

            BufferedReader br = new BufferedReader(new InputStreamReader(iStream));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) {
                sb.append(line).append("\r\n");
            }
            bMessageBody = new String(sb);
            Log.i(TAG, "Body header received is complete " + bMessageBody);
            /*  To forbid saving an empty number SMS to MSE*/
            int start = 0;
            int end = 0;
            String CRLF = "\r\n";
            String tel = new String();
            start = bMessageBody.indexOf((CRLF + "TEL:"));
            start = bMessageBody.indexOf((CRLF + "TEL:"), start + 1);
            start = start + (("TEL:").length() + CRLF.length());
            end = bMessageBody.indexOf(CRLF, start);
            tel = bMessageBody.substring(start, end);

            if (((tel.length() == 0) || (tel.trim().isEmpty() == true)) ) {
                Log.i(TAG, "Tel number length is " + tel.length() );
                Log.i(TAG,"Tel.trim().isEmpty() is " + tel.trim().isEmpty() );
                return ResponseCodes.OBEX_HTTP_FORBIDDEN;
            }

            param_retry = paramValue.retry;
            charset = paramValue.charSet;
            transparent = paramValue.transparent;

            if (!mMessageSender.performPushMessageInSeperateContext(bMessageBody,
                    param_retry,
                    charset,
                    transparent,
                    mCurrentPathForOperation)) {
                return ResponseCodes.OBEX_HTTP_UNAVAILABLE;
            }

            /* Cleanup StringBuilder and BufferedReader */
            sb.delete(0, sb.length());
            br.close();

            /* Retrieve the msgHandle of newly created pushed message */
            msgHandle = mMessageAccessor.mLastMessageHandleCreated;

            /* Setting the newly created msgHandle */
            Log.i("createSendPushMessageResponse", "Setting the msgHandle : " + msgHandle);
            response.setHeader(HeaderSet.NAME, msgHandle);
            op.sendHeaders(response);

        } catch (IOException e) {
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        return ResponseCodes.OBEX_HTTP_OK;
    }

    /**
     * Creates and sends response to Update INBOX
     * request from message access client.
     *
     * @param op
     * @param paramValue
     * @return
     */
    private int sendUpdateInboxResponse(Operation op) {
        if (D) Log.d(TAG, "sendUpdateInboxResponse called");

        /* Retrieve an error if UpdateInbox operation capability wasn't implemented */
        if (!mMessageAccessor.performUpdateInbox()) {
            return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    /**
     * This method is used to send response for setMessageStatus
     * request from message access client.
     *
     * @param op
     * @param paramValue
     * @return
     */
    private int createsetMessageStatusResponse(Operation op, BluetoothMapApplicationParameters paramValue) {
        if (D) Log.d(TAG, "createsetMessageStatusResponse called");

        String msgHandle = null;

        try {
            if (((paramValue.statusIndicator < 0x00) || (paramValue.statusIndicator > 0x01)) ||
                    ((paramValue.statusValue < 0x00) || (paramValue.statusValue > 0x01))) {
                Log.e(TAG, "Bad application parameters");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }

            /* Read the response values */
            HeaderSet request = op.getReceivedHeader();

            /* Retrieve the message handle for message
             * whose status is to be updated */
            msgHandle = (String )request.getHeader(HeaderSet.NAME);

            return mMessageAccessor.msgStatus(msgHandle, paramValue);
        } catch (IOException e) {
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
    }

    /**
     *
     * @param AppParam
     * @return
     */
    public int handleNotificationRegistration(int notificationStatus) {
        BluetoothSocket thisDevice = null;
        BluetoothMapService thisService = null;

       Log.i(TAG, "MasInstance = " + localMasInstance + "Notification Status = " + notificationStatus);

        /* Check for any internal errors */
        if (BluetoothMapService.mMessageNotifier == null) {
            Log.i(TAG, "mMessageNotifier Not Initialized : OBEX_HTTP_INTERNAL_ERROR");
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        /* Check for correct notificationStatus values */
        if ((notificationStatus != 0) && (notificationStatus != 1)) {
            Log.i(TAG, "Illegal Value for notificationStatus = " + notificationStatus);
            return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
        }

        boolean isMNSConnected = BluetoothMapMessageNotificationClient.isAnyNotificationsTurnedOn();

        Log.i(TAG, "isMNSConnected " + isMNSConnected);

        if (notificationStatus == 0x01) {
            Log.i(TAG, "turnOnMASInstanceNotification for " + localMasInstance);
            BluetoothMapMessageNotificationClient.turnOnMASInstanceNotification(localMasInstance);
        } else {
            Log.i(TAG, "TurnOMASInstanceNotification for " + localMasInstance);
            BluetoothMapMessageNotificationClient.turnOffMASInstanceNotification(localMasInstance);
        }

        if (isMNSConnected) {
        if (BluetoothMapMessageNotificationClient.areAllNotificationsTurnedOff()) {
                /* Disconnect the notification channel as well since no MAS Instance remains for which registration is on */
                Log.i(TAG, "mMessageNotifier.diconnectMnsChannel ");
                BluetoothMapService.mMessageNotifier.diconnectMnsChannel();
            }
        } else if (BluetoothMapMessageNotificationClient.isAnyNotificationsTurnedOn()) {
                Log.i(TAG, "Starting Msg Notification Session");
                /* Retrieve this device */
                thisService = (BluetoothMapService)mContext;
                thisDevice = thisService.GetServerSocket(localMasInstance);

            /* Notification MAS Instance is registered for first time connect the Notification channel now */
            BluetoothMapService.mMessageNotifier.connectMnsChannel(thisDevice.getRemoteDevice().getAddress());
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public int onPut(Operation op) {
        if (D) Log.d(TAG, "onPut called");
        int obexResponse = ResponseCodes.OBEX_HTTP_OK;
        String typeHeader;
        byte [] applicationParams;

        BluetoothMapApplicationParameters applicationParamsValue = new BluetoothMapApplicationParameters();

        synchronized(this) {
            try {
                HeaderSet request = op.getReceivedHeader();
                typeHeader = (String) request.getHeader(HeaderSet.TYPE);
                applicationParams = (byte [])request.getHeader(HeaderSet.APPLICATION_PARAMETER);

                Log.i(TAG, "Received header is " + typeHeader);

                if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_MESSAGE) == 0) {
                    /* Get Folder Listing operation */
                    parsePushMessageApplicationParams(applicationParams, applicationParamsValue);

                    /* Start forming response to get folder listing request */
                    return createSendPushMessageResponse(op, applicationParamsValue);

                } else if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_UPDATE_INBOX) == 0) {
                    /* Update inbox operation */
                    return sendUpdateInboxResponse(op);

                } else if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_SET_MESSAGE_STATUS) == 0) {
                    /* Set message status operation */
                    boolean parseOk;
                    parseOk = parsesetMessageStatusApplicationParams(applicationParams,
                            applicationParamsValue);

                    /* one or both application param are missing */
                    if (!parseOk) {
                        return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
                    }
                    /* Start forming response to get folder listing request */
                    return createsetMessageStatusResponse(op, applicationParamsValue);
                } else if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_SET_NOTIFICATION_REGISTRATION) == 0) {
                    /* Set Notification Registration */
                    parseSetNotificationRegApplicationParams(applicationParams,
                            applicationParamsValue);

                    return handleNotificationRegistration(applicationParamsValue.notificationStatus);
                }
            } catch (IOException e) {
                Log.e(TAG, e.toString());
            }
            /* De-initialize the message accessor data */
            mMessageAccessor.deinitializeMessageAccessorData();
        }
        return obexResponse;
    }

    @Override
    public int onConnect(HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onConnect called");
        if (V) Constants.GetHeader(request);

        /* Since a connect request is received, turn off the Notification registration for it by default */
        BluetoothMapMessageNotificationClient.turnOffMASInstanceNotification(localMasInstance);

        if (mPathString == null) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        try {
            byte[] uuid = (byte[]) request.getHeader(HeaderSet.TARGET);

            if (uuid != null) {
                if (V) Log.v(TAG, "onConnect(): uuid =" + Arrays.toString(uuid));
                reply.setHeader(HeaderSet.WHO, uuid);
            } else {
                if (V) Log.v(TAG, "onConnect(): no uuid");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            }
        } catch (IOException e) {
            if (D) Log.d(TAG, "error while retrieving TARGET header");
            Log.e(TAG, e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        mCurrentPath = Constants.MAP_ROOT_FOLDER_DIR;

        Message msg = Message.obtain(mCallback);
        msg.what = BluetoothMapService.MSG_OBEX_SESSION_CONNECTED;
        msg.sendToTarget();
        Log.i(TAG, "BluetoothMapService.MSG_OBEX_SESSION_CONNECTED sent");

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public void onDisconnect(HeaderSet req, HeaderSet resp) {
        if (D) Log.d(TAG, "onDisconnect");
        resp.responseCode = ResponseCodes.OBEX_HTTP_OK;
        mCurrentPath = Constants.MAP_ROOT_FOLDER_DIR;
        mPathString = Constants.MAP_ROOT_FOLDER_DIR;

        /**
         * Since a disconnect request is received, check if there is
         * any notification turned on, if yes then turn off the Notification
         * registration for it and disconnect the channel, if no then dont do anything
         * since disconnection would already be in progress */
        if (BluetoothMapMessageNotificationClient.isAnyNotificationsTurnedOn() == true) {
            BluetoothMapMessageNotificationClient.turnOffMASInstanceNotification(localMasInstance);
            if (BluetoothMapMessageNotificationClient.areAllNotificationsTurnedOff() == true) {
                /* Disconnect the notification channel as well since no
                 * MAS Instance remains for which registration is on */
                BluetoothMapService.mMessageNotifier.diconnectMnsChannel();
            }
        }

        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = BluetoothMapService.MSG_OBEX_SESSION_DISCONNECTED;
            msg.sendToTarget();
            if (V) Log.v(TAG, "onDisconnect(): msg MSG_OBEX_SESSION_DISCONNECTED sent to service callback");
        }
    }

    @Override
    public void onClose() {
        if (V) Log.v(TAG, "release WakeLock");
        if (mWakeLock.isHeld()) {
            mWakeLock.release();
        }
        if (mPartialWakeLock.isHeld()) {
            mPartialWakeLock.release();
        }

        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = BluetoothMapService.MSG_OBEX_TRANSPORT_CLOSED;
            msg.sendToTarget();
            if (V) Log.v(TAG, "onDisconnect(): msg MSG_OBEX_TRANSPORT_CLOSED sent to service callback");
        }
    }

    @Override
    public int onGet(Operation op) {
        if (D) Log.d(TAG, "onGet called for MasInstance " + localMasInstance);
        Log.i(TAG, "onGet called for MasInstance EMAIL=1, SMS=0" + localMasInstance);
        String typeHeader;
        byte [] applicationParams;

        BluetoothMapApplicationParameters applicationParamsValue = new BluetoothMapApplicationParameters();

        try {
            HeaderSet request = op.getReceivedHeader();
            typeHeader = (String) request.getHeader(HeaderSet.TYPE);
            applicationParams = (byte []) request.getHeader(HeaderSet.APPLICATION_PARAMETER);

            synchronized(this) {
                Log.i(TAG, "Received header is" + typeHeader);

                Log.i(TAG, "Comparison result " +
                        typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_FOLDER_LISTING));
                if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_FOLDER_LISTING) == 0) {
                    /* Get Folder Listing operation */
                    parseFolderListingApplicationParameters(applicationParams, applicationParamsValue);
                    /* Start forming response to get folder listing request */
                    return createSendGetFolderResponse(op, applicationParamsValue);

                } else if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_GET_MESSAGE_LISTING) == 0) {
                    /* GetMessageListing operation */

                    /* Parse the application parameters associated with this operation */
                    parseGetMessageListingApplicationParams(applicationParams, applicationParamsValue);

                    /* Form and send GetMessageListing response */
                    return createSendGetMessageListingResponse(op, applicationParamsValue);

                } else if (typeHeader.compareTo(Constants.OBEX_HEADER_TYPE_GET_MESSAGE) == 0) {
                    /* Get Message operation Parse the application parameters associated with this operation */
                    parseGetMessageApplicationParams(applicationParams, applicationParamsValue);

                    /* Log the application headers */
                    applicationParamsValue.logApplicationParameters();

                    /* Form and send GetMessageListing response */
                    return createSendGetMessageResponse(op, applicationParamsValue);

                }
            }
        } catch (IOException e) {
            Log.e(TAG, "Exception occured" + e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        /* De-initialize the message accessor data */
        mMessageAccessor.deinitializeMessageAccessorData();

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public int onSetPath(HeaderSet request, HeaderSet reply, boolean backup, boolean create) {
        if (D) Log.d(TAG, "onSetPath called");

        int tempCurrentPathState;
        String name = null;
        byte[] uuid = null;
        if (D) Log.d(TAG, "initial mPathString in onSetPath " + mPathString);
        Log.i(TAG, "mCurrentPathState" + mCurrentPathState + "Path :" + mPathString);
        Log.i(TAG, "create" + create + "backup" + backup);

        try {
            name = (String) request.getHeader(HeaderSet.NAME);
            uuid = (byte[]) request.getHeader(HeaderSet.TARGET);

            if (D) Constants.GetHeader(request);

            if (backup == true) {
                if (mCurrentPathState == Constants.MAP_FOLDER_ROOT) {
                    mPathString = mapCurrentFolderToString();

                    Log.i(TAG, "mCurrentPathState" + mCurrentPathState + "Path :" + mPathString);

                    /*We are already on top.*/
                    return ResponseCodes.OBEX_HTTP_NOT_FOUND;
                } else {
                    tempCurrentPathState = changeFolderOneLevelUp(mCurrentPathState);
                    if (tempCurrentPathState != Constants.MAP_INVALID_PATH_REQ) {
                        mCurrentPathState = tempCurrentPathState;
                        mPathString = mapCurrentFolderToString();
                        Log.i(TAG, "mCurrentPathState" + mCurrentPathState + "Path :" + mPathString);

                        if (name != null) {
                            Log.i(TAG, "mCurrentPathState" + mCurrentPathState + "Path :" + mPathString);
                             if (name.compareTo("Trash") == 0) {
                                name = "deleted";
                            }
                            int temp = mFolderAccessor.checkPathValidity(mPathString, name);
                            if (temp == Constants.MAP_INVALID_PATH_REQ) {
                                return ResponseCodes.OBEX_HTTP_NOT_FOUND;
                            }

                            /* Go inside the folder named "name" */
                            mPathString = mPathString.concat(Constants.PATH_SEPARATOR).concat(name);
                        }

                        /* Update mCurrentPathState */
                        mCurrentPathState = mFolderAccessor.getCurrentPathStateFromString(mPathString);

                        return ResponseCodes.OBEX_HTTP_OK;
                    }
                }

            } else if (backup == false) {
                if (name != null) {
                    if (name.isEmpty() == true) {
                        /* Reset to root folder */
                        mCurrentPathState = Constants.MAP_FOLDER_ROOT;
                        mPathString = mapCurrentFolderToString();
                        return ResponseCodes.OBEX_HTTP_OK;
                    } else {
                        Log.i(TAG, "mCurrentPathState" + mCurrentPathState + "Path :" + mPathString);
                        if (name.compareTo("Trash") == 0) {
                            name = "deleted";
                        }
                        /* Go down one level into this directory name */
                        int temp = mFolderAccessor.checkPathValidity(mPathString, name.trim());
                        if (temp == Constants.MAP_INVALID_PATH_REQ) {
                            return ResponseCodes.OBEX_HTTP_NOT_FOUND;
                        }

                        String [] splitted = mPathString.split("/");
                        if (name.compareToIgnoreCase(splitted[splitted.length - 1]) != 0) {
                            /* Go inside the folder named "name" */
                            mPathString = mPathString.concat(Constants.PATH_SEPARATOR).concat(name);
                        }

                        /* Update mCurrentPathState */
                        mCurrentPathState  = mFolderAccessor.getCurrentPathStateFromString(mPathString);
                    }
                } else {
                    /**
                     *If name is null, it is the case when some device omit the
                     * name header altogether, it is equal to reset to root request
                     * Reset to root folder
                     */
                    Log.i(TAG, "Reset to root folder request");
                    mCurrentPathState = Constants.MAP_FOLDER_ROOT;
                    mPathString = mapCurrentFolderToString();
                    return ResponseCodes.OBEX_HTTP_OK;
                }
            }

            if (uuid != null) {
                reply.setHeader(HeaderSet.WHO, uuid);
            }
        } catch (IOException e) {
            Log.e(TAG, "exception caught in onSetPath " + e.toString());
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }
        return ResponseCodes.OBEX_HTTP_OK;

    }

    @Override
    public int onDelete(HeaderSet request, HeaderSet reply) {
        if (D) Log.d(TAG, "onDelete called : MAP doesn't implement onDelete");

        return ResponseCodes.OBEX_HTTP_FORBIDDEN;
    }

    /* Returns current path state */
    public int getCurrentPath() {
        return mCurrentPathState;
    }
}
