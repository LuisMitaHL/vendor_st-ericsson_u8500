/**
 * 
 */
package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * @author xhelciz
 * 
 */
public class DummyEquipmentGetAvailableSecurityPropertiesTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    static final String PROPERTY_NAME = "Name";

    static final String PROPERTY_ID = "ID";

    static final String PROPERTY_WRITE = "Write";

    static final String PROPERTY_READ = "Read";

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentGetAvailableSecurityPropertiesTask(DummyEquipment equipment) {
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
        notifyTaskStart();

        notifyTaskMessage("Displaying list of available equipment properties.");

        String propertiesList = getPropertiesList();
        try {
            if (propertiesList != null) {
                resultCode = ERROR_NONE;
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode) + propertiesList;

            } else {
                // fail
                resultCode = 5;
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode) + " No properties found.";
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    private String getPropertiesList() {
        // SecurityProperty Name=IMEI ID=0x01 Write=true Read=true
        String propertyList = "IMEI:0x01:true:true";
        return propertyList;
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.BACKEND_GET_AVAILABLE_SECURITY_PROPERTIES.name();
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
