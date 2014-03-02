/*
 * Copyright (C) ST-Ericsson SA 2010
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
 * - Neither the name of the ST-Ericsson, Inc. nor the names of its contributors
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

import android.util.Log;
import android.net.Uri;
import java.io.IOException;

import javax.obex.HeaderSet;

/**
 * Bluetooth MAP internal constants definition
 */
public class Constants {

    /**
     * TAG
     */
    public static final String TAG = "BtMapConstants";

    /**
     * Debug level logging
     */
    public static final boolean DEBUG = true;

    /**
     * Verbose level logging
     */
    public static final boolean VERBOSE = true;

    public static final String MEDIA_SCANNED = "scanned";

    public static boolean USE_TCP_DEBUG = false;

    public static final int TCP_DEBUG_PORT = 21;

    public static boolean USE_TCP_SIMPLE_SERVER = false;

    public static final int MSG_AUTHORIZE_INCOMING_MAP_CONNECTION = 100;

    public static final int MSG_INCOMING_BTMAP_CONNECTION = 101;

    public static final int MSG_USER_TIMEOUT = 102;
    public static final int MSG_AUTH_TIMEOUT = 103;
    /**
     * Timeout value in ms to release authorization hook
     */
    public static final int USER_CONFIRM_TIMEOUT_VALUE_IN_MS = 30000;

    /**
     * Intent indicating incoming connection request which is sent to
     * BluetoothFtpActivity
     */
    public static final String AUTHORIZE_REQUEST_ACTION = "com.stericsson.bluetooth.map.authorizerequest";

    /**
     * Intent indicating incoming connection request accepted by user which is
     * sent from BluetoothFtpActivity
     */
    public static final String AUTHORIZE_ALLOWED_ACTION = "com.stericsson.bluetooth.map.authorizeallowed";

    /**
     * Intent indicating incoming connection request denied by user which is
     * sent from BluetoothFtpActivity
     */
    public static final String AUTHORIZE_DISALLOWED_ACTION = "com.stericsson.bluetooth.map.authorizedisallowed";

    /**
     * Intent indicating timeout for user confirmation, which is sent to
     * BluetoothFtpActivity
     */
    public static final String USER_CONFIRM_TIMEOUT_ACTION = "com.stericsson.bluetooth.map.userconfirmtimeout";

    /**
     * Intent Extra name indicating always allowed which is sent from
     * BluetoothFtpActivity
     */
    public static final String EXTRA_AUTHORIZE_ALWAYS_ALLOWED = "com.stericsson.bluetooth.map.alwaysallowed";

    public static final String MAP_ROOT_FOLDER_DIR = "";

    public static final String SMS_URI = "content://sms/";
    public static final Uri SMSURI = Uri.parse(SMS_URI);

    public static final String INBOX_URI = "content://sms/inbox";
    public static final Uri INBOXURI = Uri.parse(INBOX_URI);

    public static final String OUTBOX_URI = "content://sms/outbox";
    public static final Uri OUTBOXURI = Uri.parse(OUTBOX_URI);

    public static final String SENT_URI = "content://sms/sent";
    public static final Uri SENTURI = Uri.parse(SENT_URI);

    public static final String DRAFT_URI = "content://sms/draft";
    public static final Uri DRAFTURI = Uri.parse(DRAFT_URI);

    public static final String DELETED_URI = "content://sms/deleted";
    public static final Uri DELETEDURI = Uri.parse(DELETED_URI);

    public static final String FAILED_URI = "content://sms/failed";

    public static final String QUEUED_URI = "content://sms/queued";

    public static final String EMAIL_URI = "content://com.android.email.provider/message";
    public static final Uri EMAILURI = Uri.parse(EMAIL_URI);

    public static final String EMAIL_URI_ACCOUNT = "content://com.android.email.provider/account";

    public static final String EMAIL_URI_NOTIFICATION = "content://com.android.email.provider";

    public static final String EMAIL_URI_MAILBOX = "content://com.android.email.provider/mailbox";

    public static final String EMAIL_URI_SYNC_SERVER = "content://com.android.email.provider/syncedMessage";

    public static final String EMAIL_URI_ATTACHMENT = "content://com.android.email.provider/attachment";

    public static final String EMAIL_URI_BODY = "content://com.android.email.provider/body";

