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
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.common.ui.wizards.SettingsKeys;

/**
 * Page of Default Data Sheet Wizard. This page is displayed only if "Select all platforms" is not
 * checked on the previous (SelectFileWizardPage) page, i.e. only one platform is selected. Files to
 * be created and their paths are set on this page.
 */
public class OnePlatformOutputPage extends AbstractWizardPage {

    private Button gDVARCheckBox;

    private Text gDVARPathField;

    private Button gDVARBrowseButton;

    private Button gDFCheckBox;

    private Text gDFPathField;

    private Button gDFBrowseButton;

    private Button gDFMergeCheckBox;

    private Text gDFMergePathField;

    private Button gDFMergeBrowseButton;

    private Text eEVARPathField;

    private Button eEVARCheckBox;

    private Button eEVARBrowseButton;

    private Button eECheckBox;

    private Button eEBrowseButton;

    private Text eEPathField;

    private Button excelCheckBox;

    private Button excelBrowseButton;

    private Text excelPathField;

    private IDialogSettings wizardSettings;

    private Button generateReportButton;

    private static final String[] GDVAR_EXTENSIONS = new String[] {
        "*.h", "*.*"};

    private static final String[] GDF_EXTENSIONS = new String[] {
        "*.gdf", "*.*"};

    private static final String[] EEVAR_EXTENSIONS = new String[] {
        "*.h", "*.*"};

    private static final String[] EE_EXTENSIONS = new String[] {
        "*.eef", "*.*"};

    private static final String[] EXCEL_EXTENSIONS = new String[] {
        "*.xls", "*.*"};

    private class OutputFileWidgets {
        Button checkBox;

        Text path;

        Button browse;

        OutputFileWidgets(Button pCheckBox, Text pPath, Button pBrowse) {
            checkBox = pCheckBox;
            path = pPath;
            browse = pBrowse;
        }
    }

