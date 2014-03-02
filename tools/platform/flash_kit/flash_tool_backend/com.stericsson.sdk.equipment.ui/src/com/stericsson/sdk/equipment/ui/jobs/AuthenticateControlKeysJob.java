/**
 * 
 */
package com.stericsson.sdk.equipment.ui.jobs;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.structures.ControlKeyCrate;

/**
 * Class used as eclipse job
 * 
 * @author mbodan01
 */
public class AuthenticateControlKeysJob extends Job implements IEquipmentTaskListener {

    private IEquipment equipment;

    private ControlKeyCrate keysCrate;

    private IEquipmentTask authenticateCertificatecontrolKeysTask;

    /**
     * Concrete constructor
     * 
     * @param pKeysCrate
     *            input control keys passed as crate of keys
     * @param pEquipment
     *            selected equipment
     */
    public AuthenticateControlKeysJob(ControlKeyCrate pKeysCrate, IEquipment pEquipment) {
        super("Authenticate with control keys");
        keysCrate = pKeysCrate;
        equipment = pEquipment;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        try {
            String port = equipment.getPort().getPortName();

            // schedule task
            authenticateCertificatecontrolKeysTask = equipment.createTask(new String[] {
                CommandName.SYSTEM_AUTHENTICATE_CONTROL_KEYS.name(), port, "0x10=" + keysCrate.toString()});
            if (authenticateCertificatecontrolKeysTask == null) {
                throw new ExecutionException("Command not supported by equipment connected to " + port);
            }
            authenticateCertificatecontrolKeysTask.addTaskListener(this);
            EquipmentTaskResult result = authenticateCertificatecontrolKeysTask.execute();

            if (result.getResultCode() == 0) {
                return new Status(IStatus.OK, Activator.PLUGIN_ID, result.getResultMessage());
            } else {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, result.getResultMessage());
            }

        } catch (Exception e) {
            authenticateCertificatecontrolKeysTask.cancel();
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            monitor.done();
        }
    }

    /**
     * @param task
     *            .
     * @param message
     *            .
     */
    public void taskMessage(IEquipmentTask task, String message) {
        // N/A
    }

    /**
     * @param task
     *            .
     * @param totalBytes
     *            .
     * @param transferredBytes
     *            .
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        // N/A
    }

    /**
     * @param task
     *            .
     */
    public void taskStart(IEquipmentTask task) {
        // N/A
    }
}
