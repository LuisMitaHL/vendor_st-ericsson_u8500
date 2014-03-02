package com.stericsson.sdk.signing;

import java.util.HashMap;

/**
 * @author xtomlju
 */
public abstract class AbstractSignerSettings implements ISignerSettings {

    private HashMap<String, Object> signerSettings;

    /**
     * Constructor
     */
    public AbstractSignerSettings() {
        signerSettings = new HashMap<String, Object>();
        addSignerSetting(KEY_SIGN_PACKAGE);
        addSignerSetting(KEY_SIGN_PACKAGE_ALIAS);
        addSignerSetting(KEY_SIGNER_SERVICE);
        addSignerSetting(KEY_INPUT_FILE);
        addSignerSetting(KEY_OUTPUT_FILE);
        addSignerSetting(KEY_INPUT_FILE_TYPE);
        addSignerSetting(KEY_LOCAL);
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerSettings#getSignerSetting(java.lang.String)
     */
    /**
     * Method to retrieve the value for a specified key.
     * 
     * @param key
     *            Settings key to retrieve value for
     * @return Value instance or null if no value available or not previously set
     */
    public Object getSignerSetting(String key) {
        return signerSettings.get(key);
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerSettings#setSignerSetting(java.lang.String,
     * java.lang.Object)
     */
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
    public void setSignerSetting(String key, Object value) throws SignerSettingsException {
        if (signerSettings.containsKey(key)) {
            signerSettings.put(key, value);
        } else {
            throw new SignerSettingsException("Option not allowed for the selected sign package", 1);
        }
    }

    /**
     * @param key
     *            Signer settings key to allow
     */
    protected void addSignerSetting(String key) {
        signerSettings.put(key, null);
    }
}
