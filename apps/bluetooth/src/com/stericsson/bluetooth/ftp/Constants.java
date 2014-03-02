/*
 * Copyright (c) 2008-2009, Motorola, Inc.
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
 * - Neither the name of the Motorola, Inc. nor the names of its contributors
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

package com.stericsson.bluetooth.ftp;

import android.os.SystemProperties;

/** Defines global constants for FTP server */
public class Constants {
    public static final boolean DEBUG = (SystemProperties.getInt("ro.debuggable", 0) == 1);
    public static final boolean VERBOSE = (SystemProperties.getInt("debug.bt.ftp", 0) == 1);

    public static final String PATH_SEPARATOR = "/";
    public static final String ROOT_CONTAINER_FILE = "root.config";

    /** Intents for handling incoming connection authorization */
    public static final String ACTION_AUTHORIZE_REQUEST = "stericsson.btftp.intent.action.AUTHORIZE_REQUEST";
    public static final String ACTION_AUTHORIZE_ALLOWED = "stericsson.btftp.intent.action.AUTHORIZE_ALLOWED";
    public static final String ACTION_AUTHORIZE_DISALLOWED = "stericsson.btftp.intent.action.AUTHORIZE_DISALLOWED";
    public static final String ACTION_AUTHORIZE_TIMEOUT = "stericsson.btftp.intent.action.AUTHORIZE_TIMEOUT";
    public static final String EXTRA_ALWAYS_ALLOWED = "stericsson.btftp.intent.extra.ALWAYS_ALLOWED";
    public static final String EXTRA_REMOTE_NAME = "stericsson.btftp.intent.extra.REMOTE_NAME";
    public static final int AUTHORIZE_TIMEOUT_VALUE = 30000;

    /** Intents for handling media file scanning */
    public static final String ACTION_SCAN_REQUEST = "stericsson.btftp.intent.action..SCAN_REQUEST";
    public static final String EXTRA_SCANTYPE = "stericsson.btftp.intent.extra.SCANTYPE";
    public static final String EXTRA_SCANPATH = "stericsson.btftp.intent.extra.SCANPATH";
    public static final String EXTRA_MIMETYPE = "stericsson.btftp.intent.extra.MIMETYPE";
    public static final int SCAN_TYPE_NEW = 1;
    public static final int SCAN_TYPE_DELETED = 2;
}
