package com.stericsson.sdk.equipment.ui.editors.security;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.List;
import java.util.Locale;
import java.util.Random;

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author xdancho
 * 
 */
public class U8500SecurityPropertiesHandler implements ISecurityPropertiesHandler {

    List<Text> controlKeyList = new ArrayList<Text>();

    Hashtable<String, String> readIdValueMap = new Hashtable<String, String>();

    Hashtable<String, String> writeIdValueMap = new Hashtable<String, String>();

    Text imeiInFlash;

    List<IConfigurationRecord> configurationRecords = new ArrayList<IConfigurationRecord>();

    private static String[] controlKeys =
        new String[] {
            "Network Lock Key", "Network Subset Lock Key", "Service Provider Lock Key", "Corporate Lock Key",
            "Flexible ESL Lock Key"};

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#getConfigurationRecords()
     *      {@inheritDoc}
     */
    public List<IConfigurationRecord> getConfigurationRecords() {

        return configurationRecords;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#getReadIdValueMap()
     *      {@inheritDoc}
     */
    public Hashtable<String, String> getReadIdValueMap() {

        return readIdValueMap;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#getWriteIdValueMap()
     *      {@inheritDoc}
     */
    public Hashtable<String, String> getWriteIdValueMap() {
        return writeIdValueMap;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#
     *      setConfigurationRecords(java.util.Hashtable) {@inheritDoc}
     */
    public void setConfigurationRecords(Hashtable<String, EquipmentProperty> properties) {

        for (EquipmentProperty p : properties.values()) {
            MESPConfigurationRecord mespRecord = new MESPConfigurationRecord();
            mespRecord.setName(p.getPropertyName());
            // if (p.getPropertyName().equals("Default Data")) {
            // Add the default path to the data
            // mespRecord.setValue("Default Data", "C:\\temp");
            // }
            mespRecord.setValue(READ_VALUE_NAME, p.isReadable() ? "true" : "false");
            mespRecord.setValue(WRITE_VALUE_NAME, p.isWritable() ? "true" : "false");
            mespRecord.setValue(PROPERTY_ID_VALUE_NAME, p.getPropertyId());
            configurationRecords.add(mespRecord);

            if (p.isReadable()) {
                readIdValueMap.put(p.getPropertyId(), "");
            }

            if (p.isWritable()) {
                writeIdValueMap.put(p.getPropertyId(), "");
            }

        }

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#
     *      addReferenceToComponents(com.stericsson.sdk.common.ui.EditorUIControl,
     *      com.stericsson.sdk.common.ui.EditorXML.Section.Record) {@inheritDoc}
     */
    public void addReferenceToComponents(EditorUIControl control, EditorXML.Section.Record record) {
        for (Control con : control.getComposite().getChildren()) {
            if (con instanceof Text && record.getName().equalsIgnoreCase(CONTROL_KEYS_RECORD_NAME)) {
                controlKeyList.add((Text) con);
            } else if (con instanceof Text && record.getName().equalsIgnoreCase(IMEI_RECORD_NAME)) {
                imeiInFlash = (Text) con;
            }
        }
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler#
     *      updateUIFromConfigurationRecord(com.stericsson.sdk.common.ui.EditorUIControl)
     *      {@inheritDoc}
     */
    public void updateUIFromConfigurationRecord(EditorUIControl source) {

        MESPConfigurationRecord rec = (MESPConfigurationRecord) source.getRecord();
        if (source.isValidationOk()) {

            if (rec.getName().toLowerCase(Locale.getDefault()).equals(CONTROL_KEYS_RECORD_NAME)) {
                writeIdValueMap.put(rec.getValue(PROPERTY_ID_VALUE_NAME), getControlKeys(rec));
            } else if (rec.getName().toLowerCase(Locale.getDefault()).equals(DEFAULT_DATA_RECORD_NAME)) {

                writeIdValueMap.put(rec.getValue(PROPERTY_ID_VALUE_NAME), rec.getValue(DEFAULT_DATA_VALUE_NAME));

                // TODO: HACK
                // } else if (rec.getName().equals("IMEI_in_flash")) {
                // writeIdValueMap.put(rec.getValue("ID"), rec.getValue("IMEI in flash"));
            }
            // TODO HACK no OTP
        }

        if (rec.getName().toLowerCase(Locale.getDefault()).equals(IMEI_RECORD_NAME)) {

            writeIdValueMap.put(rec.getValue(PROPERTY_ID_VALUE_NAME), rec.getValue(IMEI_VALUE_NAME));
        }
        // TODO: END HACK no OTP
        if (null != rec.getValue(CONTROL_KEYS_FILL_VALUE_NAME)) {
            if (!rec.getValue(CONTROL_KEYS_FILL_VALUE_NAME).equals("None")) {

                final String value = rec.getValue(CONTROL_KEYS_FILL_VALUE_NAME);
                rec.setValue(CONTROL_KEYS_FILL_VALUE_NAME, "None");
                new Thread(new Runnable() {
                    public void run() {
                        Display.getDefault().asyncExec(new Runnable() {
                            public void run() {
                                fillControlKeys(value);
                            }
                        });

                    }

                }).start();

            }
        }

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.equipment.ui.editors.security.ISecurityPropertiesHandler
     *      #updateUI(com.stericsson.sdk.equipment.IEquipment) {@inheritDoc}
     */
    public void updateUI(IEquipment eq) {
        updateConfiguration(eq.getProperties());

    }

    private void fillControlKeys(String value) {
        if (value.equalsIgnoreCase("Random")) {
            Random r = new Random(System.currentTimeMillis());
            for (int i = 0; i < controlKeyList.size(); i++) {
                StringBuilder sb = new StringBuilder(16);
                for (int j = 0; j < 16; j++) {
                    sb.append(r.nextInt(10));
                }
                controlKeyList.get(i).setText(sb.toString());
            }

        } else {
            char fillChar = value.charAt(0);

            char[] fill = new char[16];
            for (int i = 0; i < controlKeyList.size(); i++) {
                Arrays.fill(fill, fillChar);
                controlKeyList.get(i).setText(new String(fill));
            }

        }

    }

    private String getControlKeys(MESPConfigurationRecord record) {

        StringBuilder sb = new StringBuilder();
        sb.append("{");
        for (int i = 0; i < controlKeys.length; i++) {
            if (record.getValue(controlKeys[i]) == null) {
                return null;
            } else {
                sb.append(controlKeys[i] + "=" + record.getValue(controlKeys[i]));
                if (i == controlKeys.length - 1) {
                    sb.append("}");

                } else {
                    sb.append(",");
                }
            }
        }

        return sb.toString();
    }

    private void updateConfiguration(Hashtable<String, EquipmentProperty> prop) {
        // only imei
        for (String s : prop.keySet()) {
            if (s.equals(IMEI_PROPERTY_ID)) {
                String decodeIMEI = "";
                try {
                    decodeIMEI = ImeiConverter.decodeIMEI(prop.get(s).getPropertyValue().getBytes("UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                imeiInFlash.setText(decodeIMEI);
            }
        }

    }

}
