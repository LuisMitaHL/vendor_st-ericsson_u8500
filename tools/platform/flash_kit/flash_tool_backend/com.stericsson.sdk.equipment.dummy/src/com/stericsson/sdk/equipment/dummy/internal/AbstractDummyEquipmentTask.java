package com.stericsson.sdk.equipment.dummy.internal;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;

/**
 * AbstractM7X00EquipmentFileSystemTask is template for tasks which only return Integer value which
 * means error or not error.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public abstract class AbstractDummyEquipmentTask extends AbstractEquipmentTask {

    /**
     * resultCode
     */
    protected int resultCode;

    /**
     * resultMessage
     */
    protected String resultMessage;

    /**
     * At arguments[0] is always methodName. the rest is set by setArguments() and contains all
     * arguments for task.
     */
    protected Object[] arguments;

    private final CommandName command;

    /**
     * @param equipment
     *            TBD
     * @param pCommand
     *            value from CommandName.*
     */
    public AbstractDummyEquipmentTask(DummyEquipment equipment, CommandName pCommand) {
        super(equipment);
        command = pCommand;
        resultCode = ERROR_NONE;
        resultMessage = "Operation successful. Operation finished successfully.";
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        throw new RuntimeException("not supported yet");
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return command.name();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * this method create and fill arguments parameter to be [arg1, arg2, ...]
     * 
     * @param pArguments
     *            array of arguments for method contains [sth, sth, arg1, arg2, ...]
     */
    public void setArguments(String[] pArguments) {
        arguments = new Object[pArguments.length - 2];
        System.arraycopy(pArguments, 2, arguments, 0, pArguments.length - 2);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + getEquipment();
    }
}
