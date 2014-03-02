/*
 * Copyright (c) 2011-2012, Modified by ST-Ericsson. All rights reserved.
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
import android.text.format.Time;
import android.util.Log;
import android.util.TimeFormatException;

public class BluetoothMapQueryUtils {

        private static final int DELETED_THREAD_ID = -1;

        private static final String TAG = "BluetoothMapQueryUtils";

        public static String getWhereIsQueryForType(int folder) {

        String query = null;

        switch (folder) {
        case Constants.MAP_FOLDER_INBOX :
            query = "type = 1 AND thread_id <> " + DELETED_THREAD_ID;
            break;
        case Constants.MAP_FOLDER_OUTBOX :
            query = "(type = 4 OR type = 5 OR type = 6) AND thread_id <> " + DELETED_THREAD_ID;
            break;
        case Constants.MAP_FOLDER_SENT :
            query = "type = 2 AND thread_id <> " + DELETED_THREAD_ID;
            break;
        case Constants.MAP_FOLDER_DRAFT :
            query = "type = 3 AND thread_id <> " + DELETED_THREAD_ID;
            break;
        case Constants.MAP_FOLDER_DELETED :
            query = "thread_id = " + DELETED_THREAD_ID;
            break;
        default :
            query = "type = -1";
        }
        return query;

    }
    public static String getConditionStringSms(int folderName, BluetoothMapApplicationParameters appParams) {
        String whereClause = getWhereIsQueryForType(folderName);

        /* Filter readstatus: 0 no filtering, 0x01 get unread, 0x10 get read */
        if (appParams.filterReadStatus != 0) {
            if (appParams.filterReadStatus == 0x01) {
                if (whereClause != "") {
                    whereClause += " AND ";
                }
                whereClause += " read=0 ";
            }
            if (appParams.filterReadStatus == 0x02) {
                if (whereClause != "") {
                    whereClause += " AND ";
                }
                whereClause += " read=1 ";
            }
        }
        /* Filter Period Begin */
        if ((appParams.filterPeriodBegin != null)
                && (appParams.filterPeriodBegin.length() > 0)) {
            Time time = new Time();
            try {
                time.parse(appParams.filterPeriodBegin);
                if (whereClause != "") {
                    whereClause += " AND ";
                }
                whereClause += "date >= " + time.toMillis(false);
            } catch (TimeFormatException e) {
                Log.e(TAG, "Bad formatted filterPeriodBegin "
                        + appParams.filterPeriodBegin);
            }
        }

        /* Filter Period End */
        if ((appParams.filterPeriodEnd != null)
                && (appParams.filterPeriodEnd.length() > 0 )) {
            Time time = new Time();
            try {
                time.parse(appParams.filterPeriodEnd);
                if (whereClause != "") {
                    whereClause += " AND ";
                }
                whereClause += "date < " + time.toMillis(false);
            } catch (TimeFormatException e) {
                Log.e(TAG, "Bad formatted filterPeriodEnd "
                        + appParams.filterPeriodEnd);
            }
        }

        return whereClause;
    }

}
