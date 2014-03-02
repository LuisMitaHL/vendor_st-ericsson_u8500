/**
 * 
 */
package com.stericsson.sdk.common.ui.editors.mesp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.validators.IValidator;

/**
 * @author xhelciz
 * 
 */
public class MespSettingsSection {
    private static final String SECTION_CONFIGURATION = "Configuration Settings";

    private MespEditorPage parent;

    private FormToolkit toolkit;

    private ScrolledForm form;

    private Section settingsSection;

    private Composite configSettings;

    private ToolBar settingsToolbar;

    private StackLayout stackLayout;

    private List<EditorUIControl> uiControls;

    private EditorXML.Section structure;

    /**
     * @param pToolkit
     *            form toolkit
     * @param pForm
     *            scrolled form
     * @param pParent
     *            parent editor page
     * @param pStructure
     *            section structure (widgets description read from xml configuration file)
     */
    public MespSettingsSection(FormToolkit pToolkit, ScrolledForm pForm, MespEditorPage pParent,
        EditorXML.Section pStructure) {
        toolkit = pToolkit;
        form = pForm;
        parent = pParent;
        structure = pStructure;
        uiControls = new ArrayList<EditorUIControl>();

        settingsSection = toolkit.createSection(form.getBody(), Section.TITLE_BAR | Section.TWISTIE | Section.EXPANDED);
        settingsSection.setText(SECTION_CONFIGURATION);
    }

    /**
     * sets layout data for this section
     * 
     * @param data
     *            layout data to be set
     */
    public void setLayoutData(Object data) {
        settingsSection.setLayoutData(data);
    }

    /**
     * called when refresh of overview section is needed
     * 
     * @param configRecords
     *            configuration Records
     * @return true if refresh was succesfull, false otherwise
     */
    public boolean refreshSection(List<MespConfigTreeItem> configRecords) {
        cleanSection();

        if (fillSection(configRecords)) {
            return true;
        }
        return false;
    }

    /**
     * notifies the settings section that source of its data has been changed
     * 
     * @param record
     *            new record to be displayed
     */
    public void selectionChanged(IConfigurationRecord record) {
        stackLayout.topControl = findUIControl(record);
        settingsToolbar.setVisible(true);
        configSettings.layout();
    }

    private void createToolbar() {
        settingsToolbar = new ToolBar(settingsSection, SWT.FLAT | SWT.HORIZONTAL);
        settingsToolbar.setLayoutData(new GridData(SWT.RIGHT, SWT.NONE, true, false));

        ToolItem remove = new ToolItem(settingsToolbar, SWT.PUSH);
        remove.setData("GD_COMP_NAME", "MespSettingsSection_Remove_btn");
        remove.setText("Remove");
        settingsToolbar.setVisible(false);

        remove.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                openRemoveEntryDialog(settingsToolbar.getShell());
            }
        });

        settingsSection.setTextClient(settingsToolbar);

    }

    private boolean fillSection(List<MespConfigTreeItem> configRecords) {

        configSettings = toolkit.createComposite(settingsSection);
        stackLayout = new StackLayout();
        configSettings.setLayout(stackLayout);
        settingsSection.setClient(configSettings);

        createToolbar();
        HashMap<String, Integer> gdIds = new HashMap<String, Integer>();

        for (MespConfigTreeItem item : configRecords) {
            EditorXML.Section.Record recordStructure = structure.findMatchingRecord(item.getRecord().toString());
            if (recordStructure != null) {
                Integer gdId=gdIds.get(recordStructure.getMatch());
                if(gdId==null) {
                    gdId=1;
                } else {
                    gdId=gdId+1;
                }
                gdIds.put(recordStructure.getMatch(), gdId);
                IConfigurationRecord configRecord = item.getRecord();
                EditorUIControl control = new EditorUIControl(toolkit, configSettings, recordStructure, configRecord, gdId);
                control.addChangeListener(parent);
                uiControls.add(control);
            }
        }

        /*
         * for (MespConfigTreeItem item : configRecords) { uiControls
         * .add(MespEditorUIControlFactory.getMespEditorUIControl(toolkit, configSettings,
         * item.getRecord())); }
         */

        stackLayout.topControl = null;
        settingsToolbar.setVisible(false);
        configSettings.layout();

        for (EditorUIControl control : uiControls) {
            if (control != null) {
                for (IValidator validator : control.getValidators()) {
                    validator.addValidatorListener(parent);
                    validator.checkCurrentValue();
                }
                control.addChangeListener(parent);
            }
        }

        settingsSection.layout();
        return false;
    }

    private void cleanSection() {
        for (EditorUIControl control : uiControls) {
            if (control != null) {
                control.dispose();
            }
        }
        if ((configSettings != null) && (!configSettings.isDisposed())) {
            configSettings.dispose();
        }
        uiControls.clear();

    }

    private Control findUIControl(IConfigurationRecord record) {
        EditorUIControl retValue = null;
        for (EditorUIControl uiControl : uiControls) {
            if (uiControl != null && uiControl.getRecord() == record) {
                retValue = uiControl;
                break;
            }
        }
        return retValue;
    }

    private void openRemoveEntryDialog(Shell shell) {

        MessageDialog removeMessage =
            new MessageDialog(shell, "Remove entry", null, "Remove entry definition?", MessageDialog.QUESTION,
                new String[] {
                    "Yes", "No"}, 0);

        if (removeMessage.open() == Dialog.OK) {

            Control control = stackLayout.topControl;
            if (control instanceof EditorUIControl) {
                IConfigurationRecord rec = ((EditorUIControl) control).getRecord();
                for (MespConfigTreeItem item : parent.getConfigFile().getConfigRecords()) {
                    if (item.getRecord().equals(rec)) {
                        parent.getConfigFile().removeRecord(item);
                        break;
                    }
                }

                uiControls.remove(control);
                control.dispose();

                refreshSection(parent.getConfigFile().getConfigRecords());
                parent.setDirty(true);
            }
        }
    }
}
