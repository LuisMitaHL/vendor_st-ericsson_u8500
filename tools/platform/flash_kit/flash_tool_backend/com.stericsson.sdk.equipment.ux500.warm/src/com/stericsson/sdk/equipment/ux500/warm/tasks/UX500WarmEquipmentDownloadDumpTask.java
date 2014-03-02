package com.stericsson.sdk.equipment.ux500.warm.tasks;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * Download dump task
 * 
 * @author esrimpa
 * 
 */
public class UX500WarmEquipmentDownloadDumpTask extends AbstractWarmEquipmentTask {

    /** Equipment instance */
    private AbstractUX500WarmEquipment mobileEquipment;

    /** Result message for this task */
    private String resultMessage;

    /** Result code for this task */
    private int resultCode;

    private String downloadPath = null;

    private String coreFileName = null;

    private boolean isAutoDelete = false;

    /**
     * Constructor.
     * 
     * @param equipment
     *            Equipment interface
     */
    public UX500WarmEquipmentDownloadDumpTask(AbstractUX500WarmEquipment equipment) {
        super(equipment);

        // Store UX500 equipment object
        mobileEquipment = equipment;

        // Initialize task result with default information
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        long start = System.currentTimeMillis();
        if (resultCode == -1) {
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }
        notifyTaskStart();
        notifyTaskMessage("Attempting to download dump file from ME");
        int result =
            mobileEquipment.getFtpService().downloadDump(mobileEquipment.getPort(), coreFileName, downloadPath,
                isAutoDelete, this);
        if (result != ERROR_NONE) {
            resultCode = result;
            resultMessage = "Failed to download file from equipment";
        } else {
            mobileEquipment.setStatus(EquipmentState.IDLE);
        }
        long stop = System.currentTimeMillis();
        return new EquipmentTaskResult(resultCode, resultMessage, null, false, stop - start);
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
    public void cancel() {
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
    public String getId() {
        return CommandName.COREDUMP_DOWNLOAD_DUMP.name();
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
        coreFileName = null;
        downloadPath = null;
        isAutoDelete = false;

        // pArguments contains: name of command, device_id, name of property=value to be set
        if (pArguments.length < 4) {
            resultMessage = "Missing arguments. Probably some of the arguments are not provided is not provided.";
            resultCode = -1;
            return;
        }
        coreFileName = pArguments[2];
        downloadPath = pArguments[3];

        // Need to get the auto delete parameter
        if (pArguments.length == 5) {
            isAutoDelete = Boolean.parseBoolean(pArguments[4]);
        }
    }
}
