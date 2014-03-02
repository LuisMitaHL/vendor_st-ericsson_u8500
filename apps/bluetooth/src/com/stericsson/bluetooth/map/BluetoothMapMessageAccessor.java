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

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.ContactsContract.CommonDataKinds.Email;
import android.provider.ContactsContract.Contacts;
import android.provider.ContactsContract.PhoneLookup;
import android.telephony.*;
import android.text.format.Time;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;
import javax.obex.ResponseCodes;

/* This class provides access to the Sms/Mms ContentProviders */
public class BluetoothMapMessageAccessor {

    /**
     *Modifiable and should be updated at every connection
     * after reading specific values from UI settings */
    public static int msg_mode = (Constants.MAP_MODE_EMAIL_ON | Constants.MAP_MODE_SMS_ON);

    private static final String TAG = "BluetoothMapMessageAccessor";

    private static final String RECEPTION_STATUS_COMPLETE = "complete";

    private static final String YES_STRING = "yes";
    private static final String NO_STRING = "no";

    private static final String READ_STATUS_TRUE = "1";
    private static final String READ_STATUS_FALSE = "0";

    private static final String MESSAGE_TYPE_SMS_GSM = "SMS_GSM";     /* GSM short message */

    public static final String INBOX = "inbox";
    public static final String OUTBOX = "outbox";
    public static final String SENT = "sent";
    public static final String DELETED = "deleted";
    public static final String DRAFT = "draft";
    public static final String DRAFTS = "drafts";
    public static final String UNDELIVERED = "undelivered";
    public static final String FAILED = "failed";
    public static final String QUEUED = "queued";

    private static Uri ThreadstableSmsUri = Uri.parse(Constants.THREAD_TABLE_SMS_URI);
    private static Uri CanonicaladdressestableSmsUri = Uri.parse(Constants.CANONICAL_SMS_URI);

    private static final int MAX_SUBJECT_LEN = 256;

    public static final int MSG_HANDLE_LENGTH_HEXADECIMAL_REPRESENTATION = 16;

    private final int DELETED_THREAD_ID = -1;

    /**
     * Handle related data starts here
     * Base handles for each type of message
     * box
     */
    public static final long INBOX_BASE_MSG_HANDLE = ((long)0x000000f1 << 32);
    public static final long OUTBOX_BASE_MSG_HANDLE = ((long)0x000000f2 << 32);
    public static final long SENT_BASE_MSG_HANDLE = ((long)0x000000f3 << 32);
    public static final long DRAFT_BASE_MSG_HANDLE = ((long)0x000000f4 << 32);
    public static final long DELETED_BASE_MSG_HANDLE = ((long)0x000000f5 << 32);
    public static final int EMAIL_HDLR_CONSTANT = 200000;

    /**
     * Contains the end of Msg Handle range for each type
     * of message box, maintaining a safe gap of 0xff between
     * end of one box's message handle range and start of another's.
     */
    public static final long INBOX_MSG_HANDLE_END = ((long)0x000000f2 << 32) - 0xff;
    public static final long OUTBOX_MSG_HANDLE_END = ((long)0x000000f3 << 32) - 0xff;
    public static final long SENT_MSG_HANDLE_END = ((long)0x000000f4 << 32) - 0xff;
    public static final long DRAFT_MSG_HANDLE_END = ((long)0x000000f5 << 32) - 0xff;
    public static final long DELETED_MSG_HANDLE_END = ((long)0x000000f6 << 32) - 0xff;

    /**
     * Contains the current Message Handle for
     * each type of message box
     */
    private long mapCurrentInboxMessageHandle;
    private long mapCurrentOutboxMessageHandle;
    private long mapCurrentSentMessageHandle;
    private long mapCurrentDraftMessageHandle;
    private long mapCurrentDeletedMessageHandle;

    /* Contains the current Message handle in Long format */
    private long mapCurrentMessageHandle;

    private int mapCurrentMessageListingSize;

    /**
     * Contains for the last retrieved message store
     * whether there's an unread message.
     */
    private boolean isNewMessagePresent;

    private boolean saveToSentFlag;

    /**
     * Contains the last formed Message Listing Response's
     * messageListingSize application parameter
     */
    public int paramMessageListingSize;

    /**
     * Application parameter MSETime : Time in
     * YYYYMMDDTHHMMSS(+/-)hhmm format.
     */
    public String paramMSEtime;

    /* Application parameter NewMessage */

    public byte paramNewMessage;

    /**
     * mLastMessageHandleCreated contains msgHandleCreated for last
     * message pushed by PushMessage request, in HexString Format.
     */
    public String mLastMessageHandleCreated;

    public String pendingMessageNumber;

    public String pendingMessageBody;

    private StringBuffer mLastMessageListing = null;

    /**
     * This contains,facilitates storing Message Listing Object
     * as per the fields specified by the "x-bt/MAP-msg-listing"
     * DTD
     *
     */
    private class MessageListingDetails {
        public StringBuilder string_field_handle;    /*msgHandle in Hex String format */
        public StringBuilder idmapped_msg_handle;  /*msgHandle actually mapped for */
        /* real messages IDs */
        public StringBuilder subject;         /* CDATA #REQUIRED For Sms, first words of the Body will be used */
        public StringBuilder datetime;        /* CDATA #REQUIRED */
        public StringBuilder sender_name;             /*CDATA #IMPLIED */
        public StringBuilder sender_addressing;         /*CDATA #IMPLIED */
        public StringBuilder replyto_addressing;         /*CDATA #IMPLIED */
        public StringBuilder recipient_name;             /*CDATA #IMPLIED */
        public StringBuilder recipient_addressing;     /* CDATA #REQUIRED */
        public StringBuilder type;                     /* CDATA #REQUIRED */
        public StringBuilder size;                     /* CDATA #REQUIRED */
        public StringBuilder text;                     /*(yes|no) "no" */
        public StringBuilder reception_status;     /* CDATA #REQUIRED */
        public StringBuilder attachment_size;     /* CDATA #REQUIRED */
        public StringBuilder priority;         /*(yes|no) "no" */
        public StringBuilder read;             /*(yes|no) "no" */
        public StringBuilder sent;             /*(yes|no) "no" */
        public StringBuilder is_protected;         /*(yes|no) "no" */

        MessageListingDetails() {
            /* Initialize all the yes/no field to default values */
            this.text = new StringBuilder();
            this.priority = new StringBuilder();
            this.read = new StringBuilder();
            this.sent = new StringBuilder();
            this.is_protected = new StringBuilder();
            this.recipient_addressing = new StringBuilder();
            this.recipient_addressing.append("");
            this.replyto_addressing = new StringBuilder();
            this.replyto_addressing.append("");

            this.string_field_handle = new StringBuilder();
            this.idmapped_msg_handle = new StringBuilder();
            this.subject = new StringBuilder();
            this.datetime = new StringBuilder();
            this.sender_name = new StringBuilder();
            this.sender_addressing = new StringBuilder();
            this.recipient_name = new StringBuilder();
            this.type = new StringBuilder();
            this.size = new StringBuilder();
            this.reception_status = new StringBuilder();
            this.attachment_size = new StringBuilder();
        }

        public void clearMessageDetails() {
            this.text.delete(0, this.text.length());
            this.priority.delete(0, this.priority.length());
            this.read.delete(0, this.read.length());
            this.sent.delete(0, this.sent.length());
            this.is_protected.delete(0, this.is_protected.length());
            this.recipient_addressing.delete(0, recipient_addressing.length());
            this.replyto_addressing.delete(0, this.replyto_addressing.length());

            this.string_field_handle.delete(0, this.string_field_handle.length());
            this.idmapped_msg_handle.delete(0, this.idmapped_msg_handle.length());
            this.subject.delete(0, this.subject.length());
            this.datetime.delete(0, this.datetime.length());
            this.sender_name.delete(0, this.sender_name.length());
            this.sender_addressing.delete(0, this.sender_addressing.length());
            this.recipient_name.delete(0, this.recipient_name.length());
            this.type.delete(0, this.type.length());
            this.size.delete(0, this.size.length());
            this.reception_status.delete(0, this.reception_status.length());
            this.attachment_size.delete(0, this.attachment_size.length());
        }
    };

    public class BluetoothMasMessageRsp {
        public byte fractionDeliver = 0;
        public File file = null;
        public int rsp = ResponseCodes.OBEX_HTTP_OK;
        boolean success = false;
        StringBuilder bMsg = null;

        BluetoothMasMessageRsp() {
            bMsg = new StringBuilder();
        }
    }

    /**
     * To cater a messagelisting request
     * this class allows storage of the number
     * of types of message of each type i.e. email
     * and sms subject to a limit on total number of
     * messages in the message list object */
    public class ListSizesCouplet {
        private int smsListSize;
        private int emailListSize;
        private int totalListSize;

        ListSizesCouplet(int sms, int email, int maxlistcount) {
            smsListSize = sms;
            emailListSize = email;
            totalListSize = maxlistcount;
        }

        public void setListSizes(int sms, int email, int maxlistcount) {
            smsListSize = sms;
            emailListSize = email;
            totalListSize = maxlistcount;
        }

        private void calculateListSizes() {
            if (smsListSize == 0) {
                if (emailListSize > totalListSize) {
                    emailListSize = totalListSize;
                    return;
                }
            }
            if (emailListSize == 0) {
                if (smsListSize > totalListSize) {
                    smsListSize = totalListSize;
                    return;
                }
            }
            if (totalListSize <= 0) {
                smsListSize = 0;
                emailListSize = 0;
                return;
            }

            int i = 0, j = 0, count = 0;

            boolean smsFinished = false;
            boolean emailFinished = false;
            do {
                if ((smsFinished == true) && (emailFinished == true)) {
                    break;
                }
                if (i < smsListSize) {
                    count++;
                    i++;
                } else {
                    smsFinished = true;
                }
                if (count == totalListSize) {
                    break;
                }
                if (j < emailListSize) {
                    count++;
                    j++;
                } else {
                    emailFinished = true;
                }
                if (count == totalListSize) {
                    break;
                }
            } while (count <= totalListSize);

            smsListSize = i;
            emailListSize = j;
        }
    };

    /**
     * Contains DELETED message details for current session.
     */
    private MessageListingDetails [] mLastMessageList;

    /**
     * Contains the last bMessage object formed
     */
    public StringBuilder mLastbMessageObject = null;

    /**
     * Contains the current context
     */
    private Context mContext;

    BluetoothMapObexServerSession thisObexSession;

    private int mThisMasInstance;

    /**
     * Constructor - Resetting all the message listing
     * related information.
     */
    BluetoothMapMessageAccessor(Context paramContext, BluetoothMapObexServerSession obexSession, int masInstanceType) {
        mapCurrentMessageHandle = 0x00;
        paramMessageListingSize = 0x00;
        paramMSEtime = null;
        paramNewMessage = 0x00;
        mContext = paramContext;
        isNewMessagePresent = false;
        mapCurrentMessageListingSize = 0;
        thisObexSession = obexSession;

        mapCurrentInboxMessageHandle = INBOX_BASE_MSG_HANDLE;
        mapCurrentOutboxMessageHandle = OUTBOX_BASE_MSG_HANDLE;
        mapCurrentSentMessageHandle = SENT_BASE_MSG_HANDLE;
        mapCurrentDraftMessageHandle = DRAFT_BASE_MSG_HANDLE;
        mapCurrentDeletedMessageHandle = DELETED_BASE_MSG_HANDLE;

        pendingMessageNumber = null;
        pendingMessageBody = null;
        saveToSentFlag = false;
        mThisMasInstance = masInstanceType;

        mLastbMessageObject = new StringBuilder();

        mLastMessageListing = new StringBuffer();
    }