    /**
     * Constructs a new instance of this class.
     * 
     * @param pWizardSettings
     *            wizard settings (files to create, paths and etc.) are stored in this object
     */
    public OnePlatformOutputPage(IDialogSettings pWizardSettings) {
        super("OutputFile");
        setTitle("Create Output File");
        setDescription("Creates the output files.");
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
        container.setLayout(new GridLayout());
        setControl(container);

        // Output files
        Group group = new Group(container, SWT.NULL);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        final GridLayout fileGridLayout = new GridLayout(3, false);
        // extend horizontal spacing so that "info" icons does not cover labels
        fileGridLayout.horizontalSpacing = IUIConstants.WIZARDS_GRID_LAYOUT_HORIZONTAL_SPACING;
        group.setLayout(fileGridLayout);
        group.setText("Output files");

        OutputFileWidgets widgets;
        widgets =
            addOutputFileWidgets(group, "Create GDVAR file", GDVAR_EXTENSIONS, SettingsKeys.GDVAR_FILE,
                SettingsKeys.CREATE_GDVAR_FILE, "Gdvar", true);
        gDVARPathField = widgets.path;
        gDVARCheckBox = widgets.checkBox;
        gDVARBrowseButton = widgets.browse;

        widgets =
            addOutputFileWidgets(group, "Create GDF file", GDF_EXTENSIONS, SettingsKeys.GDF_FILE,
                SettingsKeys.CREATE_GDF_FILE, "Gdf", true);
        gDFPathField = widgets.path;
        gDFCheckBox = widgets.checkBox;
        gDFBrowseButton = widgets.browse;

        widgets =
            addOutputFileWidgets(group, "Merge with input GDF file", GDF_EXTENSIONS, SettingsKeys.INPUT_GDF_FILE,
                SettingsKeys.MERGE_INPUT_GDF_FILE, "GdfMerge", false);
        gDFMergePathField = widgets.path;
        gDFMergeCheckBox = widgets.checkBox;
        gDFMergeBrowseButton = widgets.browse;

        gDFMergeCheckBox.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));

        widgets =
            addOutputFileWidgets(group, "Create EEVAR file", EEVAR_EXTENSIONS, SettingsKeys.EEVAR_FILE,
                SettingsKeys.CREATE_EEVAR_FILE, "Eevar", true);
        eEVARPathField = widgets.path;
        eEVARCheckBox = widgets.checkBox;
        eEVARBrowseButton = widgets.browse;

        widgets =
            addOutputFileWidgets(group, "Create EE file", EE_EXTENSIONS, SettingsKeys.EE_FILE,
                SettingsKeys.CREATE_EE_FILE, "Ee", true);
        eEPathField = widgets.path;
        eECheckBox = widgets.checkBox;
        eEBrowseButton = widgets.browse;

        widgets =
            addOutputFileWidgets(group, "Create product 19062 Excel file", EXCEL_EXTENSIONS, SettingsKeys.EXCEL_FILE,
                SettingsKeys.CREATE_EXCEL_FILE, "Excel", true);
        excelPathField = widgets.path;
        excelCheckBox = widgets.checkBox;
        excelBrowseButton = widgets.browse;

        gDFCheckBox.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                gDFMergePathField.setEnabled(gDFCheckBox.getSelection());
                gDFMergeCheckBox.setEnabled(gDFCheckBox.getSelection());
                gDFMergeBrowseButton.setEnabled(gDFCheckBox.getSelection());
                updatePageComplete();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        addReportWidgets(container);
    }

    private OutputFileWidgets addOutputFileWidgets(Composite pParent, final String pLabel, final String[] pFilter,
        final SettingsKeys pValueKey, final SettingsKeys pEnabledKey, String pGdNamePart, final boolean output) {

        final Button checkButton = new Button(pParent, SWT.CHECK);
        checkButton.setData(IUIConstants.GD_COMP_NAME, "OnePlatformOutputPage_" + pGdNamePart + "FileCreate_Button");
        checkButton.setText(pLabel);
        checkButton.setSelection(wizardSettings.getBoolean(pEnabledKey.toString()));

        final Text pathField = new Text(pParent, SWT.BORDER);
        pathField.setData(IUIConstants.GD_COMP_NAME, "OnePlatformOutputPage_" + pGdNamePart + "FilePath_Text");
        pathField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                wizardSettings.put(pValueKey.toString(), pathField.getText());
                updatePageComplete();
            }
        });
        pathField.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        final Button browseButton = new Button(pParent, SWT.NONE);
        browseButton.setData(IUIConstants.GD_COMP_NAME, "OnePlatformOutputPage_" + pGdNamePart + "FileBrowse_Button");
        browseButton.setText("Browse...");
        browseButton.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                String file = showFileDialog(pLabel, pFilter, output);
                if (file != null && !file.trim().equals("")) {
                    pathField.setText(file);
                }
            }
        });

        checkButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                wizardSettings.put(pEnabledKey.toString(), checkButton.getSelection());
                pathField.setEnabled(checkButton.getSelection());
                browseButton.setEnabled(checkButton.getSelection());
                updatePageComplete();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        return new OutputFileWidgets(checkButton, pathField, browseButton);
    }

    private void addReportWidgets(Composite pParent) {
        generateReportButton = new Button(pParent, SWT.CHECK);
        generateReportButton.setData(IUIConstants.GD_COMP_NAME, "OnePlatformOutputPage_GenerateReport_Button");
        generateReportButton.setSelection(wizardSettings.getBoolean(SettingsKeys.GENERATE_REPORT.toString()));
        wizardSettings.put("generateReport", generateReportButton.getSelection());
        generateReportButton.setText("Show detailed report when finished");
        generateReportButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                wizardSettings.put(SettingsKeys.GENERATE_REPORT.toString(), generateReportButton.getSelection());
            }

            public void widgetDefaultSelected(SelectionEvent pE) {

            }
        });
    }

    /**
     * Checks if output and input paths are valid. Called with each updatePageComplete.
     * 
     * @return true if all paths are valid
     */
    private boolean validateFilePaths() {
        boolean valid = true;

        if (gDVARCheckBox.getSelection()) {
            valid &= validateOutputFile(gDVARPathField);
        } else {
            removeMessage(gDVARPathField);
        }

        if (gDFCheckBox.getSelection()) {
            valid &= validateOutputFile(gDFPathField);

            if (gDFMergeCheckBox.getSelection()) {
                valid &= validateInputFile(gDFMergePathField);
            } else {
                removeMessage(gDFMergePathField);
            }
        } else {
            removeMessage(gDFPathField);
            removeMessage(gDFMergePathField);
        }

        if (eEVARCheckBox.getSelection()) {
            valid &= validateOutputFile(eEVARPathField);
        } else {
            removeMessage(eEVARPathField);
        }

        if (eECheckBox.getSelection()) {
            valid &= validateOutputFile(eEPathField);
        } else {
            removeMessage(eEPathField);
        }

        if (excelCheckBox.getSelection()) {
            valid &= validateOutputFile(excelPathField);
        } else {
            removeMessage(excelPathField);
        }

        return valid;
    }

    private boolean validateOutputFile(Text pathWidget) {
        boolean isValid = true;
        removeMessage(pathWidget);
        String path = pathWidget.getText();
        if (path != null && !path.trim().equals("")) {
            File file = new File(path);
            File parentFile = file.getParentFile();
            if (parentFile == null || !parentFile.isDirectory() || file.isDirectory()) {
                isValid = false;
                addMessage(pathWidget, "Invalid output file: " + path, MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(pathWidget, "Output file must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

    private boolean validateInputFile(Text pathWidget) {
        boolean isValid = true;
        removeMessage(pathWidget);
        String path = pathWidget.getText();
        if (path != null && !path.trim().equals("")) {
            File file = new File(path);
            if ((!file.exists() || file.isDirectory())) {
                isValid = false;
                addMessage(pathWidget, "Invalid input file: " + path, MESSAGE_TYPE.ERROR);
            }
        } else {
            isValid = false;
            addMessage(pathWidget, "Input file must be specified", MESSAGE_TYPE.INFO);
        }
        return isValid;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setVisible(boolean pVisible) {
        super.setVisible(pVisible);
        setDefaultFilePaths();
        fillDialog();
        updatePageComplete();
    }

    /**
     * Fills dialog with the same values as were set the last time the dialog was visible (or
     * default values if this is the first time the dialog is shown).
     */
    private void fillDialog() {
        gDVARPathField.setText(wizardSettings.get(SettingsKeys.GDVAR_FILE.toString()));
        gDFPathField.setText(wizardSettings.get(SettingsKeys.GDF_FILE.toString()));
        eEVARPathField.setText(wizardSettings.get(SettingsKeys.EEVAR_FILE.toString()));
        eEPathField.setText(wizardSettings.get(SettingsKeys.EE_FILE.toString()));
        excelPathField.setText(wizardSettings.get(SettingsKeys.EXCEL_FILE.toString()));

        gDVARCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_GDVAR_FILE.toString()));
        gDVARPathField.setEnabled(gDVARCheckBox.getSelection());
        gDVARBrowseButton.setEnabled(gDVARCheckBox.getSelection());

        gDFCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_GDF_FILE.toString()));
        gDFPathField.setEnabled(gDFCheckBox.getSelection());
        gDFBrowseButton.setEnabled(gDFCheckBox.getSelection());

        gDFMergeCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.MERGE_INPUT_GDF_FILE.toString()));
        gDFMergeCheckBox.setEnabled(gDFCheckBox.getSelection());
        gDFMergePathField.setEnabled(gDFCheckBox.getSelection() && gDFMergeCheckBox.getSelection());
        gDFMergeBrowseButton.setEnabled(gDFCheckBox.getSelection() && gDFMergeCheckBox.getSelection());

        eEVARCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EEVAR_FILE.toString()));
        eEVARPathField.setEnabled(eEVARCheckBox.getSelection());
        eEVARBrowseButton.setEnabled(eEVARCheckBox.getSelection());

        eECheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EE_FILE.toString()));
        eEPathField.setEnabled(eECheckBox.getSelection());
        eEBrowseButton.setEnabled(eECheckBox.getSelection());

        excelCheckBox.setSelection(wizardSettings.getBoolean(SettingsKeys.CREATE_EXCEL_FILE.toString()));
        excelPathField.setEnabled(excelCheckBox.getSelection());
        excelBrowseButton.setEnabled(excelCheckBox.getSelection());
    }

    /**
     * Checks if page is complete - checks if all mandatory fields are set and checks if all the
     * fields that are set contain correct values. If page is complete finish button will be
     * enabled.
     */
    protected void updatePageComplete() {
        boolean isValid = true;

        isValid &= validateFilePaths();

        boolean anyEnabled;
        anyEnabled = gDFCheckBox.getSelection() || gDVARCheckBox.getSelection();
        anyEnabled |= eEVARCheckBox.getSelection() || eECheckBox.getSelection() || excelCheckBox.getSelection();

        isValid &= anyEnabled;

        setPageComplete(isValid);
    }

    /**
     * @param pTitle
     *            Dialog title
     * @param filterExtensions
     *            filter extensions
     * @param output
     *            if true this is output file dialog, otherwise it is input file dialog
     * @return string with file path or null if the dialog was canceled
     */
    protected String showFileDialog(String pTitle, String[] filterExtensions, boolean output) {
        FileDialog dialog = new FileDialog(getShell(), (output ? SWT.SAVE : SWT.OPEN));
        dialog.setText(pTitle);
        dialog.setFilterExtensions(filterExtensions);
        return dialog.open();
    }

    private void setDefaultFilePaths() {
        File f = new File(wizardSettings.get(SettingsKeys.SOURCE_FILE.toString()));
        String parentPath = "";
        if (f.isFile()) {
            File parentFile = f.getParentFile();
            if (parentFile != null) {
                parentPath = parentFile.getAbsolutePath() + File.separator;
            }
            updateWizardSettingsForFile(parentPath);
        } else {
            updateWizardSettingsForNoFile();
        }
    }

    private void updateWizardSettingsForNoFile() {
        if (null == wizardSettings.get(SettingsKeys.GDVAR_FILE.toString())) {
            wizardSettings.put(SettingsKeys.GDVAR_FILE.toString(), "");
        }
        if (null == wizardSettings.get(SettingsKeys.GDF_FILE.toString())) {
            wizardSettings.put(SettingsKeys.GDF_FILE.toString(), "");
        }
        if (null == wizardSettings.get(SettingsKeys.EEVAR_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EEVAR_FILE.toString(), "");
        }
        if (null == wizardSettings.get(SettingsKeys.EE_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EE_FILE.toString(), "");
        }
        if (null == wizardSettings.get(SettingsKeys.EXCEL_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EXCEL_FILE.toString(), "");
        }
    }

    private void updateWizardSettingsForFile(String parentPath) {
        if (null == wizardSettings.get(SettingsKeys.GDVAR_FILE.toString())) {
            wizardSettings.put(SettingsKeys.GDVAR_FILE.toString(), parentPath + "gdvar.h");
        }
        if (null == wizardSettings.get(SettingsKeys.GDF_FILE.toString())) {
            wizardSettings.put(SettingsKeys.GDF_FILE.toString(), parentPath + "19062.gdf");
        }
        if (null == wizardSettings.get(SettingsKeys.EEVAR_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EEVAR_FILE.toString(), parentPath + "eevar.h");
        }
        if (null == wizardSettings.get(SettingsKeys.EE_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EE_FILE.toString(), parentPath + "19062.eef");
        }
        if (null == wizardSettings.get(SettingsKeys.EXCEL_FILE.toString())) {
            wizardSettings.put(SettingsKeys.EXCEL_FILE.toString(), parentPath + "19062_product.xls");
        }
    }
}
