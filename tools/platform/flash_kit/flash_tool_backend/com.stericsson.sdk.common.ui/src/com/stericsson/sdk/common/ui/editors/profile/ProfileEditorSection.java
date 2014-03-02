package com.stericsson.sdk.common.ui.editors.profile;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorUIControl;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.editors.mesp.MespConfigTreeItem;
import com.stericsson.sdk.common.ui.validators.IValidator;
import com.stericsson.sdk.common.ui.validators.SectionValidator;

/**
 * Sections of Profile Editor page.
 * 
 * @author xadazim
 * @author xhelciz
 */
public class ProfileEditorSection {
    private static final String RECORD_TYPE_SECURITY_PROPERTY = "SecurityProperty";

    private static final String RECORD_TYPE_PROPERTY = "Property";

    private Section section;

    private FormToolkit toolkit;

    private ScrolledForm form;

    private Composite sectionComposite;

    private List<Composite> uiControls;

    ProfileEditorPage editorPage;

    private EditorXML.Section structure;

    SectionValidator sectionValidator;

    private ArrayList<IConfigurationRecord> propertiesDefined = new ArrayList<IConfigurationRecord>();

    private ArrayList<IConfigurationRecord> securityPropertiesDefined = new ArrayList<IConfigurationRecord>();

    /**
     * @param pToolkit
     *            form toolkit
     * @param pForm
     *            scrolled form
     * @param pStructure
     *            TODO
     * @param parent
     *            parent editor page
     */
    public ProfileEditorSection(FormToolkit pToolkit, ScrolledForm pForm, ProfileEditorPage parent,
        EditorXML.Section pStructure) {
        toolkit = pToolkit;
        form = pForm;
        editorPage = parent;
        structure = pStructure;
        uiControls = new ArrayList<Composite>();

        section = ProfileEditorSectionUtils.createSection(form, toolkit, structure);

        sectionValidator = new SectionValidator(section, securityPropertiesDefined);
        sectionValidator.addValidatorListener(editorPage);

        ProfileEditorSectionUtils.createToolbar(this, section, structure);
    }

    /**
     * called when refresh of a section is needed
     * 
     * @param configRecords
     *            configuration Records
     * @return true if refresh was succesful, false otherwise
     */
    public boolean refreshSection(List<MespConfigTreeItem> configRecords) {
        cleanSection();

        if (fillSection(configRecords)) {
            form.layout(true, true);
            return true;
        }
        return false;
    }

    /**
     * called when refresh of a section is needed
     * 
     * @return true if refresh was successful, false otherwise
     */
    public boolean refreshSection() {
        return refreshSection(this.editorPage.getConfigFile().getConfigRecords());
    }

    private boolean fillSection(List<MespConfigTreeItem> configRecords) {

        sectionComposite = toolkit.createComposite(section);

        GridLayout layout = new GridLayout(structure.getColumns(), false);
        // ((GridLayout)layout).marginHeight = 0;
        sectionComposite.setLayout(layout);

        section.setClient(sectionComposite);
        HashMap<String, Integer> gdIds = new HashMap<String, Integer>();

        propertiesDefined.clear();
        securityPropertiesDefined.clear();

        for (MespConfigTreeItem item : configRecords) {
            EditorXML.Section.Record recordStructure = structure.findMatchingRecord(item.getRecord().toString());
            if (recordStructure != null) {
                Integer gdId = gdIds.get(recordStructure.getMatch());
                if (gdId == null) {
                    gdId = 1;
                } else {
                    gdId = gdId + 1;
                }
                gdIds.put(recordStructure.getMatch(), gdId);
                IConfigurationRecord configRecord = item.getRecord();

                // keep list of certain type of records as it is needed for consistency check.
                if (recordStructure.getName().equals(RECORD_TYPE_PROPERTY)) {
                    propertiesDefined.add(configRecord);
                }
                if (recordStructure.getName().equals(RECORD_TYPE_SECURITY_PROPERTY)) {
                    securityPropertiesDefined.add(configRecord);
                }

                Composite controlComposite =
                    ProfileEditorSectionUtils.creteControlComposite(sectionComposite, recordStructure.getName(), gdId);

                final EditorUIControl control =
                    new EditorUIControl(toolkit, controlComposite, recordStructure, configRecord, gdId);

                if (structure.isExtendable()
                    && recordStructure.getMinCount().intValue() != recordStructure.getMaxCount().intValue()) {
                    final Button remove =
                        ProfileEditorSectionUtils.createRemoveButton(controlComposite, recordStructure.getName(), gdId);
                    remove.addSelectionListener(new SelectionAdapter() {
                        @Override
                        public void widgetSelected(SelectionEvent pE) {
                            openRemoveEntryDialog(control);
                        }
                    });
                }
                uiControls.add(controlComposite);
            }
        }

        sectionComposite.layout();
        checkValidators();
        section.layout();
        return true;
    }

    private void checkValidators() {
        sectionValidator.checkCurrentValue();

        for (Composite control : uiControls) {
            if (control != null) {
                for (Control child : control.getChildren()) {
                    if (child instanceof EditorUIControl) {
                        for (IValidator validator : ((EditorUIControl) child).getValidators()) {
                            validator.addValidatorListener(editorPage);
                            validator.checkCurrentValue();
                        }
                        ((EditorUIControl) child).addChangeListener(editorPage);
                    }
                }
            }
        }
    }

    private void cleanSection() {
        for (Composite control : uiControls) {
            if (control != null) {
                control.dispose();
            }
        }
        if ((sectionComposite != null) && (!sectionComposite.isDisposed())) {
            sectionComposite.dispose();
        }
        uiControls.clear();
    }

    private void openRemoveEntryDialog(EditorUIControl control) {

        if (ProfileEditorSectionUtils.openRemoveEntryDialog(control.getShell())) {

            IConfigurationRecord rec = control.getRecord();
            for (MespConfigTreeItem item : editorPage.getConfigFile().getConfigRecords()) {
                if (item.getRecord().equals(rec)) {
                    editorPage.getConfigFile().removeRecord(item);
                    break;
                }
            }

            uiControls.remove(control.getParent());
            control.getParent().dispose();

            refreshSection(editorPage.getConfigFile().getConfigRecords());
            editorPage.setDirty(true);
        }

    }

    /**
     * Checks whether that kind of property already defined. Used by property dialog
     * 
     * @param recordStructure
     *            Kind of property record
     * @return True if this kind of property defined.
     */
    public boolean checkIfPropertyAdded(EditorXML.Section.Record recordStructure) {
        for (IConfigurationRecord record : propertiesDefined) {
            if (recordStructure.equals(structure.findMatchingRecord(record.toString()))) {
                return true;
            }
        }
        return false;
    }
}
