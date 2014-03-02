/**
 * 
 */
package com.stericsson.sdk.signing.ui.wizards;

import org.eclipse.jface.dialogs.IPageChangedListener;
import org.eclipse.jface.dialogs.PageChangedEvent;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.ui.wizards.signedsoftware.SignJobsUtils;
import com.stericsson.sdk.signing.ui.wizards.signedsoftware.SigningWizardGeneralSettingsPage;

/**
 * @author kapalpaw
 * 
 */
public abstract class AbstractSignWizard extends Wizard implements INewWizard {

    private final SigningWizardGeneralSettingsPage signPage;

    /**
     * Constructor.
     */
    public AbstractSignWizard() {
        signPage = new SigningWizardGeneralSettingsPage();
    }

    /**
     * Constructor.
     * 
     * @param signPageName Name of the signing page.
     */
    public AbstractSignWizard(String signPageName) {
        signPage = new SigningWizardGeneralSettingsPage(signPageName);
    }

    /**
     * Supplies the sign page with payload file path.
     * 
     * @return Payload file path.
     */
    public abstract String getPayloadFilePath();

    /**
     * Supplies the sign page with payload type.
     * 
     * @return Payload type.
     */
    public abstract GenericSoftwareType getPayloadType();

    /**
     *{@inheritDoc}
     */
    @Override
    public boolean performFinish() {
        // The last page was the sign page, so additionally signing needs to be done
        final IWizardPage page = getContainer().getCurrentPage();
        if (page != null && page instanceof SigningWizardGeneralSettingsPage) {
            final SigningWizardGeneralSettingsPage sp = (SigningWizardGeneralSettingsPage) page;
            SignJobsUtils.runSignJob(sp);
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    /**
     * 
     *{@inheritDoc}
     */
    @Override
    public void addPages() {
        addPage(signPage);
    }

    /**
     * 
     *{@inheritDoc}
     */
    @Override
    public void createPageControls(Composite pPageContainer) {

        WizardDialog wd = (WizardDialog) getContainer();
        if (wd != null) {
            wd.addPageChangedListener(new IPageChangedListener() {

                public void pageChanged(PageChangedEvent pEvent) {
                    if (getContainer() == null || getContainer().getCurrentPage() == null) {
                        return;
                    }
                    final IWizardPage page = getContainer().getCurrentPage();

                    // If the current page is signed software page, and we're in the wizard that has
                    // this page added as last page
                    if (page instanceof SigningWizardGeneralSettingsPage && page.getWizard() instanceof AbstractSignWizard) {
                        final AbstractSignWizard signWizard = (AbstractSignWizard) page.getWizard();
                        final SigningWizardGeneralSettingsPage sswp = (SigningWizardGeneralSettingsPage) page;
                        sswp.setPayloadFilePath(signWizard.getPayloadFilePath(), false);
                        sswp.setPayloadType(signWizard.getPayloadType(), false);
                        sswp.refreshSignPackages(true);
                    }

                }
            });
        }

        super.createPageControls(pPageContainer);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean canFinish() {
        for (IWizardPage page : getPages()) {
            // Exclude sign page if it is not current one
            if (page.equals(signPage) && !getContainer().getCurrentPage().equals(signPage)) {
                continue;
            }
            if (!page.isPageComplete()) {
                return false;
            }
        }
        return true;
    }

}
