package com.stericsson.sdk.signing.ui.wizards.signedsoftware;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.common.ui.validators.newdesign.AbstractCBValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u7x00.M7X00SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;
import com.stericsson.sdk.signing.l9540.L9540SignerSettings;
import com.stericsson.sdk.signing.ui.Activator;
import com.stericsson.sdk.signing.ui.jobs.RefreshSignPackagesJob;
import com.stericsson.sdk.signing.ui.utils.SignedSoftwareUtils;

/**
 * General Settings Page of Signing Wizard
 * 
 * @author xhelciz
 * 
 */
public class SigningWizardGeneralSettingsPage extends AbstractWizardPage {

    private Text inputFileField;

    private Button inputFileButton;

    private Button outputFileButton;

    private Button refreshButton;

    private Text outputFileField;

    private String inputFile;

    private String outputFile;

    private Combo softwareTypeCombo;

    private Combo targetPlatformCombo;

    private Combo signPackageCombo;

    private AbstractCBValidator signPackageValidator;

    private static final String PAGE_NAME = "General Settings";

    private static final String PAGE_DESCRIPTION =
        "Select software to be signed and specify its type if not recognized automatically."
            + " All fields are mandatory. Advanced options may be defined on next page.";

    private String initialPayloadFile;

    private String initialPayloadTypeName;

    private IProject initialProject;

    /**
     * Constructor
     */
    public SigningWizardGeneralSettingsPage() {
        this(PAGE_NAME);
    }

    /**
     * Constructor
     * 
     * @param pPageName
     *            name of the page
     */
    public SigningWizardGeneralSettingsPage(String pPageName) {
        super(PAGE_NAME);
        if (pPageName != null) {
            setTitle(pPageName);
        }
        setDescription(PAGE_DESCRIPTION);
    }

