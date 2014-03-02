package com.stericsson.sdk.assembling;

import java.util.HashMap;

/**
 * @author xolabju
 */
public abstract class AbstractAssemblerSettings implements IAssemblerSettings {

    private HashMap<String, Object> assemblerSettings;

    /**
     * Constructor
     */
    public AbstractAssemblerSettings() {
        assemblerSettings = new HashMap<String, Object>();
        addKey(KEY_CONFIGURATION_FILE);
        addKey(KEY_FILE_LIST_FILE);
        addKey(KEY_OUTPUT_FILE);
        addKey(KEY_OUTPUT_TYPE);
        addKey(KEY_PLATFORM_TYPE);
        addKey(KEY_BUFFER_SIZE);
        addKey(KEY_ALIGNMENT_SIZE);
    }

    /**
     * {@inheritDoc}
     */
    public Object get(String key) {
        return assemblerSettings.get(key);
    }

    /**
     * {@inheritDoc}
     */
    public void put(String key, Object value) throws AssemblerSettingsException {
        if (assemblerSettings.containsKey(key)) {
            assemblerSettings.put(key, value);
        } else {
            throw new AssemblerSettingsException("Invalid setting", 1);
        }
    }

    /**
     * @param key
     *            Settings key to allow
     */
    protected void addKey(String key) {
        assemblerSettings.put(key, null);
    }
}
