package com.stericsson.sdk.equipment.ui.jobs;

import java.util.HashMap;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.JobsArgHashKeys;

/**
 * @author xdancho
 * 
 */
public class DumpAreaJob extends Job implements IEquipmentTaskListener {

    private IEquipmentTask dumpAreaTask;

    private IEquipment equipment;

    private String start;

    private String length;

    private String dumpPath;

    private String saveToPath;

    private String skipRedundant;

    private String includeBadBlocks;

    private long progressLength;

    long lastProgressTime;

    long lastProgressBytes;

    private IProgressMonitor progressMonitor;

    /**
     * 
     * @param pDumpPath
     *            the device to dump
     * @param pStart
     *            the start of the dump
     * @param pLength
     *            the length of the dump
     * @param pSaveToPath
     *            the path to store the dump
     * @param pEquipment
     *            the equipment to perform the dump on
     * @param pSkipRedundant
     *            skip redundant area
     * @param pIncludeBadBlocks
     *            include bad blocks
     */
    public DumpAreaJob(String pDumpPath, String pStart, String pLength, String pSaveToPath, IEquipment pEquipment,
        String pSkipRedundant, String pIncludeBadBlocks) {
        super("DumpArea: " + pDumpPath + " on " + pEquipment.toString() + " -> " + pSaveToPath);
        dumpPath = pDumpPath;
        start = pStart;
        length = pLength;
        if (HexUtilities.hasHexPrefix(length)) {
            progressLength = Long.parseLong(HexUtilities.removeHexPrefix(length), 16);
        } else {
            progressLength = Long.parseLong(length);
        }
        saveToPath = pSaveToPath;
        equipment = pEquipment;
        skipRedundant = pSkipRedundant;
        includeBadBlocks = pIncludeBadBlocks;
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
    public DumpAreaJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("DumpArea equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.AREA_PATH)) {// TODO Find right key on code
            dumpPath = args.get(JobsArgHashKeys.AREA_PATH);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.AREA_PATH + "!");
        }

        if (args.containsKey(JobsArgHashKeys.OFFSET)) {
            start = args.get(JobsArgHashKeys.OFFSET);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.OFFSET + "!");
        }

        if (args.containsKey(JobsArgHashKeys.LENGTH)) {
            length = args.get(JobsArgHashKeys.LENGTH);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.LENGTH + "!");
        }

        if (HexUtilities.hasHexPrefix(length)) {
            progressLength = Long.parseLong(HexUtilities.removeHexPrefix(length), 16);
        } else {
            progressLength = Long.parseLong(length);
        }

        if (args.containsKey(JobsArgHashKeys.DESTINATION_PATH)) {
            saveToPath = args.get(JobsArgHashKeys.DESTINATION_PATH);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.DESTINATION_PATH + "!");
        }

        equipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    protected IStatus run(IProgressMonitor monitor) {
        this.progressMonitor = monitor;

        progressMonitor.beginTask("Waiting...", (int) ((progressLength / 10.0)));
        try {
            // schedule task
            dumpAreaTask =
                equipment.createTask(new String[] {
                    CommandName.FLASH_DUMP_AREA.name(), equipment.getPort().getPortName(), dumpPath, start, length,
                    saveToPath, skipRedundant, includeBadBlocks});
            if (dumpAreaTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            dumpAreaTask.addTaskListener(this);
            EquipmentTaskResult result = dumpAreaTask.execute();

            if (result.getResultCode() == 0) {

                return new Status(IStatus.OK, Activator.PLUGIN_ID, formatDuration(result.getResultMessage(), result
                    .getDuration(), progressLength));
            } else {

                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, result.getResultMessage());
            }
        } catch (Exception e) {
            dumpAreaTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(final IEquipmentTask task, long totalBytes, long transferredBytes) {
        if (task == dumpAreaTask) {
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
        if (task == dumpAreaTask) {
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
