package com.stericsson.sdk.common.configuration;

/**
 * @author xtomlju
 */
public interface IConfigurationServiceListener {

    /**
     * Called when a configuration record has been changed or made available.
     * 
     * @param service
     *            Configuration service
     * @param oldRecord
     *            Old configuration record (value) or null if record is made available
     * @param newRecord
     *            New configuration record (value)
     */
    void configurationChanged(IConfigurationService service, IConfigurationRecord oldRecord,
        IConfigurationRecord newRecord);
}
