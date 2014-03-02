package com.stericsson.sdk.signing.ui.wizards.isswcertificate;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.ui.IUIConstants;
import com.stericsson.sdk.common.ui.controls.UIListBox;
import com.stericsson.sdk.common.ui.controls.UIListBoxFactory;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.formatters.HexadecimalFormatter;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.PemKeyTFValidator;
import com.stericsson.sdk.common.ui.validators.RegexTFValidator;
import com.stericsson.sdk.common.ui.validators.ShortInputContainValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;
import com.stericsson.sdk.signing.data.ISSWCertificateType;
import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * @author xmicroh
 * 
 */
public class ISSWCertificateGeneralSettingsPage extends AbstractWizardPage implements IValidatorMessageListener {

    private static final String TF_NAME_ROOTKEY = "Root key:";

    private static final String COMBO_ROOTKEY_TYPE = "Root key type:";

    private static final String CHECKBOX_ISSW_CERT_TYPE = "Extended key type list";

    private static final String TF_NAME_RESERVED = "Reserved (hex):";

    private List<IValidator> validators;

    private Text rootKeyPath;

    private UIListBox modelId;

    private Combo rootKeyTypeCombo;

    private Button isswCertTypeCheckbox;

    private ISSWCertificateType type;

    private Text reserved;

    private RegexTFValidator reservedValidator;

