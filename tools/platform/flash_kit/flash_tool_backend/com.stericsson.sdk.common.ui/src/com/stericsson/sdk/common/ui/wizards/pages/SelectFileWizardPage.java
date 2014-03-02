package com.stericsson.sdk.common.ui.wizards.pages;

import java.io.File;
import java.util.Vector;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.wizards.SettingsKeys;
import com.stericsson.sdk.common.ui.wizards.logic.DefaultDataSheetProperties;
import com.stericsson.sdk.common.ui.wizards.logic.PlatformsInfo;

/**
 * Page of Default Data Sheet Wizard. Input file and platform is chosen on this page, and
 * consistency check log is displayed.
 */
public class SelectFileWizardPage extends WizardPage {

    private Text sourceFileField;

    private DefaultDataSheetProperties ddsProperties;

    private String dataFile;

    private Combo platformCombo;

    private Vector<PlatformsInfo> mPlatformsInfo;

    private Text logTextBox;

    private IDialogSettings wizardSettings;

    private Button allPlatformsButton;

    /**
     * @param pWizardSettings
     *            Wizard settings used to store values set in this page
     * @param pDdsp
     *            Logic processing the excel file
     */
    public SelectFileWizardPage(IDialogSettings pWizardSettings, DefaultDataSheetProperties pDdsp) {
        super("SelectFile");
        setTitle("Select input file");
        setDescription("Select the default data source file and select plaform");
        ddsProperties = pDdsp;
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
        final GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.horizontalSpacing = 3;
        container.setLayout(gridLayout);

        Group group = new Group(container, SWT.NULL);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 3, 1));
        final GridLayout fileGridLayout = new GridLayout(3, false);
        group.setLayout(fileGridLayout);
        group.setText("19062 Excel input file");

        final Label label = new Label(group, SWT.NONE);
        final GridData gridData = new GridData();
        gridData.horizontalSpan = 3;
        label.setLayoutData(gridData);

        final Label label1 = new Label(group, SWT.NONE);
        label1.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_END));
        label1.setText("File:");

        sourceFileField = new Text(group, SWT.BORDER);
        sourceFileField.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_InputFile_Text");
        sourceFileField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                wizardSettings.put(SettingsKeys.SOURCE_FILE.toString(), sourceFileField.getText());
                dataFile = sourceFileField.getText();
                if (validateDataFile() && readDocumentData()) {
                    allPlatformsButton.setEnabled(true);
                    platformCombo.setEnabled(!allPlatformsButton.getSelection());
                }
                updatePageComplete();
            }
        });
        sourceFileField.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        final Button button = new Button(group, SWT.NONE);
        button.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_InputFile_Button");
        button.setText("Browse...");

        // Platform
        Group platformGroup = new Group(container, SWT.NULL);
        platformGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 3, 3));
        final GridLayout chooserGridLayout = new GridLayout();
        chooserGridLayout.numColumns = 3;
        platformGroup.setLayout(chooserGridLayout);
        platformGroup.setText("Platform");

        // Platform label
        final Label platformLabel = new Label(platformGroup, SWT.NONE);
        final GridData platformGridData = new GridData();
        platformGridData.horizontalSpan = 1;
        platformLabel.setText("Choose:");
        platformLabel.setLayoutData(platformGridData);

        platformCombo = new Combo(platformGroup, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        platformCombo.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_Platform_Combo");
        GridData gDcombo1 = new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1);
        platformCombo.setLayoutData(gDcombo1);
        platformCombo.setEnabled(false);

        final Button consistencyCheckButton = new Button(platformGroup, SWT.PUSH);
        consistencyCheckButton.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_ConsistencyCheck_Button");
        consistencyCheckButton.setText("Consistency Check");
        consistencyCheckButton.setEnabled(false);
        new Label(platformGroup, SWT.NONE);

        // Select All Platforms button
        allPlatformsButton = new Button(platformGroup, SWT.CHECK);
        allPlatformsButton.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_AllPlatforms_Button");
        allPlatformsButton.setText("Select All Platforms");
        allPlatformsButton.setEnabled(false);
        allPlatformsButton.addSelectionListener(new SelectionListener() {

            public void widgetSelected(SelectionEvent pE) {
                if (allPlatformsButton.getSelection()) {
                    platformCombo.setEnabled(false);
                    consistencyCheckButton.setEnabled(true);
                } else {
                    platformCombo.setEnabled(true);
                    if (platformCombo.getSelectionIndex() == -1) {
                        consistencyCheckButton.setEnabled(false);
                    }
                }
                wizardSettings.put(SettingsKeys.ALL_PLATFORMS_SELECTED.toString(), allPlatformsButton.getSelection());
                updatePageComplete();
            }

            public void widgetDefaultSelected(SelectionEvent pE) {

            }
        });
        allPlatformsButton.setSelection(wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString()));
        allPlatformsButton.pack();
        new Label(platformGroup, SWT.NONE);

        Label logLabel = new Label(platformGroup, SWT.NONE);
        logLabel.setText("Consistency Check Log");
        GridData data = new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 3, 5);
        logLabel.setLayoutData(data);

        logTextBox = new Text(platformGroup, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
        logTextBox.setData(IUIConstants.GD_COMP_NAME, "SelectFileWizardPage_Log_Text");
        data = new GridData(SWT.FILL, SWT.FILL, true, true, 3, 1);
        logTextBox.setLayoutData(data);
        logTextBox.setEditable(false);

        logTextBox.addModifyListener(new ModifyListener() {

            public void modifyText(ModifyEvent pE) {
                logTextBox.setSelection(logTextBox.getText().length());
            }
        });

        button.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                dataFile = selectDataFile();
                if (dataFile != null && !dataFile.trim().equals("")) {
                    sourceFileField.setText(dataFile);
                }
                logTextBox.setText(ddsProperties.getLogTxt());
            }
        });

        consistencyCheckButton.addSelectionListener(new SelectionListener() {
            public void widgetSelected(SelectionEvent pE) {
                consistencyCheck();
                logTextBox.setText(ddsProperties.getLogTxt());
            }

            public void widgetDefaultSelected(SelectionEvent pE) {
            }
        });

        platformCombo.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                if (platformCombo.getSelectionIndex() != -1) {
                    consistencyCheckButton.setEnabled(true);

                    selectPlatform();
                    updatePageComplete();
                }
            }
        });

        setControl(container);
        setPageComplete(false);
    }

    /**
     * This method reads the Document Data (with platform selection, if necessary) and performs a
     * consistency check of the document.
     * 
     * @return True if this was done without errors
     */
    private boolean consistencyCheck() {
        boolean resultIsOk = true;

        // Read document
        if (resultIsOk) {
            resultIsOk = readDocumentData();
        }

        if (wizardSettings.getBoolean(SettingsKeys.ALL_PLATFORMS_SELECTED.toString())) {
            String[] platforms = platformCombo.getItems();

            for (int i = 0; i < platforms.length; i++) {
                // Platform Selection
                if (resultIsOk) {
                    ddsProperties.setPlatformToProcess(i, platforms[i]);
                }

                // Consistency check
                if (resultIsOk) {
                    resultIsOk = ddsProperties.consistencyCheckForSelectedPlatform();
                }
            }
        } else {
            // Platform Selection
            if (resultIsOk) {
                resultIsOk = selectPlatform();
            }

            // Consistency check
            if (resultIsOk) {
                resultIsOk = ddsProperties.consistencyCheckForSelectedPlatform();
            }
        }

        return true;
    }

    // Integrated with DefaultDataSheetProperties
    // private boolean consistencyCheckForSelectedPlatform() {
    // boolean resultIsOk = true;
    //
    // if (resultIsOk) {
    // resultIsOk = ddsProperties.readDefaultData();
    // // updateTotalProgressBar();
    // }
    //
    // if (resultIsOk) {
    // resultIsOk = ddsProperties.dataConsistensyCheck();
    // // updateTotalProgressBar();
    // }
    //
    // return resultIsOk;
    // }

    private boolean selectPlatform() {
        boolean correctPlatformSelected = false;

        // See if user already has selected a platform that is valid
        if (platformCombo.getItemCount() > 0) {
            String selectedPlatform = platformCombo.getText();

            if (selectedPlatform != null) {
                if (!selectedPlatform.equals(ddsProperties.getPlatformNameToProcess())) {
                    ddsProperties.setPlatformToProcess(platformCombo.getSelectionIndex(), selectedPlatform);
                }
                correctPlatformSelected = true;
            }
        }

        wizardSettings.put(SettingsKeys.CORRECT_PLATFORM_SELECTED.toString(), correctPlatformSelected);
        return correctPlatformSelected;
    }

    private boolean readDocumentData() {
        boolean result = false;

        int oldSelectedIndex = platformCombo.getSelectionIndex();
        platformCombo.removeAll();
        result = ddsProperties.readDocumentDataSheetProperties(dataFile);

        if (result) {
            String[] platforms = getAvailablePlatforms();
            for (String element : platforms) {
                platformCombo.add(new String(element));
            }
            if (oldSelectedIndex < platformCombo.getItemCount()) {
                if (oldSelectedIndex != -1) {
                    platformCombo.setText(platformCombo.getItem(oldSelectedIndex));
                }
            }
        }
        return result;
    }

    /**
     * @return A string array of available platforms read from the 19062 document
     */
    public String[] getAvailablePlatforms() {
        mPlatformsInfo = ddsProperties.getAvailablePlatforms();
        String[] result = null;
        result = new String[mPlatformsInfo.size()];
        for (int i = 0; i < mPlatformsInfo.size(); i++) {
            result[i] = mPlatformsInfo.elementAt(i).getName();
        }
        return result;
    }

    /**
     * @return datafile
     */
    protected String selectDataFile() {
        FileDialog dialog = new FileDialog(getShell(), SWT.OPEN);
        dialog.setText("Select *.xls data file");
        dialog.setFilterExtensions(new String[] {
            "*.xls", "*.*"});
        return dialog.open();
    }

    /**
     * Page complete?
     */
    protected void updatePageComplete() {
        boolean isValid = true;

        // Data file
        isValid =
            isValid && validateDataFile()
                && ((platformCombo.getSelectionIndex() != -1) || allPlatformsButton.getSelection());

        setPageComplete(isValid);
    }

    private boolean validateDataFile() {
        boolean isValid = true;
        String test = sourceFileField.getText();
        if (test == null || test.trim().equals("")) {
            isValid = false;
            setMessage("Data file must be specified", IStatus.ERROR);
        } else {
            File keyFile = new File(test);
            if (!keyFile.exists() || !keyFile.isFile()) {
                isValid = false;
                setMessage("Cannot find file: " + test, IStatus.ERROR);
            }
        }
        return isValid;
    }

    /**
     * @param pInitialSelection
     *            -
     */
    public void init(IStructuredSelection pInitialSelection) {

    }
}
