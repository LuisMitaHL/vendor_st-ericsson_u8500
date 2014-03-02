/**
 * 
 */
package com.stericsson.sdk.equipment.ui.actions.system;

import org.apache.log4j.Logger;
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
import com.stericsson.sdk.equipment.ui.dialogs.AutenticateControlKeysDialog;
import com.stericsson.sdk.equipment.ui.jobs.AuthenticateControlKeysJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Authenticate with certificate control keys action create and schedule
 * AuthenticateCertificateControlKeysJob.
 * 
 * @author mbodan01
 */
public class AutenticateControlKeysAction extends BackendAction implements IJobChangeListener {
    private static final String AUTHENTICATE_BY_SPECIFIED_KEYS = "Authenticate by specified keys";

    private static final String AUTHENTICATE_CONTROL_KEYS = "Authenticate Control Keys";

    private static Logger logger = Logger.getLogger(AutenticateControlKeysAction.class.getName());

    private AuthenticateControlKeysJob authenticateCertificateJob;

    private boolean isDone = false;

    private IStatus result;

    /**
     * @param pViewer
     *            parent viewer
     */
    public AutenticateControlKeysAction(IBackendViewer pViewer) {
        super(pViewer);
        setText(AUTHENTICATE_CONTROL_KEYS + "...");
        setToolTipText(AUTHENTICATE_BY_SPECIFIED_KEYS);
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(
            Activator.EQUIPMENT_ICON_AUTHENTICATE));
    }

    /**
     * Main method of Action
     */
    @Override
    public void run() {
        IEquipment equipment = getViewer().getSelectedEquipment();
        if (equipment != null) {
            equipment.setStatus(EquipmentState.AUTHENTICATING);

            AutenticateControlKeysDialog dialog =
                new AutenticateControlKeysDialog(Display.getDefault().getActiveShell(), AUTHENTICATE_CONTROL_KEYS);
            if (dialog.open() != 0) {
                logger.debug("Dialogue CANCEL.");
            } else {
                logger.debug("Dialogue Ok.");
                logger.debug(dialog.getKeys());
                authenticateCertificateJob = new AuthenticateControlKeysJob(dialog.getKeys(), equipment);
                isDone = false;
                authenticateCertificateJob.addJobChangeListener(this);

                authenticateCertificateJob.schedule();
                while (!isDone) {
                    try {
                        synchronized (this) {
                            this.wait();
                        }
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
     * {@inheritDoc}
     */
    public void aboutToRun(IJobChangeEvent event) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void awake(IJobChangeEvent event) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void done(IJobChangeEvent event) {
        if (event.getJob() == authenticateCertificateJob) {
            result = event.getResult();
            isDone = true;
            synchronized (this) {
                this.notify();
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void running(IJobChangeEvent event) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void scheduled(IJobChangeEvent event) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void sleeping(IJobChangeEvent event) {
        // N/A
    }

}
