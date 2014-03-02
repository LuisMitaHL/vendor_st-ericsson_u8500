package com.stericsson.sdk.backend.internal;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * 
 * @author xolabju
 * 
 */
public final class FlashToolBackendConfigurationServiceHelper {

    private static final String COMMENT_PREFIX = "#";

    private FlashToolBackendConfigurationServiceHelper() {
    }

    /**
     * Reads a file into a configuration record list
     * 
     * @param file
     *            the file to read
     * @param configurationRecords
     *            the configuration list to read into
     * @throws IOException
     *             on errors
     */
    public static void read(File file, List<IConfigurationRecord> configurationRecords) throws IOException {
        if (configurationRecords == null) {
            throw new IOException("Configuration records are null");
        }
        FileInputStream input = null;
        try {
            input = new FileInputStream(file);
            read(input, configurationRecords);
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    /**
     * Reads a file into a configuration record list
     * 
     * @param input
     *            the input stream to read
     * @param configurationRecords
     *            the configuration list destination
     * @throws IOException
     *             on errors
     */
    public static void read(InputStream input, List<IConfigurationRecord> configurationRecords) throws IOException {
        if (configurationRecords == null) {
            throw new IOException("Configuration records are null");
        }
        configurationRecords.clear();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        String line;

        try {
            line = reader.readLine();
            while ((line != null) && (!"".equals(line))) {
                line = line.trim();
                if (!line.startsWith(COMMENT_PREFIX)) {
                    int commentPrefixIndex = line.indexOf(COMMENT_PREFIX);
                    if (commentPrefixIndex > 0) {
                        line = line.substring(0, commentPrefixIndex);
                        line = line.trim();
                    }
                    configurationRecords.add(MESPConfigurationRecord.parse(line));
                }
                line = reader.readLine();
            }
        } catch (MESPConfigurationException e) {
            IOException ioe = new IOException("Could not read configuration");
            ioe.initCause(e);
            throw ioe;
        }
    }

    /**
     * Writes a configuration record list to file
     * 
     * @param file
     *            the file to write to
     * @param configurationRecords
     *            the configuration list to write
     * @throws IOException
     *             on errors
     */
    public static void write(File file, List<IConfigurationRecord> configurationRecords) throws IOException {
        if (configurationRecords == null) {
            throw new IOException("Configuration records are null");
        }
        FileOutputStream output = null;
        try {
            output = new FileOutputStream(file);
            write(output, configurationRecords);
        } finally {
            if (output != null) {
                output.close();
            }
        }
    }

    /**
     * Writes a configuration record list to an output stream
     * 
     * @param output
     *            the output stream to write to
     * @param configurationRecords
     *            the configuration list to write
     * @throws IOException
     *             on errors
     */
    private static void write(OutputStream output, List<IConfigurationRecord> configurationRecords) throws IOException {
        if (configurationRecords == null) {
            throw new IOException("Configuration records are null");
        }
        PrintWriter writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(output)));
        for (IConfigurationRecord record : configurationRecords) {
            writer.println(record.toString());
        }
        writer.flush();
    }
}
