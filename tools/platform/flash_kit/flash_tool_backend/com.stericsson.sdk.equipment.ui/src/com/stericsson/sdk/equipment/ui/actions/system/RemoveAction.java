package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.ui.BackendViewerAdapter;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * 
 * @author xtomlju
 * 
 */
public class RemoveAction extends BackendAction {

    /**
     * @param viewer
     *            TBD
     */
    public RemoveAction(IBackendViewer viewer) {
        super(viewer);
        setText("Remove");
        setToolTipText("Remove selected equipment from list");
    }

    /**
     * 
     */
    @Override
    public void run() {
        BackendViewerAdapter.getInstance().removeEquipment(viewer.getSelectedEquipment());
    }

}
