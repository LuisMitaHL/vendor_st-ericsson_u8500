package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.File;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for processing a file on a Dummy ME
 * 
 * @author xolabju
 * 
 */
public class DummyEquipmentProcessFileTask extends AbstractEquipmentTask {

    /** Equipment instance */
    private DummyEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String path;

    private boolean isCancelled;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public DummyEquipmentProcessFileTask(DummyEquipment equipment) {
        super(equipment);

        // Store equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        isCancelled = true;
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskMessage("ProcessFile:" + path);
        notifyTaskStart();
        mobileEquipment.setStatus(EquipmentState.FLASHING);
        // speed in bytes/s
        long speed = 60000 * 1024L;
        long numProgress = 50;
        long length = 270569024L;
        File file = new File(path);
        if (file.exists() && file.isFile()) {
            length = file.length();
        }
        double seconds = (double) length / (double) speed;
        double sleep = (seconds / (double) numProgress) * 1000;

        for (int i = 1; i < numProgress; i++) {
            try {
                Thread.sleep((long) Math.floor(sleep));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            notifyTaskProgress(length, i * (length / numProgress));
            if (isCancelled) {
                resultMessage = "Cancelled by user";
                resultCode = -1;
                mobileEquipment.setStatus(EquipmentState.IDLE);
                return new EquipmentTaskResult(resultCode, resultMessage, null, false);
            }
        }
        try {
            Thread.sleep((long) Math.ceil(sleep));
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        notifyTaskProgress(length, length);
        mobileEquipment.setStatus(EquipmentState.IDLE);
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
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
    public String getId() {
        return CommandName.FLASH_PROCESS_FILE.name();
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
        path = arguments[2];
    }

}
