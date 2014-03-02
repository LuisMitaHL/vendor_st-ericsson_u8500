package com.stericsson.sdk.signing.ui.wizards.isswcertificate;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.signing.creators.ISSWCertificateCreator;
import com.stericsson.sdk.signing.data.ISSWCertificateData;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.data.KeyData;

/**
 * @author xmicroh
 * 
 */
public class ISSWCertificateWizard extends Wizard implements INewWizard {

    private static final String WIZARD_NAME = "New Initial Secure Software Certificate";

    static final String[] EXTENSIONS_FILTER = new String[] {
        "*.pem"};

    enum WizardPage {
        FIRST_PAGE("General Settings"), KEY_SELECTION("Customer Key Selector"), SUMMARY("Summary"), ;

        private String pageName;

        private WizardPage(String pPageName) {
            pageName = pPageName;
        }

        public String getPageName() {
            return pageName;
        }
    }

    ISSWCertificateGeneralSettingsPage generalSettigsPage;

    ISSWCertificateKeySelectionPage extendedKeySelectionPage;

    ISSWCertificateSummaryPage summaryPage;

    ISSWCertificateKeySelectionPage genericKeySelectorPage;

    ISSWCertificateKeySelectionPage keySelectorResults;

    /**
     * 
     */
    public ISSWCertificateWizard() {
        super();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void addPages() {
        setWindowTitle(WIZARD_NAME);
        for (WizardPage page : WizardPage.values()) {
            switch (page) {
                case FIRST_PAGE:
                    generalSettigsPage = new ISSWCertificateGeneralSettingsPage(page.pageName);
                    addPage(generalSettigsPage);
                    break;
                case KEY_SELECTION:
                    genericKeySelectorPage = new ISSWCertificateKeySelectionPage(page.pageName);
                    extendedKeySelectionPage = new ISSWCertificateExtendedKeySelectionPage(page.pageName);
                    addPage(genericKeySelectorPage);
                    addPage(extendedKeySelectionPage);
                    break;
                case SUMMARY:
                    summaryPage = new ISSWCertificateSummaryPage(page.pageName);
                    addPage(summaryPage);
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        if (!(keySelectionCompleted() && (otherPagesCompleted()))) {
            return false;
        }
        return true;
    }

    private boolean keySelectionCompleted() {
        return (extendedKeySelectionPage.isPageComplete() && generalSettigsPage.getType() == ISSWCertificateType.TYPE2)
            || (genericKeySelectorPage.isPageComplete() && generalSettigsPage.getType() == ISSWCertificateType.TYPE1);
    }

    private boolean otherPagesCompleted() {
        return summaryPage.isPageComplete() && generalSettigsPage.isPageComplete();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        try {
            ISSWCertificateData data = getCertificateValues();
            ISSWCertificateCreator creator = new ISSWCertificateCreator();
            creator.create(data);
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private ISSWCertificateData getCertificateValues() {
        ISSWCertificateData certificateData =
            new ISSWCertificateData(summaryPage.getISSWOutputFile(), generalSettigsPage.getType(), new KeyData(
                generalSettigsPage.getRootKeyPath(), generalSettigsPage.getRootKeyType()), generalSettigsPage
                .getModelId(), keySelectorResults.getKeyData(), keySelectorResults.getUsedSoftwareTypes());

        if (generalSettigsPage.getType() == ISSWCertificateType.TYPE2) {
            certificateData.setReserved(generalSettigsPage.getReserved());
        }

        return certificateData;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench pWorkbench, IStructuredSelection selection) {
    }

    /**
     * @return Path to ISSW certificate or null if wizard was canceled.
     */
    public static String openWizard() {
        ISSWCertificateWizard certWizard = new ISSWCertificateWizard();
        WizardDialog dialog = new WizardDialog(new Shell(), certWizard);
        dialog.create();
        certWizard.getContainer().getShell().setSize(510, 540);

        int retVal = dialog.open();
        if (retVal == Window.OK) {
            return certWizard.summaryPage.getISSWOutputFile();
        } else {
            return null;
        }
    }

    /**
     * Method updates key Results field
     */
    public void updateKeySelectionResults() {
        if (generalSettigsPage.getType() == ISSWCertificateType.TYPE2) {
            keySelectorResults = extendedKeySelectionPage;
        } else {
            keySelectorResults = genericKeySelectorPage;
        }
    }

}
