/**
 * 
 */
package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.SecurityPropertiesType;

/**
 * @author cizovhel
 * 
 */
public class DummyEquipmentBindPropertiesTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentBindPropertiesTask(DummyEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        try {
            resultMessage = equipment.getMock().getLoaderErrorDesc(resultCode);
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

        notifyTaskMessage("Binding equipment security properties.");

        notifyTaskStart();

        try {
            mobileEquipment.getMock().setDelay(1500);
            mobileEquipment.getMock().setResult(0);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        Object obj =
            mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SECURITY_BIND_PROPERTIES, new Object[] {}, this);

        SecurityPropertiesType spType;

        if (obj instanceof SecurityPropertiesType) {
            spType = (SecurityPropertiesType) obj;
            resultCode = spType.getStatus();
            // resultMessage = LCErrorCodes.getErrorDescription(resultCode);
            try {
                mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        } else if (obj instanceof Integer) {
            resultCode = (Integer) obj;
            // resultMessage = LCErrorCodes.getErrorDescription(resultCode);
            try {
                mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SECURITY_BIND_PROPERTIES.name();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        // nothing to implement
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

}
