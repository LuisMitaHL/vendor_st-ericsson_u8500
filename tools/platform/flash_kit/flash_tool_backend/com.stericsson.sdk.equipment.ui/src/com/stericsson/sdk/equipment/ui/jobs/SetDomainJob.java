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
 * Set Domain job is used to schedule job on connected ME, which will set domain of this ME
 * 
 * @author xhelciz
 * 
 */
public class SetDomainJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private IProgressMonitor progressMonitor;

    private IEquipmentTask setDomainTask;

    private String domain;

    /**
     * Constructor
     * 
     * @param pEquipment
     *            the target equipment
     */
    /**
     * Constructor
     * 
     * @param pDomain
     *            domain to be set (0 - Service, 1 - Product, 2 - R&D, 3 - Factory)
     * @param pEquipment
     *            the target equipment
     */
    public SetDomainJob(int pDomain, IEquipment pEquipment) {
        super("Set domain on equipment: " + pEquipment.toString());
        equipment = pEquipment;
        domain = String.valueOf(pDomain);
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
    public SetDomainJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Set domain on equipment: " + pEquipment.toString());

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.DOMAIN)) {// TODO Find right key on code
            domain = args.get(JobsArgHashKeys.DOMAIN);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.DOMAIN + "!");
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
            setDomainTask = equipment.createTask(new String[] {
                CommandName.SECURITY_SET_DOMAIN.name(), equipment.getPort().getPortName(), domain});
            if (setDomainTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + equipment.toString());
            }
            setDomainTask.addTaskListener(this);
            EquipmentTaskResult result = setDomainTask.execute();

            return new Status(result.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID, result
                .getResultMessage());
        } catch (Exception e) {
            setDomainTask.cancel();
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
