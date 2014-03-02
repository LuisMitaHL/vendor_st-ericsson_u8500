/**
 * 
 */
package com.stericsson.sdk.common.ui.editors.profile;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.ToolBar;

import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.ui.EditorXML;
import com.stericsson.sdk.common.ui.EditorXML.Section;
import com.stericsson.sdk.common.ui.editors.mesp.MespConfigTreeItem;

/**
 * @author xhelciz
 * 
 */
public class AddRecordSelectionAdapter extends SelectionAdapter {

    private ToolBar toolbar;

    private Section structure;

    private ProfileEditorSection section;

    /**
     * Add record
     * 
     * @param sectionStructure
     *            structure of controls to be filled into poped dialog
     * @param sectionToolbar
     *            toolbar
     * @param editorSection
     *            editor section
     */
    public AddRecordSelectionAdapter(Section sectionStructure, ToolBar sectionToolbar,
        ProfileEditorSection editorSection) {
        structure = sectionStructure;
        toolbar = sectionToolbar;
        section = editorSection;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void widgetSelected(SelectionEvent event) {
        openAddPropertyDialog();
    }

    private void openAddPropertyDialog() {
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
        if (al.size() == 0) {
            MessageDialog nothingToAdd =
                new MessageDialog(toolbar.getShell(), "Nothing to add", null,
                    "All available commmon properties are already defined", MessageDialog.INFORMATION, new String[] {
                        "OK"}, 0);
            nothingToAdd.setBlockOnOpen(true);
            nothingToAdd.open();

        } else {

            AddPropertyDialog dialog =
                new AddPropertyDialog(toolbar.getShell(), "Add new property", structure, section);
            dialog.setBlockOnOpen(true);
            if (dialog.open() == AddPropertyDialog.OK) {
                // gets here after the dialog is closed
                createNewItems(dialog.getUIControlsRecords());
            }
        }
    }

    private void createNewItems(List<IConfigurationRecord> records) {
        for (IConfigurationRecord record : records) {
            section.editorPage.getConfigFile().addRecord(
                new MespConfigTreeItem(section.editorPage.getConfigFile(), record));
            section.editorPage.setDirty(true);
        }
        // section.editorPage.refreshUIControls();
        section.refreshSection();
    }
}
