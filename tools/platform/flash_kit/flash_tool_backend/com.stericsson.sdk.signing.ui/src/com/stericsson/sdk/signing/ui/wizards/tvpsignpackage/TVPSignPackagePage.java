/**
 * 
 */
package com.stericsson.sdk.signing.ui.wizards.tvpsignpackage;

import java.io.IOException;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;

/**
 * @author kapalpaw
 * 
 */
public class TVPSignPackagePage extends AbstractWizardPage {

    private static final String[] X509_FILTER_EXTENSIONS = new String[] {
        "*.der"};

    private static final String[] OUTPUT_FILTER_EXTENSIONS = new String[] {
        "*.pkg"};

    private static final int TA_HEIGHT_HINT = 100;

    // Text field labels
    private static final String TF_NAME_ALIAS = "Alias:";

    private static final String TF_NAME_REVISION = "Revision:";

    private static final String TF_NAME_DESCRIPTION = "Description:";

    private static final String TF_NAME_X509 = "X.509 certificate:";

    private static final String TF_NAME_CREATED_BY = "Created by:";

    private static final String TF_NAME_CREATED_DATE = "Created:";

    private static final String TF_NAME_OUTPUT_FILE = "Output file:";

    private static final String DESCRIPTION = "Fill in required fields..";

    private final List<TVPSignPackageUtils.ValidatedControl> validatedControls =
        new ArrayList<TVPSignPackageUtils.ValidatedControl>();

    private Text alias;

    private Text revision;

    private Text description;

    private Text x509Certificate;

    private Text outputFile;

    /**
     * @param pPageName
     *            name of the page
     */
    public TVPSignPackagePage(String pPageName) {
        super(pPageName);
    }

    /**
     *{@inheritDoc}
     */
    public void createControl(Composite pParent) {
        setControl(createPageControl(pParent));
        setDescription(DESCRIPTION);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        // Iterate over validators, to see if everything is ok
        for (TVPSignPackageUtils.ValidatedControl vc : validatedControls) {
            if (!vc.validate()) {
                return false;
            }
        }
        return true;
    }

    /**
     *{@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_tvp_sign_package.html");
    }

    /* package */void savePackage() throws IOException {
        TVPSignPackageWriter.writeSignPackage(outputFile.getText(), alias.getText(), null, revision.getText(),
            description.getText(), x509Certificate.getText());
    }

    /* package */String getOutputFile() {
        return outputFile.getText();
    }

    private void addRegexpValidator(final Control pControl, final String pRegexp) {
        TVPSignPackageUtils.ValidatedControl validatedControl = new TVPSignPackageUtils.ValidatedControl(pControl);
        final TVPSignPackageUtils.RegexpTextValidator rtv = new TVPSignPackageUtils.RegexpTextValidator(pRegexp);
        validatedControl.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.add(validatedControl);
    }

    private void addPathValidator(final Control pControl, final boolean pFileMustExist) {
        TVPSignPackageUtils.ValidatedControl validatedControl = new TVPSignPackageUtils.ValidatedControl(pControl);
        final TVPSignPackageUtils.PathTextValidator rtv = new TVPSignPackageUtils.PathTextValidator(pFileMustExist);
        validatedControl.setControlValidator(rtv, getWizard().getContainer());
        validatedControls.add(validatedControl);
    }

    private Composite createPageControl(Composite pParent) {
        // Create main, root composite - page layout holder
        int layoutColumns = 3;
        final Composite parent = new Composite(pParent, SWT.NONE);
        parent.setLayout(new GridLayout(layoutColumns, false));

        final UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);

        alias = TVPSignPackageUtils.createTextField(uiTFFactory, parent, TF_NAME_ALIAS).getUIControl();
        addGDData(alias, "PackageAlias");
        addRegexpValidator(alias, "[a-zA-Z0-9_.\\\\w]+");

        revision = TVPSignPackageUtils.createTextField(uiTFFactory, parent, TF_NAME_REVISION).getUIControl();
        addGDData(revision, "PackageRevision");
        addRegexpValidator(revision, "[a-zA-Z0-9.]+");

        description =
            TVPSignPackageUtils.createTextFieldMulti(uiTFFactory, parent, TF_NAME_DESCRIPTION, TA_HEIGHT_HINT)
                .getUIControl();
        addGDData(description, "PackageDescription");
        addRegexpValidator(description, "[a-zA-Z0-9.]+");

        TVPSignPackageUtils.createPlaceHolder(parent, layoutColumns);

        final UITextField x509CertificateTextField =
            TVPSignPackageUtils
                .createFilePathTextField(uiTFFactory, parent, TF_NAME_X509, X509_FILTER_EXTENSIONS, true);
        x509Certificate = x509CertificateTextField.getUIControl();
        addGDData(x509CertificateTextField.getUIControl(), "X509CertificateTextField");
        addGDData(x509CertificateTextField.getTextFieldButton(), "X509CertificateTextFieldButton");
        addPathValidator(x509Certificate, true);

        TVPSignPackageUtils.createPlaceHolder(parent, layoutColumns);

        final Text created =
            TVPSignPackageUtils.createTextField(uiTFFactory, parent, TF_NAME_CREATED_DATE).getUIControl();
        addGDData(created, "PackageCreated");
        created.setText(DateFormat.getDateInstance().format(new Date()));
        created.setEditable(false);

        final Text createdBy =
            TVPSignPackageUtils.createTextField(uiTFFactory, parent, TF_NAME_CREATED_BY).getUIControl();
        addGDData(createdBy, "PackageCreatedBy");
        createdBy.setText(System.getProperty("user.name"));
        createdBy.setEditable(false);

        TVPSignPackageUtils.createPlaceHolder(parent, layoutColumns);

        final UITextField outputFileTextField =
            TVPSignPackageUtils.createFilePathTextField(uiTFFactory, parent, TF_NAME_OUTPUT_FILE,
                OUTPUT_FILTER_EXTENSIONS, false);
        outputFile = outputFileTextField.getUIControl();
        addPathValidator(outputFile, false);
        addGDData(outputFileTextField.getUIControl(), "PackageOutpuFileTextField");
        addGDData(outputFileTextField.getTextFieldButton(), "PackageOutpuFileTextFieldButton");

        // Validate inital input
        for (TVPSignPackageUtils.ValidatedControl vc : validatedControls) {
            vc.validate();
            getWizard().getContainer().updateButtons();
        }

        return parent;
    }

    private void addGDData(final Control pControl, final String pDescription) {
        TVPSignPackageUtils.addGDData(pControl, getClass(), pDescription);
    }

}
