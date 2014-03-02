package com.stericsson.sdk.equipment.ui.preferences;

import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

import com.stericsson.sdk.equipment.ui.Activator;

/**
 * Flash Kit Preference Page
 * 
 * @author xolabju
 * 
 */
public class FlashKitPreferencePage extends FieldEditorPreferencePage implements IWorkbenchPreferencePage {

    /**
     * Constructor
     */
    public FlashKitPreferencePage() {
        super(GRID);
        setPreferenceStore(Activator.getDefault().getPreferenceStore());
        setDescription("Expand the tree to edit preferences for a specific feature");
    }

    /**
     * Creates the field editors. Field editors are abstractions of the common GUI blocks needed to
     * manipulate various types of preferences. Each field editor knows how to save and restore
     * itself.
     */
    public void createFieldEditors() {

    }

    /**
     * {@inheritDoc}
     */
    public void init(IWorkbench workbench) {
    }

}