    /**
     * Generates a new msg handle for based upon the
     * message box provided.
     *
     * @return void
     */
    private boolean generateMessagehandle(int MessageBox) {
        boolean generationOk = true;
        mapCurrentMessageHandle++;

        Log.i(TAG, "generateMessagehandle-folder : " + MessageBox);

        switch (MessageBox) {
        case Constants.MAP_FOLDER_INBOX:
            mapCurrentInboxMessageHandle ++;
            if (mapCurrentInboxMessageHandle >= INBOX_MSG_HANDLE_END) {
                generationOk = false;
            }
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            mapCurrentOutboxMessageHandle ++;
            if (mapCurrentOutboxMessageHandle >= OUTBOX_MSG_HANDLE_END) {
                generationOk = false;
            }
            break;
        case Constants.MAP_FOLDER_SENT:
            mapCurrentSentMessageHandle ++;
            if (mapCurrentSentMessageHandle >= SENT_MSG_HANDLE_END) {
                generationOk = false;
            }
            break;
        case Constants.MAP_FOLDER_DRAFT:
            mapCurrentDraftMessageHandle ++;
            if (mapCurrentDraftMessageHandle >= DRAFT_MSG_HANDLE_END) {
                generationOk = false;
            }
            break;

        case Constants.MAP_FOLDER_DELETED:
            mapCurrentDeletedMessageHandle ++;
            if (mapCurrentDeletedMessageHandle >= DELETED_MSG_HANDLE_END) {
                generationOk = false;
            }
            break;
        }

        return generationOk;
    }

    /**
     * resets a currentMessageHandle to its
     * base value as dictated by its message box type
     *
     * @return void
     */
    public void resetMessageHandle(int MessageBox) {

        switch (MessageBox) {
        case Constants.MAP_FOLDER_INBOX:
            mapCurrentInboxMessageHandle = INBOX_BASE_MSG_HANDLE;
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            mapCurrentOutboxMessageHandle = OUTBOX_BASE_MSG_HANDLE;
            break;
        case Constants.MAP_FOLDER_SENT:
            mapCurrentSentMessageHandle = SENT_BASE_MSG_HANDLE;
            break;
        case Constants.MAP_FOLDER_DRAFT:
            mapCurrentDraftMessageHandle = DRAFT_BASE_MSG_HANDLE;
            break;
        case Constants.MAP_FOLDER_DELETED:
            mapCurrentDeletedMessageHandle = DELETED_BASE_MSG_HANDLE;
            break;
        }
    }

    /**
     * This method resets global message listing object
     * used by ObexServerSession to respond to
     * GetMessageListing request by MCE.
     */
    public void resetGenericMessageList() {
        if (mLastMessageListing != null) {
            mLastMessageListing.delete(0, mLastMessageListing.length());
        }
    }

    public StringBuffer getLastGeneratedMessageList() {
        return mLastMessageListing;
    }

