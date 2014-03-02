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

import android.util.Log;

public class BluetoothMapApplicationParameters {

    private final String TAG = "BluetoothMapApplicationParameters";

    public int maxListCount = 1024;

    public int listStartOffset = 0;

    public int folderListingSize = 0;

    public int subjectLength = 255;

    public int parameterMask = 0xFFFF;

    public int filterMessageType = (Constants.MESSAGE_TYPE_MASK_SMS_CDMA) |
                                (Constants.MESSAGE_TYPE_MASK_EMAIL)|
                                (Constants.MESSAGE_TYPE_MASK_SMS_GSM) |
                                (Constants.MESSAGE_TYPE_MASK_MMS);

    public String filterPeriodBegin = null;

    public String filterPeriodEnd = null;

    public int filterReadStatus = 0 ;

    public String filterRecipient = null;

    public String filterOriginator = null;

    public int filterPriority = 0;

    public int attachment = -1;

    public int charSet = -1;

    public boolean isCharSetPresent = false;

    public int fractionRequest = -1;

    public boolean isFractionRequestPresent = false;

    public int fractionDeliver = -1;

    public int transparent = 0;

    public int retry = 1;

    public int statusIndicator = 0xFFFF;

    public int statusValue = 0xFFFF;

    public int notificationStatus = 0;

    public static boolean delNotification = true;

    public void logApplicationParameters() {
        Log.i(TAG, "maxListCount " + maxListCount);

        Log.i(TAG, "listStartOffset " +  listStartOffset);

        Log.i(TAG, "folderListingSize " +  folderListingSize);

        Log.i(TAG, "subjectLength " +  subjectLength);

        Log.i(TAG, "parameterMask " +  parameterMask);

        Log.i(TAG, "filterMessageType " +  filterMessageType);

        Log.i(TAG, "filterPeriodBegin " +  filterPeriodBegin);

        Log.i(TAG, "filterPeriodEnd " +  filterPeriodEnd);

        Log.i(TAG, "filterReadStatus " +  filterReadStatus);

        Log.i(TAG, "filterRecipient " +  filterRecipient);

        Log.i(TAG, "filterOriginator " +  filterOriginator);

        Log.i(TAG, "filterPriority " +  filterPriority);

        Log.i(TAG, "attachment " +  attachment);

        Log.i(TAG, "charSet " +  charSet);

        Log.i(TAG, "fractionRequest " +  fractionRequest);

        Log.i(TAG, "isFractionRequestPresent " +  isFractionRequestPresent);

        Log.i(TAG, "fractionDeliver " +  fractionDeliver);
    }
};