    public static final String THREAD_TABLE_SMS_URI = "content://mms-sms/threads";

    public static final String CANONICAL_SMS_URI = "content://mms-sms/canonical-addresses";

    public static final String SMS_MMS = "content://mms-sms/";

    public static final String SMS_MMS_INBOX = "content://mms-sms/inbox";

    public static final String SMS_MMS_SENT = "content://mms-sms/sent";

    public static final String SMS_MMS_DRAFT = "content://mms-sms/draft";

    public static final String SMS_MMS_FAILED = "content://mms-sms/failed";

    public static final String SMS_MMS_QUEUED = "content://mms-sms/queued";

    public static final String SMS_MMS_OUTBOX = "content://mms-sms/outbox";

    public static final String ACTION_UPDATE_OUTBOX = "stericsson.btmap.intent.action.UPDATE_OUTBOX";

    /**
     *  Constant to indicate that current folder is root folder.
     */
    public static final int MAP_FOLDER_ROOT = 0x01;

    /**
     *  Constant to indicate that current folder is /telecom.
     */
    public static final int MAP_FOLDER_TELECOM = 0x02;

    /**
     *  Constant to indicate that current folder is /telecom/msg.
     */
    public static final int MAP_FOLDER_MSG = 0x03;

    /**
     *  Constant to indicate that current folder is /telecom/msg/inbox.
     */
    public static final int MAP_FOLDER_INBOX = 0x04;

    /**
     *  Constant to indicate that current folder is /telecom/msg/outbox.
     */
    public static final int MAP_FOLDER_OUTBOX = 0x05;

    /**
     *  Constant to indicate that current folder is /telecom/msg/sent.
     */
    public static final int MAP_FOLDER_SENT = 0x06;

    /**
     *  Constant to indicate that current folder is /telecom/msg/deleted.
     */
    public static final int MAP_FOLDER_DELETED = 0x07;

    /**
     *  Constant to indicate that current folder is /telecom/msg/draft.
     */
    public static final int MAP_FOLDER_DRAFT = 0x08;

    /**
     *  Constant to indicate that current folder is invalid.
     */
    public static final int MAP_FOLDER_INVALID = 0xFF;

    /**
     *  Value of OBEX type header for get folder listing
     *  request
     */
    public static final String OBEX_HEADER_TYPE_FOLDER_LISTING = "x-obex/folder-listing";

    /**
     *  Value of OBEX type header for get message listing
     *  request
     */
    public static final String OBEX_HEADER_TYPE_GET_MESSAGE_LISTING = "x-bt/MAP-msg-listing";

    /**
     * Constant containing OBEX type header for setMessageStatus
     * function
     */
    public static final String OBEX_HEADER_TYPE_SET_MESSAGE_STATUS = "x-bt/messageStatus";

    /**
     * Constant containing OBEX type header for object type Message
     *
     */
    public static final String OBEX_HEADER_TYPE_MESSAGE = "x-bt/message";

    /**
     * Constant containing OBEX type header for object type
     * related to Update INBOX, a "PUT" requet.
     *
     */
    public static final String OBEX_HEADER_TYPE_UPDATE_INBOX = "x-bt/MAP-messageUpdate";

    /**
     *  Value of OBEX type header for get message request
     */
    public static final String OBEX_HEADER_TYPE_GET_MESSAGE = "x-bt/message";

    /**
     *  Value of OBEX type header for send event request
     */
    public static final String OBEX_HEADER_TYPE_SEND_EVENT = "x-bt/MAP-event-report";

    /**
     *  Value of OBEX type header for SetNotification registration request
     */
    public static final String OBEX_HEADER_TYPE_SET_NOTIFICATION_REGISTRATION = "x-bt/MAP-NotificationRegistration";

