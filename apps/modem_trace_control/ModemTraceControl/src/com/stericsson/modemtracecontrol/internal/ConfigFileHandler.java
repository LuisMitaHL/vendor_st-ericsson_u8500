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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileFilter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Handles the trace_auto.conf file. Reading/parsing and writing to it.
 */
public class ConfigFileHandler {

    private static String DEFAULT_TRACECONFIG_PATH = "/etc/trace_auto.conf";

    private static String SECOND_TRACECONFIG_PATH = "/modemfs/trace_auto.conf";

    private static final String TRACE_DEST_CMD = "trace -D SDCARD";

    private static final String DEFAULT_TRACEDIR = "/sdcard/modem_trace";

    private String mPresetFilesDirectory = "/sdcard/preset_files";

    private List<String> mFilelist;

    /**
     * Default constructor
     */
    public ConfigFileHandler() {
        mFilelist = new ArrayList<String>();
    }

    /**
     * Returns which trace_auto.conf file path to use. If the default path does
     * not exist, a secondary path is tried. If that one doesn't exist either,
     * "File not found" is returned.
     *
     * @return the path to a trace_auto.conf file
     */
    public String getTraceAutoConfFilePathToUse() {
        if (isFileFound(DEFAULT_TRACECONFIG_PATH)) {
            return DEFAULT_TRACECONFIG_PATH;
        } else if (isFileFound(SECOND_TRACECONFIG_PATH)) {
            return SECOND_TRACECONFIG_PATH;
        } else {
            return Utility.FILE_NOT_FOUND;
        }
    }

    /**
     * Sets the primary and the secondary path of where to find the
     * trace_auto.conf file. Only used for test.
     *
     * @param primaryPath - the primary search path
     * @param secondaryPath - the secondary search path
     */
    public void setTraceAutoConfFilePathToUse(String primaryPath, String secondaryPath) {
        DEFAULT_TRACECONFIG_PATH = primaryPath;
        SECOND_TRACECONFIG_PATH = secondaryPath;
    }

    /**
     * Returns true or false whether the file at the given path exists or not.
     *
     * @param filePath path to check whether it is found or not
     * @return true if the file at the given filePath is found, false otherwise.
     */
    public boolean isFileFound(String filePath) {
        File file = new File(filePath);
        return file.exists();
    }

    /**
     * Reads the file at the provided path and returns the content as a string.
     *
     * @param filepath the path of the file to read
     * @return the file content as a string
     * @throws IOException
     */
    public String readLinesFromFile(String filepath) {
        if (isFileFound(filepath)) {
            FileReader fileReader;
            try {
                fileReader = new FileReader(filepath);
            } catch (FileNotFoundException e) {
                return Utility.FILE_NOT_FOUND;
            }
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            String lines = "";
            String line = "";
            try {
                while ((line = bufferedReader.readLine()) != null) {
                    lines += line + "\n";
                }
            } catch (IOException e) {
                return Utility.FILE_NOT_FOUND;
            }
            try {
                bufferedReader.close();
            } catch (IOException e) {
                // Error closing the reader does not necessarily mean
                // file was not correctly found and parsed.
                // Print the error and proceed.
                e.printStackTrace();
            }
            return lines;
        } else {
            return Utility.FILE_NOT_FOUND;
        }
    }

    /**
     * Saves the provided text as the new trace_auto.conf file.
     *
     * @param text - the trace_auto.conf text to save.
     */
    public void saveTraceAutoConfText(String absolutePath) {
        String path = getTraceAutoConfFilePathToUse();
        copyFile(absolutePath, path);
    }

    /**
     * Copies a file from a given source to a given destination.
     * Overwrites the file if it already exists.
     *
     * @param from - the source
     * @param to - the destination
     */
    public void copyFile(String from, String to) {
        File inputFile = new File(from);
        File outputFile = new File(to);
        try {
            FileReader in = new FileReader(inputFile);
            FileWriter out = new FileWriter(outputFile);
            int c;
            while ((c = in.read()) != -1)
                out.write(c);
            in.close();
            out.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Retrieves the trace directory path from trace_auto.conf file and returns
     * it. If no trace_auto.conf file found, returns default trace directory.
     *
     * @return the trace directory path from the trace_auto.conf file
     */
    public String getTraceDirectoryPath() {
        String path = DEFAULT_TRACEDIR;

        File file = new File(getTraceAutoConfFilePathToUse());
        Scanner fileScanner;
        try {
            fileScanner = new Scanner(file);
        } catch (FileNotFoundException e) {
            // If no trace_auto.conf file found, return default dir
            return path;
        }
        Pattern pattern = Pattern.compile(Utility.TRACE_DESTINATION_COMMAND);
        Matcher matcher = null;

        while (fileScanner.hasNextLine()) {
            String line = fileScanner.nextLine();
            matcher = pattern.matcher(line);
            if (matcher.find()) {
                path = line.substring(matcher.end(), line.length());
            }
        }
        fileScanner.close();
        return path;
    }

    public List<String> getFileList() {
        mFilelist.add("trace_auto.conf");
        File presetFilesDir = new File(mPresetFilesDirectory);
        if (!presetFilesDir.isDirectory()) {
            return null;
        }
        File presetFiles[] = presetFilesDir.listFiles(new ConfigFileFilter());
        for(File f:presetFiles){
            mFilelist.add(f.getName());
        }
        return mFilelist;
    }

    public String getPresetFilesLocation() {
        return  mPresetFilesDirectory;
    }

    public void setPresetFilesDirectory(String presetFilesDirectory) {
        mFilelist.clear();
        mPresetFilesDirectory = presetFilesDirectory;
    }

    private class ConfigFileFilter implements FileFilter
    {
        private final String[] okFileExtensions = new String[] {"conf"};

        @Override
        public boolean accept(File file) {
            for (String extension : okFileExtensions) {
                if (file.getName().toLowerCase().endsWith(extension)) {
                    return true;
                }
            }
            return false;
        }
    }
}
