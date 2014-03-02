package com.stericsson.sdk.equipment.dummy.internal.task;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;

/**
 * Task for downloading dump from a Dummy ME
 * 
 * @author esrimpa
 * 
 */
public class DummyEquipmentDownloadDumpTask extends AbstractWarmEquipmentTask {

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
    public DummyEquipmentDownloadDumpTask(DummyEquipment equipment) {
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
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to download dump file from ME");

        long speed = 60000 * 1024L;
        long numProgress = 50;
        long length = 270569024L;
        double seconds = (double) length / (double) speed;
        double sleep = (seconds / (double) numProgress) * 1000;

        for (int i = 1; i < numProgress; i++) {
            try {
                Thread.sleep((long) Math.floor(sleep));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            notifyTaskProgress(length, i * (length / numProgress));
        }
        try {
            Thread.sleep((long) Math.ceil(sleep));
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        notifyTaskProgress(length, length);
        notifyTaskMessage("Download dump is completed");
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
        return CommandName.COREDUMP_DOWNLOAD_DUMP.name();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return getId() + "@" + mobileEquipment.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
    }

}