    /**
     * This method creates message listing objects after
     * retrieving list of messages from given path within
     * the message store.
     * @param folderPath
     * @param appParam
     * @return
     */
    public void createMessageListingResponse(int folderPath, BluetoothMapApplicationParameters appParam) {

        List <MessageListingDetails> messagesList = null;

        int iterFolderList = 0;

        /* Retrieve the messages details */
        messagesList = retrieveMessageDetailsListinFolder(appParam, folderPath);

        mLastMessageListing.append("");

        mLastMessageListing.append("<MAP-msg-listing version = \"1.0\">\n");

        /*
         * Create this message listing only if messagesList is not
         * null, for null cases the empty folder listing shall still
         * go to the remote devices */
        if (messagesList != null) {
            for (iterFolderList = 0; iterFolderList < messagesList.size(); iterFolderList ++) {
                mLastMessageListing.append("<msg ");
                mLastMessageListing.append("handle = ");

                mLastMessageListing.append("\"" +
                        messagesList.get(iterFolderList).string_field_handle + "\" ");

                if ((appParam.parameterMask & Constants.BIT_SUBJECT) != 0) {
                    mLastMessageListing.append("subject = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).subject + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_DATETIME) != 0) {
                    mLastMessageListing.append("datetime = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).datetime + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_SENDER_NAME) != 0) {
                    mLastMessageListing.append("sender_name = ");
                    if (messagesList.get(iterFolderList).sender_name.length() == 0 )
                        mLastMessageListing.append("\"" +
                                messagesList.get(iterFolderList).replyto_addressing + "\" ");
                    else
                        mLastMessageListing.append("\"" +
                                messagesList.get(iterFolderList).sender_name + "\" ");
                }

                /* replyto_addressing added after the simulator testing */
                if ((appParam.parameterMask & Constants.BIT_REPLYTO_ADDRESSING) != 0) {

                    mLastMessageListing = mLastMessageListing.append("replyto_addressing = ");
                    mLastMessageListing = mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).replyto_addressing + "\" ");
                }
                if ((appParam.parameterMask & Constants.BIT_SENDER_ADDRESSING) != 0) {
                    mLastMessageListing.append("sender_addressing = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).sender_addressing + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_RECIPIENT_NAME) != 0) {
                    mLastMessageListing.append("recipient_name = ");

                    if (messagesList.get(iterFolderList).recipient_name.length() == 0 )
                        mLastMessageListing.append("\"" +
                                messagesList.get(iterFolderList).recipient_addressing + "\" ");
                    else
                        mLastMessageListing.append("\"" +
                                messagesList.get(iterFolderList).recipient_name + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_RECIPIENT_ADDRESSING) != 0) {
                    mLastMessageListing.append("recipient_addressing = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).recipient_addressing + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_TYPE) != 0) {
                    mLastMessageListing.append("type = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).type + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_SIZE) != 0) {
                    mLastMessageListing.append("size = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).size + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_READ) != 0) {
                    mLastMessageListing.append("read = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).read + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_TEXT) != 0) {
                    mLastMessageListing.append("text = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).text + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_RECEPTION_STATUS) != 0) {
                    mLastMessageListing.append("reception_status = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).reception_status + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_ATTACHMENT_SIZE) != 0) {
                    mLastMessageListing.append("attachment_size = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).attachment_size + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_PRIORITY) != 0) {
                    mLastMessageListing.append("priority = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).priority + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_SENT) != 0) {
                    mLastMessageListing.append("sent = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).sent + "\" ");
                }

                if ((appParam.parameterMask & Constants.BIT_PROTECTED) != 0) {
                    mLastMessageListing.append("protected = ");
                    mLastMessageListing.append("\"" +
                            messagesList.get(iterFolderList).is_protected + "\" ");
                }

                mLastMessageListing.append("/>\n");

            }
            mLastMessageListing.append("</MAP-msg-listing>");
        } else {
            Log.i(TAG, "createMessageListingResponse: Empty message list was retrieved");
            mLastMessageListing.append("</MAP-msg-listing>");
        }

        /* Free the MessageListingDetails List */
        if (messagesList != null) {
            /* Clear each of the objects inside it */
            for (int i = 0; i < messagesList.size(); i++) {
                messagesList.get(i).clearMessageDetails();
            }
            messagesList.clear();
        }
    }

    /**
     * This method retrieves all the messages located in
     * the specified folder, and fills-in those details
     * in the MessageListingDetails type array.
     * @return
     */
    List <MessageListingDetails> retrieveMessageDetailsListinFolder(BluetoothMapApplicationParameters appParam, int folderPath) {
        List <MessageListingDetails> msgListToReturn = null;
        List <MessageListingDetails> smsMsgList = null;
        List <MessageListingDetails> emailMsgList = null;

        Log.i(TAG, "Entering retrieveMessageDetailsListinFolder " + folderPath);

        /* Compare for SMS_GSM or SMS_CDMA filter
         * 000xxx1 = "SMS_GSM"
         * 0000xx1x = "SMS_CDMA"
         * 0000x1xx = "EMAIL"
         * 00001xxx = "MMS"
        */
        if (mThisMasInstance == Constants.SMS_MASINSTANCE) {

            switch (folderPath) {
            case Constants.MAP_FOLDER_INBOX:
            case Constants.MAP_FOLDER_OUTBOX:
            case Constants.MAP_FOLDER_SENT:
            case Constants.MAP_FOLDER_DRAFT:
            case Constants.MAP_FOLDER_DELETED:
                smsMsgList = retrieveSmsMessageDetails(appParam, folderPath);
                break;

            case Constants.MAP_FOLDER_ROOT:
            case Constants.MAP_FOLDER_TELECOM:
            case Constants.MAP_FOLDER_MSG:
                smsMsgList = null;
                break;
            default:
                smsMsgList = null;
                break;
            }

            msgListToReturn = smsMsgList;
        }

        if (mThisMasInstance == Constants.EMAIL_MASINSTANCE) {
            /* Apply the Filter filterMessageType */

            /* fetch email messages list */
            switch (folderPath) {
                case Constants.MAP_FOLDER_INBOX:
                case Constants.MAP_FOLDER_OUTBOX:
                case Constants.MAP_FOLDER_SENT:
                case Constants.MAP_FOLDER_DRAFT:
                case Constants.MAP_FOLDER_DELETED:
                    emailMsgList = fetchEmailMessagesList(appParam, folderPath);
                    break;
                case Constants.MAP_FOLDER_ROOT:
                case Constants.MAP_FOLDER_TELECOM:
                case Constants.MAP_FOLDER_MSG:
                    emailMsgList = null;
                    break;
                default:
                    emailMsgList = null;
                    break;
            }

            if (emailMsgList != null) {
                int startOffset, maxNum, currSize, finalSize, lastOffset;
                mapCurrentMessageListingSize += emailMsgList.size();
                startOffset = appParam.listStartOffset;
                maxNum = appParam.maxListCount;
                currSize = emailMsgList.size();

                if ((startOffset >= 0 ) && (startOffset <= currSize) && (maxNum > 0)) {
                    finalSize = currSize - startOffset;
                    if (finalSize > maxNum) {
                        finalSize = maxNum;
                    }
                    lastOffset = startOffset + finalSize - 1;
                    /* trim the first listStartOffset items */
                    emailMsgList = emailMsgList.subList(startOffset, lastOffset + 1);
                    mapCurrentMessageListingSize = emailMsgList.size();
                } else if (startOffset >= currSize) {
                    emailMsgList = null;
                    mapCurrentMessageListingSize = 0;
                }
            }

            msgListToReturn = emailMsgList;
        }

        /**
         * Append sms and email messages lists subject to a maximum of
         * number of entries in the complete to be maxListCount
         * msgListToReturn = mergeMessagesList(smsMsgList, emailMsgList, appParam.maxListCount);
         */

        dumpMessageList(msgListToReturn);
        return msgListToReturn;
    }

    private void dumpMessageList(List <MessageListingDetails> msgListToReturn) {
        if (msgListToReturn != null) {
            Log.d(TAG, "dumpMessageList size = " + msgListToReturn.size());
            for (int i = 0; i < msgListToReturn.size(); i ++) {
                Log.d(TAG, "Handle = " + msgListToReturn.get(i).string_field_handle);
                Log.d(TAG, "date time = " + msgListToReturn.get(i).datetime);
                Log.d(TAG, "Subject = " + msgListToReturn.get(i).subject);
            }
        }
    }

    public int calculateMessageListingSize(BluetoothMapApplicationParameters appParam, int folder) {
        int numOfMessages = 0;
        Cursor cursorSms = null;
        String whereClause = BluetoothMapQueryUtils.getConditionStringSms(folder, appParam);

        try {
            cursorSms = mContext.getContentResolver().query(Constants.SMSURI, null, whereClause,
                    null, "date desc");

            if (cursorSms != null) {
                numOfMessages = cursorSms.getCount();
            }
        } finally {
            cursorSms.close();
        }
        mapCurrentMessageListingSize = numOfMessages;
        return numOfMessages;
    }

    /**
     * This method calculates the number of email
     * that would be retrieved for the specified filtering
     * criteria. This corresponds to the cases when the MCE
     * has specified the maxListCount to be Zero.
     *
     * @param appParam
     * @param folder
     * @return
     */
    public int calculateEmailListingSize(BluetoothMapApplicationParameters appParam, int folder) {
        int numOfMessages = 0;
        List <MessageListingDetails> emailMsgList = null;

        emailMsgList = fetchEmailMessagesList(appParam, folder);

        /* in case null Msg list is retrieved, just return
         * zero as the message listing size */
        if (emailMsgList == null) {
            return 0;
        }

        numOfMessages = emailMsgList.size();
        mapCurrentMessageListingSize = numOfMessages;

        /* Free the MessageListingDetails List */
        if (emailMsgList != null) {
            /* Clear each of the objects inside it */
            for (int i = 0; i < emailMsgList.size(); i++) {
                emailMsgList.get(i).clearMessageDetails();
            }
            emailMsgList.clear();
        }
        emailMsgList = null;
        return numOfMessages;
    }

    /**
     * Returns all the message in the INBOX messages folder
     * @return MessageListingDetails, is null if the message store is empty
     */
    private List <MessageListingDetails> retrieveSmsMessageDetails(BluetoothMapApplicationParameters appParam, int folder) {
        List <MessageListingDetails> msgListToReturn = null;
        List <MessageListingDetails> newMsgList = null;
        Cursor cursorSms = null;
        int numOfMessages = 0, indexAtMessages = 0;
        String subject = "", datetime = "", smsBody;    /* datetime in YYYYMMDDTHHMMSS format */
        Long lnDateTime;
        int dateTimeColumnIndex, smsBodyColumnIndex, idColumnIndex, readColumnIndex;
        int typeIndex, addressIndex,threadIndex;

        StringTokenizer st;

        String whereClause = BluetoothMapQueryUtils.getConditionStringSms(folder, appParam);

        try {
            cursorSms = mContext.getContentResolver().query(Constants.SMSURI, null, whereClause,
                    null, "date desc");

            if (cursorSms != null) {
                numOfMessages = cursorSms.getCount();

                if (cursorSms.moveToFirst() == false) {
                    /* cursorSms is empty, return null */
                    msgListToReturn = null;
                } else {
                    Log.i(TAG, "Allocate MessageListing Details : " + numOfMessages);

                    /* Initialize msgListToReturn */
                    msgListToReturn = new ArrayList<MessageListingDetails>(numOfMessages);
                    newMsgList = msgListToReturn;
                    MessageListingDetails localMessageListItem = null;

                    /* Read the entire set of rows obtained */
                    indexAtMessages = 0;

                    /* Imp. part of logic reset the current InboxMessageHandle to base value */
                    resetMessageHandle(Constants.MAP_FOLDER_INBOX);

                    do {
                        int msgType;
                        localMessageListItem = new MessageListingDetails();

                        /* Extract data from cursor */
                        smsBodyColumnIndex = cursorSms.getColumnIndex("body");
                        dateTimeColumnIndex = cursorSms.getColumnIndex("date");
                        idColumnIndex = cursorSms.getColumnIndex("_id");
                        readColumnIndex = cursorSms.getColumnIndex("read");
                        typeIndex = cursorSms.getColumnIndex("type");
                        addressIndex = cursorSms.getColumnIndex("address");
                        threadIndex = cursorSms.getColumnIndex("thread_id");
                        String threadId = cursorSms.getString(threadIndex);

                        /* Extract subject from body, use first three words */
                        smsBody = cursorSms.getString(smsBodyColumnIndex);

                        int count = 0;
                        st = new StringTokenizer(smsBody);
                        subject = "";
                        while (st.hasMoreTokens() && (count++ < 3)) {
                            subject = subject.concat(st.nextToken());
                            subject = subject.concat(" ");
                        }

                        StringBuilder tmp = new StringBuilder(subject);
                        removeSpecialCharsFromSubject(tmp);
                        /* apply max subject length parameter */
                        if (appParam.subjectLength < tmp.length()) {
                            tmp.setLength(appParam.subjectLength);
                        }

                        resizeToMaxSubjectLen(tmp);

                        tmp.trimToSize();
                        subject = tmp.toString();

                        /* Extract date time and convert to YYYYMMDDTHHMMSS format */
                        lnDateTime = cursorSms.getLong(dateTimeColumnIndex);
                        datetime = converToYYYYMMDDTHHMMSSFormat(lnDateTime);

                        /* put the first record in it */

                        localMessageListItem.string_field_handle.append
                        (cursorSms.getString(idColumnIndex));

                        localMessageListItem.subject.append(subject);
                        localMessageListItem.datetime.append(datetime);

                        /* Get the type of message */
                        msgType = cursorSms.getInt(typeIndex);

                        /* Get sender_addressing */
                        String senderAddressing = null;
                        if (isOutgoingSMSMessage(msgType) == true) {
                            senderAddressing = getOwnerNumber();
                        } else {
                            senderAddressing = cursorSms.getString(addressIndex);
                        }
                        localMessageListItem.sender_addressing.append(senderAddressing);

                        /* Get sender name */
                        String senderName = null;
                        if (isOutgoingSMSMessage(msgType) == true) {
                            senderName = getOwnerName();
                        } else {
                            senderName = getContactName(cursorSms
                                    .getString(addressIndex));
                        }
                        localMessageListItem.sender_name.append(senderName);

                        /* Get Recipient name */
                        String recipientName = null;
                        if (folder == Constants.MAP_FOLDER_DRAFT) {
                            if (isOutgoingSMSMessage(msgType) == false) {
                                recipientName = getOwnerName();
                            } else {
                                recipientName = getContactName(getRecipientAddressForDrafts(threadId));
                            }
                            if (recipientName.isEmpty() != true)
                                localMessageListItem.recipient_name.append(recipientName);

                        } else {
                            if (isOutgoingSMSMessage(msgType) == false) {
                                recipientName = getOwnerName();
                            } else {
                                recipientName = getContactName(cursorSms
                                        .getString(addressIndex));
                            }
                            if (recipientName.isEmpty() != true)
                                localMessageListItem.recipient_name.append(recipientName);
                        }
                        /* Get recipient addressing */
                        String recipientAddressing = null;
                        if (folder == Constants.MAP_FOLDER_DRAFT) {
                            if (isOutgoingSMSMessage(msgType) == false) {
                                recipientAddressing = getOwnerNumber();
                            } else {
                                recipientAddressing = getRecipientAddressForDrafts(threadId);
                            }
                            localMessageListItem.recipient_addressing.append(recipientAddressing);
                        } else {
                            if (isOutgoingSMSMessage(msgType) == false) {
                                recipientAddressing = getOwnerNumber();
                            } else {
                                recipientAddressing = cursorSms
                                .getString(addressIndex);
                            }
                            localMessageListItem.recipient_addressing.append(recipientAddressing);
                        }

                        /* Apply the filterRecipient and filterOriginator filters now */
                        if ((appParam.filterOriginator != null) && (appParam.filterOriginator.isEmpty() == false)) {
                            if ((localMessageListItem.sender_addressing.indexOf(appParam.filterOriginator) == -1) &&
                                    (localMessageListItem.sender_name.indexOf(appParam.filterOriginator) == -1)) {
                                continue;
                            }
                        }

                        if ((appParam.filterRecipient != null) && (appParam.filterRecipient.isEmpty() == false)) {
                            if ((localMessageListItem.recipient_addressing.indexOf(appParam.filterRecipient) == -1) &&
                                    (localMessageListItem.recipient_name.indexOf(appParam.filterRecipient) == -1)) {
                                continue;
                            }
                        }

                        /* Get reply to addressing only for Incoming messages */
                        String replytoAddressing = null;
                        if (isOutgoingSMSMessage(msgType) == true) {
                            replytoAddressing = "";
                        } else {
                            replytoAddressing = cursorSms.getString(addressIndex);
                        }
                        localMessageListItem.replyto_addressing.append(replytoAddressing);

                        localMessageListItem.type.append(MESSAGE_TYPE_SMS_GSM);
                        localMessageListItem.size.append(Integer.toString(smsBody.length()));
                        localMessageListItem.reception_status.append(RECEPTION_STATUS_COMPLETE);
                        localMessageListItem.text.append(YES_STRING);
                        localMessageListItem.attachment_size.append("0");

                        if (cursorSms.getString(readColumnIndex).equalsIgnoreCase("1")) {
                            localMessageListItem.read.append(YES_STRING);
                            isNewMessagePresent = true;
                        } else {
                            localMessageListItem.read.append(NO_STRING);
                        }

                        if (isSentSMSMessage(msgType)) {
                            localMessageListItem.sent.append(YES_STRING);
                        } else {
                            localMessageListItem.sent.append(NO_STRING);
                        }
                        /* Add this message list item to the complete list */
                        msgListToReturn.add(indexAtMessages, localMessageListItem);

                        /* Increment the index now */
                        indexAtMessages++;
                    } while ((cursorSms.moveToNext() == true) && (indexAtMessages < numOfMessages));

                    /**
                     *Trim the message listing according to listStartOffset
                     *Trim the folderList according to the appParam
                     */
                    int startOffset, maxNum, currSize, finalSize, lastOffset;
                    mapCurrentMessageListingSize = msgListToReturn.size();
                    startOffset = appParam.listStartOffset;
                    maxNum = appParam.maxListCount;
                    currSize = msgListToReturn.size();

                    if ((startOffset >= 0 ) && (startOffset <= currSize) && (maxNum > 0)) {
                        finalSize = currSize - startOffset;
                        if (finalSize > maxNum) {
                            finalSize = maxNum;
                        }
                        lastOffset = startOffset + finalSize - 1;
                        /* trim the first listStartOffset items */
                        newMsgList = msgListToReturn.subList(startOffset, lastOffset + 1);
                        mapCurrentMessageListingSize = newMsgList.size();
                    } else if (startOffset > currSize) {
                        if ((msgListToReturn != null) && (msgListToReturn.size() > 0)) {
                            for (int ind = 0; ind < msgListToReturn.size(); ind++) {
                                msgListToReturn.get(ind).clearMessageDetails();
                                msgListToReturn.remove(ind);
                            }
                        }
                        newMsgList = null;
                        mapCurrentMessageListingSize = 0;
                    }
                }
            } else {
                Log.e(TAG, "Query on ContentProvider yielded no results");
            }
        } finally {
            cursorSms.close();
        }

        dumpMessageList(newMsgList);
        return newMsgList;
    }

    /**
     * Returns Recipient Address
     * Written Specifically for DRAFT SMS
     */
    public String getRecipientAddressForDrafts(String threadId) {

        Cursor cursorthreadtableSms = null;
        Cursor cursorCanonicalSms = null;
        String Address = null;
        int threadid = Integer.parseInt(threadId);
        String whereClause1 = "_id = "+ threadid;
        int receipientid = 0;
        try {
            cursorthreadtableSms = mContext.getContentResolver().query(ThreadstableSmsUri, null, whereClause1,
                    null,null);
            if (cursorthreadtableSms != null) {
                cursorthreadtableSms.moveToFirst();
                int ReceipientIdindex = cursorthreadtableSms.getColumnIndex("recipient_ids");
                String ReceipientId = cursorthreadtableSms.getString(ReceipientIdindex);
                receipientid = Integer.parseInt(ReceipientId);
            } else {
                Log.e(TAG, "cursorthreadtableSms is null ");
            }
        } finally {
            if (cursorthreadtableSms != null)
                cursorthreadtableSms.close();
        }
        String whereClause2 = "";
        whereClause2 = " _id = " + receipientid;
        try {
            cursorCanonicalSms = mContext.getContentResolver().query(CanonicaladdressestableSmsUri, null, whereClause2,
                    null,null);
            if (cursorCanonicalSms != null) {
                cursorCanonicalSms.moveToFirst();
                int CanonicaladdessIndex = cursorCanonicalSms.getColumnIndex("address");
                Address = cursorCanonicalSms.getString(CanonicaladdessIndex);
            } else {
            Log.e(TAG, "cursorCanonicalSms is null");
            }
        } finally {
            if (cursorCanonicalSms != null)
                cursorCanonicalSms.close();
        }
        return Address;
    }

    /**
     * Get the message listing size of the last retrieved message
     * listing request, it indicates the number of folders in it.
     *
     * @return
     */
    public int getMessageListingSize() {
        return mapCurrentMessageListingSize;
    }

    /**
     * Tells whether there's an unread message or not.
     * @return boolean
     */
    public boolean isNewMessagePresent() {
        return isNewMessagePresent;
    }

    /**
     * Return current time basis of MSE in the
     * format YYYYMMDDTHHMMSS+0100
     * @return
     */
    public String getLocalTimeBasisOfMSE() {
        Time time = new Time();
        time.setToNow();

        String time3339 = time.format3339(false);
        int timeStrLength = time3339.length();

        String datetimeStr = time.toString().substring(0, 15) +
        time3339.substring(timeStrLength - 6, timeStrLength - 3) +
        time3339.substring(timeStrLength - 2, timeStrLength);

        return datetimeStr.substring(0, 15);
    }

    /**
     * Return Message corresponding to gievn handle number
     * @return
     */
    public File createGetMessageResponse(String msgHandle, int currFolder, boolean charsetUtf) {
        boolean success = false;
        File fileInfo = null;

        Log.i(TAG, "Entering createGetMessageResponse");
        /* Check msgHandle for validity */
        if (msgHandle != null) {
            if (mThisMasInstance == Constants.SMS_MASINSTANCE) {
                if (charsetUtf == true) {
                    /* create bMessage for utf8 format */
                    success = createBmsgObjectWithVcard(msgHandle, currFolder, 1);
                } else {
                    /* create bMessage for native format */
                    success = createBmsgObjectWithVcard(msgHandle, currFolder, 1);
                }
            } else if (mThisMasInstance == Constants.EMAIL_MASINSTANCE) {
                if (isHandleFromEmailRepository(msgHandle) && (success == false)) {
                    /* Get the bMessage object for email */
                    BluetoothMasMessageRsp resp = null;
                    resp = createObjectbMessageForEmails(msgHandle);
                    mLastbMessageObject.delete(0, mLastbMessageObject.length());
                    if (resp == null) {
                        success = false;
                    } else {
                        mLastbMessageObject.append(resp.bMsg);
                        success = true;
                    }
                } else {
                    Log.i(TAG, "createGetMessageResponse : No message found for corresponding handle");
                    success = false;
                }
            }
        } else {
            Log.i(TAG, "createGetMessageResponse : Invalid handle supplied");
            success = false;
        }

        /* Write the mLastbMessageObject to file and free the object */
        if (success == true) {
            fileInfo = writeStringBufferToFile(mLastbMessageObject);
            if (mLastbMessageObject != null) {
                mLastbMessageObject.delete(0, mLastbMessageObject.length());
            }
        } else {
            fileInfo = null;
            Log.i(TAG, "createGetMessageResponse : Invalid handle supplied");
        }

        return fileInfo;
    }

    public File writeStringBufferToFile(StringBuilder strBuffer) {
        File fileInfo = null;

        if (strBuffer != null && (strBuffer.length() > 0)) {
            final String FILENAME = "bMessage.txt";
            FileOutputStream bos = null;
            File file = new File(mContext.getFilesDir() + "/" + FILENAME);
            file.delete();

            try {
                bos = mContext.openFileOutput(FILENAME, Context.MODE_PRIVATE);
                bos.write(strBuffer.toString().getBytes());
                bos.flush();
                bos.close();
            } catch (FileNotFoundException e) {
                Log.e(TAG, "FileNotFoundException " + e);
            } catch (IOException e) {
                Log.e(TAG, "IOException " + e);
            }

            File fileR = new File(mContext.getFilesDir() + "/" + FILENAME);
            if (fileR.exists() == true) {
                fileInfo = fileR;
            }
        }

        return fileInfo;
    }

    /**
     * This method creates a bMessage Object from the given
     * pointer (Cursor) to a message
     * @param messageCursor
     * @return String
     */
    boolean createObjectbMessageFromCursorUtf8Format(int indexMessageList, int currFolder) {
        String bMessage = "", msgBody;
        int msgBodyLen;
        boolean success = true;

        Log.i(TAG, "Entering createObjectbMessageFromCursorUtf8Format()");

        if (indexMessageList != -1) {
            TelephonyManager telephonyManager = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);

            /* Version */
            bMessage = bMessage.concat("VERSION:1.0\n");
            bMessage = bMessage.concat("STATUS:");

            /* Get status read/unread */
            if (mLastMessageList[indexMessageList].read.toString().compareTo(YES_STRING) == 0) {
                bMessage = bMessage.concat("READ\nTYPE:");
            } else {
                bMessage = bMessage.concat("UNREAD\nTYPE:");
            }

            /* Get the type of current system type CDMA/GSM */
            switch (telephonyManager.getPhoneType()) {
            case TelephonyManager.PHONE_TYPE_GSM:
                bMessage = bMessage.concat("SMS_GSM\n");
                break;
            case TelephonyManager.PHONE_TYPE_CDMA:
                bMessage = bMessage.concat("SMS_CDMA\n");
            }

            /* Folder name */
            bMessage = bMessage.concat("FOLDER:");
            bMessage = bMessage.concat(BluetoothMapFolderAccessor.mapGivenFolderToString(currFolder).toUpperCase());
            bMessage = bMessage.concat("\n");

            /* Include vCard */
            bMessage = bMessage.concat("BEGIN:VCARD\n");
            bMessage = bMessage.concat("VERSION:2.1\n");
            bMessage = bMessage.concat("N:");            /*doesn't include name */
            bMessage = bMessage.concat(mLastMessageList[indexMessageList].sender_name + "\n");
            bMessage = bMessage.concat("TEL:");
            /* Telephone number */
            bMessage = bMessage.concat(mLastMessageList[indexMessageList].sender_addressing.toString());
            bMessage = bMessage.concat("\n");
            bMessage = bMessage.concat("END:VCARD\n");

            /* use 7-bit for now */
            bMessage = bMessage.concat("BEGIN:BENV\n");
            bMessage = bMessage.concat("BEGIN:BBODY\n");
            bMessage = bMessage.concat("ENCODING:G-7BIT\n");
            bMessage = bMessage.concat("BEGIN:MSG\n");
            bMessage = bMessage.concat("END:MSG\n");
            bMessage = bMessage.concat("END:BBODY\n");
            bMessage = bMessage.concat("END:BENV\n");
            bMessage = bMessage.concat("END:BMSG\n");

            Log.i(TAG, "The formed bMessage document is " + bMessage);
        } else {
            success = false;
        }

        mLastbMessageObject.delete(0, mLastbMessageObject.length());
        mLastbMessageObject.append(bMessage);

        return success;
    }

    /**
     *
     * @param msgHandle
     */
    BluetoothMasMessageRsp createObjectbMessageForEmails(String msgHandle) {
        int emailMsgID = 0;
        EmailUtils eu = new EmailUtils();
        BluetoothMasMessageRsp rsp = new BluetoothMasMessageRsp();

        if ((msgHandle != null) && (msgHandle.length() != 0)) {
            /* Email message */

            emailMsgID = (Integer.valueOf(msgHandle) - EMAIL_HDLR_CONSTANT);
            StringBuilder str = new StringBuilder();
            String returnStr = null;
            returnStr = eu.bldEmailBmsg(emailMsgID, mContext);
            str.append(eu.bldEmailBmsg(emailMsgID, mContext));

            rsp.bMsg.append(str);
            if (returnStr == null)
                rsp = null;

            /* Clear the message string */
            str.delete(0, str.length());
        }
        return rsp;
    }

    /**
     *
     * @param msgHandle
     * @param statusInd
     * @param statusValue
     * @return
     */
    public boolean setMessageStatus(String msgHandle, int statusInd, int statusValue) {
        boolean success = true;

        switch (statusInd) {
        case Constants.STATUS_IND_READ_STATUS:
            success = setMessageReadStatus(msgHandle, statusValue);
            break;
        case Constants.STATUS_IND_DELETE_STATUS:
            success = setMessageDeleteStatus(msgHandle, statusValue);
            break;
        default:
            success = false;
        }

        return success;
    }

    /**
     *
     * @param msgHandle
     * @param statusValue
     * @return
     */
    private boolean setMessageReadStatus(String msgHandle, int statusValue) {
        Uri thisMsgUri = null;
        boolean msgHandleFound = false;
        String thisMsgUriString = null;

        Log.i(TAG, "Entering setMessageReadStatus()");

        if (msgHandle != null) {
            if (mLastMessageList != null) {
                /* Query and update read status */
                if (msgHandleFound == true) {
                    thisMsgUri = Uri.parse(thisMsgUriString);
                    ContentResolver content = mContext.getContentResolver();
                    ContentValues values;
                    /* Put new content values for read field */
                    values = new ContentValues();

                    if (statusValue == Constants.STATUS_VALUE_YES) {
                        values.put("read", SmsManager.STATUS_ON_ICC_READ);
                    } else {
                        values.put("read", SmsManager.STATUS_ON_ICC_UNREAD);
                    }

                    content.update(thisMsgUri, values, null, null);
                } else {
                    Log.i(TAG, "setMessageReadStatus : No message record found for the speified handle");
                    return false;
                }
            }
        } else {
            Log.i(TAG, "setMessageReadStatus : Null handle supplied");
            return false;
        }

        return true;
    }

    /**
     *
     * @param msgHandle
     * @param statusValue
     * @return
     */
    private boolean setMessageDeleteStatus(String msgHandle, int statusValue) {
        Uri thisMsgUri = null;
        boolean msgHandleFound = false;
        String thisMsgUriString = null;

        Log.i(TAG, "Entering setMessageDeleteStatus()");

        if (msgHandle != null) {
            /* Query and update read status */
            if (msgHandleFound == true) {
                thisMsgUri = Uri.parse(thisMsgUriString);
                ContentResolver content = mContext.getContentResolver();
                /* Put new content values for read field */

                if (statusValue == Constants.STATUS_VALUE_YES) {
                   content.delete(thisMsgUri, null, null);
                } else {
                    Log.i(TAG, "setMessageDeleteStatus : Restoring deleted message unsupported");
                }
            } else {
                Log.i(TAG, "setMessageDeleteStatus : No message record found for the specified handle");
                return false;
            }
        }
        return true;
    }

    /**
     *
     * @param phoneNum
     * @param msgBody
     */
    public String saveSmsToSentFolder(String [] phoneNum, String msgBody) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();

        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);
        values.put("status", 0x20);
        Uri uri = localContent.insert(Constants.SENTURI, values);

        String str = uri.toString();
        String[] splitStr = str.split("/");
        Log.d(TAG, " NEW HANDLE " + splitStr[3]);
        setLastMessageHandleCreated(splitStr[3]);
        return splitStr[3];
    }

    /**
     *
     * @param phoneNum
     * @param msgBody
     */
    public String saveSmsToOutboxFolder(String [] phoneNum, String msgBody) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        int statusValuePending = 0x20;

        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);
        values.put("status", statusValuePending);
        Uri uri = localContent.insert(Constants.OUTBOXURI, values);

        String str = uri.toString();
        String[] splitStr = str.split("/");
        Log.d(TAG, " NEW HANDLE " + splitStr[3]);
        setLastMessageHandleCreated(splitStr[3]);
        return splitStr[3];
    }

    /**
     *
     * @param phoneNum
     * @param msgBody
     * returns MessageHandle
     */
    public String saveSmsToFailedFolder(String [] phoneNum, String msgBody) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        int statusValueFailed = 0x40;

        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);
        values.put("status", statusValueFailed);
        values.put("type", 5);
        Uri uri = localContent.insert(Uri.parse(Constants.SMS_URI), values);

        String str = uri.toString();
        String[] splitStr = str.split("/");
        Log.d(TAG, " NEW HANDLE " + splitStr[3]);
        setLastMessageHandleCreated(splitStr[3]);
        return splitStr[3];
    }

    /**
     *
     * @param phoneNum
     * @param msgBody
     * returns MessageHandle
     */
    public String saveSmsToPendingFolder(String [] phoneNum, String msgBody) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        int statusValuePending = 0x20;

        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);
        values.put("status", statusValuePending);
        values.put("type", 6);
        Uri uri = localContent.insert(Uri.parse(Constants.SMS_URI), values);

        String str = uri.toString();
        String[] splitStr = str.split("/");
        Log.d(TAG, " NEW HANDLE " + splitStr[3]);
        setLastMessageHandleCreated(splitStr[3]);
        return splitStr[3];
    }

