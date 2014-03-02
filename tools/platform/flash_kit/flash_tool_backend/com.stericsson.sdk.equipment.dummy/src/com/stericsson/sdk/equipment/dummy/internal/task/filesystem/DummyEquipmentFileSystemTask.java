package com.stericsson.sdk.equipment.dummy.internal.task.filesystem;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * AbstractM7X00EquipmentFileSystemTask is template for tasks which only return Integer value which
 * means error or not error.
 * 
 * @author Vit Sykala <vit.sykala@tieto.com>
 */
public class DummyEquipmentFileSystemTask extends AbstractEquipmentTask {

    /**
     * resultCode
     */
    protected int resultCode;

    /**
     * resultMessage
     */
    protected String resultMessage;

    /**
     * result object
     */
    protected Object resultObject;

    /**
     * At arguments[0] is always methodName. the rest is set by setArguments() and contains all
     * arguments for task.
     */
    protected Object[] arguments;

    private final CommandName command;

    /**
     * some method from LCDefinitions.*
     */
    protected final String methodName;

    /**
     * mobileEquipment
     */
    protected DummyEquipment mobileEquipment;

    /**
     * @param equipment
     *            TBD
     * @param pMethodName
     *            value from LCDefinitions.*
     * @param pCommand
     *            value from CommandName.*
     */
    public DummyEquipmentFileSystemTask(DummyEquipment equipment, String pMethodName, CommandName pCommand) {
        super(equipment);
        command = pCommand;
        methodName = pMethodName;
        resultCode = ERROR_NONE;
        mobileEquipment = equipment;
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
        throw new RuntimeException("not supported yet");
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        resultObject = ((DummyEquipment) getEquipment()).sendCommand(methodName, arguments, this);
        updateResult(resultObject);
        return new EquipmentTaskResult(resultCode, resultMessage, resultObject, false);
    }

    /**
     * Update values of resultCode and resultMessage.
     * 
     * @param resultObj
     *            is return value from equipment.
     */
    protected void updateResult(Object resultObj) {
        if (resultObj instanceof Integer) {
            resultCode = (Integer) resultObj;
            try {
                resultMessage = mobileEquipment.getMock().getLoaderErrorDesc(resultCode);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
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
     * @param pArguments
     *            array of arguments for method
     */
    public void setArguments(String[] pArguments) {
        arguments = new Object[pArguments.length - 2];
        // arguments[0] = methodName;
        System.arraycopy(pArguments, 2, arguments, 0, pArguments.length - 2);
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + getEquipment();
    }
}
