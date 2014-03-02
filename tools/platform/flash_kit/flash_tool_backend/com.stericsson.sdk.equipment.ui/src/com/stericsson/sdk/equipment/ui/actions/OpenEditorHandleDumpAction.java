package com.stericsson.sdk.equipment.ui.actions;

import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.stericsson.sdk.equipment.ui.editors.EquipmentEditorInput;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 *
 * Editor for the handle dump
 *
 * @author esrimpa
 *
 */
public class OpenEditorHandleDumpAction extends BackendAction {

    /**
     * @param pViewer
     *            the associated equipment viewer
     */
    public OpenEditorHandleDumpAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Handle Dump");
        setToolTipText("Open core dump equipment in editor");
    }

    /**
     *
     */
    public void run() {
        try {
            PlatformUI
                .getWorkbench()
                .getActiveWorkbenchWindow()
                .getActivePage()
                .openEditor(new EquipmentEditorInput(getViewer().getSelectedEquipment()),
                    "com.stericsson.sdk.equipment.ui.editors.EquipmentDumpEditor");
        } catch (PartInitException e) {
            e.printStackTrace();
        }
    }

}
