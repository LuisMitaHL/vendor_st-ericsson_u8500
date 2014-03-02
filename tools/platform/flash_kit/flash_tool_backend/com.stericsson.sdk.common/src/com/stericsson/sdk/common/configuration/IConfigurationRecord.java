package com.stericsson.sdk.common.configuration;

import java.util.List;

/**
 * @author xtomlju
 */
public interface IConfigurationRecord {

    /**
     * Set configuration record name.
     * 
     * @param name
     *            Configuration record name
     */
    void setName(String name);

    /**
     * Get configuration record name.
     * 
     * @return Configuration record name
     */
    String getName();

    /**
     * Test if record contains values for given value name;
     *
     * @param valueName Value name to be checked.
     * @return True if value name is set for this record, false otherwise.
     */
    boolean isValueNameSet(String valueName);

    /**
     * @return List of names of all values held by this record.
     */
    List<String> getValuesNames();

    /**
     * Set value for specified value name.
     * 
     * @param valueName
     *            Value name
     * @param value
     *            Value
     */
    void setValue(String valueName, String value);

    /**
     * Set array value for specified value name.
     * 
     * @param valueName
     *            Value name
     * @param values
     *            Array of values
     */
    void setArrayValue(String valueName, String[] values);

    /**
     * Get value for specified value name.
     * 
     * @param valueName
     *            Value name
     * @return Value
     */
    String getValue(String valueName);

    /**
     * Get array value for specified value name.
     * 
     * @param valueName
     *            Value name
     * @return Array of values
     */
    String[] getArrayValue(String valueName);

    /**
     * Return a deep copy of this configuration record.
     * @return Configuration record
     */
    IConfigurationRecord deepCopy();
}
