/**
 * 
 */
package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.UnsupportedEncodingException;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentProperty;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;
import com.stericsson.sdk.security.util.ImeiConverter;

/**
 * @author xhelciz
 * 
 */
public class DummyEquipmentGetPropertyTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /** Equipment properties gained at device run */

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
    public DummyEquipmentGetPropertyTask(DummyEquipment equipment) {
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

        if (propertyID.startsWith("0x")) {
            propertyID = propertyID.replace("0x", " ").trim();
        }
        int id = Integer.parseInt(propertyID, 16);
        try {
            if (id == 01) { // imei in flash
                mobileEquipment.getMock().setResult(
                    new SecurityPropertiesType(0, ImeiConverter.encodeIMEI("12345678901234")));
            }

        } catch (Exception e) {
            System.out.println(e.getStackTrace());
        }

        Object obj = mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_GET_PROPERTY, new Object[] {
            id}, this);
        try {
            if (obj instanceof SecurityPropertiesType) {
                spType = (SecurityPropertiesType) obj;

                resultCode = spType.getStatus();
                if (resultCode == 0) {
                    try {
                        propertyValue = new String(spType.getBuffer(), "UTF-8");
                    } catch (UnsupportedEncodingException e) {
                        propertyValue = "";
                        e.printStackTrace();
                    }
                    mobileEquipment.setProperty(String.valueOf(id), propertyName, propertyValue, property.isSecurity(),
                        property.isReadable(), property.isWritable());
                    resultMessage =
                        mobileEquipment.getMock().getLoaderErrorDesc(resultCode) + "\n" + propertyName + ": "
                            + propertyValue;
                } else {
                    resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
                }
            } else if (obj instanceof Integer) {
                resultCode = (Integer) obj;

                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);

            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return propertyValue;
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
