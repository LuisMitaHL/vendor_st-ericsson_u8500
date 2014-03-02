package com.stericsson.sdk.equipment.ux500.warm.tasks;

import java.util.Iterator;
import java.util.List;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.AbstractUX500WarmEquipment;
import com.stericsson.sdk.equipment.AbstractWarmEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;

/**
 * List file task
 *
 * @author esrimpa
 *
 */
public class UX500WarmEquipmentListFileTask extends AbstractWarmEquipmentTask {

    /** File not Downloaded */
    private static final String LINE_BREAK = "\n";

    /** Equipment instance */
    private AbstractUX500WarmEquipment mobileEquipment;

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
    public UX500WarmEquipmentListFileTask(AbstractUX500WarmEquipment equipment) {
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
    public void cancel() {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        notifyTaskMessage("Attempting to list dump files on the ME");
        List<String> result = mobileEquipment.getFtpService().listFile(mobileEquipment.getPort());

        // If task fails
        if (result == null) {
            resultCode = ERROR;
            resultMessage = "Failed to list files in equipment. FTP connection or FTP command failed.";
            return new EquipmentTaskResult(resultCode, resultMessage, null, false);
        }

        // If task does not fail, it will return the list of file names. Now construct the result
        // message according to the file name to parse later. For example.
        // resultMassage = "filaname1|Downloaded\nfilename2|Not Downloaded
        Iterator<String> it = result.iterator();
        resultMessage = "";
        while (it.hasNext()) {
            String value = (String) it.next();
            resultMessage = resultMessage + value + LINE_BREAK;
        }
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
        return CommandName.COREDUMP_LIST_FILE.name();
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
    }
}
