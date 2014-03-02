package com.stericsson.sdk.backend;

import java.io.IOException;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;

/**
 * @author xtomlju
 */
public interface IBackendService {

    /**
     * Shutdown backend in a controlled way
     */
    void shutdown();

    /**
     * @return True if backend accepts clients (some remote service is running).
     */
    boolean isAcceptingClients();

    /**
     * @return True if backend accepts connections from equipments.
     */
    boolean isAcceptingEquipments();

    /**
     * Toggle backend to accept clients.
     * 
     * @param accept
     *            Accept clients
     */
    void setAcceptClients(boolean accept);

    /**
     * Toggle backend to accept equipment connections.
     * 
     * @param accept
     *            Accept equipment connections.
     */
    void setAcceptEquipments(boolean accept);

    /**
     * Set active profile.
     * 
     * @param alias
     *            Active profile alias.
     */
    void setActiveProfile(String alias);

    /**
     * (Re-)load configuration
     * 
     * @throws IOException
     *             exception
     */
    void loadConfiguration() throws IOException;

    /**
     * Save configuration
     */
    void saveConfiguration();

    /**
     * Will inform all configuration listeners of type {link IConfigurationServiceListener} about
     * configuration record change.
     * 
     * @param oldRecord
     *            Old configuration record.
     * @param newRecord
     *            New configuration record.
     */
    void notifyConfigurationChanged(IConfigurationRecord oldRecord, IConfigurationRecord newRecord);

    /**
     * Sets the configuration service to be used by the backend service
     * 
     * @param configurationService
     *            the configuration service to be used by the backend service
     * @throws IOException
     *             on errors
     */
    void setConfigurationService(IConfigurationService configurationService) throws IOException;

    /**
     * 
     * @return the associated configuration service
     */
    IConfigurationService getConfigurationService();

    /**
     * Toggle backend to automatically download new dumps.
     * 
     * @param status
     *            True if enabled. False if disabled
     */
    void setAutoDownload(boolean status);

    /**
     * Get if backend is automatically downloading new dumps.
     * 
     * @return boolean true or false
     */
    boolean getAutoDownload();

    /**
     * Sets the core dump download path
     * 
     * @param coreDumpDirectory
     *            the coreDumpDirectory to set
     */
    void setCoreDumpLocation(String coreDumpDirectory);

    /**
     * Gets the dump download path. If no path exists default path will be returned.
     * 
     * @return the coreDumpDirectory
     */
    String getCoreDumpLocation();

    /**
     * Toggle backend to automatically delete downloaded dumps.
     * 
     * @param status
     *            True if enabled. False if disabled
     * @return True/False. If AutoDownload is not set, then it will return false. Autodownload must
     *         be set to true to get this functionality.
     */
    boolean setAutoDelete(boolean status);

    /**
     * Get if backend is automatically delete downloaded dumps.
     * 
     * @return boolean true or false
     */
    boolean getAutoDelete();

    /**
     * Toggle backend to use local signing.
     * 
     * @param localSigning
     *            true or false
     */
    void setLocalSigning(boolean localSigning);

}
