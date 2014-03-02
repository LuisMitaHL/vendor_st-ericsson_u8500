package com.stericsson.sdk.signing.ui.wizards.tvp;

import java.io.File;

import org.apache.log4j.Logger;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.ui.jobs.GenerateKeyPairJob;

/**
 * Wizard for generating of private and public pair keys.
 * 
 * @author vsykal01
 * 
 */
public class TVPKeygenWizard extends Wizard implements INewWizard {

    private static final String WIZARD_NAME = "RSA Keys Generator Wizard";

    private TVPKeygenPage keygenPage = new TVPKeygenPage();

    Logger logger = Logger.getLogger(TVPKeygenWizard.class.getName());

    /**
     * Constructor
     */
    public TVPKeygenWizard() {
        super();
        setHelpAvailable(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle(WIZARD_NAME);
        addPage(keygenPage);
        super.addPages();
    }

    /**
     * {@inheritDoc}
     */
    public boolean performFinish() {
        File privF = keygenPage.getPrivateKeyFile();
        File pubF = keygenPage.getPublicKeyFile();
        if (privF.exists() && pubF.exists()) {
            boolean replace =
                MessageDialog.openQuestion(getShell(), "Files exist", "The files " + privF.getAbsolutePath() + ", "
                    + pubF.getAbsolutePath() + " already exist.\nDo you want to replace them?");
            if (!replace) {
                return false;
            }
        } else {
            String file = null;
            if (privF.exists()) {
                file = privF.getAbsolutePath();
            } else if (pubF.exists()) {
                file = pubF.getAbsolutePath();
            }
            if (file != null) {
                boolean replace =
                    MessageDialog.openQuestion(getShell(), "File exists", "The file " + file
                        + " already exists.\nDo you want to replace it?");
                if (!replace) {
                    return false;
                }
            }
        }

        GenerateKeyPairJob generate = new GenerateKeyPairJob(privF, pubF, keygenPage.getKeyLength());
        generate.setUser(true);
        generate.schedule();
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        return super.canFinish() && keygenPage.isPageComplete();
    }

    /**
     *{@inheritDoc}
     */
    public GenericSoftwareType getPayloadType() {
        return GenericSoftwareType.TVP_LICENSE;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        // TODO Auto-generated method stub
    }
}
