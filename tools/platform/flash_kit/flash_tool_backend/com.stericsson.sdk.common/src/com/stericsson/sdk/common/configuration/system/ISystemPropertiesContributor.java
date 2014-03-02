package com.stericsson.sdk.common.configuration.system;

/**
 * Interface for system properties contributor.
 * The purpose of system properties contributor is to extend System.getProperty functionality.
 * The implementation of this interface should be registered as a service.
 * 
 * @see com.stericsson.sdk.common.configuration.system.SystemProperties
 *
 * @author xadazim
 *
 */
public interface ISystemPropertiesContributor {

    /**
     * Gets the property indicated by the specified key.
     * 
     * @param key
     *            the name of the property
     * @return the string value of the system property, or null if there is no property with that
     *         key
     */
    String getProperty(String key);
}
