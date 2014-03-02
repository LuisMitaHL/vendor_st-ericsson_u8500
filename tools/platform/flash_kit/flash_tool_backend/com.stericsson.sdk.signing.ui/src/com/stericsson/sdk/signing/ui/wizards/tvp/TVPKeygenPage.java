package com.stericsson.sdk.signing.ui.wizards.tvp;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import org.apache.log4j.Logger;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.UnrecognizedVisualTypeException;
import com.stericsson.sdk.common.ui.controls.newdesign.UITextField;
import com.stericsson.sdk.common.ui.controls.newdesign.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.IValidatorMessageListener;
import com.stericsson.sdk.common.ui.validators.OutputPathTFValidator;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.validators.RegexTFValidator;
import com.stericsson.sdk.common.ui.wizards.AbstractWizardPage;

/**
 * Wizard page with output settings.
 * 
 * @author vsykal01
 * @author pkutac01
 * 
 */
public class TVPKeygenPage extends AbstractWizardPage implements Observer, IValidatorMessageListener {

    private static final String KEY_PREFIX = "";

    private static final String PRIV_POSTFIX = "_priv.pem";

    private static final String PUB_POSTFIX = "_pub.pem";

    private static final String NAME = "Output Settings";

    private static final String PAGE_DESCRIPTION =
        "This wizard is able to generate RSA key pair which can be used for signing.";

    Logger logger = Logger.getLogger(TVPKeygenPage.class.getName());

    private static final String TF_NAME_OUTPUT_DIRECTORY = "Output directory:";

    private static final String TF_NAME_KEY_PREFIX = "Key name (prefix):";

    private static final String TF_NAME_PRIVATE_KEY_LOCATION = "Private key:";

    private static final String TF_NAME_PUBLIC_KEY_LOCATION = "Public key:";

    private static final String CB_NAME_KEY_LENGTH = "Key length (bits):";

    private List<IValidator> validators;

    private UITextField path;

    private IValidator pathValidator;

    private UITextField name;

    private IValidator nameValidator;

    private UITextField privKey;

    private UITextField pubKey;

    private Button advanced;

    private Combo keyLength;

    private String[] keyLengthItems = new String[] {
        "1024", "2048", "4096"};

    private TVPKeygenPageModel model;

    /**
     * Constructor
     */
    public TVPKeygenPage() {
        super(NAME);
        validators = new ArrayList<IValidator>();
    }

    /**
     * {@inheritDoc}
     */
    public void createControl(Composite pParent) {
        try {
            setControl(createPageControl(pParent));
            setTitle(getName());
            setDescription(PAGE_DESCRIPTION);
            setModel(new TVPKeygenPageModel());
            initFunctions();
            setData();
        } catch (UnrecognizedVisualTypeException e) {
            logger.error(e.getMessage());
        }

    }

    /**
     * Create all the page controls
     * 
     * @param pParent
     *            parent composite
     * @return page control (ready to be later set with setControl)
     * @throws UnrecognizedVisualTypeException
     */
    private Control createPageControl(Composite pParent) throws UnrecognizedVisualTypeException {
        final Composite parent = new Composite(pParent, SWT.NONE);
        parent.setLayout(new GridLayout(3, false));
        setControl(parent);

        UITextFieldsFactory tfFactory = UITextFieldsFactory.getInstance(null);

        path = tfFactory.createDirPathTextField(parent, SWT.BORDER, TF_NAME_OUTPUT_DIRECTORY, "", 0, "", false);
        pathValidator = new PathTFValidator(TF_NAME_OUTPUT_DIRECTORY, path.getUIControl(), true);
        validators.add(pathValidator);
        name = tfFactory.createTextField(parent, SWT.BORDER, TF_NAME_KEY_PREFIX, "", 2);
        nameValidator =
            new RegexTFValidator(TF_NAME_KEY_PREFIX, name.getUIControl(), "[-a-zA-Z0-9_]+",
                " may contain only alphanumeric characters, dash and underscore");
        validators.add(nameValidator);

        advanced = new Button(parent, SWT.CHECK);
        advanced.setText("Specify keys locations separately:");
        GridData spanAllGD = new GridData();
        spanAllGD.horizontalSpan = 3;
        advanced.setLayoutData(spanAllGD);
        privKey =
            tfFactory.createFilePathTextField(parent, SWT.BORDER, TF_NAME_PRIVATE_KEY_LOCATION, "", 0, "",
                new String[] {
                    "*.pem", "*.*"}, false);
        validators.add(new OutputPathTFValidator(TF_NAME_PRIVATE_KEY_LOCATION, privKey.getUIControl()));
        pubKey =
            tfFactory.createFilePathTextField(parent, SWT.BORDER, TF_NAME_PUBLIC_KEY_LOCATION, "", 0, "", new String[] {
                "*.pem", "*.*"}, false);
        validators.add(new OutputPathTFValidator(TF_NAME_PUBLIC_KEY_LOCATION, pubKey.getUIControl()));

        Label separator = new Label(parent, SWT.NONE);
        GridData spanAllGD2 = new GridData();
        spanAllGD2.horizontalSpan = 3;
        separator.setLayoutData(spanAllGD2);

        Label keyLengthLabel = new Label(parent, SWT.NONE);
        keyLengthLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
        keyLengthLabel.setText(CB_NAME_KEY_LENGTH);
        keyLength = new Combo(parent, SWT.VERTICAL | SWT.DROP_DOWN | SWT.BORDER | SWT.READ_ONLY);
        keyLength.setItems(keyLengthItems);
        keyLength.select(1);

        for (IValidator validator : validators) {
            validator.addValidatorListener(this);
            validator.switchOn();
            validator.checkCurrentValue();
        }

        return parent;
    }

