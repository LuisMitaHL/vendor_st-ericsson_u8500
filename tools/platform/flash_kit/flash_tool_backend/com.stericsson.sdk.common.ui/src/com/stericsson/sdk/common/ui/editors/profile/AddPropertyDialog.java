/**
 * 
 */
package com.stericsson.sdk.common.ui.editors.profile;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.EditorXML.Section;
import com.stericsson.sdk.common.ui.EditorXML.Section.Record.Attribute;
import com.stericsson.sdk.common.ui.EditorXML.Section.Record.Attribute.Visual.Option;
import com.stericsson.sdk.common.ui.controls.UIControl;

/**
 * This class provides dialog box for creating new properties.
 * 
 * @author xhelciz
 * 
 */
public class AddPropertyDialog extends Dialog {

    private String title;

    Section structure;

    private List<EditorUIControl> uiControls = new ArrayList<EditorUIControl>();

    private List<IConfigurationRecord> uiControlRecords = new ArrayList<IConfigurationRecord>();

    StackLayout stackLayout;

    Combo combo;

    EditorUIControl[] selectedContents;

    Composite[] contents;

    private ProfileEditorSection section;

    /**
     * @param pParentShell
     *            parent for this dialog
     * @param pTitle
     *            title for this dialog
     * @param pStructure
     *            XML structure of UI controls to be displayed
     * @param pSection
     *            Property Section
     */
    protected AddPropertyDialog(Shell pParentShell, String pTitle, Section pStructure, ProfileEditorSection pSection) {
        super(pParentShell);
        title = pTitle;
        structure = pStructure;

        section = pSection;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected Control createDialogArea(Composite pParent) {
        Composite dialogContent = (Composite) super.createDialogArea(pParent);

        dialogContent.setLayout(new GridLayout());

        List<EditorXML.Section.Record> records = structure.getRecords();

        if (structure.getRecords().size() > 1) {

            selectedContents = new EditorUIControl[records.size()];
            createCombobox(dialogContent);

            Group valueGroup = new Group(dialogContent, SWT.NONE);
            valueGroup.setText("Value:");

            stackLayout = new StackLayout();
            valueGroup.setLayout(stackLayout);
            valueGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));

            contents = new Composite[records.size()];

            int i = 0;
            for (EditorXML.Section.Record recordStructure : records) {

                // skip if already defined
                if (section.checkIfPropertyAdded(recordStructure)) {
                    continue;
                }
                Composite composite = new Composite(valueGroup, SWT.NONE);
                composite.setLayout(new GridLayout());
                contents[i] = composite;

                if (recordStructure != null) {

                    selectedContents[i] = new EditorUIControl(null, composite, recordStructure, null);
                    i++;
                }
            }

            stackLayout.topControl = contents[combo.getSelectionIndex()];
            valueGroup.layout();
        } else {
            for (EditorXML.Section.Record recordStructure : records) {
                if (recordStructure != null) {
                    uiControls.add(new EditorUIControl(null, dialogContent, recordStructure, null));
                }
            }
        }

