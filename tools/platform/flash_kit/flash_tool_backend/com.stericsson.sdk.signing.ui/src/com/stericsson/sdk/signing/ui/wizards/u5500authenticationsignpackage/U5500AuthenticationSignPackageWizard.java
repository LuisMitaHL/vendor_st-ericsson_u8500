package com.stericsson.sdk.signing.ui.wizards.u5500authenticationsignpackage;

import java.io.File;
import java.io.IOException;

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
public class U5500AuthenticationSignPackageWizard extends Wizard implements INewWizard {

    U5500AuthenticationSignPackagePage page;

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
        } catch (IOException e) {
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
        setWindowTitle("New Authentication Sign Package");
        page = new U5500AuthenticationSignPackagePage();
        addPage(page);
    }

}
