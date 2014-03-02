package com.stericsson.sdk.common.ui.wizards.pages;

import java.io.File;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.common.ui.wizards.SettingsKeys;

/**
 * Default data sheet page implementation. In this page types of output files are chosen and output
 * folder is chosen. This page is shown only when "Select all" platforms is chosen on the first page
 * of the wizard.
 */
public class AllPlatformsOutputPage extends AbstractWizardPage {

    private Text outputDirPathField;

    private Button gDVARCheckBox;

    private Button gDFCheckBox;

    private Button eEVARCheckBox;

    private Button eECheckBox;

    private Button excelCheckBox;

    private IDialogSettings wizardSettings;

    private Button generateReportButton;

    private class CheckButtonSelectionListener implements SelectionListener {
        private SettingsKeys createKey;

        private Button checkBox;

        public CheckButtonSelectionListener(Button pCheckBox, SettingsKeys pCreateKey) {
            checkBox = pCheckBox;
            createKey = pCreateKey;
        }

        public void widgetSelected(SelectionEvent pE) {
            wizardSettings.put(createKey.toString(), checkBox.getSelection());
            updatePageComplete();
        }

        public void widgetDefaultSelected(SelectionEvent pE) {
        }
    };

    /**
     * Constructs the page.
     * 
     * @param pWizardSettings
     *            wizardSettings used to store values set in this page
     */
    public AllPlatformsOutputPage(IDialogSettings pWizardSettings) {
        super("Create Output File");
        setDescription("Creates output files for all platforms.");
        wizardSettings = pWizardSettings;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/default_data_wizard/default_data_wizard.html");
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        Composite container = new Composite(pParent, SWT.NULL);
        final GridLayout gridLayout = new GridLayout();
        container.setLayout(gridLayout);
        setControl(container);

        Composite outputDirContainer = new Composite(container, SWT.NULL);
        GridLayout outputDirContainerLayout = new GridLayout(3, false);
        outputDirContainerLayout.horizontalSpacing = IUIConstants.WIZARDS_GRID_LAYOUT_HORIZONTAL_SPACING;
        outputDirContainer.setLayout(outputDirContainerLayout);
        outputDirContainer.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        UITextFieldsFactory uiTFFactory = UITextFieldsFactory.getInstance(null);
        UITextField tf =
            uiTFFactory.createDirPathTextField(outputDirContainer, SWT.BORDER, "Output folder:", "", 0, "*.*", false);
        outputDirPathField = tf.getUIControl();
        outputDirPathField.setText(wizardSettings.get(SettingsKeys.OUTPUT_DIR.toString()));
        outputDirPathField.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        outputDirPathField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                updatePageComplete();
                wizardSettings.put(SettingsKeys.OUTPUT_DIR.toString(), outputDirPathField.getText());
            }
        });

        // Output files
        Group group = new Group(container, SWT.NULL);
        group.setLayout(new GridLayout());
        group.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        group.setText("Output files");

        gDVARCheckBox = new Button(group, SWT.CHECK);
        gDVARCheckBox.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_GdvarFileCreate_Button");
        gDVARCheckBox.setText("Create GDVAR file");
        gDVARCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_GDVAR_FILE.toString()));
        gDVARCheckBox.addSelectionListener(new CheckButtonSelectionListener(gDVARCheckBox,
            SettingsKeys.CREATE_GDVAR_FILE));

        gDFCheckBox = new Button(group, SWT.CHECK);
        gDFCheckBox.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_GdfFileCreate_Button");
        gDFCheckBox.setText("Create GDF file");
        gDFCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_GDF_FILE.toString()));
        gDFCheckBox.addSelectionListener(new CheckButtonSelectionListener(gDFCheckBox, SettingsKeys.CREATE_GDF_FILE));

        eEVARCheckBox = new Button(group, SWT.CHECK);
        eEVARCheckBox.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_EevarFileCreate_Button");
        eEVARCheckBox.setText("Create EEVAR file");
        eEVARCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EEVAR_FILE.toString()));
        eEVARCheckBox.addSelectionListener(new CheckButtonSelectionListener(eEVARCheckBox,
            SettingsKeys.CREATE_EEVAR_FILE));

        eECheckBox = new Button(group, SWT.CHECK);
        eECheckBox.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_EeFileCreate_Button");
        eECheckBox.setText("Create EE file");
        eECheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EE_FILE.toString()));
        eECheckBox.addSelectionListener(new CheckButtonSelectionListener(eECheckBox, SettingsKeys.CREATE_EE_FILE));

        excelCheckBox = new Button(group, SWT.CHECK);
        excelCheckBox.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_ExcelFileCreate_Button");
        excelCheckBox.setText("Create product 19062 Excel file");
        excelCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EXCEL_FILE.toString()));
        excelCheckBox.addSelectionListener(new CheckButtonSelectionListener(excelCheckBox,
            SettingsKeys.CREATE_EXCEL_FILE));

        generateReportButton = new Button(container, SWT.CHECK);
        generateReportButton.setData(IUIConstants.GD_COMP_NAME, "AllPlatformsOutputPage_GenerateReport_Button");
        generateReportButton.setSelection(wizardSettings.getBoolean(SettingsKeys.GENERATE_REPORT.toString()));
        generateReportButton.setText("Show detailed report when finished");
        generateReportButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                wizardSettings.put(SettingsKeys.GENERATE_REPORT.toString(), generateReportButton.getSelection());
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        updatePageComplete();
    }

    /**
     * Checks if page is complete (and data are valid) and enables finish button if so.
     */
    protected void updatePageComplete() {
        boolean isValid = true;
        isValid &= validateOutputDir();

        boolean anyEnabled;
        anyEnabled = gDFCheckBox.getSelection() || gDVARCheckBox.getSelection();
        anyEnabled |= eEVARCheckBox.getSelection() || eECheckBox.getSelection() || excelCheckBox.getSelection();

        isValid &= anyEnabled;

        setPageComplete(isValid);
    }

    /**
     * Validates output directory. Output directory is valid if it exists.
     * 
     * @return true if output directory is valid
     */
    private boolean validateOutputDir() {
        boolean isValid = true;
        removeMessage(outputDirPathField);
        String path = outputDirPathField.getText();
        if (path != null && !path.trim().equals("")) {
            File file = new File(path);
            if (!file.isDirectory()) {
                isValid = false;
                addMessage(outputDirPathField, "Output folder must exist, invalid value: " + path, MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(outputDirPathField, "Output folder must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

}
