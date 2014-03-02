package com.stericsson.sdk.equipment.ui.jobs;

import org.apache.log4j.Logger;
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
public class DownloadDumpJob extends Job implements IEquipmentTaskListener {

    static final Logger log = Logger.getLogger(DownloadDumpJob.class);

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private long progressLength;

    private long lastProgressTime;

    private long lastProgressBytes;

    private IEquipmentTask downloadDumpTask;

    private String filePath;

    private String coreFileName;

    private long filesize = -1;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     * 
     * @param pCoreFileName
     *            The core file name to be downloaded
     * 
     * @param pFilePath
     *            The path where to download the file
     */
    public DownloadDumpJob(IEquipment pEquipment, String pCoreFileName, String pFilePath) {
        super("Download Dump " + pCoreFileName + " from: " + pEquipment.toString());
        equipment = pEquipment;
        filePath = pFilePath;
        coreFileName = pCoreFileName;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        this.progressMonitor = pMonitor;
        progressMonitor.beginTask("WAITING...", 100);
        try {

            // schedule task
            downloadDumpTask = equipment.createTask(new String[] {
                CommandName.COREDUMP_DOWNLOAD_DUMP.name(), equipment.getPort().getPortName(), coreFileName, filePath});
            if (downloadDumpTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            downloadDumpTask.addTaskListener(this);
            EquipmentTaskResult result = downloadDumpTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                formatDuration(result.getResultMessage(), result.getDuration(), progressLength));
        } catch (Exception e) {
            downloadDumpTask.cancel();
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
        if (task == downloadDumpTask) {
            if (filesize == -1) {
                filesize = totalBytes;
                progressMonitor.beginTask("Download Dump " + coreFileName + " " + equipment.toString(), 100);
            }
            long currentTimeMillis = System.currentTimeMillis();
            long time = currentTimeMillis - lastProgressTime;
            long bytes = transferredBytes - lastProgressBytes;
            double speed = 0;
            double seconds = (time / 1000.0);
            double precentageOfTotal = ((bytes * 100.0) / filesize);
            if (seconds != 0 && bytes > 0) {
                speed = bytes / seconds;
            }

            lastProgressBytes = transferredBytes;
            lastProgressTime = currentTimeMillis;
            progressMonitor.setTaskName(String.format("%.2f", speed / 1024.0) + " KB/s" + " Downloaded: "
                + (transferredBytes / 1024) + "KB/" + (filesize / 1024) + "KB");
            progressMonitor.worked((int) precentageOfTotal);
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task == downloadDumpTask) {
            progressMonitor.beginTask("Download Starting", 100);
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
