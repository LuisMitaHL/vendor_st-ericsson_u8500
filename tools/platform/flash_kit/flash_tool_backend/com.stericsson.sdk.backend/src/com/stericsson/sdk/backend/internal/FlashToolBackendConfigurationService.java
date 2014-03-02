package com.stericsson.sdk.backend.internal;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.common.configuration.system.SystemProperties;

/**
 * Configuration service implementation to keep track on the flash tool backend configuration.
 * 
 * @author xtomlju
 */
public class FlashToolBackendConfigurationService implements IConfigurationService {

    // /** Default configuration filename */
    // private static final String DEFAULT_CONFIGURATION_FILENAME = "backend.mesp";

    private Logger logger = Logger.getLogger(FlashToolBackendConfigurationService.class.getName());

    /** List of configuration records */
    private List<IConfigurationRecord> configurationRecords;

    /**
     * Constructor.
     */
    public FlashToolBackendConfigurationService() {
        configurationRecords = new LinkedList<IConfigurationRecord>();
    }

    /**
     * {@inheritDoc}
     */
    public void addRecord(IConfigurationRecord record) {
        configurationRecords.add(record);
    }

    /**
     * {@inheritDoc}
     */
    public IConfigurationRecord[] getAllRecords() {
        return configurationRecords.toArray(new IConfigurationRecord[0]);
    }

    /**
     * {@inheritDoc}
     */
    public IConfigurationRecord[] getRecords(String[] recordNames) {
        List<IConfigurationRecord> records = new LinkedList<IConfigurationRecord>();

        for (IConfigurationRecord record : configurationRecords) {

            if (recordNames == null) {
                break;
            }

            for (String name : recordNames) {
                if (record.getName().equalsIgnoreCase(name)) {
                    records.add(record);
                    break;
                }
            }
        }

        return records.toArray(new IConfigurationRecord[0]);
    }

    /**
     * {@inheritDoc}
     */
    public void setRecords(IConfigurationRecord[] records) {
        configurationRecords.clear();
        for (IConfigurationRecord record : records) {
            configurationRecords.add(record);
        }
    }

    /**
     * {@inheritDoc}
     */
    public IConfigurationRecord getRecord(String recordName) {
        IConfigurationRecord result = null;
        for (IConfigurationRecord record : configurationRecords) {
            if (record.getName().equalsIgnoreCase(recordName)) {
                result = record;
                break;
            }
        }
        return result;
    }

    /**
     * {@inheritDoc}
     */
    public IConfigurationRecord getNewRecord() {
        return MESPConfigurationRecord.newRecord();
    }

    /**
     * {@inheritDoc}
     */
    public void load() throws IOException, NullPointerException {
        String configurationsRoot = SystemProperties.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);
        if (configurationsRoot == null) {
            throw new NullPointerException("Configuration file property is not defined.");
        }

        File file = new File(configurationsRoot);
        if (file.exists()) {
            logger.info("Loading flash tool backend configuration file from: " + file.getAbsolutePath());
            FlashToolBackendConfigurationServiceHelper.read(file, configurationRecords);
        } else {
            String errorMessage = "Configuration file " + file.getAbsolutePath() + " not found.";
            throw new FileNotFoundException(errorMessage);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void save() {
        try {
            String configurationsRoot = SystemProperties.getProperty(SystemProperties.BACKEND_CONFIGURATIONS_ROOT);
            if (configurationsRoot != null) {
                File file = new File(configurationsRoot);
                logger.info("Saving flash tool backend configuration file to: " + file.getAbsolutePath());
                FlashToolBackendConfigurationServiceHelper.write(file, configurationRecords);
            } else {
                logger.error("Unable to write configuration because configuration property is not defined.");
            }
        } catch (IOException e) {
            logger.error("Failed to write configuration file", e);
        }

    }

}
