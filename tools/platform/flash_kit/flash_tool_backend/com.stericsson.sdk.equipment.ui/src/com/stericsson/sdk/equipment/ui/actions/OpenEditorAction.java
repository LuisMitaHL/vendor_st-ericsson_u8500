package com.stericsson.sdk.equipment.ui.actions;

import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.equipment.ui.editors.EquipmentEditorInput;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xtomlju
 */
public class OpenEditorAction extends BackendAction {

    /**
     * @param pViewer TBD
     */
    public OpenEditorAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Open in Editor");
        setToolTipText("Open selected equipment in editor");
    }

    /**
     * 
     */
    public void run() {
        try {
            PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().openEditor(
                new EquipmentEditorInput(getViewer().getSelectedEquipment()),
                "com.stericsson.sdk.equipment.ui.editors.EquipmentEditor");
        } catch (PartInitException e) {
            e.printStackTrace();
        }
    }
}
