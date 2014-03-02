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
public class ReadEquipmentPropertyJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private String propertyId;

    private IEquipmentTask readPropertyTask;

    /**
     * @param pEquipment
     *            tbd
     * @param id
     *            tbd
     */
    public ReadEquipmentPropertyJob(IEquipment pEquipment, String id) {
        super("ReadEquipmentProperty equipment: " + pEquipment.toString());
        equipment = pEquipment;
        propertyId = id;
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
    public ReadEquipmentPropertyJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("ReadEquipmentProperty equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.ID)) {
            propertyId = args.get(JobsArgHashKeys.ID);
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
            // schedule task
            readPropertyTask =
                equipment.createTask(new String[] {
                    CommandName.SECURITY_GET_EQUIPMENT_PROPERTY.name(), equipment.getPort().getPortName(),
                    "" + propertyId});
            if (readPropertyTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            readPropertyTask.addTaskListener(this);
            EquipmentTaskResult result = readPropertyTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            readPropertyTask.cancel();
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
        progressMonitor.beginTask("ReadEquipmentProperty " + equipment.toString(), IProgressMonitor.UNKNOWN);

    }

}
