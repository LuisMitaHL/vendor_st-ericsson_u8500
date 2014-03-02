package com.stericsson.sdk.equipment.ui.jobs;

import java.io.File;
import java.util.HashMap;
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
 * @author xtomlju
 */
public class WriteGlobalDataSetJob extends Job implements IEquipmentTaskListener {

    private String globalDataFilename;

    private String globalDataAreaPath;

    private IEquipment targetEquipment;

    private IEquipmentTask writeGlobalDataTask;

    private IProgressMonitor progressMonitor;

    /**
     * Constructor.
     * 
     * @param pFilename
     *            Global data file
     * @param pEquipment
     *            Target equipment
     * @param pAreaPath
     *            Parameter storage area path
     */
    public WriteGlobalDataSetJob(String pFilename, IEquipment pEquipment, String pAreaPath) {
        super("Write Global Data to " + pEquipment.toString());
        targetEquipment = pEquipment;
        globalDataFilename = pFilename;
        globalDataAreaPath = pAreaPath;
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
    public WriteGlobalDataSetJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Write Global Data to " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.SOURCE_PATH)) {
            globalDataFilename = args.get(JobsArgHashKeys.SOURCE_PATH);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.SOURCE_PATH + "!");
        }

        if (args.containsKey(JobsArgHashKeys.STORAGE)) {
            globalDataAreaPath = args.get(JobsArgHashKeys.STORAGE);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.STORAGE + "!");
        }

        targetEquipment = pEquipment;
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
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        progressMonitor = monitor;

        File file = new File(globalDataFilename);
        if (!file.exists()) {
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "File " + globalDataFilename + " not found.");
        }
        progressMonitor.beginTask("Waiting...", 100);
        writeGlobalDataTask = null;

        try {
            writeGlobalDataTask =
                targetEquipment.createTask(new String[] {
                    CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_SET.name(),
                    targetEquipment.getPort().getPortName(), globalDataAreaPath, globalDataFilename});
            if (writeGlobalDataTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + targetEquipment.toString());
            }
            writeGlobalDataTask.addTaskListener(this);
            EquipmentTaskResult result = writeGlobalDataTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                result.getResultMessage());
        } catch (Exception e) {
            writeGlobalDataTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }
    }

}
