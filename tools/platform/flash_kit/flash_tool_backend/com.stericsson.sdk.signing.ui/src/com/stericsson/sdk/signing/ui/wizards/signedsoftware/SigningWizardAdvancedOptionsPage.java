package com.stericsson.sdk.signing.ui.wizards.signedsoftware;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;

/**
 * Advanced Option Page of Signing Wizard
 * 
 * @author xhelciz
 * 
 */
public class SigningWizardAdvancedOptionsPage extends AbstractWizardPage {

    private static final String PAGE_NAME = "Advanced Options";

    private static final String PAGE_DESCRIPTION =
        "Specify advanced options if needed." + " Defined options will be applied during signing."
            + " Sign input selected on previous page by pressing the Finish button.";

    private static final String BUILD_VERSION_REGEX =
        "^([0-9]|([1-9][0-9])|(1[0-9]{1,2})|(2[0-4][0-9])|(25[0-5]))|(0(x|X)[0-9a-fA-F]{1,2})+";

    private static final String FLAGS_REGEX = "^([0-9]|([1-9][0-9]*))|(0(x|X)[0-9a-fA-F]{1,8})+";

    private Text loadAddressField;

    private Text startAddressField;

    private Text bufferSizeField;

    private Text softwareVersionField;

    private Text hashBlockSizeField;

    private Combo rootKeyHashTypeCombo;

    private Combo payloadHashTypeCombo;

    private Combo signatureHashTypeCombo;

    private Button pkaEnabledButton;

    private Button dmaEnabledButton;

    private Text majorBuildVersionField;

    private Text minorBuildVersionField;

    private Text flagsField;

    /**
     * Constructor
     */
    public SigningWizardAdvancedOptionsPage() {
        this(PAGE_NAME);
    }