    /**
     *  Values of ID for application parameters field
     *  Max List Count used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FOLDER_LISTING_SIZE = 0x11;

    /**
     *  Values of ID for application parameters field
     *  Max List Count used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_MAX_LIST_COUNT = 0x01;

    /**
     *  Values of ID for application parameters field
     *  List Start Offset used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_LIST_START_OFFSET = 0x02;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_SUBJECT_LEGTH = 0x13;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_PARAMETER_MASK = 0x10;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_MSG_TYPE = 0x03;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_PERIOD_BEGIN = 0x04;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_PERIOD_END = 0x05;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_READ_STATUS = 0x06;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILETER_RECIPIENT = 0x07;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_ORIGINATOR = 0x08;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_FILTER_PRIORITY = 0x09;

    /*****
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_NEW_MSG = 0x0D;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_MSET_TIME = 0x19;

    /**
     *  Values of ID for application parameters field
     *  Folder Listing Size used in get folder listing request
     */
    public static final byte APPLI_PARAM_ID_MASINSTANCE_ID = 0x0F;

    /**
     *  Values of ID for application parameters field
     *  message listing size used in get message listing request
     */
    public static final byte APPLI_PARAM_ID_MSG_LISTING_SIZE = 0x12;

    /**
     * Values of ID used for application parameters field
     * attachment in the GetMessage Request
     */
    public static final byte APPLI_PARAM_ID_ATTACHMENT = 0x0A;

    /**
     * Values of ID used for application parameters field
     * transparent in the GetMessage Request
     */
    public static final byte APPLI_PARAM_ID_TRANSPARENT = 0x0B;

    /**
     * Values of ID used for application parameters field
     * retry in the GetMessage Request
     */
    public static final byte APPLI_PARAM_ID_RETRY = 0x0C;

    /**
     * Values of ID used for application parameters field
     * charset in the GetMessage Request
     */
    public static final byte APPLI_PARAM_ID_CHARSET = 0x14;

    /**
     * Values of ID used for application parameters field
     * fraction-request in the GetMessage Request
     */
    public static final byte APPLI_PARAM_ID_FRACTION_REQUEST = 0x15;

    /**
     * Values of ID used for application parameters field
     * fraction-request in the GetMessage response
     */
    public static final byte APPLI_PARAM_ID_FRACTION_DELIVER = 0x16;

    /**
     * Values of ID used for application parameters field
     * statusIndicator in the setMessageStatus response
     */
    public static final byte APPLI_PARAM_ID_STATUS_INDICATOR = 0x17;

    /**
     * Values of ID used for application parameters field
     * statusIndicator in the setMessageStatus response
     */
    public static final byte APPLI_PARAM_ID_NOTIFICATION_STATUS = 0x0E;

    /**
     * Values of ID used for application parameters field
     * statusValue in the setMessageStatus response
     */
    public static final byte APPLI_PARAM_ID_STATUS_VALUE = 0x18;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte FOLDER_LISTING_SIZE_FIELD_LENGTH = 0x02;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte MAX_LIST_COUNT_FIELD_LENGTH = 0x02;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte LIST_START_OFFSET_FIELD_LENGTH = 0x02;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte APP_ID_SUBJECT_LENGTH_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte PARAMETER_MASK_FIELD_LENGTH = 0x04;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte FILTER_MESSAGE_TYPE_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte FILTER_READ_STATUS_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte FILTER_PRIORITY_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field Folder Listing Size
     *  used in folder listing response
     */
    public static final byte MESSAGE_LISTING_SIZE_FIELD_LENGTH = 0x02;

    /**
     *  Size of the application parameter attachment
     *  used in get message request
     */
    public static final byte ATTACHMENT_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field charset
     *  used in get message request
     */
    public static final byte CHARSET_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field charset
     *  used in get message request
     */
    public static final byte FRACTION_REQUEST_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field charset
     *  used in get message request
     */
    public static final byte FRACTION_DELIVER_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field charset
     *  used in get message request
     */
    public static final byte TRANSPARENT_FIELD_LENGTH = 0x01;

    /**
     *  Size of the application parameter field charset
     *  used in get message request
     */
    public static final byte RETRY_FIELD_LENGTH = 0x01;

    /**
     * Generic one byte length constants
     */
    public static final byte FIELD_LENGTH_ONE_BYTE = 0x01;

    /**
     * Path separator used for folder path related operations
     */
    public static final String PATH_SEPARATOR = "/";

    /**
     * constants defining remote client requests and permissions
     */

    public static final int MAP_RESPONSE_OK = 0;

    public static final int MAP_INVALID_PATH_REQ = 13;

    public static final int STATUS_IND_READ_STATUS = 0x00;
    public static final int STATUS_IND_DELETE_STATUS = 0x01;
    public static final int STATUS_VALUE_YES = 0x01;

