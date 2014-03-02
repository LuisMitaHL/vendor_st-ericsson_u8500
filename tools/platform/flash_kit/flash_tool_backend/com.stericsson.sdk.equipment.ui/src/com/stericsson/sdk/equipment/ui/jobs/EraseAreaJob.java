package com.stericsson.sdk.equipment.ui.jobs;

import java.util.HashMap;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import com.stericsson.sdk.brp.CommandName;
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
public class EraseAreaJob extends Job implements IEquipmentTaskListener {

    private IEquipmentTask eraseAreaTask;

    IEquipment equipment;

    private String start;

    private String length;

    private String path;

    private IProgressMonitor progressMonitor;

    /**
     * 
     * @param pPath
     *            the path to erase
     * @param pStart
     *            the start of the erase
     * @param pLength
     *            the length of the erase
     * @param pEquipment
     *            the equipment to operate on
     */
    public EraseAreaJob(String pPath, String pStart, String pLength, IEquipment pEquipment) {
        super("Erasing : " + pPath + " " + pStart + " - " + pLength);

        path = pPath;
        start = pStart;
        length = pLength;
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
    public EraseAreaJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Erasing equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.AREA_PATH)) {
            path = args.get(JobsArgHashKeys.AREA_PATH);
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
        equipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    protected IStatus run(IProgressMonitor monitor) {
        this.progressMonitor = monitor;
        progressMonitor.beginTask("Waiting...", SWT.INDETERMINATE);
        try {
            // schedule task
            eraseAreaTask = equipment.createTask(new String[] {
                CommandName.FLASH_ERASE_AREA.name(), equipment.getPort().getPortName(), path, start, length});
            if (eraseAreaTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by " + equipment.toString());
            }
            eraseAreaTask.addTaskListener(this);
            EquipmentTaskResult result = eraseAreaTask.execute();
            progressMonitor.setTaskName("Erasing...");

            if (result.getResultCode() == 0) {
                return new Status(IStatus.OK, Activator.PLUGIN_ID, result.getResultMessage());
            } else {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, result.getResultMessage());
            }

        } catch (Exception e) {
            eraseAreaTask.cancel();
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

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {

    }

}