    /**
     * 
     */
    private void initFunctions() {
        path.getUIControl().addListener(SWT.Modify, new Listener() {
            public void handleEvent(Event event) {
                if (!model.isAdvanced()) {
                    String s = path.getUIControl().getText();
                    model.setPrivKeyPath(s);
                    model.setPubKeyPath(s);
                }
                model.notifyObservers();
            }
        });
        name.getUIControl().addListener(SWT.Modify, new Listener() {
            public void handleEvent(Event event) {
                if (!model.isAdvanced()) {
                    String s = name.getUIControl().getText();
                    model.setPrivKeyName(s + PRIV_POSTFIX);
                    model.setPubKeyName(s + PUB_POSTFIX);
                }
                model.notifyObservers();
            }
        });
        advanced.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                model.setAdvanced(advanced.getSelection());
                if (!advanced.getSelection()) {// changed to not advance
                    model.setPrivKeyPath(model.getPubKeyPath());
                    String privateName = model.getPrivKeyName();
                    String publicName = model.getPubKeyName();
                    String tmpName = KEY_PREFIX;
                    if (privateName.endsWith(PRIV_POSTFIX)) {
                        tmpName = privateName.substring(0, privateName.length() - PRIV_POSTFIX.length());
                    } else if (publicName.endsWith(PUB_POSTFIX)) {
                        tmpName = publicName.substring(0, publicName.length() - PUB_POSTFIX.length());
                    }
                    model.setPubKeyName(tmpName + PUB_POSTFIX);
                    model.setPrivKeyName(tmpName + PRIV_POSTFIX);
                }
                model.notifyObservers();
            }
        });
        keyLength.addListener(SWT.Selection, new Listener() {
            public void handleEvent(Event event) {
                int index = keyLength.getSelectionIndex();
                if (index >= 0 && index < keyLength.getItemCount()) {
                    try {
                        model.setKeyLength(Integer.parseInt(keyLength.getItem(index)));
                    } catch (Exception e) {
                        model.setKeyLength(2048);
                    }
                }
            }
        });
    }

    /**
     * Returns file to which public key should be created.
     * 
     * @return File to which public key should be created.
     */
    public File getPublicKeyFile() {
        if (model.isAdvanced()) {
            File f = new File(pubKey.getUIControl().getText());
            model.setPubKeyName(f.getName());
            model.setPubKeyPath(f.getParent());
        }
        return new File(model.getPubKeyPath(), model.getPubKeyName());
    }

    /**
     * Returns file to which private key should be created.
     * 
     * @return File to which private key should be created.
     */
    public File getPrivateKeyFile() {
        if (model.isAdvanced()) {
            File f = new File(privKey.getUIControl().getText());
            model.setPrivKeyName(f.getName());
            model.setPrivKeyPath(f.getParent());
        }
        return new File(model.getPrivKeyPath(), model.getPrivKeyName());
    }

    /**
     * Returns key length in bits.
     * 
     * @return Key length.
     */
    public int getKeyLength() {
        return model.getKeyLength();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/keys_generator_wizard/keys_generator_wizard.html");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isPageComplete() {
        for (IValidator validator : validators) {
            if (model != null && model.isAdvanced() && (validator == pathValidator || validator == nameValidator)) {
                continue;
            } else if (!validator.isValueOk()) {
                return false;
            }
        }
        return true;
    }

    /**
     * Sets the model.
     * 
     * @param pModel
     *            Model.
     */
    public void setModel(TVPKeygenPageModel pModel) {
        if (model != null) {
            model.deleteObserver(this);
        }
        model = pModel;
        model.addObserver(this);
        update(model, null);
    }

    /**
     * {@inheritDoc}
     */
    public void update(Observable o, Object arg) {
        if (o == model) {
            boolean adv = model.isAdvanced();
            pubKey.getUIControl().setEnabled(adv);
            pubKey.getTextFieldButton().setEnabled(adv);
            privKey.getUIControl().setEnabled(adv);
            privKey.getTextFieldButton().setEnabled(adv);

            path.getUIControl().setEnabled(!adv);
            path.getTextFieldButton().setEnabled(!adv);
            name.getUIControl().setEnabled(!adv);

            setText(privKey, new File(model.getPrivKeyPath(), model.getPrivKeyName()).getAbsolutePath());
            setText(pubKey, new File(model.getPubKeyPath(), model.getPubKeyName()).getAbsolutePath());

            if (!adv) {
                setText(path, model.getPubKeyPath());
                String tmpName = model.getPubKeyName();
                setText(name, tmpName.substring(0, tmpName.length() - PUB_POSTFIX.length()));
            }

            if (getWizard().getContainer().getCurrentPage() != null) {
                getWizard().getContainer().updateButtons();
            }
        }
    }

    /**
     * @param textUI
     * @param value
     */
    private void setText(UITextField textUI, String value) {
        if (!textUI.getUIControl().getText().equals(value)) {
            if (value != null) {
                textUI.getUIControl().setText(value);
            } else {
                textUI.getUIControl().setText("");
            }
        }
    }

    /**
     * 
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
     * Model class.
     */
    class TVPKeygenPageModel extends Observable {
        private boolean advanced = false;

        private String privKeyPath = "";

        private String privKeyName = KEY_PREFIX + PRIV_POSTFIX;

        private String pubKeyPath = privKeyPath;

        private String pubKeyName = KEY_PREFIX + PUB_POSTFIX;

        private int keyLength = 2048;

        /**
         * Returns true if advanced.
         * 
         * @return True if advanced.
         */
        public boolean isAdvanced() {
            return advanced;
        }

        /**
         * Sets whether advanced or not.
         * 
         * @param pAdvanced
         *            True if advanced.
         */
        public void setAdvanced(boolean pAdvanced) {
            if (advanced != pAdvanced) {
                advanced = pAdvanced;
                setChanged();
            }
        }

        /**
         * Returns private key path.
         * 
         * @return Private key path.
         */
        public String getPrivKeyPath() {
            return privKeyPath;
        }

        /**
         * Sets private key path.
         * 
         * @param pPrivKeyPath
         *            Private key path.
         */
        public void setPrivKeyPath(String pPrivKeyPath) {
            if (!privKeyPath.equals(pPrivKeyPath)) {
                privKeyPath = pPrivKeyPath;
                setChanged();
            }
        }

        /**
         * Returns private key name.
         * 
         * @return Private key name.
         */
        public String getPrivKeyName() {
            return privKeyName;
        }

        /**
         * Sets private key name.
         * 
         * @param pPrivKeyName
         *            Private key name.
         */
        public void setPrivKeyName(String pPrivKeyName) {
            if (!privKeyName.equals(pPrivKeyName)) {
                privKeyName = pPrivKeyName;
                setChanged();
            }
        }

        /**
         * Returns public key path.
         * 
         * @return Public key path.
         */
        public String getPubKeyPath() {
            return pubKeyPath;
        }

        /**
         * Sets public key path.
         * 
         * @param pPubKeyPath
         *            Public key path.
         */
        public void setPubKeyPath(String pPubKeyPath) {
            if (!pubKeyPath.equals(pPubKeyPath)) {
                pubKeyPath = pPubKeyPath;
                setChanged();
            }
        }

        /**
         * Returns public key name.
         * 
         * @return Public key name.
         */
        public String getPubKeyName() {
            return pubKeyName;
        }

        /**
         * Sets public key name.
         * 
         * @param pPubKeyName
         *            Public key name.
         */
        public void setPubKeyName(String pPubKeyName) {
            if (!pubKeyName.equals(pPubKeyName)) {
                pubKeyName = pPubKeyName;
                setChanged();
            }
        }

        /**
         * Sets key length in bits.
         * 
         * @param pKeyLength
         *            Key length.
         */
        public void setKeyLength(int pKeyLength) {
            keyLength = pKeyLength;
        }

        /**
         * Returns key length in bits.
         * 
         * @return Key length.
         */
        public int getKeyLength() {
            return keyLength;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public String toString() {
            return "advanced=" + advanced + "\nprivKeyName=" + privKeyName + ", privKeyPath=" + privKeyPath
                + "\npubKeyName=" + pubKeyName + ", pubKeyPath=" + pubKeyPath + ", keyLength=" + keyLength;
        }
    }

    /**
     * Only for GUI Dancer.
     */
    private void setData() {
        setGuiData(path.getUIControl(), "pathTF");
        setGuiData(path.getTextFieldButton(), "pathBT");
        setGuiData(name.getUIControl(), "nameTF");
        setGuiData(advanced, "advancedBT");
        setGuiData(privKey.getUIControl(), "privateKeyTF");
        setGuiData(privKey.getTextFieldButton(), "privateKeyBT");
        setGuiData(pubKey.getUIControl(), "pubKeyTF");
        setGuiData(pubKey.getTextFieldButton(), "pubKeyBT");
        setGuiData(keyLength, "keyLengthCMB");
    }

    private void setGuiData(Widget w, String data) {
        w.setData("GD_COMP_NAME", "TVPKeygenPage_" + data);
    }
}