    public static final int PARAM_TRANSPARENT_OFF = 0x00;

    public static final int MAP_EVENT_TYPE_NEW_MSG = 0x10;
    public static final int MAP_EVENT_TYPE_MSG_DELETED = 0x11;
    public static final int MAP_EVENT_TYPE_SENT_SUCCESS = 0x12;
    public static final int MAP_EVENT_TYPE_SENT_FAILED = 0x13;
    public static final int MAP_EVENT_TYPE_DELIVERY_SUCCESS = 0x14;
    public static final int MAP_EVENT_TYPE_DELIVERY_FAILED = 0x15;
    public static final int MAP_EVENT_TYPE_MEMORY_FULL = 0x16;
    public static final int MAP_EVENT_TYPE_MEMORY_AVAIL = 0x17;
    public static final int MAP_EVENT_TYPE_MSG_SHIFT = 0x18;

    public static final int MAP_MODE_SMS_ON = 0x001;
    public static final int MAP_MODE_EMAIL_ON = 0x100;

    public static final int BIT_SUBJECT = 0x1;
    public static final int BIT_DATETIME = 0x2;
    public static final int BIT_SENDER_NAME = 0x4;
    public static final int BIT_SENDER_ADDRESSING = 0x8;

    public static final int BIT_RECIPIENT_NAME = 0x10;
    public static final int BIT_RECIPIENT_ADDRESSING = 0x20;
    public static final int BIT_TYPE = 0x40;
    public static final int BIT_SIZE = 0x80;

    public static final int BIT_RECEPTION_STATUS = 0x100;
    public static final int BIT_TEXT = 0x200;
    public static final int BIT_ATTACHMENT_SIZE = 0x400;
    public static final int BIT_PRIORITY = 0x800;

    public static final int BIT_READ = 0x1000;
    public static final int BIT_SENT = 0x2000;
    public static final int BIT_PROTECTED = 0x4000;
    public static final int BIT_REPLYTO_ADDRESSING = 0x8000;

    public static final int FILTER_READ_STATUS_UNDREAD = 0x01;
    public static final int FILTER_READ_STATUS_READ = 0x10;
    public static final int FILTER_READ_STATUS_BOTH = 0x00;

    public static final int EMAIL_MASINSTANCE = 0x01;
    public static final int SMS_MASINSTANCE = 0x00;

    public static final int MESSAGE_TYPE_MASK_SMS_GSM = 0x01;
    public static final int MESSAGE_TYPE_MASK_SMS_CDMA = 0x02;
    public static final int MESSAGE_TYPE_MASK_MMS = 0x08;
    public static final int MESSAGE_TYPE_MASK_EMAIL = 0x04;

    public static void GetHeader(HeaderSet hs) {
        Log.v(TAG, "Dumping HeaderSet " + hs.toString());
        try {
            Log.v(TAG, "COUNT : " + hs.getHeader(HeaderSet.COUNT));
            Log.v(TAG, "NAME : " + hs.getHeader(HeaderSet.NAME));
            Log.v(TAG, "TYPE : " + hs.getHeader(HeaderSet.TYPE));
            Log.v(TAG, "LENGTH : " + hs.getHeader(HeaderSet.LENGTH));
            Log.v(TAG, "TIME_ISO_8601 : " + hs.getHeader(HeaderSet.TIME_ISO_8601));
            Log.v(TAG, "TIME_4_BYTE : " + hs.getHeader(HeaderSet.TIME_4_BYTE));
            Log.v(TAG, "DESCRIPTION : " + hs.getHeader(HeaderSet.DESCRIPTION));
            Log.v(TAG, "TARGET : " + hs.getHeader(HeaderSet.TARGET));
            Log.v(TAG, "HTTP : " + hs.getHeader(HeaderSet.HTTP));
            Log.v(TAG, "WHO : " + hs.getHeader(HeaderSet.WHO));
            Log.v(TAG, "OBJECT_CLASS : " + hs.getHeader(HeaderSet.OBJECT_CLASS));
            Log.v(TAG, "APPLICATION_PARAMETER : " + hs.getHeader(HeaderSet.APPLICATION_PARAMETER));
        } catch (IOException e) {
            Log.e(TAG, "dump HeaderSet error " + e);
        }
    }

}
