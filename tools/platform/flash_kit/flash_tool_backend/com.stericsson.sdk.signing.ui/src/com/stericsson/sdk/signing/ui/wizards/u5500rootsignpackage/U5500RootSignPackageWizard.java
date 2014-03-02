package com.stericsson.sdk.signing.ui.wizards.u5500rootsignpackage;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.signing.creators.RootSignPackageCreator;
import com.stericsson.sdk.signing.data.RootSignPackageData;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;

/**
 * @author xmicroh
 * 
 */
public class U5500RootSignPackageWizard extends Wizard implements INewWizard {

    private static final String WIZARD_NAME = "New Root Sign Package";

    U5500RootSignPkgPage rootSgnPkgPage;

    U5500RootSignPackageSummaryPage summaryPage;

    /**
     * 
     */
    public U5500RootSignPackageWizard() {
        super();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        for (IWizardPage page : getPages()) {
            if (!page.isPageComplete()) {
                return false;
            }
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        try {
            RootSignPackageData data =
                new RootSignPackageData(summaryPage.getSaveTo(), rootSgnPkgPage.getIsswPath(), rootSgnPkgPage
                    .getAlias(), rootSgnPkgPage.getRevision(), rootSgnPkgPage.getDesc());
            RootSignPackageCreator creator = new RootSignPackageCreator();
            creator.create(data);

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    GenericISSWCertificate getCertificate() throws Exception {
        GenericISSWCertificate isswCert = new GenericISSWCertificate();

        FileInputStream fis = null;

        try {
            fis = new FileInputStream(rootSgnPkgPage.getIsswPath());
            byte[] isswData = new byte[fis.available()];
            int read = fis.read(isswData);
            if (read < 0) {
                throw new IOException("Failed to read certificate");
            }
            // setting up root certificate
            isswCert.setData(isswData);
            return isswCert;
        } finally {
            if (fis != null) {
                fis.close();
            }
        }
    }

    String getRootPkgName(boolean parentName) {
        if (!parentName) {
            return summaryPage.getSaveTo() + File.separator + rootSgnPkgPage.getAlias() + "_ROOT.pkg";
        } else {
            return rootSgnPkgPage.getAlias();
        }
    }

    String getGenericSPkgName(int index, boolean aliasOnly) {
        if (!aliasOnly) {
            return summaryPage.getSaveTo() + File.separator + rootSgnPkgPage.getAlias() + "_GENERIC_" + index + ".pkg";
        } else {
            return rootSgnPkgPage.getAlias() + "_GENERIC_" + index;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench pWorkbench, IStructuredSelection selection) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle(WIZARD_NAME);
        rootSgnPkgPage = new U5500RootSignPkgPage("Root Sign Package");
        summaryPage = new U5500RootSignPackageSummaryPage("Summary");
        addPage(rootSgnPkgPage);
        addPage(summaryPage);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        IWizardPage p = super.getNextPage(page);
        if (super.getNextPage(page) == summaryPage) {
            summaryPage.updatePage();
        }
        return p;
    }
}