    /**
     * Constructor
     * 
     * @param pPageName
     *            name of the page
     */
    public SigningWizardAdvancedOptionsPage(String pPageName) {
        super(PAGE_NAME);
        if (pPageName != null) {
            setTitle(pPageName);
        }
        setDescription(PAGE_DESCRIPTION);
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        Composite container = new Composite(pParent, SWT.NULL);
        final GridLayout gridLayout = new GridLayout(2, false);
        gridLayout.horizontalSpacing = 3;
        container.setLayout(gridLayout);

        ModifyListener mlistener = new ModifyListener() {
            public void modifyText(ModifyEvent pE) {
                updatePageComplete();
            }
        };

        final Label hashBlockSizeLabel = new Label(container, SWT.NONE);
        hashBlockSizeLabel.setText("Hash Block Size:");

        hashBlockSizeField = new Text(container, SWT.BORDER);
        GridData gDText = new GridData();
        gDText.horizontalAlignment = SWT.FILL;
        gDText.grabExcessHorizontalSpace = true;
        hashBlockSizeField.setLayoutData(gDText);
        hashBlockSizeField.addModifyListener(mlistener);

        final Label rootKeyHashTypeLabel = new Label(container, SWT.NONE);
        rootKeyHashTypeLabel.setText("Root Key Hash Type:");

        rootKeyHashTypeCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        rootKeyHashTypeCombo.setItems(new String[] {
            "SHA-1", "SHA-256", "SHA-384", "SHA-512"});
        rootKeyHashTypeCombo.setText("SHA-256");
        GridData gDrootKeyHashTypeCombo = new GridData();
        gDrootKeyHashTypeCombo.horizontalAlignment = SWT.FILL;
        gDrootKeyHashTypeCombo.grabExcessHorizontalSpace = true;
        rootKeyHashTypeCombo.setLayoutData(gDrootKeyHashTypeCombo);
        rootKeyHashTypeCombo.setEnabled(true);

        final Label payloadHashTypeLabel = new Label(container, SWT.NONE);
        payloadHashTypeLabel.setText("Payload Hash Type:");

        payloadHashTypeCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        payloadHashTypeCombo.setItems(new String[] {
            "SHA-1", "SHA-256", "SHA-384", "SHA-512"});
        payloadHashTypeCombo.setText("SHA-256");
        GridData gDpayloadHashTypeCombo = new GridData();
        gDpayloadHashTypeCombo.horizontalAlignment = SWT.FILL;
        gDpayloadHashTypeCombo.grabExcessHorizontalSpace = true;
        payloadHashTypeCombo.setLayoutData(gDpayloadHashTypeCombo);
        payloadHashTypeCombo.setEnabled(true);

        final Label signatureHashTypeLabel = new Label(container, SWT.NONE);
        signatureHashTypeLabel.setText("Signature Hash Type:");

        signatureHashTypeCombo = new Combo(container, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        signatureHashTypeCombo.setItems(new String[] {
            "SHA-1", "SHA-256", "SHA-384", "SHA-512"});
        signatureHashTypeCombo.setText("SHA-256");
        GridData gDsignatureHashTypeCombo = new GridData();
        gDsignatureHashTypeCombo.horizontalAlignment = SWT.FILL;
        gDsignatureHashTypeCombo.grabExcessHorizontalSpace = true;
        signatureHashTypeCombo.setLayoutData(gDsignatureHashTypeCombo);
        signatureHashTypeCombo.setEnabled(true);

        final Label loadAddressLabel = new Label(container, SWT.NONE);
        loadAddressLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        loadAddressLabel.setText("Load Address:");

        loadAddressField = new Text(container, SWT.BORDER);
        GridData gDText1 = new GridData();
        gDText1.horizontalAlignment = SWT.FILL;
        gDText1.grabExcessHorizontalSpace = true;
        loadAddressField.setLayoutData(gDText1);
        loadAddressField.addModifyListener(mlistener);

        final Label startAddressLabel = new Label(container, SWT.NONE);
        startAddressLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        startAddressLabel.setText("Start Address:");

        startAddressField = new Text(container, SWT.BORDER);
        GridData gDText2 = new GridData();
        gDText2.horizontalAlignment = SWT.FILL;
        gDText2.grabExcessHorizontalSpace = true;
        startAddressField.setLayoutData(gDText2);
        startAddressField.addModifyListener(mlistener);

        final Label bufferSizeLabel = new Label(container, SWT.NONE);
        bufferSizeLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        bufferSizeLabel.setText("Buffer size:");

        bufferSizeField = new Text(container, SWT.BORDER);
        GridData gDText3 = new GridData();
        gDText3.horizontalAlignment = SWT.FILL;
        gDText3.grabExcessHorizontalSpace = true;
        bufferSizeField.addModifyListener(mlistener);
        bufferSizeField.setLayoutData(gDText3);

        final Label softwareVersionLabel = new Label(container, SWT.NONE);
        softwareVersionLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        softwareVersionLabel.setText("Software version:");

        softwareVersionField = new Text(container, SWT.BORDER);
        softwareVersionField.addModifyListener(mlistener);
        softwareVersionField.setLayoutData(gDText3);

        final Label majorBuildVersionLabel = new Label(container, SWT.NONE);
        majorBuildVersionLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        majorBuildVersionLabel.setText("Major build version:");

        majorBuildVersionField = new Text(container, SWT.BORDER);
        majorBuildVersionField.addModifyListener(mlistener);
        majorBuildVersionField.setLayoutData(gDText3);

        final Label minorBuildVersionLabel = new Label(container, SWT.NONE);
        minorBuildVersionLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        minorBuildVersionLabel.setText("Minor build version:");

        minorBuildVersionField = new Text(container, SWT.BORDER);
        minorBuildVersionField.addModifyListener(mlistener);
        minorBuildVersionField.setLayoutData(gDText3);

        final Label flagsLabel = new Label(container, SWT.NONE);
        flagsLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        flagsLabel.setText("Flags:");

        flagsField = new Text(container, SWT.BORDER);
        flagsField.addModifyListener(mlistener);
        flagsField.setLayoutData(gDText3);

        final Label label1 = new Label(container, SWT.NONE);
        label1.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));

        pkaEnabledButton = new Button(container, SWT.CHECK);
        pkaEnabledButton.setText("PKA enabled");
        pkaEnabledButton.setEnabled(true);

