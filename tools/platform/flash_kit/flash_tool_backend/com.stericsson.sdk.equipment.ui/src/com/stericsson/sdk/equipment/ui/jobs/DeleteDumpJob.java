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
 * Job to execute the download dump files from ME.
 * 
 * @author esrimpa
 * 
 */
public class DeleteDumpJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private long progressLength;

    long lastProgressTime;

    long lastProgressBytes;

    private IEquipmentTask deleteDumpTask;

    String coreFileName;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     * 
     * @param pCoreFileName
     *            The core file name to be downloaded
     * 
     */
    public DeleteDumpJob(IEquipment pEquipment, String pCoreFileName) {
        super("Delete Dump: " + pEquipment.toString());
        equipment = pEquipment;
        coreFileName = pCoreFileName;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        this.progressMonitor = pMonitor;
        progressMonitor.beginTask("Waiting...", (int) ((progressLength / 10.0)));

        try {

            // schedule task
            deleteDumpTask = equipment.createTask(new String[] {
                CommandName.COREDUMP_DELETE_DUMP.name(), equipment.getPort().getPortName(), coreFileName});
            if (deleteDumpTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            deleteDumpTask.addTaskListener(this);
            EquipmentTaskResult result = deleteDumpTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                formatDuration(result.getResultMessage(), result.getDuration(), progressLength));
        } catch (Exception e) {
            deleteDumpTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask pTask, String pMessage) {
        // TODO Auto-generated method stub
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        if (task == deleteDumpTask) {
            long currentTimeMillis = System.currentTimeMillis();
            long time = currentTimeMillis - lastProgressTime;
            long bytes = transferredBytes - lastProgressBytes;
            double speed = 0;
            double seconds = (time / 1000.0);
            if (seconds != 0) {
                speed = bytes / seconds;
            }

            lastProgressBytes = transferredBytes;
            lastProgressTime = currentTimeMillis;
            progressMonitor.setTaskName(String.format("%.2f", speed / 1024.0) + " KB/s");
            progressMonitor.worked((int) ((bytes / 10)));
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        progressMonitor.beginTask("Delete Dump " + equipment.toString(), IProgressMonitor.UNKNOWN);

        if (task == deleteDumpTask) {
            lastProgressTime = System.currentTimeMillis();

        }

    }

    /**
     * {@inheritDoc}
     */
    private String formatDuration(String message, long duration, long totalTransferred) {
        long hours = duration / 3600000;
        long minutes = (duration - hours * 3600000) / 60000;
        long seconds = (duration - hours * 3600000 - minutes * 60000) / 1000;
        long milliseconds = duration - hours * 3600000 - minutes * 60000 - seconds * 1000;

        double transferred = totalTransferred / (1024.0 * 1024.0);
        double speed = (totalTransferred / (duration / 1000.0)) / 1024.0;

        return String.format("%s (%.2fMB in %02d:%02d:%02d.%03d, avg. speed %.2fKB/s)", message, transferred, hours,
            minutes, seconds, milliseconds, speed);
    }

}
