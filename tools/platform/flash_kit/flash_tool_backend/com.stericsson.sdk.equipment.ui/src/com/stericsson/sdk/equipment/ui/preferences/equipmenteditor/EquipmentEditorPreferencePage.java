package com.stericsson.sdk.equipment.ui.preferences.equipmenteditor;

import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.jface.preference.RadioGroupFieldEditor;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Equipment Editor Preference Page
 * 
 * @author xolabju
 * 
 */
public class EquipmentEditorPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    /**
     * Constructor
     */
    public EquipmentEditorPreferencePage() {
        super(GRID);
        setPreferenceStore(Activator.getDefault().getPreferenceStore());
        setDescription("Equipment Editor preferences");
    }

    /**
     * Creates the field editors. Field editors are abstractions of the common GUI blocks needed to
     * manipulate various types of preferences. Each field editor knows how to save and restore
     * itself.
     */
    public void createFieldEditors() {
        String[][] closeOnDisconnectValues = new String[][] {
            {
                "Yes", FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_CLOSE}, {
                "No", FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_NO_CLOSE}, {
                "Always Ask", FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT_ASK}};
        addField(new RadioGroupFieldEditor(FlashKitPreferenceConstants.EQUIPMENT_EDITOR_DISCONNECT,
            "&Close editor when equipment is disconnected", 3, closeOnDisconnectValues, getFieldEditorParent(), true));
    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench) {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void performHelp() {
        PlatformUI.getWorkbench().getHelpSystem().displayHelpResource(
            "/com.stericsson.sdk.equipment.ui.help/html/gettingstarted/overview_preferences.html");
    }
}
