/**
 * 
 */
package com.stericsson.sdk.equipment.tasks;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.internal.EquipmentProfile;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;
import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author cizovhel
 * 
 */
public class EquipmentGetPropertyTask extends AbstractEquipmentTask {

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
    public EquipmentGetPropertyTask(AbstractLoaderCommunicationEquipment equipment) {
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
        if (propertyID == null || propertyName == null || property == null) {
            // wrong propertyName
            resultCode = ERROR;
        } else {
            notifyTaskStart();
            notifyTaskMessage("Gaining equipment properties.");
            getProperty();
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    private String getProperty() {
        SecurityPropertiesType spType;

        if (!property.isSecurity()) {

            propertyValue = property.getPropertyValue();
            mobileEquipment.setProperty(propertyID, propertyName, propertyValue, false);
            resultMessage =
                mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode) + "\n" + propertyName
                    + ": " + propertyValue;
        } else {
            int id = Integer.parseInt(propertyID);
            Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_PROPERTY, new Object[] {
                id}, this);

            if (obj instanceof SecurityPropertiesType) {
                spType = (SecurityPropertiesType) obj;

                resultCode = spType.getStatus();
                if (resultCode == 0) {
                    try {
                        propertyValue = getValueFromByteBuffer(spType.getBuffer());

                    } catch (UnsupportedEncodingException e) {
                        resultCode = ERROR;
                        resultMessage = e.getMessage();
                        return null;
                    } catch (Exception e) {
                        resultCode = ERROR;
                        resultMessage = e.getMessage();
                        return null;
                    }
                    mobileEquipment.setProperty(String.valueOf(id), propertyName, propertyValue, property.isSecurity(),
                        property.isReadable(), property.isWritable());

                    resultMessage =
                        mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode) + "\n"
                            + propertyName + ": " + propertyValue;
                } else {
                    resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
                }
            } else if (obj instanceof Integer) {
                resultCode = (Integer) obj;
                resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);

            }
        }
        return propertyValue;
    }

    private String getValueFromByteBuffer(byte[] buffer) throws UnsupportedEncodingException, Exception {
        if (propertyName.equalsIgnoreCase("Control_Keys")) {
            return composeControlKeysPropertyValue(buffer);
        } else if (propertyName.equalsIgnoreCase("imei")) {
            return ImeiConverter.getImeiAsString(buffer);
        } else if (propertyName.equalsIgnoreCase("imei_in_flash")) {
            return ImeiConverter.getImeiAsString(buffer);
        } else {
            return new String(buffer, "UTF-8");
        }
    }

    private String composeControlKeysPropertyValue(byte[] buffer) throws UnsupportedEncodingException {
        ArrayList<String> namesOrder = new ArrayList<String>();
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY);
        namesOrder.add(EquipmentProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);

        String value = new String(buffer, "UTF-8");
        StringBuffer composedValue = new StringBuffer();
        int i = 0;
        for (int index = 0; index < value.length(); index += 16, i++) {
            composedValue.append(namesOrder.get(i) + " " + value.substring(index, index + 15) + ", ");
        }
        return composedValue.toString();
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arg) {
        propertyID = null;
        propertyName = null;
        property = null;
        // pArguments contains: name of command, device_id, name or id of property to be gained
        if (arg.length <= 2) {
            resultMessage = "Missing arguments. Probably property name or property id is not defined.";
            return;
        }

        if (arg[2].contains("0x") || isNumber(arg[2])) {
            String idDec;

            if (arg[2].startsWith("0x")) {
                idDec = arg[2].replace("0x", " ").trim();
                idDec = String.valueOf(Integer.parseInt(idDec, 16));
            } else {
                idDec = arg[2];
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
            propertyName = arg[2];
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
