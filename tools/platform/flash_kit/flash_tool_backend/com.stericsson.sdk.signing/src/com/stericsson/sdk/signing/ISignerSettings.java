package com.stericsson.sdk.signing;

/**
 * The ISignerSettings interface is the super interface for all signer settings.
 * 
 * @author xtomlju
 */
public interface ISignerSettings {

    /** Sign package settings key. To hold an instance of ISignPackage */
    String KEY_SIGN_PACKAGE = "SIGN_PACKAGE";

    /** Sign package alias settings key. To hold a string of the sign package alias */
    String KEY_SIGN_PACKAGE_ALIAS = "SIGN_PACKAGE_ALIAS";

    /** Input file settings key. To hold the path/filename string to the input file */
    String KEY_INPUT_FILE = "INPUT_FILE";

    /** Output file settings key. To hold the path/filename string to the output file */
    String KEY_OUTPUT_FILE = "OUTPUT_FILE";

    /** Input file format settings key. To hold the format identifier string for the input file */
    String KEY_INPUT_FILE_TYPE = "INPUT_FILE_FORMAT";

    /** Signer service instance settings key. To hold an instance of ISignerService */
    String KEY_SIGNER_SERVICE = "SIGNER_SERVICE";

    /** Output load address file, */
    String KEY_LOAD_ADDRESS_XML = "LOAD_ADDRESS_XML";

    /** Output hash table file */
    String KEY_HASH_FILE = "KEY_HASH_FILE";

    /** Local signing */
    String KEY_LOCAL = "KEY_LOCAL";

    /**Keys and packages in common folder */
    String COMMON_FOLDER_KEY_AND_PACKAGES = "COMMON_FOLDER_KEY_AND_PACKAGES";

    /** */
    String KEY_SIGN_SERVICE_SERVERS = "SIGN_SERVICE_SERVERS";

    /** */
    String KEY_SIGN_PACKAGE_ROOT = "SIGN_PACKAGE_ROOT";

    /** */
    String KEY_LOCAL_SIGN_PACKAGE_ROOT = "LOCAL_SIGN_PACKAGE_ROOT";

    /** */
    String KEY_LOCAL_KEY_ROOT = "LOCAL_KEY_ROOT";

    /** */
     String KEY_LOCAL_ENCRYPTION_KEY_ROOT = "LOCAL_ENCRYPTION_KEY_ROOT";
    /**
     * Method to associate a value for a specified key. This method should throw an exception if the
     * setting key is not supported by implementation.
     * 
     * @param key
     *            Settings key to set value for
     * @param value
     *            Value for the specified key
     * @throws SignerSettingsException
     *             If an signer settings related error occurred
     */
    void setSignerSetting(String key, Object value) throws SignerSettingsException;

    /**
     * Method to retrieve the value for a specified key.
     * 
     * @param key
     *            Settings key to retrieve value for
     * @return Value instance or null if no value available or not previously set
     */
    Object getSignerSetting(String key);

    /**
     * Set all possible settings from another ISignerSettings instance
     * 
     * @param settings
     *            ISignerSettings instance to read values from
     * @throws SignerSettingsException
     *             If a signer settings related error occurred
     */
    void setFrom(ISignerSettings settings) throws SignerSettingsException;
}
