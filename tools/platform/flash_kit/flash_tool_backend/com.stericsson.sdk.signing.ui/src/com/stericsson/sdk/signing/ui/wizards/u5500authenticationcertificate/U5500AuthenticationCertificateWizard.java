package com.stericsson.sdk.signing.ui.wizards.u5500authenticationcertificate;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Display;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.ui.wizards.AbstractSignWizard;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500AuthenticationCertificateWizard extends AbstractSignWizard {

    private final U5500AuthenticationCertificatePage authPage;

    private String outputFile;

    /**
     * Constructor
     */
    public U5500AuthenticationCertificateWizard() {
        super("Sign Certificate");
        authPage = new U5500AuthenticationCertificatePage();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        outputFile = authPage.getOutputFile();
        FileOutputStream fos = null;
        try {
            File file = new File(outputFile);
            if (file.exists()) {
                boolean replace =
                    MessageDialog.openQuestion(getShell(), "File exists", "The file " + outputFile
                        + " already exists.\nDo you want to replace it?");
                if (!replace) {
                    return false;
                }
            }
            fos = new FileOutputStream(outputFile);
            fos.write(authPage.getCertificate().getData());

            super.performFinish();
        } catch (IOException ioe) {
            MessageDialog.openError(getShell(), "Write error", "Failed to write authentication certificate:\n"
                + ioe.getMessage());
            return false;
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle("New Authentication Certificate");
        setHelpAvailable(true);
        addPage(authPage);
        super.addPages();
    }

    /**
     * 
     *{@inheritDoc}
     */
    public String getPayloadFilePath() {
        return authPage.getOutputFile();
    }

    /**
     * 
     *{@inheritDoc}
     */
    public GenericSoftwareType getPayloadType() {
        return GenericSoftwareType.AUTH_CERT;
    }

    /**
     * 
     * @return output file
     */
    public String getOutputFile() {
        return outputFile;
    }

    U5500AuthenticationCertificatePage getAuthenticationCertificatePage() {
        return authPage;
    }

    /**
     * @return Path to ISSW certificate or null if wizard was canceled.
     */
    public static String openWizard() {
        U5500AuthenticationCertificateWizard certWizard = new U5500AuthenticationCertificateWizard();
        WizardDialog dialog = new WizardDialog(Display.getDefault().getActiveShell(), certWizard);
        dialog.create();
        certWizard.getContainer().getShell().setSize(510, 800);

        int retVal = dialog.open();
        if (retVal == Window.OK) {
            return certWizard.getOutputFile();
        } else {
            return null;
        }
    }
}
