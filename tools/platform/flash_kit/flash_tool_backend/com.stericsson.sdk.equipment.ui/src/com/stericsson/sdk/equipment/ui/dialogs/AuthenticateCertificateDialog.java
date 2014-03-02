package com.stericsson.sdk.equipment.ui.dialogs;

import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.signing.ui.jobs.RefreshSignPackagesJob;
import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;

/**
 * @author vsykal01
 */
public class AuthenticateCertificateDialog extends InputDialog implements IPropertyChangeListener {

    private Combo signPackages;

    private Display display;

    private ProgressBar pb;

    private String selectedSignPackageAlias;

    private RefreshSignPackagesJob job;

    /**
     * @param parent
     *            .
     * @param equipment
     *            .
     */
    public AuthenticateCertificateDialog(Shell parent, IEquipment equipment) {
        super(parent, "Authenticate Certificate");
    }

    /**
     * @param parent
     *            .
     */
    @Override
    protected void createInputArea(Composite parent) {
        Composite dialogArea = new Composite(parent, SWT.NONE);

        GridData gData = new GridData(SWT.FILL, SWT.FILL, true, true);
        dialogArea.setLayout(new GridLayout(1, false));
        dialogArea.setLayoutData(gData);

        Composite certificateArea = new Composite(dialogArea, SWT.NONE);
        certificateArea.setLayout(new GridLayout(2, false));

        Label label = new Label(certificateArea, SWT.NONE);
        label.setText("Certificate:");

        signPackages = new Combo(certificateArea, SWT.DROP_DOWN);
        signPackages.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                selectedSignPackageAlias = signPackages.getText();
            }
        });

        pb = new ProgressBar(dialogArea, SWT.INDETERMINATE);
        pb.setLayoutData(new GridData(SWT.FILL, SWT.NONE, true, false));
        pb.setToolTipText("Loading sign packages.");
        parent.pack();
        refreshSignPackages();
    }

    /**
     * Refreshes sign package combo (method connected to Refresh button). Also assures that this is
     * run in GUI thread.
     */
    public void refreshSignPackages() {
        display = Display.getDefault();

        enableGui(false);
        Thread t = new Thread("Loading sign packages") {
            public void run() {
                job = SignedSoftwareUtils.createRefreshSignPackagesJob();
                final String[] items = SignedSoftwareUtils.getAuthenticateSignPackageAliases(getShell(), job);

                if (!display.isDisposed()) {

                    display.asyncExec(new Runnable() {
                        public void run() {
                            if (signPackages.isDisposed()) {
                                return;
                            }
                            if (items == null || items.length == 0) {
                                enableGui(false);
                            } else {
                                signPackages.setItems(items);
                                signPackages.select(0);
                                enableGui(true);
                            }
                        }
                    });

                }
            }
        };
        t.start();
    }

    private void enableGui(final boolean enable) {
        display.asyncExec(new Runnable() {
            public void run() {
                pb.setVisible(!enable);
                signPackages.setEnabled(enable);
                getButton(OK).setEnabled(enable);
            }
        });
    }

    /**
     * @param event
     *            .
     */
    @Override
    public void propertyChange(PropertyChangeEvent event) {

    }

    /** */
    @Override
    protected void setDefaultValues() {
    }

    /** */
    @Override
    protected void storeValues() {
    }

    /**
     * @return false temporary
     */
    @Override
    protected boolean validatedBeforeExit() {
        return true;
    }

    /**
     * @return .
     */
    public String getCertificate() {
        return selectedSignPackageAlias;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void buttonPressed(int pButtonId) {
        if (pButtonId == InputDialog.CANCEL) {
            job.cancel();
        }
        super.buttonPressed(pButtonId);
    }
}
