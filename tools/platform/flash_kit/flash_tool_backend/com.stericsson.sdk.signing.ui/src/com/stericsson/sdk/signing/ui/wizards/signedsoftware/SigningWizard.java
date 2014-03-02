package com.stericsson.sdk.signing.ui.wizards.signedsoftware;

import java.io.File;

import org.apache.log4j.Logger;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;

/**
 * Signing Wizard
 * 
 * @author xhelciz
 * 
 */
public class SigningWizard extends Wizard implements INewWizard {

    private SigningWizardGeneralSettingsPage generalSettingsPage;

    private SigningWizardAdvancedOptionsPage advancedOptionsPage;

    /**
     * Logger for the use of the wizard
     */
    protected Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     */
    public SigningWizard() {
        setWindowTitle("Signed Software");
        setHelpAvailable(true);
        generalSettingsPage = new SigningWizardGeneralSettingsPage();
        advancedOptionsPage = new SigningWizardAdvancedOptionsPage();
    }

    /**
     * Constructor
     * 
     * @param payloadFile
     *            Payload file.
     * @param payloadTypeName
     *            Payload type name.
     * @param project
     *            Project of which payload file comes from
     */
    public SigningWizard(String payloadFile, String payloadTypeName, IProject project) {
        setWindowTitle("Signed Software");
        setHelpAvailable(true);
        generalSettingsPage = new SigningWizardGeneralSettingsPage(project, payloadFile, payloadTypeName);
        advancedOptionsPage = new SigningWizardAdvancedOptionsPage();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {

        final String outputFile = generalSettingsPage.getOutputFilePath();
        final File file = new File(outputFile);
        if (file.exists()) {
            boolean replace =
                MessageDialog.openQuestion(getShell(), "File exists", "The file " + outputFile
                    + " already exists.\nDo you want to replace it?");
            if (!replace) {
                return false;
            }
        }

        try {
            ISignerSettings settings = generalSettingsPage.getSigningSettingsFromPage();
            if (settings != null) {
                advancedOptionsPage.addAdvancedSigningSettings(settings);
            }

            SignJobsUtils.runSignJob(settings, generalSettingsPage.getPayloadFileProject());

        } catch (SignerException e) {
            logger.error("Failed to initialize settings for signing. " + e.getMessage());
        }

        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        if (generalSettingsPage.isPageComplete() && advancedOptionsPage.isPageComplete()) {
            return true;
        }
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        super.addPages();
        addPage(generalSettingsPage);
        addPage(advancedOptionsPage);
    }
}
