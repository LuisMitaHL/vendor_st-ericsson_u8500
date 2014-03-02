/*
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

import android.util.Log;
import android.util.Xml;
import java.io.IOException;
import java.io.StringWriter;
import java.util.List;
import org.xmlpull.v1.XmlSerializer;

public class MapUtils {

    private static final String CRLF = "\r\n";

    /**
     * msgListingGetHdrXML
     *
     * This method returns a String with the XML header
     *
     * @return This method returns a String
     */

    public static String msgListingGetHdrXML() {
        String str1 = "<MAP-msg-listing version = \"1.0\">\n";
        return str1;
    }

    /**
     * msgListingGetFooterXML
     *
     * This method returns a String with the XML footer
     *
     * @return This method returns a String
     */

    public static String msgListingGetFooterXML() {
        String str1 = "</MAP-msg-listing>\n";
        return str1;
    }

    /**
     * msgListingGetMsgsXML
     *
     * This method takes a list of message objects and returns a String with the
     * XML messages
     *
     * @param list
     *            An array of message objects where each element represents a
     *            message
     * @return This method returns either null or a String
     */

    public static String msgListingGetMsgsXML(List<MsgListingConsts> list) {
        XmlSerializer serializer = Xml.newSerializer();
        StringWriter writer = new StringWriter();
        try {
            String str1;
            serializer.setOutput(writer);
            serializer.startDocument("", false);
            serializer.text("\n");
            for (MsgListingConsts msg : list) {
                serializer.startTag("", "");
                serializer.attribute("", "msg handle", ("" + msg.getMsg_handle()));
                if (msg.getSubject() != null) {
                    serializer.attribute("", "subject", msg.getSubject().toString());
                }
                if (msg.getDatetime() != null) {
                    serializer.attribute("", "datetime", msg.getDatetime().toString());
                }
                if (msg.getSender_name() != null) {
                    if (msg.getSender_name().length() == 1 )
                        serializer.attribute("", "sender_name", msg.getReplyTo_addressing().toString());
                    else
                        serializer.attribute("", "sender_name", msg.getSender_name().toString());
                }

                if (msg.getReplyTo_addressing() != null) {
                    serializer.attribute("", "sender_addressing",
                            msg.getReplyTo_addressing().toString());
                }
                if (msg.getRecepient_name() != null) {
                    if (msg.getRecepient_name().length() == 1)
                        serializer.attribute("", "recipient_name",msg.getRecepient_addressing().toString());
                    else
                        serializer.attribute("", "recipient_name",msg.getRecepient_name().toString());
                }
                if (msg.getRecepient_addressing() != null) {
                    serializer.attribute("", "recipient_addressing",
                            msg.getRecepient_addressing().toString());
                }
                if (msg.getType() != null) {
                    serializer.attribute("", "type", msg.getType().toString());
                }
                if (msg.getSize() != 0) {
                    serializer.attribute("", "size", ("" + msg.getSize()));
                }
                if (msg.getAttachment_size() != -1) {
                    serializer.attribute("", "attachment_size",
                            ("" + Integer.toString(msg.getAttachment_size())));
                }
                if (msg.getContains_text() != null) {
                    serializer.attribute("", "text", msg.getContains_text().toString());
                }
                if (msg.getPriority() != null) {
                    serializer.attribute("", "priority", msg.getPriority().toString());
                }
                if (msg.getRead() != null) {
                    serializer.attribute("", "read", msg.getRead().toString());
                }
                if (msg.getSent() != null) {
                    serializer.attribute("", "sent", msg.getSent().toString());
                }

                if (msg.getReplyTo_addressing() != null) {
                    serializer.attribute("", "replyto_addressing",
                            msg.getReplyTo_addressing().toString());
                }

                serializer.endTag("", "");

            }
            serializer.endDocument();
            str1 = writer.toString();

            str1 = removeMsgHdrSpace(str1);
            int line1 = 0;
            line1 = str1.indexOf("\n");
            if (line1 > 0) {
                return (str1.substring((line1)));
            } else {
                return str1;
            }

        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * mapEventReportXML
     *
     * This method takes a list of map event report object and returns a String
     * with the XML message
     *
     * @param type
     *            Report type (e.g. NewMessage)
     * @param handle
     *            handle created by caller
     * @param folder
     *            Path to folder to use
     * @param Oldfolder
     *            Path to old folder to use
     * @param msgType
     *            Type of message (SMS_GSM, SMS_CDMA)
     *
     * @return This method returns either null or a String
     */

    public static String mapEventReportXML(String type, String handle, String folder,
            String oldFolder, String msgType) {
        XmlSerializer serializer = Xml.newSerializer();
        StringWriter writer = new StringWriter();

        try {
            String str1;
            serializer.setOutput(writer);
            serializer.startDocument("", false);
            serializer.text("\n");
            serializer.startTag("", "MAP-event-report");
            serializer.attribute("", "version", "1.0");
            serializer.text("\n");

            serializer.startTag("", "");
            if (type != null) {
                serializer.attribute("", "event type", ("" + type));
            }
            if (handle != null) {
                serializer.attribute("", "handle", ("" + handle));
            }
            if (folder != null) {
                serializer.attribute("", "folder", ("" + folder));
            }
            if (oldFolder != null) {
                serializer.attribute("", "old_folder", ("" + oldFolder));
            }

            if (msgType != null) {
                serializer.attribute("", "msg_type", ("" + msgType));
            }
            serializer.endTag("", "");
            serializer.text("\n");
            serializer.endTag("", "MAP-event-report");
            serializer.endDocument();
            str1 = writer.toString();
            int line1 = 0;
            line1 = str1.indexOf("\n");
            if (line1 > 0) {
                int index = str1.indexOf("event type");
                String str2 = "<" + str1.substring(index);
                str1 = "<MAP-event-report version=\"1.0\">" + "\n" + str2;
                return str1;
            } else {
                return str1;
            }

        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    /**
     * toBmessageSMS
     *
     * This method takes as input a list of Bmessage objects and creates a
     * String in the bMessage format.
     *
     * @param list
     *            An array of message objects where each element represents a
     *            message
     * @return This method returns either null or a String
     */

    public static String toBmessageSMS(Bmessage bmsg) {

        StringBuilder sb = new StringBuilder();

        try {
            sb.append("BEGIN:BMSG");
            sb.append("\r\n");
            if (bmsg.getBmsg_version() != null) {
                sb.append("VERSION:").append(bmsg.getBmsg_version()).append("\r\n");
            }
            if (bmsg.getStatus() != null) {
                sb.append("STATUS:").append(bmsg.getStatus()).append("\r\n");
            }
            if (bmsg.getType() != null) {
                sb.append("TYPE:").append(bmsg.getType()).append("\r\n");
            }
            if (bmsg.getFolder() != null) {
                sb.append("FOLDER:").append(bmsg.getFolder()).append("\r\n");
            }
            /* Originator */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getOriginatorVcard_name() != null) {
                sb.append("N:").append(bmsg.getOriginatorVcard_name()).append("\r\n");
            }
            if (bmsg.getOriginatorVcard_phone_number() != null) {
                sb.append("TEL:").append(bmsg.getOriginatorVcard_phone_number()).append("\r\n");
            }

            sb.append("END:VCARD").append("\r\n");
            /* End Originator */

            sb.append("BEGIN:BENV").append("\r\n");

            /* Recipient */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getRecipientVcard_name() != null) {
                sb.append("N:").append(bmsg.getRecipientVcard_name()).append("\r\n");
            }
            if ((bmsg.getRecipientVcard_phone_number() != null) && (bmsg.getRecipientVcard_phone_number().trim().isEmpty() != true)) {
                sb.append("TEL:").append(bmsg.getRecipientVcard_phone_number()).append("\r\n");
            }
            sb.append("END:VCARD").append("\r\n");
            /* End Recipient */

            sb.append("BEGIN:BBODY").append("\r\n");

            if (bmsg.getBody_charset() != null) {
                sb.append("CHARSET:").append(bmsg.getBody_charset()).append("\r\n");
            }

            if (bmsg.getBody_encoding() != null && bmsg.getBody_charset() != "UTF-8") {
                sb.append("ENCODING:").append(bmsg.getBody_encoding()).append("\r\n");
            }
            if (bmsg.getBody_length() != 0) {
                sb.append("LENGTH:").append(bmsg.getBody_length()).append("\r\n");
            }
            if (bmsg.getBody_msg() != null) {
                sb.append("BEGIN:MSG\r\n");
                sb.append(bmsg.getBody_msg()).append("\r\n");
                sb.append("END:MSG\r\n");

            }

            sb.append("END:BBODY").append("\r\n");
            sb.append("END:BENV").append("\r\n");

            sb.append("END:BMSG");
            sb.append("\r\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return sb.toString();

    }

    /**
     * toBmessageMMS
     *
     * This method takes as input a list of Bmessage objects and creates a
     * String in the bMessage format.
     *
     * @param list
     *            An array of message objects where each element represents a
     *            message
     * @return This method returns either null or a String
     */

    public static String toBmessageMMS(Bmessage bmsg) {

        StringBuilder sb = new StringBuilder();

        try {
            sb.append("BEGIN:BMSG");
            sb.append("\r\n");
            if (bmsg.getBmsg_version() != null) {
                sb.append("VERSION:").append(bmsg.getBmsg_version()).append("\r\n");
            }
            if (bmsg.getStatus() != null) {
                sb.append("STATUS:").append(bmsg.getStatus()).append("\r\n");
            }
            if (bmsg.getType() != null) {
                sb.append("TYPE:").append(bmsg.getType()).append("\r\n");
            }
            if (bmsg.getFolder() != null) {
                sb.append("FOLDER:").append(bmsg.getFolder()).append("\r\n");
            }
            /* Originator */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getOriginatorVcard_name() != null) {
                sb.append("N:").append(bmsg.getOriginatorVcard_name()).append("\r\n");
            }
            if (bmsg.getOriginatorVcard_phone_number() != null) {
                sb.append("TEL:").append(bmsg.getOriginatorVcard_phone_number()).append("\r\n");
            }

            sb.append("END:VCARD").append("\r\n");
            /* End Originator */

            sb.append("BEGIN:BENV").append("\r\n");

            /* Recipient */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getRecipientVcard_name() != null) {
                sb.append("N:").append(bmsg.getRecipientVcard_name()).append("\r\n");
            }
            if (bmsg.getRecipientVcard_phone_number() != null) {
                sb.append("TEL:").append(bmsg.getRecipientVcard_phone_number()).append("\r\n");
            }
            sb.append("END:VCARD").append("\r\n");
            /* End Recipient */

            sb.append("BEGIN:BBODY").append("\r\n");

            sb.append("PARTID:26988").append("\r\n");

            if (bmsg.getBody_encoding() != null) {
                sb.append("ENCODING:").append(bmsg.getBody_encoding()).append("\r\n");
            }
            sb.append("CHARSET:UTF-8").append("\r\n");
            sb.append("LANGUAGE:").append("\r\n");

            if (bmsg.getBody_length() != 0) {
                sb.append("LENGTH:").append(bmsg.getBody_length()).append("\r\n");
            }
            if (bmsg.getBody_msg() != null) {
                sb.append("BEGIN:MSG\r\n");
                sb.append(bmsg.getBody_msg()).append("\r\n");
                sb.append("END:MSG\r\n");

            }

            sb.append("END:BBODY").append("\r\n");
            sb.append("END:BENV").append("\r\n");

            sb.append("END:BMSG");
            sb.append("\r\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return sb.toString();

    }

    /**
     * toBmessageEmail
     *
     * This method takes as input a list of Bmessage objects and creates a
     * String in the bMessage format.
     *
     * @param list
     *            An array of message objects where each element represents a
     *            message
     * @return This method returns either null or a String
     */

    public static String toBmessageEmail(Bmessage bmsg) {

        StringBuilder sb = new StringBuilder();

        try {
            sb.append("BEGIN:BMSG");
            sb.append("\r\n");
            if (bmsg.getBmsg_version() != null) {
                sb.append("VERSION:").append(bmsg.getBmsg_version()).append("\r\n");
            }
            if (bmsg.getStatus() != null) {
                sb.append("STATUS:").append(bmsg.getStatus()).append("\r\n");
            }
            if (bmsg.getType() != null) {
                sb.append("TYPE:").append(bmsg.getType()).append("\r\n");
            }
            if (bmsg.getFolder() != null) {
                sb.append("FOLDER:").append(bmsg.getFolder()).append("\r\n");
            }

            /* Originator */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getOriginatorVcard_name() != null) {
                sb.append("N:").append(bmsg.getOriginatorVcard_name()).append("\r\n");
            }
            sb.append("TEL:").append("\r\n");
            if (bmsg.getOriginatorVcard_email() != null) {
                sb.append("EMAIL:").append(bmsg.getOriginatorVcard_email()).append("\r\n");
            }

            sb.append("END:VCARD").append("\r\n");
            /* End Originator */

            sb.append("BEGIN:BENV").append("\r\n");

            /* Recipient */
            sb.append("BEGIN:VCARD").append("\r\n");

            if (bmsg.getVcard_version() != null) {
                sb.append("VERSION:").append(bmsg.getVcard_version()).append("\r\n");
            }
            if (bmsg.getRecipientVcard_name() != null) {
                sb.append("N:").append(bmsg.getRecipientVcard_name()).append("\r\n");
            }
            if (bmsg.getRecipientVcard_name() != null) {
                sb.append("FN:").append(bmsg.getRecipientVcard_name()).append("\r\n");
            }
            sb.append("TEL:").append("\r\n");
            if (bmsg.getRecipientVcard_email() != null) {
                sb.append("EMAIL:").append(bmsg.getRecipientVcard_email()).append("\r\n");
            }
            sb.append("END:VCARD").append("\r\n");
            /* End Recipient */

            sb.append("BEGIN:BBODY").append("\r\n");

            if (bmsg.getBody_charset() != null) {
                sb.append("CHARSET:").append(bmsg.getBody_charset()).append("\r\n");
            } else {
                sb.append("CHARSET:UTF-8").append("\r\n");
            }

            if (bmsg.getBody_length() != 0) {
                sb.append("LENGTH:").append(bmsg.getBody_length()).append("\r\n");
            }
            if (bmsg.getBody_msg() != null) {
                sb.append("BEGIN:MSG\r\n");
                sb.append(bmsg.getBody_msg()).append("\r\n");
                sb.append("END:MSG\r\n");

            }

            sb.append("END:BBODY").append("\r\n");
            sb.append("END:BENV").append("\r\n");

            sb.append("END:BMSG");
            sb.append("\r\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return sb.toString();

    }


    /**
     * fromBmessageSMS
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and loads a Bmessage object that is returned
     *
     * @param String
     *            - which is a bMessage formatted SMS message
     * @return This method returns a Bmessage object
     */

    public static Bmessage fromBmessageSMS(String bmsg) {

        Bmessage bMsgObj = new Bmessage();

        /* Extract Telephone number of sender */
        String phoneNumber = null;
        String vCard = null;
        vCard = fetchRecipientVcard(bmsg);
        phoneNumber = fetchVcardTel(vCard);
        bMsgObj.setRecipientVcard_phone_number(phoneNumber);

        /* Extract vCard Version */
        bMsgObj.setVcard_version(fetchVcardVersion(vCard));

        /* Extract vCard Name */
        bMsgObj.setVcard_version(fetchVcardVersion(vCard));

        /* Extract bMessage Version */
        bMsgObj.setBmsg_version(fetchVersion(bmsg));

        /* Extract Message Status */
        bMsgObj.setStatus(fetchReadStatus(bmsg));

        /* Extract Message Type */
        bMsgObj.setType(fetchType(bmsg));

        /* Extract Message Folder */
        bMsgObj.setFolder(fetchFolder(bmsg));

        /* Fetch Message Length */
        bMsgObj.setBody_length(fetchBodyLength(bmsg));

        /* Extract Message */
        bMsgObj.setBody_msg(fetchBodyMsg(bmsg));

        /* Extract Message encoding */
        bMsgObj.setBody_encoding(fetchBodyEncoding(bmsg));

        return bMsgObj;

    }

    /**
     * fromBmessageMMS
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and loads a Bmessage object that is returned
     *
     * @param String
     *            - which is a bMessage formatted SMS message
     * @return This method returns a Bmessage object
     */

    public static Bmessage fromBmessageMMS(String bmsg) {

        Bmessage bMsgObj = new Bmessage();

        /* Extract Telephone number of sender */
        String phoneNumber = null;
        String vCard = null;
        vCard = fetchRecipientVcard(bmsg);

        if (vCard.indexOf("EMAIL:") > 0) {
            phoneNumber = fetchVcardEmailforMms(vCard);
        } else {
            phoneNumber = fetchVcardTel(vCard);
        }

        bMsgObj.setRecipientVcard_phone_number(phoneNumber);

        /* Extract vCard Version */
        bMsgObj.setVcard_version(fetchVcardVersion(vCard));

        /* Extract vCard Name */
        bMsgObj.setVcard_version(fetchVcardVersion(vCard));

        /* Extract bMessage Version */
        bMsgObj.setBmsg_version(fetchVersion(bmsg));

        /* Extract Message Status */
        bMsgObj.setStatus(fetchReadStatus(bmsg));

        /* Extract Message Type */
        bMsgObj.setType(fetchType(bmsg));

        /* Extract Message Folder */
        bMsgObj.setFolder(fetchFolder(bmsg));

        /* Fetch Message Length */
        bMsgObj.setBody_length(fetchBodyLength(bmsg));

        /* Extract Message */
        bMsgObj.setBody_msg(fetchBodyMsg(bmsg));

        /* Extract Message encoding */
        bMsgObj.setBody_encoding(fetchBodyEncoding(bmsg));

        return bMsgObj;

    }

    /**
     * fromBmessageEmail
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and loads a Bmessage object that is returned
     *
     * @param String
     *            - which is a bMessage formatted Email message
     * @return This method returns a Bmessage object
     */

    public static Bmessage fromBmessageEmail(String bmsg) {

        Bmessage bMsgObj = new Bmessage();
        Log.d("MapUtils", "Inside fromBmessageEmail method ");
        /* Extract Telephone number of sender */
        String email = null;
        String vCard = null;
        vCard = fetchRecepientVcardEmail(bmsg);
        Log.d("MapUtils", "vCard Info  "+vCard);
        email = fetchVcardEmail(vCard);

        Log.d("MapUtils", "email Info  "+email);
        bMsgObj.setRecipientVcard_email(email);

        String vcardOrig = fetchOriginatorVcardEmail(bmsg);
        String emailOrig = fetchVcardEmail(vcardOrig);
        Log.d("MapUtils", "Vcard Originator Email  "+emailOrig);
        bMsgObj.setOriginatorVcard_email(emailOrig);

        Log.d("MapUtils", "Vcard Originatore Name  "+fetchVcardName(vcardOrig));
        String nameOrig = fetchVcardName(vcardOrig);
        bMsgObj.setOriginatorVcard_name(nameOrig);

        Log.d("MapUtils", "Vcard version  "+fetchVcardVersion(vCard));
        /* Extract vCard Version */
        bMsgObj.setVcard_version(fetchVcardVersion(vCard));

        /* Extract vCard Name */

        Log.d("MapUtils", "Bmsg version  "+fetchVersion(bmsg));
        /* Extract bMessage Version */
        bMsgObj.setBmsg_version(fetchVersion(bmsg));

        Log.d("MapUtils", "Read status  "+fetchReadStatus(bmsg));
        /* Extract Message Status */
        bMsgObj.setStatus(fetchReadStatus(bmsg));

        Log.d("MapUtils", "Message Type  "+fetchType(bmsg));
        /* Extract Message Type */
        bMsgObj.setType(fetchType(bmsg));

        Log.d("MapUtils", "Folder  "+fetchFolder(bmsg));
        /* Extract Message Folder */
        bMsgObj.setFolder(fetchFolder(bmsg));

        Log.d("MapUtils", "body length  "+fetchBodyLength(bmsg));
        /* Fetch Message Length */
        bMsgObj.setBody_length(fetchBodyLength(bmsg));

        Log.d("MapUtils", "Message body  "+fetchBodyMsgEmail(bmsg));
        /* Extract Message */
        bMsgObj.setBody_msg(fetchBodyMsgEmail(bmsg));

        Log.d("MapUtils", "Message encoding  "+fetchBodyEncoding(bmsg));
        /* Extract Message encoding */
        bMsgObj.setBody_encoding(fetchBodyEncoding(bmsg));

        /* Extract Subject of the email */
        bMsgObj.setSubject(fetchSubjectEmail(bmsg));

        return bMsgObj;

    }
    /**
     * fetchVcardEmail
     *
     * This method takes as input a vCard formatted String. It parses the String
     * and returns the vCard Email as a String
     *
     * @param
     * @return String This method returns a vCard Email String
     */

    private static String fetchVcardEmail(String vCard) {

        int pos = vCard.indexOf(("EMAIL:"));
        int beginVersionPos = pos + (("EMAIL:").length());
        Log.d("Map Utils","Begin Version Position Email  "+beginVersionPos);
        int endVersionPos = vCard.indexOf("\n", beginVersionPos);
        Log.d("Map Utils","End version Pos Email  "+endVersionPos);
        return vCard.substring(beginVersionPos, endVersionPos);
    }
    private static String fetchRecepientVcardEmail(String bmsg) {

        /* Find the position of the first vCard in the string */
        int pos = bmsg.indexOf("BEGIN:BENV");
        if (pos > 0) {
            Log.d("fetchOriginatorVcard", "vCard start position greater than 0 ");
            int beginVcardPos = pos + ("\r\n".length());
            int endVcardPos = bmsg.indexOf("END:BENV");

            return bmsg.substring(beginVcardPos, endVcardPos);

        } else {

            return null;

        }
    }

    private static String fetchOriginatorVcardEmail(String bmsg) {

        /* Find the position of the first vCard in the string */
        int pos = bmsg.indexOf("BEGIN:VCARD");
        Log.d("fetchOriginatorVcard", "vCard start position  "+pos);
        if (pos > 0) {
            Log.d("fetchOriginatorVcard", "vCard start position greater than 0 ");
            int beginVcardPos = pos + ("\r\n".length());
            int endVcardPos = bmsg.indexOf("END:VCARD");

            return bmsg.substring(beginVcardPos, endVcardPos);

        } else {

            return null;

        }
    }
    private static String fetchSubjectEmail(String body) {

        int pos = body.indexOf("Subject:");

        if (pos > 0) {
            int beginVersionPos = pos + (("Subject:").length());
            int endVersionPos = body.indexOf("\n", beginVersionPos);
            return body.substring(beginVersionPos, endVersionPos);

        } else {

            return null;
        }
    }


    /**
     * fetchVersion
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and returns the bMessage version string
     *
     * @param
     * @return String This method returns a Version String
     */

    private static String fetchVersion(String bmsg) {
        int pos = bmsg.indexOf("VERSION:");
        if (pos > 0) {

            int beginVersionPos = pos + (("VERSION:").length());
            int endVersionPos = bmsg.indexOf(CRLF, beginVersionPos);
            return bmsg.substring(beginVersionPos, endVersionPos);

        } else {

            return null;

        }
    }

    /**
     * fetchRecipientVcard
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String looking for the first envelope. Once it finds the envelop, it then
     * looks for the first vCard and returns it as a String
     *
     * @param
     * @return String This method returns a Vcard String
     */
    private static String fetchRecipientVcard(String bmsg) {

        /* Locate BENV */
        int locBENV = 0;
        int pos = 0;
        locBENV = bmsg.indexOf(CRLF + "BEGIN:BENV");
        pos = bmsg.indexOf(CRLF + "BEGIN:VCARD", locBENV);
        if (locBENV < pos) {
            pos = bmsg.indexOf(CRLF + "BEGIN:VCARD", locBENV);
        } else {
            pos = bmsg.indexOf(CRLF + "BEGIN:VCARD");
        }
        if (pos > 0) {
            int beginVcardPos = pos;
            int endVcardPos = bmsg.indexOf("END:VCARD", pos);
            return bmsg.substring(beginVcardPos, endVcardPos);

        } else {
            return null;
        }

    }

    /**
     * fetchReadStatus
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and returns the bMessage read status String
     *
     * @param
     * @return String This method returns a Read Status String
     */

    private static  String fetchReadStatus(String bmsg) {
        int pos = bmsg.indexOf("STATUS:");
        if (pos > 0) {

            int beginStatusPos = pos + (("STATUS:").length());
            int endStatusPos = bmsg.indexOf(CRLF, beginStatusPos);
            return bmsg.substring(beginStatusPos, endStatusPos);

        } else {

            return null;

        }
    }

    /**
     * fetchType
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and returns the bMessage type String
     *
     * @param
     * @return String This method returns a message type String
     */

    public static String fetchType(String bmsg) {
        int pos = bmsg.indexOf("TYPE:");
        if (pos > 0) {
            int beginTypePos = pos + (("TYPE:").length());
            int endTypePos = bmsg.indexOf(CRLF, beginTypePos);

            return bmsg.substring(beginTypePos, endTypePos);

        } else {
            return null;
        }

    }

    /**
     * fetchFolder
     *
     * This method takes as input a String in the bMessage format. It parses the
     * String and returns the bMessage Folder path String
     *
     * @param
     * @return String This method returns a Folder path String
     */

    private static String fetchFolder(String bmsg) {
        int pos = bmsg.indexOf("FOLDER:");
        if (pos > 0) {
            int beginVersionPos = pos + (("FOLDER:").length());
            int endVersionPos = bmsg.indexOf(CRLF, beginVersionPos);

            return bmsg.substring(beginVersionPos, endVersionPos);

        } else {
            return null;
        }

    }

    /**
     * fetchBodyEncoding
     *
     * This method takes as input a String consisting of the body portion of the
     * bMessage. It parses the String and returns the bMessage Body Encoding as
     * a String
     *
     * @param
     * @return String This method returns a Body Encoding String
     */

    private static String fetchBodyEncoding(String body) {
        int pos = body.indexOf("ENCODING:");
        if (pos > 0) {
            int beginVersionPos = pos + (("ENCODING:").length());
            int endVersionPos = body.indexOf(CRLF, beginVersionPos);
            return body.substring(beginVersionPos, endVersionPos);

        } else {

            return null;
        }
    }

    /**
     * fetchBodyLength
     *
     * This method takes as input a String consisting of the body portion of the
     * bMessage. It parses the String and returns the bMessage Body Length as an
     * Integer
     *
     * @param
     * @return String This method returns a Body Length Integer
     */

    private static Integer fetchBodyLength(String body) {

        int pos = body.indexOf("LENGTH:");
        if (pos > 0) {
            int beginVersionPos = pos + (("LENGTH:").length());
            int endVersionPos = body.indexOf(CRLF, beginVersionPos);
            String bd = body.substring(beginVersionPos,
                                        endVersionPos);
            Integer bl = Integer.valueOf(bd);
            return bl;

        } else {

            return null;
        }

    }

    /**
     * fetchBodyMsg
     *
     * This method takes as input a String consisting of the body portion of the
     * bMessage. It parses the String and returns the bMessage Body Message as a
     * String
     *
     * @param
     * @return String This method returns a Body Message String
     */

    private static String fetchBodyMsg(String body) {
        int pos = body.indexOf("BEGIN:MSG");
        if (pos > 0) {
            int beginVersionPos = pos + (("BEGIN:MSG").length() + CRLF.length());
            int endVersionPos = (body.indexOf("END:MSG", beginVersionPos) - CRLF.length());

            return body.substring(beginVersionPos, endVersionPos);

        } else {

            return null;
        }
    }

    private static String fetchBodyMsgEmail(String body) {
        Log.d("MapUtils", "bMessageEmail inside fetch body  "+body);
        int pos = body.indexOf("Content-Disposition:inline");
        if (pos > 0) {
            int beginVersionPos = pos + (("Content-Disposition:inline").length() + CRLF.length());
            int endVersionPos = (body.indexOf("--RPI-Messaging", beginVersionPos) - CRLF.length());

            return body.substring(beginVersionPos, endVersionPos);

        } else if (pos < 0) {
            pos = body.indexOf("BEGIN:MSG");
            int beginVersionPos = pos + (("BEGIN:MSG").length() + CRLF.length());
            int endVersionPos = (body.indexOf("END:MSG", beginVersionPos) - CRLF.length());

            return body.substring(beginVersionPos, endVersionPos);

        } else {

            return null;
        }
    }

    /**
     * fetchVcardVersion
     *
     * This method takes as input a vCard formatted String. It parses the String
     * and returns the vCard version as a String
     *
     * @param
     * @return String This method returns a vCard version String
     */

    private static String fetchVcardVersion(String vCard) {

        int pos = vCard.indexOf(CRLF + "VERSION:");
        int beginVersionPos = pos + (("VERSION:").length() + CRLF.length());
        int endVersionPos = vCard.indexOf(CRLF, beginVersionPos);

        return vCard.substring(beginVersionPos, endVersionPos);
    }

    /**
     * fetchVcardName
     *
     * This method takes as input a vCard formatted String. It parses the String
     * and returns the vCard name as a String
     *
     * @param
     * @return String This method returns a vCard name String
     */
    private static String fetchVcardName(String vCard) {

        int pos = vCard.indexOf((CRLF + "N:"));
        int beginNPos = pos + "N:".length() + CRLF.length();
        int endNPos = vCard.indexOf(CRLF, beginNPos);
        return vCard.substring(beginNPos, endNPos);
    }

    /**
     * fetchVcardTel
     *
     * This method takes as input a vCard formatted String. It parses the String
     * and returns the vCard phone number as a String
     *
     * @param
     * @return String This method returns a vCard phone number String
     */

    private static String fetchVcardTel(String vCard) {

        int pos = vCard.indexOf((CRLF + "TEL:"));
        int beginVersionPos = pos + (("TEL:").length() + CRLF.length());
        int endVersionPos = vCard.indexOf(CRLF, beginVersionPos);
        return vCard.substring(beginVersionPos, endVersionPos);
    }

    /**
     * removeMsgHdrSpace
     *
     * This method takes as input a String that contains message listings and
     * removes a space between the < and the msg handle value. This space is
     * inserted by the serializer causing an error in processing the message
     * correctly. This private method parses the message and removes the spaces.
     *
     * @param String
     *            Message to be parsed
     * @return String This method returns the message String
     */

    private static String removeMsgHdrSpace(String message) {
        String str1 = null;
        String str2 = null;

        Integer index = 0;
        Integer endSubstring = 0;

        index = message.indexOf("< msg handle");
        if (index < 0) {
            str2 = message;

        } else {
            str2 = message.substring(0, index);
            str2 = str2 + "\n";
            index = 0;
            while ((index = message.indexOf("msg handle", index)) > 0) {
                endSubstring = message.indexOf("/>", index) + "/>".length();
                str1 = "<" + message.substring(index, endSubstring);
                index = index + 1;
                str2 = str2 + str1;

            }
        }

        return str2 + "</MAP-msg-listing>";

    }
    /**
     * fetchVcardEmail
     *
     * This method takes as input a vCard formatted String. It parses the String
     * and returns the vCard phone number as a String
     *
     * @param
     * @return String This method returns a vCard phone number String
     */

    private static String fetchVcardEmailforMms(String vCard) {

        int pos = vCard.indexOf((CRLF + "EMAIL:"));
        int beginVersionPos = pos + (("EMAIL:").length() + CRLF.length());
        int endVersionPos = vCard.indexOf(CRLF, beginVersionPos);
        return vCard.substring(beginVersionPos, endVersionPos);
    }
}