    /**
     * @param pageName
     *            Name for the title of this page.
     */
    public ISSWCertificateGeneralSettingsPage(String pageName) {
        super(pageName);
        validators = new ArrayList<IValidator>();
        type = ISSWCertificateType.TYPE1;
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

        UITextField root =
            uiTFFactory.createFilePathTextField(c, SWT.BORDER, TF_NAME_ROOTKEY, "", 0, null, new String[] {
                "*.pem"}, true);
        rootKeyPath = root.getUIControl();
        rootKeyPath.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateGeneralSettingsPage_RookKeyPath_Text");
        root.getTextFieldButton().setData(IUIConstants.GD_COMP_NAME,
            "ISSWCertificateGeneralSettingsPage_RookKeyPath_Button");

        GridData rootKeyPathData = (GridData) rootKeyPath.getLayoutData();
        rootKeyPathData.horizontalAlignment = SWT.FILL;
        rootKeyPathData.grabExcessHorizontalSpace = true;

        validators.add(new PemKeyTFValidator(TF_NAME_ROOTKEY, rootKeyPath));

        Label rootKeyTypeLabel = new Label(c, NONE);
        rootKeyTypeLabel.setText(COMBO_ROOTKEY_TYPE);
        rootKeyTypeCombo = new Combo(c, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        rootKeyTypeCombo.setItems(new String[] {
            SignatureType.RSASSA_PKCS_V1_5.name(), SignatureType.RSASSA_PPS.name()});
        rootKeyTypeCombo.setText(SignatureType.RSASSA_PKCS_V1_5.name());
        GridData gDrootKeyTypeCombo = new GridData();
        gDrootKeyTypeCombo.horizontalAlignment = SWT.FILL;
        gDrootKeyTypeCombo.grabExcessHorizontalSpace = true;
        gDrootKeyTypeCombo.horizontalIndent = 5;

        rootKeyTypeCombo.setLayoutData(gDrootKeyTypeCombo);

        // placeholder
        GridData phData = new GridData(SWT.FILL, SWT.FILL, false, false);
        phData.horizontalSpan = layoutColumns;
        new Label(c, SWT.NONE).setLayoutData(phData);

        isswCertTypeCheckbox = new Button(c, SWT.CHECK);
        isswCertTypeCheckbox.setText(CHECKBOX_ISSW_CERT_TYPE);
        isswCertTypeCheckbox.setSelection(false);
        isswCertTypeCheckbox.addSelectionListener(new TypeSelectionListener());
        GridData gdISSWCertTypeCheckbox = new GridData();
        gdISSWCertTypeCheckbox.horizontalAlignment = SWT.FILL;
        gdISSWCertTypeCheckbox.grabExcessHorizontalSpace = true;
        gdISSWCertTypeCheckbox.horizontalSpan = 2;
        isswCertTypeCheckbox.setLayoutData(gdISSWCertTypeCheckbox);

        // placeholder
        phData = new GridData(SWT.FILL, SWT.FILL, false, false);
        phData.horizontalSpan = layoutColumns;
        new Label(c, SWT.NONE).setLayoutData(phData);

        Group group = new Group(c, SWT.TITLE);
        group.setLayout(new GridLayout(2, false));
        group.setText("Customer Part");
        GridData gd = new GridData(SWT.FILL, SWT.FILL, false, false);
        gd.horizontalSpan = layoutColumns;
        group.setLayoutData(gd);

        reserved = uiTFFactory.createTextField(group, SWT.BORDER, TF_NAME_RESERVED, " ", 0).getUIControl();
        reserved.setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateGeneralSettingsPage_Flags_Text");
        reserved.setEditable(false);
        gd = (GridData) reserved.getLayoutData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        reservedValidator =
            new RegexTFValidator(TF_NAME_RESERVED, reserved, "^([0-9a-fA-F]{2}){0,16}+",
                ": Hexadecimal representation of reserved bytes data (maximal length is 16 bytes)"
                    + " must have even length (e.g. 02A1B2).");
        reservedValidator.addValidatorListener(this);
        reservedValidator.switchOn();

        // placeholder
        phData = new GridData(SWT.FILL, SWT.FILL, false, false);
        phData.horizontalSpan = layoutColumns;
        new Label(group, SWT.NONE).setLayoutData(phData);
        ShortInputContainValidator inputValidator = new ShortInputContainValidator();
        HexadecimalFormatter inputFormatter = new HexadecimalFormatter();

        modelId =
            UIListBoxFactory.getInstance(null).createListBox(group, "Model ID", "Enter a Model ID(Hex):", SWT.BORDER,
                "Model ID:", layoutColumns, inputValidator, inputFormatter);
        modelId.getUIControl().setData(IUIConstants.GD_COMP_NAME, "ISSWCertificateGeneralSettingsPage_ModelId_Table");
        modelId.getAddButton().setData(IUIConstants.GD_COMP_NAME,
            "ISSWCertificateGeneralSettingsPage_ModelId_Button_Add");
        modelId.getRemoveButton().setData(IUIConstants.GD_COMP_NAME,
            "ISSWCertificateGeneralSettingsPage_ModelId_Button_Remove");
        inputValidator.setList(modelId.getListBoxItems());

        for (IValidator validator : validators) {
            validator.addValidatorListener(this);
            validator.switchOn();
            validator.checkCurrentValue();
        }

        setDescription("Select Root Key for signing ISSW and fill in required fields for customer part. "
            + "Continue by clicking next button to specify customer keys.");
    }

    /**
     * Get reserved.
     * 
     * @return The reserved as byte array.
     */
    protected byte[] getReserved() {
        String text;
        if (!HexUtilities.hasHexPrefix(reserved.getText().trim())) {
            text = reserved.getText();
        } else {
            text = reserved.getText().substring(2);
        }
        return HexUtilities.toByteArray(text);
    }

    List<Short> getModelId() {
        List<Short> modelList = new ArrayList<Short>();
        for (String id : modelId.getListBoxItems()) {
            if (HexUtilities.hasHexPrefix(id)) {
                modelList.add((short) Integer.parseInt(HexUtilities.removeHexPrefix(id), 16));
            } else {
                modelList.add((short) Integer.parseInt(id, 16));
            }
        }
        return modelList;
    }

    String getRootKeyPath() {
        return rootKeyPath.getText();
    }

    /**
     * @return type of root key
     */
    public SignatureType getRootKeyType() {
        return SignatureType.valueOf(rootKeyTypeCombo.getText());
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

    /**
     * {@inheritDoc}
     */
    @Override
    public IWizardPage getNextPage() {
        if (getType() == ISSWCertificateType.TYPE1) {
            return ((ISSWCertificateWizard) getWizard()).genericKeySelectorPage;
        }
        return ((ISSWCertificateWizard) getWizard()).extendedKeySelectionPage;
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
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/certificates_sign_packages/howto_create_issw_cert.html");
    }

    /**
     * 
     * @return Selected type
     */
    public ISSWCertificateType getType() {
        return type;
    }

    class TypeSelectionListener implements SelectionListener {

        public void widgetDefaultSelected(SelectionEvent e) {
            widgetSelected(e);
        }

        public void widgetSelected(SelectionEvent e) {
            if (isswCertTypeCheckbox.getSelection()) {
                type = ISSWCertificateType.TYPE2;
                extendedTypeSelected();
            } else {
                type = ISSWCertificateType.TYPE1;
                genericTypeSelected();
            }
        }

        private void genericTypeSelected() {
            reserved.setText("");
            reserved.setEditable(false);
            reservedValidator.checkCurrentValue();
            validators.remove(reservedValidator);
        }

        private void extendedTypeSelected() {
            reserved.setText("");
            reserved.setEditable(true);
            validators.add(reservedValidator);
            reservedValidator.checkCurrentValue();
        }

    }
}
