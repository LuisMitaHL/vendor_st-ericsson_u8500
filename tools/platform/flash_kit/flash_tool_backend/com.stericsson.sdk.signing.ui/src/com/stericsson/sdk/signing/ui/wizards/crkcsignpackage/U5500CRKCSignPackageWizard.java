package com.stericsson.sdk.signing.ui.wizards.crkcsignpackage;

import java.io.File;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

/**
 * 
 * @author xolabju
 * 
 */
public class U5500CRKCSignPackageWizard extends Wizard implements INewWizard {

    U5500CRKCSignPackagePage page;

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        boolean isOk = true;
        try {
            String outputFile = page.getOutputFile();
            File file = new File(outputFile);
            if (file.exists()) {
                boolean replace =
                    MessageDialog.openQuestion(getShell(), "File exists", "The file " + outputFile
                        + " already exists.\nDo you want to replace it?");
                if (!replace) {
                    return false;
                }
            }
            page.savePackage();
        } catch (Exception e) {
            isOk = false;
            MessageDialog.openError(getShell(), "Error when writing sign package",
                "An error occurred when writing the sign package to file:\n" + e.getMessage());
        }
        return isOk;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        setHelpAvailable(true);
        setWindowTitle("New Customer Root Key Certificate Sign Package");
        page = new U5500CRKCSignPackagePage();
        addPage(page);
    }

}
