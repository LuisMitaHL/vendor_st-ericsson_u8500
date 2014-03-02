package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Dummy implementation of authenticate with control keys task.
 * Does nothing, returns success.
 * @author xadazim
 *
 */
public class DummyEquipmentAuthenticateControlKeysTask extends AbstractEquipmentTask {

    /**
     * Constructor
     * @param pEquipment ignored
     */
    public DummyEquipmentAuthenticateControlKeysTask(DummyEquipment pEquipment) {
        super(pEquipment);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void cancel() {
    }

    /**
     * 
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        return new EquipmentTaskResult(0, "Authentication successfull", null, false);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name();
    }

    /**
     * 
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
    }

}
