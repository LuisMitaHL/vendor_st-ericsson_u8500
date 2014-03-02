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

import android.content.Context;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class BluetoothMapFolderAccessor {

    private static final boolean D = true;

    private static final String [] mTelecomMsgFolderList = {"inbox", "sent", "draft", "outbox", "deleted"};

    private static final String TAG = "BluetoothMapFolderAccessor";

    Context mContext;

    /**
     * Contains the number of folder in the current folder
     * under processing, for GetFolderListing and SetFolder
     * operations
     */
    private int mCurrentNumFolders;

    public BluetoothMapFolderAccessor(Context context) {
        this.mCurrentNumFolders = 0;
        mContext = context;
    }

    /**
     * This method selects the kind of folder to read its content
     * and redirects the folder list request to appropriate handlers
     * @param String folderPath
     * @return String[]
     */
    public String [] readFolderList(String folderPath, int MasInstance, BluetoothMapApplicationParameters paramValue) {
        String[] folderList = {""};
        int numFolders;
        EmailUtils eUtil = new EmailUtils();
        List <String> tempFolderList = new ArrayList<String>();

        Log.i(TAG, "Reading folder listing from " + folderPath);

        /* Clear previous value for mCurrentNumFolders */

        this.mCurrentNumFolders = 0;
        if (folderPath.compareToIgnoreCase("/telecom/msg") == 0) {
            /* If SMS mode is on, use only inbox to draft sequence of folders */
            if ((BluetoothMapMessageAccessor.msg_mode & Constants.MAP_MODE_SMS_ON) != 0) {
                numFolders = mTelecomMsgFolderList.length;
                folderList = new String[numFolders];
                this.mCurrentNumFolders = numFolders;
                for (int i = 0; i < mTelecomMsgFolderList.length; i ++) {
                    folderList[i] = mTelecomMsgFolderList[i];
                    tempFolderList.add(folderList[i]);
                }
            }
        } else if (folderPath.compareToIgnoreCase("/telecom") == 0) {
            numFolders = 1;
            folderList = new String[numFolders];
            this.mCurrentNumFolders = numFolders;

            /* Telecom shall contain folder /msg */
            folderList[0] = "msg";
        } else if (folderPath.compareTo("") == 0) {
            /* Empty string means root folder */
            numFolders = 1;
            folderList = new String[numFolders];
            this.mCurrentNumFolders = numFolders;
            folderList[0] = "telecom";
        } else if ((folderPath.compareToIgnoreCase("/telecom/msg/inbox") == 0) || (folderPath.compareToIgnoreCase("/telecom/msg/sent") == 0) ||
                   (folderPath.compareToIgnoreCase("/telecom/msg/draft") == 0) || (folderPath.compareToIgnoreCase("/telecom/msg/outbox") == 0) ||
                   (folderPath.compareToIgnoreCase("/telecom/msg/deleted") == 0)) {
            this.mCurrentNumFolders = 0;
            folderList = null;
        }

        /* Trim the folderList according to the appParam */
        int startOffset, maxNum, currSize, finalSize, lastOffset;
        int k =0, i = 0;
        startOffset = paramValue.listStartOffset;

        if (folderList != null) {
            maxNum = paramValue.maxListCount;
            currSize = folderList.length;

            if ((startOffset >= 0 ) && (startOffset <= currSize) && (maxNum > 0)) {
                finalSize = currSize - startOffset;
                if (finalSize > maxNum) {
                    finalSize = maxNum;
                }
                lastOffset = startOffset + finalSize - 1;
                String [] finalList = new String[finalSize];

                for (i = startOffset, k = 0; i <= lastOffset; i++, k++) {
                    finalList[k] = folderList[i];
                }
                folderList = finalList;
                mCurrentNumFolders = folderList.length;
            } else if (startOffset > currSize) {
                mCurrentNumFolders = 0;
                folderList = null;
            }
        }

        return folderList;
    }

    public int getFolderListSize() {

        return this.mCurrentNumFolders;
    }
    /**
     * This method creates folder listing objects after retrieving list of
     * folders from given path within the message store.
     * @param folderPath
     * @return
     */
    public String createFolderListing(String folderPath, int MasInstance, BluetoothMapApplicationParameters paramValue) {
        String str = "";
        String[] list = null;

        list = readFolderList(folderPath, MasInstance, paramValue);
        str = str.concat("<?xml version=\"1.0\"?>\n");
        str = str.concat("<!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\">\n");
        str = str.concat("<folder-listing version=\"1.0\">\n");

        if ((list != null) && (list.length > 0)) {
            for (int i = 0; i < list.length; i++) {
                str = str.concat(" <folder name=\"" + list[i] + "\"/>\n");
            }
        }
        str = str.concat("</folder-listing>");

        return str;
    }

    /**
     * This method checks if the presented folder name is present
     * within the current folder structure of the presented
     * MAP folder.
     * @param pathToValidate
     */
    public int checkPathValidity(String path, String name) {
        int response = Constants.MAP_RESPONSE_OK;
        boolean pathExists = false;

        path.concat(Constants.PATH_SEPARATOR.concat(name));

        Log.i(TAG, "path " + path + " name " + name);

        String [] splitted = path.split("(/+)");
        path = "";
        for (int i = 1; i < splitted.length; i++) {
            path += "/" + splitted[i];
        }

        if ((path.isEmpty() == true) && (name.compareTo("telecom") == 0)) {
            pathExists = true;
        } else if ((path.compareTo("/telecom") == 0) && (name.compareTo("msg") == 0)) {
            pathExists = true;
        } else if (((path.compareTo("/telecom/msg") == 0) && (name.compareTo("inbox") ==0))||
                ((path.compareTo("/telecom/msg") == 0) && (name.compareTo("outbox") == 0))||
                ((path.compareTo("/telecom/msg") == 0) && (name.compareTo("sent") == 0))||
                ((path.compareTo("/telecom/msg") == 0) && (name.compareTo("draft") == 0)) ||
                ((path.compareTo("/telecom/msg") == 0) && (name.compareTo("deleted") == 0))) {
            pathExists = true;
        } else if ((name.isEmpty() == true)) {
            pathExists = true;
        }

        if (pathExists == false) {
            if (D) Log.d(TAG, "invalid path browsing request");
            response = Constants.MAP_INVALID_PATH_REQ;
        }
        return response;
    }

    /**
     * This method converts given path in String form to enumerable
     * path states
     *
     * @param path
     * @return int - Current Path State
     */
    public int getCurrentPathStateFromString(String path) {
        int pathToReturn = Constants.MAP_FOLDER_ROOT;

        Log.d(TAG, "Full path for conversion is " + path);

        String [] splitted = path.split("(/+)");
        path = "";
        for (int i = 1; i < splitted.length; i++) {
            path += "/" + splitted[i];
        }

        if ((path.compareTo("") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_ROOT;
        }

        if ((path.compareTo("/telecom") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_TELECOM;
        } else if ((path.compareTo("/telecom/msg") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_MSG;
        } else if ((path.compareTo("/telecom/msg/inbox") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_INBOX;
        } else if ((path.compareTo("/telecom/msg/outbox") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_OUTBOX;
        } else if ((path.compareTo("/telecom/msg/sent") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_SENT;
        } else if ((path.compareTo("/telecom/msg/draft") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_DRAFT;
        } else if ((path.compareTo("/telecom/msg/deleted") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_DELETED;
        }

        Log.i(TAG, "Path being returned is " + pathToReturn);
        return pathToReturn;
    }

    /**
     * This method converts folder name in String form to enumerable
     * path states
     *
     * @param path
     * @return int - Current Path State
     */
    public int getCurrentPathStateFromFolderName(String name) {
        int pathToReturn = Constants.MAP_FOLDER_INVALID;

        Log.i(TAG, "Folder name for conversion is " + name);

        if ((name.compareTo("") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_ROOT;
        }

        if ((name.compareTo("telecom") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_TELECOM;
        } else if ((name.compareTo("msg") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_MSG;
        } else if ((name.compareTo("inbox") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_INBOX;
        } else if ((name.compareTo("outbox") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_OUTBOX;
        }
        else if ((name.compareTo("sent") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_SENT;
        } else if ((name.compareTo("draft") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_DRAFT;
        } else if ((name.compareTo("deleted") == 0)) {
            pathToReturn = Constants.MAP_FOLDER_DELETED;
        }

        Log.i(TAG, "Path being returned is " + pathToReturn);
        return pathToReturn;
    }

    /**
     * Converts the folder paths present from enumerable
     * defined path state to the String format.
     *
     * @return String
     */
    public static String mapGivenFolderToString(int mGivenPathState) {
        String returnString;

        switch (mGivenPathState) {
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

            Log.i(TAG, "Path being returned is " + returnString);
        }

        return returnString;
    }

    /**
     * Converts the folder paths present from enumerable
     * defined path state to the String format.
     *
     * @return String
     */
    public static String mapGivenFolderToStringWithoutLeadingBar(int mGivenPathState) {
        String returnString;

        switch (mGivenPathState) {
        case Constants.MAP_FOLDER_ROOT:
            /* Use empty string for root */
            returnString = "";
            break;
        case Constants.MAP_FOLDER_TELECOM:
            returnString = "telecom";
            break;
        case Constants.MAP_FOLDER_MSG:
            returnString = "telecom/msg";
            break;
        case Constants.MAP_FOLDER_INBOX:
            returnString = "telecom/msg/inbox";
            break;
        case Constants.MAP_FOLDER_OUTBOX:
            returnString = "telecom/msg/outbox";
            break;
        case Constants.MAP_FOLDER_SENT:
            returnString = "telecom/msg/sent";
            break;
        case Constants.MAP_FOLDER_DELETED:
            returnString = "telecom/msg/deleted";
            break;
        case Constants.MAP_FOLDER_DRAFT:
            returnString = "telecom/msg/draft";
            break;
        default:
            returnString = "";

        }

        return returnString;
    }
}