    public void moveToPendingFolder(String handle) {
        ContentValues values = new ContentValues();
        values.put("type", getSMSFolderType("queued"));
        Uri uri = Uri.parse(Constants.SMS_URI + handle);
        mContext.getContentResolver().update(uri, values, null, null);
    }

    public void moveToFailedFolder(String handle) {
        ContentValues values = new ContentValues();
        values.put("type", getSMSFolderType("failed"));
        Uri uri = Uri.parse(Constants.SMS_URI + handle);
        mContext.getContentResolver().update(uri, values, null, null);
    }

    public void moveToSentFolder(String handle) {
        ContentValues values = new ContentValues();
        int statusValuePending = 0x20;
        values.put("status", statusValuePending);
        values.put("type", getSMSFolderType("sent"));
        Uri uri = Uri.parse(Constants.SMS_URI + handle);
        mContext.getContentResolver().update(uri, values, null, null);
    }

    public void setDeliveryStatusOK(String msgHandle) {
        ContentValues values = new ContentValues();
        int statusSuccess = 0x00;
        values.put("status", statusSuccess);
        Uri uri = Uri.parse(Constants.SMS_URI + msgHandle);
        mContext.getContentResolver().update(uri, values, null, null);
    }

    public void setDeliveryStatusFailed(String msgHandle) {
        ContentValues values = new ContentValues();
        int statusFailed = 64;
        values.put("status", statusFailed);
        Uri uri = Uri.parse(Constants.SMS_URI + msgHandle);
        mContext.getContentResolver().update(uri, values, null, null);
    }