        final Label label2 = new Label(container, SWT.NONE);
        label2.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));

        dmaEnabledButton = new Button(container, SWT.CHECK);
        dmaEnabledButton.setText("DMA enabled");
        dmaEnabledButton.setEnabled(true);

        setControl(container);
        updatePageComplete();
    }

    /**
     * @return PKA preference
     */
    public boolean getDMAPreference() {
        return dmaEnabledButton.getSelection();
    }

    /**
     * @return PKA preference
     */
    public boolean getPKAPreference() {
        return pkaEnabledButton.getSelection();
    }

    /**
     * @return root key hash type
     */
    public HashType getRootKeyHashType() {
        return SignJobsUtils.getHashType(rootKeyHashTypeCombo.getText());
    }

    /**
     * @return payload hash type
     */
    public HashType getPayloadHashType() {
        return SignJobsUtils.getHashType(payloadHashTypeCombo.getText());
    }

    /**
     * @return signature hash type
     */
    public HashType getSignatureHashType() {
        return SignJobsUtils.getHashType(signatureHashTypeCombo.getText());
    }

    /**
     * @return load address
     */
    public String getLoadAddress() {
        if (loadAddressField.getText().length() == 0) {
            return null;
        }
        return loadAddressField.getText();
    }

    /**
     * @return start address
     */
    public String getStartAddress() {
        if (startAddressField.getText().length() == 0) {
            return null;
        }
        return startAddressField.getText();
    }

    /**
     * @return buffer size
     */
    public String getBufferSize() {
        return bufferSizeField.getText();
    }

    /**
     * @return software version
     */
    public long getSoftwareVesion() {
        String text = softwareVersionField.getText();
        long swVersion = 0;
        if (text != null && !text.equals("")) {
            swVersion = Long.decode(text);
        }
        return swVersion;
    }

    /**
     * @return major build version
     */
    public short getMajorBuildVersion() {
        String text = majorBuildVersionField.getText();
        short majorBuildVersion = 0;
        if (text != null && !text.equals("")) {
            majorBuildVersion = Short.decode(text);
        }
        return majorBuildVersion;
    }

    /**
     * @return minor build version
     */
    public short getMinorBuildVersion() {
        String text = minorBuildVersionField.getText();
        short minorBuildVersion = 0;
        if (text != null && !text.equals("")) {
            minorBuildVersion = Short.decode(text);
        }
        return minorBuildVersion;
    }

    /**
     * @return flags
     */
    public long getFlags() {
        String text = flagsField.getText();
        long flags = 0;
        if (text != null && !text.equals("")) {
            flags = Long.decode(text);
        }
        return flags;
    }

    /**
     * @return hash block size
     */
    public String getHashBlockSize() {
        return hashBlockSizeField.getText();
    }

    private boolean validateHashBlockSizeField() {
        boolean isValid = true;
        String test = hashBlockSizeField.getText();
        removeMessage(hashBlockSizeField);

        if (test.length() == 0) {
            addMessage(hashBlockSizeField, "Flash archive hash block size in bytes.", MESSAGE_TYPE.INFO);
        } else if (!test.matches("^[0-9]+?")) {
            isValid = false;
            addMessage(hashBlockSizeField, "Invalid value: block size in bytes.", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(hashBlockSizeField);
        }
        return isValid;
    }

    private boolean validateBufferSizeField() {
        boolean isValid = true;
        String test = bufferSizeField.getText();
        removeMessage(bufferSizeField);

        if (test.length() == 0) {
            addMessage(bufferSizeField, "Buffer size in bytes. Valid suffixes are \"k\" and \"M\", e.g 128k or 1M ",
                MESSAGE_TYPE.INFO);
        } else if (!test.matches("^[0-9]+[kKmM]?")) {
            isValid = false;
            addMessage(bufferSizeField,
                "Invalid value: Buffer size in bytes. Valid suffixes are \"k\" and \"M\", e.g 128k or 1M",
                MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(bufferSizeField);
        }
        return isValid;
    }

    private boolean validateAddressField(Text textField) {
        boolean isValid = true;
        String test = textField.getText();
        removeMessage(textField);

        if (test.length() == 0) {
            addMessage(textField, "Please enter hexadecimal value", MESSAGE_TYPE.INFO);
        } else if (!test.matches("^0?x?[0-9A-F]+")) {
            isValid = false;
            addMessage(textField, "Invalid value: Please enter hexadecimal value", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(textField);
        }
        return isValid;
    }

    private boolean validateSoftwareVersionField() {

        boolean isValid = true;
        String test = softwareVersionField.getText();
        removeMessage(softwareVersionField);

        if (test.length() == 0) {
            addMessage(softwareVersionField, "Please enter decimal value.", MESSAGE_TYPE.INFO);
        } else if (!test.matches("^[0-9]+")) {
            isValid = false;
            addMessage(softwareVersionField, "Invalid value: Please enter decimal value.", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(softwareVersionField);
        }
        return isValid;
    }

    private boolean validateMajorBuildVersionField() {
        boolean isValid = true;
        String test = majorBuildVersionField.getText();
        removeMessage(majorBuildVersionField);

        short majorBuildVersion = 0;
        try {
            majorBuildVersion = getMajorBuildVersion();
        } catch (NumberFormatException e) {
            isValid = false;
        }

        if (test.length() == 0) {
            addMessage(
                majorBuildVersionField,
                "Please enter value in the range 0 to 255. Hexadecimal values must start with prefix 0x.",
                MESSAGE_TYPE.INFO);
        } else if (!isValid || !test.matches(BUILD_VERSION_REGEX) || majorBuildVersion < 0 || majorBuildVersion > 255) {
            isValid = false;
            addMessage(majorBuildVersionField, "Invalid value: Please enter value in the range 0 to 255."
                + " Hexadecimal values must start with prefix 0x.", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(majorBuildVersionField);
        }
        return isValid;
    }

    private boolean validateMinorBuildVersionField() {
        boolean isValid = true;
        String test = minorBuildVersionField.getText();
        removeMessage(minorBuildVersionField);

        short minorBuildVersion = 0;
        try {
            minorBuildVersion = getMinorBuildVersion();
        } catch (NumberFormatException e) {
            isValid = false;
        }

        if (test.length() == 0) {
            addMessage(
                minorBuildVersionField,
                "Please enter value in the range 0 to 255. Hexadecimal values must start with prefix 0x.",
                MESSAGE_TYPE.INFO);
        } else if (!isValid || !test.matches(BUILD_VERSION_REGEX) || minorBuildVersion < 0 || minorBuildVersion > 255) {
            isValid = false;
            addMessage(minorBuildVersionField, "Invalid value: Please enter value in the range 0 to 255."
                + " Hexadecimal values must start with prefix 0x.", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(minorBuildVersionField);
        }
        return isValid;
    }

    private boolean validateFlagsField() {
        boolean isValid = true;
        String test = flagsField.getText();
        removeMessage(flagsField);

        long flags = 0;
        try {
            flags = getFlags();
        } catch (NumberFormatException e) {
            isValid = false;
        }

        if (test.length() == 0) {
            addMessage(flagsField,
                "Please enter value in the range 0x0 to 0xFFFFFFFF. Hexadecimal values must start with prefix 0x.",
                MESSAGE_TYPE.INFO);
        } else if (!isValid || !test.matches(FLAGS_REGEX) || flags < 0 || flags > 0xFFFFFFFFL) {
            isValid = false;
            addMessage(flagsField, "Invalid value: Please enter value in the range 0x0 to 0xFFFFFFFF."
                + " Hexadecimal values must start with prefix 0x.", MESSAGE_TYPE.ERROR);
        } else {
            removeMessage(flagsField);
        }
        return isValid;
    }

    private void updatePageComplete() {
        boolean isValid = true;

        if (!validateBufferSizeField() || !validateAddressField(loadAddressField)
            || !validateAddressField(startAddressField) || !validateHashBlockSizeField()
            || !validateSoftwareVersionField() || !validateMajorBuildVersionField()
            || !validateMinorBuildVersionField() || !validateFlagsField()) {
            isValid = false;
        }
        setPageComplete(isValid);
    }

    /**
     * Adds settings from Advanced Settings Page to existing Signer settings
     * 
     * @param pSettings
     *            signer settings
     * @throws SignerSettingsException
     *             thrown if and error during setting up of signer settings occurs
     */
    public void addAdvancedSigningSettings(ISignerSettings pSettings) throws SignerSettingsException {
        pSettings.setSignerSetting(U5500SignerSettings.KEY_HASH_BLOCK_SIZE, getHashBlockSize());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_ROOT_KEY_HASH_TYPE, getRootKeyHashType());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_PAYLOAD_HASH_TYPE, getPayloadHashType());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_SIGNATURE_HASH_TYPE, getSignatureHashType());

        pSettings.setSignerSetting(U5500SignerSettings.KEY_LOAD_ADDRESS, getLoadAddress());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_START_ADDRESS, getStartAddress());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_BUFFER_SIZE, getBufferSize());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_PKA_FLAG, getPKAPreference());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_DMA_FLAG, getDMAPreference());
        pSettings.setSignerSetting(U5500SignerSettings.KEY_SW_VERSION, getSoftwareVesion());
        pSettings.setSignerSetting(GenericSignerSettings.KEY_MAJOR_BUILD_VERSION, getMajorBuildVersion());
        pSettings.setSignerSetting(GenericSignerSettings.KEY_MINOR_BUILD_VERSION, getMinorBuildVersion());
        pSettings.setSignerSetting(GenericSignerSettings.KEY_FLAGS, (int) getFlags());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/signed_software/how_to_sign_software.html");
    }
}
