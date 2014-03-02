package com.stericsson.sdk.equipment.ui.editors.security;

import java.util.Hashtable;
import java.util.List;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipment;

/**
 * @author xdancho
 *
 */
public interface ISecurityPropertiesHandler {

    /**
     * Property ID value name.
     */
    String PROPERTY_ID_VALUE_NAME = "ID";

    /**
     * Read value name.
     */
    String READ_VALUE_NAME = "Read";

    /**
     * Write value name.
     */
    String WRITE_VALUE_NAME = "Write";

    /**
     * Control Keys record name.
     */
    String CONTROL_KEYS_RECORD_NAME = "control_keys";

    /**
     * Fill value name.
     */
    String CONTROL_KEYS_FILL_VALUE_NAME = "Fill with";

    /**
     * Default Data record name.
     */
    String DEFAULT_DATA_RECORD_NAME = "default_data";

    /**
     * Default Data value name.
     */
    String DEFAULT_DATA_VALUE_NAME = "Default_Data";

    /**
     * IMEI record name.
     */
    String IMEI_RECORD_NAME = "imei_in_flash";

    /**
     * IMEI value name.
     */
    String IMEI_VALUE_NAME = "IMEI_in_flash";

    /**
     * IMEI property ID.
     */
    String IMEI_PROPERTY_ID = "1";

    /**
     * @param properties tbd
     */
    void setConfigurationRecords(Hashtable<String, EquipmentProperty> properties);

    /**
     * @return tbd
     */
    List<IConfigurationRecord> getConfigurationRecords();

    /**
     * @return tbd
     */
    Hashtable<String, String> getReadIdValueMap();

    /**
     * @return tbd
     */
    Hashtable<String, String> getWriteIdValueMap();

    /**
     * @param control
     *            tbd
     * @param record
     *            tbd
     */
    void addReferenceToComponents(EditorUIControl control, EditorXML.Section.Record record);

    /**
     * @param source
     *            tbd
     */
    void updateUIFromConfigurationRecord(EditorUIControl source);

    /**
     * @param eq
     *            tbd
     */
    void updateUI(IEquipment eq);

}
