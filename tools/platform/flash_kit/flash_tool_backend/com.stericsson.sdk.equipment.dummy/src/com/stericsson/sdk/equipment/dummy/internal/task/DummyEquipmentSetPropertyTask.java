/**
 * 
 */
package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Locale;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.equipment.dummy.internal.DummyProfile;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author cizovhel
 * 
 */
public class DummyEquipmentSetPropertyTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String propertyID = null;

    private String propertyName = null;

    private String propertyValue = null;

    EquipmentProperty property = null;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentSetPropertyTask(DummyEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        try {
            resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        if (propertyID == null || propertyValue == null || propertyName == null || property == null) {
            // wrong propertyName or missing propertyValue
            resultCode = ERROR;
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }

        notifyTaskMessage("Setting equipment property: propertyID = " + propertyID + ", propertyName = " + propertyName
            + ", value = " + propertyValue);

        notifyTaskStart();
        try {
            mobileEquipment.getMock().setResult(0);
        } catch (Exception e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }

        try {
            byte[] propertyBytes = gainBytesFromPropertyValue(propertyValue);
            int id = Integer.parseInt(propertyID, 10);

            Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_SET_PROPERTY, new Object[] {
                id, propertyBytes.length, propertyBytes}, this);

            resultCode = (Integer) obj;

            resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);

        } catch (UnsupportedEncodingException e) {
            resultCode = ERROR;
            resultMessage = e.getMessage();
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        } catch (UnsupportedPropertyException e) {
            resultCode = ERROR;
            resultMessage = e.getMessage();
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    private byte[] gainBytesFromPropertyValue(String value) throws UnsupportedEncodingException,
        UnsupportedPropertyException {

        // array in property value
        if (value.startsWith("{") && value.endsWith("}")) {
            String parsedProperty;
            // expecting array of property values for one particular property, e.g. control keys
            if (propertyName.equalsIgnoreCase("Control_Keys")) {
                parsedProperty = parseControlKeysPropertyValue(value);
            } else {
                throw new UnparsablePropertyException("Parsing array values is not supported for property "
                    + propertyName);
            }
            return parsedProperty.getBytes("UTF-8");
        } else if (value.startsWith("{") && !value.endsWith("}") || !value.startsWith("{") && value.endsWith("}")) {
            throw new UnparsablePropertyException(
                "The property value is not specified well. Did you want to specify array of values for this property?"
                    + "\nThe array must start with \"{\" and end with \"}\", fields separated by \",\"."
                    + "\nIf you have blank signs inside, it is necessary to encapsulate the array into quotation marks");
        }

        // default data file in property value
        if (value.endsWith("csd")) {
            File cops = new File(value);
            if (cops.exists()) {
                try {
                    return read(cops);
                } catch (Exception e) {
                    throw new UnparsablePropertyException("Parsing of cops data failed.");
                }

            }
        }

        // imei
        if (propertyName.toLowerCase(Locale.getDefault()).contains("imei")) {
            return ImeiConverter.encodeIMEI(propertyValue);
        }
        return propertyValue.getBytes("UTF-8");

    }

    /**
     * Read from file.
     * 
     * @param file
     *            File
     * @throws IOException
     *             If an I/O error occurred
     * @return content of the file in byte array
     */
    public byte[] read(File file) throws IOException {
        FileInputStream input = null;
        try {
            input = new FileInputStream(file);
            return read(input);
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    /**
     * Read from input stream.
     * 
     * @param input
     *            Input stream
     * @throws IOException
     *             If an I/O error occurred
     * @return content of the input stream in byte array
     */
    public byte[] read(InputStream input) throws IOException {

        BufferedReader reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        String line;
        StringBuilder content = new StringBuilder();

        line = reader.readLine();
        while ((line != null) && (!"".equals(line))) {
            content.append(line);
            line = reader.readLine();
        }
        return content.toString().getBytes("UTF-8");
    }

    private String parseControlKeysPropertyValue(String pPropertyValue) throws UnparsablePropertyException {
        StringBuilder parsedValues = new StringBuilder();
        ArrayList<String> namesOrder = new ArrayList<String>();
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);

        if (pPropertyValue.startsWith("{") && pPropertyValue.endsWith("}")) {
            pPropertyValue = pPropertyValue.substring(pPropertyValue.indexOf("{") + 1, pPropertyValue.lastIndexOf("}"));
        }
        if (pPropertyValue.contains("=")) {
            // names of values defined
            String[] values = pPropertyValue.split(",");
            if (values.length != namesOrder.size()) {
                throw new UnparsablePropertyException(
                    "The property value is not specified well. Please specify all requested value fields for "
                        + propertyName + ", for which the names are following: "
                        + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY + ", "
                        + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                        + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                        + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                        + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
            }
            String[] orderedValues = new String[values.length];
            for (int i = 0; i < values.length; i++) {
                String valueName = values[i].substring(0, values[i].indexOf("=")).trim();
                String value = values[i].substring(values[i].indexOf("=") + 1).trim();
                int j = namesOrder.indexOf(valueName);
                if (j == -1) {
                    throw new UnparsablePropertyException(
                        "The property value is not specified well. Please verify that all specified values names are correct."
                            + "The names are following: " + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY
                            + ", " + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                            + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                            + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                            + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
                }
                orderedValues[j] = value;
            }

            for (int i = 0; i < orderedValues.length; i++) {
                parsedValues.append(orderedValues[i]);
            }
        } else {
            throw new UnparsablePropertyException(
                "The property value is not specified well."
                    + " It should follow the pattern: {<value1_name>=<value1>,<value2_name>=<value2>,...<value5_name>=<value5>}"
                    + ", where the names are following: " + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY
                    + ", " + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                    + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                    + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                    + DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
        }
        return parsedValues.toString();
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        propertyID = null;
        propertyName = null;
        propertyValue = null;
        property = null;
        // pArguments contains: name of command, device_id, name of property=value to be set
        if (pArguments.length <= 2) {
            resultMessage = "Missing arguments. Probably property name or property value are not defined.";
            return;
        }
        if (pArguments[2].indexOf("=") == -1) {
            // value for such property is not defined
            resultMessage = "No property value defined. Please define property value in format: property=value";
            return;
        }

        String arg = pArguments[2].substring(0, pArguments[2].indexOf("="));
        if (arg.contains("0x") || isNumber(arg)) {
            String idDec;

            if (arg.startsWith("0x")) {
                idDec = arg.replace("0x", " ").trim();
                idDec = String.valueOf(Integer.parseInt(idDec, 16));
            } else {
                idDec = arg;
            }

            propertyID = idDec;
            if (mobileEquipment.isSecurity(propertyID, propertyName)) {
                property = mobileEquipment.getSecurityProperty(propertyID);
            } else {
                property = mobileEquipment.getProperty(propertyID);
            }
            if (property != null) {
                propertyName = property.getPropertyName();
            } else {
                resultMessage = "Property id is not valid.";
            }
        } else {
            propertyName = arg;
            if (mobileEquipment.isSecurity(propertyID, propertyName)) {
                property = mobileEquipment.getSecurityPropertyViaName(propertyName);
            } else {
                property = mobileEquipment.getPropertyViaName(propertyName);
            }
            if (property != null) {
                propertyID = property.getPropertyId();
            } else {
                resultMessage = "Property name is not valid.";
            }
        }
        propertyValue = pArguments[2].substring(pArguments[2].indexOf("=") + 1);

    }

    private boolean isNumber(String str) {
        char[] chars = str.toCharArray();

        for (int i = 0; i < chars.length - 1; i++) {
            if (!Character.isDigit(chars[i])) {
                return false;
            }
        }
        return true;

    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * UnparsablePropertyException should be used when the given property, its value or name is not
     * possible to parse to gain needed data
     */
    class UnsupportedPropertyException extends Exception {

        public UnsupportedPropertyException(String message) {
            super(message);
        }

        /**
         * 
         */
        private static final long serialVersionUID = 2069856118530650576L;
    }

    /**
     * UnparsablePropertyException should be used when the given property, its value or name is not
     * possible to parse to gain needed data
     */
    class UnparsablePropertyException extends UnsupportedPropertyException {

        public UnparsablePropertyException(String message) {
            super(message);
        }

        /**
         * 
         */
        private static final long serialVersionUID = 2842866983665932964L;
    }

}
