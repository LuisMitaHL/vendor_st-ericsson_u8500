package com.stericsson.sdk.common.configuration.mesp;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.LinkedList;
import java.util.List;

/**
 * Class for reading MESP formatted configuration files
 * 
 * @author tomas
 */
public class MESPConfiguration {

    private List<MESPConfigurationRecord> configurationRecords;

    /**
     * @param file
     *            File pointing to a MESP configured file
     * @return MESPConfiguration instance
     * @throws IOException
     *             If an I/O related error occurred
     * @throws MESPConfigurationException
     *             If an MESP related error occurred
     */
    public static MESPConfiguration read(File file) throws IOException, MESPConfigurationException {

        MESPConfiguration configuration = null;
        FileInputStream input = null;

        try {
            input = new FileInputStream(file);
            configuration = read(input);
        } finally {
            if (input != null) {
                input.close();
            }
        }

        return configuration;
    }

    /**
     * @param data
     *            MESP configuration data
     * @return MESPConfiguration instance
     * @throws IOException
     *             If an I/O related error occurred
     * @throws MESPConfigurationException
     *             If an MESP related error occurred
     */
    public static MESPConfiguration read(byte[] data) throws IOException, MESPConfigurationException {
        return read(new ByteArrayInputStream(data));
    }

    /**
     * @param input
     *            Input stream to read mesp configured data from
     * @return MESPConfiguration instance
     * @throws IOException
     *             If an I/O related error occurred
     * @throws MESPConfigurationException
     *             If an MESP related error occurred
     */
    public static MESPConfiguration read(InputStream input) throws IOException, MESPConfigurationException {

        MESPConfiguration configuration = new MESPConfiguration();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        String line;

        line = reader.readLine();
        while ((line != null) && (!"".equals(line))) {
            configuration.addConfigurationRecord(MESPConfigurationRecord.parse(line));
            line = reader.readLine();
        }

        return configuration;
    }

    /**
     * @param file
     *            File to write MESP configuration to
     * @param configuration
     *            Configuration to write
     * @throws IOException
     *             If an I/O related error occurred
     */
    public static void write(File file, MESPConfiguration configuration) throws IOException {

        FileOutputStream output = null;

        try {
            output = new FileOutputStream(file);
            write(output, configuration);
        } finally {
            if (output != null) {
                output.close();
            }
        }
    }

    /**
     * @param output
     *            Output stream to write configuration to
     * @param configuration
     *            Configuration to write
     */
    public static void write(OutputStream output, MESPConfiguration configuration) {

        PrintWriter writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(output)));
        MESPConfigurationRecord[] entries = configuration.getAllRecords();

        for (MESPConfigurationRecord entry : entries) {
            writer.println(entry.toString());
        }

        writer.flush();
    }

    /**
     * Construction
     */
    public MESPConfiguration() {
        configurationRecords = new LinkedList<MESPConfigurationRecord>();
    }

    /**
     * @param entry
     *            Record entry to add
     */
    public void addConfigurationRecord(MESPConfigurationRecord entry) {
        configurationRecords.add(entry);
    }

    /**
     * @param recordNames
     *            Record names to retrieve
     * @return An array of configuration records
     */
    public MESPConfigurationRecord[] getRecords(String[] recordNames) {

        List<MESPConfigurationRecord> records = new LinkedList<MESPConfigurationRecord>();

        for (MESPConfigurationRecord record : configurationRecords) {

            if (recordNames == null) {
                break;
            }

            for (String name : recordNames) {
                if (record.getName().equalsIgnoreCase(name)) {
                    records.add(record);
                    break;
                }
            }
        }

        return records.toArray(new MESPConfigurationRecord[0]);
    }

    /**
     * @return All configuration records
     */
    public MESPConfigurationRecord[] getAllRecords() {
        return configurationRecords.toArray(new MESPConfigurationRecord[0]);
    }

    /**
     * 
     * @param inputName
     *            input name
     * @param inputValue
     *            input value
     * @return This method returns all MESPConfigurationRecords, where inputValue equals with value
     *         from MESPConfigurationRecord for specified name.
     */
    public MESPConfigurationRecord[] getRecordByValue(String inputName, String inputValue) {
        List<MESPConfigurationRecord> records = new LinkedList<MESPConfigurationRecord>();

        for (MESPConfigurationRecord record : configurationRecords) {
            Object value = record.getValue(inputName);
            if (value instanceof String) {
                if (((String) value).equalsIgnoreCase(inputValue)) {
                    records.add(record);
                }
            }
        }

        return records.toArray(new MESPConfigurationRecord[0]);
    }
}
