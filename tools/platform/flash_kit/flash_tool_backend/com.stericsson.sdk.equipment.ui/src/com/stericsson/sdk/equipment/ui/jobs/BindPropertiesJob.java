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

/**
 * @author xdancho
 * 
 */
public class BindPropertiesJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private IEquipmentTask bindPropertiesTask;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     */
    public BindPropertiesJob(IEquipment pEquipment) {
        super("BindProperties equipment: " + pEquipment.toString());
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
    public BindPropertiesJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("BindProperties equipment: " + pEquipment.toString());

        if (args != null && !args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
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
            bindPropertiesTask = equipment.createTask(new String[] {
                CommandName.SECURITY_BIND_PROPERTIES.name(), equipment.getPort().getPortName()});
            if (bindPropertiesTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            bindPropertiesTask.addTaskListener(this);
            EquipmentTaskResult result = bindPropertiesTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            bindPropertiesTask.cancel();
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
        progressMonitor.beginTask("BindProperties " + equipment.toString(), IProgressMonitor.UNKNOWN);
    }
}
