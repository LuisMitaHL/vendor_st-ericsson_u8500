package com.stericsson.sdk.equipment.m7x00.tasks;

import java.io.File;

import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;

/**
 * Check compatibility of a load module (residing on a PC) with the load modules that are installed
 * on the ME. This command is valid for m7x00 only.
 * 
 * @author xadazim
 * 
 */
public class M7X00EquipmentCheckCompatibilityTask extends AbstractEquipmentTask {

    /**
     * Id of this task.
     */
    public static final String TASK_ID = "CHECK_COMPATIBILITY";

    private String[] arguments;

    /**
     * Constructs the task.
     * 
     * @param pEquipment
     *            device against which the module compatibility shall be checked
     */
    public M7X00EquipmentCheckCompatibilityTask(AbstractLoaderCommunicationEquipment pEquipment) {
        super(pEquipment);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        if (arguments == null || arguments.length != 3) {
            return new EquipmentTaskResult(-1, "Check compatibility task was started with wrong number of arguments",
                null, false);
        }
        // arguments are command;equipment;path
        File loadModule = new File(arguments[2]);

        notifyTaskStart();
        getEquipment().setStatus(EquipmentState.BUSY);
        BSUUtils.BSUResult bsuResult = null;
        try {
             bsuResult = BSUUtils.checkCompatibility((M7x00Equipment) getEquipment(), loadModule);
        } catch (Exception e) {
            return new EquipmentTaskResult(-1, e.getMessage(), null, false);
        } finally {
            getEquipment().setStatus(EquipmentState.IDLE);
        }

        return new EquipmentTaskResult(bsuResult.getExitCode(), bsuResult.getText(), bsuResult, false);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String getId() {
        return TASK_ID;
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
    public void cancel() {
        throw new RuntimeException("Task is not cancellable");
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        arguments = pArguments;
    }

}
