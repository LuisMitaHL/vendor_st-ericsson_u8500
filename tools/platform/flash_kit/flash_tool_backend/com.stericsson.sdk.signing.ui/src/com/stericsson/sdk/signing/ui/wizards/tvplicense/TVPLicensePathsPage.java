package com.stericsson.sdk.signing.ui.wizards.tvplicense;

import java.util.ArrayList;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.UnrecognizedVisualTypeException;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.AbstractValidator;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.OutputPathTFValidator;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;

/**
 * Wizard page with files selection. Set sources XML license and certificate X.509 file and
 * destination unsigned TVP license file;
 * 
 * @author mielcluk
 * 
 */
public class TVPLicensePathsPage extends AbstractWizardPage implements IValidatorMessageListener {

    private static final String PAGE_DESCRIPTION =
        "Select XML license and X.509 certificate file. Choose output file destination for TVP unsigned license.";

    private static final int NUMBER_OF_COLUMN = 3;

    private static final String[] XML_FILE_EXTENSION = new String[] {
        "*.xml"};

    private static final String[] CERT_FILE_EXTENSION = new String[] {
        "*.der"};

    private static final String[] LICENSE_FILE_EXTENSION = new String[] {
        "*.bin"};

    private static final String XML_LICENSE = "XML license:";

    private static final String CERTIFICATE = "Owner's certificate (X.509):";

    private static final String TVPLICENSE_FILE = "Output file:";

    private Text licenseFile;

    private Text xmlFile;

    private Text certificateFile;

    private ArrayList<AbstractValidator> validators = new ArrayList<AbstractValidator>();

    Logger logger = Logger.getLogger(TVPLicensePathsPage.class.getName());

    /**
     * @param pPageName
     *            name of general settings wizard page
     * 
     */
    public TVPLicensePathsPage(String pPageName) {
        super(pPageName);
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        try {
            setControl(createPageControl(pParent));
            setTitle(getName());
            setDescription(PAGE_DESCRIPTION);
            for (AbstractValidator validator : validators) {
                validator.checkCurrentValue();
            }
        } catch (UnrecognizedVisualTypeException e) {
            logger.error(e.getMessage());
        }

    }

    /**
     * Create all the page controls
     * 
     * @param pParent
     *            parent composite
     * @param pSection
     *            xml ui configuration file section on which the page should be generated
     * @return page control (ready to be later set with setControl)
     * @throws UnrecognizedVisualTypeException
     */
    private Control createPageControl(Composite pParent) throws UnrecognizedVisualTypeException {
        // Create main, root composite - page layout holder
        final int layoutColumns = NUMBER_OF_COLUMN;
        final Composite parent = new Composite(pParent, SWT.NONE);
        parent.setLayout(new GridLayout(layoutColumns, false));

        // Get instance of text field factory
        final UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);
        UITextField xmlTF = createFilePathTextField(uiTFFactory, parent, XML_LICENSE, XML_FILE_EXTENSION, true);
        xmlFile = xmlTF.getUIControl();
        xmlFile.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "XMLLicenseText" + "_Text");
        ensureValidating(XML_LICENSE, xmlFile, false);
        xmlTF.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME,
            getClass().getSimpleName() + "XMLLicenseText" + "_Btn");

        UITextField certTF = createFilePathTextField(uiTFFactory, parent, CERTIFICATE, CERT_FILE_EXTENSION, true);
        certificateFile = certTF.getUIControl();
        certificateFile.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "CertificateText" + "_Text");
        ensureValidating(CERTIFICATE, certificateFile, false);
        certTF.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME,
            getClass().getSimpleName() + "CertificateText" + "_Btn");

        UITextField outputTF =
            createFilePathTextField(uiTFFactory, parent, TVPLICENSE_FILE, LICENSE_FILE_EXTENSION, false);
        licenseFile = outputTF.getUIControl();
        licenseFile.setData(IUIConstants.GD_COMP_NAME, getClass().getSimpleName() + "TVPLicenseText" + "_Text");
        ensureValidating(TVPLICENSE_FILE, licenseFile, true);
        outputTF.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME,
            getClass().getSimpleName() + "TVPLicenseText" + "_Btn");

        return parent;
    }

    private void ensureValidating(String name, Text pathField, boolean outputFile) {
        AbstractValidator pathValidator;
        if (!outputFile) {
            pathValidator = new PathTFValidator(name, pathField, false);
        } else {
            pathValidator = new OutputPathTFValidator(name, pathField);
        }

        pathValidator.switchOn();
        pathValidator.addValidatorListener(this);
        validators.add(pathValidator);

    }

    /**
     * Gets path of XML license file.
     * 
     * @return path to XML file
     */
    public String getXMLFile() {
        return xmlFile.getText();
    }

    /**
     * Gets path of certificate license file.
     * 
     * @return path to certificate file
     */
    public String getCertificateFile() {
        return certificateFile.getText();
    }

    /**
     * Gets path to unsigned TVP license file.
     * 
     * @return path to unsigned TVP license file
     */
    public String getTVPLicenseFile() {
        return licenseFile.getText();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_tvp_license.html");
    }

    // TODO: merge into UITextFieldsFactory?
    /**
     * Create File Path text field
     * 
     * @param uiTFFactory
     *            factory to be used for creating
     * @param parent
     *            parent
     * @param name
     *            Name of File Path text field
     * @param filterExtensions
     *            extensions to be filtered in opened dialog\
     * @param open
     *            True if input path, false if output path.
     * @return File Path text field
     */
    public static UITextField createFilePathTextField(final UITextFieldsFactory uiTFFactory, final Composite parent,
        final String name, final String[] filterExtensions, boolean open) {
        final UITextField uitf =
            uiTFFactory.createFilePathTextField(parent, SWT.BORDER, name, null, 0, null, filterExtensions, open);
        uitf.getControlLabel().setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        GridData gd = (GridData) uitf.getUIControl().getLayoutData();
        if (gd == null) {
            gd = new GridData();
            uitf.getUIControl().setLayoutData(gd);
        }
        // TODO: verify at least if file exists
        gd.horizontalAlignment = GridData.FILL;
        gd.grabExcessHorizontalSpace = true;
        return uitf;
    }

    /**
     * {@inheritDoc}
     */
    public void validatorMessage(IValidator src, String message) {
        if (src.isValueOk()) {
            removeMessage(src.getValidatedObject());
        } else {
            addMessage(src.getValidatedObject(), message, MESSAGE_TYPE.ERROR);
        }

        if (getWizard().getContainer().getCurrentPage() != null) {
            getWizard().getContainer().updateButtons();
        }

    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        for (IValidator validator : validators) {
            if (!validator.isValueOk()) {
                return false;
            }
        }
        return true;
    }

}
