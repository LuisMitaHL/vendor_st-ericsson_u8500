package com.stericsson.sdk.common.configuration;

import com.stericsson.sdk.common.configuration.system.ISystemPropertiesContributor;

/**
 * Interface which notify changes of system contributor
 * 
 * @author mbodan01
 */
public interface ISystemPropertiesListener {

    /**
     * interface method which notify changes
     * 
     * @param contributor
     *            new contributor
     */
    void configurationChanged(ISystemPropertiesContributor contributor);

}
