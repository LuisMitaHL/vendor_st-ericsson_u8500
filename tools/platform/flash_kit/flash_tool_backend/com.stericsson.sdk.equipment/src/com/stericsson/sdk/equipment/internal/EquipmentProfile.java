package com.stericsson.sdk.equipment.internal;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.common.configuration.system.SystemProperties;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipmentProfile;

/**
 * @author xtomlju
 */
public class EquipmentProfile implements IEquipmentProfile {

    private static final String COMMENT_PREFIX = "#";

    /** */
    private List<MESPConfigurationRecord> profileRecords;

    static final String NAME_SECURITY_PROPERTY = "SecurityProperty";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY = "Network Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY = "Network Subset Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY = "Service Provider Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY = "Corporate Lock Key";

    /** */
    public static final int MAX_SIMLOCK_CODE_LENGTH = 16;

    /** */
    public static final int MIN_SIMLOCK_CODE_LENGTH = 8;

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY = "Flexible ESL Lock Key";

    /**
     * Constructor.
     */
    public EquipmentProfile() {
        profileRecords = new LinkedList<MESPConfigurationRecord>();
    }

    /**
     * {@inheritDoc}
     */
    public String getAlias() {
        String alias = null;
        for (MESPConfigurationRecord record : profileRecords) {
            if (record.getName().equalsIgnoreCase("Profile")) {
                alias = record.getValue("Name");
                break;
            }
        }

        return alias;
    }

    /**
     * {@inheritDoc}
     */
    public String getPlatformFilter() {
        String filter = null;
        for (MESPConfigurationRecord record : profileRecords) {
            if (record.getName().equalsIgnoreCase("Platform")) {
                filter = record.getValue("filter");
                break;
            }
        }

        return filter == null ? filter : filter.toLowerCase(Locale.getDefault());
    }

    /**
     * {@inheritDoc}
     */
    public String getSofwarePath(String type) {
        String path = null;
        if ("Loader".equalsIgnoreCase(type)) {
            for (MESPConfigurationRecord record : profileRecords) {
                if (record.getName().equalsIgnoreCase("Software")) {
                    path = record.getValue("Path");
                    break;
                }
            }
        }

        if ((path != null) && (path.startsWith("@loaders"))) {
            String loadersRoot = SystemProperties.getProperty(SystemProperties.BACKEND_LOADER_ROOT);
            if (loadersRoot != null) {
                path = path.replace("@loaders", loadersRoot);
            }
        }

        return path;
    }

    /**
     * {@inheritDoc}
     */
    public Hashtable<String, EquipmentProperty> getSupportedSecurityProperties() {
        Hashtable<String, EquipmentProperty> securityProperties = new Hashtable<String, EquipmentProperty>();
        for (MESPConfigurationRecord record : profileRecords) {
            if (record.getName().equalsIgnoreCase(NAME_SECURITY_PROPERTY)) {
                String id = record.getValue("ID");
                String idDec;

                if (id.startsWith("0x")) {
                    idDec = id.replace("0x", " ").trim();
                    idDec = String.valueOf(Integer.parseInt(idDec, 16));
                } else {
                    idDec = id;
                }

                String name = record.getValue("Name");
                boolean readable = Boolean.parseBoolean(record.getValue("Read"));
                boolean writable = Boolean.parseBoolean(record.getValue("Write"));
                EquipmentProperty property = new EquipmentProperty(id, name, null, true, readable, writable);
                securityProperties.put(idDec, property);
            }
        }
        return securityProperties;
    }

    /**
     * Read profile from file.
     * 
     * @param file
     *            File
     * @throws IOException
     *             If an I/O error occurred
     */
    public void read(File file) throws IOException {
        FileInputStream input = null;
        try {
            input = new FileInputStream(file);
            read(input);
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    /**
     * Read profile from input stream.
     * 
     * @param input
     *            Input stream
     * @throws IOException
     *             If an I/O error occurred
     */
    public void read(InputStream input) throws IOException {

        BufferedReader reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        String line;

        try {
            line = reader.readLine();
            while ((line != null) && (!"".equals(line))) {
                line = line.trim();
                if (!line.startsWith(COMMENT_PREFIX)) {
                    int commentPrefixIndex = line.indexOf(COMMENT_PREFIX);
                    if (commentPrefixIndex > 0) {
                        line = line.substring(0, commentPrefixIndex);
                        line = line.trim();
                    }
                    profileRecords.add(MESPConfigurationRecord.parse(line));
                }
                line = reader.readLine();
            }
        } catch (MESPConfigurationException e) {
            IOException ioe = new IOException("Could not read configuration");
            ioe.initCause(e);
            throw ioe;
        }
    }

    /**
     * {@inheritDoc}
     */
    public String getProperty(String key) {
        String value = null;
        for (MESPConfigurationRecord record : profileRecords) {
            if ("Property".equalsIgnoreCase(record.getName())) {
                value = record.getValue(key);
                if (value != null) {
                    break;
                }
            }
        }
        return value;
    }
}
