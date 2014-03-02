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

package com.stericsson.modemtracecontrol.test.internal;

import java.io.File;

import android.test.AndroidTestCase;

import com.stericsson.modemtracecontrol.internal.TraceFileStatusHandler;

public class TraceFileStatusHandlerTest extends AndroidTestCase {

    private TraceFileStatusHandler mClassUnderTest;

    @Override
    public void setUp() {
        mClassUnderTest = new TraceFileStatusHandler();
    }

    @Override
    public void tearDown() {
        mClassUnderTest = null;
    }

    public void testDirListDirHasFiles() {
        String result = mClassUnderTest.listFourLastChangedFiles(".");
        String refString =
                "default.prop (0 KB)\ninit (91 KB)\ninit.goldfish.rc (1 KB)\ninit.rc (13 KB)";
        assertEquals(result, refString);
    }

    public void testDirListDirHasNoFiles() {
        assertEquals(mClassUnderTest.listFourLastChangedFiles("/config"), "");
    }

    public void testDirListInvalidDir() {
        assertNull(mClassUnderTest.listFourLastChangedFiles(""));
    }

    public void testGetTraceFilePath() {
        assertEquals(TraceFileStatusHandler.getTraceFilePath(), "/sdcard/modem_trace");
    }

    public void testSetTraceFilePath() {
        String newPath = "new";
        TraceFileStatusHandler.setTraceFilePath(newPath);
        assertEquals(TraceFileStatusHandler.getTraceFilePath(), newPath);
    }

    public void testgetLastModifiedDirectoryInvalidDir() {
        assertNull(mClassUnderTest.getLastModifiedDirectory(""));
    }

    public void testgetLastModifiedDirectoryEmptyDir() {
        assertEquals(mClassUnderTest.getLastModifiedDirectory("/config"), "");
    }
}
