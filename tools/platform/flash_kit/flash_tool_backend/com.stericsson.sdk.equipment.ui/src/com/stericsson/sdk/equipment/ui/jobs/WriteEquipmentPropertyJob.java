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
 * @author xdancho
 * 
 */
public class WriteEquipmentPropertyJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private IEquipmentTask writePropertyTask;

    private String propertyIdValue;

    /**
     * @param pEquipment
     *            tbd
     * @param idValue
     *            tbd
     */
    public WriteEquipmentPropertyJob(IEquipment pEquipment, String idValue) {
        super("WriteEquipmentProperty equipment: " + pEquipment.toString());
        equipment = pEquipment;
        propertyIdValue = idValue;
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
    public WriteEquipmentPropertyJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("WriteEquipmentProperty equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.ID)) {
            propertyIdValue = args.get(JobsArgHashKeys.ID);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.ID + "!");
        }

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
            writePropertyTask =
                equipment.createTask(new String[] {
                    CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), equipment.getPort().getPortName(),
                    propertyIdValue});
            if (writePropertyTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            writePropertyTask.addTaskListener(this);
            EquipmentTaskResult result = writePropertyTask.execute();
            progressMonitor.done();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            writePropertyTask.cancel();
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
        progressMonitor.beginTask("WriteEquipmentProperty " + equipment.toString(), IProgressMonitor.UNKNOWN);

    }

}
