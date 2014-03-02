package com.stericsson.sdk.common.configuration;

import java.io.IOException;

/**
 * @author xtomlju
 */
public interface IConfigurationService {

    /**
     * Get first configuration record with the specified record name.
     * 
     * @param recordName
     *            Name of record
     * @return Interface to configuration record
     */
    IConfigurationRecord getRecord(String recordName);

    /**
     * Get all configuration records matching the supplied array of configuration record names.
     * 
     * @param recordNames
     *            An array of record names
     * @return An array of interfaces to configuration records
     */
    IConfigurationRecord[] getRecords(String[] recordNames);

    /**
     * Get all configuration records.
     * 
     * @return An array of interfaces to configuration records
     */
    IConfigurationRecord[] getAllRecords();

    /**
     * Add specified configuration record to configuration.
     * 
     * @param record
     *            Configuration record
     */
    void addRecord(IConfigurationRecord record);

    /**
     * Set configuration records in this configuration.
     * 
     * @param records
     *            An array of configuration records
     */
    void setRecords(IConfigurationRecord[] records);

    // /**
    // * Write configuration to specified file.
    // *
    // * @param file
    // * File object
    // * @throws IOException
    // * If an I/O related error occurred.
    // */
    // void write(File file) throws IOException;
    //
    // /**
    // * Write configuration to specified output stream.
    // *
    // * @param output
    // * Output stream
    // * @throws IOException
    // * If an I/O related error occurred.
    // */
    // void write(OutputStream output) throws IOException;
    //
    // /**
    // * Read configuration from specified file.
    // *
    // * @param file
    // * File object
    // * @throws IOException
    // * If an I/O related error occurred
    // */
    // void read(File file) throws IOException;
    //
    // /**
    // * Read configuration from specified input stream.
    // *
    // * @param input
    // * Input stream
    // * @throws IOException
    // * If an I/O related error occurred
    // */
    // void read(InputStream input) throws IOException;

    /**
     * Saves the configuration
     * 
     * @throws IOException
     *             on errors
     */
    void save() throws IOException;

    /**
     * Loads the configuration
     * 
     * @throws IOException
     *             on errors
     */
    void load() throws IOException;

    /**
     * Creates new configuration record
     * 
     * @return new configuration record
     */
    IConfigurationRecord getNewRecord();
}
