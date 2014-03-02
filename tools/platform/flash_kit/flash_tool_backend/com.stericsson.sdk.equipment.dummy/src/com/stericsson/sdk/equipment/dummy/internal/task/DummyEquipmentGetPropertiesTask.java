/**
 * 
 */

package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.equipment.dummy.internal.DummyProfile;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;

/**
 * @author cizovhel
 * @author xolabju
 * 
 */
public class DummyEquipmentGetPropertiesTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /** Equipment properties gained at device run */

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentGetPropertiesTask(DummyEquipment equipment) {
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
        notifyTaskStart();

        notifyTaskMessage("Gaining equipment properties.");

        Hashtable<String, EquipmentProperty> securityProperties = mobileEquipment.getSupportedSecurityProperties();
        Iterator<String> it = securityProperties.keySet().iterator();
        while (it.hasNext()) {
            String key = it.next();
            EquipmentProperty secProp = securityProperties.get(key);
            getPropertyValue(secProp);
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
            try {
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
                        mobileEquipment.getMock().getLoaderErrorDesc(resultCode) + "\n"
                            + securityProperty.getPropertyName() + ": " + propertyValue;

                } else {
                    resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else if (obj instanceof Integer) {
            resultCode = (Integer) obj;
            try {
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                e.printStackTrace();
            }

        }

        return propertyValue;
    }

    private String getValueFromByteBuffer(String name, byte[] buffer) throws UnsupportedEncodingException {
        if (name.equalsIgnoreCase("Control_Keys")) {
            return composeControlKeysPropertyValue(buffer);
        } else {
            return new String(buffer, "UTF-8");
        }
    }

    private String composeControlKeysPropertyValue(byte[] buffer) throws UnsupportedEncodingException {
        ArrayList<String> namesOrder = new ArrayList<String>();
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_NETWORK_SUBSET_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_SERVICE_PROVIDER_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_CORPORATE_LOCK_KEY);
        namesOrder.add(DummyProfile.SECURITY_PROPERTY_CTRL_KEYS_FLEXIBLE_ESL_LOCK_KEY);

        String value = new String(buffer, "UTF-8");
        StringBuilder composedValue = new StringBuilder();
        int i = 0;
        for (int index = 0; index < value.length(); index += 16, i++) {
            composedValue.append(namesOrder.get(i));
            composedValue.append(" ");
            composedValue.append(value.substring(index, index + 15));
            composedValue.append(", ");
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
    public boolean isCancelable() {
        return false;
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
