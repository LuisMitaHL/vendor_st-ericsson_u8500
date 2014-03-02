/**
 * 
 */
package com.stericsson.sdk.common.ui.wizards.pages;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.wizards.NewProfileWizard;

/**
 * @author xhelciz
 * @author Pawel Kapala
 */
public class NewProfileWizardSummaryPage extends WizardPage {

    private static final int BROWSER_WIDTH_HINT = 450;

    // Includes summary
    private Browser browser = null;

    // Color
    private String browserBkgColor;

    // Browser data style
    private String cssStyle;

    /**
     * @param pageName
     *            - name of summary wizard page
     */
    public NewProfileWizardSummaryPage(String pageName) {
        super(pageName);
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        setControl(createPageControl(pParent));
        setTitle(getName());
        setDescription("Check all settings and click finish.");
    }

    /**
     * Fills the summary page with the summary of created profile
     */
    public void updatePage() {
        browser.setText(getHTMLReport());
    }

    private Composite createPageControl(final Composite pParent) {
        int layoutColumns = 3;
        Composite c = new Composite(pParent, SWT.NONE);
        c.setLayout(new GridLayout(layoutColumns, false));

        // placeholder
        new Label(c, SWT.NONE).setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, layoutColumns, 1));

        browser = new Browser(c, SWT.NONE);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, false, true, layoutColumns, 1);
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

        // validator.checkCurrentValue();
        return c;
    }

    private String getHTMLReport() {
        StringBuilder sb =
            new StringBuilder("<html><body bgcolor=\"" + browserBkgColor + "\" style=\"" + cssStyle + "\">");

        String fileContent = ((NewProfileWizard) this.getWizard()).getFileContent().replace("<newLine>", "</br>");

        sb.append("<p>You are about to create profile file with following content:</p>");
        sb.append("<p>");
        sb.append(fileContent);
        sb.append("</p>");
        sb.append("</body></html>");

        return sb.toString();
    }

    /**
     * {@inheritDoc}
     */
    public void validatorMessage(IValidator src, String message) {
        if (getWizard().getContainer().getCurrentPage() != null) {
            getWizard().getContainer().updateButtons();
        }
    }
}
