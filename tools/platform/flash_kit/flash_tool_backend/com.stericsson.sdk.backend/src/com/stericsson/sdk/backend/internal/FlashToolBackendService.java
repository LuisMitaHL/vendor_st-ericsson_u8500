package com.stericsson.sdk.backend.internal;

import java.io.IOException;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleException;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.IBackendService;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.IConfigurationServiceListener;
import com.stericsson.sdk.common.log4j.LogFactory;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;

/**
 * Backend service implementation for the Flash Tool Backend.
 * 
 * @author xtomlju
 */
public class FlashToolBackendService implements IBackendService {

    private static final String ARGUMENT_ACTIVE_PROFILE = "active.profile";

    /** Logger interface */
    private Logger logger = Logger.getLogger(FlashToolBackendService.class.getName());

    /** Configuration service */
    private IConfigurationService configurationService = null;

    /**
     * Constructor.
     */
    public FlashToolBackendService() {
        ServiceReference[] references = null;

        String filter = ("(&(type=backend)(format=mesp))");

        try {
            references =
                Activator.getBundleContext().getServiceReferences(IConfigurationService.class.getName(), filter);
            if ((references != null) && (references.length > 0)) {
                configurationService = (IConfigurationService) Activator.getBundleContext().getService(references[0]);
            }
        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        LogFactory.sysLogCreator(7001);
    }

    /**
     * {@inheritDoc}
     */
    public void shutdown() {
        saveConfiguration();
        try {
            Activator.getBundleContext().getBundle(0).stop();
            System.exit(0);
        } catch (BundleException e) {
            logger.error("Unable to shutdown backend service", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isAcceptingClients() {
        boolean result = false;

        IConfigurationRecord[] records = configurationService.getRecords(new String[] {
            ConfigurationOption.MONITOR});
        if (records.length > 0) {
            String value = records[0].getValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS);
            if ("true".equalsIgnoreCase(value)) {
                result = true;
            } else if (!"false".equalsIgnoreCase(value)) {
                logger.error("Unrecognized configuration value for " + ConfigurationOption.MONITOR_ACCEPT_CLIENTS
                    + ". It should be 'true' or 'false'. Defaulting to " + result);
            }
        } else {
            logger.warn("No Monitor configuration record found. Defaulting to " + result + " for "
                + ConfigurationOption.MONITOR_ACCEPT_CLIENTS);
        }

        return result;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isAcceptingEquipments() {
        boolean result = false;

        IConfigurationRecord[] records = configurationService.getRecords(new String[] {
            ConfigurationOption.MONITOR});
        if (records.length > 0) {
            String value = records[0].getValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS);
            if ("true".equalsIgnoreCase(value)) {
                result = true;
            } else if (!"false".equalsIgnoreCase(value)) {
                logger.error("Unrecognized configuration value for " + ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS
                    + ". It should be 'true' or 'false'. Defaulting to " + result);
            }
        } else {
            logger.warn("No Monitor configuration record found. Defaulting to " + result + " for "
                + ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS);
        }

        return result;

    }

    /**
     * {@inheritDoc}
     */
    public void setAcceptClients(boolean accept) {

        IConfigurationRecord record = configurationService.getRecord(ConfigurationOption.MONITOR);

        if (record != null) {
            IConfigurationRecord oldRecord = record.deepCopy();
            if (accept) {
                record.setValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS, "true");
            } else {
                record.setValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS, "false");
            }

            notifyConfigurationChanged(oldRecord, record);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void setAcceptEquipments(boolean accept) {
        IConfigurationRecord record = configurationService.getRecord(ConfigurationOption.MONITOR);

        if (record != null) {
            IConfigurationRecord oldRecord = record.deepCopy();
            if (accept) {
                record.setValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS, "true");
            } else {
                record.setValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS, "false");
            }

            notifyConfigurationChanged(oldRecord, record);
        }
    }

    /**
     * {@inheritDoc}
     * 
     * @throws IOException
     */
    public void loadConfiguration() throws IOException, NullPointerException {
        configurationService.load();
        IConfigurationRecord[] allRecords = configurationService.getAllRecords();
        for (IConfigurationRecord record : allRecords) {
            notifyConfigurationChanged(null, record);
        }
        processCommandLineArguments();
    }

    /**
     * {@inheritDoc}
     */
    public void saveConfiguration() {
        try {
            configurationService.save();
        } catch (IOException e) {
            logger.error("Failed to save configuration", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void notifyConfigurationChanged(IConfigurationRecord oldRecord, IConfigurationRecord newRecord) {
        ServiceReference[] allServiceReferences = null;

        try {
            allServiceReferences =
                Activator.getBundleContext().getAllServiceReferences(IConfigurationServiceListener.class.getName(),
                    null);
        } catch (InvalidSyntaxException e) {
            logger.error("Failed to get service references", e);
        }

        if (allServiceReferences == null) {
            return;
        }

        logger.debug("Configuration record changed from '" + oldRecord + "' to '" + newRecord + "'");

        for (ServiceReference ref : allServiceReferences) {
            IConfigurationServiceListener listener =
                (IConfigurationServiceListener) Activator.getBundleContext().getService(ref);

            listener.configurationChanged(configurationService, oldRecord, newRecord);
            Activator.getBundleContext().ungetService(ref);
        }
    }

    private void processCommandLineArguments() {
        // -Dactive.profile
        String activeProfile = System.getProperty(ARGUMENT_ACTIVE_PROFILE);
        if (activeProfile != null && !activeProfile.trim().equals("")) {
            setActiveProfile(activeProfile);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void setActiveProfile(String pAlias) {

        IConfigurationRecord record = configurationService.getRecord(ConfigurationOption.ACTIVE_PROFILE);
        IConfigurationRecord oldRecord = null;

        if (record != null) {
            oldRecord = record.deepCopy();
            record.setValue(ConfigurationOption.PROFILE_ALIAS, pAlias);
        } else {
            record = configurationService.getNewRecord();
            configurationService.addRecord(record);
            record.setName(ConfigurationOption.ACTIVE_PROFILE);
            record.setValue(ConfigurationOption.PROFILE_ALIAS, pAlias);
        }
        notifyConfigurationChanged(oldRecord, record);
    }

    /**
     * {@inheritDoc}
     */
    public void setConfigurationService(IConfigurationService confService) throws IOException {
        this.configurationService = confService;
        loadConfiguration();
    }

    /**
     * {@inheritDoc}
     */
    public IConfigurationService getConfigurationService() {
        return this.configurationService;
    }

    /**
     * {@inheritDoc}
     */
    public void setAutoDownload(boolean status) {
        FtpPortHandler.getInstance().setAutoDownload(status);
    }

    /**
     * {@inheritDoc}
     */
    public boolean getAutoDownload() {
        return FtpPortHandler.getInstance().getAutoDownload();
    }

    /**
     * {@inheritDoc}
     */
    public void setCoreDumpLocation(String coreDumpDirectory) {
        FtpPortHandler.getInstance().setAutoDownloadDirectory(coreDumpDirectory);
    }

    /**
     * {@inheritDoc}
     */
    public String getCoreDumpLocation() {
        return FtpPortHandler.getInstance().getAutoDownloadDirectory();
    }

    /**
     * {@inheritDoc}
     */
    public boolean setAutoDelete(boolean status) {
        return FtpPortHandler.getInstance().setAutoDelete(status);

    }

    /**
     * {@inheritDoc}
     */
    public boolean getAutoDelete() {
        return FtpPortHandler.getInstance().getAutoDelete();
    }

    /**
     * {@inheritDoc}
     */
    public void setLocalSigning(boolean localSigning) {
        IConfigurationRecord record = configurationService.getRecord(ConfigurationOption.SIGN_TOOL);
        IConfigurationRecord oldRecord = null;

        if (record != null) {
            oldRecord = record.deepCopy();
        } else {
            record = configurationService.getNewRecord();
            configurationService.addRecord(record);
            record.setName(ConfigurationOption.SIGN_TOOL);
        }

        if (localSigning) {
            record.setValue(ConfigurationOption.USE_LOCAL_SIGNING, "true");
        } else {
            record.setValue(ConfigurationOption.USE_LOCAL_SIGNING, "false");
        }

        notifyConfigurationChanged(oldRecord, record);
    }

}
