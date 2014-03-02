package com.stericsson.sdk.assembling;

/**
 * The IAssemblerSettings interface is the super interface for all assembler settings.
 * 
 * @author xolabju
 * 
 */
public interface IAssemblerSettings {

    /** Configuration file key. To hold the path/filename string to the configuration file */
    String KEY_CONFIGURATION_FILE = "CONFIGURATION_FILE";

    /** File list key. To hold the path/filename string to the file list file */
    String KEY_FILE_LIST_FILE = "FILE_LIST_FILE";

    /** Output file settings key. To hold the path/filename string to the output file */
    String KEY_OUTPUT_FILE = "OUTPUT_FILE";

    /** The type of software to assemble (e.g flash_archive or ram_image */
    String KEY_OUTPUT_TYPE = "OUTPUT_TYPE";

    /** The platform type (e.g a2 or u8500) */
    String KEY_PLATFORM_TYPE = "PLATFORM_TYPE";

    /** Flash Kit Profile */
    String KEY_PROFILE = "PROFILE";

    /***/
    String KEY_BUFFER_SIZE = "BUFFER_SIZE";

    /** Archive alignment size */
    String KEY_ALIGNMENT_SIZE = "ALIGNMENT_SIZE";

    /**
     * Method to associate a value for a specified key. This method should throw an exception if the
     * setting key is not supported by implementation.
     * 
     * @param key
     *            Settings key to set value for
     * @param value
     *            Value for the specified key
     * @throws AssemblerSettingsException
     *             If an assembler settings related error occurred
     */
    void put(String key, Object value) throws AssemblerSettingsException;

    /**
     * Method to retrieve the value for a specified key.
     * 
     * @param key
     *            Settings key to retrieve value for
     * @return Value instance or null if no value available or not previously set
     */
    Object get(String key);
}
