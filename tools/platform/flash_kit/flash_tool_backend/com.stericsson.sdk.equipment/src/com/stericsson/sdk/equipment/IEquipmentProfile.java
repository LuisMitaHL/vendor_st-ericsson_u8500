package com.stericsson.sdk.equipment;

import java.util.Hashtable;

/**
 * @author xtomlju
 */
public interface IEquipmentProfile {

    /**
     * Get platform filter string.
     * 
     * @return Platform filter string
     */
    String getPlatformFilter();

    /**
     * Get profile alias.
     * 
     * @return Profile alias
     */
    String getAlias();

    /**
     * Get path to software.
     * 
     * @param type
     *            Type of software
     * @return Path string
     */
    String getSofwarePath(String type);

    /**
     * Returns list of security properties information encoded in profile
     * 
     * @return list of security properties
     */
    Hashtable<String, EquipmentProperty> getSupportedSecurityProperties();

    /**
     * @param key
     *            Property key
     * @return Property value or null if property does not exits
     */
    String getProperty(String key);
}
