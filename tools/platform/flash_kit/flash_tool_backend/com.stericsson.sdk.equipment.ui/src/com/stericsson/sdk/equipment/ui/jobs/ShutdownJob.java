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
 * @author zapletom
 * 
 */
public class ShutdownJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private IEquipmentTask shutdownTask;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     */
    public ShutdownJob(IEquipment pEquipment) {
        super("Shutdown equipment: " + pEquipment.toString());
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
    public ShutdownJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Shutdown equipment: " + pEquipment.toString());

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
            shutdownTask = equipment.createTask(new String[] {
                CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), equipment.getPort().getPortName()});
            if (shutdownTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            shutdownTask.addTaskListener(this);
            EquipmentTaskResult result = shutdownTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            shutdownTask.cancel();
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
        progressMonitor.beginTask("Shutdown " + equipment.toString(), IProgressMonitor.UNKNOWN);
    }
}
