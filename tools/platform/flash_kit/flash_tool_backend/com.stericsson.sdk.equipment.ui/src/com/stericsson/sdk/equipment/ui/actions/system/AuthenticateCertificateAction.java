package com.stericsson.sdk.equipment.ui.actions.system;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;

import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.dialogs.AuthenticateCertificateDialog;
import com.stericsson.sdk.equipment.ui.jobs.AuthenticateCertificateJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Authenticate with certificate action create and schedule AuthenticateCertificateJob.
 * 
 * @author vsykal01
 */
public class AuthenticateCertificateAction extends BackendAction implements IJobChangeListener {

    private static final String AUTHENTICATE_BY_SPECIFIED_CERTIFICATE = "Authenticate by specified certificate";

    private static final String AUTHENTICATE_CERTIFICATE = "Authenticate Certificate...";

    private AuthenticateCertificateJob authenticateCertificateJob;

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer.
     */
    public AuthenticateCertificateAction(IBackendViewer pViewer) {
        super(pViewer);
        setText(AUTHENTICATE_CERTIFICATE);
        setToolTipText(AUTHENTICATE_BY_SPECIFIED_CERTIFICATE);
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(
            Activator.EQUIPMENT_ICON_AUTHENTICATE_CERT));
    }

    /**
     * Create and schedule AuthenticateCertificateJob.
     */
    @Override
    public void run() {
        IEquipment equipment = getViewer().getSelectedEquipment();
        if (equipment != null) {
            equipment.setStatus(EquipmentState.AUTHENTICATING);
            AuthenticateCertificateDialog dialog =
                new AuthenticateCertificateDialog(Display.getDefault().getActiveShell(), equipment);
            if (dialog.open() == AuthenticateCertificateDialog.OK) {

                authenticateCertificateJob = new AuthenticateCertificateJob(dialog.getCertificate(), equipment);
                done = false;
                authenticateCertificateJob.addJobChangeListener(this);

                authenticateCertificateJob.schedule();
                while (!done) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                if ((result.getSeverity() & IStatus.ERROR) == 0) {
                    MessageBox message = new MessageBox(Display.getDefault().getActiveShell(), SWT.ICON_INFORMATION);
                    message.setText("Authenticate OK");
                    message.setMessage(result.getMessage());
                    message.open();
                }
            }
            equipment.setStatus(EquipmentState.IDLE);
        }
    }

    /**
     * @param event
     *            .
     */
    public void aboutToRun(IJobChangeEvent event) {
    }

    /**
     * @param event
     *            .
     */
    public void awake(IJobChangeEvent event) {
    }

    private boolean done = false;

    private IStatus result = null;

    /**
     * @param event
     *            .
     */
    public void done(IJobChangeEvent event) {
        if (event.getJob() == authenticateCertificateJob) {
            result = event.getResult();
            done = true;
        }
    }

    /**
     * @param event
     *            .
     */
    public void running(IJobChangeEvent event) {
    }

    /**
     * @param event
     *            .
     */
    public void scheduled(IJobChangeEvent event) {
    }

    /**
     * @param event
     *            .
     */
    public void sleeping(IJobChangeEvent event) {
    }

}
