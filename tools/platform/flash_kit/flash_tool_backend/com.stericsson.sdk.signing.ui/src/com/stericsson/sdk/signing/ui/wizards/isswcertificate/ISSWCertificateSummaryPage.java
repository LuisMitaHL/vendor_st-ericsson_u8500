package com.stericsson.sdk.signing.ui.wizards.isswcertificate;

import java.io.File;
import java.util.Map.Entry;

import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.AbstractTFValidator;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.data.KeyData;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;

/**
 * @author xmicroh
 * 
 */
public class ISSWCertificateSummaryPage extends AbstractWizardPage implements IValidatorMessageListener {

    private static final String ISSW_TF_NAME = "Output ISSW certificate:";

    private static final int BROWSER_WIDTH_HINT = 450;

    private String pathToISSWCertificate;

    private IValidator validator;

    private Browser browser;

    private String cssStyle;

    private String browserBkgColor;

    /**
     * @param pageName
     *            Name for the title of this page.
     */
    public ISSWCertificateSummaryPage(String pageName) {
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

        UITextField issw = uiTFFactory.createFilePathTextField(c, SWT.BORDER, ISSW_TF_NAME, "", 0, null, new String[] {
            "*.bin"}, false);
        issw.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateSummaryPage_ISSWPath_Button");
        final Text isswPath = issw.getUIControl();
        isswPath.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateSummaryPage_ISSWPath_Text");

        GridData gd = (GridData) isswPath.getLayoutData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        validator = new AbstractTFValidator(ISSW_TF_NAME, isswPath) {
            boolean changeOK = false;

            public boolean isValueOk() {
                return changeOK;
            }

            @Override
            public void checkValue(String str) {
                changeOK = false;
                if ((str.trim().length() > 0) && (!str.endsWith(".bin"))) {
                    notifyValidatorListeneres("File extension should be .bin!");
                    return;
                }

                File outputFile = new File(str);
                File parentFile = outputFile.getParentFile();
                if (outputFile.exists() || (parentFile != null && parentFile.exists())) {
                    changeOK = true;
                    notifyValidatorListeneres(null);
                } else {
                    notifyValidatorListeneres(this.getValueName() + ": Cannot find given path '" + str + "'");
                }
            }
        };
        validator.switchOn();
        validator.addValidatorListener(this);
        isswPath.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                pathToISSWCertificate = isswPath.getText();
            }
        });

        // placeholder
        new Label(c, SWT.NONE).setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, layoutColumns, 1));

        browser = new Browser(c, SWT.NONE);
        gd = new GridData(SWT.FILL, SWT.FILL, false, true, layoutColumns, 1);
        gd.widthHint = BROWSER_WIDTH_HINT;
        browser.setLayoutData(gd);
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
        setDescription("Select where to store ISSW Certificate, check all settings and click finish..");
    }

    void updatePage() {
        browser.setText(getHTMLReport());
    }

    private String getHTMLReport() {
        StringBuilder sb =
            new StringBuilder("<html><body bgcolor=\"" + browserBkgColor + "\" style=\"" + cssStyle + "\">");

        ISSWCertificateWizard wizard = (ISSWCertificateWizard) getWizard();
        // Selected root key:
        sb.append("<b>Selected Root Key:</b>");
        sb.append("<ul>");
        sb.append("<li>" + wizard.generalSettigsPage.getRootKeyPath() + "</li>");
        sb.append("</ul>");

        // Selected customer keys:
        sb.append("<b>Selected Customer Keys:</b>");
        sb.append("<ul>");

        for (int keyIndex = 0; keyIndex < wizard.keySelectorResults.getNumberOfKeys(); keyIndex++) {
            sb.append("<li>" + ((KeyData)wizard.keySelectorResults.getKeyData().toArray()[keyIndex]).getPath() + "</li>");
        }
        sb.append("</ul>");
        StringBuilder swTypesSB = new StringBuilder();
        for (Entry<GenericSoftwareType, Integer> entries : wizard.keySelectorResults.getValues().entrySet()) {
            if (entries.getValue() == ISSWCertificateKeySelectionPage.KEY_NOT_SELECTED) {
                swTypesSB.append("<li>" + entries.getKey().getName().replaceAll("&&", "&") + "</li>");
            }
        }
        if (swTypesSB.length() > 0) {
            sb.append("<b>Software types which cannot be signed:</b>");
            sb.append("<ul>");
            sb.append(swTypesSB.toString());
            sb.append("</ul>");
        }

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

    String getISSWOutputFile() {
        return pathToISSWCertificate;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_issw_cert.html");
    }

}