    /**
     * Constructor
     * 
     * @param pProject
     *            IProject pProject Project of which payload file comes from.
     * @param pPayloadFilePath
     *            Initial payload file
     * @param pPayloadTypeName
     *            Initial payload type
     */
    public SigningWizardGeneralSettingsPage(IProject pProject, String pPayloadFilePath, String pPayloadTypeName) {
        super(PAGE_NAME);
        initialPayloadFile = pPayloadFilePath;
        initialPayloadTypeName = pPayloadTypeName;
        initialProject = pProject;
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        Composite container = new Composite(pParent, SWT.NULL);
        final GridLayout gridLayout = new GridLayout(3, false);
        gridLayout.horizontalSpacing = 3;
        container.setLayout(gridLayout);

        final Label inputFilelabel = new Label(container, SWT.NONE);
        inputFilelabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        inputFilelabel.setText("Input File:");

        inputFileField = new Text(container, SWT.BORDER);
        inputFileField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                if (inputFileField.isEnabled()) {
                    inputFile = inputFileField.getText();
                    try {
                        if (detectElfFormat(inputFile)) {
                            // select *.ELF
                            String[] comboItems = softwareTypeCombo.getItems();// .select(0);
                            for (int index = 0; index < comboItems.length; index++) {
                                if (comboItems[index].equalsIgnoreCase(GenericSoftwareType.ELF.getName())) {
                                    softwareTypeCombo.select(index);
                                    break;
                                }
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    updatePageComplete();
                }
            }
        });
        inputFileField.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        inputFileButton = new Button(container, SWT.NONE);
        inputFileButton.setText("Browse...");
        inputFileButton.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                inputFile = selectDataFile(SWT.OPEN);
                if (inputFile != null && !inputFile.trim().equals("")) {
                    inputFileField.setText(inputFile);
                    String outputFilePath = SignedSoftwareUtils.getDefaultOutputPath(inputFile);
                    outputFileField.setText(outputFilePath);
                }
            }
        });

        Label softwareTypeLabel = new Label(container, SWT.NONE);
        softwareTypeLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        softwareTypeLabel.setText("Software type:");

        softwareTypeCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        softwareTypeCombo.setItems(SignedSoftwareUtils.getPayloadTypeItems());
        GridData gDsoftwareTypeCombo = new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1);
        softwareTypeCombo.setLayoutData(gDsoftwareTypeCombo);
        softwareTypeCombo.setEnabled(true);
        softwareTypeCombo.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                if (softwareTypeCombo.getText().equalsIgnoreCase(GenericSoftwareType.CRKC.getName())) {
                    String fileName = inputFileField.getText();
                    if (fileName != null && !fileName.trim().equals("")) {
                        outputFileField.setText(fileName + ".crkc");
                    }
                }
            }
        });

        Label placeHolderLabel = new Label(container, SWT.NONE);
        placeHolderLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_END));

        final Label outputFileLabel = new Label(container, SWT.NONE);
        outputFileLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        outputFileLabel.setText("Output File:");

        outputFileField = new Text(container, SWT.BORDER);
        outputFileField.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                outputFile = outputFileField.getText();
                updatePageComplete();
            }
        });
        outputFileField.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        outputFileButton = new Button(container, SWT.NONE);
        outputFileButton.setText("Browse...");
        outputFileButton.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                outputFile = selectDataFile(SWT.SAVE);
                if (outputFile != null && !outputFile.trim().equals("")) {
                    outputFileField.setText(outputFile);
                }
            }
        });

        final Label targetPlatformLabel = new Label(container, SWT.NONE);
        targetPlatformLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        targetPlatformLabel.setText("Target Platform:");

        targetPlatformCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        targetPlatformCombo.setItems(new String[] {
            "U5500", "U8500", "m7x00", "l9540"});
        targetPlatformCombo.select(0);
        GridData gDtargetPlatformCombo = new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1);
        targetPlatformCombo.setLayoutData(gDtargetPlatformCombo);
        targetPlatformCombo.setEnabled(true);

        final Label label2 = new Label(container, SWT.NONE);
        label2.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_END));

        final Label signPackageLabel = new Label(container, SWT.NONE);
        signPackageLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        signPackageLabel.setText("Sign Package:");

        signPackageCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        GridData gDsignPackageCombo = new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1);
        signPackageCombo.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event e) {
                updatePageComplete();
            }
        });
        signPackageCombo.setLayoutData(gDsignPackageCombo);
        signPackageCombo.setEnabled(true);
        signPackageValidator = new EmptyCBValidator(IStatus.ERROR, "Sign package", signPackageCombo);
        signPackageValidator.switchOn();
        signPackageValidator.checkCurrentValue();

        refreshButton = new Button(container, SWT.NONE);
        refreshButton.setText("Refresh");
        final GridData gButtonRefresh = new GridData(SWT.BEGINNING, SWT.CENTER, false, false);
        refreshButton.setLayoutData(gButtonRefresh);
        gButtonRefresh.horizontalAlignment = GridData.FILL;
        refreshButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                refreshSignPackages(true);
                super.widgetSelected(e);
            }
        });

        putInitValues();
        setControl(container);
        updatePageComplete();
        setData();
    }

    private void putInitValues() {
        if (initialPayloadFile != null) {
            setPayloadFilePath(initialPayloadFile, true);
        }
        if (initialPayloadTypeName != null) {
            setPayloadType(SignedSoftwareUtils.getSofwareType(initialPayloadTypeName), true);
        }
    }

    /**
     * Gets project of input file.
     * 
     * @return Project of which payload file comes from.
     */
    public IProject getPayloadFileProject() {
        return initialProject;
    }

    /**
     * Returns sign package name.
     * 
     * @return Sign package name.
     */
    public String getSignPackage() {
        return signPackageCombo.getText();
    }

    /**
     * Returns payload type.
     * 
     * @return Payload type.
     */
    public GenericSoftwareType getPayloadType() {
        return SignedSoftwareUtils.getSelectedElement(softwareTypeCombo);
    }

    /**
     * Sets the payload type.
     * 
     * @param pPayloadType
     *            Payload type.
     * @param pEnabled
     *            Enable or disable field after the value is set.
     */
    public void setPayloadType(final GenericSoftwareType pPayloadType, final boolean pEnabled) {
        // Make sure the changes are made on the GUI Thread
        Display display = softwareTypeCombo.getDisplay();
        if (!display.isDisposed()) {
            display.asyncExec(new Runnable() {
                public void run() {
                    if (softwareTypeCombo.isDisposed()) {
                        return;
                    }
                    softwareTypeCombo.select(SignedSoftwareUtils.getElementIndex(softwareTypeCombo.getItems(),
                        pPayloadType.getName()));
                    softwareTypeCombo.setEnabled(pEnabled);
                }
            });
        }
    }

    /**
     * Returns payload file path.
     * 
     * @return Payload file path.
     */
    public String getPayloadFilePath() {
        return inputFileField.getText();
    }

    /**
     * Sets the payload file path.
     * 
     * @param pPayloadFilePath
     *            Payload file path.
     * @param pEnabled
     *            Enable or disable field after the value is set.
     */
    public void setPayloadFilePath(final String pPayloadFilePath, final boolean pEnabled) {
        // Make sure the changes are made on the GUI Thread
        Display display = inputFileField.getDisplay();
        if (!display.isDisposed()) {
            display.asyncExec(new Runnable() {
                public void run() {
                    if (inputFileField.isDisposed() || inputFileButton.isDisposed()) {
                        return;
                    }
                    inputFileField.setEnabled(pEnabled);
                    inputFileField.setText(pPayloadFilePath);
                    inputFileButton.setEnabled(pEnabled);
                    String outputFilePath = SignedSoftwareUtils.getDefaultOutputPath(pPayloadFilePath);
                    outputFileField.setText(outputFilePath);
                }
            });
        }
    }

    /**
     * Returns output file path.
     * 
     * @return Output file path.
     */
    public String getOutputFilePath() {
        return outputFileField.getText();
    }

    private String selectDataFile(int mode) {
        FileDialog dialog = new FileDialog(getShell(), mode);
        dialog.setText("Select *.* data file");
        dialog.setFilterExtensions(new String[] {
            "*.*"});
        return dialog.open();
    }

    private boolean validateInputFile() {
        boolean isValid = true;
        String test = inputFileField.getText();
        if (inputFileField.isEnabled() && (test == null || test.trim().equals(""))) {
            isValid = false;
            // setMessage("Data file must be specified", IStatus.ERROR);
            addMessage(inputFileField, "Data file must be specified", MESSAGE_TYPE.ERROR);
        } else {
            File keyFile = new File(test);
            if (inputFileField.isEnabled() && (!keyFile.exists() || !keyFile.isFile())) {
                isValid = false;
                // setMessage("Cannot find file: " + test, IStatus.ERROR);
                addMessage(inputFileField, "Cannot find file", MESSAGE_TYPE.ERROR);
            } else {
                removeMessage(inputFileField);
            }
        }
        return isValid;
    }

    private void updatePageComplete() {
        boolean isValid = true;

        if (!validateInputFile() || !validateOutputFile()) {
            isValid = false;
        }
        isValid &= signPackageValidator.isValueOk();
        setPageComplete(isValid);
    }

    private boolean validateOutputFile() {
        boolean isValid = true;
        String test = outputFileField.getText();
        if (test == null || test.trim().equals("")) {
            isValid = false;
            // setMessage("Output file must be specified", IStatus.ERROR);
            addMessage(outputFileField, "Output file must be specified", MESSAGE_TYPE.ERROR);
        } else {
            File file = new File(test);
            File parentFile = file.getParentFile();
            if ((parentFile == null || !parentFile.isDirectory() || file.isDirectory())) {
                isValid = false;
                addMessage(outputFileField, "Invalid output file: " + test, MESSAGE_TYPE.ERROR);
            } else {
                removeMessage(outputFileField);
            }
        }
        return isValid;
    }

    private boolean detectElfFormat(String fileName) throws IOException {

        File file = new File(fileName);
        FileInputStream fin = new FileInputStream(file);
        // first four bytes contain the magic number which defines if it ELF file or not
        byte[] fileContent = new byte[4];
        fin.read(fileContent);

        if (0x7F == fileContent[0] && 0x45 == fileContent[1] && 0x4C == fileContent[2] && 0x46 == fileContent[3]) {
            return true;
        }
        return false;

    }

    /**
     * Called when refresh of sign packages is needed
     * 
     * @param pSavePreviousValue
     *            true if save of previous value is required
     */
    public void refreshSignPackages(final boolean pSavePreviousValue) {
        Display display = signPackageCombo.getDisplay();
        if (!display.isDisposed()) {
            Cursor waitCursor = display.getSystemCursor(SWT.CURSOR_WAIT);
            signPackageCombo.setCursor(waitCursor);

            display.asyncExec(new Runnable() {
                public void run() {
                    if (signPackageCombo.isDisposed()) {
                        return;
                    }

                    String previousValue = null;
                    if (pSavePreviousValue) {
                        previousValue = signPackageCombo.getText();
                    }

                    RefreshSignPackagesJob job = SignedSoftwareUtils.createRefreshSignPackagesJob();
                    String[] items = SignedSoftwareUtils.getAllSignPackageAliases(getShell(), job);
                    signPackageCombo.setItems(items);
                    if (items.length > 0) {
                        final boolean defaultPackageEnabled =
                            Activator.getDefault().getPreferenceStore().getBoolean(
                                FlashKitPreferenceConstants.SIGNING_USE_DEFAULT_SIGN_PACKAGE);
                        if (pSavePreviousValue) {
                            signPackageCombo.select(SignedSoftwareUtils.getElementIndex(items, previousValue));

                        } else if (defaultPackageEnabled) {
                            final String defaultPackage =
                                Activator.getDefault().getPreferenceStore().getString(
                                    FlashKitPreferenceConstants.SIGNING_DEFAULT_SIGN_PACKAGE);

                            signPackageCombo.select(SignedSoftwareUtils.getElementIndex(items, defaultPackage));

                        } else {
                            signPackageCombo.select(0);
                        }
                    }
                    updatePageComplete();
                }
            });

            signPackageCombo.setCursor(null);
        }
    }

    /**
     * Creates and fills signer settings with data contained on this page
     * 
     * @return signer settings
     * @throws SignerException
     *             thrown if and error during setting up of signer settings occurs
     */
    public ISignerSettings getSigningSettingsFromPage() throws SignerException {
        ISignerSettings settings = null;
        String target = targetPlatformCombo.getText();
        if (target.equalsIgnoreCase("u5500")) {
            settings = new U5500SignerSettings(getPayloadFilePath(), getOutputFilePath());
            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, getSignPackage());
            settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, getPayloadType());
        } else if (target.equalsIgnoreCase("u8500")) {
            settings = new U8500SignerSettings(getPayloadFilePath(), getOutputFilePath());
            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, getSignPackage());
            settings.setSignerSetting(U8500SignerSettings.KEY_SW_TYPE, getPayloadType());
        } else if (target.equalsIgnoreCase("l9540")) {
            settings = new L9540SignerSettings(getPayloadFilePath(), getOutputFilePath());
            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, getSignPackage());
            settings.setSignerSetting(L9540SignerSettings.KEY_SW_TYPE, getPayloadType());
        } else {
            settings = new M7X00SignerSettings(getPayloadFilePath(), getOutputFilePath());
            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, getSignPackage());
            settings.setSignerSetting(M7X00SignerSettings.KEY_SW_TYPE, getPayloadType());
        }

        return settings;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/signed_software/how_to_sign_software.html");
    }

    /**
     * Only for GUI Dancer.
     */
    private void setData() {
        setGuiData(inputFileField, "InputFile_Text");
        setGuiData(inputFileButton, "InputFile_Btn");
        setGuiData(outputFileField, "OutputFile_Text");
        setGuiData(outputFileButton, "OutputFile_Btn");
        setGuiData(refreshButton, "Refresh_Btn");
        setGuiData(softwareTypeCombo, "SoftwareType_cmb");
        setGuiData(targetPlatformCombo, "TargetPlatform_cmb");
        setGuiData(signPackageCombo, "SignPackage_cmb");
    }

    private void setGuiData(Control w, String data) {
        w.setData("GD_COMP_NAME", "SigningWizardGeneralSettingsPage" + data);
    }

    /**
     * @author Vit Sykala <vit.sykala@tieto.com>
     * 
     */
    private class EmptyCBValidator extends AbstractCBValidator {
        boolean valueOk = true;

        /**
         * @param severity
         * @param pValueName
         * @param pValidatedObject
         */
        public EmptyCBValidator(int severity, String pValueName, Combo pValidatedObject) {
            super(severity, pValueName, pValidatedObject);
        }

        public boolean isValueOk() {
            return valueOk;
        }

        @Override
        public void checkValue(String str) {
            setValueOk(!(str == null || str.trim().equals("")));
        }

        /**
         * @param pValueOk
         *            the valueOk to set
         */
        public void setValueOk(boolean pValueOk) {
            if (valueOk != pValueOk) {
                valueOk = pValueOk;
                if (valueOk) {
                    notifyValidatorListeneres(null);
                } else {
                    notifyValidatorListeneres(getValueName() + " is not correctly set.");
                }
            }
        }
    }
}
