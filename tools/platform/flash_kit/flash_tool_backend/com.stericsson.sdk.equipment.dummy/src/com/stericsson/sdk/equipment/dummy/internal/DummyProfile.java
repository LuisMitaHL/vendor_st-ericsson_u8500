package com.stericsson.sdk.equipment.dummy.internal;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipmentProfile;
import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * 
 * @author xolabju
 * 
 */
public class DummyProfile implements IEquipmentProfile {

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY = "Network Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY = "Network Subset Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY = "Service Provider Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY = "Corporate Lock Key";

    /** */
    public static final String SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY = "Flexible ESL Lock Key";

    /** */
    private List<MESPConfigurationRecord> profileRecords = new LinkedList<MESPConfigurationRecord>();

    static final String NAME_SECURITY_PROPERTY = "SecurityProperty";

    static final String SEC_DEF_DATA = "Default_Data";

    static final String SEC_DEF_DATA_ID = "0x100";

    static final String SEC_IMEI_IN_FLASH = "IMEI_in_flash";

    static final String SEC_IMEI_IN_FLASH_ID = "0x01";

    static final String SEC_CONT_KEYS = "Control_Keys";

    static final String SEC_CONT_KEYS_ID = "0x10";

    private final String portName;

    /**
     * Initializes a newly created DummyProfile with given port.
     * 
     * @param port
     *            port
     */
    public DummyProfile(IPort port) {
        portName = port.getPortName();

        // create security properties
        MESPConfigurationRecord mesp = new MESPConfigurationRecord();
        mesp.setName(NAME_SECURITY_PROPERTY);
        mesp.setValue("Name", SEC_IMEI_IN_FLASH);
        mesp.setValue("ID", SEC_IMEI_IN_FLASH_ID);
        mesp.setValue("Read", "true");
        mesp.setValue("Write", "true");
        profileRecords.add(mesp);

        mesp = new MESPConfigurationRecord();
        mesp.setName(NAME_SECURITY_PROPERTY);
        mesp.setValue("Name", SEC_DEF_DATA);
        mesp.setValue("ID", SEC_DEF_DATA_ID);
        mesp.setValue("Read", "false");
        mesp.setValue("Write", "true");
        mesp.setValue("Default Data", "C:\temp");
        profileRecords.add(mesp);

        mesp = new MESPConfigurationRecord();
        mesp.setName(NAME_SECURITY_PROPERTY);
        mesp.setValue("Name", SEC_CONT_KEYS);
        mesp.setValue("ID", SEC_CONT_KEYS_ID);
        mesp.setValue("Read", "false");
        mesp.setValue("Write", "true");
        profileRecords.add(mesp);
    }

    /**
     * {@inheritDoc}
     */
    public String getAlias() {
        return "DummyProfile";
    }

    /**
     * {@inheritDoc}
     */
    public String getPlatformFilter() {
        return "(&(" + DummyPlatform.PLATFORM_PROPERTY + "=" + DummyPlatform.PLATFORM + ")("
            + DummyPlatform.PORT_PROPERTY + "=" + portName + "))";
    }

    /**
     * {@inheritDoc}
     */
    public String getSofwarePath(String type) {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getProperty(String key) {
        return null;
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
                    idDec = String.valueOf(Integer.parseInt(idDec,16));
                } else{
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

}
