/**
 * 
 */
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
 * @author xhelciz
 * 
 */
public class SetPropertiesJob extends Job implements IEquipmentTaskListener {

    private String propertyId;

    private String propertyValue;

    private IEquipment targetEquipment;

    private IEquipmentTask setPropertiesTask;

    private IProgressMonitor progressMonitor;

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
    public SetPropertiesJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Set properties");

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }
        if (args.containsKey(JobsArgHashKeys.UNIT)) {
            propertyId = args.get(JobsArgHashKeys.UNIT);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.UNIT + "!");
        }
        if (args.containsKey(JobsArgHashKeys.DATA)) {
            propertyValue = args.get(JobsArgHashKeys.DATA);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.DATA + "!");
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
        setPropertiesTask = null;
        String idDec;

        if (propertyId.startsWith("0x")) {
            idDec = propertyId.replace("0x", " ").trim();
            idDec = String.valueOf(Integer.parseInt(idDec, 16));
        } else {
            idDec = propertyId;
        }

        try {
            setPropertiesTask =
                targetEquipment.createTask(new String[] {
                    CommandName.SECURITY_SET_EQUIPMENT_PROPERTY.name(), targetEquipment.getPort().getPortName(),
                    idDec + "=" + propertyValue});
            if (setPropertiesTask == null) {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command not supported by "
                    + targetEquipment.toString());
            }
            setPropertiesTask.addTaskListener(this);
            EquipmentTaskResult taskResult = setPropertiesTask.execute();

            return new Status(taskResult.getResultCode() == 0 ? IStatus.OK : IStatus.ERROR, Activator.PLUGIN_ID,
                taskResult.getResultMessage());
        } catch (Exception e) {
            setPropertiesTask.cancel();
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
