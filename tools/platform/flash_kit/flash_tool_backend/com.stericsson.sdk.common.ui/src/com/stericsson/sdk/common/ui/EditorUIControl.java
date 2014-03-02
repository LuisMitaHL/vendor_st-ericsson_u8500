/**
 * 
 */
package com.stericsson.sdk.common.ui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.forms.widgets.FormToolkit;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorXML.Section.Record;
import com.stericsson.sdk.common.ui.behaviors.PathTFBehavior;
import com.stericsson.sdk.common.ui.behaviors.SelectAllBehavior;
import com.stericsson.sdk.common.ui.controls.ITextFieldsFactory;
import com.stericsson.sdk.common.ui.controls.UIButton;
import com.stericsson.sdk.common.ui.controls.UIButtonFactory;
import com.stericsson.sdk.common.ui.controls.UIComboBox;
import com.stericsson.sdk.common.ui.controls.UIComboBoxFactory;
import com.stericsson.sdk.common.ui.controls.UIControl;
import com.stericsson.sdk.common.ui.controls.UITextField;
import com.stericsson.sdk.common.ui.controls.UITextFieldsFactory;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.PathTFValidator;
import com.stericsson.sdk.common.ui.validators.RegexTFValidator;

/**
 * A utilities class for UI Controls used in editors (e.g. profile editor)
 * 
 * @author xhelciz
 */
public class EditorUIControl extends Composite {

    /** UI control types */
    public enum CONTROL_TYPE {
        /** */
        TEXT_FIELD_NO_EDIT, TEXT_FIELD, COMBO, PATH, MULTIPLE_VALUES, CHECK_BUTTON
    };

    private IConfigurationRecord configRecord;

    private Composite composite;

    private FormToolkit toolkit;

    private List<UIControl> uiControls;

    private List<IValidator> validators;

    private List<IEditorUIChangeListener> changeListeners;

    private EditorXML.Section.Record structure;

    private int gdId;

    /**
     * Constructs Editor UI control.
     * 
     * @param pToolkit
     *            form toolkit or null if the composite is not placed in form
     * @param pParent
     *            parent for this control
     * @param pRecordStructure
     *            definition of attributes
     * @param pRecord
     *            configuration record
     */
    public EditorUIControl(FormToolkit pToolkit, Composite pParent, EditorXML.Section.Record pRecordStructure,
        IConfigurationRecord pRecord) {
        this(pToolkit, pParent, pRecordStructure, pRecord, 0);
    }

    /**
     * Constructs Editor UI control.
     * 
     * @param pToolkit
     *            form toolkit or null if the composite is not placed in form
     * @param pParent
     *            parent for this control
     * @param pRecordStructure
     *            definition of attributes
     * @param pRecord
     *            configuration record
     * @param pGdId
     *            id for guidancer names, to distinguish multiple identical widgets
     */
    public EditorUIControl(FormToolkit pToolkit, Composite pParent, EditorXML.Section.Record pRecordStructure,
        IConfigurationRecord pRecord, int pGdId) {
        super(pParent, SWT.NONE);
        this.setLayout(new FillLayout());

        toolkit = pToolkit;
        structure = pRecordStructure;
        configRecord = pRecord;
        gdId = pGdId;

        uiControls = new ArrayList<UIControl>();
        changeListeners = new ArrayList<IEditorUIChangeListener>();
        validators = new ArrayList<IValidator>();

        if (toolkit != null) {
            if (structure.getShowGroup()) {
                Group g = new Group(this, SWT.NONE);
                String name = configRecord.getValue("Name");
                if (name != null) {
                    g.setText(getHumanReadableString(name));
                }
                g.setLayout(new GridLayout(3 * structure.getColumns(), false));
                composite = g;
            } else {
                composite = toolkit.createComposite(this);
                composite.setLayout(new GridLayout(3 * structure.getColumns(), false));
            }
        } else {
            composite = new Composite(this, SWT.NONE);
            composite.setLayout(new GridLayout(3 * structure.getColumns(), false));
        }

        for (EditorXML.Section.Record.Attribute attr : structure.getAttributes()) {
            switch (attr.getVisual().getType()) {
                case TEXT_FIELD_NO_EDIT:
                    uiControls.add(createTextFieldControl(attr, false));
                    break;
                case TEXT_FIELD:
                    uiControls.add(createTextFieldControl(attr, true));
                    break;
                case COMBO:
                    uiControls.add(createComboControl(attr));
                    break;
                case PATH:
                    uiControls.add(createPathControl(attr));
                    break;
                case CHECK_BUTTON:
                    uiControls.add(createCheckButton(attr));
                    break;
                default:
                    Label l;
                    if (toolkit != null) {
                        l = toolkit.createLabel(composite, "Control hasn't been defined yet..", SWT.BORDER);
                    } else {
                        l = new Label(composite, SWT.BORDER);
                        l.setText("Control hasn't been defined yet.");
                    }
                    GridData gd = new GridData(SWT.FILL, SWT.CENTER, true, false);
                    gd.horizontalSpan = 3;
                    l.setLayoutData(gd);

                    break;
            }
        }
    }

