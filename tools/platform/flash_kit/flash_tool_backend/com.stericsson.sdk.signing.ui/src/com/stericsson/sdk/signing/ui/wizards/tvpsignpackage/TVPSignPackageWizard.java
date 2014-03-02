/**
 * 
 */
package com.stericsson.sdk.signing.ui.wizards.tvpsignpackage;

import java.io.File;
import java.io.IOException;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

/**
 * @author kapalpaw
 * 
 */
public class TVPSignPackageWizard extends Wizard implements INewWizard {

    private static final String WIZARD_NAME = "TVP Sign Package";

    private static final String GENERAL_SETTINGS_PAGE = "TVP Sign Package";

    private final TVPSignPackagePage signPackagePage;

    /**
     * Default constructor
     */
    public TVPSignPackageWizard() {
        signPackagePage = new TVPSignPackagePage(GENERAL_SETTINGS_PAGE);
    }

    /**
     *{@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        try {
            final String outputFile = signPackagePage.getOutputFile();
            final File file = new File(outputFile);
            if (file.exists()) {
                boolean replace =
                    MessageDialog.openQuestion(getShell(), "File exists", "The file " + outputFile
                        + " already exists.\nDo you want to replace it?");
                if (!replace) {
                    return false;
                }
            }
            signPackagePage.savePackage();
        } catch (IOException e) {
            MessageDialog.openError(getShell(), "Error when writing sign package",
                "An error occurred when writing the sign package to file:\n" + e.getMessage());
            return false;
        }
        return true;
    }

    /**
     *{@inheritDoc}
     */
    public void init(IWorkbench pWorkbench, IStructuredSelection pSelection) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle(WIZARD_NAME);
        addPage(signPackagePage);
    }

}
