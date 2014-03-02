/*
 * Copyright (C) ST-Ericsson SA 2011
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

package com.stericsson.ril.oem;

public final class STErilOemHookConstants {
        public static final int SEND_OEM_HOOK_MSG     = 0x0A;
        public static final int RESPONSE_MSG          = 0xB0;

        public static final byte OEM_TAG = 0x00;
        public static final byte STE_TAG = (byte)(0xFF & 0xFF);

        // Intent action for unsolicited responses
        public static final String ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW
                        = "com.stericsson.ril.oem.UNSOL_RESPONSE_OEM_HOOK_RAW";

        public static final String ACTION_UNSOL_RESPONSE_STE_HOOK_RAW
                        = "com.stericsson.ril.oem.UNSOL_RESPONSE_STE_HOOK_RAW";
}
