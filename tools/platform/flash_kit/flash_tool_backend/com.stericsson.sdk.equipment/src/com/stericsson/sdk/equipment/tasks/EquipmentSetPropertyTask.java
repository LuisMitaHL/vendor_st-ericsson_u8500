/**
 * 
 */
package com.stericsson.sdk.equipment.tasks;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Locale;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.UnparsablePropertyException;
import com.stericsson.sdk.equipment.UnsupportedPropertyException;
import com.stericsson.sdk.equipment.internal.EquipmentProfile;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author cizovhel
 * 
 */
public class EquipmentSetPropertyTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

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
    public EquipmentSetPropertyTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
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
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        if (propertyID == null || propertyValue == null || propertyName == null || property == null) {
            // wrong propertyName or missing propertyValue
            resultCode = ERROR;
        } else {
            notifyTaskMessage("Setting equipment property: propertyID = " + propertyID + ", propertyName = "
                + propertyName + ", value = " + propertyValue);
            notifyTaskStart();

            try {
                byte[] propertyBytes = gainBytesFromPropertyValue(propertyValue);
                int id = Integer.parseInt(propertyID, 10);

                Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_SET_PROPERTY, new Object[] {
                    id, propertyBytes.length, propertyBytes}, this);

                resultCode = (Integer) obj;
                resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            } catch (UnsupportedEncodingException e) {
                resultCode = ERROR;
                resultMessage = e.getMessage();
                return new EquipmentTaskResult(resultCode, resultMessage, null, false);
            } catch (UnsupportedPropertyException e) {
                resultCode = ERROR;
                resultMessage = e.getMessage();
                return new EquipmentTaskResult(resultCode, resultMessage, null, false);
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    private byte[] gainBytesFromPropertyValue(String value) throws UnsupportedEncodingException,
        UnsupportedPropertyException {

        // array in property value
        if (value.startsWith("{") && value.endsWith("}")) {
            // String parsedProperty;
            byte[] parsedProperty;
            // expecting array of property values for one particular property, e.g. control keys
            if (propertyName.equalsIgnoreCase("Control_Keys")) {
                parsedProperty = parseControlKeysPropertyValue(value);
            } else {
                throw new UnparsablePropertyException("Parsing array values is not supported for property "
                    + propertyName);
            }
            return parsedProperty;
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
                } catch (IOException e) {
                    throw new UnparsablePropertyException("Parsing of cops data failed.");
                }

            }
        }

        // imei
        if (propertyName.toLowerCase(Locale.getDefault()).contains("imei")) {
            return ImeiConverter.getImeiAsBytes(propertyValue);
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

            byte[] binary = new byte[(int) file.length()];
            input = new FileInputStream(file);
            if (input.read(binary) != file.length()) {
                throw new IOException("Cannot read file '" + file.getAbsolutePath() + "' into byte array!");
            }

            return binary;
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    private byte[] parseControlKeysPropertyValue(String pPropertyValue) throws UnparsablePropertyException {
        ArrayList<String> namesOrder = new ArrayList<String>();
        byte[] temp = null;
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);

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
                        + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY + ", "
                        + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                        + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                        + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                        + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
            }
            String[] orderedValues = new String[values.length];
            for (int i = 0; i < values.length; i++) {
                String valueName = values[i].substring(0, values[i].indexOf("=")).trim();
                String value = values[i].substring(values[i].indexOf("=") + 1).trim();
                int j = namesOrder.indexOf(valueName);
                if (j == -1) {
                    throw new UnparsablePropertyException(
                        "The property value is not specified well. Please verify that all specified values names are correct."
                            + "The names are following: "
                            + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY + ", "
                            + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                            + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                            + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                            + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
                }

                // key length check
                if (value.length() < EquipmentProfile.MIN_SIMLOCK_CODE_LENGTH
                    || value.length() > EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH) {
                    throw new UnparsablePropertyException(
                        "The property value is not specified well. Please verify that all keys are "
                            + EquipmentProfile.MIN_SIMLOCK_CODE_LENGTH + " to "
                            + EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH + " digits long");
                }
                orderedValues[j] = value;
            }

            // padding keys to 16 bytes
            temp = new byte[EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH * orderedValues.length];
            int offset = 0;
            int ii = 0;
            Byte filler = 0x00;
            for (int i = 0; i < orderedValues.length; i++) {
                byte[] temp1 = new byte[EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH];
                for (ii = 0; ii < orderedValues[i].length(); ii++) {
                    temp1[ii] = (byte) orderedValues[i].charAt(ii);
                }
                while (ii < EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH) {
                    temp1[ii] = filler;
                    ii++;
                }
                System.arraycopy(temp1, 0, temp, offset, EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH);
                offset += EquipmentProfile.MAX_SIMLOCK_CODE_LENGTH;
            }
        } else {
            throw new UnparsablePropertyException(
                "The property value is not specified well."
                    + " It should follow the pattern: {<value1_name>=<value1>,<value2_name>=<value2>,...<value5_name>=<value5>}"
                    + ", where the names are following: "
                    + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY + ", "
                    + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY + ", "
                    + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY + ", "
                    + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY + ", "
                    + EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);
        }
        return temp;
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
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}
