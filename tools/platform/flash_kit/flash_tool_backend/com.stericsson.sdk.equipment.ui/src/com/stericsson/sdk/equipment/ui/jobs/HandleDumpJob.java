package com.stericsson.sdk.equipment.ui.jobs;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Job to list dump files from ME.
 * 
 * @author esrimpa
 * 
 */
public class HandleDumpJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private IEquipmentTask listFileTask;

    private boolean isDone;

    private String resultMessage;

    private int resultValue;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     */
    public HandleDumpJob(IEquipment pEquipment) {
        super("Handle Dump: " + pEquipment.toString());
        equipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        this.progressMonitor = pMonitor;
        progressMonitor.setTaskName("Waiting...");
        try {
            // schedule task
            listFileTask = equipment.createTask(new String[] {
                CommandName.COREDUMP_LIST_FILE.name()});
            if (listFileTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            listFileTask.addTaskListener(this);
            EquipmentTaskResult result = listFileTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            listFileTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }
    }


    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask pTask, String pMessage) {

    }

    /**
     * Need to get the error message
     * 
     * @return return the result messsage
     */
    public String taskMessage() {
        if ((resultValue == 0) && (resultMessage != null)) {
            return resultMessage;
        } else {
            return null;
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask pTask, long pTotalBytes, long pTransferredBytes) {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask pTask) {
        progressMonitor.beginTask("List File " + equipment.toString(), IProgressMonitor.UNKNOWN);
    }

    /**
     * Check job status
     * 
     * @return true on task completion
     */
    public boolean taskCompletedStatus() {
        return isDone;
    }
}
