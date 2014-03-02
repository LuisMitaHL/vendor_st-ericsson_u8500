/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.stericsson.modemtracecontrol.internal;

/**
 * Defines common attributes.
 */
public class Utility {

    public static final int SUCCESS = 0;

    public static final int FAILURE = -1;

    public static final Boolean DEBUG_PROGRAM = false;

    public static final String TRACE_DESTINATION_COMMAND = "trace -D SDCARD ";

    public static final String TRACE_ROUTE_PRE_COMMAND = "trace -t "; // Ends with a single space.

    public static final String TRACE_ROLFILE_PRE_COMMAND = "trace -l "; // Ends with a single space.

    public static final String TRACE_START_COMMAND = "trace -t BUFFER";

    public static final String TRACE_STOP_COMMAND = "trace -c";

    public static final String TRACE_TRIGGER_COMMAND = "trace -r";

    public static final String TRACE_SDCARD_LOGGING_COMMAND = "trace -q SDCARD";

    public static final String COMMAND_RESP_OK = "OK";

    public static final String COMMAND_RESP_KO = "KO";

    public static final String APP_NAME = "MTC";

    public static final String TRACE_REPARSE_AUTOCONF_COMMAND = "trace -i";

    public static final String TRACE_OVERWRITE_AUTOCONF_COMMAND = "trace -O ";

    public static final String FILE_NOT_FOUND = "File not found!";

    public enum MessageType {
        INFO, ERROR
    }
}
