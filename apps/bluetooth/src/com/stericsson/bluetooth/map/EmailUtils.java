/*
 * Modified by, ST-Ericsson SA (2011) All rights reserved.
 * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in the
 *          documentation and/or other materials provided with the distribution.
 *        * Neither the name of Code Aurora nor
 *          the names of its contributors may be used to endorse or promote
 *          products derived from this software without specific prior written
 *          permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.stericsson.bluetooth.map;
import java.util.Date;
import java.util.List;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.text.format.Time;
import android.util.Log;
import android.util.TimeFormatException;
import java.util.regex.*;

public class EmailUtils {

    public final String TAG = "EmailUtils";

    public static final int EMAIL_HDLR_CONSTANT = 200000;

    private StringBuilder quoted_printable = new StringBuilder();

    public List<String> folderListEmail(List<String> folderList, Context context) {
        String[] projection = new String[] {"displayName"};
        Uri uri = Uri.parse(Constants.EMAIL_URI_MAILBOX);

        Cursor cr = null;
        try {
            cr = context.getContentResolver().query(uri, projection, null, null, null);

            if ( cr.moveToFirst()) {
                do {
                    Log.d(TAG, " Column Name: "+ cr.getColumnName(0) + " Value: " + cr.getString(0));
                    int folderFlag = 0;
                    for (int i=0; i< folderList.size(); i++) {
                        if (folderList.get(i).equalsIgnoreCase(cr.getString(0))) {
                            folderFlag = 1;
                            break;
                        }
                    }
                    if (cr.getString(0).equalsIgnoreCase("DRAFTS")) {
                        folderFlag = 1;
                    }
                    if (folderFlag == 0) {
                        folderList.add(cr.getString(0));
                    }

                } while ( cr.moveToNext());
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }
        Log.d(TAG, " Folder Listing of SMS,MMS and EMAIL: "+folderList);

        return folderList;

    }

    public String getWhereIsQueryForTypeEmail(String folder, Context context) {

        Log.d(TAG, "  Inside getWhereIsQueryForTypeEmail  "+ folder);
        String query = "mailboxKey = -1";
        String folderId;
        Cursor cr = null;
        if (folder.compareTo("deleted") == 0)
            folder = "Trash";
        Uri uri = Uri.parse(Constants.EMAIL_URI_MAILBOX);
        String whereClause = "UPPER(serverId) LIKE '"+folder.toUpperCase().trim()+"%'";
        try {
            cr = context.getContentResolver().query(
                    uri, null, whereClause , null, null);

            if ( cr.moveToFirst()) {
                do {
                    Log.d(TAG, "  Inside getWhereIsQueryForTypeEmail Folder Name  "+ cr.getString(cr.getColumnIndex("displayName")));
                    folderId = cr.getString(cr.getColumnIndex("_id"));
                    query = "mailboxKey = "+ folderId;
                    break;
                } while ( cr.moveToNext());
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }

        return query;
    }

    public int getMessageSizeEmail(int messageId, Context context) {

        Log.d(TAG, "  Inside getMessageSizeEmail  "+ messageId);
        int msgSize = 0;
        String textContent = null;
        String htmlContent = null;

        Cursor cr = null;

        try {
            cr = context.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI_BODY), null, "messageKey = "+ messageId , null, null);
            if (cr != null) {
                if ( cr.moveToFirst()) {
                    do {
                        textContent = cr.getString(cr.getColumnIndex("textContent"));
                        if (textContent != null) {
                            msgSize = textContent.getBytes().length;
                        } else {
                            Log.i(TAG, "Null/No Text Content Retrieved");
                            htmlContent = cr.getString(cr.getColumnIndex("htmlContent"));
                            if (htmlContent != null) {
                                msgSize = htmlContent.getBytes().length;
                            } else {
                                Log.i(TAG, "Null/No HTML Content Retrieved");
                            }
                        }
                        break;
                    } while ( cr.moveToNext());
                }
            } else {
                Log.e(TAG, "getMessageSizeEmail : Error Reading Message Size!!!");
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }

        Log.d(TAG, "  Exiting getMessageSizeEmail  "+ messageId);
        return msgSize;
    }

    public String getFolderName(String[] splitStringsEmail) {
        String folderName;
        Log.d(TAG, "  Inside getFolderName  "+ splitStringsEmail);

        if (splitStringsEmail[2].trim().equalsIgnoreCase("[Gmail]") || splitStringsEmail[2].trim().contains("Gmail")) {
            folderName = splitStringsEmail[2]+"/"+splitStringsEmail[3];
        } else {
            folderName = splitStringsEmail[2];
        }

        Log.d(TAG, "folderName   " + folderName);

        return folderName;
    }

    public String getConditionString(String folderName, Context context, BluetoothMapApplicationParameters appParams) {
        String whereClauseEmail = getWhereIsQueryForTypeEmail(folderName, context);

        /* Filter readstatus: 0 no filtering, 0x01 get unread, 0x10 get read */
        if (appParams.filterReadStatus != 0) {
            if ((appParams.filterReadStatus & 0x1) != 0) {
                if (whereClauseEmail != "") {
                    whereClauseEmail += " AND ";
                }
                whereClauseEmail += " flagRead = 0 ";
            }
            Log.d(TAG, " Filter Read Status Value :"+appParams.filterReadStatus);
            if ((appParams.filterReadStatus & 0x2) != 0) {
                if (whereClauseEmail != "") {
                    whereClauseEmail += " AND ";
                }
                whereClauseEmail += " flagRead = 1 ";
            }
        }

        Log.d(TAG, " Filter Recipient Value :"+appParams.filterRecipient);
        /* Filter Recipient */
        if ((appParams.filterRecipient != null) && (appParams.filterRecipient != "")  && (appParams.filterRecipient.length() > 0)) {
            if (whereClauseEmail != "") {
                whereClauseEmail += " AND ";
            }
            whereClauseEmail += " toList LIKE '%"+appParams.filterRecipient.trim()+"%'";
        }

        if ((appParams.filterOriginator != null)
                && (appParams.filterOriginator.length() != 0)) {

            /*  For incoming message */
            if (whereClauseEmail != "") {
                whereClauseEmail += " AND ";
            }
            whereClauseEmail += "fromList LIKE '%"+appParams.filterOriginator.trim()+"%'";
        }
        Log.d(TAG, " whereClauseEmail :" + whereClauseEmail);

        /* Filter Period Begin */
        if ((appParams.filterPeriodBegin != null)
                && (appParams.filterPeriodBegin != "")) {
            Time time = new Time();

            try {
                time.parse(appParams.filterPeriodBegin);
                if (whereClauseEmail != "") {
                    whereClauseEmail += " AND ";
                }
                whereClauseEmail += "timeStamp > " + time.toMillis(false);

            } catch (TimeFormatException e) {
                Log.e(TAG, "Bad formatted filterPeriodBegin, Ignore"
                        + appParams.filterPeriodBegin);
            }
        }

        /* Filter Period End */
        if ((appParams.filterPeriodEnd != null)
                && (appParams.filterPeriodEnd != "")) {
            Time time = new Time();
            try {
                time.parse(appParams.filterPeriodEnd);
                if (whereClauseEmail != "") {
                    whereClauseEmail += " AND ";
                }
                whereClauseEmail += "timeStamp < " + time.toMillis(false);
            } catch (TimeFormatException e) {
                Log.e(TAG, "Bad formatted filterPeriodEnd, Ignore"
                        + appParams.filterPeriodEnd);
            }
        }

        return whereClauseEmail;
    }

    public MsgListingConsts bldEmailMsgLstItem(Context context, String folderName,
            BluetoothMapApplicationParameters appParams, String subject, String timestamp,
            String senderName, String senderAddressing, String recipientName,
            String recipientAddressing, String msgId, String readStatus, String replyToStr,String attachSizeStr) {

        /* TODO to check if really needed ?? */
        String ASCII_STX = "";
        String ASCII_SOH = "";
        char ASCII_SOH_C = '';

        MsgListingConsts emailMsg = new MsgListingConsts();
        emailMsg.setMsg_handle(Integer.valueOf(msgId)+ EMAIL_HDLR_CONSTANT);

        Time time = new Time();
        time.set(Long.valueOf(timestamp));

        String datetimeStr = time.toString().substring(0, 15);

        emailMsg.msgInfo.setDateTime(datetimeStr);

        if ((appParams.parameterMask & Constants.BIT_SUBJECT) != 0) {

            Log.d(TAG, "Fileter Subject Length  "+appParams.subjectLength);
            if ((subject != null && appParams.subjectLength > 0)
                    && (subject.length() > appParams.subjectLength)) {
                subject = subject.substring(0,
                        appParams.subjectLength);
            }
            emailMsg.setSubject(subject.trim());
        }

        if ((appParams.parameterMask & Constants.BIT_DATETIME) != 0) {
            emailMsg.setDatetime(datetimeStr);
        }

        if ((appParams.parameterMask & Constants.BIT_SENDER_NAME) != 0) {
            if (senderName.contains(ASCII_STX)) {
                String[] senderStr = senderName.split(ASCII_STX);
                if (senderStr !=null && senderStr.length > 0) {
                    Log.d(TAG, "  Sender name split String 0  " + senderStr[0]
                                                                              + " Sender name split String 1  " + senderStr[1]);
                    emailMsg.setSender_name(senderStr[1].trim());
                }
            } else {
                emailMsg.setSender_name(senderName.trim());
            }
        }

        if ((appParams.parameterMask & Constants.BIT_SENDER_ADDRESSING) != 0) {
            if (senderAddressing.contains(ASCII_STX)) {
                String[] senderAddrStr = senderAddressing.split(ASCII_STX);
                if (senderAddrStr !=null && senderAddrStr.length > 0) {
                    Log.d(TAG, "Sender Addressing split String 0 " + senderAddrStr[0]
                                                                                        + "Sender Addressing split String 1 " + senderAddrStr[1]);
                    emailMsg.setSender_addressing(senderAddrStr[0].trim());
                }
            } else {
                emailMsg.setSender_addressing(senderAddressing.trim());
            }

        }

        if ((appParams.parameterMask & Constants.BIT_RECIPIENT_NAME) != 0) {
            String multiRecepients = null;

            if (recipientName.contains(ASCII_STX)) {
                String[] recepientStr = recipientName.split(ASCII_STX);
                if (recepientStr !=null && recepientStr.length > 0) {
                    Log.d(TAG, "  Recepient name split String 0  " + recepientStr[0]
                          + " Recepient name split String 1  " + recepientStr[1]);
                    emailMsg.setRecepient_name(recepientStr[1].trim());
                }
            }
            else if (recipientName.contains(ASCII_SOH)) {
                multiRecepients = recipientName.replace(ASCII_SOH_C, ';');
                Log.d(TAG, "  Recepient name   " + multiRecepients);
                       emailMsg.setRecepient_name(multiRecepients.trim());
            } else {
                emailMsg.setRecepient_name(recipientName.trim());
            }
        }

        if ((appParams.parameterMask & Constants.BIT_RECIPIENT_ADDRESSING) != 0) {
            String multiRecepientAddrs = null;

            if (recipientAddressing.contains(ASCII_STX)) {
                String[] recepientAddrStr = recipientAddressing.split(ASCII_STX);
                if (recepientAddrStr !=null && recepientAddrStr.length > 0) {
                    Log.d(TAG, "  Recepient addressing split String 0  " + recepientAddrStr[0]
                                                                                              + " Recepient addressing split String 1  " + recepientAddrStr[1]);
                    emailMsg.setRecepient_addressing(recepientAddrStr[0].trim());
                }
            } else if (recipientAddressing.contains(ASCII_SOH)) {
                multiRecepientAddrs = recipientAddressing.replace(ASCII_SOH_C, ';');
                Log.d(TAG, "  Recepient Address  " + multiRecepientAddrs);
                emailMsg.setRecepient_addressing(multiRecepientAddrs.trim());
            } else {
                emailMsg.setRecepient_addressing(recipientAddressing.trim());
            }
        }

        if ((appParams.parameterMask & Constants.BIT_TYPE) != 0) {
            emailMsg.setType("EMAIL");
        }

        if ((appParams.parameterMask & Constants.BIT_SIZE) != 0) {
            int  msgSize = 0;
            msgSize = getMessageSizeEmail(Integer.parseInt(msgId), context);
            emailMsg.setSize(msgSize);
        }

        if ((appParams.parameterMask & Constants.BIT_RECEPTION_STATUS) != 0) {
            emailMsg.setReception_status("complete");
        }

        if ((appParams.parameterMask & Constants.BIT_TEXT) != 0) {
            emailMsg.setContains_text("yes");
        }

        if ((appParams.parameterMask & Constants.BIT_ATTACHMENT_SIZE) != 0) {
            int size = Integer.parseInt(attachSizeStr);
            emailMsg.setAttachment_size(size);

        }

        if ((appParams.parameterMask & Constants.BIT_PRIORITY) != 0) {
            emailMsg.setPriority("no");
        }

        if ((appParams.parameterMask & Constants.BIT_READ) != 0) {
            Log.d(TAG, "  Read Status  " + readStatus);
            if (readStatus.equalsIgnoreCase("1")) {
                emailMsg.setRead("yes");
            } else {
                emailMsg.setRead("no");
            }
        }

        if ((appParams.parameterMask & Constants.BIT_SENT) != 0) {
            if (folderName.equalsIgnoreCase("sent") || folderName.toUpperCase().contains("SENT")) {
                emailMsg.setSent("yes");
            } else {
                emailMsg.setSent("no");
            }
        }

        if ((appParams.parameterMask & Constants.BIT_PROTECTED) != 0) {
            emailMsg.setMsg_protected("no");
        }

        if ((appParams.parameterMask & Constants.BIT_REPLYTO_ADDRESSING) != 0) {
            Log.d(TAG, "  Reply To addressing  " + replyToStr);
            emailMsg.setReplyTo_addressing(replyToStr);
        }

        return emailMsg;
    }

    public String bldEmailBmsg(int msgHandle, Context context) {
        Log.d(TAG, "Inside bldEmailBmsg:");
        String str = null;

        /* TODO to check if really needed ?? */
        String ASCII_STX = "";
        String ASCII_SOH = "";
        char ASCII_SOH_C = '';

        /* Query the message table for obtaining the message related details */
        Cursor cr1 = null;
        int folderId;
        String timeStamp = null;
        StringBuilder subjectText = new StringBuilder();
        Uri uri1 = Uri.parse(Constants.EMAIL_URI);
        String whereClause = " _id = " + msgHandle;

        try {
            cr1 = context.getContentResolver().query(uri1, null, whereClause, null,
                    null);

            if (cr1.getCount() > 0) {
                cr1.moveToFirst();
                folderId = cr1.getInt(cr1.getColumnIndex("mailboxKey"));
                String containingFolder = getContainingFolderEmail(folderId, context);
                timeStamp = cr1.getString(cr1.getColumnIndex("timeStamp"));
                subjectText.append(cr1.getString(cr1.getColumnIndex("subject")));
                Bmessage bmsg = new Bmessage();

                /* Create a bMessage */
                bmsg.setType("EMAIL");
                bmsg.setBmsg_version("1.0");
                if (cr1.getString(cr1.getColumnIndex("flagRead")).equalsIgnoreCase("1")) {
                    bmsg.setStatus("READ");
                } else {
                    bmsg.setStatus("UNREAD");
                }

                bmsg.setFolder("TELECOM/MSG/" + containingFolder);

                bmsg.setVcard_version("2.1");

                String senderName = null;
                senderName = cr1.getString(cr1.getColumnIndex("fromList"));
                if (senderName.contains(ASCII_STX)) {
                    String[] senderStr = senderName.split(ASCII_STX);
                    if (senderStr !=null && senderStr.length > 0) {
                        bmsg.setOriginatorVcard_name(senderStr[1].trim());
                        bmsg.setOriginatorVcard_email(senderStr[0].trim());
                    }
                } else {
                    bmsg.setOriginatorVcard_name(senderName.trim());
                    bmsg.setOriginatorVcard_email(senderName.trim());
                }

                String recipientName = null;
                String multiRecepients = null;
                recipientName = cr1.getString(cr1.getColumnIndex("toList"));
                if (recipientName.contains(ASCII_STX)) {
                    String[] recepientStr = recipientName.split(ASCII_STX);
                    if (recepientStr !=null && recepientStr.length > 0) {
                        bmsg.setRecipientVcard_name(recepientStr[1].trim());
                        bmsg.setRecipientVcard_email(recepientStr[0].trim());
                    }
                } else if (recipientName.contains(ASCII_SOH)) {
                    multiRecepients = recipientName.replace(ASCII_SOH_C, ';');
                    Log.d(TAG, "  Recepient name   " + multiRecepients);
                    bmsg.setRecipientVcard_name(multiRecepients.trim());
                    bmsg.setRecipientVcard_email(multiRecepients.trim());
                } else {
                    bmsg.setRecipientVcard_name(recipientName.trim());
                    bmsg.setRecipientVcard_email(recipientName.trim());
                }

                bmsg.setBody_charset("UTF-8");
                StringBuilder sb = new StringBuilder();
                Date date = new Date(Long.parseLong(timeStamp));

                boolean boolSubjectLike = true;
                StringBuilder fromField = new StringBuilder();
                StringBuilder toField = new StringBuilder();

                quoted_printable.delete(0, quoted_printable.length());
                convertToQuotedPrintableFormat(subjectText.toString(), boolSubjectLike);
                subjectText.delete(0, subjectText.length());
                subjectText.append(quoted_printable);

                quoted_printable.delete(0, quoted_printable.length());
                convertToQuotedPrintableFormat(bmsg.getOriginatorVcard_name(), boolSubjectLike);
                fromField.append(quoted_printable);

                quoted_printable.delete(0, quoted_printable.length());
                convertToQuotedPrintableFormat(bmsg.getRecipientVcard_name(), boolSubjectLike);
                toField.append(quoted_printable);

                sb.append("From: =?UTF-8?Q?=").append(fromField + "?= <").append(bmsg.getOriginatorVcard_email() + ">").append("\r\n");
                sb.append("To: =?UTF-8?Q?=").append(toField + "?= <").append(bmsg.getRecipientVcard_email() + ">").append("\r\n");

                sb.append("MIME-Version: 1.0").append("\r\n");
                sb.append("Content-Type: text/plain\"").append("\r\n");
                sb.append("Content-Transfer-Encoding: quoted-printable\r\n");
                String time1 = date.toString();
                sb.append("Date: ").append(time1).append("\r\n");
                sb.append("Subject: =?UTF-8?Q?=").append(subjectText + "?=").append("\r\n\r\n");

                /* Query the body table for obtaining the message body */
                Cursor cr2 = null;
                StringBuilder emailBody = new StringBuilder();
                StringBuilder htmlBody = new StringBuilder();
                String whereStr = " messageKey = " + msgHandle;

                try {
                    cr2 = context.getContentResolver().query(Uri.parse(Constants.EMAIL_URI_BODY), null, whereStr, null,
                            null);

                    if (cr2.getCount() > 0) {
                        cr2.moveToFirst();
                        emailBody.append(cr2.getString(cr2.getColumnIndex("textContent")));
                        htmlBody.append(cr2.getString(cr2.getColumnIndex("htmlContent")));
                        if (emailBody.length() != 0) {
                            htmlBody.delete(0, htmlBody.length());
                        } else {
                            emailBody = htmlBody;
                        }
                    }
                } finally {
                    if (cr2 != null) {
                        cr2.close();
                    }
                }

                quoted_printable.delete(0, quoted_printable.length());
                convertToQuotedPrintableFormat(emailBody.toString(), false);   /* GC causing lines */
                emailBody = quoted_printable;
                sb.append(emailBody).append("\r\n");

                bmsg.setBody_msg(sb.toString());
                bmsg.setBody_length(sb.toString().getBytes().length);

                str = MapUtils.toBmessageEmail(bmsg);
                Log.d(TAG, str);

                /* free the String builder object, quoted_printable and the emailBody as well */
                sb.delete(0, sb.length());
                emailBody.delete(0, emailBody.length());
                quoted_printable.delete(0, quoted_printable.length());
            }
        } finally {
            if (cr1 != null) {
                cr1.close();
            }
        }

        return str;
    }

    /**
     * Get the folder name (MAP representation) for Email based on the
     * mailboxKey value in message table
     */
    public String getContainingFolderEmail(int folderId, Context context) {
        Cursor cr = null;
        String folderName = null;
        String whereClause = "_id = " + folderId;

        try {
            cr = context.getContentResolver().query(
                    Uri.parse(Constants.EMAIL_URI_MAILBOX),
                    null, whereClause, null, null);
            if (cr.getCount() > 0) {
                cr.moveToFirst();
                folderName = cr.getString(cr.getColumnIndex("displayName"));
                return folderName;
            }
        } finally {
            if (cr != null) {
                cr.close();
            }
        }
        return null;
    }

    private void convertToQuotedPrintableFormat(String input, boolean boolSubject) {
        byte [] byteRep = null;

        if ((input == null) || (input.compareTo("") == 0)) {
            return;
        } else {
            if (boolSubject == false) {
                input = removeHyperLinksChars(input);
                byteRep = input.getBytes();

                for (int i = 0; i < byteRep.length; i++) {
                    if ((byteRep[i] >= '0') && (byteRep[i] <= '~') && (byteRep[i] != '=')) {
                        quoted_printable.append((char)byteRep[i]);    /* GC causing lines */
                    } else if (byteRep[i] >= ' ') {
                        quoted_printable.append((char)byteRep[i]);    /* GC causing lines */
                    } else if (byteRep[i] == '\n') {
                        quoted_printable.append("=0A");              /* GC causing lines */
                    } else {
                        quoted_printable.append("=" + Integer.toHexString(byteRep[i] & 0xFF).toUpperCase()); /* GC causing lines */
                    }
                }
            } else {
                byteRep = input.getBytes();
                for (int i = 0; i < byteRep.length; i++) {
                    quoted_printable.append("=" + Integer.toHexString(byteRep[i] & 0xFF).toUpperCase());
                }
            }
        }
        return;
    }

    private static String removeHyperLinksChars(String input) {
        String samplestr = input;

        String patternstr = "(<http(.*?)>)";
        String patternstr2 = "(\r\n)+";

        Pattern pattern = Pattern.compile(patternstr2);
        Matcher matcher = pattern.matcher(samplestr);

        samplestr = matcher.replaceAll("\n");

        pattern = Pattern.compile(patternstr);
        matcher = pattern.matcher(samplestr);
        samplestr = matcher.replaceAll("");

        return samplestr;
    }
}
