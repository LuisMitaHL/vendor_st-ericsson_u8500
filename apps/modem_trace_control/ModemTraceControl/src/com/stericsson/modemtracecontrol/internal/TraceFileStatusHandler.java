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

import java.io.File;
import java.io.FileFilter;
import java.util.Arrays;
import java.util.Comparator;

/**
 * Lists the last changed files and directory
 */
public class TraceFileStatusHandler {

    private static String traceFilePath = "/sdcard/modem_trace";

    /**
     * Default constructor
     */
    public TraceFileStatusHandler() {
    }

    /**
     * Lists the four last changed files at the given path.
     * @param path location where to find the four last changed files
     * @return a string containing the four last changed files.
     */
    @SuppressWarnings({
            "unchecked", "rawtypes"
    })
    public String listFourLastChangedFiles(String path) {
        File folder = new File(path);
        if (!folder.isDirectory()) {
            return null;
        }
        File files[] = folder.listFiles();
        String out = "";
        if (files != null) {
            Arrays.sort(files, new Comparator() {
                @Override
                public int compare(final Object o1, final Object o2) {
                    return new Long(((File) o2).lastModified()).compareTo(new Long(((File) o1)
                            .lastModified()));
                }
            });

            int i = 0;
            int j = 0;
            while (j < 4 && i < files.length) {
                if (files[i].isFile()) {
                    int bufidx = files[i].getName().indexOf("_L");
                    String bufname = files[i].getName().substring(bufidx + 1);
                    out += bufname + " (" + files[i].length() / 1024 + " KB)";
                    if (j < 3) {
                        out += "\n";
                    }
                    j++;
                }
                i++;
            }
        }
        return out;
    }

    /**
     * Lists the last changed directory at the given path.
     *
     * @param path location where to find the last changed directory
     * @return a string containing the last changed directory.
     */
    @SuppressWarnings({
        "unchecked", "rawtypes"})
    public String getLastModifiedDirectory(String path) {
        File folder = new File(path);
        if (!folder.isDirectory()) {
            return null;
        }

        FileFilter directoryFilter = new FileFilter() {
            @Override
            public boolean accept(File file) {
                return file.isDirectory();
            }
        };

        File files[] = folder.listFiles(directoryFilter);
        String out = "";
        if (files != null) {
            Arrays.sort(files, new Comparator() {
                @Override
                public int compare(final Object o1, final Object o2) {
                    return new Long(((File) o2).lastModified()).compareTo(new Long(((File) o1).lastModified()));
                }
            });

            if (files.length > 0) {
                out = files[0].getAbsolutePath();
            }
        }
        return out;
    }

    /**
     * Returns the file path where modem traces are to be saved.
     *
     * @return file path where modem traces are to be saved.
     */
    public static String getTraceFilePath() {
        return traceFilePath;
    }

    /**
     * Sets the file path where modem traces are to be saved.
     * @param traceFilePath the path where modem traces are to be saved.
     */
    public static void setTraceFilePath(String traceFilePath) {
        TraceFileStatusHandler.traceFilePath = traceFilePath;
    }

}