    /**
     * Get the type (as in Sms ContentProvider) for the given table name
     */
    private int getSMSFolderType(String folder) {
        int type = 0;
        if (folder.equalsIgnoreCase(INBOX)) {
            type = 1;
        } else if (folder.equalsIgnoreCase(SENT)) {
            type = 2;
        } else if (folder.equalsIgnoreCase(DRAFT)) {
            type = 3;
        } else if (folder.equalsIgnoreCase(OUTBOX)) {
            type = 4;
        } else if (folder.equalsIgnoreCase(FAILED)) {
            type = 5;
        } else if (folder.equalsIgnoreCase(QUEUED)) {
            type = 6;
        }
        return type;
    }

    /**
     * This method saves an incoming sms received from MAP client
     * to the current folder. Current folder is the folder which was
     * set in the last SetPath request received on this MAP server.
     * It also keeps a handle generated for the destined message box
     * the actual MessageHandle to Message will be done when
     * GetMessageListing operation is executed after this
     *
     * @param phoneNum
     * @param msgBody
     */
    public void saveSmsToDestinedFolder(String [] phoneNum, String msgBody) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        Uri uri = null;

        Log.i(TAG, "saveSmsToDestinedFolder() entering.");
        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);

        /* Also save this message for later saving to
         * sent folder */
        pendingMessageNumber = phoneNum[0];
        pendingMessageBody = msgBody;
        saveToSentFlag = true;

        switch (thisObexSession.getCurrentPath()) {
        case Constants.MAP_FOLDER_INBOX:
            uri = localContent.insert(Constants.INBOXURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_INBOX);
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            uri = localContent.insert(Constants.OUTBOXURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_OUTBOX);
            break;
        case Constants.MAP_FOLDER_SENT:
            values.put("status", SmsManager.STATUS_ON_ICC_SENT);
            uri = localContent.insert(Constants.SENTURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_SENT);
            break;
        case Constants.MAP_FOLDER_DRAFT:
            values.put("status", SmsManager.STATUS_ON_ICC_UNSENT);
            uri = localContent.insert(Constants.DRAFTURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_DRAFT);
            break;
        case Constants.MAP_FOLDER_DELETED:
            uri = localContent.insert(Constants.DELETEDURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_DELETED);
            break;
        default:
            Log.i(TAG, "saveSmsToDestinedFolder : Unknown current path specified : " +
                        thisObexSession.getCurrentPath());
            break;
        }

        /* Update the last message handle created field */
        updateLastMessageHandleCreated();

        /* Extract the new message handle */
        if (uri != null) {
            String str = uri.toString();
            String[] splitStr = str.split("/");
            Log.d(TAG, "saveSmsToDestinedFolder->NEW HANDLE " + splitStr[3]);
            setLastMessageHandleCreated(splitStr[3]);
        }
    }

    /**
     * This method saves an incoming sms received from MAP client
     * to the current folder. Current folder is the folder which was
     * set in the last SetPath request received on this MAP server.
     * It also keeps a handle generated for the destined message box
     * the actual MessageHandle to Message will be done when
     * GetMessageListing operation is executed after this
     *
     * @param phoneNum
     * @param msgBody
     */
    public void saveSmsToSpecifiedFolder(String [] phoneNum, String msgBody, int folderPath) {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        Uri uri = null;

        Log.i(TAG, "saveSmsToSpecifiedFolder() entering.");
        /* Currently support for a single recipient address
         * is provided */
        values.put("address", phoneNum[0]);
        values.put("body", msgBody);
        values.put("status", 0);
        values.put("read", 0);

        /* Also save this message for later saving to
         * sent folder */
        pendingMessageNumber = phoneNum[0];
        pendingMessageBody = msgBody;

        switch (folderPath) {
        case Constants.MAP_FOLDER_INBOX:
            uri = localContent.insert(Constants.INBOXURI, values);
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            uri = localContent.insert(Constants.OUTBOXURI, values);
            break;
        case Constants.MAP_FOLDER_SENT:
            uri = localContent.insert(Constants.SENTURI, values);
            break;
        case Constants.MAP_FOLDER_DRAFT:
            uri = localContent.insert(Constants.DRAFTURI, values);
            break;
        case Constants.MAP_FOLDER_DELETED:
            uri = localContent.insert(Constants.DELETEDURI, values);
            break;
        default:
            Log.i(TAG, "saveSmsToDestinedFolder : Unknown current path specified : " +
                    folderPath);
            break;
        }

        /* Extract the new message handle */
        if (uri != null) {
            String str = uri.toString();
            String[] splitStr = str.split("/");
            Log.d(TAG, "saveSmsToDestinedFolder->Handle Generated " + splitStr[3]);
            setLastMessageHandleCreated(splitStr[3]);
        }
    }

    /**
     *
     * @return
     */
    public boolean performUpdateInbox() {
        boolean retVal = false;

        /* return false as it is not yet implemented. */

        return retVal;
    }

    /**
     * Checks a message handle for its validity
     * @param msgHandle
     * @return
     */
    public static boolean isMessageHandleValid(String msgHandle) {
        boolean retValue = true;

        if ((msgHandle == null) || (msgHandle.compareTo("") == 0)) {
            retValue = false;
        } else {
            /* Examine each char for range 0-9 and a-f/A-F */
            for (int i = 0; i < msgHandle.length(); i++) {
                if (!((msgHandle.charAt(i) >= '0') && (msgHandle.charAt(i) <= '9')) &&
                        !((msgHandle.charAt(i) >= 'a') && (msgHandle.charAt(i) <= 'f')) &&
                        !((msgHandle.charAt(i) >= 'A') && (msgHandle.charAt(i) <= 'F'))) {
                    retValue = false;
                    break;
                }
            }
        }

        Log.i(TAG, "isMessageHandleValid : Returning " + retValue);
        return retValue;
    }

    /**
     * Appends leading zeros to message handle if it's
     * length in hexa-decimal string format is less than
     * 16
     * @param handle
     * @return
     */
    private String appendLeadingZerosToMsgHandle(String handle) {
        String out = "";

        for (int i = 0; i < (MSG_HANDLE_LENGTH_HEXADECIMAL_REPRESENTATION - handle.length()); i ++) {
            out = out.concat("0");
        }
        out = out.concat(handle);

        return out;
    }

    /**
     * Returns current message handle in Hexadecimal value represented
     * in String
     *
     * @return
     */
    public String getMapCurrentMessageHandle() {
        return appendLeadingZerosToMsgHandle(Long.toHexString(mapCurrentMessageHandle));
    }

    public void updateLastMessageHandleCreated() {
        mLastMessageHandleCreated = getMapCurrentMessageHandle();
    }

    public void setLastMessageHandleCreated(String handle) {
        mLastMessageHandleCreated = handle;
    }

    /**
     * This method saves an incoming sms received from MAP client
     * to the sent folder.
     *
     * @param phoneNum
     * @param msgBody
     */
    public void savePendingMessage() {
        ContentValues values = new ContentValues();
        ContentResolver localContent = mContext.getContentResolver();
        Log.i(TAG, "savePendingMessage() entering.");
        if (saveToSentFlag == true) {
            /* Currently support for a single recipient address
             * is provided */
            values.put("address", pendingMessageNumber);
            values.put("body", pendingMessageBody);
            saveToSentFlag = false;

            values.put("status", SmsManager.STATUS_ON_ICC_SENT);
            localContent.insert(Constants.SENTURI, values);
            generateMessagehandle(Constants.MAP_FOLDER_SENT);

            /* Update the last message handle created field */
            updateLastMessageHandleCreated();
        } else {
            Log.i("savePendingMessage", "Message not marked for saving to sent");
        }
    }

    /**
     * This method fetched email messagesList
     * @param folderPath
     * @return
     */
    List <MessageListingDetails> fetchEmailMessagesList(BluetoothMapApplicationParameters appParams, int folderPath) {
        List <MessageListingDetails> emailMsgList = null;
        int messageListingSize = 0, numMessages = 0;
        String tempPath = null;
        int writeCount = 0;
        Cursor cursor = null;
        Cursor cursorAttach = null;
        Log.i(TAG, "fetchEmailMessagesList entering msg_mode=" + msg_mode);

        if ((msg_mode & Constants.MAP_MODE_EMAIL_ON) != 0) {
            /*Email messages */
            EmailUtils eu = new EmailUtils();
            String folderName;

            /* Map the numeric representation of folder to
             * String */
            tempPath = BluetoothMapFolderAccessor.mapGivenFolderToStringWithoutLeadingBar(folderPath);

            String splitStringsEmail[] = tempPath.split("/");
            Log.i(TAG, "splitStringsEmail[2] = " + splitStringsEmail[2]);

            folderName = eu.getFolderName(splitStringsEmail);
            if (folderName != null && folderName.equalsIgnoreCase("draft")) {
                folderName = "Drafts";
            }

            String urlEmail = Constants.EMAIL_URI;
            Uri uriEmail = Uri.parse(urlEmail);
            /* Reading attachment data */
            Uri uriEmailAttach = Uri.parse(Constants.EMAIL_URI_ATTACHMENT);
            ContentResolver crEmail = mContext.getContentResolver();

            String whereClauseEmail  = eu.getConditionString(folderName, mContext, appParams);

            try {
                Log.d(TAG, "  whereClauseEmail " + whereClauseEmail);
                cursor = crEmail.query(uriEmail, null, whereClauseEmail, null, "timeStamp desc");

                /* If cursor is null or cursor.moveToFirst evaluates to false, return
                 * a null <MessageListingDetails> List, handling of a null <MessageListDetails>
                 * shall be taken care of by the calling method */
                if ((cursor == null) || (cursor.moveToFirst() == false)) {
                    return emailMsgList;
                }

                int idInd = cursor.getColumnIndex("_id");
                int fromIndex = cursor.getColumnIndex("fromList");
                int toIndex = cursor.getColumnIndex("toList");
                int dateInd = cursor.getColumnIndex("timeStamp");
                int readInd = cursor.getColumnIndex("flagRead");
                int subjectInd = cursor.getColumnIndex("subject");
                int replyToInd = cursor.getColumnIndex("replyToList");

                int attachSizeInd = cursor.getColumnIndex("flagAttachment");

                Log.d(TAG, "move to Liststartoffset"
                        + cursor.moveToPosition(appParams.listStartOffset));

                if (cursor != null) {
                    numMessages = cursor.getCount();
                    /*emailMsgList = new MessageListingDetails[numMessages]; */
                    emailMsgList = new ArrayList<MessageListingDetails>(numMessages);
                }

                StringBuilder subject = new StringBuilder();
                StringBuilder timestamp = new StringBuilder();
                StringBuilder senderName = new StringBuilder();
                StringBuilder senderAddressing = new StringBuilder();
                StringBuilder recipientName = new StringBuilder();
                StringBuilder recipientAddressing = new StringBuilder();
                StringBuilder msgId = new StringBuilder();
                StringBuilder readStatus = new StringBuilder();
                StringBuilder replyToStr = new StringBuilder();
                StringBuilder attachSizeStr = new StringBuilder();
                if (cursor.moveToFirst()) {

                    do {
                        /*
                         * Apply remaining filters
                         */

                        Log.d(TAG, " msgListSize " + messageListingSize
                                + "write count " + writeCount);

                        messageListingSize++;
                        subject.append(cursor.getString(subjectInd));
                        timestamp.append(cursor.getString(dateInd));
                        senderName.append(cursor.getString(fromIndex));
                        senderAddressing.append(cursor.getString(fromIndex));
                        recipientName.append(cursor.getString(toIndex));
                        recipientAddressing.append(cursor.getString(toIndex));
                        msgId.append(cursor.getString(idInd));
                        readStatus.append(cursor.getString(readInd));
                        replyToStr.append(cursor.getString(replyToInd));
                        attachSizeStr.append(cursor.getString(attachSizeInd));
                        Log.i(TAG, "subject " + subject);
                        Log.i(TAG, "timestamp " + timestamp);
                        Log.i(TAG, "senderName " + senderName);
                        Log.i(TAG, "senderAddressing " + senderAddressing);
                        Log.i(TAG, "recipientName " + recipientName);
                        Log.i(TAG, "recipientAddressing " + recipientAddressing);
                        Log.i(TAG, "msgId " + msgId);
                        Log.i(TAG, "readStatus " + readStatus);
                        Log.i(TAG, "replyToStr " + replyToStr);
                        Log.i(TAG, "attachSizeStr " + attachSizeStr);
                        if (Integer.parseInt(attachSizeStr.toString()) != 0 ) {
                            StringBuilder whereClauseEmailAttach = new StringBuilder();
                            whereClauseEmailAttach.append(" messageKey = ").append(msgId).append(" OR ");
                            whereClauseEmailAttach.append(" _id = ").append(msgId).append(" OR ");
                            whereClauseEmailAttach.append(" flags = ").append(msgId);

                            Log.d(TAG, "Where clause for attachment is " + whereClauseEmailAttach);

                            cursorAttach = crEmail.query(uriEmailAttach, null, whereClauseEmailAttach.toString(), null, null);
                            int sizeInd = cursorAttach.getColumnIndex("size");
                            StringBuilder sizeStr = new StringBuilder();

                            if (cursorAttach.moveToFirst()) {
                                sizeStr.append(cursorAttach.getString(sizeInd));
                                attachSizeStr = sizeStr;
                            }
                        }
                        /*
                         * Don't want the listing; just send the listing size after
                         * applying all the filters.
                         */

                        /* Purify the subject */
                        removeSpecialCharsFromSubject(subject);

                        if (appParams.subjectLength < subject.length()) {
                            subject.setLength(appParams.subjectLength);
                        }

                        resizeToMaxSubjectLen(subject);

                        subject.trimToSize();
                        Log.i(TAG, "subjectLength chars " + subject.length());
                        MsgListingConsts emailMsg = new MsgListingConsts();
                        emailMsg = eu.bldEmailMsgLstItem(mContext, folderName, appParams,
                                subject.toString(), timestamp.toString(), senderName.toString(),
                                senderAddressing.toString(),
                                recipientName.toString(), recipientAddressing.toString(),
                                msgId.toString(), readStatus.toString(), replyToStr.toString(),attachSizeStr.toString());

                        /* New Message? */
                        if (readStatus.toString().equalsIgnoreCase("1")) {
                            isNewMessagePresent = true;
                        }
                        emailMsgList.add(messageListingSize - 1, convertToMessageListingObject(emailMsg, "EMAIL", subject.toString()));
                        writeCount++;

                        emailMsg.clearMsgListingConsts();
                        subject.delete(0, subject.length());
                        timestamp.delete(0, timestamp.length());
                        senderName.delete(0, senderName.length());
                        senderAddressing.delete(0, senderAddressing.length());
                        recipientName.delete(0, recipientName.length());
                        recipientAddressing.delete(0, recipientAddressing.length());
                        msgId.delete(0, msgId.length());
                        readStatus.delete(0, readStatus.length());
                        replyToStr.delete(0, replyToStr.length());
                        attachSizeStr.delete(0, attachSizeStr.length());
                    } while (cursor.moveToNext());
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
                if (cursorAttach != null) {
                    cursorAttach.close();
                }
            }
        }/*end email Messages if */

        return emailMsgList;
    }

    /**
     *
     * @param MsgToConvert
     * @return
     */
    MessageListingDetails convertToMessageListingObject(MsgListingConsts MsgToConvert, String type, String subject) {
        MessageListingDetails obj = null;
        MsgListingConsts src = MsgToConvert;
        Log.i(TAG, "convertToMessageListingObject");
        if (src != null) {
            src.dumpEmailDetails();
            obj = new MessageListingDetails();
            obj.string_field_handle.append(Integer.toString(src.getMsg_handle()));
            obj.attachment_size.append(Integer.toString(src.getAttachment_size()));
            obj.datetime.append(src.getDatetime());
            obj.is_protected.append(src.getMsg_protected());
            obj.priority.append(src.getPriority());
            obj.read.append(src.getRead());
            obj.reception_status.append(src.getReception_status());
            obj.recipient_addressing.append(src.getRecepient_addressing());
            obj.recipient_name.append(src.getRecepient_name());
            obj.replyto_addressing.append(src.getReplyTo_addressing());
            obj.sender_addressing.append(src.getSender_addressing());
            obj.sender_name.append(src.getSender_name());
            obj.sent.append(src.getSent());
            obj.size.append(Integer.toString(src.getSize()));
            obj.subject.append(src.getSubject());
            obj.type.append(type);
        }
        return obj;
    }

    /**
     * This method merges two MessageListingDetails arrays into a single array
     * @param listA
     * @param listB
     * @return
     */
    List <MessageListingDetails> mergeMessagesList(List <MessageListingDetails> listA,
            List <MessageListingDetails> listB, int maxCount) {
        List <MessageListingDetails> listC = null;
        int lengthA, lengthB;

        if (listA != null) {
            Log.i(TAG, "listA size= " + listA.size());
        }

        if (listB != null) {
            Log.i(TAG, "listB size = " + listB.size());
        }

        if (listA == null) {
            listC = listB;
        } else if (listB == null) {
            listC = listA;
        } else {
            lengthA = listA.size();
            lengthB = listB.size();
            int listCSize = lengthA + lengthB;

            if (listCSize > maxCount) {
                /* Update lengthA and lengthB to contains an
                 * optimum mix of two kind of messages.
                 */
                ListSizesCouplet cc = new ListSizesCouplet(lengthA, lengthB, maxCount);
                cc.calculateListSizes();
                lengthA = cc.smsListSize;
                lengthB = cc.emailListSize;
                listCSize = cc.totalListSize;
            }
            int index = 0;
            listC = new ArrayList<MessageListingDetails>(listCSize);
            for (index = 0; index < lengthA; index++) {
                listC.add(index, listA.get(index));
            }
            for (index = 0; index < lengthB; index++) {
                listC.add(lengthA + index, listB.get(index));
            }
        }
        return listC;
    }

    /**
     *
     * @param handle
     * @return
     */
    boolean isHandleFromEmailRepository(String msgHandle) {
        boolean retVal = false;

        if ((msgHandle != null) && (msgHandle.length() != 0)) {
            /* Offsets for message IDs can only assume positive values */
            if ((Integer.valueOf(msgHandle) - EMAIL_HDLR_CONSTANT) > 0) {
                retVal = true;
            }
        }
        Log.i(TAG, "isHandleFromEmailRepository " + retVal);
        return retVal;
    }

    public static String converToYYYYMMDDTHHMMSSFormat(Long timeLongFmt) {
        Time time = new Time();
        time.set(timeLongFmt);
        /*Return a string in the RFC 3339(YYYYMMDDTHHMMSS,19850412T232050) format. */
        String time3339 = time.format3339(false);
        int timeStrLength = time3339.length();

        String datetimeStr = time.toString().substring(0, 15) +
        time3339.substring(timeStrLength - 6, timeStrLength - 3) +
        time3339.substring(timeStrLength - 2, timeStrLength);

        Log.i("converToYYYYMMDDTHHMMSSFormat", "date time is " + datetimeStr.substring(0, 15));
        return datetimeStr.substring(0, 15);
    }

    /**
     * Sets the message status (read/unread, delete)
     *
     * @return Obex response code
     */
    public int msgStatus(String name,
            BluetoothMapApplicationParameters bluetoothMasAppParams) {

        if ((bluetoothMasAppParams.statusIndicator != 0)
                && (bluetoothMasAppParams.statusIndicator != 1)) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        if ((bluetoothMasAppParams.statusValue != 0)
                && (bluetoothMasAppParams.statusValue != 1)) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        Uri uri = Uri.parse(Constants.SMS_URI + name);
        Cursor cr = null;

        try {
            cr = mContext.getContentResolver().query(uri, null, null, null,
                    null);
            Log.i(TAG, "bluetoothMasAppParams.statusValue " + bluetoothMasAppParams.statusValue);
            if (cr.moveToFirst()) {
                if (bluetoothMasAppParams.statusIndicator == 0) {
                    /* Read Status */

                    ContentValues values = new ContentValues();
                    values.put("read", bluetoothMasAppParams.statusValue);
                    mContext.getContentResolver().update(uri, values, null, null);
                } else {
                    if (bluetoothMasAppParams.statusValue == 1) {
                        deleteSms(name);
                    } else if (bluetoothMasAppParams.statusValue == 0) {
                        undeleteSms(name);
                    }
                }
                return ResponseCodes.OBEX_HTTP_OK;
            }

            /*Email */
            /*Query the mailbox table to get the id values for INBOX and Trash folder */
            Uri uri1 = Uri.parse(Constants.EMAIL_URI_MAILBOX);
            Cursor cr1 = null;

            try {
                cr1 = mContext.getContentResolver().query(uri1, null,
                        "(UPPER(serverId) = 'INBOX' OR UPPER(serverId) LIKE 'TRASH')", null, null);
                int inboxFolderId = 0;
                int deletedFolderId = 0;
                String folderName;
                if (cr1.getCount() > 0) {
                    cr1.moveToFirst();
                    do {
                        folderName = cr1.getString(cr1.getColumnIndex("serverId"));
                        if (folderName.equalsIgnoreCase("INBOX")) {
                            inboxFolderId = cr1.getInt(cr1.getColumnIndex("_id"));
                        } else if (folderName.equalsIgnoreCase("TRASH")) {
                            deletedFolderId = cr1.getInt(cr1.getColumnIndex("_id"));
                        }
                    } while ( cr1.moveToNext());
                }

                /*Query the message table for the given message id */
                int emailMsgId = 0;
                Log.d(TAG, "\n name " + (Integer.valueOf(name)));
                emailMsgId = (Integer.valueOf(name) - EMAIL_HDLR_CONSTANT);

                Uri uriDelete = Uri.parse(Constants.EMAIL_URI + "/" + emailMsgId);
                Uri uriUpdateDelete = Uri.parse(Constants.EMAIL_URI_SYNC_SERVER + "/" + emailMsgId);
                cr = mContext.getContentResolver().query(uriDelete, null, "_id = " + emailMsgId,
                         null, null);
                if (cr.moveToFirst()) {
                    if (bluetoothMasAppParams.statusIndicator == 0) {
                        /* Read Status */
                        ContentValues values = new ContentValues();
                        values.put("flagRead", bluetoothMasAppParams.statusValue);
                        mContext.getContentResolver().update(uriUpdateDelete, values, null, null);
                    } else {
                        if (bluetoothMasAppParams.statusValue == 1) { /*delete mail */
                            ContentValues values = new ContentValues();
                            values.put("mailboxKey", deletedFolderId);
                            mContext.getContentResolver().update(uriUpdateDelete,values,null,null);
                        }
                        if (bluetoothMasAppParams.statusValue == 0) {
                            return ResponseCodes.OBEX_HTTP_FORBIDDEN;
                        }
                    }
                    return ResponseCodes.OBEX_HTTP_OK;
                }
            } finally {
                if (cr1 != null) {
                    cr1.close();
                }
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }

        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
    }

    private void updateSMSThreadId(String msgHandle, int threadId) {
        ContentValues values = new ContentValues();
        values.put("thread_id", threadId);
        mContext.getContentResolver().update( Uri.parse(Constants.SMS_URI + msgHandle), values, null, null);
    }

    public void deleteSms(String msgHandle) {
        Cursor cr = mContext.getContentResolver().query(Uri.parse(Constants.SMS_URI + msgHandle), null, null, null, null);
        if ( cr.moveToFirst()) {
            int threadId = cr.getInt(cr.getColumnIndex(("thread_id")));
            if ( threadId != DELETED_THREAD_ID) {
                /* Move to deleted folder */
                updateSMSThreadId(msgHandle, Integer.valueOf(DELETED_THREAD_ID));
            } else {
                /* Delete the message permanently */
                mContext.getContentResolver().delete(Uri.parse(Constants.SMS_URI + msgHandle), null, null);
            }
            cr.close();
        }
    }

    private void undeleteSms(String msgHandle) {

        Cursor cr = mContext.getContentResolver().query(Uri.parse(Constants.SMS_URI + msgHandle), null, null, null, null );

        if (cr.moveToFirst()) {

            /* Make sure that the message is in delete folder */
            String currentThreadId = cr.getString(cr.getColumnIndex("thread_id"));
            if ( currentThreadId != null && Integer.valueOf(currentThreadId) != -1) {
                Log.d(TAG, " Not in delete folder");
                return;
            }

            /* Fetch the address of the deleted message */
            String address = cr.getString(cr.getColumnIndex("address"));

            /* Search the database for the given address */
            Cursor crThreadId = mContext.getContentResolver().query(Constants.SMSURI,
                    null, "address = " + address + " AND thread_id != -1", null, null);
            if (crThreadId.moveToFirst()) {
                /* A thread for the given address exists in the database */
                String threadIdStr = crThreadId.getString(crThreadId.getColumnIndex("thread_id"));
                updateSMSThreadId(msgHandle, Integer.valueOf(threadIdStr));
            } else {
                /* No thread for the given address
                 * Create a fake message to obtain the thread, use that thread_id
                 * and then delete the fake message
                 */
                ContentValues tempValue = new ContentValues();
                tempValue.put("address", address);
                tempValue.put("type", "20");
                Uri tempUri = mContext.getContentResolver().insert( Constants.SMSURI, tempValue);

                if ( tempUri != null ) {
                    Cursor tempCr = mContext.getContentResolver().query(tempUri, null, null, null, null);
                    tempCr.moveToFirst();
                    String newThreadIdStr = tempCr.getString(tempCr.getColumnIndex("thread_id"));
                    tempCr.close();

                    updateSMSThreadId(msgHandle, Integer.valueOf(newThreadIdStr));

                    mContext.getContentResolver().delete(tempUri, null, null);
                }
            }
            crThreadId.close();
        } else {
            Log.d(TAG, "msgHandle not found");
        }
        cr.close();
    }

    public void resetNewMessagePresence() {
        isNewMessagePresent = false;
    }

    public void resetMessageListingSize() {
        mapCurrentMessageListingSize = 0;
    }

    /**
     * Get the folder name (MAP representation) based on the message Handle
     */
    private String getContainingFolder(String msgHandle) {
        Cursor cr = null;

        try {
            cr = mContext.getContentResolver().query(
                    Uri.parse(Constants.SMS_URI + msgHandle),
                    new String[] { "_id", "type", "thread_id" }, null, null, null);
            if (cr.getCount() > 0) {
                cr.moveToFirst();
                return getMAPFolder(cr.getString(cr.getColumnIndex("type")),
                        cr.getString(cr.getColumnIndex("thread_id")));
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }
        return null;
    }

    /**
     * Get the folder name (MAP representation) based on the folder type value
     * in SMS database
     */
    private String getMAPFolder(String type, String threadId) {
        String folder = null;
        if ( type == null || threadId == null) {
            Log.d(TAG, "getMapFolder cannot parse folder type");
            return folder;
        }

        if ( Integer.valueOf(threadId) == DELETED_THREAD_ID) {
            folder = "deleted";
        } else {
            switch (Integer.valueOf(type)) {
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
        }
        return folder;
    }

    /* Constants and methods for parsing vCards */
    private class VcardContent {
        public String name = "";
        public String tel = "";
    }

    static final int PHONELOOKUP_ID_COLUMN_INDEX = 0;
    static final int PHONELOOKUP_LOOKUP_KEY_COLUMN_INDEX = 1;
    static final int PHONELOOKUP_DISPLAY_NAME_COLUMN_INDEX = 2;
    static final int EMAIL_DATA_COLUMN_INDEX = 0;

    private VcardContent getVcardContent(String phoneAddress) {

        Log.i(TAG, "getVcardContent");

        VcardContent vCard = new VcardContent();
        vCard.tel = phoneAddress;
        Cursor cursorContacts = null;
        if (phoneAddress.isEmpty() != true) {
            try {
                try {
                    Uri uriContacts = Uri.withAppendedPath(PhoneLookup.CONTENT_FILTER_URI,
                            Uri.encode(phoneAddress));

                    cursorContacts = mContext.getContentResolver().query(
                            uriContacts,
                            new String[] { PhoneLookup._ID, PhoneLookup.LOOKUP_KEY,
                                    PhoneLookup.DISPLAY_NAME }, null, null, null);
                }
                catch (Exception e) {
                    Log.e(TAG, "Caught exception " + e);
                }
                try {
                    cursorContacts.moveToFirst();
                }
                catch (Exception e) {
                    Log.e(TAG, "Exception " + e);
                }

                Log.i(TAG, "CursorContacts.getCount() is " + cursorContacts.getCount());
                if (cursorContacts.getCount() > 0) {
                    /* look up details */
                    long contactId = cursorContacts
                    .getLong(PHONELOOKUP_ID_COLUMN_INDEX);
                    String lookupKey = cursorContacts
                    .getString(PHONELOOKUP_LOOKUP_KEY_COLUMN_INDEX);

                    Uri lookUpUri = Contacts.getLookupUri(contactId, lookupKey);
                    String Id = lookUpUri.getLastPathSegment();
                }
            }
            catch (Exception e) {
                Log.e(TAG, "Caught exception " + e);
            } finally {
                cursorContacts.close();
            }
        }
        return vCard;
    }

    /* information for owner's info */
    private class OwnerInfo {
        public String Name;
        public String Number;
    }

    private OwnerInfo getOwnerInfo() {
        OwnerInfo info = new OwnerInfo();

        Log.i(TAG, "getOwnerInfo");

        TelephonyManager tm = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
        if (tm != null) {
            String sLocalPhoneNum = tm.getLine1Number();
            String sLocalPhoneName = "";

            Method m;
            try {
                m = tm.getClass().getMethod("getLine1AlphaTag", new Class[] { });
                sLocalPhoneName = (String) m.invoke(tm);
            } catch (SecurityException e) {
                Log.e(TAG,"SecurityException " + e.toString());
            } catch (NoSuchMethodException e) {
                Log.e(TAG,"NoSuchMethodException " + e.toString());
            } catch (IllegalArgumentException e) {
                Log.e(TAG,"IllegalArgumentException " + e.toString());
            } catch (IllegalAccessException e) {
                Log.e(TAG,"IllegalAccessException " + e.toString());
            } catch (InvocationTargetException e) {
                Log.e(TAG,"InvocationTargetException " + e.toString());
            }

            if (TextUtils.isEmpty(sLocalPhoneNum)) {
                sLocalPhoneNum = " ";
            }
            if (TextUtils.isEmpty(sLocalPhoneName)) {
                sLocalPhoneName = " ";
            }
            info.Name = sLocalPhoneName;
            info.Number = sLocalPhoneNum;
        }
        return info;
    }

    private OwnerInfo ownerInfo = null;

    /**
     * Get the owners name
     */
    public String getOwnerName() {
        if ( ownerInfo == null ) {
            ownerInfo = getOwnerInfo();
        }
        return ownerInfo.Name;
    }

    /**
     * Get the owners phone number
     */
    public String getOwnerNumber() {
        if ( ownerInfo == null ) {
            ownerInfo = getOwnerInfo();
        }
        return ownerInfo.Number;
    }

    /* Method for retrieving the Sms Deliver PDU */
    /**
     * Get the SMS Deliver PDU for the given SMS
     */
    private String getSMSDeliverPdu(String smsBody, String dateTime, String address) {

        Time time = new Time();
        time.set(Long.valueOf(dateTime));

        String timeStr = time.format3339(false);

        /* Extract the YY, MM, DD, HH, MM, SS from time */
        String tempTimeStr = timeStr.substring(2,4) + timeStr.substring(5, 7)
        + timeStr.substring(8, 10) + timeStr.substring(11, 13) +
        timeStr.substring(14, 16) + timeStr.substring(17, 19);

        /* Calculate the time zone offset
         * An offset of 1 indicates 15 min difference between local
         * time and GMT. MSB of 1 in offset indicates it is negative
         */
        String tZoneStr = timeStr.substring(timeStr.length()- 6);
        int tempInt = Integer.valueOf(tZoneStr.substring(tZoneStr.length()-2));
        int tZone15offset = tempInt / 15;

        tZone15offset += (Integer.valueOf(tZoneStr.substring(tZoneStr.length()-5, tZoneStr.length()-3)) * 4);
        if ( timeStr.charAt(timeStr.length()-6) == '-') {
            tZone15offset = tZone15offset | 0x80;
        }

        String tZone15OffsetHexStr = "";

        /* Add 0 as prefix for single digit offset */
        if (((int) tZone15offset & 0xff) < 0x10) {
            tZone15OffsetHexStr += "0";
        }
        tZone15OffsetHexStr += Integer.toHexString(tZone15offset);

        tempTimeStr += tZone15OffsetHexStr;

        /* Swap the nibble */
        String encodedTimeStr = "";
        for ( int i=0; i<tempTimeStr.length(); i=i+2) {
            encodedTimeStr += tempTimeStr.substring(i+1, i+2);
            encodedTimeStr += tempTimeStr.substring(i, i+1);
        }

        byte[] byteAddress = address.getBytes();

        /* Let the service center number be 0000000000 */
        String smsPdu = "0681000000000004";

        /* Extract only digits out of the phone address */
        StringBuffer strbufAddress = new StringBuffer(address.length() + 1);
        for ( int i=0; i<address.length(); i++) {
            Log.d(TAG, " VAL " + address.substring(i, i+1));
            if ( byteAddress[i] >= 48 && byteAddress[i] <= 57 ) {
                strbufAddress.append(Integer.parseInt(address.substring(i, i+1)));
            }
        }

        int addressLength = strbufAddress.length();

        String addressLengthStr = "";

        if (((int) addressLength & 0xff) < 0x10)
            addressLengthStr += "0";
        addressLengthStr += Integer.toHexString(addressLength);

        smsPdu = smsPdu + addressLengthStr;
        smsPdu = smsPdu + "81";

        String strAddress = new String(strbufAddress);

        /* Use getSubmitPdu only to obtain the encoded msg and encoded address */
        byte[] msg = SmsMessage.getSubmitPdu(null, strAddress, smsBody, false).encodedMessage;

        int addLength = Integer.valueOf(msg[2]);
        if ( addLength %2 != 0) {
            addLength++;
        }
        addLength = addLength / 2;

        /* Extract the message from the SubmitPdu */
        int msgOffset = 7 + addLength;
        int msgLength = msg.length - msgOffset;

        StringBuffer strbufMessage = new StringBuffer(msgLength * 2);

        /* Convert from byte to Hex String */
        for (int i=msgOffset; i<msgLength + msgOffset; i++) {
            if (((int) msg[i] & 0xff) < 0x10) {
                strbufMessage.append("0");
            }
            strbufMessage.append((Long.toString((int) msg[i] & 0xff, 16)));
        }

        int encodedAddressLength = strAddress.length() / 2;
        if ( strAddress.length() % 2 != 0) {
            encodedAddressLength++;
        }

        StringBuffer strbufAddress1 = new StringBuffer(msgLength * 2);

        /* Convert from byte to Hex String */
        for (int i=4; i<encodedAddressLength + 4; i++) {
            if (((int) msg[i] & 0xff) < 0x10)
                strbufAddress1.append("0");
            strbufAddress1.append((Long.toString((int) msg[i] & 0xff, 16)));
        }

        smsPdu += strbufAddress1;
        smsPdu += "0000";
        smsPdu += encodedTimeStr;

        int smsBodyLength = smsBody.length();
        String smsMessageTextLengthStr = "";

        if (((int) smsBodyLength & 0xff) < 0x10) {
            smsMessageTextLengthStr += "0";
        }
        smsMessageTextLengthStr += Integer.toHexString(smsBodyLength);

        smsPdu += smsMessageTextLengthStr;
        smsPdu += strbufMessage;
        smsPdu = smsPdu.toUpperCase();
        return smsPdu;
    }

    /**
     * This method creates a bMessage Object from the given
     * pointer (Cursor) to a message
     * @param messageCursor
     * @return String
     */
    public boolean createBmsgObjectWithVcard(String msgHandleToFetch, int currFolder, int Charset) {
        String bMessage = "", msgHandle = null;
        BluetoothMasMessageRsp rsp = new BluetoothMasMessageRsp();

        Log.i(TAG, "Entering createBmsgObjectWithVcard()");

        Cursor cr = null;
        msgHandle = msgHandleToFetch;
        Uri uri = Constants.SMSURI;
        String whereClause = " _id = " + msgHandle;
        Uri allCanonical = Uri.parse(Constants.CANONICAL_SMS_URI);
        Cursor cr2 = null;
        String phoneNum = "";
        try {
            cr = mContext.getContentResolver().query(uri, null, whereClause, null,
                    null);
            if (cr.getCount() > 0) {
                cr.moveToFirst();
                String containingFolder = getContainingFolder(msgHandle);
                Bmessage bmsg = new Bmessage();

                /* Create a bMessage */
                bmsg.setType("SMS_GSM");
                bmsg.setBmsg_version("1.0");
                if (cr.getString(cr.getColumnIndex("read")).equalsIgnoreCase("1")) {
                    bmsg.setStatus("READ");
                } else {
                    bmsg.setStatus("UNREAD");
                }

                bmsg.setFolder("TELECOM/MSG/" + containingFolder);
                bmsg.setVcard_version("2.1");
                String whereClause2 = " _id = " + cr.getString(cr.getColumnIndex("thread_id"));

                try {
                    cr2 = mContext.getContentResolver().query(allCanonical, null, whereClause2, null,null);
                    if (cr2.getCount() > 0) {
                        cr2.moveToFirst();
                        phoneNum = phoneNum + cr2.getString(cr2.getColumnIndex("address"));
                    }
                } catch (Exception e) {
                    Log.e(TAG, "Exception " + e);
                    rsp.rsp = ResponseCodes.OBEX_HTTP_BAD_REQUEST;

                    mLastbMessageObject.delete(0, mLastbMessageObject.length());
                    mLastbMessageObject.append(bMessage);
                    return false;
                } finally {
                    if (cr2 != null) {
                        cr2.close();
                    }
                }

                VcardContent vcard = getVcardContent(phoneNum);
                String type = cr.getString(cr.getColumnIndex("type"));
                /* INBOX is type 1. */
                if (type.equalsIgnoreCase("1")) {
                    /* The address in database is of originator */
                    bmsg.setOriginatorVcard_name(vcard.name);
                    bmsg.setOriginatorVcard_phone_number(vcard.tel);
                    bmsg.setRecipientVcard_name(getOwnerName());
                    bmsg.setRecipientVcard_phone_number(getOwnerNumber());
                } else {
                    bmsg.setRecipientVcard_name(vcard.name);
                    bmsg.setRecipientVcard_phone_number(vcard.tel);
                    bmsg.setOriginatorVcard_name(getOwnerName());
                    bmsg.setOriginatorVcard_phone_number(getOwnerNumber());
                }

                String smsBody = " ";

                if ( (int)Charset == 1) {
                    bmsg.setBody_charset("UTF-8");
                    smsBody = cr.getString(cr.getColumnIndex("body"));
                }

                if ( (int)Charset == 0) {
                    bmsg.setBody_encoding("G-7BIT");
                    String smsBodyUnicode = cr.getString(cr.getColumnIndex("body"));
                    smsBody = getSMSDeliverPdu(smsBodyUnicode, cr.getString(cr.getColumnIndex("date")), vcard.tel);
                }

                bmsg.setBody_msg(smsBody);

                /* Send a bMessage */
                Log.d(TAG, "bMessageSMS test\n");
                String str = MapUtils.toBmessageSMS(bmsg);
                /* calculate length as per errata 3603 */
                bmsg.setBody_length(bmsgLength(str));
                /* update str as length has been changed */
                str = MapUtils.toBmessageSMS(bmsg);
                bMessage = str;
            } else {
                Log.d(TAG, "createBmsgObjectWithVcard no results yielded \n");
                return false;
            }
        } catch (Exception e) {
            Log.e(TAG, "Exception " + e);
            rsp.rsp = ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            mLastbMessageObject.delete(0, mLastbMessageObject.length());
            mLastbMessageObject.append(bMessage);

            return false;
        } finally {
            if (cr != null) {
                cr.close();
            }
        }

        mLastbMessageObject.delete(0, mLastbMessageObject.length());
        mLastbMessageObject.append(bMessage);

        return true;
    }

    private int bmsgLength(String body){

         int pos = body.indexOf("BEGIN:MSG");
         int end = body.indexOf("END:MSG") + ("END:MSG").length();
         int len = body.substring(pos,end).getBytes().length;
         return len;

    }

    private void resizeToMaxSubjectLen(StringBuilder input) {
        if (input.toString().getBytes().length > MAX_SUBJECT_LEN ) {
            byte[] tmp= input.toString().getBytes();
            byte[] newSub  = new byte[MAX_SUBJECT_LEN - 6];
            /* Copy 6 bytes less to avoid utf-8 conversion to increased bytes */
            System.arraycopy (tmp, 0, newSub, 0, MAX_SUBJECT_LEN - 6);
            input.delete(0, input.length());
            input.append(new String(newSub));
        }
    }

    private void removeSpecialCharsFromSubject(StringBuilder input) {
        for (int i =0; i < input.length(); i++) {
            switch (input.charAt(i)) {
            case '"':
                input.replace(i, i + 1, "&quot;");
                i += 5;
                break;
            case '\'':
                input.replace(i, i + 1, "&apos;");
                i += 5;
                break;
            case '&':
                input.replace(i, i + 1, "&amp;");
                i += 4;
                break;
            case '>':
                input.replace(i, i + 1, "&gt;");
                i += 3;
                break;
            case '<':
                input.replace(i, i + 1, "&lt;");
                i += 3;
                break;
            case '\\':
            case '/':
                input.setCharAt(i, ' ');
                break;
            default:
                break;
            }
        }
    }

    private boolean isOutgoingSMSMessage(int type) {
        if (type == 1) {
            return false;
        }
        return true;
    }

    private boolean isSentSMSMessage(int type) {
        if (type == 2) {
            return true;
        }
        return false;
    }

    /**
     * Get the contact name for the given phone number
     */
    private String getContactName(String phoneNumber) {
        String name = "Unknown Name";

        Log.i(TAG, "getContactName");
        VcardContent foundEntry = null;
        if (phoneNumber == null) {
            return name;
        }

        /* Read the name from phonebook provider */
        foundEntry = getVcardContent(phoneNumber);
        if (foundEntry != null) {
            return foundEntry.name;
        }
        return name;
    }

    public void deinitializeMessageAccessorData() {
        mLastMessageList = null;
        mLastbMessageObject.delete(0, mLastbMessageObject.length());
    }

};
