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

import android.test.AndroidTestCase;

import com.stericsson.modemtracecontrol.internal.ConfigFileHandler;

public class ConfigFileHandlerTest extends AndroidTestCase {

    ConfigFileHandler mClassUnderTest;

    private static final String DEFAULT_TRACECONFIG_PATH = "/sdcard/trace_auto.conf";

    private static final String SECOND_TRACECONFIG_PATH = "/sdcard/trace_auto.conf";

    private static final String DEFAULT_TRACEDIR = "/sdcard/modem_trace";

    private static final String FILE_NOT_FOUND = "File not found!";

    private static final String traceAutoConfFileContent =
            "dump -D SDCARD /sdcard/core_dumps/modem\ndump -l 10\n"
                    + "trace -l ON 450 120\ntrace -t STP\nAUTOSTART 0\n"
                    + "trace -D SDCARD /sdcard/modem_trace\ntrace -t BUFFER\n";

    // TODO; How to test the final strings? Getters & setters, or have a
    // MockConfigHandler which extends the real one?

    @Override
    public void setUp() {
        mClassUnderTest = new ConfigFileHandler();
        mClassUnderTest.setTraceAutoConfFilePathToUse(DEFAULT_TRACECONFIG_PATH,
                SECOND_TRACECONFIG_PATH);
    }

    @Override
    public void tearDown() {
        mClassUnderTest = null;
    }

    public void testReadLinesFromFile() {
        String result = mClassUnderTest.readLinesFromFile(DEFAULT_TRACECONFIG_PATH);
        assertEquals(result, traceAutoConfFileContent);
    }

    public void testReadLinesFileNotFoundOnce() {
        String result = mClassUnderTest.readLinesFromFile(SECOND_TRACECONFIG_PATH);
        assertEquals(result, traceAutoConfFileContent);
    }

    public void testReadLinesFileNotFoundTwice() {
        assertEquals(mClassUnderTest.readLinesFromFile(""), FILE_NOT_FOUND);
    }

    public void testGetPathToUseReturnsDefault() {
        assertEquals(mClassUnderTest.getTraceAutoConfFilePathToUse(), DEFAULT_TRACECONFIG_PATH);
    }

    public void testGetPathToUseReturnsSecond() {
        mClassUnderTest.setTraceAutoConfFilePathToUse("non-existing-path",
                SECOND_TRACECONFIG_PATH);

        assertEquals(mClassUnderTest.getTraceAutoConfFilePathToUse(), SECOND_TRACECONFIG_PATH);
    }

    public void testGetPathToUseReturnsNotFound() {
        mClassUnderTest.setTraceAutoConfFilePathToUse("non-existing-path",
                "another-non-existing-path");

        assertEquals(mClassUnderTest.getTraceAutoConfFilePathToUse(), FILE_NOT_FOUND);
    }

    public void testIsFileFoundAndReturnTrue() {
        assertTrue(mClassUnderTest.isFileFound(DEFAULT_TRACECONFIG_PATH));
    }

    public void testIsFileFoundAndReturnFalse() {
        assertFalse(mClassUnderTest.isFileFound(""));
    }

    public void testGetTraceDirPath() {
        assertEquals(mClassUnderTest.getTraceDirectoryPath(), DEFAULT_TRACEDIR);
    }

    public void testGetTraceDirPathNotFound() {
        mClassUnderTest.setTraceAutoConfFilePathToUse("non-existing-path",
                "another-non-existing-path");
        assertEquals(mClassUnderTest.getTraceDirectoryPath(), DEFAULT_TRACEDIR);
    }

}
