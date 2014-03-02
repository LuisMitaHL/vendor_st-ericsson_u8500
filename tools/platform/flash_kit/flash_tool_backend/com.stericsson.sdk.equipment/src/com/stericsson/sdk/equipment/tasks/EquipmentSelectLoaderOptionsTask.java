package com.stericsson.sdk.equipment.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.loader.communication.LCDefinitions;


/**
 * @author - Goran Gjorgoski(xxvs0005)  <Goran.Gjorgoski@seavus.com>
 *
 */
public class EquipmentSelectLoaderOptionsTask extends AbstractEquipmentTask {

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private long uiProperty;

    private long uiValue;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public EquipmentSelectLoaderOptionsTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        // Store equipment object
        mobileEquipment = equipment;

    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Force writing task.");

        Object obj =
            mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_FLASH_SELECT_LOADER_OPTIONS, 
                new Object[] {uiProperty, uiValue}, this);

        resultCode = (Integer) obj;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);

        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.FLASH_SELECT_LOADER_OPTIONS.name();
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
        uiProperty = Long.parseLong(pArguments[2]);
        uiValue = Long.parseLong(pArguments[3]);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }
}