    private UIButton createCheckButton(EditorXML.Section.Record.Attribute pAttribute) {

        final String attrLabel = pAttribute.getVisual().getLabel();
        final String attrName = pAttribute.getName();

        final String attrValue;
        if (configRecord != null) {
            attrValue = configRecord.getValue(pAttribute.getName());
        } else {
            attrValue = "";
        }

        UIButtonFactory factory = UIButtonFactory.getInstance(toolkit);
        final UIButton uiButton = factory.createButton(composite, SWT.CHECK, attrLabel, 1, false);
        uiButton.getUIControl().setData("GD_COMP_NAME",
            "EditorUIControl_" + structure.getName() + pAttribute.getName() + "Value" + gdId + "_Button");

        // converting Yes to True, No to False
        String trueText = "true";
        String falseText = "false";
        for (EditorXML.Section.Record.Attribute.Visual.Option option : pAttribute.getVisual().getOptions()) {
            if (option.getLabel().equals("true")) {
                trueText = option.getValue();
            }
            if (option.getLabel().equals("false")) {
                falseText = option.getValue();
            }
        }
        final HashMap<Boolean, String> logicToText = new HashMap<Boolean, String>();
        logicToText.put(Boolean.TRUE, trueText);
        logicToText.put(Boolean.FALSE, falseText);

        boolean selected = attrValue.equalsIgnoreCase(trueText);
        uiButton.getUIControl().setSelection(selected);
        // modify existing configuration record automatically, if record doesn't exist, do nothing
        // since the record will be created during save
        if (configRecord != null) {
            uiButton.getUIControl().addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent pE) {
                    configRecord.setValue(attrName, logicToText.get(uiButton.getUIControl().getSelection()));
                    notifyChangeListeners();
                }

                @Override
                public void widgetDefaultSelected(SelectionEvent pE) {
                    uiButton.getUIControl().setSelection(attrValue.equalsIgnoreCase(logicToText.get(Boolean.TRUE)));
                }
            });
        }
        return uiButton;
    }

    private UITextField createPathControl(EditorXML.Section.Record.Attribute pAttribute) {
        final String attrName = pAttribute.getName();
        final String attrLabel = pAttribute.getVisual().getLabel();

        final String[] filterExtensions = new String[pAttribute.getVisual().getOptions().size()];
        int i = 0;
        for (EditorXML.Section.Record.Attribute.Visual.Option option : pAttribute.getVisual().getOptions()) {
            filterExtensions[i] = option.getValue();
        }

        // convert record to relative path
        String path = "";
        String filterPath = "";
        final String attrValue;
        if (configRecord != null) {
            path = configRecord.getValue(attrName);
            if (path == null) {
                path = "";
            }
            String relativePath = PathTFBehavior.getRelativePath(path);
            if (path.indexOf(relativePath) != -1) {
                filterPath = path.substring(0, path.indexOf(relativePath));
            }
            configRecord.setValue(attrName, relativePath);
            attrValue = configRecord.getValue(attrName);
        } else {
            attrValue = "";
        }

        ITextFieldsFactory factory = UITextFieldsFactory.getInstance(toolkit);
        UITextField uiTextField =
            factory.createFilePathTextField(composite, SWT.BORDER, attrLabel + ":", attrValue, 0, filterPath,
                filterExtensions, true);

        final Text pathTF = uiTextField.getUIControl();
        pathTF.setData("GD_COMP_NAME", "EditorUIControl_" + structure.getName() + pAttribute.getVisual().getLabel()
            + "Value" + gdId + "_Text");
        pathTF.setEditable(false);

        uiTextField.getTextFieldButton().setData("GD_COMP_NAME",
            "EditorUIControl_" + structure.getName() + pAttribute.getVisual().getLabel() + "Browse" + gdId + "_Button");

        IValidator validator = new PathTFValidator(attrName, pathTF, false);
        new PathTFBehavior(pathTF).switchOn();
        validator.switchOn();
        validators.add(validator);
        (new SelectAllBehavior(pathTF)).switchOn();

        // modify existing configuration record automatically, if record doesn't exist, do nothing
        // since the record will be created during save
        if (configRecord != null) {
            pathTF.addModifyListener(new ModifyListener() {
                public void modifyText(ModifyEvent e) {
                    pathTF.setToolTipText(pathTF.getText());
                    configRecord.setValue(attrName, pathTF.getText());
                    notifyChangeListeners();
                }
            });
        }

        return uiTextField;
    }

    private UIComboBox createComboControl(EditorXML.Section.Record.Attribute pAttribute) {
        final String attrLabel = pAttribute.getVisual().getLabel();
        final String attrName = pAttribute.getName();
        String[] comboLabels = new String[pAttribute.getVisual().getOptions().size()];
        final String[] comboValues = new String[pAttribute.getVisual().getOptions().size()];
        int i = 0;
        int selected = 0;
        for (EditorXML.Section.Record.Attribute.Visual.Option option : pAttribute.getVisual().getOptions()) {
            comboLabels[i] = option.getLabel();
            comboValues[i] = option.getValue();
            if (configRecord != null && option.getValue().equalsIgnoreCase(configRecord.getValue(attrName))) {
                selected = i;
            }
            i++;
        }

        UIComboBoxFactory factory = UIComboBoxFactory.getInstance(toolkit);
        final UIComboBox comboBox =
            factory.createComboBox(composite, SWT.READ_ONLY, attrLabel, comboLabels, 1, selected);
        comboBox.getUIControl().setData("GD_COMP_NAME",
            "EditorUIControl_" + structure.getName() + pAttribute.getVisual().getLabel() + "Value" + gdId + "_Combo");

        // modify existing configuration record automatically, if record doesn't exist, do nothing
        // since the record will be created during save
        if (configRecord != null) {
            comboBox.getUIControl().addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent pE) {
                    configRecord.setValue(attrName, comboValues[comboBox.getUIControl().getSelectionIndex()]);
                    notifyChangeListeners();
                }
            });
        }

        return comboBox;
    }

    private UITextField createTextFieldControl(EditorXML.Section.Record.Attribute pAttribute, boolean pEditable) {
        final String attrName = pAttribute.getName();
        final String attrLabel = pAttribute.getVisual().getLabel();
        String regex = (String) pAttribute.getVisual().getRegexpValidator();
        String value = "";
        if (configRecord != null) {
            value = configRecord.getValue(attrName);
        }

        ITextFieldsFactory factory = UITextFieldsFactory.getInstance(toolkit);
        UITextField uiTextField = factory.createTextField(composite, SWT.NONE, attrLabel + ":", value, 1);
        final Text tf = uiTextField.getUIControl();
        tf.setData("GD_COMP_NAME", "EditorUIControl_" + structure.getName() + attrLabel + "Value" + gdId + "_Text");
        tf.setEditable(pEditable);

        if (pEditable) {
            // modify existing configuration record automatically, if record doesn't exist, do
            // nothing since the record will be created during save
            if (configRecord != null) {
                tf.addModifyListener(new ModifyListener() {
                    public void modifyText(ModifyEvent pE) {
                        if (tf.getText().length() > tf.getTextLimit()) {
                            tf.setToolTipText(tf.getText());
                        } else {
                            tf.setToolTipText(null);
                        }
                        configRecord.setValue(attrName, tf.getText());
                        notifyChangeListeners();
                    }
                });
            }
            IValidator validator = new RegexTFValidator(attrName, tf, regex);
            validator.switchOn();
            validators.add(validator);
            (new SelectAllBehavior(tf)).switchOn();
        }
        return uiTextField;
    }

    /**
     * Runs validation by calling all attached validators.
     * 
     * @return true if validation is successful
     */
    public boolean isValidationOk() {
        for (IValidator validator : validators) {
            validator.checkCurrentValue();
            if (!validator.isValueOk()) {
                return false;
            }
        }
        return true;
    }

    /**
     * Returns list of validators used for this UI component.
     * 
     * @return list of validators
     */
    public List<IValidator> getValidators() {
        return validators;
    }

    /**
     * {@inheritDoc}
     */
    public void dispose() {
        validators = null;
        changeListeners = null;
        composite.dispose();
        super.dispose();
    }

    /**
     * {@inheritDoc}
     */
    public Composite getComposite() {
        return composite;
    }

    /**
     * Gains list of UIControls created within.
     * 
     * @return list of UIControls
     */
    public List<UIControl> getUIControls() {
        return uiControls;
    }

    /**
     * Returns record read from configuration file.
     * 
     * @return configuration record
     */
    public IConfigurationRecord getRecord() {
        return configRecord;
    }

    /**
     * Add change listener into changeListeners list.
     * 
     * @param pListener
     *            change listener to be added
     * @return true if successful
     */
    public boolean addChangeListener(IEditorUIChangeListener pListener) {
        return changeListeners.add(pListener);
    }

    /**
     * Remove change listener from changeListeners list.
     * 
     * @param pListener
     *            change listener to be removed
     * @return true if successful
     */
    public boolean removeChangeListener(IEditorUIChangeListener pListener) {
        return changeListeners.remove(pListener);
    }

    private void notifyChangeListeners() {
        for (IEditorUIChangeListener listener : changeListeners) {
            listener.uiControlChanged(this);
        }
    }

    private static String getHumanReadableString(String pStr) {
        return pStr.replaceAll("_", " ");
    }

    /**
     * Get XML structure of this UI Control.
     * 
     * @return XML structure of this UI Control
     */
    public Record getControlStructure() {
        return structure;
    }
}
