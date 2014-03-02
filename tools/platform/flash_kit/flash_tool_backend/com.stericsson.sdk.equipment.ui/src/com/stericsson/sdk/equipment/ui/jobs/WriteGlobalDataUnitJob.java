package com.stericsson.sdk.equipment.ui.jobs;

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
 * @author xadazim
 * 
 */
public class WriteGlobalDataUnitJob extends Job implements IEquipmentTaskListener {

    private String globalDataAreaPath;

    private String unitId;

    private String data;

    private IEquipment targetEquipment;

    private IEquipmentTask writeGlobalDataUnitTask;

    private IProgressMonitor progressMonitor;

    /**
     * Constructor.
     * 
     * @param pEquipment
     *            Target equipment
     * @param pAreaPath
     *            Parameter storage area path
     * @param pUnitId
     *            Parameter unit id
     * @param pData
     *            Parameter containing data to be written; sting containing hex values
     */
    public WriteGlobalDataUnitJob(String pUnitId, IEquipment pEquipment, String pAreaPath, String pData) {
        super("Write Global Unit from: " + pEquipment.toString());
        targetEquipment = pEquipment;
        globalDataAreaPath = pAreaPath;
        unitId = pUnitId;
        data=pData;
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
    public WriteGlobalDataUnitJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Write Global Data Unit from: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.STORAGE)) {
            globalDataAreaPath = args.get(JobsArgHashKeys.STORAGE);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.STORAGE + "!");
        }

        if (args.containsKey(JobsArgHashKeys.UNIT_ID)) {
            unitId = args.get(JobsArgHashKeys.UNIT_ID);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.UNIT_ID + "!");
        }

        if (args.containsKey(JobsArgHashKeys.UNIT_DATA)) {
            data = args.get(JobsArgHashKeys.UNIT_DATA);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.UNIT_DATA + "!");
        }

        targetEquipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        progressMonitor = pMonitor;

        progressMonitor.beginTask("Waiting...", 100);
        writeGlobalDataUnitTask = null;

        try {
            writeGlobalDataUnitTask =
                targetEquipment.createTask(new String[] {
                    CommandName.PARAMETER_STORAGE_WRITE_GLOBAL_DATA_UNIT.name(),
                    targetEquipment.getPort().getPortName(), globalDataAreaPath, unitId, data});
            if (writeGlobalDataUnitTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + targetEquipment.toString());
            }
            writeGlobalDataUnitTask.addTaskListener(this);
            EquipmentTaskResult result = writeGlobalDataUnitTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                result.getResultMessage());
        } catch (Exception e) {
            writeGlobalDataUnitTask.cancel();
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
    public void taskProgress(IEquipmentTask pTask, long pTotalBytes, long pTransferredBytes) {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask pTask) {
        // TODO Auto-generated method stub

    }

}
