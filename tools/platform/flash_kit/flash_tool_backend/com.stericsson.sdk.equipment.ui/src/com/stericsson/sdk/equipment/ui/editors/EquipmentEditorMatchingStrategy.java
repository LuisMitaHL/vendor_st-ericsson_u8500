package com.stericsson.sdk.equipment.ui.editors;

import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorMatchingStrategy;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.PartInitException;

/**
 * 
 * @author xolabju
 * 
 */
public class EquipmentEditorMatchingStrategy implements IEditorMatchingStrategy {

    /**
     * {@inheritDoc}
     */
    public boolean matches(IEditorReference editorRef, IEditorInput input) {
        if (input != null && input instanceof EquipmentEditorInput) {
            try {
                IEditorInput refInput = editorRef.getEditorInput();
                if (refInput != null && refInput.getName().equals(input.getName())) {
                    return true;
                }
            } catch (PartInitException e) {
                e.printStackTrace();
            }
        }
        return false;
    }

}
