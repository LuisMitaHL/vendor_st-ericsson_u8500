package com.stericsson.sdk.signing.ui.wizards.u5500rootsignpackage;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.data.SubpackageData;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;

/**
 * @author xmicroh
 * 
 */
public class U5500RootSignPackageSummaryPage extends AbstractWizardPage implements IValidatorMessageListener {

    private static final String TF_NAME_SAVE_TO = "Save to folder:";

    private static final int BROWSER_HEIGHT_HINT = 160;

    private Text saveToTF;

    private PathTFValidator validator;

    private Browser browser;

    private String browserBkgColor;

    private String cssStyle;

    private ArrayList<Text> spares = new ArrayList<Text>();

    private ScrolledComposite scrollCertTable;

    /**
     * @param pageName
     *            Name for the title of this page.
     */
    public U5500RootSignPackageSummaryPage(String pageName) {
        super(pageName);
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite parent) {
        int layoutColumns = 3;
        Composite c = new Composite(parent, SWT.NONE);
        c.setLayout(new GridLayout(layoutColumns, false));
        setControl(c);

        UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);

        // placeholder
        new Label(c, SWT.NONE).setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, layoutColumns, 1));

        saveToTF =
            uiTFFactory.createDirPathTextField(c, SWT.BORDER, TF_NAME_SAVE_TO, "", layoutColumns - 3, null, false)
                .getUIControl();
        saveToTF.setData(IUIConstants.GD_COMP_NAME, "U5500RootSignPackageSummaryPage_SaveToTF_Text");

        GridData gd = (GridData) saveToTF.getLayoutData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        validator = new PathTFValidator(TF_NAME_SAVE_TO, saveToTF, true);
        validator.switchOn();
        validator.addValidatorListener(this);

        browser = new Browser(c, SWT.NONE);
        gd = new GridData(SWT.FILL, SWT.CENTER, false, false, layoutColumns, 1);
        gd.heightHint = BROWSER_HEIGHT_HINT;
        browser.setLayoutData(gd);
        browser.setBackgroundMode(SWT.INHERIT_FORCE);

        FontData[] fontData = getShell().getDisplay().getSystemFont().getFontData();
        if ((fontData != null) && (fontData.length > 0)) {
            StringBuilder style = new StringBuilder();
            style.append("font-family: " + fontData[0].getName());
            style.append(";font-size: " + (fontData[0].getHeight() + 1) + "pt");
            style.append(";overflow:auto");
            cssStyle = style.toString();
            browserBkgColor =
                "#" + Integer.toHexString(c.getBackground().getRed())
                    + Integer.toHexString(c.getBackground().getGreen())
                    + Integer.toHexString(c.getBackground().getBlue());

        }

        validator.checkCurrentValue();

        setDescription("Select where to store sign package(s), check all settings and click finish..");
    }

    void updatePage() {
        U5500RootSignPackageWizard wizard = (U5500RootSignPackageWizard) getWizard();
        GenericISSWCertificate cert;
        try {
            cert = wizard.getCertificate();
        } catch (Exception e) {
            browser.setText("Attempt to read ISSW certificate failed!");
            return;
        }
        browser.setText(getHTMLReport(wizard, cert));
    }

    private String getHTMLReport(U5500RootSignPackageWizard wizard, GenericISSWCertificate cert) {
        StringBuilder sb =
            new StringBuilder("<html><body bgcolor=\"" + browserBkgColor + "\" style=\"" + cssStyle + "\">");

        sb.append("<b>*** Please store the packages and keys used in packages in same folder.</b><br/><br/>");

        sb.append("<b>Selected ISSW Certificate:</b>");

        sb.append("<ul>");
        sb.append("<li>" + wizard.rootSgnPkgPage.getIsswPath() + "</li>");
        sb.append("</ul>");

        sb.append("<p>You are about to create a root sign package. Please put the keys and packages in same folder.</p>");
        sb.append("</body></html>");

        return sb.toString();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        return validator.isValueOk();
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

    String getSaveTo() {
        return saveToTF.getText();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_root_pkg.html");
    }

    /**
     * @return Subpackage data collection.
     */
    public List<SubpackageData> getSubpackages() {
        List<SubpackageData> result = new ArrayList<SubpackageData>();
        for (int i = 0; i < spares.size(); i++) {
            result.add(new SubpackageData(i + 1, getSaveTo(), getSpare(i)));
        }
        return result;
    }

    /**
     * @param i
     *            Index of subpackage.
     * @return Subpackage spare value.
     */
    private int getSpare(int i) {
        return (int) (0xFFFFFFFF & Long.parseLong(spares.get(i).getText().toLowerCase(), 16));
    }
}
