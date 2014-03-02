package com.stericsson.sdk.equipment.ui.preferences.backend;

import java.io.BufferedReader;
import java.io.Closeable;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.preferences.ScopedPreferenceStore;

import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;

/**
 * Class for converting between IPreferenceStore and IConfigurationRecord[]
 * 
 * @author xolabju
 * 
 */
public final class BackendPreferenceHelper {

    private static Logger logger = Logger.getLogger(BackendPreferenceHelper.class.getName());

    private BackendPreferenceHelper() {
    }

    /**
     * Converts a preference store to an array of configuration records
     * 
     * @param store
     *            the store to convert
     * @return an array of configuration records
     */
    public static IConfigurationRecord[] toConfigurationRecords(IPreferenceStore store) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        if (store != null) {
            records.addAll(convertUSB(store));
            records.addAll(convertActiveProfile((store)));
            records.addAll(convertMonitor((store)));
            records.addAll(parseProfiles((store)));
        }

        return records.toArray(new IConfigurationRecord[records.size()]);
    }

    private static Vector<IConfigurationRecord> convertUSB(IPreferenceStore store) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        boolean usbEnabled = store.getBoolean(FlashKitPreferenceConstants.BACKEND_USB_ENABLED);
        MESPConfigurationRecord record = new MESPConfigurationRecord();
        record.setName(ConfigurationOption.PORT_USB);
        record.setValue(ConfigurationOption.PORT_USB_ENABLED, usbEnabled ? "true" : "false");
        records.add(record);
        return records;
    }

    private static Vector<IConfigurationRecord> convertActiveProfile(IPreferenceStore store) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        String profile = store.getString(FlashKitPreferenceConstants.BACKEND_ACTIVE_PROFILE);
        if (profile != null && !profile.trim().equals("")) {
            MESPConfigurationRecord record = new MESPConfigurationRecord();
            record.setName(ConfigurationOption.ACTIVE_PROFILE);
            record.setValue(ConfigurationOption.PROFILE_ALIAS, profile);
            records.add(record);
        }
        return records;
    }

    private static Vector<IConfigurationRecord> convertMonitor(IPreferenceStore store) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        boolean acceptClients = store.getBoolean(FlashKitPreferenceConstants.BACKEND_ACCEPT_CLIENTS);
        boolean acceptEquipment = store.getBoolean(FlashKitPreferenceConstants.BACKEND_ACCEPT_EQUIPMENT);
        MESPConfigurationRecord record = new MESPConfigurationRecord();
        record.setName(ConfigurationOption.MONITOR);
        record.setValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS, acceptClients ? "true" : "false");
        record.setValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS, acceptEquipment ? "true" : "false");
        records.add(record);
        return records;
    }

    private static Vector<IConfigurationRecord> parseProfiles(IPreferenceStore store) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        String profilePath = store.getString(FlashKitPreferenceConstants.BACKEND_PROFILE_PATH);
        if (profilePath != null && !profilePath.trim().equals("")) {
            File profileDir = new File(profilePath);
            if (profileDir.exists() && profileDir.isDirectory()) {
                File[] mespFiles = profileDir.listFiles(new FileFilter() {

                    public boolean accept(File path) {
                        return path != null && path.isFile() && path.getName().endsWith(".prfl");
                    }
                });
                if (mespFiles != null && mespFiles.length > 0) {
                    records.addAll(parseProfiles(mespFiles));
                }
            }
        }
        return records;
    }

    private static Vector<IConfigurationRecord> parseProfiles(File[] mespFiles) {
        Vector<IConfigurationRecord> records = new Vector<IConfigurationRecord>();
        FileInputStream input = null;
        BufferedReader reader = null;
        if (mespFiles != null) {
            for (File f : mespFiles) {
                try {
                    input = new FileInputStream(f);
                    reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
                    String line;
                    line = reader.readLine();
                    if ((line != null) && (!"".equals(line))
                        && line.toLowerCase(Locale.getDefault()).trim().startsWith("profile name=")) {
                        MESPConfigurationRecord record = MESPConfigurationRecord.parse(line);
                        String name = record.getValue("Name");
                        if (name != null && !name.equals("")) {
                            MESPConfigurationRecord profileRecord = new MESPConfigurationRecord();
                            profileRecord.setName(ConfigurationOption.PROFILE);
                            profileRecord.setValue(ConfigurationOption.PROFILE_ALIAS, name);
                            profileRecord.setValue(ConfigurationOption.PROFILE_PATH, f.getAbsolutePath());
                            records.add(profileRecord);
                        }
                    }
                } catch (Exception e) {
                    logger.warn("Failed to parse profile " + f.getAbsolutePath() + " : " + e.getMessage());
                } finally {
                    closeStreams(reader, input);
                }
            }
        }
        return records;
    }

    /**
     * Updates the backend preference store with value contained in a configuration record array
     * 
     * @param store
     *            the store to update
     * @param records
     *            the configuration records to read
     */
    public static void updatePreferenceStore(IPreferenceStore store, IConfigurationRecord[] records) {
        if (store != null && records != null && records.length > 0) {
            for (IConfigurationRecord record : records) {
                if (ConfigurationOption.PORT_USB.equalsIgnoreCase(record.getName())) {
                    store.setValue(FlashKitPreferenceConstants.BACKEND_USB_ENABLED, "true".equalsIgnoreCase(record
                        .getValue(ConfigurationOption.PORT_USB_ENABLED)) ? true : false);
                } else if (ConfigurationOption.MONITOR.equalsIgnoreCase(record.getName())) {
                    store.setValue(FlashKitPreferenceConstants.BACKEND_ACCEPT_CLIENTS, "true".equalsIgnoreCase(record
                        .getValue(ConfigurationOption.MONITOR_ACCEPT_CLIENTS)) ? true : false);
                    store.setValue(FlashKitPreferenceConstants.BACKEND_ACCEPT_EQUIPMENT, "true".equalsIgnoreCase(record
                        .getValue(ConfigurationOption.MONITOR_ACCEPT_EQUIPMENTS)) ? true : false);
                } else if (ConfigurationOption.ACTIVE_PROFILE.equalsIgnoreCase(record.getName())) {
                    store.setValue(FlashKitPreferenceConstants.BACKEND_ACTIVE_PROFILE, record
                        .getValue(ConfigurationOption.PROFILE_ALIAS));
                }
            }
            try {
                ((ScopedPreferenceStore) store).save();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

    }

    private static void closeStreams(Closeable... streams) {
        if (streams != null) {
            for (Closeable stream : streams) {
                if (stream != null) {
                    try {
                        stream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

}
