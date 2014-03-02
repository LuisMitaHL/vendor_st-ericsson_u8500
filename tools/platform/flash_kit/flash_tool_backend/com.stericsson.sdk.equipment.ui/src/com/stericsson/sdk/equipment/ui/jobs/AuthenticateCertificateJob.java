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

/**
 * @author vsykal01
 */
public class AuthenticateCertificateJob extends Job implements IEquipmentTaskListener {

    private String certificate;

    private IEquipment equipment;

    private IEquipmentTask authenticateCertificateTask;

    /**
     * @param signCertificate
     *            .
     * @param iequipment
     *            .
     */
    public AuthenticateCertificateJob(String signCertificate, IEquipment iequipment) {
        super("Authenticate with certificate: " + signCertificate);
        this.certificate = signCertificate;
        this.equipment = iequipment;
    }

    /**
     * @param monitor
     *            .
     * @return .
     */
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        // monitor.beginTask("Authenticate by Certificate " + certificate, SWT.INDETERMINATE);
        try {
            String port = equipment.getPort().getPortName();

            // schedule task
            authenticateCertificateTask = equipment.createTask(new String[] {
                CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name(), port, certificate});
            if (authenticateCertificateTask == null) {
                throw new ExecutionException("Command not supported by equipment connected to " + port);
            }
            authenticateCertificateTask.addTaskListener(this);
            EquipmentTaskResult result = authenticateCertificateTask.execute();

            if (result.getResultCode() == 0) {
                return new Status(IStatus.OK, Activator.PLUGIN_ID, result.getResultMessage());
            } else {
                return new Status(IStatus.ERROR, Activator.PLUGIN_ID, result.getResultMessage());
            }

        } catch (Exception e) {
            authenticateCertificateTask.cancel();
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
    }

    /**
     * @param task
     *            .
     */
    public void taskStart(IEquipmentTask task) {
    }
}
