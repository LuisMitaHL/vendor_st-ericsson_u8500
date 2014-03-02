/**
 * 
 */
package com.stericsson.sdk.equipment.tasks;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;

import org.apache.log4j.Logger;

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
 * U8500EquipmentGetPropertiesTask serves to get a list of all properties and its values available
 * in connected mobile equipment. It has to be stated which property is of security type.
 * 
 * @author cizovhel
 * 
 */
public class EquipmentGetPropertiesTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private AbstractLoaderCommunicationEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /**
     * 
     */
    private static Logger logger = Logger.getLogger(EquipmentBootTask.class);

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentGetPropertiesTask(AbstractLoaderCommunicationEquipment equipment) {
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
        notifyTaskStart();

        notifyTaskMessage("Gaining equipment properties.");

        Hashtable<String, EquipmentProperty> securityProperties = mobileEquipment.getSupportedSecurityProperties();
        Iterator<String> it = securityProperties.keySet().iterator();
        while (it.hasNext()) {
            String key = it.next();
            EquipmentProperty secProp = securityProperties.get(key);
            if (secProp != null && secProp.isReadable()) {
                getPropertyValue(secProp);
            }
        }

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    private String getPropertyValue(EquipmentProperty securityProperty) {
        SecurityPropertiesType spType;
        String propertyValue = null;

        String propertyID = securityProperty.getPropertyId();

        if (propertyID.startsWith("0x")) {
            propertyID = propertyID.replace("0x", " ").trim();
        }

        int id = Integer.parseInt(propertyID, 16);

        Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_PROPERTY, new Object[] {
            id}, this);
        if (obj instanceof SecurityPropertiesType) {
            spType = (SecurityPropertiesType) obj;

            resultCode = spType.getStatus();
            if (resultCode == 0) {
                try {
                    propertyValue = getValueFromByteBuffer(securityProperty.getPropertyName(), spType.getBuffer());

                } catch (UnsupportedEncodingException e) {
                    resultCode = ERROR;
                    resultMessage = e.getMessage();
                    return null;
                }
                mobileEquipment.setProperty(String.valueOf(id), securityProperty.getPropertyName(), propertyValue,
                    securityProperty.isSecurity(), securityProperty.isReadable(), securityProperty.isWritable());
                resultMessage =
                    mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode) + "\n"
                        + securityProperty.getPropertyName() + ": " + propertyValue;
            } else {
                resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            }
        } else if (obj instanceof Integer) {
            resultCode = (Integer) obj;
            resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);

        }
        return propertyValue;
    }

    private String getValueFromByteBuffer(String name, byte[] buffer) throws UnsupportedEncodingException {
        if (name.equalsIgnoreCase("Control_Keys")) {
            return composeControlKeysPropertyValue(buffer);
        } else if (name.equalsIgnoreCase("imei_in_flash")) {
            String s = "";
            try {
                s = ImeiConverter.getImeiAsString(buffer);
            } catch (Exception e) {
                StringBuffer sb = new StringBuffer();
                for (int i = 0; i < buffer.length; i++) {
                    sb.append(buffer[i]);
                }
                logger.error("Unconvertible IMEI! Buffer content : " + sb.toString());
                throw new UnsupportedEncodingException();
            }
            return s;
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

        // String test =
        // "00112233445566771122334455667788223344556677889933445566778899004455667788990011";
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
        return CommandName.SECURITY_GET_EQUIPMENT_PROPERTIES.name();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

}
