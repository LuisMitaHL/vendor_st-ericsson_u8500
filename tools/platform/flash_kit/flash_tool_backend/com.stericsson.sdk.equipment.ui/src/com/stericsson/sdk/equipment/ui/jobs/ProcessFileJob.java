package com.stericsson.sdk.equipment.ui.jobs;

import java.io.File;
import java.util.HashMap;
import java.util.Locale;

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
import com.stericsson.sdk.equipment.ui.JobsArgHashKeys;

/**
 * Process file Job
 * 
 * @author xolabju
 * 
 */
public class ProcessFileJob extends Job implements IEquipmentTaskListener {

    private IEquipmentTask processFileTask;

    private long fileSize;

    private IEquipment equipment;

    private String filePath;

    long lastProgressTime;

    long lastProgressBytes;

    private IProgressMonitor progressMonitor;

    /**
     * Constructor
     * 
     * @param pFilePath
     *            the file to flash (process)
     * @param pEquipment
     *            the target equipment
     */
    public ProcessFileJob(String pFilePath, IEquipment pEquipment) {
        super("ProcessFile: " + pFilePath + " -> " + pEquipment.toString());
        filePath = pFilePath;
        equipment = pEquipment;

    }

    /**
     * Custom constructor used by job factories
     * 
     * @param args
     *            arguments of constructor in the order the constructor accepts them
     * @param pEquipment
     *            equipment on which the job should be run
     * @throws NoSuchMethodException
     *             thrown if the arguments do not fit to the arguments requested by constructor
     */
    public ProcessFileJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("ProcessFile on equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.SOURCE_PATH)) { // TODO Find right key
            filePath = args.get(JobsArgHashKeys.SOURCE_PATH);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.SOURCE_PATH + "!");
        }

        equipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public IStatus run(IProgressMonitor monitor) {
        File file = new File(filePath);
        if (!file.exists()) {
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "File " + filePath + " not found.");
        }
        fileSize = file.length();
        this.progressMonitor = monitor;
        progressMonitor.beginTask("Waiting...", (int) ((fileSize / 100.0)));
        try {
            // schedule task
            processFileTask = equipment.createTask(new String[] {
                CommandName.FLASH_PROCESS_FILE.name(), equipment.getPort().getPortName(), filePath});
            if (processFileTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            processFileTask.addTaskListener(this);
            EquipmentTaskResult result = processFileTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                formatDuration(result.getResultMessage(), result.getDuration(), fileSize));

        } catch (Exception e) {
            processFileTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {

    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        if (task == processFileTask) {
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
            progressMonitor.worked((int) ((bytes / 100.0)));

        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task == processFileTask) {
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

        return String.format(Locale.getDefault(), "%s (%.2fMB in %02d:%02d:%02d.%03d, avg. speed %.2fKB/s)", message,
            transferred, hours, minutes, seconds, milliseconds, speed);
    }
}