        return dialogContent;
    }

    private Combo createCombobox(final Composite parent) {

        List<EditorXML.Section.Record> records = structure.getRecords();
        ArrayList<String> al = new ArrayList<String>();
        // String[] comboLabels = new String[records.size()];
        int i = 0;
        for (EditorXML.Section.Record recordStructure : records) {
            // skip if already defined
            if (section.checkIfPropertyAdded(recordStructure)) {
                continue;
            }
            // comboLabels[i] = (recordStructure.getLabel());
            al.add(recordStructure.getLabel());
            i++;
        }

        String[] comboLabels = new String[al.size()];
        comboLabels = al.toArray(comboLabels);

        Label label = new Label(parent, SWT.NONE);
        label.setData("GD_COMP_NAME", "UIControl" + "_Property_Name" + "_Label");
        label.setText("Name:");
        combo = new Combo(parent, SWT.READ_ONLY);
        combo.setData("GD_COMP_NAME", "UIControl" + "_Property_Name" + "_Combo");
        combo.setItems(comboLabels);
        combo.select(0);
        GridData gd = new GridData(GridData.FILL, GridData.CENTER, false, false);
        gd.horizontalIndent = 10;
        combo.setLayoutData(gd);
        combo.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                stackLayout.topControl = contents[combo.getSelectionIndex()];
                parent.layout(true, true);
            }
        });
        return combo;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void buttonPressed(int buttonId) {
        if (buttonId == IDialogConstants.OK_ID) {
            if (structure.getRecords().size() > 1) {
                uiControls.add(selectedContents[combo.getSelectionIndex()]);
            }
            for (EditorUIControl control : uiControls) {
                fillWithControlRecords(control);
            }
        }
        super.buttonPressed(buttonId);
    }

    private void fillWithControlRecords(EditorUIControl uiControl) {
        IConfigurationRecord record = new MESPConfigurationRecord();

        List<Attribute> attributes = uiControl.getControlStructure().getAttributes();
        List<UIControl> children = uiControl.getUIControls();

        for (UIControl child : children) {
            Control control = child.getUIControl();
            String valueName = "";
            String value = "";
            if (control instanceof Button) {
                valueName = gainName(attributes, ((Button) control));
                value = gainValue(attributes, ((Button) control));
            } else {
                valueName = gainName(attributes, child.getControlLabel());
                value = gainValue(attributes, control);
            }
            record.setValue(valueName, value);
            /*
             * if (!(child instanceof Label)) { String valueName = ""; String value = ""; Composite
             * group = (Composite) child; for (Control component : group.getChildren()) { if
             * (component instanceof Label) { if (valueName.length() == 0) { valueName =
             * gainName(attributes, component); } } else if (component instanceof Button) {
             * valueName = gainName(attributes, component); value = gainValue(attributes,
             * component); } else { value = gainValue(attributes, component); } }
             * record.setValue(valueName, value);
             * 
             * }
             */
        }
        record.setName(uiControl.getControlStructure().getName());
        uiControlRecords.add(record);

    }

    private String gainName(List<Attribute> attributes, Control component) {
        String label;
        if (component instanceof Button) {
            label = ((Button) component).getText();
        } else if (component instanceof Combo) {
            label = ((Combo) component).getText();
        } else {
            label = ((Label) component).getText();
        }

        for (Attribute attr : attributes) {
            if (label.trim().toLowerCase(Locale.getDefault()).startsWith(
                attr.getName().toLowerCase(Locale.getDefault()))) {
                return attr.getName();
            } else if (label.trim().toLowerCase(Locale.getDefault()).startsWith(
                attr.getVisual().getLabel().toLowerCase(Locale.getDefault()))) {
                return attr.getName();
            }
        }
        return null;
    }

    private String gainValue(List<Attribute> attributes, Control component) {
        String value = "";

        if (component instanceof Text) {
            value = ((Text) component).getText();
        } else if (component instanceof Combo) {
            value = ((Combo) component).getItem(((Combo) component).getSelectionIndex());
        } else if (component instanceof Text) {
            value = ((Text) component).getText();
        } else if (component instanceof Button) {
            for (Attribute attr : attributes) {
                if (((Button) component).getText().toLowerCase(Locale.getDefault()).startsWith(
                    attr.getName().toLowerCase(Locale.getDefault()))
                    || ((Button) component).getText().toLowerCase(Locale.getDefault()).startsWith(
                        attr.getVisual().getLabel().toLowerCase(Locale.getDefault()))) {
                    value = getLogicOption(attr, (Button) component);
                }
            }
        }
        return value;
    }

    private String getLogicOption(Attribute attr, Button component) {
        String value = "";
        String trueText = "true";
        String falseText = "false";
        List<Option> opt = attr.getVisual().getOptions();
        for (EditorXML.Section.Record.Attribute.Visual.Option option : opt) {
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
        value = logicToText.get(component.getSelection());
        return value;

    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null) {
            shell.setText(title);
        }
    }

    /**
     * @return list of UI control's records
     */
    public List<IConfigurationRecord> getUIControlsRecords() {
        return uiControlRecords;
    }

}
